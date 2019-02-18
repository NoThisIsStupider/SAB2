#include <glad/glad.h>

#include "./entity.h"
#include "../models/models.h"
#include "../toolbox/vector.h"
#include "rocket.h"
#include "../renderEngine/renderEngine.h"
#include "../objLoader/objLoader.h"
#include "../engineTester/main.h"
#include "../entities/car.h"
#include "../toolbox/maths.h"
#include "../animation/body.h"
#include "../entities/camera.h"
#include "./camera.h"
#include "./point.h"
#include "../collision/collisionmodel.h"
#include "../collision/collisionchecker.h"
#include "../audio/audioplayer.h"
#include "../audio/source.h"
#include "../particles/particle.h"
#include "../particles/particleresources.h"

#include <list>
#include <iostream>
#include <algorithm>
#include <cmath>

extern float dt;

std::list<TexturedModel*> Rocket::modelsRocket;
std::list<TexturedModel*> Rocket::modelsBase;
CollisionModel* Rocket::cmBase;

Rocket::Rocket(int point1ID, int point2ID)
{
    //get the positions of the start and end points
    pointPositionStart = getPointPosition(point1ID);
    pointPositionEnd = getPointPosition(point2ID);

    //set some variables to their initial values
    position = pointPositionStart;
	canActivate = true;
	isActive = false;
	percentOfPathCompleted = 0;
	startupTimer = STARTUP_TIMER_INITIAL_VALUE;

	rocketAppearSoundPlayed = false; //these are outside that function so it can also be used for the end of the rocket's launch
    visible = true;

    setupRocketBase();

	position.y += 10; //place the rocket above the base of the rocket

    //for rotating the rocket to face the end, as well as for the actual movement
	rocketPathPositionDifference = pointPositionEnd - position;

	rotY = calculateRocketYRotation();
	rotZ = calculateRocketZRotation();
	base->setRotY(rotY);

	rocketPathPositionDifferenceNormalized = rocketPathPositionDifference;
	rocketPathPositionDifferenceNormalized.normalize();

    rocketPathPositionDifferenceLength = rocketPathPositionDifference.length();

    //update transforms for both models and the collision since they were changed
    updateTransformationMatrix();
    cmBase->transformModel(collideModelTransformed, base->getPosition());
	base->updateTransformationMatrix();
}

void Rocket::step()
{
    if (abs(getX() - Global::gameCamera->eye.x) > ENTITY_RENDER_DIST) //not within visible range on the x-axis
	{
		setVisible(false);
		base->setVisible(false);
	}
	else
	{
		if (abs(getZ() - Global::gameCamera->eye.z) > ENTITY_RENDER_DIST)//not within visible range on the z-axis
		{
			setVisible(false);
			base->setVisible(false);
		}
		else //is within visible range
		{
            setVisible(true);
            base->setVisible(true);

			//The players current position as of this frame
            Vector3f playerPos = Global::gameMainVehicle->getPosition();

			playerToRocketPositionDifference = playerPos - position;

			playerToRocketPositionDifferenceHorizontalSquared = getPlayerToRocketDifferenceHorizontalSquared();

			if (!rocketAppearSoundPlayed && playerWithinAppearSoundRange())
			{
				rocketAppearSoundPlayed = true;
				rocketAudioSource = AudioPlayer::play(54, getPosition(), 1, false);
			}
			else if (playerOutsideAppearSoundResetRange())
			{
				rocketAppearSoundPlayed = false;
				rocketAudioSource = nullptr;
			}

			if (!isActive && canActivate && playerWithinRocketHitbox())
			{
				//activate rocket
				isActive = true;
				canActivate = false;

				//sound of the rocket launching
				rocketAudioSource = AudioPlayer::play(55, getPosition(), 1, false);
			}

			if (isActive)
			{
				playRocketLaunchSound();

				setPlayerVariablesRocketActive();

				if (!rocketStartedMoving()) //rocket is starting up
				{
					makeDirtParticles(PARTICLE_POSITION_OFFSET_ROCKET_STARTUP);

					setPlayerPositionToHoldRocketHandle();

					startupTimer -= dt;
				}
				else //rocket is moving
				{
					makeDirtParticles(PARTICLE_POSITION_OFFSET_ROCKET_MOVING);

					calculateNewRocketPosition();

					setPlayerPositionToHoldRocketHandle();

					calculateNewPercentOfPathCompletedValue();	
				}	
			}

			if (fullPathTraveled())
			{
				//stop moving and deactivate rocket
				setPlayerVariablesRocketStopping();

				resetRocketVariables(); //reuse of function used in the constructor as well
			}

			updateTransformationMatrix();
			Global::gameMainVehicle->animate();
        }
    }
}

