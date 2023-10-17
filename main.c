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

char **declared;
size_t declared_len;
size_t declared_alloc_s;

void add_declared(char *name) {
    // checks if all characters are alphanumeric
    for (size_t i = 0; i < strlen(name); i++) {
        if (!isalnum(name[i])) {
            ERR("Function names must be alphanumeric!");
        }
    }

    if (declared_len >= declared_alloc_s) {
        declared_alloc_s *= 2;
        char **new = realloc(declared, declared_alloc_s * sizeof(char *));
        if (new == NULL) {
            ERR("Out of memory!");
        }
        declared = new;
    }
    declared[declared_len++] = name;
}

// returns the end position of the name or -1 if not found. only checks until the next non-alphanumeric character in the given name.
int is_declared(char *name) {
    for (size_t i = 0; i < declared_len; i++) {
        char *decl = declared[i];
        size_t j = 0;
        size_t ml = strlen(decl);
        bool match = true;
        for (j = 0; j < ml; j++) {
            if (j >= strlen(name)) {
                match = false;
                break;
            }
            if (!isalnum(name[j])) {
                break;
            }
            if (decl[j] != name[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return j;
        }
    }
    return -1;
}

void compile(char *code, size_t len, FILE *main, FILE *top) {
    for (size_t i = 0; i < len; i++) {
        char *curr = code + i;
        UC(curr, "⟼") { // named function declaration
            // example:   ⟼TestFunc (1 2 +)
            // becomes:   void f_TestFunc() { push_number(s, 1); push_number(s, 2); add(s); }

            // find function name
            size_t j = i + 1;
            while (j < len && code[j] != ' ' && code[j] != '\n' && code[j] != '(') {
                j++;
            }
            if (j == len) {
                ERR("Expected function name!");
            }
            char *name = malloc(j - i + 1);
            memset(name, 0, j - i + 1);
            if (name == NULL) {
                ERR("Out of memory!");
            }
            strncpy(name, code + i + 1, j - i - 1);
            name[j - i] = '\0';
            if (is_declared(name) != -1) {
                ERR("Function already declared!");
            }
            add_declared(name);
            i = j;

            // find function body
            // 1. find opening parenthesis
            while (i < len && code[i] != '(') {
                i++;
            }
            if (i == len) {
                ERR("Expected function body!");
            }
            i++;

            // 2. find closing parenthesis
            size_t indent = 0;
            j = i;
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
            char *body = malloc(j - i + 1);
            if (body == NULL) {
                ERR("Out of memory!");
            }
            strncpy(body, code + i, j - i);
            body[j - i] = '\0';

            // 3. compile function body
            FILE *f = tmpfile();
            compile(body, j - i, f, top);
            i = j;
            fprintf(top, "void f_%s(stack *s) {\n", name);
            rewind(f);
            int c;
            while ((c = getc(f)) != EOF) {
                putc(c, top);
            }
            fclose(f);
            fprintf(top, "}\n");
            free(body);
            continue;
        }
        UC(curr, "∵") {
            fprintf(main, "  each(s);\n");
            continue;
        }
        UC(curr, "×") {
            fprintf(main, "  mul(s);\n");
            continue;
        }
        UC(curr, "÷") {
            fprintf(main, "  div_op(s);\n");
            continue;
        }
        UC(curr, "↙") {
            fprintf(main, "  take(s);\n");
            continue;
        }
        UC(curr, "↘") {
            fprintf(main, "  drop(s);\n");
            continue;
        }
        UC(curr, "↻") {
            fprintf(main, "  rot(s);\n");
            continue;
        }
        UC(curr, "⇌") {
            fprintf(main, "  rev(s);\n");
            continue;
        }
        UC(curr, "♭") {
            fprintf(main, "  deshape(s);\n");
            continue;
        }
        UC(curr, "≅") {
            fprintf(main, "  match(s);\n");
            continue;
        }
        UC(curr, "⊂") {
            fprintf(main, "  join(s);\n");
            continue;
        }
        UC(curr, "⊏") {
            fprintf(main, "  select_op(s);\n");
            continue;
        }
        UC(curr, "⊡") {
            fprintf(main, "  pick(s);\n");
            continue;
        }
        UC(curr, "⊗") {
            fprintf(main, "  indexof(s);\n");
            continue;
        }
        UC(curr, "∊") {
            fprintf(main, "  member(s);\n");
            continue;
        }
        UC(curr, "∘") {
            fprintf(main, "  push_addr(s, f_spec_noop);\n");
            continue;
        }
        UC(curr, "△") {
            fprintf(main, "  shape(s);\n");
            continue;
        }
        UC(curr, "⍥") {
            fprintf(main, "  repeat(s);\n");
            continue;
        }
        UC(curr, "↯") {
            fprintf(main, "  reshape(s);\n");
            continue;
        }
        UC(curr, "∅") { // empty set
            fprintf(main, "  new_array(s);\n  end_array(s);\n");
            continue;
        }
        UC(curr, "⚙") {
            fprintf(main, "  type(s);\n");
            continue;
        }
        UC(curr, "⧻") {
            fprintf(main, "  len(s);\n");
            continue;
        }
        UC(curr, "⊢") {
            fprintf(main, "  first(s);\n");
            continue;
        }
        UC(curr, "⇡") {
            fprintf(main, "  range(s);\n");
            continue;
        }
        UC(curr, "⊚") {
            fprintf(main, "  where(s);\n");
            continue;
        }
        UC(curr, "⊝") {
            fprintf(main, "  deduplicate(s);\n");
            continue;
        }
        UC(curr, "⊔") {
            fprintf(main, "  unbox(s);\n");
            continue;
        }
        UC(curr, "□") {
            fprintf(main, "  box(s);\n");
            continue;
        }
        UC(curr, "⋯") {
            fprintf(main, "  enumerate(s);\n");
            continue;
        }
        UC(curr, "⍉") {
            fprintf(main, "  transpose(s);\n");
            continue;
        }
        UC(curr, "⍏") {
            fprintf(main, "  sort_asc(s);\n");
            continue;
        }
        UC(curr, "⍖") {
            fprintf(main, "  sort_desc(s);\n");
            continue;
        }
        UC(curr, "~") {
            fprintf(main, "  trace(s);\n");
            continue;
        }
        UC(curr, "π") {
            fprintf(main, "  do_pi(s);\n");
            continue;
        }
        UC(curr, "η") {
            fprintf(main, "  do_eta(s);\n");
            continue;
        }
        UC(curr, "τ") {
            fprintf(main, "  do_tau(s);\n");
            continue;
        }
        UC(curr, "⚂") {
            fprintf(main, "  do_rand(s);\n");
            continue;
        }
        UC(curr, "∞") {
            fprintf(main, "  do_inf(s);\n");
            continue;
        }
        UC(curr, "∩") {
            fprintf(main, "  both(s);\n");
            continue;
        }
        UC(curr, "⊙") {
            fprintf(main, "  dip(s);\n");
            continue;
        }
        UC(curr, "▽") {
            fprintf(main, "  keep(s);\n");
            continue;
        }
        UC(curr, "⌕") {
            fprintf(main, "  find(s);\n");
            continue;
        }
        UC(curr, "⊟") {
            fprintf(main, "  couple(s);\n");
            continue;
        }
        UC(curr, "∺") {
            fprintf(main, "  distribute(s);\n");
            continue;
        }
        UC(curr, "∧") {
            fprintf(main, "  fold(s);\n");
            continue;
        }
        UC(curr, "⊤") {
            fprintf(main, "  true_op(s);\n");
            continue;
        }
        UC(curr, "⊥") {
            fprintf(main, "  false_op(s);\n");
            continue;
        }
        UC(curr, "♮") {
            fprintf(main, "  nan_op(s);\n");
            continue;
        }
        UC(curr, "¬") {
            fprintf(main, "  not_op(s);\n");
            continue;
        }
        UC(curr, "¯") {
            fprintf(main, "  negate(s);\n");
            continue;
        }
        UC(curr, "=") {
            fprintf(main, "  eq(s);\n");
            continue;
        }
        UC(curr, "≠") {
            fprintf(main, "  neq(s);\n");
            continue;
        }
        UC(curr, "<") {
            fprintf(main, "  lt(s);\n");
            continue;
        }
        UC(curr, "≤") {
            fprintf(main, "  lte(s);\n");
            continue;
        }
        UC(curr, ">") {
            fprintf(main, "  gt(s);\n");
            continue;
        }
        UC(curr, "≥") {
            fprintf(main, "  gte(s);\n");
            continue;
        }
        UC(curr, "⍤") {
            fprintf(main, "  assert(s);\n");
            continue;
        }
        UC(curr, "↥") {
            fprintf(main, "  max_op(s);\n");
            continue;
        }
        UC(curr, "↧") {
            fprintf(main, "  min_op(s);\n");
            continue;
        }
        UC(curr, "◫") {
            fprintf(main, "  split_op(s);\n");
            continue;
        }
        UC(curr, "⊞") {
            fprintf(main, "  table(s);\n");
            continue;
        }
        UC(curr, "⊕") {
            fprintf(main, "  group(s);\n");
            continue;
        }
        UC(curr, "⊓") {
            fprintf(main, "  bracket(s);\n");
            continue;
        }
        UC(curr, "≡") {
            fprintf(main, "  dearray(s);\n");
            continue;
        }
        UC(curr, "⬡") {
            fprintf(main, "  fragment(s);\n");
            continue;
        }
        UC(curr, "⇱") {
            fprintf(main, "  write_file(s);\n");
            continue;
        }
        UC(curr, "⇲") {
            fprintf(main, "  read_file(s);\n");
            continue;
        }
        UC(curr, "nl") { // TODO: find character for this
            fprintf(main, "  push_char(s, '\\n');\n");
            continue;
        }
        UC(curr, "ℤ") {
            fprintf(main, "  cast_integer(s);\n");
            continue;
        }
        UC(curr, "ℝ") {
            fprintf(main, "  cast_real(s);\n");
            continue;
        }
        UC(curr, "𝕐") {
            fprintf(main, "  cast_string(s);\n");
            continue;
        }
        UC(curr, "⑀") {
            fprintf(main, "  makefract(s);\n");
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
                fprintf(top, "void f_%i(stack *s) {\n", anon);
                rewind(f);
                int c;
                while ((c = getc(f)) != EOF) {
                    putc(c, top);
                }
                fclose(f);
                fprintf(top, "}\n");
                fprintf(main, "  push_addr(s, f_%i);\n", anon);
                i = j;
                break;
            }
            case '"': {
                // parse string
                // handle escape sequences
                size_t j = i + 1;
                while (j < len && code[j] != '"') {
                    if (code[j] == '\\') {
                        j++;
                    }
                    j++;
                }
                if (j == len) {
                    ERR("Unmatched quote!");
                }
                fprintf(main, "  push_string(s, \"%.*s\");\n", (int) (j - i - 1), code + i + 1);
                i = j;
                break;
            }
            case '@': {
                char c = code[++i];
                fprintf(main, "  push_char(s, '%c');", c);
                break;
            }
            case '[': {
                fprintf(main, "  new_array(s);\n");
                break;
            }
            case ']': {
                fprintf(main, "  end_array(s);\n");
                break;
            }
            case '!': {
                fprintf(main, "  call(s);\n");
                break;
            }
            case ';': {
                fprintf(main, "  pop(s);\n");
                break;
            }
            case '.': {
                fprintf(main, "  dup(s);\n");
                break;
            }
            case ':':
                fprintf(main, "  swap(s);\n");
                break;
            case ',':
                fprintf(main, "  over(s);\n");
                break;
            case '+':
                fprintf(main, "  add(s);\n");
                break;
            case '-':
                fprintf(main, "  sub(s);\n");
                break;
            case '/':
                fprintf(main, "  reduce(s);\n");
                break;
            case '\\':
                fprintf(main, "  scan(s);\n");
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
                    fprintf(main, "  push_number(s, %.*s);\n", (int) (j - i), code + i);
                    i = j - 1;
                    break;
                }
                int decl = is_declared(code + i);
                if (decl != -1) {
                    fprintf(main, "  push_addr(s, f_%.*s);\n", decl, code + i);
                    i += decl - 1;
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

    declared_alloc_s = 10;
    declared = malloc(declared_alloc_s);
    if (declared == NULL) {
        ERR("Out of memory!");
    }
    declared_len = 0;

    FILE *topf = tmpfile();
    FILE *bottomf = tmpfile();

    printf("#include \"runtime/main.h\"\n#include \"runtime/operators.h\"\n\n");
    compile(code, pos, bottomf, topf);

    rewind(topf);
    rewind(bottomf);

    while ((c = getc(topf)) != EOF) {
        putchar(c);
    }

    printf("void f_spec_noop(stack *s) {\n  return;\n}\n\n");
    printf("int main() {\n  stack st;\n  stack *s = &st;\n  initrt();\n  sinit(s);\n\n");
    while ((c = getc(bottomf)) != EOF) {
        putchar(c);
    }
    printf("\n  sdump(s);\n  sfree(s);\n  stoprt();\n}\n");

    fclose(topf);
    fclose(bottomf);

    free(code);
    free(declared);
    return 0;
}
