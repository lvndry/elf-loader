#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <elf.h>

void load_segments(int elf, Elf64_Ehdr header, int *is_stack_exec);
void init_auxv(Elf64_auxv_t *stack, Elf64_auxv_t auxv[], int *curs);

void *create_stack(int argc, char *argv[], char *envp[], int is_stack_exec);

#endif /* ELF_LOADER_H */
