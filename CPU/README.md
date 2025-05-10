# Processor

This programm is a lightweight stack based processor.

## Build and Run
1) Cloning repository:
```
git@github.com:rAch-kaplin/CPU.git
```

2) You should create .txt file which contains assembly code.
3) Compiling:
```
make asm
make proc
```
or
```
make all
```
4) To create the executable code of your assembly file (file_asm.asm), enter:
```
./asm -f "file_asm.asm"
```

5) To calculate:
```
./proc
```
or
```
./proc -m COLOR_MODE        // An option for displaying a color logo in the terminal
```
<img src="img/color-log.png" height="300">


## Main features
- Registers
- Stack
- RAM

## Commands

### Features
- 21 commands
- 3 types of arguments
    - digit
    - register
    - memory (RAM)
- methods (`call` and `ret` commands)

### Syntax example
```
push ax           ; push value from ax register to stack
pop [bx + 1]      ; pop value from stack to RAM to address from register bx + 1
jmp label:        ; jump to label named "label"
jbe label:        ; jump if the condition <= is true
```
#### Jumps

You need to specify label in code. Then you can give it to jump function, and it will jump on this label.
P.S.
```
jmp next:
...
next:
```
```
call main:

main:
...
ret
```
### Sample programs

Programs such were written in this assembler:
-   solving quadratic equations
-   calculating factorial recursively
-   iteratively calculating Fibonacci numbers

