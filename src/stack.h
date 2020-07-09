#pragma once

#include <elf.h>

#define STACK_PAGES	100

#define ARRAY_SIZE(a) \
	(sizeof((a))/sizeof((a)[0]))

static const uint64_t auxv_fields[] = {
	AT_HWCAP,
	AT_PAGESZ,
	AT_CLKTCK,
	AT_ENTRY,
	AT_FLAGS,
	AT_UID,
	AT_EUID,
	AT_GID,
	AT_EGID,
	AT_SECURE,
	AT_RANDOM,
	AT_HWCAP2,
	AT_PLATFORM,
};
