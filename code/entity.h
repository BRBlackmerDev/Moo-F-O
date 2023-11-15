
//----------
// DEBUG_PANEL
//----------

#define PANEL_LABEL_Y  ( 14.0f )
struct PANEL {
    boo32 Show;
    rect  Bound;
    
    rect  Inner;
    vec2  AtPos;
    flo32 AdvanceY;
    flo32 ScrollT;
};

//----------
// STRING
//----------

struct STRING {
    char * String;
    uint32 nChar;
};

internal boo32
MatchString( char * String0, char * String1 ) {
    boo32 doMatch = true;
    while( ( *String0 ) && ( *String1 ) ) {
        if( *String0 != *String1 ) { doMatch = false; }
        String0++;
        String1++;
    }
    if( ( *String0 ) || ( *String1 ) ) { doMatch = false; }
    return doMatch;
}

internal boo32
MatchSegment( char * String0, char * String1, uint32 nChar ) {
    boo32 doMatch = true;
    for( uint32 iChar = 0; ( doMatch ) && ( iChar < nChar ); iChar++ ) {
        if( String0[ iChar ] != String1[ iChar ] ) { doMatch = false; }
    }
    return doMatch;
}

internal boo32
MatchSegmentNoCase( char * StringA, char * StringB, uint32 nChar ) {
    boo32 DoMatch = true;
    for( uint32 iChar = 0; ( DoMatch ) && ( iChar < nChar ); iChar++ ) {
        uint32 CharA = ( uint32 )StringA[ iChar ];
        uint32 CharB = ( uint32 )StringB[ iChar ];
        if( CharA < 'a' ) { CharA += 0X00000020; }
        if( CharB < 'a' ) { CharB += 0x00000020; }
        if( CharA != CharB ) { DoMatch = false; }
    }
    return DoMatch;
}

internal void
WriteSegment( MEMORY * Memory, char * str ) {
    uint32 length = ( uint32 )strlen( str );
    uint8 * dest = ( uint8 * )_PushSize( Memory, length );
    memcpy( dest, str, length );
}

internal void
WriteString( MEMORY * Memory, char * str, uint32 nChar ) {
    uint8 * dest = ( uint8 * )_PushSize( Memory, nChar );
    memcpy( dest, str, nChar );
    
    _writem( Memory, uint8, 0 );
}

internal void
WriteString( MEMORY * Memory, char * str ) {
    uint32 nChar = ( uint32 )strlen( str );
    WriteString( Memory, str, nChar );
}

internal boo32
MatchString( STRING strA, STRING strB ) {
    boo32 Result = ( strA.nChar == strB.nChar );
    if( Result ) {
        Result = MatchSegment( strA.String, strB.String, strA.nChar );
    }
    return Result;
}

internal boo32
MatchString( STRING strA, char * str ) {
    uint32 nChar = ( uint32 )strlen( str );
    boo32 Result = ( strA.nChar == nChar );
    if( Result ) {
        Result = MatchSegment( strA.String, str, strA.nChar );
    }
    return Result;
}

//----------
// STRING BUFFER
//----------

struct STRING_BUFFER {
    uint32 nStr;
    uint32 nStrMax;
    char ** str;
    MEMORY  Memory;
};

internal void
Reset( STRING_BUFFER * buffer ) {
    buffer->nStr        = 0;
    buffer->Memory.Used = 0;
}

internal STRING_BUFFER
StringBuffer( MEMORY * parent_Memory, uint32 maxString, uint32 maxSize ) {
    STRING_BUFFER Result = {};
    Result.str     = _PushArray_Clear( parent_Memory, char *, maxString );
    Result.Memory  = SubArena( parent_Memory, maxSize );
    Result.nStrMax = maxString;
    return Result;
}

internal void
StringBuffer_FreeMemory( STRING_BUFFER Buffer, MEMORY * Parent_Memory ) {
    _PopArray( Parent_Memory, char *, Buffer.nStrMax );
    _PopSize ( Parent_Memory, Buffer.Memory.Size );
}

internal char *
AddString( STRING_BUFFER * s, char * String ) {
    char * dest = 0;
    if( s->nStr < s->nStrMax ) {
        uint32 length = ( uint32 )strlen( String );
        dest = ( char * )_PushSize( &s->Memory, length + 1 );
        strcpy( dest, String );
        
        s->str[ s->nStr++ ] = dest;
    }
    
    return dest;
}

//----------
// COLLISION
//----------

#if 0
struct COLLISION_STATE {
};
#endif


//----------
// PARTICLES
//----------

struct PARTICLE {
    TEXTURE_ID TextureID;
    
    vec2  Position;
    vec2  Velocity;
    vec2  Accel;
    flo32 Friction;
    
    vec2   Dim;
    vec2  dDim;
    
    flo32  Radians;
    flo32 dRadians;
    
    flo32  Timer;
    flo32  TargetTime;
    vec4   Color;
    vec4  dColor;
};

#define PARTICLE_MAX_COUNT  ( 8192 )
struct PARTICLE_STATE {
    uint32    nParticle;
    PARTICLE   Particle[ PARTICLE_MAX_COUNT ];
};

//----------
// SCORE
//----------

#define ENTITY_SHOW_DAMAGE_TARGET_TIME  ( 20.0f / 60.0f )

#define ENTITY_TYPE_LABELS  char * EntityTypeLabels[ EntityType_Count ] = { "Turret", "Chaser", "Swarm", "Drifter", "Looper", "Starburst", "Star", "Enemy_Bullet", "PersonalPod", "Transport", "MiniBoss", "Asteroid", "Asteroid_Tiny", "Asteroid_Small", "Asteroid_Medium", "Asteroid_Large", "Miner", "Bomber", "Bomb", "AutoTurret", "Explosive", "Bumper", "Bumper_Bounce", "Bank", "Bank_Hit", "BombLauncher", "Blimp", "Yacht", "Bumper_Stationary", "MegaTurret", "Miner_Stationary", "Yacht_Hit", "Asteroid_BombOnly", "Asteroid_LaserOnly", "Asteroid_SwarmOnly", "MegaMiner", "Escort", "BonusPoints", "Rogue", "Missile", "Worm", "Fireball", "Laser", "Spitter", "Collector", "Spit", "Loader", "Damage Zone", "Barrel", "Security Drone", "Bolt", "Gunner", "Missile Launcher", "Observation Deck", "Observation Deck Barrier", "Recon", "Satellite" };

#define ENTITY_TYPE_MAX_COUNT  ( 128 )
enum ENTITY_TYPE {
    EntityType_Turret,
    EntityType_Chaser,
    EntityType_Swarm,
    EntityType_Drifter,
    EntityType_Looper,
    EntityType_Starburst,
    EntityType_Star,
    EntityType_EnemyBullet,
    
    EntityType_PersonalPod,
    EntityType_Transport,
    EntityType_MiniBoss,
    
    EntityType_Asteroid,
    EntityType_Asteroid_Tiny,
    EntityType_Asteroid_Small,
    EntityType_Asteroid_Medium,
    EntityType_Asteroid_Large,
    
    EntityType_Miner,
    EntityType_Bomber,
    EntityType_Bomb,
    
    EntityType_AutoTurret,
    EntityType_Explosive,
    
    EntityType_Bumper,
    EntityType_Bumper_Bounce,
    
    EntityType_Bank,
    EntityType_Bank_Hit,
    
    EntityType_BombLauncher,
    
    EntityType_Blimp,
    EntityType_Yacht,
    EntityType_Bumper_Stationary,
    EntityType_MegaTurret,
    EntityType_Miner_Stationary,
    EntityType_Yacht_Hit,
    
    EntityType_Asteroid_BombOnly,
    EntityType_Asteroid_LaserOnly,
    EntityType_Asteroid_SwarmOnly,
    
    EntityType_MegaMiner,
    EntityType_Escort,
    EntityType_BonusPoints,
    EntityType_Rogue,
    EntityType_Missile,
    EntityType_Worm,
    EntityType_Fireball,
    EntityType_Laser,
    EntityType_Spitter,
    EntityType_Collector,
    EntityType_Spit,
    EntityType_Loader,
    EntityType_DamageZone,
    EntityType_Barrel,
    EntityType_SecurityDrone,
    EntityType_Bolt,
    EntityType_Gunner,
    EntityType_MissileLauncher,
    EntityType_ObservationDeck,
    EntityType_ObservationDeckBarrier,
    EntityType_Recon,
    EntityType_Satellite,
    
    EntityType_Count,
};

#define TURRET_BASE_POINTS              ( 50 )
#define STARBURST_BASE_POINTS           ( 15 )
#define STAR_BASE_POINTS                (  5 )
#define CHASER_BASE_POINTS              ( 10 )
#define SWARM_BASE_POINTS               ( 10 )
#define DRIFTER_BASE_POINTS             (  5 )
#define LOOPER_BASE_POINTS              (  5 )
#define PERSONAL_POD_BASE_POINTS             (  2 )
#define TRANSPORT_BASE_POINTS           ( 1000 )
#define MINIBOSS_BASE_POINTS            ( 2500 )
#define ASTEROID_BOMB_ONLY_BASE_POINTS  ( 75 )
#define ASTEROID_TINY_BASE_POINTS       ( 25 )
#define MINER_BASE_POINTS               ( 75 )
#define BOMBER_BASE_POINTS              ( 75 )
#define BUMPER_BASE_POINTS              ( 100 )
#define BUMPER_BOUNCE_BASE_POINTS       ( 1 )
// Turret             : 50pts, 15 mul (20% +10 mul)
// Starburst          : 15pts,  0 mul
// Star               :  5pts,  1 mul
// Chaser             : 10pts,  2 mul (20% +1  mul)
// Swarm              : 10pts,  1 mul (20% +1  mul)
// Drifter            :  5pts,  2 mul (50% +1  mul)
// Looper             :  5pts,  2 mul (50% +3  mul)
// PersonalPod            :  2pts,  1 mul
// Transport          : 1000pts, 100 mul
// MiniBoss           : 2500pts, 175 mul
// Asteroid Bomb-Only : 75pts, 50 mul
// Asteroid Tiny      : 25pts, 1 mul
// Miner              : 75pts, 50 mul
// Bomber             : 75pts, 50 mul

struct SCORE_KILL {
    ENTITY_TYPE Type;
    vec2        Position;
    vec2        Velocity;
    int32       CustomPoints;
    boo32       DoTriggerBonusMultiplier;
};

struct SCORE_ENTITY_KILL_COUNT {
    int32 nKilled;
    int32 nMissed;
    int32 nTotal;
};

#define SCORE_LOG_MAX_COUNT  ( 1024 )
struct SCORE_LOG {
    ENTITY_TYPE Type;
    flo32       Points;
    flo32       AtScoreMultiplier;
    flo32       AtBonusMultiplier;
    int32       nScoreMultiplierOnScreen;
    flo32       PointsMissed;
};

#define BONUS_OBJECTIVE_TEXT_BASE_COLOR  ( Vec4( 1.0f, 1.0f, 0.0f, 1.0f ) )

struct SCORE_STATE {
    boo32 Level_DisplayTimer_DoUpdate;
    flo32 Level_DisplayTimer;
    flo32 Level_PlayTimer;
    
    flo32 Score;
    flo32 Score_Positive;
    flo32 Score_Negative;
    flo32 Multiplier;
    boo32 DoBonusObjective;
    
#define SCORE_KILL_MAX_COUNT  ( 512 )
    int32     nKill;
    int32     nKill_Positive;
    int32     nKill_Negative;
    SCORE_KILL Kill[ SCORE_KILL_MAX_COUNT ];
    
    SCORE_ENTITY_KILL_COUNT EntityKillCount[ EntityType_Count ];
    
#define BONUS_MULTIPLIER_MAX_VALUE                         ( 5.0f )
#define BONUS_MULTIPLIER_LAST_KILL_TARGET_TIME             ( 30.0f / 60.0f )
#define BONUS_MULTIPLIER_DRAIN_COOLDOWN_TARGET_TIME_FULL   ( 76.0f / 60.0f )
#define BONUS_MULTIPLIER_DRAIN_COOLDOWN_TARGET_TIME_PAUSE  (  8.0f / 60.0f )
#define BONUS_MULTIPLIER_DRAIN_RATE                        (  6.0f / 60.0f )
    flo32  BonusMultiplier;
    flo32  BonusMultiplier_LastKill_Timer;
    flo32  BonusMultiplier_Drain_CooldownTimer;
    flo32  BonusMultiplier_Radians;
    
    flo32  BonusMultiplier_TotalTime; // NOTE: This is the same as total game time.
    flo32  BonusMultiplier_Total;
    flo32  BonusMultiplier_FullTime;
    
    //int32    nScoreLog;
    //SCORE_LOG ScoreLog[ SCORE_LOG_MAX_COUNT ];
};

//----------
// SCORE MULTIPLIER
//----------

#define SCORE_MULTIPLIER_LIFE_TARGET_TIME  ( 240.0f / 60.0f )

#define SCORE_MULTIPLIER_ROTATE_SPEED   ( TAU / 4.0f )

#define SCORE_MULTIPLIER_RADIUS       ( 0.25f )
#define SCORE_MULTIPLIER_PULL_RADIUS  ( 4.5f  )

#define SCORE_MULTIPLIER_BASE_COLOR  ( Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) )

#define SCORE_MULTIPLIER_MAX_COUNT          ( 512 )
#define SCORE_MULTIPLIER_SPAWNER_MAX_COUNT  (   4 )
struct SCORE_MULTIPLIER {
    boo32  IsBonusPoints;
    boo32  DoSeek;
    vec2   Position;
    vec2   Velocity;
    vec2   Direction;
    flo32  Friction;
    flo32  Speed;
    flo32  Radians;
    
    flo32  Life_Timer;
};

struct SCORE_MULTIPLIER_SPAWNER {
    vec2  Position;
    int32 nScoreMultiplier;
};

struct SCORE_MULTIPLIER_STATE {
    int32 nScoreMultiplierSpawned;
    flo32 Powerup_Timer;
    //flo32 Powerup_TimeToAdd;
    //flo32 Powerup_RadiusToAdd;
    
    int32           nScoreMultiplier;
    SCORE_MULTIPLIER ScoreMultiplier[ SCORE_MULTIPLIER_MAX_COUNT ];
    
    int32                   nSpawner;
    SCORE_MULTIPLIER_SPAWNER Spawner[ SCORE_MULTIPLIER_SPAWNER_MAX_COUNT ];
};

//----------
// SCORE PARTICLE
//----------

#define SCORE_PARTICLE_LIFE_TARGET_TIME            ( 40.0f / 60.0f )
#define SCORE_PARTICLE_MILESTONE_LIFE_TARGET_TIME  ( 80.0f / 60.0f )

#define SCORE_PARTICLE_MAX_COUNT  ( 512 )
struct SCORE_PARTICLE {
    boo32  IsMultiplier;
    uint32 Value;
    vec2   Position;
    vec2   Velocity;
    flo32  Life_Timer;
    flo32  Life_TargetTime;
};

struct SCORE_PARTICLE_STATE {
    uint32        nScoreParticle;
    SCORE_PARTICLE ScoreParticle[ SCORE_PARTICLE_MAX_COUNT ];
};

//----------
// TURRET
//----------

struct ENTITY_SHIELD {
    boo32 HasShield;
    flo32 bRadians;  // Rotation
    flo32 Radians;   // Arc Length Cover
    flo32 Dist;      // Inner Radius
    flo32 Width;
    flo32 RotateSpeed;
};

#define TURRET_SHIELD_MAX_T  ( 1.75f )
#define TURRET_SHIELD_TEMP_SPEED_TO_INDICATE_TARGET_SHIELD  ( -PLAYER_ROTATE_SPEED_FAST * 2.0f )

#define TURRET_FIRE_TARGET_TIME  ( 180.0f / 60.0f )

