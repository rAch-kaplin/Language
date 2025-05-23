## A PROJECT IN DEVELOPMENT

# Language 

## Contents
- [Overview](#overview)
- [Frontend](#frontend)
- [Backend & CPU](#backend--cpu)
- [Syntax](#syntax)
- [Tree Example](#tree-example)
- [Programs](#programs)
- [How to Build](#how-to-build)

---

## Overview 🏐

**Volleyball Programming Language** is a project composed of three main components: *Frontend*, *Backend*, and *CPU Emulator*.

> [!NOTE]
> **Language features**. At the moment, all operations and functions have not been implemented since the session started) , so here are the important notes:
>
> - The program must have the main function of "volleyball";
> - Functions do not accept arguments, and the return type is void;
> - It is also worth considering that this implementation of the language is written on global variables (there are no local ones.

Each module has its own responsibility and cooperates to process, compile, and execute code written in a custom programming language inspired by volleyball terminology.

## Frontend
- This component reads a text file containing a program written in the volleyball language.
- It performs **lexical analysis** to tokenize numbers, variables, functions, and operators.
- Then, it applies **recursive descent parsing** to check the syntax and build nodes for a tree structure that stores: element type, value, and pointers to children.
- The frontend also includes *graphviz* integration to visualize the syntax tree.
- The final output of the frontend is an **Abstract Syntax Tree (AST)** stored in `data/tree_file.txt`.

## Backend & CPU
- The *backend* starts by reading and reconstructing the AST.
- It then generates **assembly code** for a custom **stack-based virtual machine**.
- The *CPU* part contains:
  - **Assembler** — converts the assembly code into bytecode.
  - **Processor** — reads the bytecode, decodes, and executes the commands.

## Syntax 📘

| Operation                | Keyword         | Translation            |
|--------------------------|------------------|-------------------------|
| `if`                     | `block`          |	блокировка              |
| `while`                  | `rally`          | розыгрыш                |
| `return`                 | `ace`            | ace                     |
| `printf`                 | `serve`          | подача                  |
| `scanf`                  | `receive`        | приём                   |

| Comparison Operator      | Keyword         | Translation            |
|--------------------------|------------------|-------------------------|
| `==`                     | `scores_equal`   | счёт сравнялся          |
| `!=`                     | `scores_diff`    | счёт разный             |
| `>`                      | `leading`        | впереди                 |
| `<`                      | `losing`         | отстаём                 |
| `>=`                     | `not_behind`     | 	не позади             |
| `<=`                     | `not_losing`     | не проигрываем          |

## Tree Example 🌳

<img src="graphviz/img/dump2_0.png" alt="Syntax Tree Image">

## Programs 🎮

To add a new program written in this language, first check the syntax rules, write your code in the `data` directory to a text file and run it.

 Example Program: **EquationSolver**
 
<details><summary>SolveSquare</summary>

```c
play volleyball()
{
    receive(a);
    receive(b);
    receive(c);

    block(a scores_equal 0)
    {
        block(b scores_equal 0)
        {
            serve(999);
            ace 0;
        }

        SolveLinSq();
        ace 0;
    }

    discr = b * b - 4 * a * c;
    c = sqrt(discr);

    block(discr scores_equal 0)
    {
        y = (-1 * b) / (2 * a);
        serve(y);
        ace 0;
    }

    block(discr leading 0)
    {
        SolveQuadSq();
        ace 0;
    }

    block(discr losing 0)
    {
        serve(999);
    }

    ace 0;
}

play SolveLinSq()
{
    q = (-1 * c) / b;
    serve(q);
    ace 0;
}

play SolveQuadSq()
{
    x  = (-1 * b - c) / (2 * a);
    xx = (-1 * b + c) / (2 * a);

    serve(x);
    serve(xx);

    ace 0;
}
```
</details>

Example Program: **Factorial**
<details><summary>Factorial</summary>

```c
play volleyball()
{
    receive(x);

    block (x scores_equal 0)
    {
        serve(1);
    }

    block (x scores_diff 0)
    {
        r = 1;
        c = 1;

        rally (c not_losing x)
        {
            c = c + 1;
            r = r * c;
        }

        serve(r);
    }

    ace 0;
}
```
</details>

## How to Build ⚙️

```
cd CPU
make all
cd ../build
cmake ..
make

../run.sh frontend "SolveSquare.txt"
../run.sh backend
../run.sh asm
../run.sh proc

```
