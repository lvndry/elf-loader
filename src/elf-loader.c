#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "elf-loader.h"
#include "execute.h"
#include "stack.h"
#include "utils.h"

void load_segments(int elf, Elf64_Ehdr header, int *stack_flags)
{
    // Seeking program headers
    off_t offset = lseek(elf, header.e_phoff, SEEK_SET);
    if (offset == -1)
    {
        errx(FILE_ERROR, "Could not read ELF file");
    }

    Elf64_Phdr ph;

    for (int i = 0; i < header.e_phnum; ++i)
    {
        ssize_t err = read(elf, &ph, sizeof(Elf64_Phdr));
        if (err == -1)
        {
            errx(FILE_ERROR, "Could not read program headers");
        }

        if (ph.p_type != PT_LOAD && ph.p_type != PT_GNU_STACK)
        {
            continue;
        }

        if (!ph.p_memsz)
        {
            continue;
        }

        void *seg_space = mmap(
            (void *)ph.p_vaddr,
            roundUp(ph.p_memsz, PAGE_SIZE),
            get_perms(ph.p_flags),
            MAP_PRIVATE,
            elf,
            (off_t)align(ph.p_offset)
        );

        if (seg_space == MAP_FAILED)
        {
            errx(MEM_ERROR, "Could not map segment in memory");
        }

        if (ph.p_type == PT_GNU_STACK)
        {
            *stack_flags = ph.p_flags;
        }

        if (ph.p_memsz > ph.p_filesz)
        {
            memset(
                (void *)(ph.p_vaddr + ph.p_filesz),
                0,
                ph.p_memsz - ph.p_filesz
            );
        }
    }
}

void init_auxv(Elf64_auxv_t *stack, Elf64_auxv_t auxv[], int *curs)
{
    int j = 0;
    for (size_t i = 0; auxv[i].a_type != AT_NULL; i++)
    {
        if (is_valid_auxv(auxv[i].a_type))
        {
            stack[j] = auxv[i];
            j++;
            *curs += 2;
        }
    }
    stack[j] = (Elf64_auxv_t){AT_NULL};
    *curs += 2;
}

void *create_stack(int argc, char *argv[], char *envp[], int stack_flags)
{
    size_t length = PAGE_SIZE * STACK_PAGES;
    int prot = stack_flags == PROT_NONE ? PROT_READ | PROT_WRITE : stack_flags ;

    void **stack = mmap(
        NULL,
        align(length),
        prot,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (stack == MAP_FAILED)
    {
        errx(MEM_ERROR, "Could not allocate stack");
    }

    int curs = 0;
    uint64_t cargc = argc;
    stack[curs++] = (void *)cargc;

    for (int i = 0; i < argc; ++i)
    {
        stack[curs++] = argv[i];
    }
    stack[curs++] = NULL;

    for (int i = 0; envp[i] != NULL; ++i)
    {
        stack[curs++] = envp[i];
    }
    stack[curs++] = NULL;

    while (*envp++)
    {
        continue;
    }
    *envp += 1; // NULL

    Elf64_auxv_t *auxv = (Elf64_auxv_t *)envp;
    Elf64_auxv_t *aux_stack = (Elf64_auxv_t *)&stack[curs];

    init_auxv(aux_stack, auxv, &curs);

    size_t diff = (char *)&stack[curs] - (char *)&stack[0];

    // Move the stack at the upper addresses / start of stack
    void *rsp = memmove(
        (void *)((size_t)stack + length - diff),
        (void *)stack,
        diff
    );

    return rsp;
}

int main(int argc, char *argv[], char *envp[])
{
    if (argc < 2)
    {
        errx(MISSING_ARGS, "Missing argument");
    }

    int stack_flags = PROT_NONE;
    int elf = open(argv[1], O_RDONLY);

    if (elf == -1)
    {
        errx(FILE_ERROR,
        "Could not open file\nUsage: ./elf-loader path/to/elf <args>");
    }

    Elf64_Ehdr header;

    ssize_t err = read(elf, &header, sizeof(Elf64_Ehdr));
    if (err == -1)
    {
        errx(FILE_ERROR, "Could not read elf headers correctly");
    }

    is_elf_valid(header, argv[1]);

    load_segments(elf, header, &stack_flags);

    close(elf);

    void *rsp = create_stack(argc - 1, argv + 1, envp, stack_flags);

    execute(rsp, header.e_entry);

    return 0;
}
