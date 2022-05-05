# Simple piping program

This program pipes processes passed as command line arguments to one another.

## Important

This program must be run in a Unix environment or it will not work.

## Building

To build the program run the `make` command in the project directory.

## Running

To run the command just pass the commands you want piped in the normal order, e.g.
running `./pipe ls cat wc` should have identical output to running `ls | cat | wc` in
the shell.

## Clean up

To clean up the binary files run `make clean` in the project directory.