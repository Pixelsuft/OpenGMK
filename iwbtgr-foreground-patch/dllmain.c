#include <windows.h>
#define ISNULL(ptr) ((ptr) == NULL)

#if defined(_WIN64) && _WIN64
#error "Can't be compiled in 64-bit mode"
#endif

/*
Foreground Window Patch for IWBTGR (based on tas-rw-hook)
Created by Pixelsuft, 2024

WARNING: Compile in 32-bit mode
*/

int _fltused = 0;

typedef struct
{
    char orig_bytes[6];
    char patch[6];
    void* orig_addr;
    void* hook_addr;
} Hook;

typedef struct
{
    Hook getfgwin_hook;
    HANDLE phandle;
    HMODULE gm82_core_dll;
} App;

App app_stk;
App* app;

void hook_func(Hook* bh, void* orig_addr, void* hook_addr)
{
    size_t bytes_read;
    bh->orig_addr = orig_addr;
    bh->hook_addr = hook_addr;
    ReadProcessMemory(app->phandle, orig_addr, bh->orig_bytes, 6, &bytes_read);
	bh->patch[0] = '\x68';
	*(size_t*)&bh->patch[1] = (size_t)hook_addr;
	bh->patch[5] = '\xC3';
}

void hook_enable(Hook* bh)
{
    size_t bytes_written;
    WriteProcessMemory(app->phandle, (LPVOID)bh->orig_addr, bh->patch, sizeof(bh->patch), &bytes_written);
}

double __stdcall get_foreground_window_hook(void)
{
    return 1.0;
}

DWORD WINAPI dll_main_thread(HMODULE hmod)
{
    HANDLE _gm82_dll;
    _gm82_dll = LoadLibraryExW(L"gm82core.dll", NULL, 0);
    if (ISNULL(_gm82_dll)) {
        MessageBoxW(NULL, L"Failed to load gm82core.dll", L"Error!", MB_ICONERROR);
        return 1;
    }
    app = &app_stk;
    app->phandle = GetCurrentProcess();
    app->gm82_core_dll = _gm82_dll;
    hook_func(&app->getfgwin_hook, GetProcAddress(app->gm82_core_dll, "get_foreground_window"), &get_foreground_window_hook);
    hook_enable(&app->getfgwin_hook);
    return 0;
}

#if 1
BOOL APIENTRY _DllMainCRTStartup(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
#else
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
#endif
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);
        dll_main_thread(hModule);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
