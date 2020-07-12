#include <err.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 160000

#define ARRAY_SIZE(t) sizeof((t)) / sizeof((t)[0])

int value = 2022;

static const char abc = 'b';

char chtab[3];

struct segment {
  uintptr_t begin;
  uintptr_t end;
  unsigned int perms;
  struct segment *next;
};

unsigned int set_perms(const char *perms) {
  unsigned int p = 0;

  for (size_t i = 0; perms[i] != '\0'; i++) {
    switch (perms[i]) {
    case 'r':
      p |= S_IRUSR;
      break;
    case 'w':
      p |= S_IWUSR;
      break;
    case 'x':
      p |= S_IXUSR;
      break;
    }
  }
  return p;
}

struct segment *parse_mapping(void) {
  FILE *f = fopen("/proc/self/maps", "r");

  if (f == NULL)
    errx(1, "Unable to open /proc/self/maps");

  struct segment *segment_list = NULL;
  struct segment *segment_list_end = NULL;
  char *line = NULL;
  size_t line_sz = 0;

  while (getline(&line, &line_sz, f) != -1) {
    struct segment *seg = malloc(sizeof(struct segment));

    if (!segment_list)
      segment_list = seg;

    char garbage[1024] = {0};
    char permissions[5] = {0};

    sscanf(line, "%" PRIx64 "-%" PRIx64 " %s %s\n", &(seg->begin), &(seg->end),
           permissions, garbage);

    seg->perms = set_perms(permissions);

    if (segment_list_end)
      segment_list_end->next = seg;
    else
      seg->next = segment_list_end;
    segment_list_end = seg;
  }

  free(line);
  fclose(f);
  return segment_list;
}

void check_mapping(struct segment *list) {
  struct {
    char *name;
    uintptr_t addr;
    unsigned bits_set;
    unsigned bits_notset;
    int ok;
  } check[] = {{".text", // value
                (uintptr_t)&check_mapping, S_IRUSR | S_IXUSR, S_IWUSR, 0},
               {".data", // value
                (uintptr_t)&value, S_IRUSR | S_IWUSR, S_IXUSR, 0},
               {".rodata", // abc
                (uintptr_t)&abc, S_IRUSR, S_IWUSR | S_IXUSR, 0},
               {".bss", // chtab
                (uintptr_t)&chtab, S_IRUSR | S_IWUSR, S_IXUSR, 0}};

  for (struct segment *s = list; s != NULL; s = s->next) {
    for (size_t i = 0; i != ARRAY_SIZE(check); i++) {
      if (check[i].addr >= s->begin && check[i].addr < s->end &&
          (s->perms & check[i].bits_set) == check[i].bits_set &&
          (s->perms & check[i].bits_notset) == 0) {
        check[i].ok = 1;
      }
    }
  }

  for (size_t i = 0; i != ARRAY_SIZE(check); i++)
    if (check[i].ok != 1)
      printf("A segment (%s) is not correctly mapped\n", check[i].name);
}

int main(void) {
  struct segment *segment_list = parse_mapping();
  check_mapping(segment_list);
  return 0;
}