#define TURRET_EXIT_TARGET_TIME         ( 11.0f )
#define TURRET_EXIT_WARMUP_TARGET_TIME  ( 120.0f / 60.0f )

#define MEGA_TURRET_HEALTH             ( 125 )
#define MEGA_TURRET_FIRE_TARGET_TIME   ( 150.0f / 60.0f )
#define MEGA_TURRET_BASE_COLOR         ( Vec4( 0.5f, 0.0f, 0.1f, 1.0f ) )
#define MEGA_TURRET_EXIT_TARGET_TIME       ( 8.0f )
#define MEGA_TURRET_EXIT_ANIM_TARGET_TIME  ( 80.0f / 60.0f )
#define MEGA_TURRET_SPAWN_TARGET_TIME  ( 16.0f )
#define MEGA_TURRET_SPEED              ( 4.0f )
#define MEGA_TURRET_FRICTION           ( 1.5f )

struct MEGA_TURRET_EVENT {
    uint32 _Padding;
};

#define MEGA_TURRET_RADIUS  ( 2.0f )
#define TURRET_BASE_COLOR  ( Vec4( 0.2f, 0.2f, 1.0f, 1.0f ) )

#define TURRET_MAX_COUNT  ( 64 )
struct TURRET {
    int32 WaveGuid;
    
    vec2  Position;
    vec2  Velocity;
    flo32 Radius;
    int32 Health;
    
    flo32 Move_Radians;
    flo32 Move_Speed;
    flo32 Move_Timer;
    flo32 Move_TargetTime;
    
    flo32 Fire_Timer;
    flo32 Fire_TargetTime;
    flo32 Fire_Radians;
    flo32 Base_Radians;
    flo32 Damage_Timer;
    
#define TURRET_SHIELD_SEGMENT_MAX_COUNT  ( 64 )
    flo32 Shield_SegmentT[ TURRET_SHIELD_SEGMENT_MAX_COUNT ];
    
    ENTITY_SHIELD Shield;
    
    flo32 Exit_Timer;
};

enum TURRET_SETTINGS__SPEED {
    TurretSettings_Speed_Slow,
    TurretSettings_Speed_Fast,
    
    TurretSettings_Speed_Count,
};

enum TURRET_SETTINGS__RADIANS {
    TurretSettings_Radians_0,
    TurretSettings_Radians_90,
    TurretSettings_Radians_180,
    TurretSettings_Radians_270,
    TurretSettings_Radians_360,
    
    TurretSettings_Radians_Count,
};

struct TURRET_SETTINGS_EVENT {
    int32 Spawn_nFrames;
    int32 Spawn_MaxTurret;
    int32 Numer_SpeedRadians[ TurretSettings_Speed_Count ][ TurretSettings_Radians_Count ];
};

#define MEGA_TURRET_MAX_COUNT  ( 1 )
struct TURRET_STATE {
    boo32 Spawn_IsActive;
    int32 Spawn_MaxTurret;
    flo32 Spawn_Cooldown_Timer;
    flo32 Spawn_Cooldown_TargetTime;
    flo32 Spawn_xSpacing;
    
    int32 Numer_SpeedRadians[ TurretSettings_Speed_Count ][ TurretSettings_Radians_Count ];
    
    boo32 SpawnMegaTurret_IsActive;
    flo32 SpawnMegaTurret_Timer;
    int32 SpawnMegaTurret_Health;
    int32 BonusObjective_nMegaTurretPossibleKills;
    int32 BonusObjective_nMegaTurretKills;
    
    int32 nMegaTurret;
    TURRET MegaTurret[ MEGA_TURRET_MAX_COUNT ];
    
    int32 nTurret;
    TURRET Turret[ TURRET_MAX_COUNT ];
};

//----------
// ENEMY_BULLET
//----------

#define ENEMY_BULLET_RADIUS  ( 0.5f )
#define ENEMY_BULLET_SPEED   ( 12.0f )

#define ENEMY_BULLET_BASE_COLOR  ( Vec4( 1.0f, 0.2f, 0.0f, 1.0f ) )

#define ENEMY_BULLET_MAX_COUNT  ( 512 )
struct ENEMY_BULLET {
    vec2   Position;
    vec2   Velocity;
    int32  Health;
    flo32  tColor;
    flo32  tColorDest;
    
    boo32  IsTractorBeamed;
    flo32  IsTractorBeamed_Radians;
    
    boo32  DoHitPointDrop;
    boo32  DoExplodePointDrop;
    boo32  DoShieldAbsorb;
};

struct ENEMY_BULLET_STATE {
    int32        nEnemyBullet;
    ENEMY_BULLET  EnemyBullet[ ENEMY_BULLET_MAX_COUNT ];
};

//----------
// EXPLOSIVE
//----------

#define EXPLOSIVE_RADIUS  ( 0.75f )
#define EXPLOSIVE_SPEED   ( 12.0f )

#define EXPLOSIVE_EXPLODE_TARGET_TIME  ( 3.0f / 60.0f )
#define EXPLOSIVE_EXPLODE_RADIUS       ( 3.0f )

#define EXPLOSIVE_ZERO_POINT_RADIUS    ( 8.0f )

#define EXPLOSIVE_ANIM_CYCLE_TARGET_TIME_UNARMED  ( 112.0f / 60.0f )
#define EXPLOSIVE_ANIM_CYCLE_TARGET_TIME_ARMED    (  24.0f / 60.0f )

struct EXPLOSIVE_SETTINGS_EVENT {
    int32 Spawn_nFrames;
};

#define EXPLOSIVE_MAX_COUNT  ( 512 )
struct EXPLOSIVE {
    vec2   Position;
    vec2   Direction;
    flo32  Speed;
    int32  Health;
    
    boo32  ZeroPoint_ShowUI;
    boo32  ZeroPoint_IsActive;
    flo32  ZeroPoint_Dist;
    flo32  ZeroPoint_Radians;
    
    // TODO: This will go away when we update the particle system to draw bomb explosions.
    boo32  DoArmed;
    flo32  DoArmed_Timer;
    flo32  DoExplode_Timer;
};

struct EXPLOSIVE_STATE {
    boo32 Spawn_IsActive;
    flo32 Spawn_Timer;
    flo32 Spawn_TargetTime;
    
    uint32   nExplosive;
    EXPLOSIVE Explosive[ EXPLOSIVE_MAX_COUNT ];
};

//----------
// BULLET
//----------

#define BULLET_WIDTH       ( 0.25f )
#define BULLET_HALF_WIDTH  ( BULLET_WIDTH * 0.5f )
#define BULLET_HEIGHT      ( 2.0f )
#define BULLET_DIM         ( Vec2( BULLET_WIDTH, BULLET_HEIGHT ) )

#define BULLET_SPEED   ( 200.0f )

#define BULLET_MAX_COUNT  ( 64 )
struct BULLET {
    vec2  PrevPos;
    vec2  Position;
    vec2  Direction;
    int32 Health;
    boo32 DrawCollision;
    vec2  DrawCollision_P;
    vec2  DrawCollision_N;
    boo32 HideTailForReflectedBullet;
};

struct BULLET_STATE {
    int32  nBullet;
    BULLET  Bullet[ BULLET_MAX_COUNT ];
};

//----------
// CHASER
//----------

#define CHASER_RADIUS    ( 0.9f )

#define CHASER_SPEED     ( 12.0f )
#define CHASER_FRICTION  ( 1.5f )

#define CHASER_EXIT_SPEED     ( 64.0f )
#define CHASER_EXIT_FRICTION  ( 6.0f )

#define CHASER_REPEL_RADIUS  ( 0.7f )
#define CHASER_REPEL_SPEED   ( 2.0f )

#define CHASER_EXIT_TARGET_TIME           ( 8.0f )
#define CHASER_EXIT_COOLDOWN_TARGET_TIME  ( 12.0f / 60.0f )
#define CHASER_EXIT_WARMUP_TARGET_TIME    ( 48.0f / 60.0f )

#define CHASER_BASE_COLOR  ( Vec4( 0.0f, 0.75f, 0.75f, 1.0f ) )

#define CHASER_MAX_COUNT  ( 256 )
struct CHASER {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    vec2   DirToPlayer;
    flo32  Radius;
    int32  Health;
    
    flo32  Blink_Timer;
    flo32  Blink_TargetTime;
};

struct CHASER_STATE {
    int32  nChaser;
    CHASER  Chaser[ CHASER_MAX_COUNT ];
};

//----------
// SWARM
//----------

#define SWARM_RADIUS         ( 0.3f )
#define SWARM_EDITOR_RADIUS  ( 2.0f )

#define SWARM_WIDTH        ( 0.3f )
#define SWARM_HEIGHT       ( 0.6f )
#define SWARM_HALF_WIDTH   ( SWARM_WIDTH  * 0.5f )
#define SWARM_HALF_HEIGHT  ( SWARM_HEIGHT * 0.5f )
#define SWARM_DIM          ( Vec2( SWARM_WIDTH,      SWARM_HEIGHT      ) )
#define SWARM_HALF_DIM     ( Vec2( SWARM_HALF_WIDTH, SWARM_HALF_HEIGHT ) )

//#define SWARM_SPEED           ( 20.0f )
//#define SWARM_SPEED           ( 28.0f )
//#define SWARM_FRICTION        (  4.0f )
#define SWARM_SPEED           ( 20.0f )
#define SWARM_FRICTION        (  3.0f )

#define SWARM_REPEL_RADIUS  ( 0.4f )
#define SWARM_REPEL_SPEED   ( 2.0f )

#define SWARM_SPAWN_STYLE_LABELS  char * SwarmSpawnStyleLabels[ SwarmSpawnStyle_Count ] = { "Stream", "Group", "Point" };

enum SWARM_SPAWN_STYLE {
    SwarmSpawnStyle_Stream,
    SwarmSpawnStyle_Group,
    SwarmSpawnStyle_Point_Level05,
    
    SwarmSpawnStyle_Count,
};

struct SWARM_SETTINGS_EVENT {
    boo32             IsActive;
    SWARM_SPAWN_STYLE Style;
    int32             nMaxSpawn;
    int32             nRespawnAt;
    flo32             InnerRadius;
    flo32             OuterRadius;
    int32             Delay_TargetTime_nFrames;
};

struct SWARM_SPAWN_SETTINGS {
    int32             WaveGuid;
    boo32             IsActive;
    SWARM_SPAWN_STYLE Style;
    int32             nMaxSpawn;
    int32             nRespawnAt;
    flo32             InnerRadius;
    flo32             OuterRadius;
    flo32             Delay_TargetTime;
    
    int32             SpawnCounter;
    flo32             Delay_Timer;
    vec2              Position;
};

#define SWARM_BASE_COLOR  ( Vec4( 1.0f, 0.5f, 0.5f, 1.0f ) )

#define SWARM_MAX_COUNT  ( 256 )
struct SWARM {
    int32 WaveGuid;
    
    vec2  Position;
    vec2  Velocity;
    vec2  Repel;
    flo32 Radius;
    int32 Health;
    flo32 CloakT;
};

struct SWARM_STATE {
    SWARM_SPAWN_SETTINGS Spawn;
    flo32                Spawn_Timer;
    
    int32 nSwarm;
    SWARM  Swarm[ SWARM_MAX_COUNT ];
};

struct EDITOR__SWARM {
    vec2   Position;
    uint32 nSwarm;
};

//----------
// BUMPER
//----------

#define BUMPER_RADIUS                 ( 1.2f )
#define BUMPER_STATIONARY_RADIUS      ( 1.8f )
#define BUMPER_EXIT_TARGET_TIME       ( 18.0f )
#define BUMPER_EXIT_ANIM_TARGET_TIME  ( 18.0f / 60.0f )
#define BUMPER_STATIONARY_SPAWN_TARGET_TIME  ( 24.0f )

#define BUMPER_SPEED           ( 2.0f )
#define BUMPER_BOUNCE_SPEED    ( 20.0f )
#define BUMPER_FRICTION        (  2.0f )

#define BUMPER_BOUNCE_TARGET_TIME  ( 1.0f )

#define BUMPER_BASE_COLOR             ( ToColor(   0, 100, 0 ) )
#define BUMPER_STATIONARY_BASE_COLOR  ( ToColor( 100, 100, 0 ) )

#define BUMPER_MAX_COUNT  ( 512 )
struct BUMPER {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Direction;
    vec2   PrevPos;
    vec2   PrevdPos;
    flo32  Radius;
    int32  Health;
    
    boo32  IsStationary;
    flo32  Exit_Timer;
    boo32  DoExitAnim;
    flo32  LastBump_Timer;
};

struct BUMPER_STATE {
    boo32 Spawn_IsActive;
    flo32 Spawn_Timer;
    
    int32 BonusObjective_nBumperKills;
    int32 BasePoints;
    
    int32  nBumper;
    BUMPER  Bumper[ BUMPER_MAX_COUNT ];
};

//----------
// DRIFTER
//----------

#define DRIFTER_RADIUS    ( 0.8f )
#define DRIFTER_FRICTION  ( 2.0f )

#define DRIFTER_REPEL_RADIUS  ( 1.0f )
#define DRIFTER_REPEL_SPEED   ( 1.0f )

#define DRIFTER_ANIM_CYCLE_TARGET_TIME  ( 4.0f )

#define DRIFTER_BASE_COLOR  ( Vec4( 0.75f, 0.375f, 0.0f, 1.0f ) )


#define DRIFTER_MAX_COUNT  ( 256 )
struct DRIFTER {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    
    flo32  Move_Radians;
    flo32  Move_Speed;
    flo32  Move_Timer;
    flo32  Move_TargetTime;
    
    flo32  Anim_Timer;
    
    flo32  Radius;
    int32  Health;
    
    flo32  Blink_Timer;
    flo32  Blink_TargetTime;
};

struct DRIFTER_STATE {
    int32   nDrifter;
    DRIFTER  Drifter[ DRIFTER_MAX_COUNT ];
};

//----------
// LOOPER
//----------

#define LOOPER_RADIUS    ( 0.8f )
#define LOOPER_FRICTION  ( 2.0f )

#define LOOPER_COLLISION_RADIUS  ( 0.6f )

#define LOOPER_REPEL_RADIUS  ( 1.0f )
#define LOOPER_REPEL_SPEED   ( 1.0f )

#define LOOPER_LOOP_CYCLE_TARGET_TIME  ( 1.0f )
#define LOOPER_LOOP_RADIUS             ( 0.9f )

#define LOOPER_EXIT_TARGET_TIME         ( 8.0f )
#define LOOPER_EXIT_WARMUP_TARGET_TIME  ( 200.0f / 60.0f )

#define LOOPER_BASE_COLOR  ( Vec4( 0.625f, 0.0f, 0.125f, 1.0f ) )

#define LOOPER_MAX_COUNT  ( 256 )
struct LOOPER {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    
    flo32  Radians;
    flo32  Speed;
    flo32  Change_Timer;
    flo32  Change_TargetTime;
    
    flo32  Loop_Timer;
    flo32  Loop_Radius;
    
    int32  Health;
    
    flo32  Blink_Timer;
    flo32  Blink_TargetTime;
    
};

struct LOOPER_STATE {
    int32  nLooper;
    LOOPER  Looper[ LOOPER_MAX_COUNT ];
};

//----------
// STARBURST
//----------

#define STARBURST_RADIUS    ( 0.75f )

#define STARBURST_COLLISION_RADIUS    ( 0.55f )

#define STARBURST_SPEED     ( 16.0f )
#define STARBURST_FRICTION  ( 1.5f )

#define STARBURST_REPEL_RADIUS  ( 0.7f )
#define STARBURST_REPEL_SPEED   ( 2.0f )

#define STAR_RADIUS          ( 0.35f )
#define STAR_EXPLODE_RADIUS  ( 2.0f  )

#define STAR_SPEED     ( 10.0f )
#define STAR_FRICTION  ( 1.5f )

