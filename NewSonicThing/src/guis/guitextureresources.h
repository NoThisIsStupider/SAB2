#ifndef GUITEXTURERESOURCES_H
#define GUITEXTURERESOURCES_H

#include "../renderEngine/renderEngine.h"

class GuiTexture;

class GuiTextureResources
{
public:
    static GuiTexture* textureRing;
    static GuiTexture* textureLifeIcon;
    static GuiTexture* textureBlueLine;
    static GuiTexture* textureRankA;
    static GuiTexture* textureRankB;
    static GuiTexture* textureRankC;
    static GuiTexture* textureRankD;
    static GuiTexture* textureRankE;

    static void loadGuiTextures();
};
#endif
