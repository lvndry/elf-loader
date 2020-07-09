#define _GNU_SOURCE

#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h> // DELETE BEFORE PUSH

#include "elf-loader.h"

void show_prog_mapping() {
  char *pidmaps = NULL;
  asprintf(&pidmaps, "cat /proc/%u/maps", getpid());
  system(pidmaps);
}

size_t align(size_t size) {
  if (size % 16 != 0) {
    size = ((size >> 4) << 4) + 16;
  }

  return size;
}

int main(int argc, char *argv[]) {
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
    err = read(elf, &ph, sizeof(Elf64_Phdr));
    if (err == -1) {
      errx(FILE_ERROR, "Could not read program headers correctly");
    }
    printf("addr: %p\n", (void *)ph.p_vaddr);
    printf("filesz: %ld\n", ph.p_filesz);
    printf("memsz: %ld\n\n", ph.p_memsz);
  }

  size_t length = 1;
  printf("length: %ld\n", length);
  printf("offset: %ld\n", header.e_phoff);
  printf("phnum: %d\n", header.e_phnum);
  void *addr = (void *)0x400000;
  void *elf_space =
      mmap(0x00, align(length), PROT_EXEC | PROT_READ | PROT_WRITE, MAP_SHARED,
           elf, 0x1000);

  if (elf_space == MAP_FAILED) {
    errx(MEM_ERROR, "Failed to map \"%s\" segments: %s", argv[1],
         strerror(errno));
  }

  printf("%p\n", addr);
  printf("%p\n", elf_space);

  show_prog_mapping();

  return 0;
}