#define STAR_REPEL_RADIUS  ( 0.7f )
#define STAR_REPEL_SPEED   ( 2.0f )

struct STARBURST {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    
    int32 Health;
};

struct STAR {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    
    int32 Health;
};

#define STARBURST_MAX_COUNT  ( 512  )
#define STAR_MAX_COUNT       ( 2048 )
struct STARBURST_STATE {
    uint32   nStarburst;
    STARBURST Starburst[ STARBURST_MAX_COUNT ];
    
    uint32   nStar;
    STAR      Star[ STAR_MAX_COUNT ];
};

//----------
// AUTO_TURRET
//----------

#define AUTO_TURRET_RADIUS             ( 1.5f )
#define AUTO_TURRET_ROTATION_SPEED     ( TAU / 8.0f )
#define AUTO_TURRET_ZERO_POINT_RADIUS  ( 6.5f )

#define AUTO_TURRET_REPEL_RADIUS     ( 1.4f )
#define AUTO_TURRET_REPEL_SPEED      ( 12.0f )
#define AUTO_TURRET_FRICTION         ( 14.0f )

#define AUTO_TURRET_DAMAGE_BOOST_SPEED        ( 72.0f )

#define AUTO_TURRET_FIRE_COOLDOWN_TARGET_TIME  ( 4.0f / 60.0f )
#define AUTO_TURRET_FIRE_ANIM_TARGET_TIME      ( 2.0f / 60.0f )
#define AUTO_TURRET_FIRE_RADIANS_RANGE         ( PI * 0.6f )
#define AUTO_TURRET_FIRE_RADIANS_HALF_RANGE    ( AUTO_TURRET_FIRE_RADIANS_RANGE * 0.5f )
#define AUTO_TURRET_FIRE_RADIUS                ( 30.0f )

#define AUTO_TURRET_BASE_COLOR   ( Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) )
#define AUTO_TURRET_LASER_WIDTH  ( 0.25f )

struct AUTO_TURRET {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    flo32  Radians;
    
    boo32  FireRange_ShowUI;
    boo32  ZeroPoint_ShowUI;
    boo32  ZeroPoint_IsActive;
    flo32  ZeroPoint_Dist;
    flo32  ZeroPoint_Radians;
    
    flo32  Fire_Timer;
    flo32  Fire_Anim_Timer;
    flo32  Fire_Anim_Radians;
    vec2   Fire_Anim_Position;
    
    boo32  DrawCollision;
    vec2   DrawCollision_P;
    vec2   DrawCollision_N;
    
    flo32  ShowDamage_Timer;
};

#define AUTO_TURRET_MAX_COUNT  ( 2 )
struct AUTO_TURRET_STATE {
    flo32       Powerup_Timer;
    flo32       Powerup_Rate;
    
    uint32     nAutoTurret;
    AUTO_TURRET AutoTurret[ AUTO_TURRET_MAX_COUNT ];
};

//----------
// PERSONAL_POD
//----------

#define PERSONAL_POD_RADIUS_X   ( 1.2f )
#define PERSONAL_POD_RADIUS_Y   ( 0.6f )
#define PERSONAL_POD_RADIUS     ( Vec2( PERSONAL_POD_RADIUS_X, PERSONAL_POD_RADIUS_Y )  )
#define PERSONAL_POD_FRICTION   ( 0.5f  )
#define PERSONAL_POD_SPEED      ( 26.0f )

#define PERSONAL_POD_BAY_DOORS_TARGET_TIME  ( 120.0f / 60.0f )

#define PERSONAL_POD_BASE_COLOR  ( COLOR_GRAY( 0.25f ) )

struct PERSONAL_POD {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Dest;
    flo32  Friction;
    
    flo32  ColorT;
    int32  Health;
};

enum BAY_DOOR_EVENT_TYPE {
    BayDoorEventType_PersonalPod,
    BayDoorEventType_Transport,
    BayDoorEventType_MiniBoss,
    BayDoorEventType_Yacht,
    
    BayDoorEventType_Count,
};

#define BAY_DOOR_EVENT_TYPE_LABELS  char * BayDoorEventTypeLabels[ BayDoorEventType_Count ] = { "FGT", "TRN", "BSS", "YHT" };

struct BAY_DOOR_EVENT { // sizeof( BAY_DOOR_EVENT ) = 16 Bytes
    BAY_DOOR_EVENT_TYPE Type;
    int32 iSrc;
    int32 iDest;
    flo32 TargetTime;
};

enum BAY_DOOR_EVENT_STAGE {
    BayDoorEventStage_Wait,
    BayDoorEventStage_OpenBayDoors,
    BayDoorEventStage_SpawnEntities,
    BayDoorEventStage_ClearBayDoors,
    BayDoorEventStage_CloseBayDoors,
    BayDoorEventStage_Complete,
    BayDoorEventStage_EndLevel,
    
    BayDoorEventStage_Count,
};

#define BAY_DOOR_SPAWN_PERSONAL_POD_TARGET_TIME_SLOW    ( 6.0f / 60.0f )
#define BAY_DOOR_SPAWN_PERSONAL_POD_TARGET_TIME_FAST    ( 3.0f / 60.0f )
#define BAY_DOOR_SPAWN_PERSONAL_POD_INIT_SPEED     ( 10.0f )
#define BAY_DOOR_SPAWN_TRANSPORT_INIT_SPEED   ( 6.0f )
#define BAY_DOOR_SPAWN_MINIBOSS_INIT_SPEED    ( 4.0f )
#define BAY_DOOR_SPAWN_YACHT_INIT_SPEED       ( 1.0f )
struct BAY_DOOR_SPAWN {
    int32       WaveGuid;
    
    BAY_DOOR_EVENT_TYPE  Type;
    BAY_DOOR_EVENT_STAGE Stage;
    
    flo32       Timer;
    flo32       TargetTime;
    
    int32       iSrc;
    int32       iDest;
    
    orect       Src;
    orect       Dest;
    
    flo32       Spawn_Timer;
    flo32       Spawn_TargetTime;
    flo32       BayDoors_AnimT;
};

#define TRANSPORT_WIDTH   ( 2.5f )
#define TRANSPORT_LENGTH  ( 5.0f )
#define TRANSPORT_DIM     ( Vec2( TRANSPORT_WIDTH, TRANSPORT_LENGTH ) )
#define TRANSPORT_RADIUS  ( TRANSPORT_WIDTH * 0.5f )

#define TRANSPORT_FRICTION   ( 0.25f )
#define TRANSPORT_SPEED      ( 10.0f )


#define TRANSPORT_HEALTH      ( 6 )
#define TRANSPORT_BASE_COLOR  ( COLOR_GRAY( 0.18f ) )

struct TRANSPORT {
    int32 WaveGuid;
    
    vec2  Position;
    vec2  Velocity;
    vec2  Dest;
    
    flo32 ColorT;
    
    flo32 Radius;
    int32 Health;
};

#define MINIBOSS_RADIUS              (  3.0f )
#define MINIBOSS_SPEED               ( 10.0f )
#define MINIBOSS_FRICTION            (  2.5f )
#define MINIBOSS_DAMAGE_BOOST_SPEED  (  9.0f )

#define MINIBOSS_REPEL_RADIUS  ( 3.5f )
#define MINIBOSS_REPEL_SPEED   ( 6.0f )

struct MINIBOSS {
    int32 WaveGuid;
    
    vec2  Position;
    vec2  Velocity;
    vec2  Repel;
    
    int32 Health;
    
    flo32 ColorT;
    
    flo32 iTarget_Timer;
    flo32 iTarget_TargetTime;
    
    int32         iTarget;
    ENTITY_SHIELD Shield;
    
#define MINIBOSS_SHIELD_MAX_T              ( 1.5f )
#define MINIBOSS_SHIELD_SEGMENT_MAX_COUNT  ( 128 )
    flo32 Shield_SegmentT[ MINIBOSS_SHIELD_SEGMENT_MAX_COUNT ];
};


#define YACHT_HEALTH              ( 3 )

#define YACHT_RADIUS              ( 1.75f )
#define YACHT_SPEED               (  6.0f )
#define YACHT_FRICTION            (  3.0f )
#define YACHT_EXIT_TARGET_TIME    (  2.0f )
#define YACHT_DAMAGE_BOOST_SPEED  ( 20.0f )
#define YACHT_DAMAGE_COOLDOWN_TARGET_TIME  ( 44.0f / 60.0f )

#define YACHT_BASE_COLOR  ( Vec4( 0.75f, 0.625f, 0.03125f, 1.0f ) )

struct YACHT {
    int32 WaveGuid;
    
    vec2  Position;
    vec2  Velocity;
    vec2  Dest;
    
    int32 Health;
    
    flo32 ColorT;
    flo32 Exit_Timer;
    
#define YACHT_DAMAGE_IFRAME_TARGET_TIME  ( 4.0f / 60.0f )
    flo32 Damage_IFrameTimer;
    
#define YACHT_LIGHT_CYCLE_TARGET_TIME        ( 12.0f / 60.0f )
#define YACHT_LIGHT_TARGET_TIME              ( YACHT_LIGHT_CYCLE_TARGET_TIME * 4.0f )
#define YACHT_SHOW_DAMAGE_CYCLE_TARGET_TIME  ( 12.0f / 60.0f )
#define YACHT_SHOW_DAMAGE_TARGET_TIME        ( YACHT_SHOW_DAMAGE_CYCLE_TARGET_TIME * 11.0f )
    flo32 Light_Timer;
    flo32 ShowDamage_Timer;
};

#define PERSONAL_POD_MAX_COUNT             ( 512 )
#define BAY_DOOR_SPAWN_MAX_COUNT       ( 5 )
#define BAY_DOOR_SPAWN_SRC_MAX_COUNT   ( BAY_DOOR_SPAWN_MAX_COUNT )
#define BAY_DOOR_SPAWN_DEST_MAX_COUNT  ( 3 )
#define TRANSPORT_MAX_COUNT           ( 32 )
#define MINIBOSS_MAX_COUNT            ( 2 )
#define YACHT_MAX_COUNT               ( 2 )

#define BAY_DOOR_SPAWN_PERSONAL_POD_COOLDOWN_TARGET_TIME  ( 310.0f / 60.0f )
#define BAY_DOOR_SPAWN_TRANSPORT_COOLDOWN_TARGET_TIME     ( 12.0f )
#define BAY_DOOR_SPAWN_TRANSPORT_TARGET_TIME              (  2.0f )

#define BAY_DOOR_SPAWN_TRANSPORT_YACHT_SPAWN_COUNT  ( 10 )
#define BAY_DOOR_SPAWN_PERSONAL_POD_SPAWN_COUNT     ( 24 )

struct YACHT_EXIT_RING_DATA {
    flo32 Radians;
    flo32 RotateSpeed;
    flo32 Radius;
    flo32 tColor;
    flo32 Alpha;
};

struct BAY_DOOR_STATE {
    boo32 SpawnPersonalPod_IsActive;
    flo32 SpawnPersonalPod_Timer;
    flo32 SpawnPersonalPod_MiniSpawnTimer;
    flo32 SpawnPersonalPod_TargetTimeForLevel;
    
    boo32 SpawnTransport_IsActive;
    flo32 SpawnTransport_Timer;
    int32 BonusObjective_nYachtKills;
    int32 BonusObjective_nYachtMisses;
    
    int32 SpawnPersonalPod_iStage;
    int32 SpawnPersonalPod_nPersonalPod[ BAY_DOOR_SPAWN_PERSONAL_POD_SPAWN_COUNT ];
    
    int32 SpawnTransport_iStage;
    boo32 SpawnTransport_DoYacht[ BAY_DOOR_SPAWN_TRANSPORT_YACHT_SPAWN_COUNT ];
    
    YACHT_EXIT_RING_DATA YachtExit_InnerRing;
    YACHT_EXIT_RING_DATA YachtExit_MidRing;
    flo32 YachtExit_tFlash;
    flo32 YachtExit_ParticleTimer;
    
    int32         nBayDoorSpawn;
    BAY_DOOR_SPAWN BayDoorSpawn          [ BAY_DOOR_SPAWN_MAX_COUNT ];
    orect          BayDoorSpawn_Src      [ BAY_DOOR_SPAWN_SRC_MAX_COUNT  ];
    orect          BayDoorSpawn_Dest     [ BAY_DOOR_SPAWN_DEST_MAX_COUNT ];
    flo32          BayDoorSpawn_DestAlpha[ BAY_DOOR_SPAWN_DEST_MAX_COUNT ];
    flo32          BayDoorSpawn_Radians;
    
    int32         nPersonalPod;
    PERSONAL_POD   PersonalPod[ PERSONAL_POD_MAX_COUNT ];
    
    int32         nTransport;
    TRANSPORT      Transport[ TRANSPORT_MAX_COUNT ];
    
    //int32         nMiniBoss;
    //MINIBOSS       MiniBoss[ MINIBOSS_MAX_COUNT ];
    
    int32         nYacht;
    YACHT          Yacht[ YACHT_MAX_COUNT ];
};

//----------
// ASTEROID
//----------

#define ASTEROID_CLASS_RADIUS  flo32 AsteroidClassRadius[ AsteroidClass_Count ] = { 1.0f, 2.0f, 3.0f, 4.0f };
#define ASTEROID_CLASS_HEALTH  int32 AsteroidClassHealth[ AsteroidClass_Count ] = { 1, 5, 20, 50 };

enum ASTEROID_CLASS {
    AsteroidClass_Tiny,
    AsteroidClass_Small,
    AsteroidClass_Medium,
    AsteroidClass_Large,
    
    AsteroidClass_Count,
};

enum ASTEROID_TYPE {
    AsteroidType_Default,
    AsteroidType_BombOnly,
    AsteroidType_LaserOnly,
    AsteroidType_SwarmOnly,
    
    AsteroidType_Count,
};

#define ASTEROID_FLAG__CAN_SPAWN_CHILDREN          ( 0x0001 )
#define ASTEROID_FLAG__CAN_USE_ZERO_POINT_ENERGY   ( 0x0002 )
#define ASTEROID_FLAG__BOMB_ONLY                   ( 0x0004 )
#define ASTEROID_FLAG__LASER_ONLY                  ( 0x0008 )
#define ASTEROID_FLAG__ENABLE_DEFAULT_POINT_SPAWN  ( 0x0010 )
#define ASTEROID_FLAG__SWARM_ONLY                  ( 0x0020 )

#define ASTEROID_ZERO_POINT_RADIUS  ( 9.0f )

#define ASTEROID_HI_DAMAGE_SPEED_THRESHOLD  ( 12.0f )

#define ASTEROID_DEFAULT_BASE_COLOR     ( Vec4( 0.625f, 0.0f,  0.75f,  1.0f ) )
#define ASTEROID_SWARM_ONLY_BASE_COLOR  ( Vec4( 0.0f,   1.0f,  1.0f,   1.0f ) )

#define ASTEROID_TEXTURE_ID_MAX_COUNT  ( 8 )

struct ASTEROID {
    int32      WaveGuid;
    int32      Guid;
    TEXTURE_ID TextureID;
    
    ASTEROID_CLASS Class;
    
    vec2   Position;
    vec2   Direction;
    flo32  Speed;
    flo32  Radius;
    
    flo32  Radians;
    flo32  dRadians;
    
    boo32  ZeroPoint_ShowUI;
    boo32  ZeroPoint_IsActive;
    flo32  ZeroPoint_Dist;
    flo32  ZeroPoint_Radians;
    
    int32  Health;
    uint32 Flags;
    flo32  tColor;
    flo32  Damage_Timer;
};

struct ASTEROID_SETTINGS_EVENT {
    int32 Spawn_nFrames;
    int32 Spawn_MaxAsteroid;
    int32 Numer_TypeClass[ AsteroidType_Count ][ AsteroidClass_Count ];
    
