#include <glad/glad.h>

#include "entity.h"
#include "../models/models.h"
#include "../toolbox/vector.h"
#include "maniasonicmodel.h"
#include "../renderEngine/renderEngine.h"
#include "../objLoader/objLoader.h"
#include "../engineTester/main.h"
#include "../entities/car.h"
#include "../toolbox/maths.h"

#include <list>
#include <iostream>
#include <algorithm>
#include <cmath>

std::list<TexturedModel*> ManiaSonicModel::modelDash0;
std::list<TexturedModel*> ManiaSonicModel::modelDash1;
std::list<TexturedModel*> ManiaSonicModel::modelDash2;
std::list<TexturedModel*> ManiaSonicModel::modelDash3;
std::list<TexturedModel*> ManiaSonicModel::modelDash4;
std::list<TexturedModel*> ManiaSonicModel::modelDash5;
std::list<TexturedModel*> ManiaSonicModel::modelDash6;
std::list<TexturedModel*> ManiaSonicModel::modelDash7;
std::list<TexturedModel*> ManiaSonicModel::modelDash8;
std::list<TexturedModel*> ManiaSonicModel::modelDash9;
std::list<TexturedModel*> ManiaSonicModel::modelDash10;
std::list<TexturedModel*> ManiaSonicModel::modelDash11;
std::list<TexturedModel*> ManiaSonicModel::modelJump;
std::list<TexturedModel*> ManiaSonicModel::modelJog0;
std::list<TexturedModel*> ManiaSonicModel::modelJog1;
std::list<TexturedModel*> ManiaSonicModel::modelJog2;
std::list<TexturedModel*> ManiaSonicModel::modelJog3;
std::list<TexturedModel*> ManiaSonicModel::modelJog4;
std::list<TexturedModel*> ManiaSonicModel::modelJog5;
std::list<TexturedModel*> ManiaSonicModel::modelJog6;
std::list<TexturedModel*> ManiaSonicModel::modelJog7;
std::list<TexturedModel*> ManiaSonicModel::modelJog8;
std::list<TexturedModel*> ManiaSonicModel::modelJog9;
std::list<TexturedModel*> ManiaSonicModel::modelJog10;
std::list<TexturedModel*> ManiaSonicModel::modelJog11;
std::list<TexturedModel*> ManiaSonicModel::modelJog12;
std::list<TexturedModel*> ManiaSonicModel::modelJog13;
std::list<TexturedModel*> ManiaSonicModel::modelJog14;
std::list<TexturedModel*> ManiaSonicModel::modelJog15;
std::list<TexturedModel*> ManiaSonicModel::modelJog16;
std::list<TexturedModel*> ManiaSonicModel::modelJog17;
std::list<TexturedModel*> ManiaSonicModel::modelGrind;

ManiaSonicModel::ManiaSonicModel()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	scale = 0.27f;
	visible = false;
	models = &ManiaSonicModel::modelDash0;
}

void ManiaSonicModel::step()
{

}

