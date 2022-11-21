#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "transpiler.h"

static void add_identation(unsigned int indent_level, FILE *fp) {
    for (unsigned int  i = 0; i < indent_level; ++i) {
        fputs("    ", fp);
    }
}

static size_t get_repeat_count(char c, char *code, size_t *from_ptr) {
    size_t count = 0;
    while (code[*from_ptr] == c) {
        ++(*from_ptr);
        ++count;
    }

    return count;
}

void transpile(char *code, FILE *output_fp) {
    fputs(
        "#include <stdio.h>\n\n"
        "#define MAXMEM 30000\n\n"
        "int main(void) {\n"
        "    char memory[MAXMEM];\n"
        "    char *ptr = memory;\n\n",
        output_fp
    );

    int indent_level = 1;
    for (size_t i = 0; code[i] != '\0';) {
        char c = code[i];
        if (c == '>') {
            add_identation(indent_level, output_fp);
            fprintf(output_fp, "ptr += %zu;\n", get_repeat_count('>', code, &i));
            continue;
        } else if (c == '<') {
            add_identation(indent_level, output_fp);
            fprintf(output_fp, "ptr -= %zu;\n", get_repeat_count('<', code, &i));
            continue;
        } else if (c == '+') {
            add_identation(indent_level, output_fp);
            fprintf(output_fp, "*ptr += %zu;\n", get_repeat_count('+', code, &i));
            continue;
        } else if (c == '-') {
            add_identation(indent_level, output_fp);
            fprintf(output_fp, "*ptr -= %zu;\n", get_repeat_count('-', code, &i));
            continue;
        } else if (c == '[') {
            add_identation(indent_level, output_fp);
            fputs("while (*ptr) {\n", output_fp);
            ++indent_level;
        } else if (c == ']') {
            --indent_level;
            add_identation(indent_level, output_fp);
            fputs("}\n", output_fp);
        } else if (c == ',') {
            add_identation(indent_level, output_fp);
            fputs("*ptr = getchar();\n", output_fp);
        } else if (c == '.') {
            add_identation(indent_level, output_fp);
            fputs("putchar(*ptr);\n", output_fp);
        }

        ++i;
    }

    fputs(
        "\n    return 0;\n"
        "}\n",
        output_fp
    );
}
