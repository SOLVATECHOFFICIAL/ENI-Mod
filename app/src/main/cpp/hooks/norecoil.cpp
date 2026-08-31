#include "norecoil.h"

struct RecoilConfig {
    bool enabled = true;
    float reduction = 0.85f;  // 85% reduction (not 100% - detectable)
    bool randomize = true;    // Add slight randomness to pattern
};

RecoilConfig g_RecoilConfig;

// Weapon recoil pattern data (extracted from game)
struct RecoilPattern {
    float x[30];
    float y[30];
    int bulletCount;
};

// Hook weapon fire to modify recoil
void* original_FireWeapon = nullptr;
void FireWeaponHook(Weapon* self, Vector3* recoil) {
    if (g_RecoilConfig.enabled) {
        recoil->x *= (1.0f - g_RecoilConfig.reduction);
        recoil->y *= (1.0f - g_RecoilConfig.reduction);
        
        if (g_RecoilConfig.randomize) {
            recoil->x += (rand() % 10 - 5) / 100.0f;
            recoil->y += (rand() % 10 - 5) / 100.0f;
        }
    }
    
    original_FireWeapon(self, recoil);
}

void InstallNoRecoilHook() {
    void* fireAddr = GetIL2CPPMethod("Weapon", "ApplyRecoil", 1);
    if (fireAddr) {
        HOOK(fireAddr, FireWeaponHook, original_FireWeapon);
        LOGD("No recoil hook installed");
    }
}
