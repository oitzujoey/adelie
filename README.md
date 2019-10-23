# Adelie ISA

There are two main goals of Adelie:
1. Execute compiled C code  
    None of my other CPUs can run human readable languages. If a compiler exists for Adelie, it would be much easier to write and run programs. In addition, a C compiler would provide a massive amount of code that could be run on Adelie.
2. Run Linux  
    Given a C compiler and enough memory, nearly anything should be able to run Linux. This assumes that a full computer has been built around Adelie, which means that a it will take a very long time if I ever make it to the point of being able to implement this.

Based upon these goals, here are the current specifications for version 0.3 (current):

* 32-bit data and address bus  
    32-bit operations will be fast, but 16-bit and 8-bit operations should be relatively quick as well. Also allows for at least 4 GiB of memory.
* 16-32 general purpose registers  
* Variable length instructions  
    I don't like pure RISC. It may be fast, but the code density is horrible. CISC is large and slow, but it has good instruction density. If I keep the complexity of peripherals to a minimum, I should have space to keep instruction fetch speedy and still have good instruction density. Besides, (as far as I know) after Pentium Pro, it became impossible to categorize any powerful CPUs as purely RISC or CISC.
* Memory-mapped I/O

These features should be sufficient for goal #1. Goal #2 may add more specifications, but a even a small system is a long way away.
