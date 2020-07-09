#pragma once

#include <stdint.h>

extern void execute(void* stack_ptr, uintptr_t entrypoint) __attribute__((noreturn));
