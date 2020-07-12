#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <elf.h>
#include <stddef.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

enum Errcode {
  MISSING_ARGS = 110,
  FILE_ERROR = 111,
  MEM_ERROR = 112,
  UNSUPPORTED_ELF = 113,
  SYSCONF_ERROR = 114
};

struct Error {
  enum Errcode error_code;
  char *message;
};

int get_perms(uint32_t flags);
int is_elf_valid(Elf64_Ehdr header, char *filename);
int is_valid_auxv(uint64_t type);
int roundUp(int numToRound, int multiple);

size_t align(size_t size);

void load_segments(int elf, Elf64_Ehdr header, int *is_stack_exec);
void init_auxv(Elf64_auxv_t *stack, Elf64_auxv_t auxv[], int *curs);

void *create_stack(int argc, char *argv[], char *envp[]);

#endif /* ! ELF_LOADER_H */
