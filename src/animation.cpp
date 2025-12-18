#include "animation.h"
#include "startmenu_bitmaps.h"
#include "gamecube_bitmaps.h"


// =============================================================================
// STOP ANIMATION
// =============================================================================
void Animation::stopAnimation() {
    active = false;
    currentFrame = 0;
}


// =============================================================================
// UPDATE ANIMATION (call every frame)
// =============================================================================
void Animation::updateAnimation() {
    currentFrame++;
    if (currentFrame >= frameCount) {
        currentFrame = 0;  // Loop back to start
        framesCycled = true;
        Serial.println("Frames Cycled");
        if (!active) {
            stopAnimation();// Stop if not looping
        }
    }

}
 
// =============================================================================
// CHECK IF ANIMATION IS PLAYING
// =============================================================================
bool Animation::isPlaying() {
    return active;
}

bool Animation::isComplete() {
    return framesCycled;
}

// =============================================================================
// GET CURRENT FRAME BITMAP
// =============================================================================
const unsigned char* Animation::getCurrentFrame() {
    return frames[currentFrame];
}

Animation startMenuAnimation(startMenuArray ,startMenuFramesCount);
Animation gamecubeAnimation(gamecubeArray,gamecubeFramesCount);