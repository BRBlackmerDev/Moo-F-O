
typedef DIRECTX_12_RENDERER    RENDERER;
typedef DIRECTX_12_RENDER_PASS RENDER_PASS;

struct PLATFORM {
    boo32 IsRunning;
    
    flo32 TargetSec;
    
    KEYBOARD_STATE   Keyboard;
    MOUSE_STATE      Mouse;
    
    MEMORY PermMemory;
    MEMORY TempMemory;
    
    AUDIO_SYSTEM AudioSystem;
    
    RENDERER Renderer;
    
    READ_FILE       * ReadFile;
    WRITE_FILE      * WriteFile;
    DOES_FILE_EXIST * DoesFileExist;
    DELETE_FILE     * DeleteFile;
};

//----------
// CAMERA
//----------

struct CAMERA_SHAKE {
    vec2 P;
    vec2 Q;
    
    flo32 Timer;
    flo32 UpTime;
    flo32 HoldTime;
    flo32 DownTime;
    
    flo32 t;
    flo32 TargetT;
    
    flo32 Strength;
    vec2  Offset;
};

struct CAMERA_STATE {
    vec2  Pos;
    vec2  Dim;
    vec2  HalfDim;
    flo32 Scale_Ratio;
    
    boo32 Move_Left;
    boo32 Move_Right;
    boo32 Move_Down;
    boo32 Move_Up;
    boo32 Move_Out;
    boo32 Move_In;
};

//----------
// FILE SAVE/OPEN
//----------

#define FILE_SAVE_OPEN__FILENAME_MAX_CHAR  ( 128 )
struct FILE_SAVE_OPEN {
    INPUT_STRING  InputString;
    STRING_BUFFER FileList;
    
    boo32 DoOverwrite;
    boo32 UnsavedChanges;
    boo32 DoRecentSave;
    flo32 Timer;
    boo32 ConfirmNew;
    
    uint32 Select_iSelect;
    int32  Select_iFile;
    
    char  CurrentFile[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
};

//----------
// UNDO STATE
//----------

#define UNDO_MAX_COUNT  ( 32 )
struct UNDO_STATE {
    int32             AtIndex;
    int32             nUndo;
    int32             nRedo;
    EDITOR__WAVE_STATE Undo[ UNDO_MAX_COUNT ];
};

//----------
// EDITOR STATE
//----------

enum EDITOR_MODE {
    EditorMode_Add,
    EditorMode_Move,
    EditorMode_Copy,
    
    EditorMode_ModifyShield,
};

enum ENTITY_TEMPLATE_MODE {
    EntityTemplateMode_Line,
    EntityTemplateMode_Circle,
    
    EntityTemplateMode_Count,
};

enum ENTITY_MOUSE_WHEEL_MODE {
    //EntityMouseWheelMode_Type,
    EntityMouseWheelMode_nEntity,
    EntityMouseWheelMode_TargetRadiusOrHealth,
    
    EntityMouseWheelMode_Count,
};

struct EDITOR_STATE {
    boo32 IsInitialized;
    EDITOR_MODE Mode;
    
    CAMERA_STATE Camera_WaveEditor;
    CAMERA_STATE Camera_LayoutEditor;
    
    PANEL        Editor_Panel;
    PANEL        Layout_Panel;
    boo32        DrawGrid;
    
    rect           World_Bound;
    flo32          World_YUnits;
    
    LEVEL_SETTINGS Level_Settings;
    MEMORY         LayoutBitmap_Memory;
    
    FILE_SAVE_OPEN SaveOpen;
    
    ENTITY_MOUSE_WHEEL_MODE EntityMouseWheel_Mode;
    ENTITY_TEMPLATE_MODE    EntityTemplate_Mode;
    int32                   EntityTemplate_nEntity;
    flo32                   EntityTemplate_Spacing;
    flo32                   EntityTemplate_Radians;
    int32                   Entity_iEntity;
    ENTITY                  Entity_LastEntity;
    
    // NOTE: Any variable listed below this point will be cleared to zero when ResetEditor() is called.
    uint32 Reset_Reset;
    
