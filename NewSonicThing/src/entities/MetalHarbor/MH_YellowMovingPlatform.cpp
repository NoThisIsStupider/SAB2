/*
 TODO
 -Making the platform move
	-perhaps use a state machine?
	-add a displacement variable to track the distance moved, direction inspecific
	-0 is starting displacement, displacementMax is ending
	DONE
 -do the platform stop and switch directions
	DONE
 -make the platform shake when it stops
	DONE
 -Make the wheels properly rotate in the direction of movement
	-basically done, multiply by -1 in step when reversing direction of movement
	-done
 -Make sure collisions work properly
	-fix collisions on the front and back of the platfrom when moving
	-use two circles to make the collision box, easier to calculate than rectangles
	-move into place the same way as the wheels, then update positions as it moves
	-functional, could be switched to a box check instead of cylinders,
	this would require a fair bit of rewrite in the pushSonicAway function
 -Is there a sound?
	-no
*/

#include "mhyellowmovingplatform.h"
#include "../entity.h"
#include "../camera.h"
#include "../../engineTester/main.h"
#include "../../models/models.h"
#include "../../renderEngine/renderEngine.h"
#include "../../objLoader/objLoader.h"
#include "../../toolbox/maths.h"
#include "../controllableplayer.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../animation/body.h"

#include <algorithm>

extern float dt;

std::list<TexturedModel*> MH_YellowMovingPlatform::models;
std::list<TexturedModel*> MH_YellowMovingPlatform::modelsWheelFront;
std::list<TexturedModel*> MH_YellowMovingPlatform::modelsWheelBack;
std::list<TexturedModel*> MH_YellowMovingPlatform::modelsTransparent;
CollisionModel* MH_YellowMovingPlatform::cmOriginal;

MH_YellowMovingPlatform::MH_YellowMovingPlatform() {}

MH_YellowMovingPlatform::MH_YellowMovingPlatform(float x, float y, float z, float dirX, float dirZ, float displacementMax, float speed)
{
	position.x = x;
	position.y = y;
	position.z = z;
	rotX = 0;
	rotZ = 0;

	scale = 1;
	visible = true;

	platformMoves = true;
	this->speed = speed;
	this->displacementMax = displacementMax;
	displacementCurrent = 0;
	wheelMovementDirectionMultiplier = 1;
	positionInitial = &position;

	directionVector.set(dirX, 0.0f, dirZ);
	directionVector.normalize();

	rotY = Maths::toDegrees(atan2f(directionVector.x, directionVector.z));

	if (displacementMax == 0)
	{
		platformMoves = false;
	}

	stateCurrent = MOVING_FORWARD;

	setupModelWheelFront();
	setupModelWheelBack();
	setupModelTransparent();

	syncBodyPositionsAbsolute();

	updateTransformationMatrix();

	collideModelOriginal = MH_YellowMovingPlatform::cmOriginal;
	collideModelTransformed = loadCollisionModel("Models/Objects/MetalHarbor/YellowMovingPlatform/", "YellowMovingPlatformCollision");

	CollisionChecker::addCollideModel(collideModelTransformed);

	updateCollisionModel();
}

