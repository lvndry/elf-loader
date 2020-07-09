#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define poro printf("Hello world");

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

#endif
