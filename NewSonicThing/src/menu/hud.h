#ifndef HUD_H
#define HUD_H

class GUINumber;
class GUIText;
class Timer;

#include "menu.h"
#include "../guis/guitexture.h"
#include <glad/glad.h>

class HUD : public Menu
{
private:
	float aspectRatio;
	float safeAreaX, safeAreaY;
	float s;
	float w;
	float speedometer;
	float speedometerScale;

    GUINumber* numberFPS = nullptr;
	GUINumber* numberLives = nullptr;
    GUINumber* numberRings = nullptr;
    GUINumber* numberScore = nullptr;
	GUINumber* numberSpeed = nullptr;
	GUIText* textSpeedUnits = nullptr;
	Timer* timer = nullptr;

public:
	HUD();
	~HUD();
	void draw();
	Menu* step();
	Timer* getTimer();
};

#endif