    boo32 Spawn_SideIsEnabled[ 4 ];
    rect  Spawn_Bound;
};

#define ASTEROID_MAX_COUNT  ( 256 )
struct ASTEROID_STATE {
    boo32 Spawn_IsActive;
    flo32 Spawn_Timer;
    flo32 Spawn_TargetTime;
    int32 Spawn_MaxAsteroid;
    
    int32 Numer_TypeClass[ AsteroidType_Count ][ AsteroidClass_Count ];
    
    boo32 Spawn_SideIsEnabled[ 4 ];
    rect  Spawn_Bound;
    
    int32 CurrentGuid;
    
#define ASTEROID_COLLIDE_EXCLUDE_MAX_COUNT  ( 256 )
    uint32      Collide_nExclude;
    UINT32_PAIR Collide_iExclude[ ASTEROID_COLLIDE_EXCLUDE_MAX_COUNT ];
    
    int32   nAsteroid;
    ASTEROID Asteroid[ ASTEROID_MAX_COUNT ];
};

//----------
// MINER
//----------

#define MINER_HEALTH    ( 75 )

#define MINER_RADIUS_AUTONOMOUS    ( 2.0f )
#define MINER_RADIUS_STATIONARY    ( 2.5f )
#define MINER_FRICTION  ( 2.0f )

#define MINER_LASER_WIDTH   ( 0.4f )
#define MINER_AUTONOMOUS_ROTATE_SPEED  ( TAU / 24.0f )
#define MINER_AUTONOMOUS_LASER_LENGTH  ( 20.0f )

#define MINER_STATIONARY_ROTATE_SPEED  ( TAU / 16.0f )
#define MINER_STATIONARY_LASER_LENGTH  ( 128.0f )

#define MINER_REPEL_RADIUS     ( 2.5f )
#define MINER_REPEL_SPEED      ( 12.0f )

#define MINER_ROTATE_SPEED_TARGET_PLAYER  ( TAU / 28.0f )

#define MINER_STATIONARY_LASER_INIT_TARGET_TIME  ( 120.0f / 60.0f )
#define MINER_LASER_WARMUP_TARGET_TIME     ( 120.0f / 60.0f )
#define MINER_LASER_FIRE_TARGET_TIME       ( 240.0f / 60.0f )
#define MINER_LASER_ONLY_KILL_TARGET_TIME  (   6.0f / 60.0f )
#define MINER_LASER_DAMAGE_PER_SECOND      ( 50.0f )
#define MINER_LASER_DAMAGE_TARGET_TIME     ( 1.0f / MINER_LASER_DAMAGE_PER_SECOND )

struct MINER_LASER {
    boo32 IsActive;
    vec2  Dest;
    flo32 Radians;
};

#define MINER_AUTONOMOUS_BASE_COLOR  ( Vec4( 0.625f, 0.375f,  0.0f,     1.0f ) )
#define MINER_STATIONARY_BASE_COLOR  ( Vec4( 0.75f,  0.625f, 0.03125f, 1.0f ) )

struct MINER {
    int32  WaveGuid;
    
    boo32  IsAutonomous;
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    flo32  Radius;
    
    flo32  Damage_Timer;
    int32  Health;
    
    flo32  AtRadians;
    flo32  DestRadians;
    flo32  RotateSpeed;
    
    boo32  Fire_IsActive;
    flo32  Fire_Timer;
    flo32  Fire_TargetTime;
    
    flo32  LaserDamage_Timer;
    flo32  Laser_MaxLength;
    
    flo32  Move_Radians;
    flo32  Move_Speed;
    flo32  Move_Timer;
    flo32  Move_TargetTime;
    
    boo32  DoLaserOnlyKill;
    int32  DoLaserOnlyKill_iLaser;
    flo32  DoLaserOnlyKill_Timer;
    
#define MINER_LASER_MAX_COUNT  ( 4 )
    int32      nLaser;
    MINER_LASER Laser[ MINER_LASER_MAX_COUNT ];
};

#define MEGA_MINER_REPEL_RADIUS     ( 2.5f )
#define MEGA_MINER_RADIUS  ( 3.0f )
#define MEGA_MINER_HEALTH  ( 175 )

#define MEGA_MINER_DRILL_OFFSET  ( MEGA_MINER_RADIUS * 1.0f )
#define MEGA_MINER_DRILL_SCALE   ( Vec2( MEGA_MINER_RADIUS + 0.25f, MEGA_MINER_RADIUS * 0.5f - 0.25f ) )

#define MEGA_MINER_SPEED     ( 4.0f )
#define MEGA_MINER_FRICTION  ( 2.0f )

#define MEGA_MINER_ROTATE_SPEED  ( TAU / 24.0f )
#define MEGA_MINER_LASER_DAMAGE_PER_SECOND      ( 2.5f )
#define MEGA_MINER_LASER_DAMAGE_TARGET_TIME     ( 1.0f / MEGA_MINER_LASER_DAMAGE_PER_SECOND )

#define MEGA_MINER_BASE_COLOR  ( Vec4( 0.5f, 0.4f, 0.0f, 1.0f ) )

struct MEGA_MINER_EVENT {
    uint32 _Padding;
};

struct MEGA_MINER {
    int32 WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    flo32  Radius;
    
    int32  Health;
    
    flo32  AtRadians;
    flo32  DestRadians;
    flo32  RotateSpeed;
    
    flo32  LaserDamage_Timer;
    flo32  ShowDamage_Timer;
    
    flo32  Rumble_Radians;
    
#define MEGA_MINER_DRILL_ANIM_TARGET_TIME  ( 20.0f / 60.0f )
    flo32  Drill_Timer;
    
#define MEGA_MINER_EXHAUST_MAX_COUNT  ( 4 )
    flo32 Exhaust_TargetTime[ MEGA_MINER_EXHAUST_MAX_COUNT ];
    flo32 Exhaust_Timer     [ MEGA_MINER_EXHAUST_MAX_COUNT ];
};

struct LASER {
    boo32 IsActive;
    vec2  Position;
    flo32 Radians;
    flo32 MaxLength;
    vec2  Dest;
    
    flo32 Timer;
    flo32 Fire_TargetTime;
    flo32 Reset_TargetTime;
};

#define MINER_MAX_COUNT       ( 32 )
#define MEGA_MINER_MAX_COUNT  ( 2 )
struct MINER_STATE {
    boo32 BonusObjective_DoNext;
    flo32 BonusObjective_DoNext_Timer;
    int32 BonusObjective_nMinerKills;
    int32 BonusObjective_nMegaMinerKills;
    
    int32 nMiner;
    MINER  Miner[ MINER_MAX_COUNT ];
    
    int32     nMegaMiner;
    MEGA_MINER MegaMiner[ MEGA_MINER_MAX_COUNT ];
    
#define LASER_MAX_COUNT  ( 4 )
    int32 nLaser;
    LASER  Laser[ LASER_MAX_COUNT ];
};

//----------
// BOMBER
//----------

#define BOMBER_HEALTH    ( 75 )

#define BOMBER_RADIUS    ( 2.0f )
#define BOMBER_FRICTION  ( 2.0f )

#define BOMBER_REPEL_RADIUS     ( 2.5f )
#define BOMBER_REPEL_SPEED      ( 12.0f )

#define BOMBER_LAUNCH_WARMUP_TARGET_TIME    ( 100.0f / 60.0f )

struct BOMBER {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    flo32  Radius;
    
    int32  Health;
    
    flo32  AtRadians;
    
    flo32  Launch_Timer;
    flo32  Launch_TargetTime;
    
    flo32  LaserDamage_Timer;
    
    boo32  CanMove;
    flo32  Move_Radians;
    flo32  Move_Speed;
    flo32  Move_Timer;
    flo32  Move_TargetTime;
    
#define BOMBER_BOMB_MAX_COUNT  ( 4 )
    int32 nBomb;
    flo32  Bomb_Radians[ BOMBER_BOMB_MAX_COUNT ];
};

#define BOMB_RADIUS    ( 0.8f )
#define BOMB_SPEED     ( 6.0f )
//#define BOMB_FRICTION  ( 1.5f )

#define BOMB_DETONATE_TARGET_TIME  ( 70.0f / 60.0f )
#define BOMB_DETONATE_RADIUS       ( 6.5f )

#define BOMB_DETONATE_BOMB_TARGET_TIME  ( 20.0f / 60.0f )

#define BOMB_DAMAGE_TARGET_TIME  ( 4.0f / 60.0f )
#define BOMB_DAMAGE_RADIUS       ( 6.0f )

#define BOMB_BASE_COLOR  ( Vec4( 0.25f, 0.25f, 0.25f, 1.0f ) )
#define BOMB_LIGHT_CYCLE_TARGET_TIME  ( 30.0f / 60.0f )

#define BOMB_LAUNCHER_BASE_COLOR  ( Vec4( 0.0f, 0.25f, 0.25f, 1.0f ) )

struct BOMB {
    int32 WaveGuid;
    
    vec2  Position;
    vec2  Velocity;
    flo32 Radius;
    
    boo32 IsTriggered;
    boo32 IsDetonated;
    boo32 DoDamage;
    flo32 Timer;
    flo32 Light_Timer;
    
    boo32 IsTractorBeamed;
    flo32 IsTractorBeamed_Radians;
};

#define BOMBER_MAX_COUNT  ( 64 )
#define BOMB_MAX_COUNT    ( 128 )
struct BOMB_STATE {
    //int32 nBomber;
    //BOMBER Bomber[ BOMBER_MAX_COUNT ];
    
    int32 nBomb;
    BOMB   Bomb[ BOMB_MAX_COUNT ];
};

//----------
// BOMB LAUNCHER
//----------

struct BOMB_LAUNCHER_EVENT { // sizeof( BOMB_LAUNCHER_EVENT ) = 4 Bytes
    int32 iSrc;
};

enum BOMB_LAUNCHER_STAGE {
    BombLauncherStage_Wait,
    BombLauncherStage_EnableBombLauncher,
    BombLauncherStage_SpawnBombs,
    BombLauncherStage_IsDestroyed,
    
    BombLauncherStage_Count,
};


#define BOMB_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME    ( 40.0f / 60.0f )
#define BOMB_LAUNCHER_SPAWN_ANIM_TARGET_TIME        ( 80.0f  / 60.0f )
#define BOMB_LAUNCHER_SPAWN_ANIM_HEIGHT             ( 8.0f )
#define BOMB_LAUNCHER_MOVE_DELAY_TARGET_TIME       ( 60.0f / 60.0f )

#define BOMB_LAUNCHER_RADIUS  ( 2.0f )
#define BOMB_LAUNCHER_HEALTH  ( 75 )

struct BOMB_LAUNCHER {
    int32               WaveGuid;
    
    BOMB_LAUNCHER_STAGE Stage;
    vec2                Position;
    flo32               Radius;
    int32               Health;
    flo32               Radians;
    
    flo32               Damage_Timer;
    flo32               Launch_Timer;
    flo32               Launch_TargetTime;
    
    vec2                Anim_Direction;
    flo32               Anim_Timer;
    
    boo32               CanMove;
    flo32               CanMove_Timer;
    flo32               CanMove_Dir;
};

#define BOMB_LAUNCHER_MAX_COUNT  ( 4 )
struct BOMB_LAUNCHER_STATE {
    int32          BonusObjective_nBombLauncherKills;
    int32          BonusObjective_nBombLauncherSpawns;
    int32          BonusObjective_MaxBombLauncherToSpawn;
    flo32          BonusObjective_SpawnCooldownTimer;
    
    int32         nBombLauncher;
    BOMB_LAUNCHER  BombLauncher[ BOMB_LAUNCHER_MAX_COUNT ];
};

//----------
// BANK
//----------

#define BANK_RADIUS    ( 2.5f )
#define BANK_FRICTION  ( 2.0f )

#define BANK_REPEL_RADIUS     ( 1.0f )
#define BANK_REPEL_SPEED      ( 1.0f )

#define BANK_EXIT_TARGET_TIME  ( 6.0f )

#define BANK_BASE_COLOR  ( Vec4( 0.8f, 0.8f, 0.0f, 1.0f ) )

struct BANK {
    int32  WaveGuid;
    
    vec2   Position;
    flo32  Radius;
    
    int32  Health;
    int32  Points;
    
    flo32  Exit_Timer;
};

#define BANK_MAX_COUNT  ( 4 )
struct BANK_STATE {
    int32 nBank;
    BANK   Bank[ BANK_MAX_COUNT ];
};

//----------
// BLIMP
//----------

#define BLIMP_RADIUS    ( 5.0f )
#define BLIMP_FRICTION  ( 2.0f )

#define BLIMP_HEALTH    ( 150 )

#define BLIMP_BASE_COLORA  ( Vec4( 0.65f, 0.275f, 0.0f, 1.0f ) )
#define BLIMP_BASE_COLORB  ( Vec4( 0.75f, 0.15f, 0.35f, 1.0f ) )
#define BLIMP_SEGMENT_MAX_COUNT  ( 24 )

struct BLIMP_EVENT {
    uint32 _Padding;
};

struct BLIMP {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    flo32  Radius;
    
    int32  Health;
    
    vec2   Move_Dest;
    int32  Move_iDest;
    flo32  Move_Speed;
    flo32  Move_Timer;
    flo32  Move_TargetTime;
    
    flo32  Damage_Timer;
    
#define BLIMP_DISPLAY_SEGMENT_TARGET_TIME  ( 40.0f / 60.0f )
    int32  Display_iSegment;
    flo32  Display_Timer;
};

#define BLIMP_MAX_COUNT  ( 1 )
struct BLIMP_STATE {
#define BLIMP_MOVE_DEST_MAX_COUNT  ( 4 )
    int32   BonusObjective_nBlimpKills;
    
    int32  nMoveDest;
    rect    MoveDest[ BLIMP_MOVE_DEST_MAX_COUNT ];
    
    int32  nBlimp;
    BLIMP   Blimp[ BLIMP_MAX_COUNT ];
};

//----------
// RECON
//----------

#define RECON_RADIUS        ( 1.0f )
#define RECON_SIDE_LENGTH   ( 4.0f )
#define RECON_ROTATE_SPEED  ( TAU / 6.0f )
#define RECON_POSITION_OFFSET  ( 0.25f )

#define RECON_HEALTH        ( 1 )

#define RECON_BASE_COLOR  ( COLOR_GRAY( 0.1f ) )

#define RECON_DEATH_ANIM_TARGET_TIME  ( 4.0f / 60.0f )
#define RECON_BLINK_LIGHT_COLOR       ( Vec4( 0.75f, 0.625f, 0.0f, 1.0f ) )

struct RECON_PART {
    vec2  Position;
    vec2  Velocity;
    flo32 Radians;
    flo32 dRadians;
    flo32 Timer;
    
    flo32 Blink_TimerA;
    flo32 Blink_TimerB;
    
    flo32 Blink_TargetTimeA;
    flo32 Blink_TargetTimeB;
};

struct RECON {
    int32  WaveGuid;
    
    vec2   bPos;
    vec2   Position;
    flo32  Radians;
    
    flo32  Timer;
    int32  Health;
    
    flo32  PosX_Timer;
    flo32  PosX_TargetTime;
    flo32  PosX_Rate;
    flo32  PosX_Radians;
    
    flo32  PosY_TargetTime;
    flo32  PosY_Timer;
    flo32  PosY_Rate;
    flo32  PosY_Radians;
    
#define RECON_PART_MAX_COUNT  ( 3 )
    RECON_PART Part[ RECON_PART_MAX_COUNT ];
};

#define LEVEL01B_BONUS_OBJECTIVE_RECON_X_COUNT  ( 11 )
#define LEVEL01B_BONUS_OBJECTIVE_RECON_Y_COUNT  (  7 )
#define LEVEL01B_BONUS_OBJECTIVE_RECON_COUNT    ( LEVEL01B_BONUS_OBJECTIVE_RECON_X_COUNT * 2 + LEVEL01B_BONUS_OBJECTIVE_RECON_Y_COUNT * 2  )

