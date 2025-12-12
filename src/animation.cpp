


#include "animation.h"

// =============================================================================
// PLAY FULL-SCREEN TRANSITION ANIMATION
// =============================================================================
void playFullScreenAnimation(Animation &anim, const unsigned char **frames,
                             uint8_t count, uint32_t delayMs, bool loop) {
    anim.frames = frames;
    anim.frameCount = count;
    anim.frameDelay = delayMs;
    anim.fullScreen = true;
    anim.loop = loop;
    anim.active = true;
    anim.currentFrame = 0;
    anim.lastUpdate = millis();
    anim.x = 0;
    anim.y = 0;
    anim.width = 128;  // Full screen width
    anim.height = 64;  // Full screen height
}

// =============================================================================
// PLAY OVERLAY ANIMATION AT POSITION
// =============================================================================
void playOverlayAnimation(Animation &anim, const unsigned char **frames,
                          uint8_t count, uint32_t delayMs,
                          uint8_t x, uint8_t y, uint8_t w, uint8_t h,
                          bool loop) {
    anim.frames = frames;
    anim.frameCount = count;
    anim.frameDelay = delayMs;
    anim.fullScreen = false;
    anim.loop = loop;
    anim.active = true;
    anim.currentFrame = 0;
    anim.lastUpdate = millis();
    anim.x = x;
    anim.y = y;
    anim.width = w;
    anim.height = h;
}

// =============================================================================
// STOP ANIMATION
// =============================================================================
void stopAnimation(Animation &anim) {
    anim.active = false;
    anim.currentFrame = 0;
}

// =============================================================================
// UPDATE ANIMATION (call every frame)
// =============================================================================
void updateAnimation(Animation &anim) {
    if (!anim.active || !anim.frames) return;
    
    uint32_t now = millis();
    if (now - anim.lastUpdate >= anim.frameDelay) {
        anim.currentFrame++;
        
        if (anim.currentFrame >= anim.frameCount) {
            if (anim.loop) {
                anim.currentFrame = 0;  // Loop back to start
            } else {
                anim.active = false;  // Animation finished
                return;
            }
        }
        
        anim.lastUpdate = now;
    }
}

// =============================================================================
// CHECK IF ANIMATION IS PLAYING
// =============================================================================
bool isAnimationPlaying(Animation &anim) {
    return anim.active;
}

// =============================================================================
// GET CURRENT FRAME BITMAP
// =============================================================================
const unsigned char* getCurrentFrame(Animation &anim) {
    if (!anim.active || !anim.frames || anim.currentFrame >= anim.frameCount) {
        return nullptr;
    }
    return anim.frames[anim.currentFrame];
}