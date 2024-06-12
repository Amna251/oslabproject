#include <windows.h> // Include the Windows header file for WinAPI functions and types.
#include <vector> // Include the standard library header for vectors.
#include <string> // Include the standard library header for strings.
#include <sstream> // Include the standard library header for string streams.
#include <iostream> // Include the standard library header for input/output streams.
#include <stdexcept> // Include the standard library header for exceptions.

struct PageTableEntry { // Define a struct to represent a page table entry.
    int frameNumber; // Integer to store the frame number.
    bool valid; // Boolean to indicate if the page is valid.
};

class VirtualMemoryManager { // Define a class for the virtual memory manager.
public:
    VirtualMemoryManager(int numPages, int numFrames) // Constructor to initialize the manager with the number of pages and frames.
        : pageTable(numPages), numFrames(numFrames) { // Initialize member variables using initializer list.
        if (numPages <= 0 || numFrames <= 0) { // Check if the number of pages or frames is non-positive.
            throw std::invalid_argument("Number of pages and frames must be positive."); // Throw an exception for invalid arguments.
        }
    }

    std::string accessPage(int virtualPage) { // Method to access a virtual page and return a string indicating the result.
        if (virtualPage < 0 || virtualPage >= pageTable.size()) { // Check if the virtual page number is out of range.
            throw std::out_of_range("Invalid virtual page number."); // Throw an exception for an invalid virtual page number.
        }
        std::ostringstream oss; // Create a string stream to build the output message.
        if (pageTable[virtualPage].valid) { // Check if the page is valid (in memory).
            oss << "Page Hit: Page " << virtualPage << " is in frame " << pageTable[virtualPage].frameNumber; // Append message for a page hit.
        } else { // If the page is not valid (not in memory).
            oss << "Page Fault: Page " << virtualPage << " not in memory. Fetching from memory..."; // Append message for a page fault.
            pageTable[virtualPage].valid = true; // Set the page as valid.
            pageTable[virtualPage].frameNumber = allocateFrame(); // Allocate a frame for the page.
        }
        return oss.str(); // Return the constructed message as a string.
    }

private:
    std::vector<PageTableEntry> pageTable; // Vector to store page table entries.
    int numFrames; // Integer to store the total number of frames.

    int allocateFrame() { // Method to allocate a frame for a page.
        static int nextFrame = 0; // Static variable to keep track of the next available frame.
        int frameToAllocate = nextFrame; // Get the next available frame.
        nextFrame = (nextFrame + 1) % numFrames; // Update the next available frame for future allocations.
        return frameToAllocate; // Return the allocated frame number.
    }
};

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM); // Declaration of the window procedure function.

char szClassName[] = "VirtualMemoryManagerClass"; // Define a class name for the window.
VirtualMemoryManager* vmm = NULL; // Pointer to the virtual memory manager object.
HWND hwndNumPages, hwndNumFrames, hwndVirtualPage, hwndOutput; // Handle variables for window elements.

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) { // WinMain function definition.
    HWND hwnd; // Handle variable for the window.
    MSG messages; // Message structure for Windows messages.
    WNDCLASSEX wincl; // Window class structure.

    wincl.hInstance = hThisInstance; // Set the instance handle for the window class.
    wincl.lpszClassName = szClassName; // Set the class name for the window class.
    wincl.lpfnWndProc = WindowProcedure; // Set the window procedure function for the window class.
    wincl.style = CS_DBLCLKS; // Set the window class style.
    wincl.cbSize = sizeof(WNDCLASSEX); // Set the size of the window class structure.

    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Load the default application icon.
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW); // Load the default arrow cursor.
    wincl.lpszMenuName = NULL; // Set the menu name to NULL.
    wincl.cbClsExtra = 0; // Set the number of extra bytes to allocate for the window class structure.
    wincl.cbWndExtra = 0; // Set the number of extra bytes to allocate for each window instance.
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND; // Set the background color for the window class.

    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // Load the default small application icon.

    if (!RegisterClassEx(&wincl)) return 0; // Register the window class. If registration fails, return 0.

    hwnd = CreateWindowEx( // Create the window with extended window styles.
        0, // Extended window style.
        szClassName, // Class name.
        "Virtual Memory Manager", // Window title.
        WS_OVERLAPPEDWINDOW, // Window style.
        CW_USEDEFAULT, // Initial X position.
        CW_USEDEFAULT, // Initial Y position.
        500, // Initial width.
        400, // Initial height.
        HWND_DESKTOP, // Parent window handle.
        NULL, // Menu handle.
        hThisInstance, // Instance handle.
        NULL // Additional application data.
    );

    if (!hwnd) { // If window creation fails.
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK); // Show an error message.
        return 0; // Return 0 to indicate failure.
    }

    ShowWindow(hwnd, nCmdShow); // Show the window.
    while (GetMessage(&messages, NULL, 0, 0)) { // Message loop to handle Windows messages.
        TranslateMessage(&messages); // Translate keyboard messages.
        DispatchMessage(&messages); // Dispatch messages to the window procedure.
    }

    return messages.wParam; // Return the WPARAM of the last message.
}

