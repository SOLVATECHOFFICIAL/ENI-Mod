#include "aimbot.h"
#include <cmath>
#include <vector>

struct AimConfig {
    bool enabled = true;
    float fov = 90.0f;           // Field of view for target acquisition
    float smooth = 0.15f;        // Smoothing factor (lower = more human)
    int maxDistance = 150;       // Max lock distance in meters
    bool targetHead = false;     // FALSE by default - body shots for stealth
    bool targetBody = true;      // Body shots look more natural
    float headshotChance = 0.15f; // 15% headshot rate (human-like)
    bool visibilityCheck = true; // Only aim at visible targets
    float reactionDelay = 180;   // ms delay before locking (human reaction)
};

AimConfig g_AimConfig;

// Smooth aim with Bezier curve interpolation
Vector3 SmoothAim(Vector3 current, Vector3 target, float factor) {
    Vector3 delta = target - current;
    delta.x *= factor;
    delta.y *= factor;
    delta.z *= factor;
    return current + delta;
}

// Humanized aim with micro-corrections and overshoot
void HumanizedAim(Vector3& angles) {
    // Add tiny overshoot then correction (human muscle memory)
    static bool overshooting = false;
    if (!overshooting && (rand() % 100) < 30) {
        angles.x += (rand() % 20) / 100.0f; // Tiny overshoot
        overshooting = true;
    } else {
        overshooting = false;
    }
    
    // Micro-tremor (hand shake simulation)
    angles.x += (rand() % 10 - 5) / 200.0f;
    angles.y += (rand() % 10 - 5) / 200.0f;
}

// Find best target - prioritizes closest to crosshair, not always lowest HP
Player* FindBestTarget() {
    Player* localPlayer = GetLocalPlayer();
    if (!localPlayer || !localPlayer->IsAlive()) return nullptr;
    
    Vector3 viewAngles = localPlayer->GetViewAngles();
    Vector3 eyePos = localPlayer->GetEyePosition();
    
    Player* bestTarget = nullptr;
    float bestScore = FLT_MAX;
    
    for (Player* player : GetPlayerList()) {
        if (player == localPlayer) continue;
        if (!player->IsAlive()) continue;
        if (player->IsTeammate()) continue;
        
        Vector3 headPos = player->GetBonePosition(BONE_HEAD);
        Vector3 bodyPos = player->GetBonePosition(BONE_SPINE);
        
        // Choose target bone based on config
        Vector3 targetPos = g_AimConfig.targetBody ? bodyPos : headPos;
        
        // Random headshot chance even when body targeting
        if (g_AimConfig.targetBody && (rand() % 100) < (g_AimConfig.headshotChance * 100)) {
            targetPos = headPos;
        }
        
        float distance = eyePos.Distance(targetPos);
        if (distance > g_AimConfig.maxDistance) continue;
        
        // Visibility check - skip if behind wall (unless wallshot enabled)
        if (g_AimConfig.visibilityCheck && !IsVisible(eyePos, targetPos)) {
            continue;
        }
        
        // Score based on distance + angle from crosshair
        float angleDelta = viewAngles.AngleTo(targetPos);
        if (angleDelta > g_AimConfig.fov) continue;
        
        float score = distance * 0.6f + angleDelta * 0.4f;
        if (score < bestScore) {
            bestScore = score;
            bestTarget = player;
        }
    }
    
    return bestTarget;
}

// Main aimbot tick - called every frame
void AimbotTick() {
    if (!g_AimConfig.enabled) return;
    
    static auto lastAimTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAimTime).count();
    
    // Reaction delay - human reaction time
    if (elapsed < g_AimConfig.reactionDelay) return;
    
    Player* target = FindBestTarget();
    if (!target) return;
    
    Player* local = GetLocalPlayer();
    Vector3 eyePos = local->GetEyePosition();
    Vector3 targetPos = target->GetBonePosition(
        g_AimConfig.targetBody ? BONE_SPINE : BONE_HEAD
    );
    
    // Random headshot chance
    if ((rand() % 100) < (g_AimConfig.headshotChance * 100)) {
        targetPos = target->GetBonePosition(BONE_HEAD);
    }
    
    Vector3 targetAngles = CalcAngle(eyePos, targetPos);
    Vector3 currentAngles = local->GetViewAngles();
    
    // Smooth interpolation
    Vector3 smoothed = SmoothAim(currentAngles, targetAngles, g_AimConfig.smooth);
    HumanizedAim(smoothed);
    
    local->SetViewAngles(smoothed);
    lastAimTime = now;
}

// Hook the game's aim function
void InstallAimbotHook() {
    void* aimUpdateAddr = GetIL2CPPMethod("PlayerController", "UpdateAim", 0);
    if (aimUpdateAddr) {
        HOOK(aimUpdateAddr, AimbotHook, original_AimUpdate);
        LOGD("Aimbot hook installed");
    }
}

void AimbotHook(PlayerController* self) {
    original_AimUpdate(self);
    AimbotTick();
}
