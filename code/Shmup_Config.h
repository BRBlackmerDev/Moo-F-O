
//----------
//----------

//#define APP_PermMemorySize _MB( 256 )
#define APP_PermMemorySize   _GB( 3 )
#define APP_TempMemorySize   _MB( 256 )

#define APP_isFullscreen  ( 0 )
#define LEVEL_SAVE_DIRECTORY  ( "../../asset/entity" )
#define ART_SAVE_DIRECTORY    ( "../../asset/art" )
#define AUDIO_SAVE_DIRECTORY  ( "../../asset/audio" )
#define MUSIC_SAVE_DIRECTORY  ( "../../asset/audio/music" )
#define CODE_DIRECTORY        ( "../../code" )

#define START_IN_BLANK_LEVEL  ( 0 )
#define START_IN_EDITOR       ( 0 )

#define ENABLE_EDITOR         ( 0 )
#define ENABLE_DEBUG_REPLAY   ( 0 )

#if ( DEBUG_BUILD )
#define FONT_SAVE_DIRECTORY   ( "../../asset/" )
#else
#define FONT_SAVE_DIRECTORY   ( 0 )
#endif

// NOTE: Currently, our Collision is so good that if an Entity lands perfectly on the Top corner of a Platform, they will be unable to move onto the Platform or drop down. The Collision system will hold them in place. This COLLISION_EPSILON_OFFSET will Offset the Entity from a surface after a Collision, so the second iteration/Collision will not occur. This strategy really only works in the circumstance because all the Collision geometry is orthogonal on a Grid.
#define COLLISION_EPSILON_OFFSET            ( 0.001f )
#define COLLISION_EPSILON_MIN_VALID_LENGTH  ( 0.0001f )
#define COLLISION_MAX_ITERATION             ( 4 )

#define LAYOUT_BITMAPS_PIXELS_PER_UNIT           ( 16.0f )
#define WORLD_TEXT_SCALE                         ( Vec2( 0.035f, 0.035f ) )
#define WORLD_BOUND_BASE_Y_UNITS_FOR_TEXT_SCALE  ( 36.0f )


#define FILETAG__LEVEL          ( "LEVEL___" )
#define FILETAG__SAVEGAME_FILE  ( "SAVEGAME" )


#define BUTTON_VOLUME              ( 1.0f )
#define PLAYER_BULLET_VOLUME       ( 1.0f )
#define ASTEROID_COLLISION_VOLUME  ( 1.0f )
#define SPIT_BONUS_VOLUME          ( 1.0f )
#define MINER_LASER_VOLUME         ( 1.0f )

#define POWERUP_VOLUME  ( 1.0f )

#define ENTITY_SPAWN_VOLUME  ( 0.25f )
#define ENTITY_DEATH_VOLUME  ( 1.0f )
#define ENEMY_BULLET_VOLUME  ( 1.0f )
#define FIREBALL_VOLUME      ( 0.25f )

#define MUSIC_TITLE_SCREEN_VOLUME  ( 1.0f )
#define MUSIC_LEVEL_VOLUME         ( 1.0f )

#define PLAYER_DAMAGE_VOLUME            ( 1.0f )
#define PLAYER_SECONDARY_DAMAGE_VOLUME  ( 1.0f )

#define SCORE_MULTIPLIER_VOLUME         ( 0.25f )
