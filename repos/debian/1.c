#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define TERMINAL_WIDTH 80
#define PROGRESS_BAR_WIDTH 50
#define FILE_NAME_LENGTH 12
#define MAX_FILES 100

// 着色器模板
const char* SHADER_TEMPLATE = 
"#version 330 core\n\n"
"uniform float iTime;\n"
"uniform vec2 iResolution;\n\n"
"void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
"{\n"
"    vec2 uv = fragCoord/iResolution.xy;\n"
"    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));\n"
"    fragColor = vec4(col,1.0);\n"
"}\n\n"
"void main()\n"
"{\n"
"    mainImage(gl_FragColor, gl_FragCoord.xy);\n"
"}\n";

// 结构体用于存储创建的文件信息
typedef struct {
    char filename[256];
    time_t creation_time;
} FileInfo;

// 函数声明
void clear_screen();
void get_current_time(char *time_str);
void generate_random_filename(char *filename);
void print_centered(const char *text);
void display_progress_bar(int percent);
void create_shader_file(const char *filename);
void create_output_directory();

// 清屏函数
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 获取当前时间
void get_current_time(char *time_str) {
    time_t now;
    struct tm *tm_info;
    time(&now);
    tm_info = gmtime(&now);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S UTC", tm_info);
}

// 生成随机文件名
void generate_random_filename(char *filename) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    char temp[FILE_NAME_LENGTH + 1];
    
    for (int i = 0; i < FILE_NAME_LENGTH; i++) {
        temp[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    temp[FILE_NAME_LENGTH] = '\0';
    
    sprintf(filename, "shaders/%s.glsl", temp);
}

// 居中打印文本
void print_centered(const char *text) {
    int padding = (TERMINAL_WIDTH - strlen(text)) / 2;
    printf("%*s%s\n", padding, "", text);
}

// 显示进度条
void display_progress_bar(int percent) {
    printf("\rProgress: [");
    int position = PROGRESS_BAR_WIDTH * percent / 100;
    
    for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
        if (i < position) printf("█");
        else printf(" ");
    }
    printf("] %3d%%", percent);
    fflush(stdout);
}

// 创建着色器文件
void create_shader_file(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "%s", SHADER_TEMPLATE);
        fclose(fp);
    }
}

// 创建输出目录
void create_output_directory() {
#ifdef _WIN32
    _mkdir("shaders");
#else
    mkdir("shaders", 0777);
#endif
}

int main() {
    char time_str[26];
    char current_file[256];
    FileInfo files[MAX_FILES];
    int file_count = 0;
    int progress = 0;
    
    // 初始化随机数生成器
    srand(time(NULL));
    
    // 创建输出目录
    create_output_directory();
    
    // 清屏
    clear_screen();
    
    while (progress <= 100) {
        // 移动光标到顶部
        printf("\033[H");
        
        // 显示当前时间
        get_current_time(time_str);
        printf("\n");
        print_centered("Shader Generator");
        print_centered("----------------");
        printf("\n");
        print_centered(time_str);
        printf("\n\n");
        
        // 如果需要生成新文件
        if (rand() % 3 == 0 && progress < 100) {
            generate_random_filename(current_file);
            create_shader_file(current_file);
            
            // 保存文件信息
            if (file_count < MAX_FILES) {
                strcpy(files[file_count].filename, current_file);
                files[file_count].creation_time = time(NULL);
                file_count++;
            }
        }
        
        // 显示最近创建的文件
        printf("\nRecently created files:\n");
        for (int i = (file_count > 5 ? file_count - 5 : 0); i < file_count; i++) {
            struct tm *tm_info = gmtime(&files[i].creation_time);
            char file_time[20];
            strftime(file_time, 20, "%H:%M:%S", tm_info);
            printf("  [%s] %s\n", file_time, files[i].filename);
        }
        
        printf("\n\n");
        display_progress_bar(progress);
        
        // 延迟和进度更新
        usleep((rand() % 800 + 200) * 1000); // 200-1000ms delay
        progress += (rand() % 2 == 0) ? 1 : 0;
    }
    
    // 完成消息
    printf("\n\nGeneration completed! Created %d shader files.\n", file_count);
    printf("Files are stored in the 'shaders' directory.\n");
    sleep(2);
    
    return 0;
}
