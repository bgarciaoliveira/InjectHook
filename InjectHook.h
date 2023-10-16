#pragma once

namespace HookLib {
    // Enum for different jump types. Additional jump types can be added as needed by the user.
    enum HookType {
        JMP_LONG
    };

    // Installs a hook on a specific function.
    bool InstallHook(void* targetFunction, void* hookAddress, HookType type);

    // Removes the hook.
    bool RemoveHook(void* targetFunction);
}
