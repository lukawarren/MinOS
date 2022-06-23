# Minlibc

Minlibc is a port of [PDCLib - Public Domain C Library](https://github.com/DevSolar/pdclib), distributed under the
Creative Commons CC0 license.

## File structure
* functions - main C code provided by PDCLib
* include - include code for PDCLib
* minos - target-specific code mirroring the above structure, again provided by PDCLib but modified where needed (files are automatically overwritten in the build directory at compile-time)
