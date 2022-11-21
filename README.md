# Brainfuck Interpreter

A simple Brainfuck interpreter written in C. It can also transpile a Brainfuck program to C. The transpiler doesn't check for errors at the moment.

![Screenshot](ss.png)

## Build:
```
make
```

## Usage:
```
bf [OPTION] [FILE]
```

## Examples:
Interpret:
```
bf test.bf
```

Transpile to C:
```
bf --transpile test.bf
```

This interpreter takes about 45 seconds to run [Erik Dubbelboer](https://github.com/erikdubbelboer)'s [Mandelbrot program](https://github.com/erikdubbelboer/brainfuck-jit/blob/master/mandelbrot.bf).