#include "animation.h"
#include "segway_bitmaps.h"
#include "startmenu_bitmaps.h"
#include "drake_bitmaps.h"


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
        if (!active) {
            stopAnimation();// Stop if not looping
        }
    }

}
 
// =============================================================================
// CHECK IF ANIMATION IS PLAYING
// =============================================================================
bool Animation::isAnimationPlaying() {
    return active;
}

// =============================================================================
// GET CURRENT FRAME BITMAP
// =============================================================================
const unsigned char* Animation::getCurrentFrame() {
    return frames[currentFrame];
}

Animation segwayAnimation(segwayArray ,segwayFramesCount);
Animation startMenuAnimation(startMenuArray ,startMenuFramesCount);
Animation drakeAnimation(drakeArray ,drakeFramesCount);