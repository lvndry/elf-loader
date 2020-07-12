#define _GNU_SOURCE

#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h> // DELETE
#include <stdio.h> // DELETE BEFORE PUSH

#include "elf-loader.h"
#include "execute.h"
#include "stack.h"

// DELETE
void show_prog_mapping(void) {
  char *pidmaps = NULL;
  asprintf(&pidmaps, "cat /proc/%u/maps", getpid());
  system(pidmaps);
}

// DELETE
void print_errno(void) { fprintf(stderr, "%s\n", strerror(errno)); }

size_t align(size_t size) { return size & ~(PAGE_SIZE - 1); }

// Rounds up  num to nearest multiple of m
size_t roundUp(long unsigned int num, long m) {
  return ((num + m - 1) / m) * m;
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

size_t envp_length(char *envp[]) {
  size_t size = 0;
  while (envp[size] != NULL) {
    size++;
  }
  return size;
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

    if (ph.p_type != PT_LOAD) {
      continue;
    }

    if (!ph.p_memsz) {
      continue;
    }

    void *seg_space = mmap((void *)ph.p_vaddr, roundUp(ph.p_memsz, PAGE_SIZE),
                           PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, elf,
                           (off_t)align(ph.p_offset));

    if (seg_space == MAP_FAILED) {
      errx(MEM_ERROR, "Could not map segment in memory");
    }

    if (ph.p_memsz > ph.p_filesz) {
      memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
    }

    if (!(ph.p_flags & PF_W)) {
      mprotect(seg_space, ph.p_memsz, PROT_READ);
    }

    // executable implies read
    if (ph.p_flags & PF_X) {
      mprotect(seg_space, ph.p_memsz, PROT_EXEC);
    }
  }

  size_t length = PAGE_SIZE * STACK_PAGES;
  void **ptr = mmap(NULL, align(length), PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    errx(MEM_ERROR, "Could not allocate stack");
  }

  int curs = 0;
  argc -= 1;
  uint64_t cargc = argc;
  ptr[curs++] = (void *)cargc;

  for (int i = 0; i < argc; ++i) {
    ptr[curs++] = argv[i + 1];
  }

  ptr[curs++] = NULL;

  for (int i = 0; envp[i] != NULL; ++i) {
    ptr[curs++] = envp[i];
  }
  ptr[curs++] = NULL;

  while (*envp++)
    ;
  *envp += 1;

  Elf64_auxv_t *auxv = (Elf64_auxv_t *)envp;
  Elf64_auxv_t *aux_stack = (Elf64_auxv_t *)&ptr[curs];

  init_auxv(aux_stack, auxv, &curs);

  size_t diff = (char *)&ptr[curs] - (char *)&ptr[0];

  void *rsp = memcpy((void *)((size_t)ptr + length - diff), (void *)ptr, diff);

  execute(rsp, header.e_entry);

  close(elf);

  // show_prog_mapping();

  return 0;
}