void MH_YellowMovingPlatform::step() 
{
	if (platformMoves == true)
	{	
		Vector3f movementAmount;
		switch (stateCurrent)
		{
			case MOVING_FORWARD:
				movementAmount = calculateMovementAmount(directionVector);
				position = position + movementAmount;
				displacementCurrent += movementAmount.length();
				syncBodyPositionsRelative(movementAmount);
				spinWheels();

				pushSonicAway(true, false);

				if (collideModelTransformed->playerIsOn)
				{
					movePlayer(movementAmount);
				}
				
				if (displacementCurrent >= displacementMax)
				{
					displacementCurrent = displacementMax;
					wheelMovementDirectionMultiplier *= -1;
					
					stateCurrent = STOPPED;
					positionStopped = position;
					stoppedTimer = 0;
					shakeTimer = SHAKE_TIMER_MIN;
				}
				break;				
			case MOVING_BACKWARDS:
				movementAmount = calculateMovementAmount(Vector3f() - directionVector);
				position = position + movementAmount;
				displacementCurrent -= movementAmount.length();
				syncBodyPositionsRelative(movementAmount);
				spinWheels();

				pushSonicAway(false, true);

				if (collideModelTransformed->playerIsOn)
				{
					movePlayer(movementAmount);
				}

				if (displacementCurrent <= 0)
				{
					displacementCurrent = 0;
					position = positionInitial;
					syncBodyPositionsAbsolute();
					wheelMovementDirectionMultiplier *= -1;
					
					stateCurrent = STOPPED;
					positionStopped = position;
					stoppedTimer = 0;
					shakeTimer = SHAKE_TIMER_MIN;
				}
				break;
			case STOPPED:
				stoppedTimer += dt;

				movementAmount = shakePlatform();
				position = position + movementAmount;
				syncBodyPositionsRelative(movementAmount);

				if (shakeTimer < SHAKE_TIMER_MAX && sinf(shakeTimer)/shakeTimer * 5 > 0)
				{
					pushSonicAway(true, false);
				}
				else if (shakeTimer < SHAKE_TIMER_MAX && sinf(shakeTimer)/shakeTimer * 5 < 0)
				{
					pushSonicAway(false, true);
				}

				if (collideModelTransformed->playerIsOn)
				{
					movePlayer(movementAmount);
				}

				if (stoppedTimer > 2)
				{
					if (displacementCurrent == displacementMax)
					{
						stateCurrent = MOVING_BACKWARDS;
					}
					else
					{
						stateCurrent = MOVING_FORWARD;
					}
				}
				break;
		}

		updateTransformationMatrix();
		wheelFront->updateTransformationMatrix();
		wheelBack->updateTransformationMatrix();
		bodyTransparent->updateTransformationMatrix();
		updateCollisionModel();

		Global::gameMainPlayer->animate();
		Global::gameMainPlayer->refreshCamera();
	}
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

inline void MH_YellowMovingPlatform::setupModelWheelFront()
{
	wheelFront = new Body(&MH_YellowMovingPlatform::modelsWheelFront);
	wheelFront->setVisible(true);
	INCR_NEW("Entity");
	Main_addEntity(wheelFront);
	wheelFront->renderOrder = 1;
	wheelFront->setPosition(&position);
	wheelFront->setRotY(rotY);
}

inline void MH_YellowMovingPlatform::setupModelWheelBack()
{
	wheelBack = new Body(&MH_YellowMovingPlatform::modelsWheelBack);
	wheelBack->setVisible(true);
	INCR_NEW("Entity");
	Main_addEntity(wheelBack);
	wheelBack->renderOrder = 1;
	wheelBack->setPosition(&position);
	wheelBack->setRotY(rotY);
}

inline void MH_YellowMovingPlatform::setupModelTransparent()
{
	bodyTransparent = new Body(&MH_YellowMovingPlatform::modelsTransparent);
	bodyTransparent->setVisible(true);
	INCR_NEW("Entity");
	Main_addEntity(bodyTransparent);
	bodyTransparent->renderOrder = 1;
	bodyTransparent->setRotY(rotY);
}

inline Vector3f MH_YellowMovingPlatform::calculateMovementAmount(Vector3f directionVectorLocal)
{
	return directionVectorLocal.scaleCopy(speed * dt);
}

inline void MH_YellowMovingPlatform::syncBodyPositionsRelative(Vector3f movementAmount)
{
	bodyTransparent->position = position;
	
	wheelFront->position = wheelFront->position + movementAmount;
	wheelBack->position = wheelBack->position + movementAmount;
}

inline void MH_YellowMovingPlatform::syncBodyPositionsAbsolute()
{
	bodyTransparent->position = position;

	wheelFront->position = position + directionVector.scaleCopy(WHEEL_OFFSET_FRONT_HORIZONTAL);
	wheelFront->position.y += WHEEL_OFFSET_FRONT_VERTICAL;

	wheelBack->position = position + directionVector.scaleCopy(WHEEL_OFFSET_BACK_HORIZONTAL);
	wheelBack->position.y += WHEEL_OFFSET_BACK_VERTICAL;
}

inline void MH_YellowMovingPlatform::spinWheels()
{
	wheelFront->rotX += WHEEL_SPEED_FRONT * wheelMovementDirectionMultiplier * speed/40;
	wheelBack->rotX += WHEEL_SPEED_BACK * wheelMovementDirectionMultiplier * speed/40;
}

inline void MH_YellowMovingPlatform::movePlayer(Vector3f movementAmount)
{
	Vector3f newPlayerPos = movementAmount + Global::gameMainPlayer->getPosition();
	Global::gameMainPlayer->position = newPlayerPos;
}

inline Vector3f MH_YellowMovingPlatform::shakePlatform()
{
	Vector3f distanceFromPositionStopped = positionStopped - position;

	if (shakeTimer > SHAKE_TIMER_MAX)
	{
		return distanceFromPositionStopped;
	}
	shakeTimer += dt * 30;
	return directionVector.scaleCopy(sinf(shakeTimer)/shakeTimer * 5) + distanceFromPositionStopped;
}

inline void MH_YellowMovingPlatform::pushSonicAway(bool frontHitboxes, bool backHitboxes)
{
	//first check if sonic is even close to the platform
	if (collisionCheckCylinder(position, 100, 100))
	{
		Vector3f rotateAround = Vector3f(0, 1, 0);
		Vector3f sidewaysVector = Maths::rotatePoint(&directionVector, &rotateAround, Maths::toRadians(90));
		Vector3f collisionCenterPos1;
		Vector3f collisionCenterPos2;
		Vector3f collisionCenterPos3;
		Vector3f collisionCenterPos4;

		//collisions on front of platform
		collisionCenterPos1 = position + directionVector.scaleCopy(COLLISION_POSITION_FORWARD) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIDEWAYS);
		collisionCenterPos2 = position + directionVector.scaleCopy(COLLISION_POSITION_FORWARD);
		collisionCenterPos3 = position + directionVector.scaleCopy(COLLISION_POSITION_FORWARD) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIDEWAYS * -1);
		if (collisionCheckCylinder(collisionCenterPos1, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos2, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos3, COLLISION_RADIUS, COLLISION_HEIGHT))
		{
			Global::gameMainPlayer->increasePosition(directionVector.x * speed/40, directionVector.y * speed/40, directionVector.z * speed/40);
		}

		//collisions on back of platform
		collisionCenterPos1 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_OUTER) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIDEWAYS);
		collisionCenterPos2 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_INNER) + sidewaysVector.scaleCopy(COLLISION_POSITION_BACKSIDEWAYS_INNER);
		collisionCenterPos3 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_INNER) + sidewaysVector.scaleCopy(COLLISION_POSITION_BACKSIDEWAYS_INNER * -1);
		collisionCenterPos4 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_OUTER) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIDEWAYS * -1);
		if (collisionCheckCylinder(collisionCenterPos1, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos2, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos3, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos4, COLLISION_RADIUS, COLLISION_HEIGHT))
		{
			Global::gameMainPlayer->increasePosition(directionVector.x * -speed/40, directionVector.y * -speed/40, directionVector.z * -speed/40);
		}
	}	
}

inline bool MH_YellowMovingPlatform::collisionCheckCylinder(Vector3f collisionCenterPos, float hitboxRadius, float hitboxHeight)
{
	Vector3f playerPos = Global::gameMainPlayer->position;
	Vector3f playerToCenterDistance = playerPos - collisionCenterPos;
	float playerToCenterDistanceSquared = playerToCenterDistance.x * playerToCenterDistance.x + playerToCenterDistance.z * playerToCenterDistance.z;
	if (playerToCenterDistanceSquared <= hitboxRadius * hitboxRadius && fabs(playerToCenterDistance.y) < hitboxHeight)
	{
		return true;
	}
	return false;
}