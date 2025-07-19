#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#include "Offsets.h"
#include "memory.h"
#include "config.h"

// Shit
void MouseLeftClick() {
    INPUT input[2] = {};

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN; // Click the left mouse button

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP; // Release the left mouse button

    SendInput(2, input, sizeof(INPUT));
}

// Change CMD Color
void SetCmdColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

// Main function entry point 
int main() {
    SetCmdColor(15, 3);

    std::wcout << L"by weapon_smg1" << std::endl;
    std::wcout << L"Toggle Key:Insert" << std::endl;


    Memory memory;
    bool process = false;
    bool dll = false;

    // Found Process 
    while (!process) {
        if (memory.AttachProcess(L"gmod.exe")) {
            process = true;
            std::wcout << L"Gmod found PID: " << memory.pid_ << std::endl;
        }
        else {
            std::wcout << L"gmod.exe not found waiting..." << std::endl;
            Sleep(1000);
        }
    }

    // Found DLL
    while (!dll) {
        memory.base_client_ = memory.GetModule(L"client.dll");
        if (memory.base_client_.found) {
            dll = true;
            std::wcout << L"client.dll found" << std::endl;
        }
        else {
            std::wcout << L"client.dll not found waiting..." << std::endl;
            Sleep(1000);
        }
    }

    while (true) {

        // Enable & Disable TriggerBot
        if (GetAsyncKeyState(VK_INSERT) & 1) {  
            triggerbot = !triggerbot;
            std::wcout << L"Triggerbot Toggle" << std::endl;
            Sleep(200);
        }

        if (triggerbot) {
            // Read LocalPlayer
            uintptr_t localPlayer = memory.read<uintptr_t>(memory.base_client_.base + Offsets::LocalPlayer);
            if (!localPlayer) {
                Sleep(10);
                continue;
            }

            // Read Crosshair
            int crosshair = memory.read<int>(localPlayer + Offsets::Crosshair);

            // If something gets in the sights the shot
            if (crosshair > 0 && crosshair <= 128) {
                MouseLeftClick();
                Sleep(50);
            }
        }
        Sleep(1); 
    }
}