#include <Windows.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>

#pragma region Types & Macros
    #define is_recording 1 // set to 0 for replaying. 1 for recording.
    typedef unsigned char uchar;

    #if !is_recording
        struct action_data
        {
            long long action_time;
            bool action_keydown;
            uchar action_key;
        };
    #endif
#pragma endregion
#pragma region Function declarations
    void send_key(int);
    void send_key_down(int);
    void send_key_up(int);
    void click(bool);
    void set_mouse(int, int);
    long long get_time();
    void onkeydown(uchar);
    void onkeyup(uchar);
    void update_keymap();
    #if is_recording
        void record();
    #else
        void replay();
        void prepare_replay();
    #endif
#pragma endregion
#pragma region Global Variables
    bool* keymap;
    bool* lastkeymap;
    long long app_starttime;

    const char* filepath = "data.txt";
    std::ofstream write_file;
    std::ifstream read_file;

    #if !is_recording
        action_data* replay_data;
        int action_data_count = 0;
    #endif
#pragma endregion

int main()
{
    std::cout << "Welcome to the Action Recorder/Replayer! \n";
    std::cout << "Usage: to change between modes, change source code line 8. \n\n";
#if is_recording
    std::cout << "     Program is in <Recording Mode>.\n";
    std::cout << "     In this mode, W-A-S-D-Q-E-Tab-Space keys will be recorded and output into data.txt.\n";
#else
    std::cout << "     Program is in <Replay Mode>.\n";
    std::cout << "     In this mode, the program will replay the actions read from data.txt.\n\n";
    std::cout << "***  Focus on the application you want to send the keystrokes ***\n\n";
#endif

    std::cout << "\n\nStarting the program in 5 seconds.\n";
    Sleep(5000);

    keymap = (bool*)malloc(sizeof(bool) * 256);
    lastkeymap = (bool*)malloc(sizeof(bool) * 256);
    ZeroMemory(keymap, sizeof(bool) * 256);
    ZeroMemory(lastkeymap, sizeof(bool) * 256);

    #if is_recording
        write_file = std::ofstream(filepath);
        std::cout << "Started recording. \nClose the program whenever you want to stop.\n\n\n";
    #else
        read_file = std::ifstream(filepath);
        std::cout << "Preparing data for replaying.\n";
        prepare_replay();
        std::cout << "Starting replay.\n\n\n";
    #endif

    app_starttime = get_time();
    while (1)
    {
        #if is_recording
            record();
        #else
            replay();
        #endif
    }

    write_file.close();
    read_file.close();
    return 0;
}

void send_key_down(int key)
{
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}
void send_key_up(int key)
{
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;
    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}
void send_key(int key)
{
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}
void click(bool left = 1)
{
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi = MOUSEINPUT();

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi = MOUSEINPUT();

    if (left)
    {
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    }
    else
    {
        inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    }

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}
void set_mouse(int x, int y) { SetCursorPos(x, y); }
long long get_time() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

void onkeydown(uchar key)
{
    switch (key)
    {
        case   9: // tab
        case 'W':
        case 'A':
        case 'S':
        case 'D':
        case 'Q':
        case 'E':
        case ' ':
            write_file << (get_time() - app_starttime) << ",D," << key << "\n";
            break;
    }
}
void onkeyup(uchar key)
{
    switch (key)
    {
        case   9: // tab
        case 'W':
        case 'A':
        case 'S':
        case 'D':
        case 'Q':
        case 'E':
        case ' ':
            write_file << (get_time() - app_starttime) << ",U," << key << "\n";
            break;
    }
}

void update_keymap() { for (int i = 0; i < 255; ++i) keymap[i] = GetAsyncKeyState(i); }

#if is_recording
    void record() 
    {
        update_keymap();

        for (int i = 0; i < 256; ++i)
        {
            bool key = keymap[i];
            bool last_key = lastkeymap[i];
            if (!key && last_key)
            {
                onkeyup(i);
                lastkeymap[i] = false;
            }
            else if (key && !last_key)
            {
                onkeydown(i);
                lastkeymap[i] = true;
            }
        }
    }
#else
    void replay() 
    {
        static int current_action_index = 0;
        action_data action = replay_data[current_action_index];

        auto time_passed = get_time() - app_starttime;
        if (time_passed > action.action_time)
        {
            if (action.action_keydown) send_key_down(action.action_key);
            else send_key_up(action.action_key);

            if (current_action_index + 1 < action_data_count)
            {
                ++current_action_index;
            }
            else
            {
                std::cout << "All actions replayed. GGs!\n\n";
                exit(0);
            }
        }
    }

    void prepare_replay() 
    {
        std::string text;
        replay_data = (action_data*)malloc(sizeof(action_data) * 1024 * 32);
        while (std::getline(read_file, text))
        {
            auto len = text.length();
            action_data new_action = action_data();
            int delimiter_count = 0;
            int j = 0;
            for (int i = 0; i < len; ++i)
            {
                if (text[i] == ',')
                {
                    std::string substr = text.substr(j, i-j);
                    if (delimiter_count == 0) new_action.action_time = _atoi64(substr.c_str());
                    else new_action.action_keydown = substr == "D";

                    j = i + 1;
                    ++delimiter_count;
                }
            }
            new_action.action_key = *text.substr(j, len-j).c_str();
            replay_data[action_data_count++] = new_action;
        }
    }
#endif
