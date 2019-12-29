#include <glad/glad.h>

#include "entity.h"
#include "../models/models.h"
#include "../toolbox/vector.h"
#include "sa2sonicmodel.h"
#include "../renderEngine/renderEngine.h"
#include "../objLoader/objLoader.h"
#include "../engineTester/main.h"
#include "../entities/playersonic.h"
#include "../toolbox/maths.h"
#include "../animation/body.h"
#include "../animation/limb.h"
#include "../animation/animationresources.h"
#include "../particles/particletexture.h"
#include "../particles/particleresources.h"

#include <list>
#include <iostream>
#include <algorithm>
#include <cmath>

std::list<TexturedModel*> SA2SonicModel::modelBody;
std::list<TexturedModel*> SA2SonicModel::modelHead;
std::list<TexturedModel*> SA2SonicModel::modelLeftHumerus;
std::list<TexturedModel*> SA2SonicModel::modelLeftForearm;
std::list<TexturedModel*> SA2SonicModel::modelLeftHand;
std::list<TexturedModel*> SA2SonicModel::modelLeftThigh;
std::list<TexturedModel*> SA2SonicModel::modelLeftShin;
std::list<TexturedModel*> SA2SonicModel::modelLeftFoot;
std::list<TexturedModel*> SA2SonicModel::modelRightHumerus;
std::list<TexturedModel*> SA2SonicModel::modelRightForearm;
std::list<TexturedModel*> SA2SonicModel::modelRightHand;
std::list<TexturedModel*> SA2SonicModel::modelRightThigh;
std::list<TexturedModel*> SA2SonicModel::modelRightShin;
std::list<TexturedModel*> SA2SonicModel::modelRightFoot;

std::vector<std::list<TexturedModel*>> SA2SonicModel::modelsDash;

std::list<TexturedModel*> SA2SonicModel::modelJump;

std::vector<std::list<TexturedModel*>> SA2SonicModel::modelsJog;

std::list<TexturedModel*> SA2SonicModel::modelGrind;
std::list<TexturedModel*> SA2SonicModel::modelSkid;
std::list<TexturedModel*> SA2SonicModel::modelLightdash;
std::list<TexturedModel*> SA2SonicModel::modelFreefall;
std::list<TexturedModel*> SA2SonicModel::modelStomp;
std::list<TexturedModel*> SA2SonicModel::modelGrab;

SA2SonicModel::SA2SonicModel()
{
    position.x = 0;
    position.y = 0;
    position.z = 0;
    rotX = 0;
    rotY = 0;
    rotZ = 0;
    scale = 0.27f;
    visible = false;

    /*for (int i = 0; i < 31; i++)
    {
        std::list<TexturedModel*> list;
        SA2SonicModel::modelsDash.push_back(list);
    }*/

    models = &SA2SonicModel::modelsDash[0];

    myBody =         new Body(&modelBody); INCR_NEW("Entity")
    myHead =         new Limb(&modelHead,         1.2f,  -0.3f,   0,     myBody,   nullptr);        INCR_NEW("Entity")
    myLeftHumerus =  new Limb(&modelLeftHumerus,  0.9f,   0,     -0.9f,  myBody,   nullptr);        INCR_NEW("Entity")
    myLeftForearm =  new Limb(&modelLeftForearm,  0,     -1.3f,   0,     nullptr,  myLeftHumerus);  INCR_NEW("Entity")
    myLeftHand =     new Limb(&modelLeftHand,     0,     -1.3f,   0,     nullptr,  myLeftForearm);  INCR_NEW("Entity")
    myLeftThigh =    new Limb(&modelLeftThigh,   -0.9f,   0,     -0.3f,  myBody,   nullptr);        INCR_NEW("Entity")
    myLeftShin =     new Limb(&modelLeftShin,     0,     -1.3f,   0,     nullptr,  myLeftThigh);    INCR_NEW("Entity")
    myLeftFoot =     new Limb(&modelLeftFoot,     0,     -1.1f,   0,     nullptr,  myLeftShin);     INCR_NEW("Entity")
    myRightHumerus = new Limb(&modelRightHumerus, 0.9f,   0,      0.9f,  myBody,   nullptr);        INCR_NEW("Entity")
    myRightForearm = new Limb(&modelRightForearm, 0,     -1.3f,   0,     nullptr,  myRightHumerus); INCR_NEW("Entity")
    myRightHand =    new Limb(&modelRightHand,    0,     -1.3f,   0,     nullptr,  myRightForearm); INCR_NEW("Entity")
    myRightThigh =   new Limb(&modelRightThigh,  -0.9f,   0,      0.3f,  myBody,   nullptr);        INCR_NEW("Entity")
    myRightShin =    new Limb(&modelRightShin,    0,     -1.3f,   0,     nullptr,  myRightThigh);   INCR_NEW("Entity")
    myRightFoot =    new Limb(&modelRightFoot,    0,     -1.1f,   0,     nullptr,  myRightShin);    INCR_NEW("Entity")

    AnimationResources::assignAnimationsHuman(myBody, myHead,
        myLeftHumerus, myLeftForearm, myLeftHand,
        myRightHumerus, myRightForearm, myRightHand,
        myLeftThigh, myLeftShin, myLeftFoot,
        myRightThigh, myRightShin, myRightFoot);

    Main_addEntity(myBody);
    Main_addEntity(myHead);
    Main_addEntity(myLeftHumerus);
    Main_addEntity(myLeftForearm);
    Main_addEntity(myLeftHand);
    Main_addEntity(myLeftThigh);
    Main_addEntity(myLeftShin);
    Main_addEntity(myLeftFoot);
    Main_addEntity(myRightHumerus);
    Main_addEntity(myRightForearm);
    Main_addEntity(myRightHand);
    Main_addEntity(myRightThigh);
    Main_addEntity(myRightShin);
    Main_addEntity(myRightFoot);
}

