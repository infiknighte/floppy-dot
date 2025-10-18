# Floppy Dot
A tiny **Flappy Bird-style** game written in **C** with [raylib](https://www.raylib.com/).
Created for the **Segfault Club**, **Gohar's Guide** Discord Server's _Game Jam_.

## Build Instructions

### Requirements

- \***raylib** (\*if using [Make](#make), v5.5 or newer recommended)
- **C compiler** (GCC / Clang)
- **CMake** or **Make**

### General Instructions

Clone this repository with
```bash
git clone https://github.com/infiknighte/floppy-dot.git
```
Then go inside of the newly created directory
```bash
cd floppy-dot
```

> NOTE: You should be in the `floppy-dot` directory when running the target
>       Otherwise, it would not be able to access `resources`

_Building with [**CMake**](#cmake) is recommended._

### CMake
Building with **CMake** is easier, just do
```bash
cmake -B build
cmake --build build
```
This will build the target inside of `build`.
Run it by
```
./build/floppydot
```

### Make
To build with **Make**
> NOTE: Make sure you have **raylib** installed on your system.
> With accordance, you _may_ need to edit `RAYLIB` varialbe in the `Makefile`
```bash
make
```
This will create a `build` directory, and inside of it the target `floppydot`.
To run
```bash
./build/floppydot
```
or just do, instead this. Which will build and run on the fly.
```bash
make run
```
You can clean the outputs by
```bash
make clean
```

## Controls

**Jump**           - `W`, `UP ARROW`, `SPACE`, or `Left Click` \
**Pause / Resume** - `P` \
**Quit**           - `Esc` 

Have a good one.
