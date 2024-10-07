#include <stdio.h>

unsigned long registers[16]; // Registers inside CPU
// Register names
#define rax registers[0]
#define rbx registers[1]
#define rcx registers[2]
#define rdx registers[3]
#define rsi registers[4]
#define rdi registers[5]
#define rbp registers[6]
#define rsp registers[7]
// r8 ... r15 are trivial

#define MEM_SIZE 8*512
unsigned char memory[MEM_SIZE]/* // Memory the CPU has access to*/ =
                                {0xE0, 0xEA, 0x64, 0xD4, 0xCE, 0x66, 0x55, 0x7C, 
                                 0x95, 0x48, 0x78, 0xC3, 0x3F, 0x97, 0x86, 0x39,
                                 0xBA, 0x88, 0xF3, 0x5F, 0x79, 0x23, 0xF9, 0x52,
                                 0x13, 0x71, 0xEC, 0x34, 0x61, 0x9A, 0x28, 0xCC,
                                 0xB1, 0x0F, 0xE3, 0xEF, 0x5E, 0x9F, 0x0C, 0xF8,
                                 0x7D, 0xDB, 0x16, 0x24, 0x2C, 0x8E, 0xAE, 0xCF};
#define ref(Imm, Rb, Ri, s) memory[Imm + Rb + Ri*s] // Shorthand memory reference

/*
Print the data in the CPU's memory.

    `rows`: How many rows to print from memory
    `cols`: How many bytes per row to display
*/
void memdump(int rows, int cols) {
    printf("      ");
    for (int c = 0; c < cols; c++)
        printf("+%i    ", c);
    printf("\n");
    for (int r = 0; r < rows; r++) {
        printf("0x%02X: ", r*8);
        for (int c = 0; c < cols; c++)
            printf("0x%02hhX  ", memory[r*cols + c]);
        printf("\n");
    }
}

/*
Print the data in the CPU's registers.

    `regs`: Interpreted in binary where the bit in each slot determines whether to print that register (LSB = reg[0], etc)
             
*/
void regdump(short regs) {
    char* names[16] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    for (int i = 0; i < 16; i++) {
        if ((regs >> i) & 1) {
            printf("%%%s:  %016lX\n", names[i], registers[i]);
        }
    }
}

int main(int argc, char **argv) {

    rax = 0x0000000000000008;
    rcx = 0x0000000000000003;
    rdx = 0x0000000000000000;

    memdump(6, 8);
    regdump(0b0000000000001101);

}