void SA2SonicModel::step()
{

}

const float SA2SonicModel::getDisplayBallOffset()
{
    return displayBallOffset;
}

ParticleTexture* SA2SonicModel::getBallTexture()
{
    return ParticleResources::textureLightBlueTrail;
}

void SA2SonicModel::animate(int animIndex, float time)
{
    currentAnimIndex = animIndex;
    currentAnimTime = time;
    switch (animIndex)
    {
        case 0: //stand
        {
            Vector3f off = currentUpDirection.scaleCopy(limbsScale*displayHeightOffset);
            Vector3f pos = position + off;
            myBody->setBaseOrientation(pos.x, pos.y, pos.z, rotX, rotY, rotZ, rotRoll, limbsScale);
            updateLimbs(0, time);
            updateLimbsMatrix();
            setLimbsVisibility(true);
            visible = false;
            break;
        }

        case 1: //run
        {
            setScale(0.027f);
            int index = (int)(time / 3.23f);
            models = &SA2SonicModel::modelsDash.at(index);
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        case 3: //stomp
        {
            setScale(0.27f);
            models = &SA2SonicModel::modelStomp;
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        case 8: //skid
        {
            setScale(0.27f);
            models = &SA2SonicModel::modelSkid;
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        case 11: //hitstun
        {
            updateLimbs(11, 0);
            updateLimbsMatrix();
            if (fmodf(time, 0.1f) > 0.05f)
            {
                setLimbsVisibility(true);
            }
            else
            {
                setLimbsVisibility(false);
            }
            visible = false;
            break;
        }

        case 12: //jump
        {
            setScale(0.32f);
            models = &SA2SonicModel::modelJump;
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        case 14: //dab
        {
            Vector3f off = currentUpDirection.scaleCopy(limbsScale*displayHeightOffset);
            Vector3f pos = position + off;
            myBody->setBaseOrientation(pos.x, pos.y, pos.z, rotX, rotY, rotZ, rotRoll, limbsScale);
            updateLimbs(14, time);
            updateLimbsMatrix();
            setLimbsVisibility(true);
            visible = false;
            break;
        }

        case 15: //jog
        {
            setScale(0.027f);
            int index = (int)(time / 3.23f);
            models = &SA2SonicModel::modelsDash.at(index);
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;

            /*setScale(0.27f);
            int index = (int)(time / 5.55555555f);
            switch (index)
            {
                case 0:  models = &SA2SonicModel::modelJog0;  break;
                case 1:  models = &SA2SonicModel::modelJog1;  break;
                case 2:  models = &SA2SonicModel::modelJog2;  break;
                case 3:  models = &SA2SonicModel::modelJog3;  break;
                case 4:  models = &SA2SonicModel::modelJog4;  break;
                case 5:  models = &SA2SonicModel::modelJog5;  break;
                case 6:  models = &SA2SonicModel::modelJog6;  break;
                case 7:  models = &SA2SonicModel::modelJog7;  break;
                case 8:  models = &SA2SonicModel::modelJog8;  break;
                case 9:  models = &SA2SonicModel::modelJog9;  break;
                case 10: models = &SA2SonicModel::modelJog10; break;
                case 11: models = &SA2SonicModel::modelJog11; break;
                case 12: models = &SA2SonicModel::modelJog12; break;
                case 13: models = &SA2SonicModel::modelJog13; break;
                case 14: models = &SA2SonicModel::modelJog14; break;
                case 15: models = &SA2SonicModel::modelJog15; break;
                case 16: models = &SA2SonicModel::modelJog16; break;
                case 17: models = &SA2SonicModel::modelJog17; break;
                default: std::fprintf(stdout, "warning: jog animation index out of bounds\n"); break;
            }
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;*/
        }

        case 18: //lightdash
        {
            setScale(0.27f);
            models = &SA2SonicModel::modelLightdash;
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        case 19: //die
        {
            updateLimbs(19, 0);
            updateLimbsMatrix();
            setLimbsVisibility(true);
            visible = false;
            break;
        }

        case 21: //freefall
        {
            setScale(0.27f);
            models = &SA2SonicModel::modelFreefall;
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        case 25: //grab
        {
            setScale(0.27f);
            models = &SA2SonicModel::modelGrab;
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        case 26: //grind
        {
            setScale(0.27f);
            models = &SA2SonicModel::modelGrind;
            updateTransformationMatrix();
            setLimbsVisibility(false);
            visible = true;
            break;
        }

        default:
            break;
    }
}

void SA2SonicModel::setOrientation(float x, float y, float z, float xRot, float yRot, float zRot, float spinRot, Vector3f* newUp)
{
    position.x = x;
    position.y = y;
    position.z = z;
    rotX       = xRot;
    rotY       = yRot;
    rotZ       = zRot;
    rotRoll    = spinRot;
    currentUpDirection.set(newUp);
    myBody->setBaseOrientation(x, y, z, rotX, rotY, rotZ, rotRoll, limbsScale);
}

void SA2SonicModel::setBaseColor(float r, float g, float b)
{
                    baseColour.set(r, g, b);
    myBody        ->baseColour.set(r, g, b);
    myHead        ->baseColour.set(r, g, b);
    myLeftHumerus ->baseColour.set(r, g, b);
    myLeftForearm ->baseColour.set(r, g, b);
    myLeftHand    ->baseColour.set(r, g, b);
    myLeftThigh   ->baseColour.set(r, g, b);
    myLeftShin    ->baseColour.set(r, g, b);
    myLeftFoot    ->baseColour.set(r, g, b);
    myRightHumerus->baseColour.set(r, g, b);
    myRightForearm->baseColour.set(r, g, b);
    myRightHand   ->baseColour.set(r, g, b);
    myRightThigh  ->baseColour.set(r, g, b);
    myRightShin   ->baseColour.set(r, g, b);
    myRightFoot   ->baseColour.set(r, g, b);
}

void SA2SonicModel::setBaseAlpha(float a)
{
                    baseAlpha = a;
    myBody        ->baseAlpha = a;
    myHead        ->baseAlpha = a;
    myLeftHumerus ->baseAlpha = a;
    myLeftForearm ->baseAlpha = a;
    myLeftHand    ->baseAlpha = a;
    myLeftThigh   ->baseAlpha = a;
    myLeftShin    ->baseAlpha = a;
    myLeftFoot    ->baseAlpha = a;
    myRightHumerus->baseAlpha = a;
    myRightForearm->baseAlpha = a;
    myRightHand   ->baseAlpha = a;
    myRightThigh  ->baseAlpha = a;
    myRightShin   ->baseAlpha = a;
    myRightFoot   ->baseAlpha = a;
}

void SA2SonicModel::setRenderOrder(char newOrder)
{
                    renderOrder = newOrder;
    myBody        ->renderOrder = newOrder;
    myHead        ->renderOrder = newOrder;
    myLeftHumerus ->renderOrder = newOrder;
    myLeftForearm ->renderOrder = newOrder;
    myLeftHand    ->renderOrder = newOrder;
    myLeftThigh   ->renderOrder = newOrder;
    myLeftShin    ->renderOrder = newOrder;
    myLeftFoot    ->renderOrder = newOrder;
    myRightHumerus->renderOrder = newOrder;
    myRightForearm->renderOrder = newOrder;
    myRightHand   ->renderOrder = newOrder;
    myRightThigh  ->renderOrder = newOrder;
    myRightShin   ->renderOrder = newOrder;
    myRightFoot   ->renderOrder = newOrder;
}

std::list<TexturedModel*>* SA2SonicModel::getModels()
{
    return models;
}

void SA2SonicModel::loadStaticModels()
{

    if (SA2SonicModel::modelsDash.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading SA2 sonic static models...\n");
    #endif

    loadModel(&SA2SonicModel::modelBody,         "res/Models/Characters/ManiaSonic/", "Body");
    loadModel(&SA2SonicModel::modelHead,         "res/Models/Characters/ManiaSonic/", "Head");
    loadModel(&SA2SonicModel::modelLeftHumerus,  "res/Models/Characters/ManiaSonic/", "Humerus");
    loadModel(&SA2SonicModel::modelLeftForearm,  "res/Models/Characters/ManiaSonic/", "Forearm");
    loadModel(&SA2SonicModel::modelLeftHand,     "res/Models/Characters/ManiaSonic/", "HandLeft");
    loadModel(&SA2SonicModel::modelLeftThigh,    "res/Models/Characters/ManiaSonic/", "Thigh");
    loadModel(&SA2SonicModel::modelLeftShin,     "res/Models/Characters/ManiaSonic/", "Shin");
    loadModel(&SA2SonicModel::modelLeftFoot,     "res/Models/Characters/ManiaSonic/", "ShoeLeft");
    loadModel(&SA2SonicModel::modelRightHumerus, "res/Models/Characters/ManiaSonic/", "Humerus");
    loadModel(&SA2SonicModel::modelRightForearm, "res/Models/Characters/ManiaSonic/", "Forearm");
    loadModel(&SA2SonicModel::modelRightHand,    "res/Models/Characters/ManiaSonic/", "HandRight");
    loadModel(&SA2SonicModel::modelRightThigh,   "res/Models/Characters/ManiaSonic/", "Thigh");
    loadModel(&SA2SonicModel::modelRightShin,    "res/Models/Characters/ManiaSonic/", "Shin");
    loadModel(&SA2SonicModel::modelRightFoot,    "res/Models/Characters/ManiaSonic/", "ShoeRight");

    loadVectoredStaticModels(SA2SonicModel::modelsDash, 31, "Dash");

    loadModel(&SA2SonicModel::modelJump     , "res/Models/Characters/ManiaSonic/", "Jump");

    loadModel(&SA2SonicModel::modelGrind    , "res/Models/Characters/ManiaSonic/", "Grind");
    loadModel(&SA2SonicModel::modelSkid     , "res/Models/Characters/ManiaSonic/", "Skid");
    loadModel(&SA2SonicModel::modelLightdash, "res/Models/Characters/ManiaSonic/", "Lightdash");
    loadModel(&SA2SonicModel::modelFreefall , "res/Models/Characters/ManiaSonic/", "Freefall");
    loadModel(&SA2SonicModel::modelStomp    , "res/Models/Characters/ManiaSonic/", "Stomp");
    loadModel(&SA2SonicModel::modelGrab     , "res/Models/Characters/ManiaSonic/", "Grab");
}

void SA2SonicModel::loadVectoredStaticModels(std::vector<std::list<TexturedModel*>> &models, int numberOfModels, std::string modelName)
{
    for (int i = 0; i < 31; i++)
    {
        std::list<TexturedModel*> list;
        models.push_back(list);
        loadModel(&models[i], "res/Models/Characters/SA2Sonic/", modelName + std::to_string(i));
    }
}

void SA2SonicModel::setLimbsVisibility(bool newVisible)
{
    myBody->setVisible(newVisible);
    myHead->setVisible(newVisible);
    myLeftHumerus->setVisible(newVisible);
    myLeftForearm->setVisible(newVisible);
    myLeftHand->setVisible(newVisible);
    myLeftThigh->setVisible(newVisible);
    myLeftShin->setVisible(newVisible);
    myLeftFoot->setVisible(newVisible);
    myRightHumerus->setVisible(newVisible);
    myRightForearm->setVisible(newVisible);
    myRightHand->setVisible(newVisible);
    myRightThigh->setVisible(newVisible);
    myRightShin->setVisible(newVisible);
    myRightFoot->setVisible(newVisible);
}

void SA2SonicModel::updateLimbs(int animIndex, float time)
{
    myBody->animationIndex = animIndex;
    myHead->animationIndex = animIndex;
    myLeftHumerus->animationIndex = animIndex;
    myLeftForearm->animationIndex = animIndex;
    myLeftHand->animationIndex = animIndex;
    myLeftThigh->animationIndex = animIndex;
    myLeftShin->animationIndex = animIndex;
    myLeftFoot->animationIndex = animIndex;
    myRightHumerus->animationIndex = animIndex;
    myRightForearm->animationIndex = animIndex;
    myRightHand->animationIndex = animIndex;
    myRightThigh->animationIndex = animIndex;
    myRightShin->animationIndex = animIndex;
    myRightFoot->animationIndex = animIndex;
    myBody->update(time);
    myHead->update(time);
    myLeftHumerus->update(time);
    myLeftForearm->update(time);
    myLeftHand->update(time);
    myLeftThigh->update(time);
    myLeftShin->update(time);
    myLeftFoot->update(time);
    myRightHumerus->update(time);
    myRightForearm->update(time);
    myRightHand->update(time);
    myRightThigh->update(time);
    myRightShin->update(time);
    myRightFoot->update(time);
}

void SA2SonicModel::updateLimbsMatrix()
{
    myBody->updateTransformationMatrix();
    myHead->updateTransformationMatrix();
    myLeftHumerus->updateTransformationMatrix();
    myLeftForearm->updateTransformationMatrix();
    myLeftHand->updateTransformationMatrix();
    myLeftThigh->updateTransformationMatrix();
    myLeftShin->updateTransformationMatrix();
    myLeftFoot->updateTransformationMatrix();
    myRightHumerus->updateTransformationMatrix();
    myRightForearm->updateTransformationMatrix();
    myRightHand->updateTransformationMatrix();
    myRightThigh->updateTransformationMatrix();
    myRightShin->updateTransformationMatrix();
    myRightFoot->updateTransformationMatrix();
}

void SA2SonicModel::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting SA2 sonic static models...\n");
    #endif

    Entity::deleteModels(&SA2SonicModel::modelBody);
    Entity::deleteModels(&SA2SonicModel::modelHead);
    Entity::deleteModels(&SA2SonicModel::modelLeftHumerus);
    Entity::deleteModels(&SA2SonicModel::modelLeftForearm);
    Entity::deleteModels(&SA2SonicModel::modelLeftHand);
    Entity::deleteModels(&SA2SonicModel::modelLeftThigh);
    Entity::deleteModels(&SA2SonicModel::modelLeftShin);
    Entity::deleteModels(&SA2SonicModel::modelLeftFoot);
    Entity::deleteModels(&SA2SonicModel::modelRightHumerus);
    Entity::deleteModels(&SA2SonicModel::modelRightForearm);
    Entity::deleteModels(&SA2SonicModel::modelRightHand);
    Entity::deleteModels(&SA2SonicModel::modelRightThigh);
    Entity::deleteModels(&SA2SonicModel::modelRightShin);
    Entity::deleteModels(&SA2SonicModel::modelRightFoot);

    deleteVectoredStaticModels(SA2SonicModel::modelsDash, 31);

    Entity::deleteModels(&SA2SonicModel::modelJump);

    Entity::deleteModels(&SA2SonicModel::modelGrind);
    Entity::deleteModels(&SA2SonicModel::modelSkid);
    Entity::deleteModels(&SA2SonicModel::modelLightdash);
    Entity::deleteModels(&SA2SonicModel::modelFreefall);
    Entity::deleteModels(&SA2SonicModel::modelStomp);
    Entity::deleteModels(&SA2SonicModel::modelGrab);
}

void SA2SonicModel::deleteVectoredStaticModels(std::vector<std::list<TexturedModel*>> &models, int numberOfModels)
{
    if (&models[0] == nullptr)
    {
        for (int i = 0; i < numberOfModels; i++)
        {
            std::list<TexturedModel*> list;
            models.push_back(list);
        }
    }
    for (int i = 0; i < numberOfModels; i++)
    {
        Entity::deleteModels(&models[i]);
    }
    models.clear();
}