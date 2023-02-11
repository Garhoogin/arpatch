AR Patch Maker

To use:
To start a new project, create a new folder here and copy the baseaddr.txt,
linker.x, Makefile, and symbols.x files from the test folder into it. The
baseaddr.txt file contains the base address that code should be loaded into, so
change this if the address conflicts with anything you might have, such as if
you have another such code already in place and it can't overlap. The symbols.x
file will contain a list of symbols in the base game, update or replace it as
necessary. In that folder, create a source folder. In there, write your C and/or
assembly code to get compiled/assembled and linked. Once done, create a file
called hooks.txt which contains a list of hooks to place in memory. Start each
line with an 8-character hexadecimal address of the hook location, followed by
an assembly instruction that will be placed there. Assembly instructions may
reference any symbol in the symbols.x, and any global symbols declared in
compiled code. Once all is done, simply run make in the project folder and, if
it succeeds, it will print out an Action Replay code that contains your code and
patches.