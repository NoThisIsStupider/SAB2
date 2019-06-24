#include "mhyellowmovingplatform.h"
#include "../entity.h"
#include "../camera.h"
#include "../../engineTester/main.h"
#include "../../models/models.h"
#include "../../renderEngine/renderEngine.h"
#include "../../objLoader/objLoader.h"
#include "../../toolbox/maths.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../animation/body.h"

#include <algorithm>

std::list<TexturedModel*> MH_YellowMovingPlatform::models;
std::list<TexturedModel*> MH_YellowMovingPlatform::modelsWheelFront;
std::list<TexturedModel*> MH_YellowMovingPlatform::modelsWheelBack;
std::list<TexturedModel*> MH_YellowMovingPlatform::modelsTransparent;
CollisionModel* MH_YellowMovingPlatform::cmOriginal;

MH_YellowMovingPlatform::MH_YellowMovingPlatform(float x, float y, float z, float rotY, int axis, float displacementMax, float speed)
{
	position.x = x;
	position.y = y;
	position.z = z;
	rotX = 0;
	this->rotY = rotY;
	rotZ = 0;

	scale = 1;
	visible = true;

    positionInitial = &position;

	this->axis = axis;
    this->displacementMax = displacementMax;
    this->speed = speed;

	setupModelWheelFront();
	setupModelWheelBack();
	setupModelTransparent();

	updateTransformationMatrix();

	collideModelOriginal = MH_YellowMovingPlatform::cmOriginal;
	collideModelTransformed = loadCollisionModel("Models/Objects/MetalHarbor/YellowMovingPlatform/", "YellowMovingPlatformCollision");

	CollisionChecker::addCollideModel(collideModelTransformed);

	updateCollisionModel();
}

void MH_YellowMovingPlatform::step() 
{
    
}

std::list<TexturedModel*>* MH_YellowMovingPlatform::getModels()
{
	return &MH_YellowMovingPlatform::models;
}

void MH_YellowMovingPlatform::loadStaticModels()
{
	if (MH_YellowMovingPlatform::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading MH_YellowMovingPlatform static models...\n");
	#endif

	loadModel(&MH_YellowMovingPlatform::models, "res/Models/Objects/MetalHarbor/YellowMovingPlatform/", "YellowMovingPlatform");
	loadModel(&MH_YellowMovingPlatform::modelsWheelFront, "res/Models/Objects/MetalHarbor/YellowMovingPlatform/", "YellowMovingPlatformWheelFront");
	loadModel(&MH_YellowMovingPlatform::modelsWheelBack, "res/Models/Objects/MetalHarbor/YellowMovingPlatform/", "YellowMovingPlatformWheelBack");
    loadModel(&MH_YellowMovingPlatform::modelsTransparent, "res/Models/Objects/MetalHarbor/YellowMovingPlatform/", "YellowMovingPlatformTransparent");


	if (MH_YellowMovingPlatform::cmOriginal == nullptr)
	{
		MH_YellowMovingPlatform::cmOriginal = loadCollisionModel("Models/Objects/MetalHarbor/YellowMovingPlatform/", "YellowMovingPlatformCollision");
	}
}

void MH_YellowMovingPlatform::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting MH_YellowMovingPlatform static models...\n");
	#endif

	Entity::deleteModels(&MH_YellowMovingPlatform::models);
	Entity::deleteModels(&MH_YellowMovingPlatform::modelsWheelFront);
	Entity::deleteModels(&MH_YellowMovingPlatform::modelsWheelBack);
	Entity::deleteModels(&MH_YellowMovingPlatform::modelsTransparent);
	Entity::deleteCollisionModel(&MH_YellowMovingPlatform::cmOriginal);
}

void MH_YellowMovingPlatform::setupModelWheelFront()
{
	modelWheelFront = new Body(&MH_YellowMovingPlatform::modelsWheelFront);
	modelWheelFront->setVisible(true);
	INCR_NEW("Entity");
	Main_addEntityPass2(modelWheelFront);
}

void MH_YellowMovingPlatform::setupModelWheelBack()
{
	modelWheelBack = new Body(&MH_YellowMovingPlatform::modelsWheelBack);
	modelWheelBack->setVisible(true);
	INCR_NEW("Entity");
	Main_addEntityPass2(modelWheelBack);
}

void MH_YellowMovingPlatform::setupModelTransparent()
{
	modelTransparent = new Body(&MH_YellowMovingPlatform::modelsTransparent);
	modelTransparent->setVisible(true);
	INCR_NEW("Entity");
	Main_addEntityPass3(modelTransparent);
}