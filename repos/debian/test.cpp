#include <algorithm>
#include <cmath>
#include <iostream>
#include <array>
#include <vector>
#include <thread>
#include <chrono>

float A = 0.0f, B = 0.0f, C = 0.0f;

constexpr int width = 144;
constexpr int height = 60;
constexpr char bgASCII = ' ';
constexpr float K1 = 40.0f;
constexpr float incrementSpeed = 0.6f;

float cube_width = 20.0f;
float horizontal_offset = -2 * cube_width;

std::array<float, width * height> z_buffer{};
std::array<char, width * height> buffer{};

float calculateX(const float& i, const float& j, const float& k) {
    return j * sin(A) * sin(B) * cos(C)
        - k * cos(A) * sin(B) * cos(C)
        + j * cos(A) * sin(C)
        + k * sin(A) * sin(C)
        + i * cos(B) * cos(C);
}

float calculateY(const float& i, const float& j, const float& k) {
    return j * cos(A) * cos(C)
        + k * sin(A) * cos(C)
        - j * sin(A) * sin(B) * sin(C)
        + k * cos(A) * sin(B) * sin(C)
        - i * cos(B) * sin(C);
}

float calculateZ(const float& i, const float& j, const float& k) {
    return k * cos(A) * cos(B)
        - j * sin(A) * cos(B)
        + i * sin(B);
}

void calculatePlane(const float& cube_x, const float& cube_y, const float& cube_z, const char& ch) {
    float x = calculateX(cube_x, cube_y, cube_z);
    float y = calculateY(cube_x, cube_y, cube_z);
    float z = calculateZ(cube_x, cube_y, cube_z) + 100.0f; // distanceFromCam

    float ooz = 1.0f / z;

    int xp = static_cast<int>(width / 2 + horizontal_offset + K1 * ooz * x * 2);
    int yp = static_cast<int>(height / 2 + K1 * ooz * y);

    int idx = xp + yp * width;
    if (idx >= 0 && idx < width * height) {
        if (ooz > z_buffer[idx]) {
            z_buffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main() {
    std::cout << "\x1b[2J";

    while (true) {
        std::fill(buffer.begin(), buffer.end(), bgASCII);
        std::fill(z_buffer.begin(), z_buffer.end(), 0);

        for (float cube_x = -cube_width; cube_x < cube_width; cube_x += incrementSpeed) {
            for (float cube_y = -cube_width; cube_y < cube_width; cube_y += incrementSpeed) {
                calculatePlane(cube_x, cube_y, -cube_width, '@');
                calculatePlane(cube_width, cube_y, cube_x, '$');
                calculatePlane(-cube_width, cube_y, -cube_x, '~');
                calculatePlane(-cube_x, cube_y, cube_width, '#');
                calculatePlane(cube_x, -cube_width, -cube_y, ';');
                calculatePlane(cube_x, cube_width, cube_y, '+');
            }
        }
        std::cout << "\x1b[H";
        for (int k = 0; k < width * height; ++k) {
            std::cout.put(k % width ? buffer[k] : '\n');
        }

        A += 0.05f;
        B += 0.05f;
        C += 0.01f;

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    return 0;
}
