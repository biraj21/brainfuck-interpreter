#include "stack.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXMEM 30000 // number of memory cells

enum InstructionType {
  INC_PTR = '>',
  DEC_PTR = '<',
  INC = '+',
  DEC = '-',
  READ = ',',
  WRITE = '.',
  LOOP_START = '[',
  LOOP_END = ']',

  // to mark end of program
  END = -1,
};

struct Instruction {
  enum InstructionType type;
  // how many times this instruction was "continuously" repeated in the source
  size_t count;
};

enum Action { INTERPRET, TRANSPILE };

void free_memory(void);
struct Instruction *parse_file(FILE *fp);
void interpret(struct Instruction *code);
void transpile(struct Instruction *code, FILE *output_fp);

unsigned char memory[MAXMEM] = {0}; // memory cells (1 byte each)
unsigned short ptr = 0;             // currently pointed memory cell

Stack jumps = STACK_INIT; // stores indices of '[' (opened loops)

// brainfuck code's optimized representation
// global variable so that it can be freed in the atexit() handler
struct Instruction *code = NULL;

int main(int argc, const char *argv[]) {
  enum Action action = INTERPRET;
  const char *file_name = NULL;
  if (argc > 1) {
    if (strcmp(argv[1], "--transpile") == 0) {
      action = TRANSPILE;
    } else if (strncmp(argv[1], "--", 2) == 0) {
      fprintf(stderr, "bf: Error: Invalid option %s\n", argv[1]);
      return EXIT_FAILURE;
    }

    if (action == TRANSPILE && argc > 2) {
      file_name = argv[2];
    } else if (action == INTERPRET && argc > 1) {
      file_name = argv[1];
    }
  }

  FILE *fp = file_name == NULL ? stdin : fopen(file_name, "r");

  // free memory when the program exits
  atexit(free_memory);

  code = parse_file(fp);

  if (fp == stdin) {
    puts("\nexecuting code from stdin...");
    freopen("/dev/tty", "r", stdin); // reopening stdin in case the Brainfuck
                                     // program needs user input
  } else {
    fclose(fp);
  }

  if (action == TRANSPILE) {
    transpile(code, stdout);
  } else {
    interpret(code);
  }

  return EXIT_SUCCESS;
}

void free_memory(void) {
  free(code);
  while (!stack_is_empty(&jumps)) {
    stack_pop(&jumps);
  }
}

void interpret(struct Instruction *code) {
  for (size_t i = 0; code[i].type != END;) {
    struct Instruction inst = code[i];
    if (inst.type == INC_PTR) {
      ptr += inst.count;

      if (ptr >= MAXMEM) {
        fprintf(stderr,
                "bf: Runtime Error: Cell index out of range. Pointer has "
                "exceeded the MAXIMUM memory cell index (%d).\n",
                ptr);
        exit(EXIT_FAILURE);
      }
    } else if (inst.type == DEC_PTR) {
      ptr -= inst.count;

      if (ptr < 0) {
        fprintf(stderr,
                "bf: Runtime Error: Cell index out of range. Pointer has "
                "exceeded the MINIMUM memory cell index (%d).\n",
                ptr);
        exit(EXIT_FAILURE);
      }
    } else if (inst.type == INC) {
      memory[ptr] += inst.count;
    } else if (inst.type == DEC) {
      memory[ptr] -= inst.count;
    } else if (inst.type == LOOP_START) {
      if (memory[ptr] == 0) {
        ++i;
        int loop_depth = 1;
        for (; loop_depth > 0 && code[i].type != '\0'; ++i) {
          if (code[i].type == LOOP_START) {
            ++loop_depth;
          } else if (code[i].type == LOOP_END) {
            --loop_depth;
          }
        }

        if (loop_depth > 0) {
          puts("bf: Warning: Unclosed loop. Missing a ']'.");
        }

        continue;
      } else {
        stack_push(&jumps, i + 1);
      }
    } else if (inst.type == LOOP_END) {
      if (stack_is_empty(&jumps)) {
        fputs("bf: Runtime Error: Unnecessary ']'. Opening '[' not found for "
              "current ']'\n",
              stderr);
        exit(EXIT_FAILURE);
      }

      if (memory[ptr] == 0) {
        stack_pop(&jumps);
      } else {
        i = jumps.top->value;
        continue;
      }
    } else if (inst.type == READ) {
      memory[ptr] = getchar();
    } else if (inst.type == WRITE) {
      putchar(memory[ptr]);
    }

    ++i;
  }

  if (!stack_is_empty(&jumps)) {
    puts("bf: Warning: Unclosed loop. Missing a ']'.");
  }
}

