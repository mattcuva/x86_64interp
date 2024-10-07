#include <stdio.h>
#include <assert.h>

unsigned long long registers[16]; // Registers inside CPU
// Register reference shortcuts
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
unsigned char mem[MEM_SIZE]; // Memory the CPU has access to
#define ref(Imm, Rb, Ri, s) mem[Imm + Rb + Ri*s] // memory reference translation

/*
Print the data in the CPU's mem.

    `rows`: How many rows to print from mem
    `cols`: How many bytes per row to display
*/
void memdump(int rows, int cols) {
    printf("      ");
    for (int c = 0; c < cols; c++)
        printf("+%i    ", c);
    printf("\n");
    for (int r = 0; r < rows; r++) {
        printf("0x%02x: ", r*8);
        for (int c = 0; c < cols; c++)
            printf("0x%02hhx  ", mem[r*cols + c]);
        printf("\n");
    }
}

/*
Print the data in the CPU's registers.

    `regs`: Each bit's slot # from LSD dictates whether the corrosponding register at that index is printed 
            - i.e. 0b0000000000001101: print rax, rcx, rdx   
*/
void regdump(int regs) {
    char* names[16] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    for (int i = 0; i < 16; i++) {
        if ((regs >> i) & 1) {
            printf("%%%s:  %016llx\n", names[i], registers[i]);
        }
    }
}

/*
Function for 'mov' instruction.

    `src`: Value to be moved
   `*dst`: Destination address for src
      `w`: Operand size ('b' = 1B, 'w' = 2B, 'l' = 4B, 'q' = 8B)
*/
void mov(void *src, void *dst, char w) {
    switch(w) {
        case 'b':   
            *(unsigned char *)dst = *(unsigned char*)src; // move 1 byte
            break;
        case 'w':
            *(unsigned short *)dst = *(unsigned short *)src; // move 2 bytes
            break;
        case 'l':
            *(unsigned int *)dst = *(unsigned int *)src; // move 4 bytes
            for (int i = 0; i < 16; i++) {
                if (dst == &registers[i]) {
                    registers[i] &= 0x00000000ffffffff; // Simulate x86_64 bug when performing 32-bit op on 64-bit register
                    break;
                }
            }
            break;
        case 'q':
            *(unsigned long long *)dst = *(unsigned long long *)src; // move 8 bytes
            break;
        default:
            assert(0 && "Invalid mov() call");
    }
}

int main(int argc, char **argv) {

    unsigned char m[MEM_SIZE] = {0xE0, 0xEA, 0x64, 0xD4, 0xCE, 0x66, 0x55, 0x7C, 
                                 0x95, 0x48, 0x78, 0xC3, 0x3F, 0x97, 0x86, 0x39,
                                 0xBA, 0x88, 0xF3, 0x5F, 0x79, 0x23, 0xF9, 0x52,
                                 0x13, 0x71, 0xEC, 0x34, 0x61, 0x9A, 0x28, 0xCC,
                                 0xB1, 0x0F, 0xE3, 0xEF, 0x5E, 0x9F, 0x0C, 0xF8,
                                 0x7D, 0xDB, 0x16, 0x24, 0x2C, 0x8E, 0xAE, 0xCF}; // sample data for memory
    for (int i = 0; i < 48; i++)
        mem[i] = m[i];

    rax = 0x0000000000000008;
    rcx = 0x0000000000000003;
    rdx = 0xffffffffffffffff;

    memdump(3, 8);
    regdump(0b1101);
    printf("\n");
    
    mov(&ref(0,rax,rcx,1), &rdx, 'b'); // movb (%rax, %rcx), %rdx
    regdump(0b1000);
    mov(&ref(0,rax,rcx,1), &rdx, 'w'); // movw (%rax, %rcx), %rdx
    regdump(0b1000);
    mov(&ref(0,rax,rcx,1), &rdx, 'l'); // movl (%rax, %rcx), %rdx
    regdump(0b1000);
    mov(&ref(0,rax,rcx,1), &rdx, 'q'); // movq (%rax, %rcx), %rdx
    regdump(0b1000);

    long s = ~0ll;
    mov(&s, &ref(2,0,rcx,3), 'l');     // movl $0xffffffff, 2(,%rcx,3)
    memdump(3, 8);

}