void ManiaSonicModel::animate(int animIndex, float time)
{
	setScale(0.27f);
	switch (animIndex)
	{
		case 1: //run
		{
			int index = (int)(time / 8.3333333f);
			switch (index)
			{
				case 0:  models = &ManiaSonicModel::modelDash0;  break;
				case 1:  models = &ManiaSonicModel::modelDash1;  break;
				case 2:  models = &ManiaSonicModel::modelDash2;  break;
				case 3:  models = &ManiaSonicModel::modelDash3;  break;
				case 4:  models = &ManiaSonicModel::modelDash4;  break;
				case 5:  models = &ManiaSonicModel::modelDash5;  break;
				case 6:  models = &ManiaSonicModel::modelDash6;  break;
				case 7:  models = &ManiaSonicModel::modelDash7;  break;
				case 8:  models = &ManiaSonicModel::modelDash8;  break;
				case 9:  models = &ManiaSonicModel::modelDash9;  break;
				case 10: models = &ManiaSonicModel::modelDash10; break;
				case 11: models = &ManiaSonicModel::modelDash11; break;
				default: std::fprintf(stdout, "dash animation index out of bounds"); break;
			}
			break;
		}

		case 12: //jump
		{
			models = &ManiaSonicModel::modelJump;
			setScale(0.32f);
			break;
		}

		case 15: //jog
		{
			int index = (int)(time / 5.55555555f);
			switch (index)
			{
				case 0:  models = &ManiaSonicModel::modelJog0;  break;
				case 1:  models = &ManiaSonicModel::modelJog1;  break;
				case 2:  models = &ManiaSonicModel::modelJog2;  break;
				case 3:  models = &ManiaSonicModel::modelJog3;  break;
				case 4:  models = &ManiaSonicModel::modelJog4;  break;
				case 5:  models = &ManiaSonicModel::modelJog5;  break;
				case 6:  models = &ManiaSonicModel::modelJog6;  break;
				case 7:  models = &ManiaSonicModel::modelJog7;  break;
				case 8:  models = &ManiaSonicModel::modelJog8;  break;
				case 9:  models = &ManiaSonicModel::modelJog9;  break;
				case 10: models = &ManiaSonicModel::modelJog10; break;
				case 11: models = &ManiaSonicModel::modelJog11; break;
				case 12: models = &ManiaSonicModel::modelJog12; break;
				case 13: models = &ManiaSonicModel::modelJog13; break;
				case 14: models = &ManiaSonicModel::modelJog14; break;
				case 15: models = &ManiaSonicModel::modelJog15; break;
				case 16: models = &ManiaSonicModel::modelJog16; break;
				case 17: models = &ManiaSonicModel::modelJog17; break;
				default: std::fprintf(stdout, "jog animation index out of bounds"); break;
			}
			break;
		}

		case 26: //grind
		{
			models = &ManiaSonicModel::modelGrind;
			break;
		}

		default:
			break;
	}
	updateTransformationMatrix();
}

void ManiaSonicModel::setOrientation(float x, float y, float z, float xRot, float yRot, float zRot, float spinRot)
{
	position.x = x;
	position.y = y;
	position.z = z;
	setRotX(xRot);
	setRotY(yRot);
	setRotZ(zRot);
	setRotSpin(spinRot);
}

std::list<TexturedModel*>* ManiaSonicModel::getModels()
{
	return models;
}

