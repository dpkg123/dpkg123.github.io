#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define HASH_LENGTH 64
#define PROGRESS_BAR_WIDTH 50
#define TERMINAL_WIDTH 80
#define HASH_UPDATE_COUNT 1000  // Number of hash updates per progress increment

// Function to clear screen - cross platform
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to get current time string
void get_current_time(char *time_str) {
    time_t now;
    struct tm *tm_info;
    
    time(&now);
    tm_info = gmtime(&now);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S UTC", tm_info);
}

// Function to generate random hex hash
void generate_random_hash(char *hash, int length) {
    const char hex_chars[] = "0123456789abcdef";
    for (int i = 0; i < length; i++) {
        hash[i] = hex_chars[rand() % 16];
    }
    hash[length] = '\0';
}

// Function to display progress bar
void display_progress_bar(int percent) {
    printf("\rCompiling Progress: [");
    int position = PROGRESS_BAR_WIDTH * percent / 100;
    
    for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
        if (i < position) printf("█");
        else printf(" ");
    }
    printf("] %3d%%", percent);
    fflush(stdout);
}

// Function to center text
void print_centered(const char *text) {
    int padding = (TERMINAL_WIDTH - strlen(text)) / 2;
    printf("%*s%s\n", padding, "", text);
}

int main() {
    char hash[HASH_LENGTH + 1];
    char time_str[26];
    int progress = 0;
    int hash_update_counter = 0;
    
    // Initialize random seed
    srand(time(NULL));
    
    // Clear screen once at start
    clear_screen();
    printf("Compiling shaders...\n");
    while (progress <= 100) {
        // Move cursor to top
        printf("\033[H");
        
        // Display current time
        get_current_time(time_str);
        printf("\n");
        print_centered(time_str);
        
        // Generate and display new hash
        generate_random_hash(hash, HASH_LENGTH);
        printf("\n");
        print_centered("/opt/shaders/",hash);
        printf("\n\n"); // Extra space before progress bar
        
        // Update and display progress bar
        display_progress_bar(progress);
        
        // Random delay between 0.2s and 1s
        int delay_ms = (rand() % 980) + 20; // 200ms to 1000ms
        usleep(delay_ms * 100);  // Convert to microseconds
        
        // Update hash several times before updating progress
        hash_update_counter++;
        if (hash_update_counter >= HASH_UPDATE_COUNT) {
            hash_update_counter = 0;
            progress++;
        }
    }
    
    // Final newline and small delay before exit
    printf("\n\nProcess completed successfully!\n");
    sleep(1);
    
    return 0;
}
