// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"

typedef void(__cdecl* TRAMPOLINE_FUNC)();
TRAMPOLINE_FUNC FixGxtCrash_t = nullptr;

const char invalid_text_1[] = "~k~~SWITCH_DEBUG_CAM_ON~";
const char invalid_text_2[] = "~k~~TAKE_SCREEN_SHOT~";
const char invalid_text_3[] = "~k~~KYEMAN~";

constexpr uintptr_t FixGxtCrash_Addr = 0x69DB54;

bool InitializeMinHook() {
    if (MH_Initialize() != MH_OK) {
        return false;
    }

    return true;
}

static void __declspec(naked) HOOK_FixGxtCrash() {
    __asm {
        push esi
        push edi
        push ecx

        mov esi, ecx

        test ecx, ecx
        jz invalid

        mov edi, offset invalid_text_1
        call strcmp_fix
        cmp eax, 0
        je invalid

        mov edi, offset invalid_text_2
        call strcmp_fix
        cmp eax, 0
        je invalid

        mov edi, offset invalid_text_3
        call strcmp_fix
        cmp eax, 0
        je invalid

        pop ecx
        pop edi
        pop esi
        jmp[FixGxtCrash_t]

    invalid:
        pop ecx
        pop edi
        pop esi
        //xor eax, eax
        retn

    strcmp_fix:
        push ecx
        push edx
        push esi
        push edi
    strcmp_loop:
        mov cl, [esi]
        mov dl, [edi]
        cmp cl, dl
        jne not_equal
        test cl, cl
        jz equal
        inc esi
        inc edi
        jmp strcmp_loop
    equal:
        xor eax, eax
        jmp strcmp_done
    not_equal:
        mov eax, 1
    strcmp_done:
        pop edi
        pop esi
        pop edx
        pop ecx
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