void ManiaSonicModel::loadStaticModels()
{
	if (ManiaSonicModel::modelDash0.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading mania sonic static models...\n");
	#endif

	loadModel(&ManiaSonicModel::modelDash0 , "res/Models/Characters/ManiaSonic/", "Dash0");
	loadModel(&ManiaSonicModel::modelDash1 , "res/Models/Characters/ManiaSonic/", "Dash1");
	loadModel(&ManiaSonicModel::modelDash2 , "res/Models/Characters/ManiaSonic/", "Dash2");
	loadModel(&ManiaSonicModel::modelDash3 , "res/Models/Characters/ManiaSonic/", "Dash3");
	loadModel(&ManiaSonicModel::modelDash4 , "res/Models/Characters/ManiaSonic/", "Dash4");
	loadModel(&ManiaSonicModel::modelDash5 , "res/Models/Characters/ManiaSonic/", "Dash5");
	loadModel(&ManiaSonicModel::modelDash6 , "res/Models/Characters/ManiaSonic/", "Dash6");
	loadModel(&ManiaSonicModel::modelDash7 , "res/Models/Characters/ManiaSonic/", "Dash7");
	loadModel(&ManiaSonicModel::modelDash8 , "res/Models/Characters/ManiaSonic/", "Dash8");
	loadModel(&ManiaSonicModel::modelDash9 , "res/Models/Characters/ManiaSonic/", "Dash9");
	loadModel(&ManiaSonicModel::modelDash10, "res/Models/Characters/ManiaSonic/", "Dash10");
	loadModel(&ManiaSonicModel::modelDash11, "res/Models/Characters/ManiaSonic/", "Dash11");
	loadModel(&ManiaSonicModel::modelJump  , "res/Models/Characters/ManiaSonic/", "Jump");
	loadModel(&ManiaSonicModel::modelJog0  , "res/Models/Characters/ManiaSonic/", "Jog0");
	loadModel(&ManiaSonicModel::modelJog1  , "res/Models/Characters/ManiaSonic/", "Jog1");
	loadModel(&ManiaSonicModel::modelJog2  , "res/Models/Characters/ManiaSonic/", "Jog2");
	loadModel(&ManiaSonicModel::modelJog3  , "res/Models/Characters/ManiaSonic/", "Jog3");
	loadModel(&ManiaSonicModel::modelJog4  , "res/Models/Characters/ManiaSonic/", "Jog4");
	loadModel(&ManiaSonicModel::modelJog5  , "res/Models/Characters/ManiaSonic/", "Jog5");
	loadModel(&ManiaSonicModel::modelJog6  , "res/Models/Characters/ManiaSonic/", "Jog6");
	loadModel(&ManiaSonicModel::modelJog7  , "res/Models/Characters/ManiaSonic/", "Jog7");
	loadModel(&ManiaSonicModel::modelJog8  , "res/Models/Characters/ManiaSonic/", "Jog8");
	loadModel(&ManiaSonicModel::modelJog9  , "res/Models/Characters/ManiaSonic/", "Jog9");
	loadModel(&ManiaSonicModel::modelJog10 , "res/Models/Characters/ManiaSonic/", "Jog10");
	loadModel(&ManiaSonicModel::modelJog11 , "res/Models/Characters/ManiaSonic/", "Jog11");
	loadModel(&ManiaSonicModel::modelJog12 , "res/Models/Characters/ManiaSonic/", "Jog12");
	loadModel(&ManiaSonicModel::modelJog13 , "res/Models/Characters/ManiaSonic/", "Jog13");
	loadModel(&ManiaSonicModel::modelJog14 , "res/Models/Characters/ManiaSonic/", "Jog14");
	loadModel(&ManiaSonicModel::modelJog15 , "res/Models/Characters/ManiaSonic/", "Jog15");
	loadModel(&ManiaSonicModel::modelJog16 , "res/Models/Characters/ManiaSonic/", "Jog16");
	loadModel(&ManiaSonicModel::modelJog17 , "res/Models/Characters/ManiaSonic/", "Jog17");
	loadModel(&ManiaSonicModel::modelGrind , "res/Models/Characters/ManiaSonic/", "Grind");
}

void ManiaSonicModel::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting mania sonic static models...\n");
	#endif

	Entity::deleteModels(&ManiaSonicModel::modelDash0);
	Entity::deleteModels(&ManiaSonicModel::modelDash1);
	Entity::deleteModels(&ManiaSonicModel::modelDash2);
	Entity::deleteModels(&ManiaSonicModel::modelDash3);
	Entity::deleteModels(&ManiaSonicModel::modelDash4);
	Entity::deleteModels(&ManiaSonicModel::modelDash5);
	Entity::deleteModels(&ManiaSonicModel::modelDash6);
	Entity::deleteModels(&ManiaSonicModel::modelDash7);
	Entity::deleteModels(&ManiaSonicModel::modelDash8);
	Entity::deleteModels(&ManiaSonicModel::modelDash9);
	Entity::deleteModels(&ManiaSonicModel::modelDash10);
	Entity::deleteModels(&ManiaSonicModel::modelDash11);
	Entity::deleteModels(&ManiaSonicModel::modelJump);
	Entity::deleteModels(&ManiaSonicModel::modelJog0);
	Entity::deleteModels(&ManiaSonicModel::modelJog1);
	Entity::deleteModels(&ManiaSonicModel::modelJog2);
	Entity::deleteModels(&ManiaSonicModel::modelJog3);
	Entity::deleteModels(&ManiaSonicModel::modelJog4);
	Entity::deleteModels(&ManiaSonicModel::modelJog5);
	Entity::deleteModels(&ManiaSonicModel::modelJog6);
	Entity::deleteModels(&ManiaSonicModel::modelJog7);
	Entity::deleteModels(&ManiaSonicModel::modelJog8);
	Entity::deleteModels(&ManiaSonicModel::modelJog9);
	Entity::deleteModels(&ManiaSonicModel::modelJog10);
	Entity::deleteModels(&ManiaSonicModel::modelJog11);
	Entity::deleteModels(&ManiaSonicModel::modelJog12);
	Entity::deleteModels(&ManiaSonicModel::modelJog13);
	Entity::deleteModels(&ManiaSonicModel::modelJog14);
	Entity::deleteModels(&ManiaSonicModel::modelJog15);
	Entity::deleteModels(&ManiaSonicModel::modelJog16);
	Entity::deleteModels(&ManiaSonicModel::modelJog17);
	Entity::deleteModels(&ManiaSonicModel::modelGrind);
}
