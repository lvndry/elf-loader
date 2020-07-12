#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

enum Errcode {
  MISSING_ARGS = 110,
  FILE_ERROR = 111,
  MEM_ERROR = 112,
  UNSUPPORTED_ELF = 113,
  SYSCONF_ERROR = 114
};

int is_elf_valid(Elf64_Ehdr header, char *filename);
int get_perms(const uint32_t flags);
int is_valid_auxv(uint64_t type);
int roundUp(const int num, const int m);

size_t align(size_t size);

#endif /* UTILS_H */
