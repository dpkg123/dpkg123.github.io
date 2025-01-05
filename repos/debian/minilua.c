#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char *condition;
    const char *body;
} WhileBlock;

#define MAX_NESTING 10
WhileBlock loop_stack[MAX_NESTING];
int loop_stack_top = -1;

#define MAX_VARS 128

typedef struct {
    char name[32];
    double value;
} Variable;

Variable variables[MAX_VARS];
int var_count = 0;

// ---------------- 工具函数 ----------------

int find_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_variable(const char *name, double value) {
    int idx = find_variable(name);
    if (idx == -1) {
        if (var_count >= MAX_VARS) {
            fprintf(stderr, "Error: Too many variables.\n");
            return;
        }
        strcpy(variables[var_count].name, name);
        variables[var_count].value = value;
        var_count++;
    } else {
        variables[idx].value = value;
    }
}

double get_variable(const char *name) {
    int idx = find_variable(name);
    if (idx == -1) {
        fprintf(stderr, "Error: Undefined variable '%s'.\n", name);
        return 1;
    }
    return variables[idx].value;
}

const char *skip_whitespace(const char *src) {
    while (isspace(*src)) src++;
    return src;
}

// ---------------- 表达式解析 ----------------

double parse_expression(const char **src);
void parse_code_block(const char **src);

double parse_factor(const char **src) {
    *src = skip_whitespace(*src);
    if (isdigit(**src)) {
        char *end;
        double value = strtod(*src, &end);
        *src = end;
        return value;
    } else if (isalpha(**src)) {
        char name[32];
        int len = 0;
        while (isalnum(**src) || **src == '_') {
            name[len++] = *(*src)++;
        }
        name[len] = '\0';
        return get_variable(name);
    } else if (**src == '(') {
        (*src)++;
        double value = parse_expression(src);
        if (**src != ')') {
            fprintf(stderr, "Error: Expected ')'.\n");
            return 1;
        }
        (*src)++;
        return value;
    } else {
        fprintf(stderr, "Error: Unexpected token '%c'.\n", **src);
        return 1;
    }
}

double parse_term(const char **src) {
    double value = parse_factor(src);
    while (1) {
        *src = skip_whitespace(*src);
        if (**src == '*') {
            (*src)++;
            value *= parse_factor(src);
        } else if (**src == '/') {
            (*src)++;
            value /= parse_factor(src);
        } else {
            break;
        }
    }
    return value;
}

double parse_expression(const char **src) {
    double value = parse_term(src);
    while (1) {
        *src = skip_whitespace(*src);
        if (**src == '+') {
            (*src)++;
            value += parse_term(src);
        } else if (**src == '-') {
            (*src)++;
            value -= parse_term(src);
        } else {
            break;
        }
    }
    return value;
}

// ---------------- 条件解析 ----------------

int parse_condition(const char **src) {
    double left = parse_expression(src);
    *src = skip_whitespace(*src);
    if (strncmp(*src, "==", 2) == 0) {
        *src += 2;
        double right = parse_expression(src);
        return left == right;
    } else if (strncmp(*src, "!=", 2) == 0) {
        *src += 2;
        double right = parse_expression(src);
        return left != right;
    } else if (**src == '>') {
        (*src)++;
        if (**src == '=') {
            (*src)++;
            double right = parse_expression(src);
            return left >= right;
        }
        double right = parse_expression(src);
        return left > right;
    } else if (**src == '<') {
        (*src)++;
        if (**src == '=') {
            (*src)++;
            double right = parse_expression(src);
            return left <= right;
        }
        double right = parse_expression(src);
        return left < right;
    } else {
        fprintf(stderr, "Error: Invalid condition operator.\n");
        return 1;
    }
}

// ---------------- 代码块解析 ----------------

void parse_line(const char **src);

void parse_code_block(const char **src) {
    *src = skip_whitespace(*src);
    if (**src == '{') {
        (*src)++;
        while (**src != '}') {
            parse_line(src);
            *src = skip_whitespace(*src);
        }
        (*src)++;
    } else {
        parse_line(src);
    }
}

// ---------------- 循环和输入输出 ----------------

void parse_while(const char **src) {
    *src = skip_whitespace(*src);
    if (**src != '(') {
        fprintf(stderr, "Error: Expected '(' after 'while'.\n");
        return;
    }
    (*src)++;
    const char *condition_start = *src;

    while (1) {
        const char *condition_copy = condition_start;
        if (!parse_condition(&condition_copy)) {
            break;
        }
        parse_code_block(src);
        *src = skip_whitespace(condition_start);
    }

    *src = skip_whitespace(*src);
    if (**src == ')') {
        (*src)++;
    }
}

void parse_print(const char **src) {
    *src = skip_whitespace(*src);
    if (**src == '(') {
        (*src)++;
        double value = parse_expression(src);
        printf("%g\n", value);
        *src = skip_whitespace(*src);
        if (**src == ')') {
            (*src)++;
        } else {
            fprintf(stderr, "Error: Expected ')'.\n");
            return;
        }
    } else {
        fprintf(stderr, "Error: Expected '(' after 'print'.\n");
        return;
    }
}

void parse_read(const char **src) {
    char name[32];
    int len = 0;
    *src = skip_whitespace(*src);
    while (isalnum(**src) || **src == '_') {
        name[len++] = *(*src)++;
    }
    name[len] = '\0';

    double value;
    printf("Enter value for %s: ", name);
    if (scanf("%lf", &value) != 1) {
        fprintf(stderr, "Error: Invalid input.\n");
        return;
    }
    set_variable(name, value);
}

void parse_line(const char **src) {
    skip_whitespace(*src);

    // 检查 while 关键字
    if (strncmp(*src, "while", 5) == 0) {
        *src += 5;
        skip_whitespace(*src);

        // 提取条件
        if (**src != '(') {
            printf("Error: Expected '(' after 'while'.\n");
            return;
        }
        (*src)++;  // 跳过 '('
        const char *condition_start = *src;
        while (**src && **src != ')') (*src)++;
        if (**src != ')') {
            printf("Error: Unmatched '(' in while condition.\n");
            return;
        }
        char condition[256];
        strncpy(condition, condition_start, *src - condition_start);
        condition[*src - condition_start] = '\0';
        (*src)++;  // 跳过 ')'

        // 提取循环体
        skip_whitespace(src);
        if (**src != '{') {
            printf("Error: Expected '{' after while condition.\n");
            return;
        }
        (*src)++;  // 跳过 '{'
        const char *body_start = *src;
        int braces = 1;
        while (**src && braces > 0) {
            if (**src == '{') braces++;
            if (**src == '}') braces--;
            (*src)++;
        }
        if (braces != 0) {
            printf("Error: Unmatched '{' in while body.\n");
            return;
        }
        char body[256];
        strncpy(body, body_start, *src - body_start - 1);
        body[*src - body_start - 1] = '\0';

        // 将当前循环压入栈
        if (loop_stack_top + 1 >= MAX_NESTING) {
            printf("Error: Too many nested loops.\n");
            return;
        }
        loop_stack[++loop_stack_top].condition = strdup(condition);
        loop_stack[loop_stack_top].body = strdup(body);
        return;
    }
}

void execute_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        const char *src = line;
        parse_line(&src);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // 如果提供了文件名参数，从文件执行代码
        execute_file(argv[1]);
        return 0;
    }
    char line[256];
    printf("Lua Interpreter with Loops and I/O (Subset)\n");
    printf("Enter 'exit' to quit.\n");

    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        if (strncmp(line, "exit", 4) == 0) break;
        const char *src = line;
        parse_line(&src);
    }

    return 0;
}