    boo32  ShowLayout;
    boo32  ShowLayout_NextLink;
    int32  ShowLayout_NextLink_iWave;
    
    boo32  ShowLayout_KillLink;
    int32  ShowLayout_KillLink_iWave;
    
    boo32  ShowLayout_DoMove;
    int32  ShowLayout_DoMove_iWave;
    
    boo32  ShowLayout_DoGroupMove;
    boo32  ShowLayout_DoGroupMove_DoSet;
    vec2   ShowLayout_DoGroupMove_bPos;
    rect   ShowLayout_DoGroupMove_Bound;
    
    EDITOR__WAVE_STATE WaveS;
    UNDO_STATE         UndoS;
    
    boo32 DoMove;
    boo32 DoCopy;
    boo32 DoBoxSelect_Add;
    boo32 DoBoxSelect_Remove;
    vec2  Mouse_bPos;
};

//----------
// DRAW STATE
//----------

struct DRAW_STATE {
    int32 Debug_iEntity;
    
    CAMERA_STATE           * Camera;
    
    SCORE_STATE            * ScoreS;
    
    PLAYER_STATE           * Player;
    BULLET_STATE           * BulletS;
    ENEMY_BULLET_STATE     * EnemyBulletS;
    //EXPLOSIVE_STATE        * ExplosiveS;
    TURRET_STATE           * TurretS;
    SPITTER_STATE           * SpitterS;
    CHASER_STATE           * ChaserS;
    SWARM_STATE            * SwarmS;
    //BUMPER_STATE           * BumperS;
    DRIFTER_STATE          * DrifterS;
    POWERUP_STATE          * PowerupS;
    LOOPER_STATE           * LooperS;
    //STARBURST_STATE        * StarburstS;
    AUTO_TURRET_STATE      * AutoTurretS;
    BOARD_ROOM_STATE      * BoardRoomS;
    BAY_DOOR_STATE         * BayDoorS;
    ASTEROID_STATE         * AsteroidS;
    MINER_STATE            * MinerS;
    BOMB_STATE           * BombS;
    BOMB_LAUNCHER_STATE    * BombLauncherS;
    //MISSILE_LAUNCHER_STATE    * MissileLauncherS;
    //BANK_STATE             * BankS;
    BLIMP_STATE            * BlimpS;
    RECON_STATE            * ReconS;
    SATELLITE_STATE        * SatelliteS;
    //SMOG_STATE             * SmogS;
    //ESCORT_STATE           * EscortS;
    //BOLT_STATE             * BoltS;
    GUNNER_STATE           * GunnerS;
    //ROGUE_STATE            * RogueS;
    MISSILE_STATE          * MissileS;
    WORM_STATE             * WormS;
    FIREBALL_STATE         * FireballS;
    ENTITY_SPAWN_STATE     * EntitySpawnS;
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS;
    SCORE_PARTICLE_STATE   * ScoreParticleS;
    PARTICLE_STATE         * ParticleS;
    //PROGRESS_STATE         * ProgressS;
    END_LEVEL              * EndLevel;
};

//----------
// DEBUG REPLAY
//----------

struct REPLAY_FRAME {
    CAMERA_STATE           Camera;
    SCORE_STATE            ScoreS;
    PLAYER_STATE           Player;
    
