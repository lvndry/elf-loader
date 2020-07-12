#define _GNU_SOURCE

#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "elf-loader.h"
#include "execute.h"
#include "stack.h"

size_t align(size_t size) { return size & ~(PAGE_SIZE - 1); }

int roundUp(int numToRound, int multiple) {
  int remainder = numToRound % multiple;
  if (remainder == 0)
    return numToRound;
  return numToRound + multiple - remainder;
}

int get_perms(uint32_t flags) {
  uint32_t prot = PROT_READ;

  if ((flags & PF_W) != 0)
    prot |= PROT_WRITE;
  if ((flags & PF_X) != 0)
    prot |= PROT_EXEC;

  return prot;
}

int is_elf_valid(Elf64_Ehdr header, char *filename) {
  if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0) {
    errx(FILE_ERROR, "The file \"%s\" is not an ELF", filename);
  }

  if (header.e_ident[EI_CLASS] != ELFCLASS64) {
    errx(UNSUPPORTED_ELF, "File \"%s\": ELF class not supported", filename);
  }

  if (header.e_ident[EI_OSABI] != ELFOSABI_SYSV &&
      header.e_ident[EI_OSABI] != ELFOSABI_LINUX) {
    errx(UNSUPPORTED_ELF, "File \"%s\": ELF OS ABI not supported", filename);
  }

  if (header.e_machine != EM_X86_64) {
    errx(UNSUPPORTED_ELF, "File \"%s\" unsupported machine", filename);
  }

  return 1;
}

int is_valid_auxv(uint64_t type) {
  for (size_t i = 0; i < ARRAY_SIZE(auxv_fields); i++) {
    if (auxv_fields[i] == type) {
      return 1;
    }
  }
  return 0;
}

void init_auxv(Elf64_auxv_t *stack, Elf64_auxv_t auxv[], int *curs) {
  int j = 0;
  for (size_t i = 0; auxv[i].a_type != AT_NULL; i++) {
    if (is_valid_auxv(auxv[i].a_type)) {
      stack[j] = auxv[i];
      j++;
      *curs += 2;
    }
  }
  stack[j] = (Elf64_auxv_t){AT_NULL};
  *curs += 2;
}

int main(int argc, char *argv[], char *envp[]) {
  if (argc < 2) {
    errx(MISSING_ARGS, "Missing argument");
  }

  int elf = open(argv[1], O_RDWR);
  if (elf == -1) {
    errx(FILE_ERROR,
         "Could not open file\nUsage: ./elf-loader path/to/elf <args>");
  }

  Elf64_Ehdr header;

  ssize_t err = read(elf, &header, sizeof(Elf64_Ehdr));
  if (err == -1) {
    errx(FILE_ERROR, "Could not read elf headers correctly");
  }

  is_elf_valid(header, argv[1]);

  // Seeking program headers
  lseek(elf, header.e_phoff, SEEK_SET);

  Elf64_Phdr ph;

  for (int i = 0; i < header.e_phnum; ++i) {
    read(elf, &ph, sizeof(Elf64_Phdr));

    if (ph.p_type != PT_LOAD && ph.p_type != PT_GNU_STACK) {
      continue;
    }

    if (!ph.p_memsz) {
      continue;
    }

    void *seg_space = mmap((void *)ph.p_vaddr, roundUp(ph.p_memsz, PAGE_SIZE),
                           get_perms(ph.p_flags), MAP_PRIVATE, elf,
                           (off_t)align(ph.p_offset));

    if (seg_space == MAP_FAILED) {
      errx(MEM_ERROR, "Could not map segment in memory");
    }

    if (ph.p_memsz > ph.p_filesz) {
      memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
    }
  }

  size_t length = PAGE_SIZE * STACK_PAGES;
  void **stack = mmap(NULL, align(length), PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (stack == MAP_FAILED) {
    errx(MEM_ERROR, "Could not allocate stack");
  }

  int curs = 0;
  argc -= 1;
  uint64_t cargc = argc;
  stack[curs++] = (void *)cargc;

  for (int i = 0; i < argc; ++i) {
    stack[curs++] = argv[i + 1];
  }

  stack[curs++] = NULL;

  for (int i = 0; envp[i] != NULL; ++i) {
    stack[curs++] = envp[i];
  }
  stack[curs++] = NULL;

  while (*envp++)
    ;
  *envp += 1;

  Elf64_auxv_t *auxv = (Elf64_auxv_t *)envp;
  Elf64_auxv_t *aux_stack = (Elf64_auxv_t *)&stack[curs];

  init_auxv(aux_stack, auxv, &curs);

  size_t diff = (char *)&stack[curs] - (char *)&stack[0];

  void *rsp =
      memmove((void *)((size_t)stack + length - diff), (void *)stack, diff);

  close(elf);

  execute(rsp, header.e_entry);

  return 0;
}
