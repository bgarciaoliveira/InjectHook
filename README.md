# InjectHook
A lightweight C++ library designed for function interception within injected DLLs, providing a streamlined approach to modifying application behavior at runtime. Ideal for educational purposes, debugging, and dynamic software analysis.


# InjectHook: C++ Function Hooking Library for DLL Injection

`InjectHook` is a C++ library designed to facilitate function interception within injected DLLs, enabling modification of target application behavior at runtime.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
  - [Preparing for DLL Injection](#preparing-for-dll-injection)
  - [Hooking a Function](#hooking-a-function)
  - [Removing a Hook](#removing-a-hook)
- [Notes](#notes)

## Installation

1. Add the `InjectHook.h` and `InjectHook.cpp` files to your DLL project.
2. Include `InjectHook.h` wherever you intend to leverage the hooking functionalities.

## Usage

### Preparing for DLL Injection

Your DLL should have an entry point, typically the `DllMain` function, which is called upon injection:

```cpp
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Actions to perform when the DLL is injected
            break;
        case DLL_PROCESS_DETACH:
            // Actions to perform when the DLL is unloaded
            break;
        // Handle other cases if required
    }
    return TRUE;
}
```

### Hooking a Function

Inside the `DLL_PROCESS_ATTACH` case or another suitable location, use the `InstallHook` function to initiate a hook:

```cpp
bool InstallHook(void* targetFunction, void* hookFunction, HookType type);
```

- `targetFunction`: Memory address of the function you aim to hook in the target process.
- `hookFunction`: Pointer to your `naked` function that will control flow redirection and push registers.
- `type`: Type of hook. Currently, only `JMP_LONG` is supported, but additional jump types can be added by the user as needed.

**Example**:

```cpp

__declspec(naked) void MyNakedFunction() { // Just an example
    const continueAddress = 0x12345678;
    const endFunctionAdress = 0x12345123;
    __asm {
        // Restore the overwritten assembly here
        mov eax, dword ptr ds:[ebp-0214h]
        sar eax, 2
         
        // Push some registers, etc        
        push dword ptr ss:[ebp+8]
        mov eax, dword ptr ss:[ebp+0ch]
        push eax
        lea edx
       
        // Call the __stdcall hook handler.
        call HookHandlerFunction

        // Test the return of the hook handler
        test eax, eax
        je canContinue

        jmp endFunctionAdress      

    canContinue:
        // Continue the normal execution flow
        jmp continueAddress
    }
}

void __stdcall HookHandlerFunction(int *param1, int param2, int &param3) {
    // This is the function that gets called from the naked function.
    // Use __stdcall calling convention, so you won't need to clean up the stack
    // Your hook handling code here.

    *param1 <<= 2;
    param3 *= 2;

    return param2 > 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            DWORD TargetFunctionAddress = 0x12345678;  // Use the target address of the main program
            InstallHook((void*)TargetFunctionAddress, &MyNakedFunction, HookLib::JMP_LONG);
            break;
    }
    return TRUE;
}
```

### Removing a Hook

To revert to the original behavior:

```cpp
bool RemoveHook(void* targetFunction);
```

- `targetFunction`: Memory address of the function you previously hooked.

**Example**:

```cpp
RemoveHook((void*)TargetFunctionAddress);
```

## Notes

1. When using `InjectHook` to hook a function, any overwritten content of the original function will need management within the `naked` hook function.
2. Ensure no threads are actively executing the hooked functions when setting or removing hooks to prevent unpredictable behaviors.
3. The library is tailored for the x86 architecture. Ensure compatibility with your target environment.
4. The hooking mechanism in `InjectHook` currently supports the `JMP_LONG` type, and this will work for most cases. However, users can extend the library to support additional jump types as needed.
5. You need to inject your generated dll into the target executable, using LoadLibrary.