// Window Procedure function to handle messages for the main window
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    try {
        switch (message) {
        case WM_CREATE:
            // Create controls for initializing the virtual memory manager
            // Create a group box for initialization
            CreateWindow("BUTTON", "Initialize Virtual Memory Manager", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 10, 460, 120, hwnd, NULL, NULL, NULL);

            // Create labels and text boxes for entering number of pages and frames
            CreateWindow("STATIC", "Number of Pages:", WS_VISIBLE | WS_CHILD, 20, 40, 150, 20, hwnd, NULL, NULL, NULL);
            hwndNumPages = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 40, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Number of Frames:", WS_VISIBLE | WS_CHILD, 20, 70, 150, 20, hwnd, NULL, NULL, NULL);
            hwndNumFrames = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 70, 200, 20, hwnd, NULL, NULL, NULL);

            // Create a button for initializing the virtual memory manager
            CreateWindow("BUTTON", "Initialize", WS_VISIBLE | WS_CHILD, 390, 55, 80, 30, hwnd, (HMENU)1, NULL, NULL);

            // Create a group box for page access control
            CreateWindow("BUTTON", "Page Access Control", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 140, 460, 120, hwnd, NULL, NULL, NULL);

            // Create labels and text boxes for entering the virtual page number
            CreateWindow("STATIC", "Virtual Page to Access:", WS_VISIBLE | WS_CHILD, 20, 170, 150, 20, hwnd, NULL, NULL, NULL);
            hwndVirtualPage = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 170, 200, 20, hwnd, NULL, NULL, NULL);

            // Create a button for accessing the virtual page
            CreateWindow("BUTTON", "Access Page", WS_VISIBLE | WS_CHILD, 390, 165, 80, 30, hwnd, (HMENU)2, NULL, NULL);

            // Create a text box for displaying the output
            hwndOutput = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFTNOWORDWRAP, 20, 210, 440, 100, hwnd, NULL, NULL, NULL);
            break;

        case WM_COMMAND:
            // Handle button click events
            if (LOWORD(wParam) == 1) {
                try {
                    char buffer[256];
                    // Get the number of pages entered by the user
                    GetWindowText(hwndNumPages, buffer, 256);
                    int numPages = atoi(buffer);

                    // Get the number of frames entered by the user
                    GetWindowText(hwndNumFrames, buffer, 256);
                    int numFrames = atoi(buffer);

                    // Initialize the virtual memory manager with the provided parameters
                    vmm = new VirtualMemoryManager(numPages, numFrames);
                    SetWindowText(hwndOutput, "Virtual Memory Manager initialized!"); // Display success message
                } catch (const std::exception& e) {
                    SetWindowText(hwndOutput, e.what()); // Display error message if initialization fails
                }
            }
            // Handle button click events
            else if (LOWORD(wParam) == 2) {
                try {
                    if (!vmm) {
                        SetWindowText(hwndOutput, "Please initialize the Virtual Memory Manager first!"); // Display message if VMM is not initialized
                        break;
                    }

                    char buffer[256];
                    // Get the virtual page number entered by the user
                    GetWindowText(hwndVirtualPage, buffer, 256);
                    int virtualPage = atoi(buffer);

                    // Access the virtual page and display the result
                    std::string result = vmm->accessPage(virtualPage);
                    SetWindowText(hwndOutput, result.c_str());
                } catch (const std::exception& e) {
                    SetWindowText(hwndOutput, e.what()); // Display error message if page access fails
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0); // Quit the application when the main window is closed
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam); // Handle other messages using default window procedure
        }

    } catch (const std::exception& e) {
        MessageBox(hwnd, e.what(), "Exception", MB_OK | MB_ICONERROR); // Display error message in case of exception
    }

    return 0;
}