#define RECON_MAX_COUNT  ( 64 )
struct RECON_STATE {
    int32   BonusObjective_nReconKills;
    
    int32  nRecon;
    RECON   Recon[ RECON_MAX_COUNT ];
};

//----------
// SATELLITE
//----------

#define SATELLITE_RADIUS    ( 3.5f )
#define SATELLITE_HEALTH    ( 150 )

#define SATELLITE_ANIM_ROTATION_SPEED    ( TAU / 6.0f )
#define SATELLITE_ANIM_BLIP_TARGET_TIME  ( 4.0f / 60.0f )
#define SATELLITE_ANIM_TARGET_TIME       ( 140.0f / 60.0f )

#define SATELLITE_SHIELD_DOWN_TARGET_TIME       ( 80.0f / 60.0f )
#define SATELLITE_SHIELD_WORLD_BOUND_OFFSET     ( SATELLITE_RADIUS + 4.0f )
#define SATELLITE_MOVE_INIT_OFFSET              ( 16.0f )

#define SATELLITE_BASE_COLOR  ( Vec4( 0.75f, 0.625f, 0.03125f, 1.0f ) )
#define SATELLITE_SHIELD_BASE_COLOR  ( Vec4( 0.0f, 1.0f, 1.0f, 1.0f ) )

struct SATELLITE {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    flo32  Radius;
    
    int32  Health;
    flo32  Anim_Radians;
    flo32  Anim_Timer;
    
    flo32  Move_CooldownTimer;
    flo32  Damage_Timer;
    
    boo32  IsDead;
    flo32  IsDead_Timer;
    
    flo32  Spark_Timer;
    flo32  Spark_TargetTime;
};

struct SATELLITE_SHIELD_HIT {
    vec2  Pos;
    flo32 Scale;
    flo32 Timer;
};

#define SATELLITE_MAX_COUNT  ( 1 )
struct SATELLITE_STATE {
    flo32 ShieldDown_Timer;
    rect  Shield_bBound;
    rect  Shield_Bound;
    vec2  Shield_BounceDir;
    rect  Move_Bound;
    
#define SATELLITE_SHIELD_HIT_TARGET_TIME  ( 60.0f / 60.0f )
#define SATELLITE_SHIELD_HIT_MAX_COUNT    ( 32 )
    int32               iShieldHit;
    SATELLITE_SHIELD_HIT ShieldHit[ SATELLITE_SHIELD_HIT_MAX_COUNT ];
    
    int32     BonusObjective_nSatelliteKills;
    
    int32    nSatellite;
    SATELLITE Satellite[ SATELLITE_MAX_COUNT ];
};

//----------
// SMOG
//----------

#define SMOG_RADIUS    ( 1.0f )
#define SMOG_HEALTH    ( 1 )

#define SMOG_BASE_COLOR  ( Vec4( 1.0f, 0.1f, 0.1f, 1.0f ) )

#define SMOG_REPEL_RADIUS     ( 1.25f )
#define SMOG_REPEL_SPEED      ( 4.0f )
#define SMOG_FRICTION         ( 4.0f )

struct SMOG {
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    flo32  Radius;
    int32  Health;
    vec4   Color;
    flo32  ModColor;
};

#define SMOG_MAX_COUNT  ( 512 )
struct SMOG_STATE {
    int32 nSmog;
    SMOG   Smog[ SMOG_MAX_COUNT ];
};

//----------
// ESCORT
//----------

#define ESCORT_RADIUS_LONG   ( 6.0f )
#define ESCORT_RADIUS_SHORT  ( 4.5f )
#define ESCORT_DIM           ( Vec2( ESCORT_RADIUS_SHORT, ESCORT_RADIUS_LONG ) )
#define ESCORT_HEALTH        ( 1 )

#define ESCORT_ENGINE_OFFSET       ( ESCORT_RADIUS_LONG * 0.85f )
#define ESCORT_ENGINE_MIN_SCALE_Y  ( 4.0f )

#define ESCORT_BASE_COLOR    ( Vec4( 0.1f, 0.1f, 0.1f, 1.0f ) )

#define ESCORT_SPEED            ( 1.5f )
#define ESCORT_FRICTION         ( 1.0f )

#define ESCORT_BONUS_POINTS_MAX_INDICATOR  ( 7 )

#define ESCORT_BONUS_POINTS_INDICATOR_TARGET_TIME  flo32 EscortBonusPointsIndicator_TargetTime[ ESCORT_BONUS_POINTS_MAX_INDICATOR + 1 ] = {}; \
EscortBonusPointsIndicator_TargetTime[ 0 ] = 0.0f; \
EscortBonusPointsIndicator_TargetTime[ 1 ] = EscortBonusPointsIndicator_TargetTime[ 0 ] + 0.5f; \
EscortBonusPointsIndicator_TargetTime[ 2 ] = EscortBonusPointsIndicator_TargetTime[ 1 ] + 0.5f; \
EscortBonusPointsIndicator_TargetTime[ 3 ] = EscortBonusPointsIndicator_TargetTime[ 2 ] + 0.75f; \
EscortBonusPointsIndicator_TargetTime[ 4 ] = EscortBonusPointsIndicator_TargetTime[ 3 ] + 0.75f; \
EscortBonusPointsIndicator_TargetTime[ 5 ] = EscortBonusPointsIndicator_TargetTime[ 4 ] + 1.0f; \
EscortBonusPointsIndicator_TargetTime[ 6 ] = EscortBonusPointsIndicator_TargetTime[ 5 ] + 1.25f; \
EscortBonusPointsIndicator_TargetTime[ 7 ] = EscortBonusPointsIndicator_TargetTime[ 6 ] + 2.0f;

struct ESCORT {
    int32  WaveGuid;
    vec2   Position;
    vec2   Velocity;
    int32  Health;
    
    flo32  Engine_AtT  [ 3 ];
    flo32  Engine_DestT[ 3 ];
    flo32  BonusPoints_IndicatorT [ 2 ];
    int32  BonusPoints_AtIndicator[ 2 ];
    flo32  BonusPoints_Timer      [ 2 ];
};

#define ESCORT_MAX_COUNT  ( 4096 )
struct ESCORT_STATE {
    int32 nEscort;
    ESCORT Escort[ ESCORT_MAX_COUNT ];
};

//----------
// ROGUE
//----------

#define ROGUE_HEALTH    ( 75 )

#define ROGUE_RADIUS    ( 2.5f )
#define ROGUE_FRICTION  ( 2.0f )

#define ROGUE_ROTATE_SPEED  ( TAU / 8.0f )

#define ROGUE_REPEL_RADIUS     ( 2.5f )
#define ROGUE_REPEL_SPEED      ( 12.0f )

#define ROGUE_TELEPORT_ANIM_TARGET_TIME  ( 30.0f / 60.0f )

struct ROGUE {
    int32  WaveGuid;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    flo32  Radius;
    flo32  Radians;
    
    int32  Health;
    flo32  Timer;
    
    flo32  Attack_Timer;
    
    boo32  DoTeleport;
    flo32  DoTeleport_Timer;
    boo32  DoTeleport_iPos;
};

#define ROGUE_MAX_COUNT       ( 4 )
struct ROGUE_STATE {
    int32 BonusObjective_nRogueKills;
    
    int32 nRogue;
    ROGUE  Rogue[ ROGUE_MAX_COUNT ];
};

//----------
// MISSILE_LAUNCHER
//----------

#define MISSILE_LAUNCHER_HEALTH    ( 50 )

#define MISSILE_LAUNCHER_RADIUS        ( 2.5f )
#define MISSILE_LAUNCHER_ROTATE_SPEED  ( TAU / 8.0f )

#define MISSILE_LAUNCHER_BASE_COLOR  ( Vec4( 0.0f, 0.25f, 0.25f, 1.0f ) )
#define MISSILE_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME    ( 40.0f / 60.0f )


struct MISSILE_LAUNCHER {
    vec2   Position;
    flo32  Radius;
    flo32  Radians;
    
    int32  Health;
    
    flo32  Launch_Timer;
    flo32  Damage_Timer;
};

#define MISSILE_LAUNCHER_MAX_COUNT       ( 4 )
struct MISSILE_LAUNCHER_STATE {
    int32 BonusObjective_nMissileLauncherKills;
    
    int32 nMissileLauncher;
    MISSILE_LAUNCHER  MissileLauncher[ MISSILE_LAUNCHER_MAX_COUNT ];
};

//----------
// MISSILE
//----------

#define MISSILE_RADIUS         ( 0.2f )
#define MISSILE_LENGTH         ( 0.85f )

#define MISSILE_SPEED           ( 18.0f )
#define MISSILE_FRICTION        (  1.0f )

#define MISSILE_REPEL_RADIUS  ( 0.4f )
#define MISSILE_REPEL_SPEED   ( 2.0f )

#define MISSILE_BASE_COLOR    ( Vec4( 1.0f, 0.9f, 0.5f, 1.0f ) )

#define MISSILE_MAX_COUNT  ( 256 )
struct MISSILE {
    int32 WaveGuid;
    
    vec2  Position;
    vec2  Velocity;
    vec2  Repel;
    int32 Health;
    flo32 Friction;
    
    boo32 DoTargetEscort;
    
#define MISSILE_EXHAUST_WARMUP_TARGET_TIME  ( 20.0f / 60.0f )
    flo32 Exhaust_Timer;
    flo32 Exhaust_TargetTime;
};

struct MISSILE_STATE {
    int32   nMissile;
    MISSILE  Missile[ MISSILE_MAX_COUNT ];
};

//----------
// WORM
//----------

#define WORM_HEALTH_SMALL  (  25 )
#define WORM_HEALTH_LARGE  ( 150 )

#define WORM_RADIUS_SMALL  ( 2.0f )
#define WORM_RADIUS_LARGE  ( 3.5f )
#define WORM_FRICTION  ( 2.0f )

#define WORM_ROTATE_SPEED  ( TAU / 8.0f )

#define WORM_REPEL_RADIUS     ( 2.5f )
#define WORM_REPEL_SPEED      ( 12.0f )

#define WORM_TELEPORT_ANIM_TARGET_TIME  ( 30.0f / 60.0f )

#define WORM_DEATH_CYCLE_TARGET_TIME  ( 20.0f / 60.0f )

#define WORM_MOVEMENT_X_OFFSET              ( 4.0f )
#define WORM_MOVEMENT_Y_OFFSET              ( 2.0f )
#define WORM_MOVEMENT_RATIO_FOR_SMALL_WORM  ( 0.75f )

#define WORM_POINTS_SMALL                   ( 50 )

#define WORM_BASE_COLOR  ( Vec4( 0.3f, 0.15f, 0.15f, 1.0f ) )
#define WORM_TAG_DIM     ( Vec2( 1.5f, 1.0f ) )

#define WORM_ATTACK_WARMUP_TARGET_TIME  ( 20.0f / 60.0f )

enum WORM_CLASS {
    WormClass_Large,
    WormClass_Small,
    
    WormClass_Count,
};

struct WORM {
    int32      WaveGuid;
    WORM_CLASS Class;
    
    flo32  Spawn_Velocity;
    flo32  Spawn_Length;
    flo32  Spawn_MaxLength;
    
    vec2   bPos;
    vec2   Position;
    vec2   Velocity;
    flo32  Radius;
    flo32  Radians;
    
    flo32  tMove;
    flo32  dMove;
    
    int32  nBody;
    vec2   Body_Dir;
    vec2   Body_Pos[ 4 ];
    // Body_Pos[ 0 ] refers to the body closest to the worm head.
    
    vec2   Tag_PosA;
    vec2   Tag_PosB;
    
    int32  Health;
    flo32  Timer;
    
    boo32  DoAttack;
    int32  DoAttack_iAttack;
    int32  DoAttack_Counter;
    flo32  DoAttack_Timer;
    flo32  DoAttack_TargetTime;
    
#define WORM_SHOW_DAMAGE_TARGET_TIME  ( 20.0f / 60.0f )
    flo32  ShowDamage_Timer;
    
    flo32  Death_Timer;
    int32  Death_iBody;
    boo32  CanSpawnScoreMultipliers;
    boo32  CanSpawnPoints;
    
    //CAPSULE2 Cap;
};

struct WORM_SETTINGS_EVENT {
    int32 Spawn_nFrames;
    int32 Spawn_MaxWorm;
};

#define WORM_MAX_COUNT       ( 16 )
struct WORM_STATE {
    int32 BonusObjective_nWormKills;
    int32 BonusObjective_nLargeWormKills;
    
    boo32 Spawn_IsActive;
    int32 Spawn_MaxWorm;
    flo32 Spawn_Cooldown_Timer;
    flo32 Spawn_Cooldown_TargetTime;
    int32 Spawn_nAdditionalSmallWormsAtStart;
    
    boo32 SpawnLarge_IsActive;
    int32 SpawnLarge_nWormsSpawned;
    flo32 SpawnLarge_Cooldown_Timer;
    
    int32 nWorm;
    WORM   Worm[ WORM_MAX_COUNT ];
};

//----------
// FIREBALL
//----------

#define FIREBALL_RADIUS_SMALL   ( 0.75f )
#define FIREBALL_RADIUS_LARGE   ( 1.25f )
#define FIREBALL_SPEED_SMALL    ( 10.0f )
#define FIREBALL_SPEED_LARGE    (  8.0f )

#define FIREBALL_HEALTH_LARGE   ( 5 )

#define FIREBALL_MAX_COUNT  ( 512 )

#define FIREBALL_POINTS_BASE_COLOR      ( Vec4( 0.8f, 0.0f, 0.0f, 1.0f ) )
#define FIREBALL_MULTIPLIER_BASE_COLOR  ( Vec4( 0.0f, 0.8f, 0.0f, 1.0f ) )

enum FIREBALL_TYPE {
    FireballType_Points,
    FireballType_Multiplier,
    
    FireballType_Count,
};

struct FIREBALL {
    int32         WaveGuid;
    FIREBALL_TYPE Type;
    
    vec2  Position;
    vec2  Velocity;
    flo32 Radius;
    int32 Health;
    
    flo32 tColor;
    flo32 tColorDest;
};

struct FIREBALL_STATE {
    int32    nFireball;
    FIREBALL  Fireball[ FIREBALL_MAX_COUNT ];
};

//----------
// SPITTER
//----------

#define SPIT_RADIUS  ( 0.5f )
#define SPIT_SPEED   ( 8.0f )

enum SPIT_TYPE {
    SpitType_Red,
    SpitType_Yellow,
    SpitType_Blue,
    
    SpitType_Count,
};

#define SPIT_BASE_COLORS  vec4 SpitBaseColors[ SpitType_Count ] = { \
Vec4( 1.0f, 0.0f,  0.0f, 1.0f ), \
Vec4( 1.0f, 1.0f,  0.0f, 1.0f ), \
Vec4( 0.0f, 0.05f, 1.0f, 1.0f ), \
};
#define SPIT_ALT_COLORS  vec4 SpitAltColors[ SpitType_Count ] = { \
Vec4( 0.8f, 0.0f, 0.0f, 1.0f ), \
Vec4( 0.8f, 0.8f, 0.0f, 1.0f ), \
Vec4( 0.0f, 0.0f, 0.8f, 1.0f ), \
};

#define SPIT_MAX_COUNT  ( 512 )
struct SPIT {
    boo32     IsSterilized;
    SPIT_TYPE Type;
    int32     iTexture;
    vec2      Position;
    vec2      Velocity;
    flo32     Radius;
    int32     Health;
    flo32     Radians;
    flo32     dRadians;
};



#define BARREL_HEALTH  ( 3 )
#define BARREL_DIM     ( Vec2( 1.1f, 1.5f ) )

