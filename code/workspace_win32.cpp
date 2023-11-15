
// #ifdef	STD_INCLUDE_DECL
// #endif	// STD_INCLUDE_DECL
// #ifdef	STD_INCLUDE_FUNC
// #endif	// STD_INCLUDE_FUNC

//#pragma optimize( "", off )

#include <math.h> // sqrtf, cosf, sinf
#include <stdint.h> // Type definitions
#include <stdlib.h> // rand
#include <time.h> // time
#include <float.h> // FLT_MAX
#include <cstdio> // sprintf
#include <shlwapi.h>
#include <windows.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_trueType.h"

#include "TypeDef.h"
#include "math.cpp"

#define 	STD_INCLUDE_DECL
#include "Vector.cpp"
#include "Memory.cpp"
#include "Font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_DECL

#define	STD_INCLUDE_FUNC
#include "Vector.cpp"
#include "Memory.cpp"
#include "Font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_FUNC

#include "Shmup_Config.h"
#include "AssetID.h"

#include "d3d12.cpp"
#include "inputString.cpp"

#include "Win32_Platform.h"

#define APP_TargetFPS  ( 60.0f )

#include <dsound.h>
#include "AudioSystem.h"

#include "Intersect.h"

#include "Entity.h"
#include "Shmup.h"
#include "Debug_Win32.cpp"
#include "Intersect.cpp"

#include "AudioSystem.cpp"

#include "bmp.cpp"
#include "wav.cpp"

#include "Draw.cpp"
#include "tools.cpp"

#if ENABLE_EDITOR
#include "File.cpp"
#include "EditorTools.cpp"
#endif

#include "Particle.cpp"
#include "Score.cpp"
#include "EntityTools.cpp"

#include "ScoreMultiplier.cpp"

#include "Overworld.cpp"
//#include "Market.cpp"

#include "Bullet.cpp"
#include "EnemyBullet.cpp"
//#include "Explosive.cpp"
#include "Turret.cpp"
#include "Spitter.cpp"
#include "Chaser.cpp"
#include "Swarm.cpp"
//#include "Bumper.cpp"
#include "Drifter.cpp"
#include "Looper.cpp"
//#include "Starburst.cpp"
#include "AutoTurret.cpp"
#include "Powerup.cpp"
#include "BayDoor.cpp"
#include "Miner.cpp"
#include "Bomb.cpp"
#include "Asteroid.cpp"
//#include "Bank.cpp"
#include "Blimp.cpp"
#include "Recon.cpp"
#include "BombLauncher.cpp"
#include "Satellite.cpp"
//#include "Smog.cpp"
//#include "Escort.cpp"
#include "Missile.cpp"
#include "MissileLauncher.cpp"
#include "BoardRoom.cpp"
//#include "Rogue.cpp"
#include "Fireball.cpp"
#include "Worm.cpp"
//#include "Bolt.cpp"
#include "Gunner.cpp"

#include "EntitySpawn.cpp"
#include "Wave.cpp"
#include "EntityFunctions.cpp"
#include "EndLevel.cpp"
#include "ZeroPoint.cpp"

#include "BonusObjective.cpp"
//#include "ProgressMeter.cpp"


#include "Player.cpp"
#include "UI.cpp"

//#include "parser.cpp"
//#include "GenMesh.cpp"
//#include "X.cpp"
//#include "Load.cpp"

#include "gameplay.cpp"

#if ( ( ENABLE_DEBUG_REPLAY ) || ( ENABLE_EDITOR ) )
#include "Panel.cpp"
#endif

#if ENABLE_DEBUG_REPLAY
#include "Replay.cpp"
#include "DebugPanel.cpp"
#endif

#if ENABLE_EDITOR
#include "Undo.cpp"
#include "Editor.cpp"
#include "save.cpp"
#include "EditorPanel.cpp"
#endif

#include "Shmup.cpp"
#include "Win32_Platform.cpp"
