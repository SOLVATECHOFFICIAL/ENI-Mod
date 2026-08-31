#include "anticheat_bypass.h"

// QSec detection vectors:
// 1. SHA256 hash of loaded libraries
// 2. CreateRemoteThread / pthread_create monitoring
// 3. Screenshot capture (SurfaceFlinger)
// 4. Memory scanning for known cheat signatures
// 5. Debugger detection (ptrace, /proc/self/status)

void BypassQSecScanner() {
    // Find QSec's memory scanner thread and suspend it
    // Or patch the scanner to always return "clean"
    
    void* scannerAddr = FindPatternInLib("libqsec.so",
        "\x55\x48\x89\xE5\x48\x81\xEC", "xxxxxxx");
    
    if (scannerAddr) {
        // NOP the scanner function
        uint32_t nop = 0xD503201F; // ARM64 NOP
        mprotect((void*)((uintptr_t)scannerAddr & ~0xFFF), 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
        memcpy(scannerAddr, &nop, 4);
        LOGD("QSec scanner NOPed");
    }
}

// Hide from /proc/self/maps - remove our lib entry
void HideLibraryFromMaps(const char* libName) {
    FILE* maps = fopen("/proc/self/maps", "r");
    if (!maps) return;
    
    char line[512];
    while (fgets(line, sizeof(line), maps)) {
        if (strstr(line, libName)) {
            // Parse address range and unmap from proc visibility
            // Requires kernel-level manipulation or ptrace trickery
        }
    }
    fclose(maps);
}

// Anti-debug: Return fake tracer PID
int GetTracerPid() {
    FILE* status = fopen("/proc/self/status", "r");
    if (!status) return 0;
    
    char line[256];
    int tracerPid = 0;
    while (fgets(line, sizeof(line), status)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            tracerPid = atoi(line + 10);
            break;
        }
    }
    fclose(status);
    
    // Return 0 even if being traced
    return 0;
}

// Hook ptrace to block debugger attachment
void* original_ptrace = nullptr;
long PtraceHook(int request, pid_t pid, void* addr, void* data) {
    if (request == PTRACE_TRACEME) {
        LOGD("Blocked PTRACE_TRACEME");
        return 0; // Pretend success
    }
    return ((long(*)(int, pid_t, void*, void*))original_ptrace)(request, pid, addr, data);
}

void InstallAntiDebugHooks() {
    void* ptraceAddr = dlsym(RTLD_NEXT, "ptrace");
    if (ptraceAddr) {
        A64HookFunction(ptraceAddr, (void*)PtraceHook, (void**)&original_ptrace);
    }
}