    BULLET_STATE           BulletS;
    ENEMY_BULLET_STATE     EnemyBulletS;
    //EXPLOSIVE_STATE        ExplosiveS;
    TURRET_STATE           TurretS;
    SPITTER_STATE           SpitterS;
    CHASER_STATE           ChaserS;
    SWARM_STATE            SwarmS;
    //BUMPER_STATE           BumperS;
    DRIFTER_STATE          DrifterS;
    POWERUP_STATE          PowerupS;
    LOOPER_STATE           LooperS;
    //STARBURST_STATE        StarburstS;
    AUTO_TURRET_STATE      AutoTurretS;
    BOARD_ROOM_STATE      BoardRoomS;
    BAY_DOOR_STATE         BayDoorS;
    ASTEROID_STATE         AsteroidS;
    MINER_STATE            MinerS;
    BOMB_STATE           BombS;
    BOMB_LAUNCHER_STATE    BombLauncherS;
    //MISSILE_LAUNCHER_STATE    MissileLauncherS;
    BANK_STATE             BankS;
    BLIMP_STATE            BlimpS;
    RECON_STATE            ReconS;
    SATELLITE_STATE        SatelliteS;
    //SMOG_STATE             SmogS;
    //ESCORT_STATE           EscortS;
    //BOLT_STATE             BoltS;
    GUNNER_STATE           GunnerS;
    //ROGUE_STATE            RogueS;
    MISSILE_STATE         MissileS;
    WORM_STATE            WormS;
    FIREBALL_STATE         FireballS;
    ENTITY_SPAWN_STATE     EntitySpawnS;
    SCORE_MULTIPLIER_STATE ScoreMultiplierS;
    SCORE_PARTICLE_STATE   ScoreParticleS;
    
    PARTICLE_STATE  ParticleS;
    //PROGRESS_STATE  ProgressS;
    END_LEVEL       EndLevel;
};

#define REPLAY__SECONDS_TO_CAPTURE  ( 10 )
#define REPLAY__FRAMES_PER_SECOND   ( 60 )
#define REPLAY_FRAME_MAX_COUNT  ( REPLAY__SECONDS_TO_CAPTURE * REPLAY__FRAMES_PER_SECOND )
struct REPLAY_STATE {
    boo32  Show;
    uint32 Show_AtFrame;
    boo32  Show_HoldFrame;
    
    uint32     AtFrame;
    REPLAY_FRAME Frame[ REPLAY_FRAME_MAX_COUNT ];
};

//----------
// APP STATE
//----------

enum APP_MODE {
    AppMode_TitleScreen,
    AppMode_HowToPlay,
    AppMode_Game,
    AppMode_Overworld,
    AppMode_LevelInfo,
    AppMode_Market,
    
#if ENABLE_EDITOR
    AppMode_Editor,
    AppMode_SaveLevel,
    AppMode_OpenLevel,
#endif
};

enum INGAME_MENU_MODE {
    InGameMenuMode_RestartLevel,
    InGameMenuMode_ExitToOverworld,
    InGameMenuMode_ExitToDesktop,
    
    InGameMenuMode_Count,
};

#define COW_SPEED_FOR_OVERWORLD_MAP  (  64.0f )
#define COW_SPEED_FOR_TITLE_SCREEN   (  16.0f )

struct TITLE_MENU_COW {
    boo32 IsInitialized;
    vec3  Position;
    vec3  Destination;
    vec3  Velocity;
    quat  Orientation;
    flo32 Radians;
    
    flo32 Timer;
    vec3  Direction;
    flo32 AtTheta;
    flo32 AtPhi;
    flo32 DestPhi;
};

struct APP_STATE {
	boo32 IsRunning;
	boo32 IsInitialized;
    
    AUDIO_SYSTEM * AudioSystem;
    AUDIO_DATA     AudioList  [ AudioID_Count ];
    
#define AUDIO_SCRIPT_MAX_COUNT  ( 16 )
    AUDIO_ENTRY    AudioScript[ AUDIO_SCRIPT_MAX_COUNT ];
    
    ASSET_LEVEL    LevelList[ LevelID_Count ];
    
    APP_MODE Mode;
    boo32    Mode_IsInitialized;
	
    boo32  DoSaveGame;
    boo32  DoRecentSave;
    boo32  DoTransition;
    flo32  DoTransition_Timer;
    
    vec3   FadeOut_RGB;
    flo32  FadeOut_Alpha;
    
    //boo32  DoReplayLevel;
    //boo32  DoExitToOverworld;
    //boo32  DoExitToDesktop;
    END_LEVEL_ACTION EndLevelAction;
    LEVEL_INFO_WRAPPER  CurrentLevel;
    
