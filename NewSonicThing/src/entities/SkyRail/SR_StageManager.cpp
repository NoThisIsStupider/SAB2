#include <glad/glad.h>

#include "../entity.h"
#include "srstagemanager.h"
#include "../../engineTester/main.h"
#include "../dummy.h"
#include "../controllableplayer.h"
#include "../../objLoader/objLoader.h"
#include "../../toolbox/maths.h"

std::list<TexturedModel*> SR_StageManager::modelsSkyPass1;
std::list<TexturedModel*> SR_StageManager::modelsSkyPass2;
std::list<TexturedModel*> SR_StageManager::modelsSkyPass3;
std::list<TexturedModel*> SR_StageManager::modelsSkyTransparent;

SR_StageManager::SR_StageManager()
{
	visible = true;

    skyPass2       = new Dummy(&SR_StageManager::modelsSkyPass2);       INCR_NEW("Entity");
    skyPass3       = new Dummy(&SR_StageManager::modelsSkyPass3);       INCR_NEW("Entity");
    skyTransparent = new Dummy(&SR_StageManager::modelsSkyTransparent); INCR_NEW("Entity");

    skyPass2->visible = true;
    skyPass3->visible = true;
    skyTransparent->visible = true;

    skyPass2->renderOrder = 1;
    skyPass3->renderOrder = 2;
    skyTransparent->renderOrder = 3;

    Main_addEntity(skyPass2);
    Main_addEntity(skyPass3);
    Main_addEntity(skyTransparent);
}

void SR_StageManager::step()
{
	//set the position of the background sky dome
	position.set(&Global::gameMainPlayer->position);
	updateTransformationMatrix();

    skyPass2->setPosition(&position);
    skyPass3->setPosition(&position);
    skyTransparent->setPosition(&position);

    skyPass2->updateTransformationMatrix();
    skyPass3->updateTransformationMatrix();
    skyTransparent->updateTransformationMatrix();
}

std::list<TexturedModel*>* SR_StageManager::getModels()
{
	return &SR_StageManager::modelsSkyPass1;
}

void SR_StageManager::loadStaticModels()
{
	if (SR_StageManager::modelsSkyPass1.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading SR_StageManager static models...\n");
	#endif

	loadModel(&SR_StageManager::modelsSkyPass1,       "res/Models/Levels/SkyRail/Sky/", "Pass1");
    loadModel(&SR_StageManager::modelsSkyPass2,       "res/Models/Levels/SkyRail/Sky/", "Pass2");
    loadModel(&SR_StageManager::modelsSkyPass3,       "res/Models/Levels/SkyRail/Sky/", "Pass3");
    loadModel(&SR_StageManager::modelsSkyTransparent, "res/Models/Levels/SkyRail/Sky/", "Transparent");
}

void SR_StageManager::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting SRStageManager static models...\n");
	#endif

	Entity::deleteModels(&SR_StageManager::modelsSkyPass1);
    Entity::deleteModels(&SR_StageManager::modelsSkyPass2);
    Entity::deleteModels(&SR_StageManager::modelsSkyPass3);
    Entity::deleteModels(&SR_StageManager::modelsSkyTransparent);
}