#define BARREL_REPEL_RADIUS  ( 0.5f  )
#define BARREL_REPEL_SPEED   ( 0.25f )

#define BARREL_TRACTOR_BEAM_SPEED  ( 2.5f )
#define BARREL_BASE_COLOR  ( Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) )

#define BARREL_MAX_COUNT  ( 256 )
struct BARREL {
    vec2  Position;
    vec2  Velocity;
    vec2  Repel;
    int32 Health;
    flo32 Radians;
    flo32 dRadians;
};



#define LOADER_HEALTH  ( 3 )
#define LOADER_DIM     ( Vec2( 5.0f, 3.0f ) )

#define LOADER_MOVE_SPEED_FAST              ( 8.0f )
#define LOADER_MOVE_SPEED_SLOW              ( 4.0f )
#define LOADER_MOVE_FRICTION                ( 2.0f )
#define LOADER_ROTATION_SPEED_SLOW          ( TAU / 8.0f )
#define LOADER_ROTATION_SPEED_FAST          ( TAU / 5.0f )
#define LOADER_ROTATION_WARMUP_TARGET_TIME  ( 20.0f / 60.0f )
#define LOADER_MOVE_RADIAN_RANGE            (  PI / 3.0f )
#define LOADER_MOVE_RADIAN_HALF_RANGE       ( LOADER_MOVE_RADIAN_RANGE * 0.5f )
#define LOADER_MOVE_WARMUP_TARGET_TIME      ( 20.0f / 60.0f )

#define LOADER_DAMAGE_PLAYER_COOLDOWN_TARGET_TIME  ( 120.0f / 60.0f )
#define LOADER_DAMAGE_ZONE_COOLDOWN_TARGET_TIME    ( 240.0f / 60.0f )

#define LOADER_BASE_COLOR  ( Vec4( 0.5f, 0.4f, 0.0f, 1.0f ) )

struct LOADER_EVENT {
    uint32 _Padding;
};

#define LOADER_MAX_COUNT  ( 1 )
struct LOADER {
    vec2  Position;
    vec2  Velocity;
    flo32 Radians;
    int32 Health;
    flo32 Timer;
    boo32 DoMove;
    flo32 Damage_Cooldown_Timer;
    
#define LOADER_ANIM_TARGET_TIME  ( 20.0f / 60.0f )
    flo32 Anim_Timer;
    
    flo32  Rumble_Radians;
    
#define LOADER_EXHAUST_MAX_COUNT  ( 4 )
    flo32 Exhaust_TargetTime[ LOADER_EXHAUST_MAX_COUNT ];
    flo32 Exhaust_Timer     [ LOADER_EXHAUST_MAX_COUNT ];
};



#define DAMAGE_ZONE_DIM                 ( Vec2( 8.0f, 4.0f ) )
#define DAMAGE_ZONE_WARMUP_TARGET_TIME  ( 80.0f  / 60.0f )
#define DAMAGE_ZONE_ACTIVE_TARGET_TIME  ( 480.0f / 60.0f )

#define DAMAGE_ZONE_ENABLE_TARGET_TIME  ( 60.0f / 60.0f )

#define DAMAGE_ZONE_MAX_COUNT  ( 8 )
struct DAMAGE_ZONE {
    boo32 IsEnabled;
    flo32 IsEnabled_Timer;
    
    boo32 IsInactive;
    flo32 IsInactive_Radians;
    flo32 Timer;
    orect Bound;
};



enum SPITTER_TYPE {
    SpitterType_Single,
    SpitterType_Double,
    SpitterType_Spiral,
    SpitterType_Short,
    
    SpitterType_Count,
};

#define SPITTER_GROUP_ID_LABELS  char * SpitterGroupIDLabels[ SpitterGroupID_Count ] = { "Spiral", "Double", "Single Cor", "Single Mid", "Vert Cor", "Vert Mid" };

enum SPITTER_GROUP_ID {
    SpitterGroupID_Spiral,
    SpitterGroupID_Double,
    SpitterGroupID_Single_Corner,
    SpitterGroupID_Single_Middle,
    SpitterGroupID_Vertical_Corner,
    SpitterGroupID_Vertical_Middle,
    
    SpitterGroupID_Count,
};

struct SPITTER_SETTINGS_EVENT {
    boo32 Group_IsActive[ SpitterGroupID_Count ];
    int32 nSpiralSpittersToFire;
};


#define SPITTER_WARMUP_TARGET_TIME_SPIRAL  ( 150.0f / 60.0f )
#define SPITTER_WARMUP_TARGET_TIME_SINGLE  (  80.0f / 60.0f )
#define SPITTER_WARMUP_TARGET_TIME_SHORT   (  20.0f / 60.0f )

#define SPITTER_ACTIVE_TARGET_TIME  ( 60.0f / 60.0f )

#define SPITTER_BASE_COLOR  ( Vec4( 0.2f, 0.2f, 0.2f, 1.0f ) )

#define SPITTER_SPIT_COUNT_SINGLE  (  3 )
#define SPITTER_SPIT_COUNT_SHORT   (  2 )
#define SPITTER_SPIT_COUNT_SPIRAL  ( 40 )


#define SPITTER_MAX_COUNT  ( 32 )
struct SPITTER {
    SPITTER_TYPE Type;
    
    vec2      Position;
    vec2      Direction;
    
    int32     GroupID;
    boo32     IsActive;
    flo32     Timer;
    
    boo32     DoSpit;
    SPIT_TYPE DoSpit_Type;
    int32     DoSpit_Counter;
    flo32     DoSpit_Timer;
    flo32     DoSpit_TargetTime;
    flo32     DoSpit_Reset_TargetTime; // Used by SpitterType_Double to sync pairs
};

#define SPITTER_SPIT_BONUS_CYCLE_TARGET_TIME  ( 16.0f )

struct SPITTER_STATE {
    int32     BonusObjective_nLoaderKills;
    
    boo32     SpitBonus_IsActive;
    rect      SpitBonus_Bound;
    SPIT_TYPE SpitBonus_Type;
    SPIT_TYPE SpitBonus_NextType;
    flo32     SpitBonus_Timer;
    flo32     SpitBonus_TargetTime;
    
    boo32 Barrel_IsActive;
    flo32 Barrel_Timer;
    flo32 Barrel_TargetTime;
    
    int32 Spiral_nSpitters;
    int32 Spiral_nToFire;
    flo32 Spiral_Timer;
    flo32 Spiral_TargetTime;
    
    int32 nSpitTotal;
    int32 nSpitBonusTotal;
    
    int32  nSpit;
    SPIT    Spit[ SPIT_MAX_COUNT ];
    
    int32  nBarrel;
    BARREL   Barrel[ BARREL_MAX_COUNT ];
    
    int32  nLoader;
    LOADER  Loader[ LOADER_MAX_COUNT ];
    
    int32      nDamageZone;
    DAMAGE_ZONE DamageZone[ DAMAGE_ZONE_MAX_COUNT ];
    
    int32  nSpitter;
    SPITTER Spitter[ SPITTER_MAX_COUNT ];
};

//----------
// GUNNER
//----------

#define GUNNER_RADIUS      ( 1.25f )
#define GUNNER_MOVE_SPEED  ( 24.0f )

#define GUNNER_HEALTH      ( 15 )

#define GUNNER_REPEL_RADIUS  ( 1.0f )
#define GUNNER_REPEL_SPEED   ( 1.0f )

#define GUNNER_BASE_COLOR  ( Vec4( 0.25f, 0.15f, 0.25f, 1.0f ) )

#define GUNNER_FIRE_COOLDOWN_TARGET_TIME  ( 60.0f / 60.0f )

#define GUNNER_MAX_COUNT  ( 64 )
struct GUNNER {
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    flo32  Radians;
    flo32  Radius;
    int32  Health;
    
    boo32  Move_DoMove;
    flo32  Move_Timer;
    flo32  Move_TargetTime;
    vec2   Move_Dir;
    
    flo32  Fire_Timer;
    flo32  Fire_TargetTime;
    
#define GUNNER_KILL_ANIM_TARGET_TIME  ( 8.0f / 60.0f )
    flo32  Kill_Timer;
    int32  Kill_Counter;
    
    flo32  Damage_Timer;
};

struct GUNNER_STATE {
    int32  nGunner;
    GUNNER  Gunner[ GUNNER_MAX_COUNT ];
};

//----------
// BOLT
//----------

#define BOLT_RADIUS  ( 0.65f )
#define BOLT_SPEED   ( 18.0f )

#define BOLT_BASE_COLOR  ( COLOR_YELLOW )

#define BOLT_MAX_COUNT  ( 512 )
struct BOLT {
    vec2   Position;
    vec2   Velocity;
    int32  Health;
    flo32  Radius;
    
    boo32  DoPointDrop;
};

struct BOLT_STATE {
    int32 nBolt;
    BOLT   Bolt[ BOLT_MAX_COUNT ];
};

//----------
// BOARD ROOM
//----------

#define OBSERVATION_DECK_RADIUS            ( 64.0f )
#define OBSERVATION_DECK_SHIELD_INIT_DIST  ( 10.0f )
#define OBSERVATION_DECK_HEALTH  ( 6 )

#define OBSERVATION_DECK_BARRIER_COUNT  ( 4 )
#define OBSERVATION_DECK_BARRIER_DIST   ( 3.0f )
#define OBSERVATION_DECK_BARRIER_WIDTH  ( 5.0f )
#define OBSERVATION_DECK_BARRIER_ROTATE_SPEED  ( TAU / 40.0f )
#define OBSERVATION_DECK_BARRIER_GAP_RADIANS   ( TAU * ( 3.0f / 32.0f ) )
#define OBSERVATION_DECK_BARRIER_RADIANS       ( ( TAU * 0.25f ) - OBSERVATION_DECK_BARRIER_GAP_RADIANS )
#define OBSERVATION_DECK_BARRIER_SEGMENT_COUNT ( 24 )

#define OBSERVATION_DECK_MISSILE_LAUNCHER_TARGET_TIME  ( 4.0f )

#define OBSERVATION_DECK_BASE_COLOR      ( Vec4( 0.35f, 0.35f, 0.1f, 1.0f ) )
#define OBSERVATION_DECK_WINDOW_RADIANS  ( TAU / 16.0f )

struct OBSERVATION_DECK_SHIELD_HIT {
    vec2  Pos;
    flo32 Scale;
    flo32 Timer;
    flo32 Radians;
};

struct WINDOW_CRACK_SEGMENT {
    vec2  P;
    vec2  Q;
    flo32 tLevel;
};

struct WINDOW_CRACK_BRANCH {
    vec2  AtPos;
    flo32 AtRadians;
    flo32 bRadians;
    flo32 AtLevel;
};

struct OBSERVATION_DECK_BARRIER {
    vec2  Position;
    flo32 Radians;
};

struct OBSERVATION_DECK {
    boo32  IsPresent;
    vec2   Position;
    flo32  Radius;
    int32  Health;
    
    boo32  Shield_IsActive;
    flo32  Shield_Dist;
    flo32  Shield_MaxDist;
    
    boo32  Barrier_IsActive;
    flo32  Barrier_Dist;
    flo32  Barrier_Radians;
    flo32  Barrier_RotateSpeed;
    
#define OBSERVATION_DECK_ATTACK_TARGET_TIME        ( 180.0f / 60.0f )
#define OBSERVATION_DECK_ATTACK_CYCLE_TARGET_TIME  (  30.0f / 60.0f )
#define OBSERVATION_DECK_TURRET_ANIM_TARGET_TIME   ( 120.0f / 60.0f )
    flo32  TurretAnim_Timer;
    flo32  Attack_Timer;
    int32  Attack_Counter;
    flo32  Attack_Radians;
    flo32  Attack_dRadians;
    
#define OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT  ( 2 )
    MISSILE_LAUNCHER MissileLauncher         [ OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT ];
    vec2             MissileLauncher_Pos     [ OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT ][ 2 ];
    flo32            MissileLauncher_Radians [ OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT ];
    int32           nMissileLaunchersDestroyed;
    
#define OBSERVATION_DECK_SHIELD_HIT_TARGET_TIME  ( 60.0f / 60.0f )
#define OBSERVATION_DECK_SHIELD_HIT_MAX_COUNT    ( 32 )
    int32                      iShieldHit;
    OBSERVATION_DECK_SHIELD_HIT ShieldHit[ OBSERVATION_DECK_SHIELD_HIT_MAX_COUNT ];
    flo32                       ShieldDown_Timer;
    
#define OBSERVATION_DECK_CONFERENCE_ROOM_LIGHT_TARGET_TIME  ( 60.0f / 60.0f )
    flo32 Light_Timer;
    
#define WINDOW_CRACK_SEGMENT_MAX_COUNT  ( 128 )
    WINDOW_CRACK_SEGMENT WindowCrack[ WINDOW_CRACK_SEGMENT_MAX_COUNT ];
    int32                WindowCrack_nSegments;
    int32                WindowCrack_MaxSegments;
    
    YACHT_EXIT_RING_DATA Teleport_InnerRing;
    YACHT_EXIT_RING_DATA Teleport_MidRing;
    flo32                Teleport_Timer;
    flo32                Teleport_ScaleTimer;
    flo32                Teleport_tFlash;
    flo32                Teleport_ParticleTimer;
};

#define SECURITY_DRONE_RADIUS    ( 5.0f )
#define SECURITY_DRONE_FRICTION  ( 2.0f )

#define SECURITY_DRONE_ZERO_POINT_RADIUS  ( SECURITY_DRONE_RADIUS + 4.5f )
#define SECURITY_DRONE_ZERO_POINT_TARGET_TIME_TO_FULL_SPEED  ( 120.0f / 60.0f )
#define SECURITY_DRONE_HI_DAMAGE_SPEED_THRESHOLD  ( 13.0f )
#define SECURITY_DRONE_ZERO_POINT_ADJUST_SPEED_RATIO  ( 1.35f )

#define SECURITY_DRONE_SPAWN_COOLDOWN_TARGET_TIME   ( 120.0f / 60.0f )
#define SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME      (  20.0f / 60.0f )
#define SECURITY_DRONE_BULLETS_WARMUP_TARGET_TIME   ( 120.0f / 60.0f )
#define SECURITY_DRONE_LASERS_WARMUP_TARGET_TIME    ( 120.0f / 60.0f )
#define SECURITY_DRONE_COLLIDE_RECOVER_TARGET_TIME  (  60.0f / 60.0f )
#define SECURITY_DRONE_MOVE_MARGIN       (  3.0f )
#define SECURITY_DRONE_MOVE_PLAYER_DIST  ( 14.0f )

#define SECURITY_DRONE_HIT_OBSERVATION_DECK_RECOVER_HEALTH  ( 75 )
#define SECURITY_DRONE_HIT_MISSILE_LAUNCHER_RECOVER_HEALTH  ( 50 )
#define SECURITY_DRONE_OFFSCREEN_RECOVER_HEALTH             ( 35 )

#define SECURITY_DRONE_SPAWN_SPEED  ( 12.0f )
#define SECURITY_DRONE_SWEEP_SPEED  (  8.0f )
#define SECURITY_DRONE_MOVE_SPEED   (  6.0f )

#define SECURITY_DRONE_BOOST_WARMUP_TARGET_TIME  (  30.0f / 60.0f )
#define SECURITY_DRONE_FIRE_TARGET_TIME          ( 180.0f / 60.0f )
#define SECURITY_DRONE_STUNNED_TARGET_TIME       ( 360.0f / 60.0f )

#define SECURITY_DRONE_LASER_MAX_COUNT  ( 3 )
#define SECURITY_DRONE_BASE_COLOR       ( Vec4( 0.15f, 0.25f, 0.25f, 1.0f ) )

enum SECURITY_DRONE_STAGE {
    SecurityDroneStage_Stunned,
    SecurityDroneStage_Spawn,
    SecurityDroneStage_Move,
    SecurityDroneStage_Spin_Bullets,
    SecurityDroneStage_Spin_Lasers,
    SecurityDroneStage_Sweep_Bullets,
    SecurityDroneStage_Sweep_Lasers,
    
