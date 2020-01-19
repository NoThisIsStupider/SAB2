#ifndef PROPELLERSPRING
#define PROPELLERSPRING

#include <list>
#include "entity.h"
#include "collideableobject.h"

class TexturedModel;
class Body; 

class PropellerSpring : public CollideableObject
{
private: 
	static std::list<TexturedModel*> modelsBody;
	static std::list<TexturedModel*> modelsSpring;
	static std::list<TexturedModel*> modelsBlades;
	static std::list<TexturedModel*> modelsIndicatorGreen;
	static std::list<TexturedModel*> modelsIndicatorRed;
	static CollisionModel* cmOriginal;

	Body* spring = nullptr;
	Body* blades = nullptr; 
	Body* indicatorGreen = nullptr; 
	Body* indicatorRed = nullptr;

	enum STATES {
		ON_GROUND,
		MOVING_UP
	};

	int currentState;

	float globalBaseHeight;
	float globalMaxHeight;
	float currentHeightRelativeBase;
	float verticalVelocity;

	const float GRAVITY = 0.8f;

	const float SPRING_OFFSET_MAX = 12.5f;
	const float SPRING_OFFSET_HIT = 9;
	float currentSpringOffset = SPRING_OFFSET_MAX;

	float collisionCooldownTimer = 0;
	const float COLLISION_COOLDOWN_TIMER_MAX = 0.2f;
	
	float movementCooldownTimer = 0;
	const float MOVEMENT_COOLDOWN_TIMER_MAX = 1.0001f;

	//Functions for the constructor start here

	inline void setupSpring();
	inline void setupBlades();
	inline void setupIndicatorGreen();
	inline void setupIndicatorRed();

	//Functions for step start here

	inline void checkAndHandlePlayerCollisions();
	inline void setupMovementUp(bool bigVerticalMove);
	inline void updatePositions();
	inline void updateTransformations();

public: 
	PropellerSpring();
	PropellerSpring(float x, float y, float z, float maxHeight);

	void step();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();

	const bool canHomingAttackOn();

	const Vector3f getHomingCenter();
};

#endif