// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"

typedef void(__cdecl* TRAMPOLINE_FUNC)();
TRAMPOLINE_FUNC FixGxtCrash_t = nullptr;

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
        push ecx

        mov esi, ecx

        test ecx, ecx
        jz invalid

        pop ecx
        pop esi
        jmp[FixGxtCrash_t]

    invalid:
        pop ecx
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