    SecurityDroneStage_Count,
};

struct SECURITY_DRONE {
    SECURITY_DRONE_STAGE Stage;
    
    vec2   Position;
    vec2   Destination;
    boo32  Dest_Trigger;
    vec2   Velocity;
    flo32  Friction;
    flo32  Radius;
    int32  Health;
    
    int32  Numer_Move;
    
    flo32  Radians;
    flo32  Radians_Velocity;
    flo32  Timer;
    int32  Counter;
    
    flo32  Fire_Timer;
    flo32  Fire_TargetTime;
    flo32  TimeSinceLastAttack;
    flo32  MaxTimeSinceLastAttack;
    
    int32 nLaser;
    flo32  Laser_MaxLength;
    boo32  Laser_IsActive[ SECURITY_DRONE_LASER_MAX_COUNT ];
    vec2   Laser_Dest    [ SECURITY_DRONE_LASER_MAX_COUNT ];
    
    boo32  ZeroPoint_ShowUI;
    boo32  ZeroPoint_IsActive;
    flo32  ZeroPoint_Radians;
    flo32  ZeroPoint_Dist;
    
    flo32  Boost_Timer;
    flo32  Boost_TargetTime;
    vec2   Boost_Direction;
    
#define SECURITY_DRONE_LIGHT_CYCLE_TARGET_TIME    ( 10.0f / 60.0f )
#define SECURITY_DRONE_LIGHT_TARGET_TIME          ( SECURITY_DRONE_LIGHT_CYCLE_TARGET_TIME * 12.0f )
#define SECURITY_DRONE_RECOVER_LIGHT_TARGET_TIME  ( SECURITY_DRONE_LIGHT_CYCLE_TARGET_TIME * 4.0f )
    flo32  Light_Timer;
    flo32  Recover_Timer;
    
    flo32  Damage_Timer;
};

#define BOARD_MEMBER_RADIUS  ( 0.4f )

struct BOARD_MEMBER {
    vec2  Position;
    vec2  Direction;
    flo32 Speed;
    vec2  Velocity;
    flo32 Radians;
    flo32 dRadians;
    flo32 Timer;
    flo32 LimbRadians[ 4 ];
    vec4  Color[ 3 ];
    boo32 DoFloat;
};

enum SPAWN_LAYOUT_PATTERN {
    SpawnLayoutPattern_Line,
    SpawnLayoutPattern_Circle,
    SpawnLayoutPattern_Random,
    
    SpawnLayoutPattern_Count,
};

struct SPAWN_LAYOUT {
    ENTITY_TYPE          EntityType;
    SPAWN_LAYOUT_PATTERN Pattern;
    int32                nEntity;
    vec2                 StartPos;
    union {
        vec2  EndPos;
        vec2  Scale;
    };
    flo32                Timer;
    flo32                TargetTime;
    int32                AtEntity;
};

internal SPAWN_LAYOUT
SpawnLayoutC( ENTITY_TYPE EntityType, SPAWN_LAYOUT_PATTERN Pattern, int32 nEntity, vec2 StartPos, vec2 EndPos ) {
    SPAWN_LAYOUT Result = {};
    Result.EntityType = EntityType;
    Result.Pattern    = Pattern;
    Result.nEntity    = nEntity;
    Result.StartPos   = StartPos;
    Result.EndPos     = EndPos;
    
    Result.TargetTime = ( 75.0f / 60.0f ) / ( flo32 )nEntity;
    
    return Result;
}

struct BOARD_ROOM_STATE {
    boo32 State_IsActive;
    
    boo32 Spawn_HitObservationDeck;
    boo32 Spawn_HitMissileLauncher;
    boo32 Spawn_nGunners;
    boo32 Spawn_nEntities;
    flo32 Spawn_Timer;
    flo32 Spawn_TargetTime;
    
#define SPAWN_LAYOUT_MAX_COUNT  ( 16 )
    int32       nSpawnLayout;
    SPAWN_LAYOUT SpawnLayout[ SPAWN_LAYOUT_MAX_COUNT ];
    
#define SPAWN_LAYOUT_PATTERN_MAX_COUNT  ( 17 )
    int32       SpawnLayout_Index[ SPAWN_LAYOUT_PATTERN_MAX_COUNT ];
    int32      iSpawnLayout_Index;
    
    OBSERVATION_DECK ObservationDeck;
    SECURITY_DRONE   SecurityDrone;
    
#define BOARD_MEMBER_MAX_COUNT  ( 8 )
    int32           nBoardMember;
    BOARD_MEMBER     BoardMember[ BOARD_MEMBER_MAX_COUNT ];
};

//----------
// POWERUP
//----------

#define POWERUP_RADIUS    ( 1.2f )
#define POWERUP_FRICTION  ( 2.0f )

#define POWERUP_REPEL_RADIUS  ( 1.0f )
#define POWERUP_REPEL_SPEED   ( 1.0f )

#define POWERUP_EXIT_TARGET_TIME  ( 10.0f )
#define POWERUP_PICKUP_ANIM_TARGET_TIME  ( 6.0f / 60.0f )

#define POWERUP_SEEK_PLAYER_RADIUS  ( 12.0f )

#define POWERUP_TYPE_LABELS  char * PowerupTypeLabels[ PowerupType_Count ] = { "None", "Health", "Shield", "Attack", "Multiplier", "Damage All", "Gadgets" };

enum POWERUP_TYPE {
    PowerupType_None,
    
    PowerupType_Health,
    PowerupType_NoDamage,
    PowerupType_PlayerBullets,
    PowerupType_ScoreMultipliers,
    PowerupType_DamageAllEnemies,
    PowerupType_SecondaryGadgets,
    
    PowerupType_Count,
};


#define POWERUP_MAX_COUNT  ( 256 )
struct POWERUP {
    POWERUP_TYPE Type;
    
    vec2   Position;
    vec2   Velocity;
    vec2   Repel;
    
    flo32  Move_Radians;
    flo32  Move_Speed;
    flo32  Move_Timer;
    flo32  Move_TargetTime;
    
    flo32  Radius;
    int32  Health;
    
    flo32  Exit_Timer;
    flo32  Exit_TargetTime;
    vec2   Exit_Pos;
    
    flo32  Flash_Radians;
};

struct POWERUP_ENTRY {
    int32 bNumer;
    flo64  UpgradeCost;
    flo64 bUpgradeCost;
    
    flo32  Rate;
    flo32  TargetTime;
};

#define POWERUP_TIME_DELAY_FOR_LEVEL_START  ( 2.0f )
#define POWERUP_TIME_BUFFER_FOR_LEVEL_END   ( 12.0f )

struct POWERUP_STATE_SAVE_DATA {
    uint64        TotalPoints;
    POWERUP_ENTRY PowerupEntry[ PowerupType_Count ];
};

struct POWERUP_STATE {
    boo32 Spawn_IsActive;
    flo32 Spawn_Timer;
    flo32 Spawn_TargetTime;
    flo32 Spawn_Cooldown_TargetTime;
    flo32 Spawn_Segment_TargetTime;
    
    int32 Powerup_NumerForLevel[ PowerupType_Count ];
    
    int32  nPowerup;
    POWERUP Powerup[ POWERUP_MAX_COUNT ];
};

//----------
// WAVE
//----------

#define WAVE_TYPE_LABELS  char * WaveTypeLabels[ WaveType_Count ] = { "INVALID WAVE_TYPE: End Level", "Spawn Entities", "Open Bay Doors", "Set Asteroid Settings", "Set Explosive Settings", "Set Swarm Settings", "Set Turret Settings", "Enable Bomb Launcher", "Enable Meta-Turret", "Enable Blimp", "Enable Mega-Miner", "Set Worm Settings", "Enable Loader", "Set Spitter Settings", "Enable Level Timer" };

enum WAVE_TYPE {
    WaveType_EndLevel,
    WaveType_SpawnEntities,
    WaveType_OpenBayDoors,
    WaveType_SetAsteroidSettings,
    WaveType_SetExplosiveSettings,
    WaveType_SetSwarmSettings,
    WaveType_SetTurretSettings,
    WaveType_EnableBombLauncher,
    WaveType_EnableMegaTurret,
    WaveType_EnableBlimp,
    WaveType_EnableMegaMiner,
    WaveType_SetWormSettings,
    WaveType_EnableLoader,
    WaveType_SetSpitterSettings,
    WaveType_EnableLevelTimer,
    
    WaveType_Count,
};

#define EXIT_TYPE_LABELS  char * ExitTypeLabels[ ExitType_Count ] = { "Instant", "Target Time", "No Exit", "Kill All Entities In Wave", "Kill All Entities On Screen", "Bay Door Event is Complete", "All Bay Door Events are Complete", "Bay Door Spawn Is Complete", "Kill All Turrets" };

enum EXIT_TYPE {
    ExitType_Instant,
    ExitType_TargetTime,
    ExitType_NoExit,
    
    ExitType_KillAllEntitiesInWave,
    ExitType_KillAllEntitiesOnScreen,
    
    ExitType_BayDoorEventIsComplete,
    ExitType_AllBayDoorEventsAreComplete,
    ExitType_BayDoorSpawnIsComplete,
    
    ExitType_KillAllTurrets,
    
    ExitType_Count,
};

struct ENTITY { // sizeof( ENTITY ) = 48 Bytes
    int32         WaveGuid;
    ENTITY_TYPE   Type;
    int32         nEntity;
    vec2          Position;
    flo32         Radius;
    ENTITY_SHIELD Shield;
};


#define WAVE_ENTITY_MAX_COUNT  ( 512 )

struct WAVE {
    int32      Guid;
    TEXTURE_ID Layout_TextureID;
    vec2       Layout_Position;
    
    int32      nRepeatWave;
    
    WAVE_TYPE  Type;
    EXIT_TYPE  Exit;
    
    int32 nKills;
    int32 nTotalEntitiesInWave;
    
    uint32 nEntity;
    union {
        struct {
            ENTITY  Entity[ WAVE_ENTITY_MAX_COUNT ];
        };
        struct {
            BAY_DOOR_EVENT           BayDoorEvent;
        };
        struct {
            ASTEROID_SETTINGS_EVENT  AsteroidSettingsEvent;
        };
        struct {
            EXPLOSIVE_SETTINGS_EVENT ExplosiveSettingsEvent;
        };
        struct {
            SWARM_SETTINGS_EVENT     SwarmSettingsEvent;
        };
        struct {
            TURRET_SETTINGS_EVENT    TurretSettingsEvent;
        };
        struct {
            BOMB_LAUNCHER_EVENT      BombLauncherEvent;
        };
        struct {
            MEGA_TURRET_EVENT        MegaTurretEvent;
        };
        struct {
            BLIMP_EVENT              BlimpEvent;
        };
        struct {
            MEGA_MINER_EVENT         MegaMinerEvent;
        };
        struct {
            WORM_SETTINGS_EVENT      WormSettingsEvent;
        };
        struct {
            LOADER_EVENT             LoaderEvent;
        };
        struct {
            SPITTER_SETTINGS_EVENT   SpitterSettingsEvent;
        };
    };
    
    flo32 Exit_Timer;
    flo32 Exit_TargetTime;
    int32 _Exit_TargetTime_Minutes;
    int32 _Exit_TargetTime_Seconds;
    
    flo32 InDelay_Timer;
    flo32 InDelay_TargetTime;
    int32 _InDelay_TargetTime_Minutes;
    int32 _InDelay_TargetTime_Seconds;
    
    boo32 DoOutDelay;
    flo32 OutDelay_Timer;
    flo32 OutDelay_TargetTime;
    int32 _OutDelay_TargetTime_Minutes;
    int32 _OutDelay_TargetTime_Seconds;
    
#define NEXT_WAVE_MAX_COUNT  ( 4 )
    uint32 nNextWave;
    int32   NextWave     [ NEXT_WAVE_MAX_COUNT ];
    int32   NextWave_Guid[ NEXT_WAVE_MAX_COUNT ];
    
#define KILL_WAVE_MAX_COUNT  ( 4 )
    int32  nKillWave;
    int32   KillWave_Guid[ KILL_WAVE_MAX_COUNT ];
};

struct ENTITY_GROUP_SPAWN {
    int32       WaveGuid;
    ENTITY_TYPE Type;
    flo32       TargetTime;
    flo32       Timer;
    uint32      Counter;
    vec2        Position;
};

#define ENTITY_SPAWN_TARGET_TIME  ( 120.0f / 60.0f )
#define ENTITY_SPAWN_MAX_COUNT    ( 1024 )
struct ENTITY_SPAWN_STATE {
    int32  nEntitySpawn;
    ENTITY  EntitySpawn      [ ENTITY_SPAWN_MAX_COUNT ];
    flo32   EntitySpawn_Timer[ ENTITY_SPAWN_MAX_COUNT ];
};

#define ENTITY_GROUP_SPAWN_MAX_COUNT  ( 512 )
#define ACTIVE_WAVE_MAX_COUNT         ( 16 )
#define WAVE_MAX_COUNT                ( 512 )
struct WAVE_STATE {
    int32 StartingWave;
    
    uint32            nEntityGroupSpawn;
    ENTITY_GROUP_SPAWN EntityGroupSpawn[ ENTITY_GROUP_SPAWN_MAX_COUNT ];
    
    int32 nActiveWave;
    boo32  ActiveWave_IsInitialized[ ACTIVE_WAVE_MAX_COUNT ];
    WAVE   ActiveWave              [ ACTIVE_WAVE_MAX_COUNT ];
    
    int32 nWave;
    WAVE   Wave[ WAVE_MAX_COUNT ];
};

#define WAVE_DATA_SIZE
struct EDITOR__WAVE_STATE {
    uint32  CurrentWave;
    int32   CurrentGuid;
    uint32  StartingWave;
    
    boo32   Wave_IsSelected  [ WAVE_MAX_COUNT        ];
    boo32   Entity_IsSelected[ WAVE_ENTITY_MAX_COUNT ];
    
    uint32 nWave;
    WAVE    Wave[ WAVE_MAX_COUNT ];
};

enum END_LEVEL_ACTION {
    EndLevelAction_Null,
    
    EndLevelAction_ReplayLevel,
    EndLevelAction_ExitToOverworld,
    EndLevelAction_ExitToDesktop,
};

#define END_LEVEL_FADE_IN_DELAY_TARGET_TIME  ( 40.0f / 60.0f )
#define END_LEVEL_FADE_IN_TARGET_TIME        ( 30.0f / 60.0f )

#define END_LEVEL_BUTTON_TARGET_TIME   ( 72.0f / 60.0f )
struct END_LEVEL {
    boo32 Show;
    boo32 ShowLevelComplete;
    
    flo32 FadeIn_Timer;
    
    END_LEVEL_ACTION EndLevelAction;
    flo32            EndLevelAction_Timer;
    
    //boo32 DoExitToDesktop;
    //flo32 DoExitToDesktop_Timer;
    //boo32 DoExitToOverworld;
    //flo32 DoExitToOverworld_Timer;
    //boo32 DoReplay;
    //flo32 DoReplay_Timer;
    
    boo32 ShowStats;
    flo32 ShowStats_Timer;
};

//----------
// MARKET
//----------



struct POINTS_ENTRY { // SAVE DATA
    int32  nPoints_Positive;
    int32  nPoints_Negative;
    int32  nScoreMultiplier;
};


#if 0

#define STOCK_ENTRY_FLAG__IS_ENABLED    ( 0x0001 )
#define STOCK_ENTRY_FLAG__IS_AVAILABLE  ( 0x0002 )
struct STOCK_ENTRY { // SAVE DATA
    uint32 Flags;
    uint32 Price;
    uint64 nSharesOwned;
    