// check if the character is a valid brainfuck character
static bool is_valid_bf_char(int c) {
  return c == INC_PTR || c == DEC_PTR || c == INC || c == DEC ||
         c == LOOP_START || c == LOOP_END || c == READ || c == WRITE;
}

// check if the character is a groupable brainfuck character
static bool is_groupable(int c) {
  return c == INC_PTR || c == DEC_PTR || c == INC || c == DEC;
}

struct Instruction *parse_file(FILE *fp) {
  size_t capacity = 128;
  struct Instruction *instructions = malloc(sizeof(struct Instruction) * 128);
  if (instructions == NULL) {
    fclose(fp);
    perror("bf: Error");
    exit(EXIT_FAILURE);
  }

  size_t i = 0;
  while (true) {
    int c = fgetc(fp);
    if (c == EOF) {
      break;
    }

    // skip non-brainfuck characters
    if (!is_valid_bf_char(c)) {
      continue;
    }

    if (is_groupable(c) && i > 0 && instructions[i - 1].type == c) {
      instructions[i - 1].count++;
      continue;
    }

    // add new instruction
    instructions[i].type = c;
    instructions[i].count = 1;

    ++i;
    if (i == capacity - 1) {
      capacity *= 2;
      struct Instruction *new_buf =
          realloc(instructions, sizeof(struct Instruction) * capacity);
      if (new_buf == NULL) {
        if (fp != stdin) {
          fclose(fp);
        }

        free(instructions);
        perror("bf: Error");
        exit(EXIT_FAILURE);
      }

      instructions = new_buf;
    }
  }

  instructions[i].type = END;
  return instructions;
}

static void add_indentation(unsigned int indent_level, FILE *fp) {
  for (unsigned int i = 0; i < indent_level; ++i) {
    fputs("    ", fp);
  }
}

void transpile(struct Instruction *code, FILE *output_fp) {
  fputs("// This file is automatically generated from Biraj's Brainfuck "
        "transpiler.\n\n"
        "#include <stdio.h>\n\n"
        "#define MAXMEM 30000\n\n"
        "int main(void) {\n"
        "    char memory[MAXMEM];\n"
        "    char *ptr = memory;\n\n",
        output_fp);

  int indent_level = 1;
  for (size_t i = 0; code[i].type != '\0';) {
    struct Instruction inst = code[i];
    if (inst.type == INC_PTR) {
      add_indentation(indent_level, output_fp);
      fprintf(output_fp, "ptr += %zu;\n", inst.count);
    } else if (inst.type == DEC_PTR) {
      add_indentation(indent_level, output_fp);
      fprintf(output_fp, "ptr -= %zu;\n", inst.count);
    } else if (inst.type == INC) {
      add_indentation(indent_level, output_fp);
      fprintf(output_fp, "*ptr += %zu;\n", inst.count);
    } else if (inst.type == DEC) {
      add_indentation(indent_level, output_fp);
      fprintf(output_fp, "*ptr -= %zu;\n", inst.count);
    } else if (inst.type == LOOP_START) {
      add_indentation(indent_level, output_fp);
      fputs("while (*ptr) {\n", output_fp);
      ++indent_level;
    } else if (inst.type == LOOP_END) {
      --indent_level;
      add_indentation(indent_level, output_fp);
      fputs("}\n", output_fp);
    } else if (inst.type == READ) {
      add_indentation(indent_level, output_fp);
      fputs("*ptr = getchar();\n", output_fp);
    } else if (inst.type == WRITE) {
      add_indentation(indent_level, output_fp);
      fputs("putchar(*ptr);\n", output_fp);
    }

    ++i;
  }

  fputs("\n    return 0;\n"
        "}\n",
        output_fp);
}
