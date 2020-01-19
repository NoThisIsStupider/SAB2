#include "propellerspring.h"
#include "entity.h"
#include "camera.h"
#include "../engineTester/main.h"
#include "../models/models.h"
#include "../renderEngine/renderEngine.h"
#include "../objLoader/objLoader.h"
#include "../toolbox/maths.h"
#include "controllableplayer.h"
#include "../collision/collisionmodel.h"
#include "../collision/collisionchecker.h"
#include "../animation/body.h"
#include "../audio/audioplayer.h"

#include <algorithm>

extern float dt;

std::list<TexturedModel*> PropellerSpring::modelsBody;
std::list<TexturedModel*> PropellerSpring::modelsSpring;
std::list<TexturedModel*> PropellerSpring::modelsBlades;
std::list<TexturedModel*> PropellerSpring::modelsIndicatorGreen;
std::list<TexturedModel*> PropellerSpring::modelsIndicatorRed;
CollisionModel* PropellerSpring::cmOriginal;

PropellerSpring::PropellerSpring() {}

PropellerSpring::PropellerSpring(float x, float y, float z, float maxHeight)
{
	position.x = x;
	position.y = y;
	position.z = z;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	scale = 1;
	visible = true; 
	this->globalBaseHeight = position.y;
	this->globalMaxHeight = globalBaseHeight + maxHeight;

	setupSpring();
	setupBlades();
	setupIndicatorGreen();
	setupIndicatorRed();

	updateTransformationMatrix();
	spring->updateTransformationMatrix();
	blades->updateTransformationMatrix();
	indicatorGreen->updateTransformationMatrix();
	indicatorRed->updateTransformationMatrix();

	collideModelOriginal = PropellerSpring::cmOriginal;
	collideModelTransformed = collideModelOriginal->duplicateMe();
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModel();
}

void PropellerSpring::step()
{
	if (collisionCooldownTimer > 0) collisionCooldownTimer -= dt;
	if (movementCooldownTimer  > 0) movementCooldownTimer  -= dt;

	if (currentSpringOffset > SPRING_OFFSET_MAX) //ensure the spring doesn't go too far
	{
		currentSpringOffset = SPRING_OFFSET_MAX;
		AudioPlayer::play(65, getPosition());
	}
	else if (currentSpringOffset < SPRING_OFFSET_MAX)
	{
		currentSpringOffset = Maths::interpolate(SPRING_OFFSET_HIT, SPRING_OFFSET_MAX + 0.1f, 1 - (collisionCooldownTimer / COLLISION_COOLDOWN_TIMER_MAX));
	}

	checkAndHandlePlayerCollisions();

	switch (currentState)
	{
	case ON_GROUND: //keep animating the blades here
		if (verticalVelocity < 0) verticalVelocity += GRAVITY * dt;
		else if (verticalVelocity > 0) verticalVelocity = 0;
		blades->rotY += verticalVelocity * 500 * dt;

		break;
	case MOVING_UP:
		verticalVelocity -= GRAVITY * dt;
		verticalVelocity = fmaxf(-2, verticalVelocity); //make sure it doesn't start falling too fast

		currentHeightRelativeBase += verticalVelocity;

		if (globalBaseHeight + currentHeightRelativeBase + SPRING_OFFSET_MAX > globalMaxHeight)
		{
			currentHeightRelativeBase = globalMaxHeight - globalBaseHeight - SPRING_OFFSET_MAX;
		}
		else if (currentHeightRelativeBase < 0) //below the base
		{
			currentHeightRelativeBase = 0;
			AudioPlayer::play(66, getPosition());
			currentState = ON_GROUND;
		}

		blades->rotY += verticalVelocity * 500 * dt;

		break;
	}
	
	updatePositions();
	updateTransformations();
}

std::list<TexturedModel*>* PropellerSpring::getModels()
{
	return &PropellerSpring::modelsBody;
}

void PropellerSpring::loadStaticModels()
{
	if (PropellerSpring::modelsBody.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading PropellerSpring static models...\n");
	#endif

	loadModel(&PropellerSpring::modelsBody, "res/Models/Objects/PropellerSpring/", "Body");
	loadModel(&PropellerSpring::modelsSpring, "res/Models/Objects/PropellerSpring/", "Spring");
	loadModel(&PropellerSpring::modelsBlades, "res/Models/Objects/PropellerSpring/", "Blades");
	loadModel(&PropellerSpring::modelsIndicatorGreen, "res/Models/Objects/PropellerSpring/", "IndicatorGreen");
	loadModel(&PropellerSpring::modelsIndicatorRed, "res/Models/Objects/PropellerSpring/", "IndicatorRed");

	if (PropellerSpring::cmOriginal == nullptr)
	{
		PropellerSpring::cmOriginal = loadCollisionModel("Models/Objects/PropellerSpring/", "Collision");
	}
}

void PropellerSpring::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting PropellerSpring static models..\n");
	#endif

	Entity::deleteModels(&PropellerSpring::modelsBody);
	Entity::deleteModels(&PropellerSpring::modelsSpring);
	Entity::deleteModels(&PropellerSpring::modelsBlades);
	Entity::deleteModels(&PropellerSpring::modelsIndicatorGreen);
	Entity::deleteModels(&PropellerSpring::modelsIndicatorRed);
	Entity::deleteCollisionModel(&PropellerSpring::cmOriginal);
}

