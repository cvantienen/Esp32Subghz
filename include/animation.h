#ifndef ANIMATION_H
#define ANIMATION_H
#include <Arduino.h>

// =============================================================================
// SIMPLE ANIMATION SYSTEM
// =============================================================================
// Easy way to play bitmap animations on screen or as transitions
class Animation {
public:
    // Constructor to initialize the animation with a fixed array of frames
    Animation(const unsigned char* frames[], uint8_t frameCount) 
        : frames(frames), frameCount(frameCount), currentFrame(0) {}

    void stopAnimation();
    // Update animation (call this every frame in your display task)
    void updateAnimation();
    // Check if animation is playing
    bool isAnimationPlaying();
    // Get current frame bitmap (returns nullptr if not active)
    const unsigned char* getCurrentFrame();


private:

    const unsigned char** frames;  // Array of frame pointers
    uint8_t frameCount;              // Number of frames in the array
    uint8_t currentFrame;           // Current frame index
    bool active = false; 
};
// Declare extern object
extern Animation segwayAnimation;
// Declare extern object
extern Animation startMenuAnimation;

#endif