std::list<TexturedModel*>* Rocket::getModels()
{
	return &Rocket::modelsRocket;
}

void Rocket::loadStaticModels()
{
	if (Rocket::modelsRocket.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading Rocket static models...\n");
	#endif

	loadModel(&Rocket::modelsRocket, "res/Models/Objects/Rocket/", "Rocket");
	loadModel(&Rocket::modelsBase,   "res/Models/Objects/Rocket/", "RocketPlatform");

	if (Rocket::cmBase == nullptr)
	{
		Rocket::cmBase = loadCollisionModel("Models/Objects/Rocket/", "RocketPlatformCollision");
	}
}

void Rocket::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting Rocket static models...\n");
	#endif

	Entity::deleteModels(&Rocket::modelsRocket);
	Entity::deleteModels(&Rocket::modelsBase);
}

//functions used for the constructor start here

Vector3f Rocket::getPointPosition(int pointID)
{
    Vector3f pointPos;
    extern std::list<Entity*> gameEntitiesToAdd;
	for (auto e : gameEntitiesToAdd)
	{
		if (e->isPoint())
		{
			Point* thisPoint = (Point*)e;
			if (thisPoint->getID() == pointID)
			{
				pointPos = thisPoint->getPosition();
                break;
			}
		}
	}
    return pointPos;
}

void Rocket::setupRocketBase()
{
    base = new Body(&Rocket::modelsBase);
	base->setVisible(true);
	INCR_NEW
	Main_addEntity(base);
	base->setPosition(&position);

	collideModelOriginal = Rocket::cmBase;
	collideModelTransformed = loadCollisionModel("Models/Objects/Rocket/", "RocketPlatformCollision");
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModel();
}

float Rocket::calculateRocketYRotation()
{
    return Maths::toDegrees(atan2(-rocketPathPositionDifference.z, rocketPathPositionDifference.x));
}

float Rocket::calculateRocketZRotation()
{
    return Maths::toDegrees(atan2(rocketPathPositionDifference.y, 
            sqrt(rocketPathPositionDifference.x*rocketPathPositionDifference.x +
            -rocketPathPositionDifference.z*-rocketPathPositionDifference.z)));
}

//functions used for step() start here
float Rocket::getPlayerToRocketDifferenceHorizontalSquared()
{
	return playerToRocketPositionDifference.x*playerToRocketPositionDifference.x + playerToRocketPositionDifference.z*playerToRocketPositionDifference.z;

}

bool Rocket::playerWithinAppearSoundRange()
{
	return (playerToRocketPositionDifferenceHorizontalSquared <= pow(HITBOX_RADIUS * 30, 2)
			&& fabsf(playerToRocketPositionDifference.y) < (HITBOX_HEIGHT * 10));
}

bool Rocket::playerOutsideAppearSoundResetRange()
{
	return (playerToRocketPositionDifferenceHorizontalSquared >= pow(HITBOX_RADIUS * 150, 2)
			&& fabsf(playerToRocketPositionDifference.y) < (HITBOX_HEIGHT * 50));
}

bool Rocket::playerWithinRocketHitbox()
{
	return (playerToRocketPositionDifferenceHorizontalSquared <= pow(HITBOX_RADIUS, 2)
			&& fabsf(playerToRocketPositionDifference.y) < HITBOX_HEIGHT);
}

