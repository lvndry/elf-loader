# ELF-loader

## Getting started

```bash
make
./elf-loader path/to/elf [args]
```

## Test

```bash
cd tests
BATH_PATH= BATS_INSTALL_DIR=/usr/lib TARGET=../elf-loader make check
```

## Debug

```bash
gdb ./elf-loader
add-symbol-file path/to/elf
run path/to/elf
```
