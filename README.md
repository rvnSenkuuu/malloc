> [!WARNING]
> This project is for educational purposes only and should not be used in production.

# Malloc

An implementation of a dynamic memory allocator in C that uses the `mmap` and `munmap` system calls. This allocator pre-allocates memory into three types of zones: `TINY`, `SMALL`, and `LARGE` to minimize the number of calls to these system calls.

## Build & Usage

Build the .so library:
```console
$ cd malloc
$ make
```

Basic test:
```console
$ make test
```

Usage of the library:
```console
$ LD_PRELOAD=./libft_malloc.so <binary_path>
```