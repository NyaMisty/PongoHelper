# Helper module for PongoKit

## Usage
Using this module, the following 3 commands will be available in pongoOS shell:
- mread: read DWORD from memory
    The syntax is ```mread 0xaaaaa```. The only supported input format is hex in lower case. Please also pay attention to the space, only one space is allowed
    The output will be a DWORD (little endian)
- mwrite: write to memory
    The syntax is ```mwrite 0xaaaaaaaa f 0xbbbbbbbb```. 0xaaaaaaaa is the target addr to write, f is the writing width, and 0xbbbbbbbb is the value to be written. Addrs' format is same as above, and f can be b,w,d,q which refers to bytes, word, dword, qword.
- dis: disassemble
    The symtax is ```dis 0xaaaaaaaa [0xbbb]```, 0xaaaa is the address to be disassembled; 0xbbbb is number of instructions to disassemble, which could be omitted.

## Build & Run
```
$ git clone https://github.com/checkra1n/PongoKit
$ cd PongoKit
$ git clone --recursive https://github.com/NyaMisty/PongoTest test
$ cd test
$ ./make.sh
$ python ../scripts/module_load.py test_module
```
