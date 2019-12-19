#ifndef MISSIONMENU_H
#define MISSIONMENU_H

class Button;
class GuiTexture;

#include <glad/glad.h>
#include <vector>
#include "menu.h"

class MissionMenu : public Menu
{
private:
    GLuint textureParallelogram              = GL_NONE;
    GLuint textureParallelogramBackdrop      = GL_NONE;
    GLuint textureParallelogramHalf          = GL_NONE;
    GLuint textureParallelogramHalfBackdrop  = GL_NONE;
    GLuint textureParallelogramHalf2         = GL_NONE;
    GLuint textureParallelogramHalf2Backdrop = GL_NONE;
    GLuint textureRankA  = GL_NONE;
    GLuint textureRankB  = GL_NONE;
    GLuint textureRankC  = GL_NONE;
    GLuint textureRankD  = GL_NONE;
    GLuint textureRankE  = GL_NONE;
    GLuint textureBlank  = GL_NONE;
    GLuint textureSelect = GL_NONE;

    int moveXPrevious = 0;
    int moveYPrevious = 0;
    bool selectInputPrevious = false;
    bool backInputPrevious = false;

    float holdUpTimer = 0;
    float holdDownTimer = 0;

    int currLevel = 0;
    std::vector<Button*> levelButtons;

    Button* missionButton = nullptr;
    GuiTexture* rankM1 = nullptr;
    GuiTexture* rankM2 = nullptr;
    GuiTexture* rankM3 = nullptr;
    GuiTexture* rankM4 = nullptr;
    GuiTexture* missionSelect = nullptr;

    Button* timeButton  = nullptr;
    Button* scoreButton = nullptr;

    bool visible = false;

    // Animation
    float offsetTarget = 0;
    float offsetCurr = 0;

private:
    void draw(bool updateMissionText);
    void setVisible(bool visibleStatus);

public:
    MissionMenu();
    ~MissionMenu();
    void loadResources();
    void unloadResources();
    Menu* step();
    
    float fontSize;
};

#endif
