#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINES 20
#define MAX_COLS 80
#define COLOR_DEFAULT 1
#define COLOR_KEYWORD 2
#define COLOR_STRING 3
#define COLOR_COMMENT 4

char text[MAX_LINES][MAX_COLS];
int num_lines = 5;  // 默认有5行
int cursor_row = 0, cursor_col = 0;
char *filename = NULL;
int file_modified = 0;  // 标记文件是否被修改

void init_screen() {
    initscr();             // 初始化ncurses
    cbreak();              // 禁用行缓冲
    noecho();              // 不显示用户输入
    curs_set(1);           // 显示光标
    keypad(stdscr, TRUE);  // 启用键盘映射（例如箭头键）

    // 初始化颜色
    if (has_colors()) {
        start_color();
        init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_KEYWORD, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_STRING, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_COMMENT, COLOR_YELLOW, COLOR_BLACK);
    }
}

void cleanup() {
    endwin();              // 结束ncurses
}

// 绘制当前文本内容，并为关键字和字符串添加颜色
void draw_text() {
    clear();  // 清除屏幕顶部内容

    // 打印文本内容
    for (int i = 0; i < num_lines; i++) {
        int j = 0;
        while (text[i][j] != '\0' && j < MAX_COLS) {
            char ch = text[i][j];

            // 模拟简单的语法高亮
            if (ch == '#') {
                attron(COLOR_PAIR(COLOR_COMMENT));
                mvprintw(i, j, "%c", ch);
                attroff(COLOR_PAIR(COLOR_COMMENT));
            }
            else if (ch == ' ' || ch == '\t') {
                attron(COLOR_PAIR(COLOR_DEFAULT));
                mvprintw(i, j, "%c", ch);
                attroff(COLOR_PAIR(COLOR_DEFAULT));
            } else {
                attron(COLOR_PAIR(COLOR_DEFAULT));
                mvprintw(i, j, "%c", ch);
                attroff(COLOR_PAIR(COLOR_DEFAULT));
            }

            j++;
        }
    }

    // 打印光标位置
    move(cursor_row, cursor_col);  // 移动光标到当前位置

    // 打印提示信息（底部提示信息不被覆盖）
    mvprintw(LINES - 1, 0, "Use arrow keys to move, Backspace to delete, ESC to exit. File: %s", filename ? filename : "Untitled");

    refresh();  // 刷新屏幕
}

// 插入字符到当前光标位置
void insert_char(int ch) {
    if (cursor_col < MAX_COLS - 1) {
        // 插入字符
        text[cursor_row][cursor_col] = ch;
        cursor_col++;
        file_modified = 1;  // 文件被修改
    }
}

// 删除字符
void delete_char() {
    if (cursor_col > 0) {
        cursor_col--;
        text[cursor_row][cursor_col] = ' ';  // 删除字符，替换为空格
        file_modified = 1;  // 文件被修改
    }
}

// 处理键盘输入
void handle_input(int ch) {
    switch(ch) {
        case KEY_UP:
            if (cursor_row > 0) cursor_row--;
            break;
        case KEY_DOWN:
            if (cursor_row < num_lines - 1) cursor_row++;
            break;
        case KEY_LEFT:
            if (cursor_col > 0) cursor_col--;
            break;
        case KEY_RIGHT:
            if (cursor_col < MAX_COLS - 1 && text[cursor_row][cursor_col] != '\0') cursor_col++;
            break;
        case 127:  // 退格键
        case KEY_BACKSPACE:  // 退格键的另一种表示
            delete_char();
            break;
        case '\n':  // 回车键
            if (cursor_row < MAX_LINES - 1) {
                cursor_row++;
                cursor_col = 0;
                // 如果光标到达行末，自动增加新的一行
                if (cursor_row >= num_lines) num_lines++;
            }
            break;
        default:
            if (ch >= 32 && ch <= 126) {  // 如果是可打印字符
                insert_char(ch);
            }
            break;
    }
}

// 保存文件
void save_file() {
    if (filename == NULL) {
        // 如果没有文件名，要求用户输入文件名
        echo();  // 允许显示用户输入
        mvprintw(LINES - 2, 0, "Enter filename to save: ");
        char temp_filename[256];
        getstr(temp_filename);
        noecho();  // 隐藏用户输入
        filename = strdup(temp_filename);
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to save file");
        return;
    }

    for (int i = 0; i < num_lines; i++) {
        fprintf(file, "%s\n", text[i]);
    }
    fclose(file);
    file_modified = 0;  // 保存后标记为未修改
}

// 加载文件
void load_file() {
    if (filename == NULL) {
        return;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    for (int i = 0; i < MAX_LINES && fgets(text[i], MAX_COLS, file); i++) {
        text[i][strcspn(text[i], "\n")] = '\0';  // 去掉每行的换行符
    }

    fclose(file);
}

// 退出时提示保存
int ask_save() {
    if (file_modified) {
        // 提示用户保存文件
        int ch = 0;
        while (ch != 'y' && ch != 'n') {
            mvprintw(LINES - 2, 0, "You have unsaved changes. Save changes? (y/n): ");
            ch = getch();
            refresh();
        }

        if (ch == 'y') {
            save_file();
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int ch;

    // 如果传入文件名作为参数，则使用它
    if (argc > 1) {
        filename = argv[1];
    }

    // 初始化屏幕和文本缓冲区
    init_screen();
    memset(text, ' ', sizeof(text));  // 初始化文本缓冲区为空格

    // 尝试加载文件内容
    load_file();

    while ((ch = getch()) != 27) {  // 按Esc键退出
        handle_input(ch);  // 处理用户输入
        draw_text();       // 绘制文本
    }

    // 如果文件有修改，提示保存
    if (ask_save() == 0) {
        cleanup();  // 清理和退出
    }

    return 0;
}







