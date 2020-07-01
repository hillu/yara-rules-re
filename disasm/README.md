# YARA Bytecode dumper/disassembler

This tool uses _libyara_ to either load a compiled ruleset into memory
or to compile text rules to binary form and then dumps a disassembly
dump of the program that has been built from the conditions to
standard output.

## Requirements

- C compiler
- _libyara_ 3 or 4 library and header files
- _pkg-config_, capable of providing info where to find YARA headers
  and libraries

## Author

Hilko Bengen <<bengen@hilluzination.de>>
