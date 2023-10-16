#include "InjectHook.h"
#include <Windows.h>
#include <map>

namespace HookLib {

    struct HookData {
        BYTE originalBytes[5];
        void* targetFunction;
    };

    std::map<void*, HookData> hooks;

    bool InstallHook(void* targetFunction, void* hookAddress, HookType type) {
        DWORD oldProtect;
        HookData data;

        // Save the original bytes of the target function.
        memcpy(data.originalBytes, targetFunction, 5);
        data.targetFunction = targetFunction;
        hooks[targetFunction] = data;

        // Change memory protection to read/write.
        VirtualProtect(targetFunction, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

        // Apply the hook based on the specified type.
        switch (type) {
            case JMP_LONG: {
                DWORD relativeAddress = ((DWORD)hookAddress - (DWORD)targetFunction) - 5;
                *(BYTE*)targetFunction = 0xE9;
                *(DWORD*)((DWORD)targetFunction + 1) = relativeAddress;
                break;
            }
            // Additional jump types can be added here.
            default:
                break;
        }

        // Restore the original memory protection.
        VirtualProtect(targetFunction, 5, oldProtect, &oldProtect);

        return true;
    }

    bool RemoveHook(void* targetFunction) {
        auto it = hooks.find(targetFunction);
        if (it == hooks.end()) return false;

        DWORD oldProtect;
        VirtualProtect(targetFunction, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy(targetFunction, it->second.originalBytes, 5);
        VirtualProtect(targetFunction, 5, oldProtect, &oldProtect);

        hooks.erase(it);

        return true;
    }
}
