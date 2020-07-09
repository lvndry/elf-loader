#define _GNU_SOURCE // DELETE

#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h> // DELETE
#include <stdio.h> // DELETE BEFORE PUSH

#include "elf-loader.h"
#include "execute.h"

// DELETE
void show_prog_mapping(void) {
  char *pidmaps = NULL;
  asprintf(&pidmaps, "cat /proc/%u/maps", getpid());
  system(pidmaps);
}

unsigned long *get_stack_ptr(void) {
  register unsigned long *ptr __asm("rsp");
  __asm("mov %rsp, %rax");

  return ptr;
}

// DELETE
void print_errno(void) { fprintf(stderr, "%s\n", strerror(errno)); }

size_t align(size_t size) { return size & ~(PAGE_SIZE - 1); }

// Rounds up  num to nearest multiple of m
size_t roundUp(long unsigned int num, long m) {
  return ((num + m - 1) / m) * m;
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

  if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0) {
    errx(FILE_ERROR, "The file \"%s\" is not an ELF", argv[1]);
  }

  if (header.e_ident[EI_CLASS] != ELFCLASS64) {
    errx(UNSUPPORTED_ELF, "File \"%s\": ELF class not supported", argv[1]);
  }

  if (header.e_ident[EI_OSABI] != ELFOSABI_SYSV &&
      header.e_ident[EI_OSABI] != ELFOSABI_LINUX) {
    errx(UNSUPPORTED_ELF, "File \"%s\": ELF OS ABI not supported", argv[1]);
  }

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

    printf("memsz: %ld -- roundup: %ld\n", ph.p_memsz,
           roundUp(ph.p_memsz, PAGE_SIZE));

    void *seg_space = mmap((void *)ph.p_vaddr, roundUp(ph.p_memsz, PAGE_SIZE),
                           PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, elf,
                           (off_t)align(ph.p_offset));

    if (seg_space == MAP_FAILED) {
      errx(MEM_ERROR, "Could not map segment in memory");
    }

    // read-only
    if (!(ph.p_flags & PF_W)) {
      mprotect(seg_space, ph.p_memsz, PROT_READ);
    }

    // executable implies read
    if (ph.p_flags & PF_X) {
      mprotect(seg_space, ph.p_memsz, PROT_EXEC);
    }
  }

  Elf64_auxv_t *auxv;

  // envp is null terminated and auxv is right after
  while (*envp++ != NULL)
    ;

  uintptr_t entrypoint;
  for (auxv = (Elf64_auxv_t *)envp; auxv->a_type != AT_NULL; auxv++) {
    if (auxv->a_type == AT_ENTRY) {
      entrypoint = auxv->a_un.a_val;
    }
  }
  entrypoint = entrypoint;

  int *stack_argc = (void *)&argc;
  *((int *)stack_argc) = argc - 1;
  char ***stack_argv = (char ***)stack_argc + 1;
  *stack_argv = (char **)(argv + 1);

  execute((void *)stack_argc, header.e_entry);

  show_prog_mapping();

  return 0;
}
