#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

struct PageTableEntry {
    int frameNumber;
    bool valid;
};

class VirtualMemoryManager {
public:
    VirtualMemoryManager(int numPages, int numFrames)
        : pageTable(numPages), numFrames(numFrames) {
        if (numPages <= 0 || numFrames <= 0) {
            throw std::invalid_argument("Number of pages and frames must be positive.");
        }
    }

    std::string accessPage(int virtualPage) {
        if (virtualPage < 0 || virtualPage >= pageTable.size()) {
            throw std::out_of_range("Invalid virtual page number.");
        }
        std::ostringstream oss;
        if (pageTable[virtualPage].valid) {
            oss << "Page Hit: Page " << virtualPage << " is in frame " << pageTable[virtualPage].frameNumber;
        } else {
            // Simulate fetching from memory
            oss << "Page Fault: Page " << virtualPage << " not in memory. Fetching from memory...";
            pageTable[virtualPage].valid = true;
            pageTable[virtualPage].frameNumber = allocateFrame();
        }
        return oss.str();
    }

private:
    std::vector<PageTableEntry> pageTable;
    int numFrames;

int allocateFrame() {
    static int nextFrame = 0;
    int frameToAllocate = nextFrame;
    nextFrame = (nextFrame + 1) % numFrames;
    return frameToAllocate;
}


};

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

char szClassName[] = "VirtualMemoryManagerClass";
VirtualMemoryManager* vmm = NULL;
HWND hwndNumPages, hwndNumFrames, hwndVirtualPage, hwndOutput;

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);

    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wincl)) return 0;

    hwnd = CreateWindowEx(
        0,
        szClassName,
        "Virtual Memory Manager",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        400,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    if (!hwnd) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    try {
        switch (message) {
        case WM_CREATE:
            CreateWindow("BUTTON", "Initialize Virtual Memory Manager", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 10, 460, 120, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Number of Pages:", WS_VISIBLE | WS_CHILD, 20, 40, 150, 20, hwnd, NULL, NULL, NULL);
            hwndNumPages = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 40, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Number of Frames:", WS_VISIBLE | WS_CHILD, 20, 70, 150, 20, hwnd, NULL, NULL, NULL);
            hwndNumFrames = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 70, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("BUTTON", "Initialize", WS_VISIBLE | WS_CHILD, 390, 55, 80, 30, hwnd, (HMENU)1, NULL, NULL);

            CreateWindow("BUTTON", "Page Access Control", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 140, 460, 120, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Virtual Page to Access:", WS_VISIBLE | WS_CHILD, 20, 170, 150, 20, hwnd, NULL, NULL, NULL);
            hwndVirtualPage = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 170, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("BUTTON", "Access Page", WS_VISIBLE | WS_CHILD, 390, 165, 80, 30, hwnd, (HMENU)2, NULL, NULL);

            hwndOutput = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFTNOWORDWRAP, 20, 210, 440, 100, hwnd, NULL, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                try {
                    char buffer[256];
                    GetWindowText(hwndNumPages, buffer, 256);
                    int numPages = atoi(buffer);

                    GetWindowText(hwndNumFrames, buffer, 256);
                    int numFrames = atoi(buffer);

                    vmm = new VirtualMemoryManager(numPages, numFrames);
                    SetWindowText(hwndOutput, "Virtual Memory Manager initialized!");
                } catch (const std::exception& e) {
                    SetWindowText(hwndOutput, e.what());
                }
            }
            else if (LOWORD(wParam) == 2) {
                try {
                    if (!vmm) {
                        SetWindowText(hwndOutput, "Please initialize the Virtual Memory Manager first!");
                        break;
                    }

                    char buffer[256];
                    GetWindowText(hwndVirtualPage, buffer, 256);
                    int virtualPage = atoi(buffer);

                    std::string result = vmm->accessPage(virtualPage);
                    SetWindowText(hwndOutput, result.c_str());
                } catch (const std::exception& e) {
                    SetWindowText(hwndOutput, e.what());
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }

    } catch (const std::exception& e) {
        MessageBox(hwnd, e.what(), "Exception", MB_OK | MB_ICONERROR);
    }

    return 0;
}
