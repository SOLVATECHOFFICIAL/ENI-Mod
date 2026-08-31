#include "esp.h"

// CRITICAL: ESP draws on EXTERNAL overlay, NOT game canvas
// QScans for canvas modifications - this bypasses that detection

struct ESPConfig {
    bool enabled = true;
    bool boxESP = true;
    bool boneESP = false;      // Skeleton - more detectable, disabled by default
    bool healthBar = true;
    bool nameESP = false;      // Names are reportable, disabled
    bool distanceESP = true;
    bool itemESP = true;       // Loot/weapon highlights
    int maxDistance = 200;
    Color enemyColor = {255, 50, 50, 180};   // Red, semi-transparent
    Color itemColor = {50, 255, 50, 180};    // Green
};

ESPConfig g_ESPConfig;

// World-to-screen conversion using game's camera matrices
bool WorldToScreen(Vector3 worldPos, Vector2& screenPos) {
    Matrix4x4 viewMatrix = GetViewMatrix();
    Matrix4x4 projMatrix = GetProjectionMatrix();
    
    Vector4 clipPos = projMatrix * (viewMatrix * Vector4(worldPos, 1.0f));
    
    if (clipPos.w < 0.1f) return false; // Behind camera
    
    screenPos.x = (clipPos.x / clipPos.w + 1.0f) * 0.5f * ScreenWidth;
    screenPos.y = (1.0f - clipPos.y / clipPos.w) * 0.5f * ScreenHeight;
    
    return true;
}

// Draw ESP on external SurfaceView overlay (not game canvas)
void DrawESP(Canvas* canvas) {
    if (!g_ESPConfig.enabled) return;
    
    Player* local = GetLocalPlayer();
    if (!local) return;
    
    Vector3 localPos = local->GetPosition();
    
    for (Player* player : GetPlayerList()) {
        if (player == local) continue;
        if (!player->IsAlive()) continue;
        
        Vector3 headPos = player->GetBonePosition(BONE_HEAD);
        Vector3 feetPos = player->GetBonePosition(BONE_ROOT);
        
        float distance = localPos.Distance(headPos);
        if (distance > g_ESPConfig.maxDistance) continue;
        
        Vector2 headScreen, feetScreen;
        if (!WorldToScreen(headPos, headScreen)) continue;
        if (!WorldToScreen(feetPos, feetScreen)) continue;
        
        float height = feetScreen.y - headScreen.y;
        float width = height * 0.4f;
        
        // Box ESP
        if (g_ESPConfig.boxESP) {
            DrawRect(canvas, headScreen.x - width/2, headScreen.y, 
                     width, height, g_ESPConfig.enemyColor, 2.0f);
        }
        
        // Health bar
        if (g_ESPConfig.healthBar) {
            float healthPct = player->GetHealth() / player->GetMaxHealth();
            DrawHealthBar(canvas, headScreen.x - width/2 - 8, headScreen.y,
                         4, height, healthPct);
        }
        
        // Distance text
        if (g_ESPConfig.distanceESP) {
            char distText[32];
            snprintf(distText, sizeof(distText), "%.0fm", distance);
            DrawText(canvas, distText, feetScreen.x, feetScreen.y + 5,
                    {255, 255, 255, 200}, 12);
        }
    }
    
    // Item ESP
    if (g_ESPConfig.itemESP) {
        for (Item* item : GetItemList()) {
            Vector3 itemPos = item->GetPosition();
            float distance = localPos.Distance(itemPos);
            if (distance > g_ESPConfig.maxDistance) continue;
            
            Vector2 itemScreen;
            if (WorldToScreen(itemPos, itemScreen)) {
                DrawText(canvas, item->GetName(), itemScreen.x, itemScreen.y,
                        g_ESPConfig.itemColor, 10);
            }
        }
    }
}
