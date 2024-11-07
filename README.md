# SIC-ASSEMBLER
This project provides an assembler for the SIC (Simplified Instructional Computer) architecture. The assembler translates SIC assembly code into object code, facilitating its execution or further processing.
Table of Contents
Overview
Features
Usage
Files Included
Example Input and Output
Overview
The SIC assembler reads assembly language code written for the SIC machine, processes it, and generates object code for each line of the program. The assembler uses both the OPTAB (Opcode Table) and SYMTAB (Symbol Table) to translate instructions and labels into machine-readable code.

Features
OPTAB and SYMTAB: Stores and retrieves opcodes and symbols to generate object code.
Address Assignment: Automatically calculates memory locations for each instruction.
Object Code Generation: Outputs machine code for each instruction in the input program.
Error Handling: Detects and reports errors like undefined symbols and duplicate labels.
Usage
Compiling the Code
Compile the assembler with:

bash
Copy code
g++ -std=c++11 -o sic_assembler sic_assembler.cpp table.cpp
Running the Assembler
bash
Copy code
./sic_assembler
Ensure the assembly code file, code.txt, is in the same directory.

Input File (code.txt)
This file should contain the SIC assembly language code. Each line should be formatted with labels, instructions, and operands as required by SIC.

Output
The console output includes the generated object code, starting and ending addresses, and text records formatted according to SIC conventions.
Files Included
sic_assembler.cpp: Main assembler code for SIC.
table.cpp and table.h: Contains helper functions for generating and using the OPTAB and SYMTAB.
