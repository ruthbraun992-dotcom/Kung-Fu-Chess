#pragma once
#include "PieceState.hpp"

struct AnimationConfig
{
    int framesPerSecond = 1;
    bool isLoop = true;
    double speedMetersPerSec = 0.0;
    PieceState nextStateWhenFinished = PieceState::IDLE;
    int frameCount = 0;   // מספר קבצי הפריים בתיקייה, בלי לטעון את התמונות עצמן
};