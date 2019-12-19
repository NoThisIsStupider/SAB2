#ifndef MAIN_H
#define MAIN_H

class Entity;
class Camera;
class Stage;
class SkySphere;
class Light;
class ShadowMapMasterRenderer;
class Fbo;
class Checkpoint;
class WaterRenderer;
class WaterFrameBuffers;
class WaterTile;
class FontType;
class GUIText;
class Timer;
class ControllablePlayer;

#include <string>
#include <random>
#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../toolbox/level.h"
#include "../menu/menumanager.h"
#include "../toolbox/vector.h"
#include "../entities/raceghost.h"

void Main_addEntity(Entity* entityToAdd);
void Main_deleteEntity(Entity* entityToDelete);
void Main_deleteAllEntites();

void Main_addChunkedEntity(Entity* entityToAdd);
void Main_deleteChunkedEntity(Entity* entityToAdd);
void Main_deleteAllChunkedEntities();

#define STATE_PAUSED 0
#define STATE_RUNNING 1
#define STATE_EXITING 2
#define STATE_CUTSCENE 3
#define STATE_TITLE 4
#define STATE_DEBUG 5

#define LVL_TUTORIAL         0
#define LVL_GREEN_FOREST     1
#define LVL_METAL_HARBOR     2
#define LVL_PYRAMID_CAVE     3
#define LVL_SKY_RAIL         4
#define LVL_RADICAL_HIGHWAY  5
#define LVL_GREEN_HILL_ZONE  6
#define LVL_CITY_ESCAPE      7
#define LVL_WINDY_VALLEY     8
#define LVL_SEASIDE_HILL     9
#define LVL_FROG_FOREST     10
#define LVL_TEST            11
#define LVL_SPEED_HIGHWAY   12
#define LVL_NOKI_BAY        13
#define LVL_DELFINO_PLAZA   14

#define DEV_MODE

#ifdef DEV_MODE
    #include <thread>
    #define INCR_NEW(NAME) Global::debugNew(NAME);
    #define INCR_DEL(NAME) Global::debugDel(NAME);
#else
    #define INCR_NEW(NAME) ;
    #define INCR_DEL(NAME) ;
#endif

class Global
{
public:
    static std::string pathToEXE;
    static MenuManager menuManager;
    static Timer* mainHudTimer;
    static Camera* gameCamera;
    static ControllablePlayer* gameMainPlayer;
    static Stage* gameStage;
    static SkySphere* gameSkySphere;
    static Light* gameLightSun;
    static Light* gameLightMoon;
    static int countNew;
    static int countDelete;
    static int gameState;
    static int levelID;
    static std::string levelName;
    static std::string levelNameDisplay;
    static bool shouldLoadLevel;
    static bool isNewLevel;
    static bool isAutoCam;
    static int gameRingCount;
    static int gameScore;
    static int gameLives;
    static float gameClock;
    static WaterRenderer* gameWaterRenderer;
    static WaterFrameBuffers* gameWaterFBOs;
    static std::vector<WaterTile*> gameWaterTiles;
    static int gameMainVehicleSpeed;
    static float startStageTimer;
    static float finishStageTimer;
    static Fbo* gameMultisampleFbo;
    static Fbo* gameOutputFbo;
    static Fbo* gameOutputFbo2;
    static float deathHeight;
    static float waterHeight;
    static int gameMissionNumber;
    static bool gameIsNormalMode;
    static bool gameIsHardMode;
    static bool gameIsChaoMode;
    static bool gameIsRingMode;
    static bool gameIsRaceMode;
    static float gameRaceTimeLimit;
    static std::string gameMissionDescription;
    static int gameRingTarget; //How many rings you have to collect to finish the stage in Ring mode
    static bool gameIsArcadeMode;
    static int gameTotalPlaytime; //in seconds
    static float gameArcadePlaytime;
    static bool stageUsesWater;
    static FontType* fontVipnagorgialla;
    static bool renderWithCulling; //if the stage uses backface culling or not
    static bool displayFPS;
    static int currentCalculatedFPS;
    static int renderCount;
    //static const int syncToDisplayEveryXFrames = 1; //if for some reason you want to render at double, triple, etc. the fps of the monitor
    //static float fpsTarget; //experiment to try to sleep to maintain a target fps
    static float fpsLimit; //experiment to try to busy wait to maintain a target fps
    static bool useFullscreen;
    static int displaySizeChanged; //This will be 1 for a single frame after the size of the window changes (set in callback)

    //static std::list<std::string> raceLog;
    static bool shouldLogRace;

    static int raceLogSize;
    static GhostFrame raceLog[];
    static const int raceLogSizeMax = 432000; //enough for 10 minutes at 720fps

    static bool spawnAtCheckpoint;
    static Vector3f checkpointPlayerPos;
    static Vector3f checkpointPlayerDir;
    static Vector3f checkpointCamDir;
    static float checkpointTime;

    //texts for the title card
    static GUIText* titleCardLevelName;
    static GUIText* titleCardMission;
    static GUIText* titleCardMissionDescription;

    static std::vector<Level> gameLevelData;
    static std::unordered_map<std::string, std::string> gameSaveData;

    static int gameArcadeIndex;
    static std::vector<int> gameArcadeLevelIds;

    //Debug display
    static bool debugDisplay;
    static bool frozen;
    static bool step;

    //Graphics settings
    static bool useHighQualityWater;
    static unsigned HQWaterReflectionWidth;
    static unsigned HQWaterReflectionHeight;
    static unsigned HQWaterRefractionWidth;
    static unsigned HQWaterRefractionHeight;

    static bool renderParticles;

    static bool renderBloom;

    static bool framerateUnlock;

    static bool renderShadowsFar;
    static bool renderShadowsClose;
    static int shadowsFarQuality;

    static bool unlockedSonicDoll;
    static bool unlockedMechaSonic;
    static bool unlockedDage4;
    static bool unlockedManiaSonic;
    static bool unlockedAmy;

    static std::vector<std::string> npcList;

    static void checkErrorAL(const char* description);

    static void loadSaveData();

    static void saveSaveData();

    static void saveGhostData();

    static int calculateRankAndUpdate();

    static void increaseRingCount(int rings);

    //Return a list of nearby entity sets. renderDistance is number of layers to go outwards.
    //0 = only the exact chunk.
    //1 = 4 chunks
    //2 = 9 chunks
    static void getNearbyEntities(float x, float z, int renderDistance, std::list<std::unordered_set<Entity*>*>* list);

    //Return a list of nearby entity sets. Returns either 1 chunk, 2 chunks, or 4 chunks,
    // depending on the minDistance value.
    static void getNearbyEntities(float x, float z, std::list<std::unordered_set<Entity*>*>* list, float minDistance);

    //Returns the index of 'gameChunkedEntities' for the (x, z) location
    static int getChunkIndex(float x, float z);

    static void recalculateEntityChunks(float minX, float maxX, float minZ, float maxZ, float chunkSize);

    static void createTitleCard();

    static void clearTitleCard();

    static void debugNew(const char* name);

    static void debugDel(const char* name);
};
#endif
