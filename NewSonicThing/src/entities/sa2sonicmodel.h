#ifndef SA2SONICMODEL_H
#define SA2SONICMODEL_H

class TexturedModel;
class Limb;
class Body;
class ParticleTexture;

#include <list>
#include <vector>
#include "entity.h"
#include "playermodel.h"


class SA2SonicModel : public PlayerModel
{
private:
    std::list<TexturedModel*>* models;

    const float displayHeightOffset = 0.63f;
    const float displayBallOffset = 3.0f;

    const float limbsScale = 0.85f;

    Body* myBody         = nullptr;
    Limb* myHead         = nullptr;
    Limb* myLeftHumerus  = nullptr;
    Limb* myLeftForearm  = nullptr;
    Limb* myLeftHand     = nullptr;
    Limb* myLeftThigh    = nullptr;
    Limb* myLeftShin     = nullptr;
    Limb* myLeftFoot     = nullptr;
    Limb* myRightHumerus = nullptr;
    Limb* myRightForearm = nullptr;
    Limb* myRightHand    = nullptr;
    Limb* myRightThigh   = nullptr;
    Limb* myRightShin    = nullptr;
    Limb* myRightFoot    = nullptr;

    static std::list<TexturedModel*> modelBody;
    static std::list<TexturedModel*> modelHead;
    static std::list<TexturedModel*> modelLeftHumerus;
    static std::list<TexturedModel*> modelLeftForearm;
    static std::list<TexturedModel*> modelLeftHand;
    static std::list<TexturedModel*> modelLeftThigh;
    static std::list<TexturedModel*> modelLeftShin;
    static std::list<TexturedModel*> modelLeftFoot;
    static std::list<TexturedModel*> modelRightHumerus;
    static std::list<TexturedModel*> modelRightForearm;
    static std::list<TexturedModel*> modelRightHand;
    static std::list<TexturedModel*> modelRightThigh;
    static std::list<TexturedModel*> modelRightShin;
    static std::list<TexturedModel*> modelRightFoot;

    static std::vector<std::list<TexturedModel*>> modelsDash;

    static std::vector<std::list<TexturedModel*>> modelsJog;

    static std::list<TexturedModel*> modelJump;

    static std::list<TexturedModel*> modelGrind;
    static std::list<TexturedModel*> modelSkid;
    static std::list<TexturedModel*> modelLightdash;
    static std::list<TexturedModel*> modelFreefall;
    static std::list<TexturedModel*> modelStomp;
    static std::list<TexturedModel*> modelGrab;

    void setLimbsVisibility(bool newVisible);

    void updateLimbs(int animIndex, float time);

    void updateLimbsMatrix();

public:
    SA2SonicModel();

    void step();

    void animate(int animIndex, float time);

    void setOrientation(float x, float y, float z, float xRot, float yRot, float zRot, float spinRot, Vector3f* up);

    void setBaseColor(float r, float g, float b);

    void setBaseAlpha(float a);

    void setRenderOrder(char newOrder);

    const float getDisplayBallOffset();

    ParticleTexture* getBallTexture();

    std::list<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void loadVectoredStaticModels(std::vector<std::list<TexturedModel*>> &models, int numberOfModels, std::string modelName);

    static void deleteStaticModels();

    static void deleteVectoredStaticModels(std::vector<std::list<TexturedModel*>> &models, int numberOfModels);
};
#endif
