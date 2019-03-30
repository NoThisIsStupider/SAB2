#ifndef HUD_H
#define HUD_H

class GUINumber;
class GUIText;
class Timer;

#include "menu.h"

class HUD : public Menu
{
private:
	float aspectRatio;
	float safeAreaX, safeAreaY;
	float s;
	float w;
	float speedometer;
	float speedometerScale;

	GUINumber* numberLives;
	GUINumber* numberSpeed;
	GUIText* textSpeedUnits;
	Timer* timer;

public:
	HUD();
	~HUD();
	void draw();
	Menu* step();
	Timer* getTimer();
};

#endif
