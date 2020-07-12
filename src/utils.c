#include <elf.h>
#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "utils.h"
#include "stack.h"

size_t align(size_t size)
{
    return size & ~(PAGE_SIZE -1);
}

// round to the nearest multiple of m
int roundUp(const int num, const int m)
{
    int remainder = num % m;
    if (remainder == 0)
        return num;
    return num + m - remainder;
}

int get_perms(const uint32_t flags)
{
    uint32_t prot = PROT_READ;

    if (flags & PF_W)
        prot |= PROT_WRITE;
    if (flags & PF_X)
        prot |= PROT_EXEC;

  return prot;
}

int is_valid_auxv(uint64_t type)
{
    for (size_t i = 0; i < ARRAY_SIZE(auxv_fields); i++)
    {
        if (auxv_fields[i] == type)
        {
            return 1;
        }
    }
    return 0;
}

int is_elf_valid(Elf64_Ehdr header, char *filename)
{
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0)
    {
        errx(FILE_ERROR, "The file \"%s\" is not an ELF", filename);
    }

    if (header.e_ident[EI_CLASS] != ELFCLASS64)
    {
        errx(UNSUPPORTED_ELF, "File \"%s\": ELF class not supported", filename);
    }

    if (
        header.e_ident[EI_OSABI] != ELFOSABI_SYSV &&
        header.e_ident[EI_OSABI] != ELFOSABI_LINUX
    )
    {
        errx(UNSUPPORTED_ELF, "File \"%s\": ELF OS ABI not supported", filename);
    }

    if (header.e_machine != EM_X86_64)
    {
        errx(UNSUPPORTED_ELF, "File \"%s\" unsupported machine", filename);
    }

    return 1;
}
