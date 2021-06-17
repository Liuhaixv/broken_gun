#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <Mmsystem.h>
#include "INIReader.h"
#pragma comment(lib,"winmm.lib")

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define _CRT_SECURE_NO_WARNINGS

static int toggle_key = 0x50;
static bool enabled = false;
static int x = 0;
static int y = 0;
static int delay = 0;
static int debugMode = 0;


void playSound(bool on) {
    if (on) {
        PlaySound(TEXT("sound/on.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else {
        PlaySound(TEXT("sound/off.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
}

bool check(char c) {//检测某个按键是否按下
    return KEY_DOWN(c);
}


void move_mouse(int dx, int dy) {
    mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}

void checkHotkey() {
    static bool pressed = false;
    if (check(toggle_key)) {
        if (pressed == false) {
            pressed = true;
            enabled = !enabled;
            playSound(enabled);
            Sleep(50);
        }
    }
    else {
        pressed = false;
    }
}

void random_shake(int x, int y) {
    static int movedTimes = 0;
    int down = 0;
    bool shakeEnabled = true;

    int dx = 0;
    int dy = 0;
    int sumX = 0;
    int sumY = 0;

    while (1) {
        checkHotkey();
        if (enabled) {
            if (check(WM_LBUTTONDOWN)) {
                down = 1;
                if (shakeEnabled) {
                    dx = rand() % (1 + rand() % (x + 1));
                    dy = rand() % (1 + rand() % (y + 1));
                    dx *= (sumX >= 0 ? -1 : 1);
                    dy *= (sumY >= 0 ? -1 : 1);
                    sumX += dx;
                    sumY += dy;
                    move_mouse(dx, dy);
                    if (debugMode) {
                        std::cout << "{dx=" << dx << "," << "dy=" << dy << ",sumX=" << sumX << ",sumY=" << sumY << "}" << ",times=" << ++movedTimes << "}" << std::endl;
                    }
                    dx += 0;
                    Sleep(delay);
                }
            }
            else {
                down = 0;
                shakeEnabled = true;
            }
        }
    }
}

/*
* 从配置文件读取参数
*/
bool load_from_ini() {
    INIReader reader("config.ini");
    //抖枪开关
    toggle_key = reader.GetInteger("Binding", "ToggleKey", 80);

    //抖动系数x
    x = reader.GetInteger("ShakeLevel", "X", 200);
    //抖动系数y
    y = reader.GetInteger("ShakeLevel", "Y", 200);
    //抖动延迟,毫秒计
    delay = reader.GetInteger("Delay", "Milliseconds", 10);
    debugMode = reader.GetInteger("Debug", "DebugMode", 0);
    return 1;
}

void print_parameters() {
    std::cout << "ShakeLevel:{X=" << x << ",Y=" << y << "}" << std::endl;
    std::cout << "Delay:{" << delay << "ms}" << std::endl;
}

int main(int argc, char* argv[]) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY |
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    if (load_from_ini()) {
        std::cout << "配置文件读取完毕,按下按键\"" << (char)MapVirtualKey(toggle_key,2)<< "\"启用抖枪" <<std::endl;
    }
    //读参
    {
        if (argc == 2) {
            try {
                sscanf_s(argv[0], "%d", &x);
                sscanf_s(argv[1], "%d", &y);
            }
            catch (...) {}
        }
        if (argc == 3) {
            try {
                sscanf_s(argv[0], "%d", &x);
                sscanf_s(argv[1], "%d", &y);
                sscanf_s(argv[2], "%d", &delay);
            }
            catch (...) {}
        }
    }

    print_parameters();
    random_shake(x, y);
    return 0;
}