    flo32  PriceChange_StockRate;
    flo32  PriceChange_EntityRate;
};

#define PRODUCT_ENTRY_FLAG__IS_ENABLED    ( 0x0001 )
#define PRODUCT_ENTRY_FLAG__IS_AVAILABLE  ( 0x0002 )
struct PRODUCT_ENTRY { // SAVE DATA
    uint32 Flags;
    flo32  Percent;
};

struct MARKET_STATE_SAVE_DATA { // SAVE DATA
    //uint64 TotalPoints;
    //uint64       nPoints_Positive;
    //uint64       nPoints_Negative;
    
    STOCK_ENTRY   Stock  [ ENTITY_TYPE_MAX_COUNT ];
    PRODUCT_ENTRY Product[ ENTITY_TYPE_MAX_COUNT ];
};

struct MARKET_STATE_TEMP_DATA {
    int64  Transaction_nShares;
    int32  Stock_iSelect;
};

struct MARKET_STATE {
    MARKET_STATE_TEMP_DATA TempData;
    MARKET_STATE_SAVE_DATA SaveData;
};

#endif


//----------
// PROGRESS METER
//----------

#if 0
struct PROGRESS_STAGE {
    boo32 IsActive;
    rect  Bound;
    
    boo32 IsComplete;
    boo32 IsFailed;
    flo32 tCompletion;
    
    flo32 Timer;
    int32 bCounter;
};

enum PROGRESS_DISPLAY_TYPE {
    ProgressDisplayType_Hidden,
    ProgressDisplayType_Wait,
    ProgressDisplayType_Counter,
    ProgressDisplayType_Timer,
    ProgressDisplayType_Message,
};

struct PROGRESS_DISPLAY {
    rect                  Bound;
    PROGRESS_DISPLAY_TYPE Type;
    int32                 sValue;
    flo32                 fValue;
    flo32                 Timer;
#define PROGRESS_DISPLAY_MESSAGE_CHAR_MAX_COUNT  ( 16 )
    char                  Message[ PROGRESS_DISPLAY_MESSAGE_CHAR_MAX_COUNT ];
};

#define PROGRESS_STAGE_MAX_COUNT    ( 16 )
#define PROGRESS_DISPLAY_MAX_COUNT  ( 4 )

struct PROGRESS_STATE {
    boo32            IsInitialized;
    
    int32           nStage;
    PROGRESS_STAGE   Stage  [ PROGRESS_STAGE_MAX_COUNT ];
    PROGRESS_DISPLAY Display[ PROGRESS_DISPLAY_MAX_COUNT ];
    
    PROGRESS_STAGE   Bonus;
    PROGRESS_DISPLAY BonusDisplay;
};
#endif


//----------
// OVERWORLD
//----------

struct OVERWORLD_LEVEL {
    boo32    IsAvailable;
    boo32    IsComplete;
    uint32   HighScore;
    uint32   HighMultiplier;
    flo32    HighAvgBonus;
    flo32    HighAvgBonus_FullTime;
    flo32    HighAvgBonus_TotalTime;
    
#define SCORE_RATING_MAX_RATING  ( 5 )
    int32  ScoreRating;
    boo32  BonusObjectiveWasAcquired;
};

// NOTE : Cannot remove because the editor saves this in the level files.
struct LEVEL_SETTINGS { // sizeof( LEVEL_SETTINGS ) = 128 Bytes
    union {
        struct {
            flo32 World_YUnits;
            int32 ScoreThresholdForRating[ SCORE_RATING_MAX_RATING ];
        };
        struct {
            uint32 _Padding[ 32 ];
        };
    };
};

struct ASSET_LEVEL {
    flo32  World_YUnits;
    int32  StartingWave;
    int32 nWave;
    WAVE * Wave;
};

enum ITEM_ID {
    ItemID_None,
    
    ItemID_Blaster,
    ItemID_GrappleBeam,
    ItemID_TractorBeam,
    ItemID_GravityWell,
    ItemID_Shield,
    ItemID_Collector,
    
    ItemID_Count,
};

struct LEVEL_INFO {
    TEXTURE_ID  BackgroundID;
    ITEM_ID     WeaponID;
    ITEM_ID     GadgetID;
    ENTITY_TYPE TargetID;
    int32       TargetInfo;
    flo32       Time;
    int32       ScoreRating_Threshold[ SCORE_RATING_MAX_RATING ];
};

struct LEVEL_INFO_WRAPPER {
    LEVEL_ID        LevelID;
    OVERWORLD_LEVEL Level;
    LEVEL_INFO      Info;
};

struct OVERWORLD_MAP {
    char  DisplayName[ 16 ];
    rect  Bound;
    
    boo32 ShowRating;
    
#define OVERWORLD_LEVEL_TO_UNLOCK_MAX_COUNT  ( 8 )
    int32   nLevelToUnlock;
    LEVEL_ID LevelToUnlock[ OVERWORLD_LEVEL_TO_UNLOCK_MAX_COUNT ];
    
    LEVEL_INFO Info;
};

struct OVERWORLD_DATA {
    OVERWORLD_LEVEL * Level;
    OVERWORLD_MAP   * Map;
    LEVEL_INFO      * Info;
};

#define OVERWORLD_LEVEL_MAX_COUNT  ( 16 )
struct OVERWORLD_STATE_SAVE_DATA {
    LEVEL_ID      AtLevel;
    int32          nLevel;
    OVERWORLD_LEVEL Level[ OVERWORLD_LEVEL_MAX_COUNT ];
};

#define GRAVITY_WELL_PARTICLE_MAX_COUNT  ( 128 )
struct GRAVITY_WELL_PARTICLE {
    flo32 Radians;
    flo32 Dist;
    flo32 Speed;
    flo32 Timer;
    flo32 TargetTime;
    flo32 tSpeed;
    uint8 iColor;
};

struct OVERWORLD_STATE_TEMP_DATA {
    LEVEL_ID ToLevel;
    
    vec2   Player_Position;
    flo32  Player_Radians;
    
    //#define OVERWORLD_PLAYER_ANIM_TARGET_TIME  ( 100.0f / 60.0f )
    boo32 Player_DoTravel;
    flo32 Player_DoTravel_Timer;
    flo32 Player_DoTravel_TargetTime;
    
    //flo32 ResetGameFile_Timer;
    
    flo32 GravityWell_Radians;
#define OVERWORLD_GRAVITY_WELL_PARTICLE_MAX_COUNT      ( 2048 )
#define OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_INNER  ( 10.0f )
#define OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_OUTER  ( 1200.0f )
    GRAVITY_WELL_PARTICLE GravityWell_Particle[ OVERWORLD_GRAVITY_WELL_PARTICLE_MAX_COUNT ];
    
    OVERWORLD_MAP Map[ OVERWORLD_LEVEL_MAX_COUNT ];
    POINTS_ENTRY  Points [ ENTITY_TYPE_MAX_COUNT ];
};

struct OVERWORLD_STATE {
    //OVERWORLD_STATE_SAVE_DATA SaveData;
    OVERWORLD_STATE_TEMP_DATA TempData;
};

struct SAVE_GAME {
    OVERWORLD_STATE_SAVE_DATA OverworldSave;
    POWERUP_STATE_SAVE_DATA   PowerupSave;
};

//----------
// PLAYER
//----------

// COMMON STATS
#define LOAD_CHECKPOINT_TARGET_TIME  ( 72.0f / 60.0f )
#define UI_PANEL_HEIGHT              ( 40.0f )

#define PLAYER_HEALTH                          ( 3 )
#define PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME   ( 6.0f / 60.0f )

#define PLAYER_LAST_DAMAGE_TARGET_TIME         ( 132.0f / 60.0f )

#define PLAYER_NO_DAMAGE_TARGET_TIME            ( 90.0f / 60.0f )
#define PLAYER_NO_DAMAGE_SECONDARY_TARGET_TIME  ( 60.0f / 60.0f )
#define PLAYER_SECONDARY_DAMAGE_TARGET_TIME     ( 180.0f / 60.0f )
#define PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME  ( 40.0f / 60.0f )

#define PLAYER_RADIUS      ( 1.0f )
#define PLAYER_HALF_WIDTH  ( PLAYER_RADIUS )
#define PLAYER_HALF_HEIGHT ( PLAYER_RADIUS )
#define PLAYER_WIDTH       ( PLAYER_HALF_WIDTH  * 2.0f )
#define PLAYER_HEIGHT      ( PLAYER_HALF_HEIGHT * 2.0f )
#define PLAYER_DIM         ( Vec2( PLAYER_WIDTH, PLAYER_HEIGHT ) )

#define PLAYER_ROTATE_SPEED_SLOW               ( TAU / 4.0f )
#define PLAYER_ROTATE_SPEED_FAST               ( TAU / 2.5f )
#define PLAYER_FIRE_COOLDOWN_TARGET_TIME_SLOW  ( 10.0f / 60.0f )
#define PLAYER_FIRE_COOLDOWN_TARGET_TIME_FAST  (  4.0f / 60.0f )

#define PLAYER_DAMAGE_BOOST_FRICTION     ( 14.0f )
#define PLAYER_DAMAGE_BOOST_SPEED        ( 36.0f )
#define PLAYER_DAMAGE_BOOST_TARGET_TIME  ( 6.0f / 60.0f )
#define PLAYER_BUMPER_BOOST_TARGET_TIME  ( 1.0f / 60.0f )

#define PLAYER_COLLECTOR_RADIUS            ( 2.0f )
#define PLAYER_COLLECTOR_DIST_FROM_PLAYER  ( 0.5f )

#define PLAYER_GADGET_BASE_COLOR  ( Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) )

struct ENTITY_VALID_RESULT {
    boo32  IsValid;
    uint32 iEntity;
};

#define TRACTOR_BEAM_RING_MIN_RADIUS  ( PLAYER_RADIUS * 0.75f )
#define TRACTOR_BEAM_RING_MAX_RADIUS  ( PLAYER_RADIUS * 3.5f )

#define TRACTOR_BEAM_RING_MAX_COUNT  ( 3 )
struct TRACTOR_BEAM_RING {
    boo32 DoShow;
    flo32 Radius;
    flo32 Alpha;
};

#define SCREEN_FLASH_TARGET_TIME__POWERUP_DAMAGE_ALL         ( 4.0f / 60.0f )
#define SCREEN_FLASH_TARGET_TIME__PLAYER_DAMAGE              ( 4.0f / 60.0f )
#define SCREEN_FLASH_TARGET_TIME__PLAYER_SECONDARY_DAMAGE    ( 4.0f / 60.0f )

enum SCREEN_FLASH_TYPE {
    ScreenFlashType_Powerup_DamageAll,
    ScreenFlashType_Player_Damage,
    ScreenFlashType_Player_SecondaryDamage,
    
    ScreenFlashType_Count,
};

struct PLAYER_STATE {
    vec2  Position;
    vec2  dPos;
    flo32 Facing_Radians;
    
    boo32  DoCollision;
    vec2   DoCollision_Pos;
    vec2   DoCollision_Norm;
    flo32  DoCollision_Radians;
    
    uint32 MaxHealth;
    uint32 Health;
    flo32  ShowDamage_Timer;
    boo32  IsDead;
    flo32  IsDead_Timer;
    
    flo32  DoDamage_Timer;
    vec2   DoDamage_Velocity;
    
    flo32  DoBumper_Timer;
    vec2   DoBumper_Velocity;
    
    SCREEN_FLASH_TYPE ScreenFlash_Type;
    flo32             ScreenFlash_Timer;
    flo32             ScreenFlash_TargetTime;
    
    flo32  NoDamage_Timer;
    flo32  SecondaryDamage_Timer;
    
    flo32  ShowShieldPowerup_Timer;
    
    flo32 Fire_CooldownTimer;
    flo32 Fire_CooldownTargetTime;
    
    flo32  Powerup_Timer_PlayerBullets;
    flo32  Powerup_Rate_PlayerBullets;
    flo32  Powerup_Timer_SecondaryGadgets;
    flo32  Powerup_Rate_SecondaryGadgets;
    
    
    boo32 ZeroPoint_IsEnabled;
    boo32 ZeroPoint_ShowUI;
    boo32 ZeroPoint_IsActive;
    boo32 ZeroPoint_DrawBeam;
    vec2  ZeroPoint_Pos;
    flo32 ZeroPoint_Radians;  // NOTE: This value is updated, but is ONLY used for drawing.
    flo32 ZeroPoint_Dist;     // NOTE: This value is updated, but is ONLY used for drawing.
    // NOTE: These two values are so that we can draw the ZeroPoint indicator on top of the player and other entities without sorting the render objects. Eventually, we will be sorting the render objects, so this can go away then.
    boo32 ZeroPoint_IsAsteroid;
    boo32 ZeroPoint_IsSecurityDrone;
    flo32 ZeroPoint_IsSecurityDrone_tSpeed;
    flo32 ZeroPoint_Fast_CooldownTimer;
    
#define PLAYER_SHIELD_RADIUS  ( 2.5f )
    boo32 Shield_IsEnabled;
    boo32 Shield_IsActive;
    
#define PLAYER_TRACTOR_BEAM_AMIN_TARGET_TIME  ( 52.0f / 60.0f )
    boo32 TractorBeam_IsEnabled;
    boo32 TractorBeam_IsActive;
    flo32 TractorBeam_AnimRadians;
    flo32 TractorBeam_MaxAlpha;
    flo32 TractorBeam_dRadius;
    TRACTOR_BEAM_RING TractorBeam_Ring[ TRACTOR_BEAM_RING_MAX_COUNT ];
    
#define PLAYER_COLLECTOR_FLASH_TARGET_TIME  ( 20.0f / 60.0f )
    boo32 Collector_IsEnabled;
    boo32 Collector_IsActive;
    flo32 Collector_Flash_Timer[ SpitType_Count ];
    flo32 Collector_Radians    [ SpitType_Count ];
    
#define PLAYER_COLLECTOR_ANIM_MAX_COUNT    ( 16 )
#define PLAYER_COLLECTOR_ANIM_TARGET_TIME  ( 40.0f / 60.0f )
    int32 Collector_Anim_AtIndex     [ SpitType_Count ];
    flo32 Collector_Anim_Radians     [ SpitType_Count ][ PLAYER_COLLECTOR_ANIM_MAX_COUNT ];
    flo32 Collector_Anim_Timer       [ SpitType_Count ][ PLAYER_COLLECTOR_ANIM_MAX_COUNT ];
    flo32 Collector_Anim_Dist        [ SpitType_Count ][ PLAYER_COLLECTOR_ANIM_MAX_COUNT ];
    boo32 Collector_Anim_IsSterilized[ SpitType_Count ][ PLAYER_COLLECTOR_ANIM_MAX_COUNT ];
    
    ENTITY_SHIELD Shield;
    flo32         Shield_SegmentT[ TURRET_SHIELD_SEGMENT_MAX_COUNT ];
    
    flo32 Rotate_Speed;
    
    flo32       LastDamage_Timer;
    ENTITY_TYPE LastDamage_Type;
    uint8       LastDamage_Buffer[ _KB( 4 ) ];
    
    boo32                 GravityWell_IsEnabled;
    boo32                 GravityWell_IsActive;
    flo32                 GravityWell_Timer;
    int32                nGravityWell_Particle;
    GRAVITY_WELL_PARTICLE GravityWell_Particle[ GRAVITY_WELL_PARTICLE_MAX_COUNT ];
};

//----------
// PROFILE
//----------

struct PROFILE_BLOCK {
    char   Label[ 32 ];
    uint64 CycleCounter_Start;
    uint64 CycleCounter_End;
};

#define PROFILE_BLOCK_MAX_COUNT  ( 16 )
struct PROFILE_STATE {
    boo32         ProfilingIsActive;
    uint32       nProfileBlock;
    PROFILE_BLOCK ProfileBlock[ PROFILE_BLOCK_MAX_COUNT ];
};