inline void PropellerSpring::setupSpring()
{
	spring = new Body(&PropellerSpring::modelsSpring);
	spring->setVisible(true);
	INCR_NEW("Entity");
	Main_addChunkedEntity(spring);
	spring->renderOrder = 1;
	spring->setPosition(&position);
	spring->position.y += SPRING_OFFSET_MAX;
}

inline void PropellerSpring::setupBlades()
{
	blades = new Body(&PropellerSpring::modelsBlades);
	blades->setVisible(true);
	INCR_NEW("Entity");
	Main_addChunkedEntity(blades);
	blades->renderOrder = 1;
	blades->setPosition(&position);
}

inline void PropellerSpring::setupIndicatorGreen()
{
	indicatorGreen = new Body(&PropellerSpring::modelsIndicatorGreen);
	indicatorGreen->setVisible(true);
	INCR_NEW("Entity");
	Main_addChunkedEntity(indicatorGreen);
	indicatorGreen->renderOrder = 1;
	indicatorGreen->setPosition(&position);
}

inline void PropellerSpring::setupIndicatorRed()
{
	indicatorRed = new Body(&PropellerSpring::modelsIndicatorRed);
	indicatorRed->setVisible(true);
	INCR_NEW("Entity");
	Main_addChunkedEntity(indicatorRed);
	indicatorRed->renderOrder = 1;
	indicatorRed->setPosition(&position);
	indicatorRed->position.y += globalMaxHeight;
}

const bool PropellerSpring::canHomingAttackOn()
{
	return true;
}

const Vector3f PropellerSpring::getHomingCenter()
{
	return spring->position;
}

inline void PropellerSpring::checkAndHandlePlayerCollisions()
{
	bool playerHitSpring = Maths::pointIsInSphere(Global::gameMainPlayer->getPosition(), &spring->position, 9);
	if (collisionCooldownTimer <= 0 && playerHitSpring)
	{
		bool bigVerticalMove = false;

		currentSpringOffset = SPRING_OFFSET_HIT; //make the spring pop down and then later have other stuff raise it back up

		//set the players velocity to a vertical vector scaled by the length of the old vector (aka same speed new direction), with clamping so you can't go too slow or fast
		Global::gameMainPlayer->vel = Vector3f(0.00000001f, 1, 0.000000001f).scaleCopy(Maths::fclampf(Global::gameMainPlayer->vel.length() / 2, 100, 200));

		collisionCooldownTimer = COLLISION_COOLDOWN_TIMER_MAX;

		//reset bounces and homing attacks so you can do them again
		if (Global::gameMainPlayer->isBouncing || Global::gameMainPlayer->justHomingAttacked)
		{
			if (Global::gameMainPlayer->isBouncing)
			{
				Global::gameMainPlayer->vel.y += 80;
				AudioPlayer::play(8, getPosition());
			}
			Global::gameMainPlayer->isBouncing = false;
			Global::gameMainPlayer->justHomingAttacked = false;
			bigVerticalMove = true;
			Global::gameMainPlayer->canMoveTimer = COLLISION_COOLDOWN_TIMER_MAX;
		}
		else
		{
			Global::gameMainPlayer->isJumping = false;
		}

		if (movementCooldownTimer <= 0)
		{
			setupMovementUp(bigVerticalMove);
		}
	}
}

inline void PropellerSpring::setupMovementUp(bool bigVerticalMove)
{
	if (bigVerticalMove) verticalVelocity = 1;
	else verticalVelocity = 0.25;

	AudioPlayer::play(64, getPosition());

	currentState = MOVING_UP;
}

inline void PropellerSpring::updatePositions()
{
	position.y = globalBaseHeight + currentHeightRelativeBase;
	spring->position.y = position.y + currentSpringOffset;
	blades->position.y = position.y;
}

inline void PropellerSpring::updateTransformations()
{
	updateTransformationMatrix();
	spring->updateTransformationMatrix();
	blades->updateTransformationMatrix();
	updateCMJustPosition();
}