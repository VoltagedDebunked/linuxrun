# Linuxrun

Linuxrun is a tool designed to convert Windows PE (Portable Executable) files into Linux ELF (Executable and Linkable Format) executables. This tool disassembles Windows executables, processes the assembly, and assembles and links the resulting code to create a runnable ELF file on Linux systems.

## Features

- Validates PE headers to ensure input files are executable.
- Disassembles PE files using `ndisasm`.
- Writes a complete ELF header and section header.
- Assembles the disassembled code using `nasm`.
- Links the object file into an ELF executable using `ld`.

## Requirements

- **Linux**: The tool is developed for Linux environments.
- **Tools**:
  - `gcc`: The GNU Compiler Collection.
  - `nasm`: An assembler for x86 architectures.
  - `ndisasm`: A disassembler for x86/x64 code.
  - `ld`: The GNU linker.

You can install these tools using your package manager. For example, on Debian-based systems:

```bash
sudo apt-get install gcc nasm binutils
```

## Installation

1. Clone the repository or download the source code:
```bash
git clone https://github.com/voltageddebunked/linuxrun.git
cd Linuxrun
```

2. Compile the program:
```bash
bash compile.sh
```

## Usage

To convert a Windows executable to a Linux ELF executable, run the following command:

```bash
./main <input_exe> <output_elf>
```

**Example**:

```bash
./main example.exe output.elf
```

This command will:
1. Validate the PE file.
2. Disassemble `example.exe` into an assembly file (`disassembled.asm`).
3. Generate an ELF header and section for `output.elf`.
4. Assemble the disassembled code into an object file (`output.o`).
5. Link `output.o` into `output.elf`.

## Contributing

Contributions are welcome! If you'd like to contribute to the project, please follow these steps:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes and commit them (`git commit -m 'Add new feature'`).
4. Push to the branch (`git push origin feature-branch`).
5. Open a pull request.

## License

This project is licensed under the GPL 3.0 License - see the [LICENSE](LICENSE) file for details. This software is fully free, open source and fully open to anyone, you can make money off of it, you can resell it, you can use it for work, and any circumstance as long as its legal.

## Contact

For questions or suggestions, please open an issue in the repository or contact me at [rusindanilo@gmail.com].