void Rocket::makeDirtParticles(float particlePositionOffset)
{
	int dirtToMake = 5;
	while (dirtToMake > 0)
		{
			//put the particle in the right position
			Vector3f particlePosition(getX() + (rocketPathPositionDifferenceNormalized.x * particlePositionOffset),
						 getY() + (rocketPathPositionDifferenceNormalized.y * particlePositionOffset),
						 getZ() + (rocketPathPositionDifferenceNormalized.z * particlePositionOffset));

			//setup the particle velocities so they fly in different directions to make a ball
			Vector3f particleVelocity(rocketPathPositionDifferenceNormalized);
			particleVelocity.scale(-3);
			particleVelocity.x +=   (Maths::random() - 0.5f);
			particleVelocity.y +=   (Maths::random() - 0.5f);
			particleVelocity.z +=   (Maths::random() - 0.5f);
			particlePosition.x += 4*(Maths::random() - 0.5f);
			particlePosition.y += 4*(Maths::random() - 0.5f);
			particlePosition.z += 4*(Maths::random() - 0.5f);

			//create the particle using these calculated values
			new Particle(ParticleResources::textureDust, &particlePosition, &particleVelocity, 0.08f, 60, 0, 4 * Maths::random() + 0.5f, 0, false, true);
			new Particle(ParticleResources::textureDust, &particlePosition, &particleVelocity, 0.08f, 60, 0, 4 * Maths::random() + 0.5f, 0, false, true);

			dirtToMake--;
		}
}

void Rocket::playRocketLaunchSound()
{
	if (rocketAudioSource != nullptr)
	{
		if (!rocketAudioSource->isPlaying())
		{
			rocketAudioSource = AudioPlayer::play(56, getPosition(), 1, false);
		}
	}
	else
	{
		rocketAudioSource = AudioPlayer::play(56, getPosition(), 1, false);
	}
}

void Rocket::setPlayerVariablesRocketActive()
{
	Global::gameMainVehicle->setVelocity(rocketPathPositionDifferenceNormalized.x * 1000, rocketPathPositionDifferenceNormalized.y * 1000, rocketPathPositionDifferenceNormalized.z * 1000);
	Global::gameMainVehicle->setOnRocket(true);
	Global::gameMainVehicle->setIsBall(false);
	Global::gameMainVehicle->setCanMoveTimer(1000);
	Global::gameMainVehicle->setOnGround(false);
}

bool Rocket::rocketStartedMoving()
{
	return startupTimer <= 0;
}

void Rocket::setPlayerPositionToHoldRocketHandle()
{
	Global::gameMainVehicle->setPosition(
			position.x - 6*rocketPathPositionDifferenceNormalized.x,
			position.y - 5,
			position.z - 6*rocketPathPositionDifferenceNormalized.z);
}

void Rocket::calculateNewRocketPosition()
{
	position.x = pointPositionStart.x + (rocketPathPositionDifference.x * percentOfPathCompleted);
	position.y = (pointPositionStart.y + 10) + (rocketPathPositionDifference.y * percentOfPathCompleted);
	position.z = pointPositionStart.z + (rocketPathPositionDifference.z * percentOfPathCompleted);
}

void Rocket::calculateNewPercentOfPathCompletedValue()
{
	percentOfPathCompleted += (ROCKET_SPEED * dt) / rocketPathPositionDifferenceLength;
}

bool Rocket::fullPathTraveled()
{
	return (percentOfPathCompleted >= 1);
}

void Rocket::setPlayerVariablesRocketStopping()
{
	Global::gameMainVehicle->setVelocity(rocketPathPositionDifferenceNormalized.x, rocketPathPositionDifferenceNormalized.y, rocketPathPositionDifferenceNormalized.z);
	Global::gameMainVehicle->setOnRocket(false);
	Global::gameMainVehicle->setCanMoveTimer(0);
}

void Rocket::resetRocketVariables()
{
	position = pointPositionStart;
	position.y += 10;
	canActivate = true;
	isActive = false;
	percentOfPathCompleted = 0;
	startupTimer = STARTUP_TIMER_INITIAL_VALUE;
}