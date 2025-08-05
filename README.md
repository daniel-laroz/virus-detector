## ⚠️ Disclaimer – Safe Educational Project

This project is **strictly for educational purposes** and does **not contain any real malware**.

- The file named `infected` is **not harmful** – it is a harmless executable that simply prints:
This file is infected,
I am virus1!
Clean me, Please!

- All “virus signatures” in this project are **fake** and used only to simulate basic virus detection.
- No part of this project is capable of spreading, modifying other software, or causing damage.

✅ This lab was developed as part of a university course on systems programming.


# Virus Detector

A C program that detects and neutralizes binary viruses in ELF executables.  
Built as part of a systems programming lab using low-level Linux techniques, memory-safe code, and linked lists.

## 🧠 Features

- Reads virus signatures from a binary file (`signatures-B` or `signatures-L`)  
- Stores signatures in a linked list structure (`struct link`)
- Detects virus patterns in infected executable files using naive matching
- Neutralizes viruses by patching their code with a `RET` instruction
- Interactive menu-driven CLI interface
- Memory-safe (verified using Valgrind)

## 🐳 Running in 32-bit Linux using Docker

Replace `/full/path/to/virus-detector` with the actual path to your local folder:

```bash
$ docker run --platform linux/386 --privileged --name my-linux32 \
  -it -v "/full/path/to/virus-detector:/virus-detector" \
  ubuntu:18.04 bash

🛠️ Inside the container:

$ apt update
$ apt install -y build-essential valgrind
$ cd /virus-detector
```

## 💡 Example Run

```bash

$ chmod u+x infected
$ ./infected

This file is infected,
I am virus1!
Clean me, Please!

$ make
$ ./AntiVirus infected

Select operation from the following menu (ctrl^D for exit):
1) Load signatures
2) Print signatures
3) Detect viruses
4) Fix file
5) Quit

option : 1
Within bounds
Enter signature file name: signatures-B (or signatures-L)
DONE.

option : 2
Within bounds
Virus name: Doom
Virus size: 19
signature:
B8 04 00 00 00 8B 5C 24 04 8B 4C 24 08 8B 54 24 0C CD 80 
...
DONE.

option : 3
Within bounds
Virus detected!
Starting byte location: 263
Virus name: Doom
Virus signature size: 19
DONE.

option : 4
Within bounds
DONE.

option : 5
Within bounds
Quitting...

$ ./infected
This file is infected,
Clean me, Please! 
# ./infected doesn't print 'I am virus1!' anymore.
```