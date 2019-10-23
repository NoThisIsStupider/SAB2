#include <glad/glad.h>

#include "../entity.h"
#include "dpstagemanager.h"
#include "../../engineTester/main.h"
#include "../dummy.h"
#include "../controllableplayer.h"
#include "../../objLoader/objLoader.h"
#include "../../toolbox/maths.h"

std::list<TexturedModel*> DP_StageManager::modelsSkyPass1;
std::list<TexturedModel*> DP_StageManager::modelsSkyPass2;
std::list<TexturedModel*> DP_StageManager::modelsSkyTransparent;

DP_StageManager::DP_StageManager()
{
    skyPass2       = new Dummy(&DP_StageManager::modelsSkyPass2);       INCR_NEW("Entity");
    skyTransparent = new Dummy(&DP_StageManager::modelsSkyTransparent); INCR_NEW("Entity");

    visible = true;
    skyPass2->visible = true;
    skyTransparent->visible = false;

    skyPass2->renderOrder = 1;
    skyTransparent->renderOrder = 3;

    scale = 3.0f;
    skyPass2->scale = 3.0f;
    skyTransparent->scale = 1.8f;

    Main_addEntity(skyPass2);
    Main_addEntity(skyTransparent);
}

void DP_StageManager::step()
{
	//set the position of the background sky dome
	position.set(&Global::gameMainPlayer->position);
    position.y = 0;
	updateTransformationMatrix();

    skyPass2->setPosition(&position);
    skyTransparent->setPosition(&position);

    skyPass2->updateTransformationMatrix();
    skyTransparent->updateTransformationMatrix();
}

std::list<TexturedModel*>* DP_StageManager::getModels()
{
	return &DP_StageManager::modelsSkyPass1;
}

void DP_StageManager::loadStaticModels()
{
	if (DP_StageManager::modelsSkyPass1.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading DP_StageManager static models...\n");
	#endif

	loadModel(&DP_StageManager::modelsSkyPass1,       "res/Models/Levels/DelfinoPlaza/Sky/", "Blue");
    loadModel(&DP_StageManager::modelsSkyPass2,       "res/Models/Levels/DelfinoPlaza/Sky/", "Pass2");
    loadModel(&DP_StageManager::modelsSkyTransparent, "res/Models/Levels/DelfinoPlaza/Sky/", "Trans");
}

void DP_StageManager::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting DP_StageManager static models...\n");
	#endif

	Entity::deleteModels(&DP_StageManager::modelsSkyPass1);
    Entity::deleteModels(&DP_StageManager::modelsSkyPass2);
    Entity::deleteModels(&DP_StageManager::modelsSkyTransparent);
}
