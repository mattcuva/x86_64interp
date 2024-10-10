#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

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

// Sets `*cur` to index of next non-whitespace character starting from `next`
void nextnonws(char *input, int *cur) {
    while (input[*cur] != '\0' && isspace(input[*cur]))
        (*cur)++;
}

#define NUM_INST 5
char *inststrings[NUM_INST] = {"movb", "movw", "movl", "movq", "mov"};

// Determine the instruction on this line (if known)
int getinst(char *line, int *cur) {
    for (int i = 0; i < NUM_INST; i++) {
        int instlen = strlen(inststrings[i]);
        if (memcmp(inststrings[i], line+*cur, instlen) == 0) {
            (*cur) += instlen;
            return i;
        }
    }
    return -1; // instruction not found
}

char *regstrings[8] = {"rax", "rbx", "rcx", "rdc", "rsi", "rdi", "rbp", "rsp"};

// Determine what address the next operand refers to
void* getoperand(char *line, int *cur) {
    if (strlen(line+*cur)) return NULL;
    char c = *(line+*cur);
    if (c == '%') {
        // register
        for (int i = 0; i < NUM_INST; i++) {
            int reglen = strlen(regstrings[i]);
            if (memcmp(regstrings[i], line+*cur, reglen) == 0) {
                (*cur) += reglen;
                printf("%s\n", regstrings[i]);
                break;
            }
        }
        
    } else if (c == '$') {
        // a literal number

    } else {
        // address

    }
    return NULL;
}

void parseinst(char *line) {
    int cg = 0;
    int sl = strlen(line);
    nextnonws(line, &cg); // move to instruction
    if (cg == sl) return; // blank line

    int inst = getinst(line, &cg); // determine instruction
    if (inst < 0) {
        printf("Unrecognized instruction: %s\n", line);
        return;
    }

    printf("%s\n", inststrings[inst]);

}

int main(int argc, char **argv) {

    FILE *file;
    file = fopen(argv[1], "r");
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        parseinst(line);
    }

    fclose(file);

}

