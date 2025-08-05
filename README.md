# Virus Detector

A C program that detects and neutralizes binary viruses in executables.

This project was developed as part of a systems programming lab at Ben-Gurion University, focusing on binary file manipulation and memory-safe C programming.  
It runs on a 32-bit Linux environment.

---

## 🧠 Features

- Loads virus signatures from custom binary signature files
- Displays virus names, signature lengths, and hex signatures
- Scans suspected executable files for known viruses
- Neutralizes detected viruses by overwriting them with a `RET` instruction
- CLI menu-based interface
- Written in pure C using low-level file I/O and dynamic memory

---

## 📁 File Overview

| File           | Description |
|----------------|-------------|
| `AntiVirus.c`  | Main C source file containing all program logic |
| `makefile`     | For compiling the program |
| `signatures-B` | Virus signature file (big-endian format) |
| `signatures-L` | Virus signature file (little-endian format) |
| `infected`     | Test executable that prints `I am virus1!` — used to verify detection and neutralization |

---

## 🖥️ Menu Interface

After compilation, the program runs an infinite menu loop:

Load signatures

Print signatures

Detect viruses

Fix file

Quit


### Detect viruses
Scans a suspected file (given as a command-line argument) for known virus signatures, and prints:
- Start offset
- Virus name
- Signature size

### Fix file
Neutralizes detected viruses by overwriting their first byte with a `RET` instruction (0xC3), effectively canceling their behavior.

---

## 🐳 Running on a 64-bit Machine (via Docker)

Use Docker with a 32-bit Linux environment:

```bash
docker run -it --rm -v $(pwd):/work -w /work i386/ubuntu bash
apt update && apt install -y build-essential valgrind
make
./AntiVirus infected