    INGAME_MENU_MODE InGameMenu_Mode;
    boo32            InGameMenu_ShowMenu;
    boo32            InGameMenu_ShowConfirm;
    boo32            TitleMenu_ShowConfirm;
    flo32            HowToPlay_Radians;
    TITLE_MENU_COW   TitleMenu_Cow;
    TITLE_MENU_COW   Overworld_Cow;
    vec2             TitleMenu_Background_Pos;
    vec2             LevelInfo_Background_Pos;
    
	vec2 App_Dim;
	vec2 App_HalfDim;
	rect App_Bound;
    flo32 dT;
    
    vec2  UI_Dim;
    vec2  UI_HalfDim;
    rect  UI_Bound;
    rect  World_Bound;
    
#define MARKET_LAST_TRANSACTION_TARGET_TIME  ( 120.0f / 60.0f )
    int32         Market_iSelect;
    flo32         Market_LastTransaction_Timer;
    boo32         Market_LastTransaction_DoBuy;
    uint64        Market_LastTransaction_TotalCost;
    boo32         Market_ShowBrokenLink;
    
    //uint64        Market_TotalPoints;
    //POWERUP_ENTRY PowerupEntry[ PowerupType_Count ];
    
#if DEBUG_BUILD
    FONT * Font;
#endif
    
    FONT FontList[ FontID_Count ];
    
    SAVE_GAME SaveGame;
    
    OVERWORLD_STATE OverworldS;
    //MARKET_STATE    MarketS;
    
    uint32 Reset_LevelReset_Start;
    
    WAVE_STATE             WaveS;
    
    BULLET_STATE           BulletS;
    ENEMY_BULLET_STATE     EnemyBulletS;
    //EXPLOSIVE_STATE        ExplosiveS;
    TURRET_STATE           TurretS;
    SPITTER_STATE          SpitterS;
    CHASER_STATE           ChaserS;
    SWARM_STATE            SwarmS;
    //BUMPER_STATE           BumperS;
    DRIFTER_STATE          DrifterS;
    POWERUP_STATE          PowerupS;
    LOOPER_STATE           LooperS;
    //STARBURST_STATE        StarburstS;
    AUTO_TURRET_STATE      AutoTurretS;
    BOARD_ROOM_STATE       BoardRoomS;
    BAY_DOOR_STATE         BayDoorS;
    ASTEROID_STATE         AsteroidS;
    MINER_STATE            MinerS;
    BOMB_STATE             BombS;
    BOMB_LAUNCHER_STATE    BombLauncherS;
    //MISSILE_LAUNCHER_STATE MissileLauncherS;
    //BANK_STATE             BankS;
    BLIMP_STATE            BlimpS;
    RECON_STATE            ReconS;
    SATELLITE_STATE        SatelliteS;
    //SMOG_STATE             SmogS;
    //ESCORT_STATE           EscortS;
    //BOLT_STATE             BoltS;
    GUNNER_STATE           GunnerS;
    //ROGUE_STATE            RogueS;
    MISSILE_STATE          MissileS;
    WORM_STATE             WormS;
    FIREBALL_STATE         FireballS;
    ENTITY_SPAWN_STATE     EntitySpawnS;
    SCORE_MULTIPLIER_STATE ScoreMultiplierS;
    END_LEVEL              EndLevel;
    
    CAMERA_STATE Camera;
    CAMERA_SHAKE CameraShake;
    
#if ENABLE_DEBUG_REPLAY
    REPLAY_STATE Replay;
#endif
    
    //PROGRESS_STATE       ProgressS;
    SCORE_STATE          ScoreS;
    SCORE_PARTICLE_STATE ScoreParticleS;
    PARTICLE_STATE       ParticleS;
    
    PLAYER_STATE          Player;
    
    uint32 Reset_LevelReset_End;
    
    //COLLISION_STATE  Collision;
    //MEMORY           Collision_Memory;
    
#if ENABLE_DEBUG_REPLAY
    PANEL Debug;
    int32 Debug_iEntity;
#define DEBUG_MAX_COUNT  ( 32 )
    boo32 Debug_IsActive[ DEBUG_MAX_COUNT ];
#endif
    
    PROFILE_STATE  Profile;
    
#if ENABLE_EDITOR
    EDITOR_STATE   Editor;
#endif
};