#include <glad/glad.h>

#include "entity.h"
#include "../models/models.h"
#include "../toolbox/vector.h"
#include "ringmoving.h"
#include "ring.h"
#include "../renderEngine/renderEngine.h"
#include "../objLoader/objLoader.h"
#include "../engineTester/main.h"
#include "../entities/camera.h"
#include "../entities/playersonic.h"
#include "../collision/collisionchecker.h"
#include "../collision/triangle3d.h"
#include "../toolbox/maths.h"
#include "../audio/audioplayer.h"
#include "../particles/particle.h"
#include "../particles/particleresources.h"
#include "../particles/particlemaster.h"
#include "shieldmagnet.h"

#include <list>
#include <iostream>
#include <algorithm>

RingMoving::RingMoving()
{

}

RingMoving::RingMoving(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    moves = false;
    vel.set(0, 0, 0);
    visible = true;
    ageTimer = -1.0f;
    trackingPlayer = false;
    updateTransformationMatrix();
}

RingMoving::RingMoving(float x, float y, float z, float xVel, float yVel, float zVel)
{
    position.x = x;
    position.y = y;
    position.z = z;
    moves = true;
    vel.set(xVel, yVel, zVel);
    ageTimer = 0.0f;
    visible = true;
    trackingPlayer = false;
    updateTransformationMatrix();
}

void RingMoving::step()
{
    extern float dt;
    increaseRotation(0, 5*60*dt, 0);

    if (ageTimer < 0) //if we were spawned in to move toward blue shield
    {
        if (Global::gameMainPlayer->getShieldMagnet() != nullptr)
        {
            Vector3f diff = (*getPosition()) - Global::gameMainPlayer->getCenterPosition();
        
            if (diff.lengthSquared() < 60*60)
            {
                trackingPlayer = true;
            }
        }
        else
        {
            trackingPlayer = false;
        }
    }
    else //if we were spawned to spread out after getting hit
    {
        ageTimer += dt;

        if (moves)
        {
            vel.y -= 0.1f*60*60*dt; //gravity
            vel.scale(1 - (0.01f*60*dt)); //air friction

            if (CollisionChecker::checkCollision(
                position.x, position.y - 5.0f, position.z,
                position.x + vel.x*dt, position.y + vel.y*dt - 5.0f, position.z + vel.z*dt))
            {
                position.set(CollisionChecker::getCollidePosition());
                position.y += 5.0f;

                Vector3f normal = CollisionChecker::getCollideTriangle()->normal;
                float speed = vel.length();

                if (speed > 0.5f*60.0f)
                {
                    Vector3f bounceVel = Maths::bounceVector(&vel, &normal, 0.7f);
                    vel = bounceVel;
                    increasePosition(normal.x*0.01f, normal.y*0.01f, normal.z*0.01f);
                }
                else
                {
                    moves = false;
                }
            }
            else
            {
                position = position + vel.scaleCopy(dt);
            }
        }
    }

    if (trackingPlayer)
    {
        Vector3f diff = Global::gameMainPlayer->getCenterPosition() - position;
        diff.scale(0.35f*60*dt);
        position = position + diff;
    }

    updateTransformationMatrix();

    //flashing and dissapear
    if (ageTimer > 8.0f)
    {
        visible = (fmodf(ageTimer, 0.3f) > 0.15f);

        if (ageTimer > 12.0f)
        {
            Main_deleteEntity(this);
            return;
        }
    }

    //pick up
    if (fabsf(position.y - Global::gameMainPlayer->position.y) < 40 &&
        fabsf(position.z - Global::gameMainPlayer->position.z) < 40 &&
        fabsf(position.x - Global::gameMainPlayer->position.x) < 40 &&
        (ageTimer > 1.0f || ageTimer < 0.0f))
    {
        if ((Global::gameMainPlayer->getCenterPosition() - position).lengthSquared() < (5.0f*5.0f + 9.0f*9.0f))
        {
            AudioPlayer::play(4, getPosition());

            for (int i = 0; i < 10; i++)
            {
                Vector3f pos(
                    position.x + Maths::random() * 8 - 4,
                    position.y + Maths::random() * 8 - 4,
                    position.z + Maths::random() * 8 - 4);
            
                Vector3f partVel(0, 0.4f*60, 0);
            
                ParticleMaster::createParticle(ParticleResources::textureSparkleYellow, &pos, &partVel,
                    0.025f*60*60, 0.5f, 0, 4, -(4.0f / 0.5f), false, false, 0.5f, true);
            }

            Global::increaseRingCount(1);

            if (ageTimer < 0.0f)
            {
                Global::gameScore += 10;
            }

            Main_deleteEntity(this);
            return;
        }
    }
}

std::list<TexturedModel*>* RingMoving::getModels()
{
    return &Ring::models;
}

const inline bool RingMoving::canLightdashOn()
{
    if (ageTimer < 0.0f)
    {
        return true;
    }
    return false;
}
