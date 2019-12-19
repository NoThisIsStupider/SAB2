#include <glad/glad.h>
#include <vector>
#include <string>

#include "../engineTester/main.h"
#include "entity.h"
#include "playermodel.h"
#include "../toolbox/vector.h"
#include "../particles/particletexture.h"
#include "../menu/timer.h"
#include "raceghost.h"

void PlayerModel::animate(int, float) {}

void PlayerModel::setOrientation(float, float, float, float, float, float, float, Vector3f*) {}

void PlayerModel::setBaseColor(float, float, float) {}

const float PlayerModel::getDisplayBallOffset() {return 0.0f;}

ParticleTexture* PlayerModel::getBallTexture() {return nullptr;}

void PlayerModel::log()
{
    if (Global::mainHudTimer == nullptr || Global::raceLogSize >= Global::raceLogSizeMax)
    {
        return;
    }

    GhostFrame frame;
    frame.time = Global::mainHudTimer->totalTime;
    frame.animIndex = currentAnimIndex;
    frame.animTime = currentAnimTime;
    frame.pos.set(&position);
    frame.rot.set(rotX, rotY, rotZ, rotRoll);
    frame.up.set(&currentUpDirection);

    Global::raceLog[Global::raceLogSize] = frame;
    Global::raceLogSize++;
}
