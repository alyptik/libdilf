# libdilf - Dynamic Images I'd Like to Format

Dynamically load non-shared ELF executables with dlopen().

## Dependencies

* gcc
* libdl
* libelf

## Usage

Run `make` (optionally `make install`) to build the shared/static libraries.

##### In your C source files:
```c
#include <dilf.h>
```

##### Link against either:

* `-ldilf` (shared or static)
* `-l:libdilf.a` (static only)

## Author

Joey Pabalinas <joeypabalinas@gmail.com>

## Unit Test Framework

* libtap ([zorgnax/libtap](https://github.com/zorgnax/libtap))
