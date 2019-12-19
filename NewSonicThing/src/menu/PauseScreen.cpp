#include "pausescreen.h"
#include "../engineTester/main.h"

#include <cmath>
#include <algorithm>

#include "../fontMeshCreator/guitext.h"
#include "../fontMeshCreator/fonttype.h"
#include "../renderEngine/renderEngine.h"
#include "../toolbox/input.h"
#include "../toolbox/levelloader.h"
#include "../fontMeshCreator/guitext.h"
#include "../audio/audioplayer.h"
#include "../audio/source.h"
#include "../particles/particle.h"
#include "../particles/particleresources.h"
#include "../particles/particletexture.h"
#include "../entities/camera.h"
#include "../particles/particlemaster.h"
#include "../guis/guitextureresources.h"
#include "../guis/guimanager.h"
#include "hud.h"
#include "timer.h"

PauseScreen::PauseScreen()
{
    //Pause all sound effects
    for (int i = 0; i < 14; i++)
    {
        if (AudioPlayer::getSource(i)->isPlaying())
        {
            PauseScreen::pausedSounds[i] = true;
            AudioPlayer::getSource(i)->pause();
        }
        else
        {
            PauseScreen::pausedSounds[i] = false;
        }
    }

    Global::gameState = STATE_PAUSED;

    textResume  = new GUIText("Resume",  size, Global::fontVipnagorgialla, 0.5f, 0.35f, 4, true); INCR_NEW("GUIText");
    textRestart = new GUIText("Restart", size, Global::fontVipnagorgialla, 0.5f, 0.45f, 4, true); INCR_NEW("GUIText");

    if (Global::isAutoCam)
    {
        textCamera = new GUIText("Auto Cam", size, Global::fontVipnagorgialla, 0.5f, 0.55f, 4, true); INCR_NEW("GUIText");
    }
    else
    {
        textCamera = new GUIText("Free Cam", size, Global::fontVipnagorgialla, 0.5f, 0.55f, 4, true); INCR_NEW("GUIText");
    }

    textQuit = new GUIText("Quit", size, Global::fontVipnagorgialla, 0.5f, 0.65f, 4, true); INCR_NEW("GUIText");
}

PauseScreen::~PauseScreen()
{
    textResume ->deleteMe(); delete textResume;  INCR_DEL("GUIText");
    textRestart->deleteMe(); delete textRestart; INCR_DEL("GUIText");
    textCamera ->deleteMe(); delete textCamera;  INCR_DEL("GUIText");
    textQuit   ->deleteMe(); delete textQuit;    INCR_DEL("GUIText");
}

void PauseScreen::setVisible(bool visible)
{
    textResume ->setVisibility(visible);
    textRestart->setVisibility(visible);
    textCamera ->setVisibility(visible);
    textQuit   ->setVisibility(visible);
}

void PauseScreen::selectButton()
{
    textResume ->setColour(0.5f, 0.5f, 0.5f);
    textRestart->setColour(0.5f, 0.5f, 0.5f);
    textCamera ->setColour(0.5f, 0.5f, 0.5f);
    textQuit   ->setColour(0.5f, 0.5f, 0.5f);

    switch (menuSelection)
    {
        case 0: textResume ->setColour(1.0f, 1.0f, 1.0f); break;
        case 1: textRestart->setColour(1.0f, 1.0f, 1.0f); break;
        case 2: textCamera ->setColour(1.0f, 1.0f, 1.0f); break;
        case 3: textQuit   ->setColour(1.0f, 1.0f, 1.0f); break;
        default: break;
    }
}

Menu* PauseScreen::step()
{
    Menu* retVal = nullptr;

    int moveY = Input::inputs.MENU_Y;
    bool pressedSelect = false;

    if (moveYPrevious != moveY)
    {
        int selectionBefore = menuSelection;
        menuSelection += moveY;
        menuSelection = std::max(0, std::min(menuSelectionMAX, menuSelection));

        if (selectionBefore != menuSelection)
        {
            AudioPlayer::play(36, Global::gameCamera->getFadePosition1());
        }
    }

    if (Input::inputs.INPUT_ACTION1 && !Input::inputs.INPUT_PREVIOUS_ACTION1)
    {
        pressedSelect = true;
    }

    if (Input::inputs.INPUT_START && !Input::inputs.INPUT_PREVIOUS_START)
    {
        pressedSelect = true;
        menuSelection = 0;
    }
        
    selectButton();

    //Add life and ring icon since the HUD is no longer being drawn
    GuiManager::addGuiToRender(GuiTextureResources::textureLifeIcon);
    GuiManager::addGuiToRender(GuiTextureResources::textureRing);

    if (pressedSelect)
    {
        switch (menuSelection)
        {
        case 0:
        {
            // Pop menu. Unpause.
            retVal = PopMenu::get();

            Global::gameState = STATE_RUNNING;

            //resume all sound effects
            for (int i = 0; i < 14; i++)
            {
                if (PauseScreen::pausedSounds[i])
                {
                    AudioPlayer::getSource(i)->continuePlaying();
                }
                PauseScreen::pausedSounds[i] = false;
            }
            break;
        }

        case 1:
        {
            if (Global::gameLives > 0)
            {
                Global::shouldLoadLevel = true;
                Vector3f vel(0, 0, 0);
                ParticleMaster::createParticle(ParticleResources::textureBlackFade, Global::gameCamera->getFadePosition1(), &vel, 0, 1.0f, 0.0f, 50.0f, 0, 50.0f, 0, true, false, false);
                Global::gameState = STATE_RUNNING;
                retVal = PopMenu::get();
                AudioPlayer::stopAllSFX();
                for (int i = 0; i < 14; i++)
                {
                    PauseScreen::pausedSounds[i] = false;
                }
            }
            break;
        }

        case 2:
        {
            //switch cam, reload text
            if (textCamera != nullptr)
            {
                textCamera->deleteMe(); delete textCamera; textCamera = nullptr; INCR_DEL("GUIText");
                if (Global::isAutoCam)
                {
                    textCamera = new GUIText("Free Cam", size, Global::fontVipnagorgialla, 0.5f, 0.55f, 4, true); INCR_NEW("GUIText");
                    Global::isAutoCam = false;
                }
                else
                {
                    textCamera = new GUIText("Auto Cam", size, Global::fontVipnagorgialla, 0.5f, 0.55f, 4, true); INCR_NEW("GUIText");
                    Global::isAutoCam = true;
                }
            }
            break;
        }

        case 3:
        {
            Vector3f vel(0, 0, 0);
            ParticleMaster::createParticle(ParticleResources::textureBlackFade, Global::gameCamera->getFadePosition1(), &vel, 0, 1.0f, 0.0f, 50.0f, 0, true, false, 1.0f, false);
            LevelLoader::loadTitle();
            retVal = ClearStack::get();
            AudioPlayer::stopAllSFX();
            for (int i = 0; i < 14; i++)
            {
                PauseScreen::pausedSounds[i] = false;
            }
            break;
        }

        default:
            break;
        }

        AudioPlayer::play(37, Global::gameCamera->getFadePosition1());
    }

    moveYPrevious = moveY;

    return retVal;
}
