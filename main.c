#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

/*
 # comment

 [3 9 1 5] # array literal (double array with shape 1x4)

 3         # number literal (double)
           # equals to an array with shape 1x1

 # .       # duplicate
 # ;       # drop
 # :       # swap
 # ∘       # void function (no effect)

 # +       # add (adds two arrays element wise)

 */

#define ERR(msg) do { \
    fprintf(stderr, "%s\n", msg); \
    exit(1); \
} while (0)

bool unicode_match(char *a, char *b) {
    size_t b_len = strlen(b);
    return !strncmp(a, b, b_len);
}

bool UC_UC_MA;
#define UC(a, b) \
    UC_UC_MA = unicode_match(a, b); \
    if (UC_UC_MA) { i += strlen(b) - 1; }\
    if (UC_UC_MA)

void compile(char *code, size_t len, FILE *main, FILE *top) {
    for (size_t i = 0; i < len; i++) {
        char *curr = code + i;
        UC(curr, "∵") {
            fprintf(main, "  each(&s);\n");
            continue;
        }
        UC(curr, "×") {
            fprintf(main, "  mul(&s);\n");
            continue;
        }
        UC(curr, "÷") {
            fprintf(main, "  div(&s);\n");
            continue;
        }
        UC(curr, "↙") {
            fprintf(main, "  take(&s);\n");
            continue;
        }
        UC(curr, "↘") {
            fprintf(main, "  drop(&s);\n");
            continue;
        }
        UC(curr, "↻") {
            fprintf(main, "  rot(&s);\n");
            continue;
        }
        UC(curr, "⇌") {
            fprintf(main, "  rev(&s);\n");
            continue;
        }
        UC(curr, "♭") {
            fprintf(main, "  deshape(&s);\n");
            continue;
        }
        UC(curr, "≅") {
            fprintf(main, "  match(&s);\n");
            continue;
        }
        UC(curr, "⊂") {
            fprintf(main, "  join(&s);\n");
            continue;
        }
        UC(curr, "⊏") {
            fprintf(main, "  select_op(&s);\n");
            continue;
        }
        UC(curr, "⊡") {
            fprintf(main, "  pick(&s);\n");
            continue;
        }
        UC(curr, "⊗") {
            fprintf(main, "  indexof(&s);\n");
            continue;
        }
        UC(curr, "∊") {
            fprintf(main, "  member(&s);\n");
            continue;
        }
        switch (code[i]) {
            case '#': {
                while (i < len && code[i] != '\n') {
                    i++;
                }
                break;
            }
            case '(': { // anonymous function literal
                size_t indent = 0;
                size_t j = i + 1;
                while (j < len && (code[j] != ')' || indent > 0)) {
                    if (code[j] == '(') {
                        indent++;
                    } else if (code[j] == ')') {
                        indent--;
                    }
                    j++;
                }
                if (j == len) {
                    ERR("Unmatched parenthesis!");
                }
                int anon = rand();
                FILE *f = tmpfile();
                compile(code + i + 1, j - i - 1, f, top);
                fprintf(top, "void f_%i() {\n", anon);
                rewind(f);
                int c;
                while ((c = getc(f)) != EOF) {
                    putc(c, top);
                }
                fclose(f);
                fprintf(top, "}\n");
                fprintf(main, "  push_addr(&s, f_%i);\n", anon);
                i = j;
                break;
            }
            case '[': {
                fprintf(main, "  new_array(&s);\n");
                break;
            }
            case ']': {
                fprintf(main, "  end_array(&s);\n");
                break;
            }
            case '!': {
                fprintf(main, "  call(&s);\n");
                break;
            }
            case ';': {
                fprintf(main, "  pop(&s);\n");
                break;
            }
            case '.': {
                fprintf(main, "  dup(&s);\n");
                break;
            }
            case ':':
                fprintf(main, "  swap(&s);\n");
                break;
            case ',':
                fprintf(main, "  over(&s);\n");
                break;
            case '+':
                fprintf(main, "  add(&s);\n");
                break;
            case '-':
                fprintf(main, "  sub(&s);\n");
                break;
            case ' ':
            case '\n' :
                break;
            default: {
                if (isdigit(code[i])) {
                    // until next non-digit or dot
                    size_t j = i;
                    while (j < len && (isdigit(code[j]) || code[j] == '.')) {
                        j++;
                    }
                    fprintf(main, "  push_number(&s, %.*s);\n", (int) (j - i), code + i);
                    i = j - 1;
                    break;
                }
                fprintf(stderr, "Unknown character: %c with code %i\n", code[i], code[i]);
            }
        }
    }
}

#undef UC

int main() {
    size_t alloc = 10;
    char *code = malloc(alloc);
    if (code == NULL) {
        ERR("Out of memory!");
    }
    size_t pos = 0;

    int c;
    while ((c = getchar()) != EOF) {
        if (pos >= alloc) {
            alloc *= 2;
            char *new = realloc(code, alloc);
            if (new == NULL) {
                ERR("Out of memory!");
            }
            code = new;
        }
        code[pos++] = (char) c;
    }

    FILE *topf = tmpfile();
    FILE *bottomf = tmpfile();

    printf("#include \"runtime.h\"\n\n");
    printf("stack s;\n\n");
    compile(code, pos, bottomf, topf);

    rewind(topf);
    rewind(bottomf);

    while ((c = getc(topf)) != EOF) {
        putchar(c);
    }

    printf("int main() {\n  initrt();\n  sinit(&s);\n\n");
    while ((c = getc(bottomf)) != EOF) {
        putchar(c);
    }
    printf("\n  sdump(&s);\n  sfree(&s);\n  stoprt();\n}\n");

    fclose(topf);
    fclose(bottomf);

    free(code);
    return 0;
}
