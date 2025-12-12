#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>

// =============================================================================
// SIMPLE ANIMATION SYSTEM
// =============================================================================
// Easy way to play bitmap animations on screen or as transitions

struct Animation {
    const unsigned char **frames;  // Array of bitmap pointers
    uint8_t frameCount;            // Number of frames
    uint32_t frameDelay;           // Milliseconds between frames
    uint8_t x, y;                  // Position (for overlay animations)
    uint8_t width, height;         // Bitmap dimensions
    bool fullScreen;               // If true, fills entire screen
    bool loop;                     // Loop animation?
    bool active;                   // Is animation playing?
    uint8_t currentFrame;          // Current frame index
    uint32_t lastUpdate;           // Last frame change time
};

// =============================================================================
// SIMPLE HELPER FUNCTIONS
// =============================================================================

// Play a full-screen    transition animation
void playFullScreenAnimation(Animation &anim, const unsigned char **frames, 
                             uint8_t count, uint32_t delayMs, bool loop = false);

// Play an overlay animation at a specific position
void playOverlayAnimation(Animation &anim, const unsigned char **frames,
                         uint8_t count, uint32_t delayMs, 
                         uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
                         bool loop = false);

// Stop animation
void stopAnimation(Animation &anim);

// Update animation (call this every frame in your display task)
void updateAnimation(Animation &anim);

// Check if animation is playing
bool isAnimationPlaying(Animation &anim);

// Get current frame bitmap (returns nullptr if not active)
const unsigned char* getCurrentFrame(Animation &anim);

#endif
