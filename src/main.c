#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define PE_HEADER_OFFSET 0x3C
#define PE_SIGNATURE 0x4550 // "PE"
#define ELF_MAGIC 0x7F454C46 // ELF magic number
#define PROGRAM_HEADER_SIZE 56 // Size of program header
#define SECTION_HEADER_SIZE 64 // Size of section header
#define NUMBER_OF_PROGRAM_HEADERS 1 // Number of program headers
#define NUMBER_OF_SECTION_HEADERS 1 // Number of section headers for now
#define ENTRY_POINT_ADDRESS 0x0000000000401000 // Typical entry point for x86-64
#define SECTION_NAME ".text" // Section name for code

// Function to check if a file exists
int file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}

// Function to read and validate the PE header
void read_exe_headers(const char *exe_path) {
    if (!file_exists(exe_path)) {
        fprintf(stderr, "Error: File %s does not exist\n", exe_path);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(exe_path, "rb");
    if (!file) {
        perror("Error: Failed to open EXE file");
        exit(EXIT_FAILURE);
    }

    // Get the PE header offset
    fseek(file, PE_HEADER_OFFSET, SEEK_SET);
    uint32_t pe_offset;
    if (fread(&pe_offset, sizeof(pe_offset), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to read PE header offset\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Read the PE header
    fseek(file, pe_offset, SEEK_SET);
    uint32_t signature;
    if (fread(&signature, sizeof(signature), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to read PE signature\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (signature != PE_SIGNATURE) {
        fprintf(stderr, "Error: Not a valid PE file\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    printf("Valid PE file detected.\n");
    fclose(file);
}

// Function to disassemble the EXE using ndisasm
void disassemble_exe(const char *exe_path) {
    char command[256];
    snprintf(command, sizeof(command), "ndisasm -o 0x00400000 \"%s\" > disassembled.asm", exe_path);
    
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to disassemble the EXE file\n");
        exit(EXIT_FAILURE);
    }
    printf("Disassembled %s to disassembled.asm\n", exe_path);
}

// Function to write a complete ELF header and section header
void write_elf_headers(const char *elf_path) {
    FILE *file = fopen(elf_path, "wb");
    if (!file) {
        perror("Error: Failed to create ELF file");
        exit(EXIT_FAILURE);
    }

    // Write ELF header
    uint8_t elf_header[64] = {
        0x7F, 'E', 'L', 'F', // ELF Magic
        2, // 64-bit
        1, // Little Endian
        1, // ELF version
        0, // OS ABI
        0, // ABI version
        0, 0, 0, 0, // Padding
        2, // Type: Executable
        0x3E, // Machine: x86-64
        1, // Version
        ENTRY_POINT_ADDRESS, // Entry point address
        0x40, // Program header offset
        0, // Section header offset
        0, // Flags
        0x40, // Size of this header
        PROGRAM_HEADER_SIZE, // Size of the program header
        NUMBER_OF_PROGRAM_HEADERS, // Number of program headers
        SECTION_HEADER_SIZE, // Size of the section header
        NUMBER_OF_SECTION_HEADERS, // Number of section headers
        0 // Section header string table index
    };

    if (fwrite(elf_header, sizeof(elf_header), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write ELF header\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Write section header
    uint8_t section_header[SECTION_HEADER_SIZE] = {0};
    strncpy((char*)section_header, SECTION_NAME, 16); // Section name
    section_header[16] = 0x00; // Section type: SHT_PROGBITS
    section_header[18] = 0x01; // Section flags: SHF_EXECINSTR
    section_header[24] = ENTRY_POINT_ADDRESS; // Address in memory
    section_header[40] = sizeof(section_header); // Size of the section

    if (fwrite(section_header, sizeof(section_header), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write section header\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

// Function to assemble the disassembled code
void assemble_code() {
    char command[256];
    snprintf(command, sizeof(command), "nasm -f elf64 disassembled.asm -o output.o");
    
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to assemble the disassembled code\n");
        exit(EXIT_FAILURE);
    }
    printf("Assembled disassembled.asm to output.o\n");
}

// Function to link the object file into an executable
void link_executable(const char *output_executable) {
    char command[256];
    snprintf(command, sizeof(command), "ld -o %s output.o", output_executable);
    
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to link the executable\n");
        exit(EXIT_FAILURE);
    }
    printf("Linked output.o to %s\n", output_executable);
}

// Main function to drive the converter
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_exe> <output_elf>\n", argv[0]);
        return EXIT_FAILURE;
    }

    read_exe_headers(argv[1]);
    disassemble_exe(argv[1]);
    write_elf_headers(argv[2]);

    // Assemble the disassembled code and link it
    assemble_code();
    link_executable(argv[2]);

    printf("Conversion completed successfully.\n");
    return EXIT_SUCCESS;
}