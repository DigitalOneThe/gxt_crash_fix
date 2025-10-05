// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"

typedef void(__cdecl* TRAMPOLINE_FUNC)();
TRAMPOLINE_FUNC FixGxtCrash_t = nullptr;

char* text = nullptr;

// Данные
const char* invalid_strings[] = {
    "~k~~SWITCH_DEBUG_CAM_ON~",
    "~k~~TAKE_SCREEN_SHOT~",
    "~k~~KYEMAN~"
};
const int NUM_INVALID_STRINGS = sizeof(invalid_strings) / sizeof(invalid_strings[0]);
constexpr uintptr_t FixGxtCrash_Addr = 0x69DB54;

bool InitializeMinHook() {
    if (MH_Initialize() != MH_OK) {
        return false;
    }

    return true;
}

void __declspec(naked) HOOK_FixGxtCrash() {
    __asm {
        push esi
        push edi
        push ecx
        push ebx

        mov esi, ecx
        mov text, esi

        test ecx, ecx
        jz invalid

        pop ebx
        pop ecx
        pop edi
        pop esi
        jmp[FixGxtCrash_t]

    invalid:
        pop ebx
        pop ecx
        pop edi
        pop esi
        retn
    }
}

void InstallFixGxtCrasher() {
    MH_CreateHook((void*)FixGxtCrash_Addr, &HOOK_FixGxtCrash,
        reinterpret_cast<void**>(&FixGxtCrash_t));
    if (MH_EnableHook((void*)FixGxtCrash_Addr) != MH_OK) {}
}

DWORD WINAPI InitializeAndLoad(LPVOID param) {
    while (*reinterpret_cast<unsigned char*>(0xC8D4C0) != 9) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1u));
    }

    InitializeMinHook();
    InstallFixGxtCrasher();

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, &InitializeAndLoad, 0, 0, 0);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

