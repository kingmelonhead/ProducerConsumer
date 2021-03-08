#!/bin/bash
gcc -c lib_mon.c -o lib_mon.o
ar rcs lib_mon.a lib_mon.o
