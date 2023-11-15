
// TODO: Just putting this function here for now. No reason.
internal rect
GetWorldBound( vec2 App_Dim, flo32 World_YUnits ) {
    flo32  AspectRatio = App_Dim.x / App_Dim.y;
    rect   Result      = RectCD( Vec2( 0.0f, 0.0f ), Vec2( World_YUnits * AspectRatio, World_YUnits ) );
    return Result;
}

internal void
AddUnlockToLevel( APP_STATE * AppState, LEVEL_ID LevelID, LEVEL_ID LevelToUnlock ) {
    OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
    OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
    
    OVERWORLD_MAP * Map = OverworldT->Map + LevelID;
    Assert( Map->nLevelToUnlock < OVERWORLD_LEVEL_TO_UNLOCK_MAX_COUNT );
    Map->LevelToUnlock[ Map->nLevelToUnlock++ ] = LevelToUnlock;
}

internal OVERWORLD_DATA
NewLevel( APP_STATE * AppState, rect * BoundList, LEVEL_ID LevelID, char * DisplayName ) {
    OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
    OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
    
    OVERWORLD_LEVEL * Level = 0;
    OVERWORLD_MAP   * Map   = 0;
    LEVEL_INFO      * Info  = 0;
    
    if( OverworldS->nLevel < OVERWORLD_LEVEL_MAX_COUNT ) {
        OverworldS->nLevel++;
        
        Level = OverworldS->Level + LevelID;
        Map   = OverworldT->Map   + LevelID;
        Info  = &Map->Info;
        
        strcpy( Map->DisplayName, DisplayName );
        Map->Bound        = BoundList[ LevelID ];
        Map->ShowRating   = true;
    }
    
    Assert( Level );
    Assert( Map   );
    Assert( Info  );
    
    OVERWORLD_DATA Result = {};
    Result.Level = Level;
    Result.Map   = Map;
    Result.Info  = Info;
    
    return Result;
}

internal vec2
GetLevelPosForOverworldMap( vec2 A, vec2 B, vec2 C, flo32 tX, flo32 tY ) {
    vec2 P = lerp( A, tY, C );
    vec2 Q = lerp( B, tY, C );
    
    vec2 Result = lerp( P, tX, Q );
    return Result;
}

internal void
GenOverworld( APP_STATE * AppState ) {
    OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
    OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
    
    flo32 Level_AspectRatio = AppState->UI_Dim.x / AppState->UI_Dim.y;
    vec2  Level_Dim         = Vec2( 130.0f * Level_AspectRatio, 130.0f );
    
    flo32 Radius   = 640.0f;
    vec2  Center   = Vec2( 840.0f, 425.0f );
    flo32 bRadians = PI * 1.1667f - PI / 64.0f;
    
    flo32 yMin  = 200.0f;
    flo32 yMax  = 880.0f;
    flo32 xMid  = 840.0f;
    flo32 Width = 1000.0f;
    
    vec2 A = Vec2( xMid - Width * 0.5f,  yMin );
    vec2 B = Vec2( xMid + Width * 0.5f,  yMin );
    vec2 C = Vec2( xMid,                 yMax );
    A.y += 15.0f;
    B.y -= 15.0f;
    C.x += 15.0f;
    
    vec2 Centroid = ( A + B + C ) / 3.0f;
    flo32 Length = GetLength( C - Centroid );
    
    vec2 V0 = ToDirection2D( bRadians ) * Radius;
    vec2 V1 = ToDirection2D( bRadians + TAU * 0.3333f ) * Radius;
    vec2 V2 = ToDirection2D( bRadians + TAU * 0.6667f ) * Radius;
    
    V0.y *= 0.6f;
    V1.y *= 0.6f;
    V2.y *= 0.6f;
    
    A = Center + V0;
    B = Center + V1;
    C = Center + V2;
    
    
    
    int32 nLevel = 12;
    
    rect Level_Bound[ LevelID_Count ] = {};
    vec2 Level_Pos  [ LevelID_Count ] = {};
    Level_Pos[ LevelID_Market          ] = GetLevelPosForOverworldMap( A, B, C, -0.275f,  0.265f );
    Level_Pos[ LevelID_TurretShield    ] = GetLevelPosForOverworldMap( A, B, C,  0.0f,    0.0f );
    Level_Pos[ LevelID_Turret          ] = GetLevelPosForOverworldMap( A, B, C,  0.3333f, 0.0f );
    Level_Pos[ LevelID_Blimp           ] = GetLevelPosForOverworldMap( A, B, C,  0.6667f, -0.05f );
    Level_Pos[ LevelID_Worm            ] = GetLevelPosForOverworldMap( A, B, C,  1.0f,    0.0f );
    Level_Pos[ LevelID_AutoTurret      ] = GetLevelPosForOverworldMap( A, B, C,  1.425f,  0.3f );
    Level_Pos[ LevelID_MegaMiner       ] = GetLevelPosForOverworldMap( A, B, C,  0.0f,    0.3333f );
    Level_Pos[ LevelID_BombLauncher    ] = GetLevelPosForOverworldMap( A, B, C,  0.5f,    0.3333f );
    Level_Pos[ LevelID_GravityWell     ] = GetLevelPosForOverworldMap( A, B, C,  1.0f,    0.3333f );
    Level_Pos[ LevelID_PlayerShield    ] = GetLevelPosForOverworldMap( A, B, C,  0.0f,    0.6667f );
    Level_Pos[ LevelID_Collector       ] = GetLevelPosForOverworldMap( A, B, C,  1.0f,    0.6667f );
    Level_Pos[ LevelID_ObservationDeck ] = GetLevelPosForOverworldMap( A, B, C,  0.0f,    1.0f );
    
    for( int32 iLevel = 0; iLevel < nLevel; iLevel++ ) {
        flo32 tScale = GetT01( Level_Pos[ iLevel ].y, yMin, yMax ) * 0.4f + 1.0f;
        Level_Bound[ iLevel ] = RectCD( Level_Pos[ iLevel ], Level_Dim * tScale );
    }
    Level_Bound[ LevelID_Market ] = RectCD( Level_Pos[ LevelID_Market ], Vec2( Level_Dim.y * 1.25f, Level_Dim.y ) );
    Level_Bound[ LevelID_AutoTurret ] = RectCD( Level_Pos[ LevelID_AutoTurret ], Level_Dim * 0.9f );
    
    // Rating 1 : 
    // Rating 2 : 
    // Rating 3 : Best performance without 1 or 2 lessons applied.
    // Rating 4 : Best performance without 2x Bonus Objective.
    // Rating 5 : Best performance with    2x Bonus Objective. About 80% of my highest score.
    
    { // Market
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_Market, "Market" );
        Data.Map->ShowRating = false;
    }
    
    { // LEVEL 02a
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_TurretShield, "7" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  150000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 2 ] =  500000;
        Data.Info->ScoreRating_Threshold[ 3 ] =  750000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 1000000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_None;
        Data.Info->TargetID   = EntityType_MegaTurret;
        Data.Info->TargetInfo = MEGA_TURRET_HEALTH;
        Data.Info->Time       = 130.0f;
    }
    
    { // LEVEL 01b
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_Turret, "8" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  500000;
        Data.Info->ScoreRating_Threshold[ 2 ] =  750000;
        Data.Info->ScoreRating_Threshold[ 3 ] = 1000000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 1500000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_None;
        Data.Info->TargetID   = EntityType_Recon;
        Data.Info->TargetInfo = LEVEL01B_BONUS_OBJECTIVE_RECON_COUNT;
        Data.Info->Time       = 0.0f;
    }
    
    { // LEVEL 01a
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_Blimp, "9" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  750000;
        Data.Info->ScoreRating_Threshold[ 2 ] = 1250000;
        Data.Info->ScoreRating_Threshold[ 3 ] = 1750000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 2500000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Level->IsAvailable = true;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_None;
        Data.Info->TargetID   = EntityType_Blimp;
        Data.Info->TargetInfo = BLIMP_HEALTH;
        Data.Info->Time       = 120.0f;
    }
    
    { // LEVEL 07
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_Worm, "10" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  125000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 2 ] =  500000;
        Data.Info->ScoreRating_Threshold[ 3 ] = 1000000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 1750000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_None;
        Data.Info->TargetID   = EntityType_Worm;
        Data.Info->TargetInfo = WORM_HEALTH_LARGE * 2;
        Data.Info->Time       = 150.0f;
    }
    
    { // LEVEL 03a
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_AutoTurret, "6" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  575000;
        Data.Info->ScoreRating_Threshold[ 2 ] = 1000000;
        Data.Info->ScoreRating_Threshold[ 3 ] = 2000000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 4000000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_GrappleBeam;
        Data.Info->TargetID   = EntityType_Yacht;
        Data.Info->TargetInfo = 3;
        Data.Info->Time       = -135.0f;
    }
    
    { // LEVEL 04b
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_MegaMiner, "3" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  100000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  200000;
        Data.Info->ScoreRating_Threshold[ 2 ] =  350000;
        Data.Info->ScoreRating_Threshold[ 3 ] =  750000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 1250000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_GrappleBeam;
        Data.Info->TargetID   = EntityType_MegaMiner;
        Data.Info->TargetInfo = MEGA_MINER_HEALTH;
        Data.Info->Time       = 150.0f;
    }
    
    { // LEVEL 02b
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_BombLauncher, "4" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  750000;
        Data.Info->ScoreRating_Threshold[ 2 ] = 1000000;
        Data.Info->ScoreRating_Threshold[ 3 ] = 1250000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 2500000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_TractorBeam;
        Data.Info->TargetID   = EntityType_BombLauncher;
        Data.Info->TargetInfo = BOMB_LAUNCHER_HEALTH * BOMB_LAUNCHER_MAX_COUNT;
        Data.Info->Time       = 120.0f;
    }
    
    { // LEVEL 05
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_GravityWell, "5" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  125000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 2 ] =  500000;
        Data.Info->ScoreRating_Threshold[ 3 ] = 1000000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 2500000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_GravityWell;
        Data.Info->TargetID   = EntityType_Satellite;
        Data.Info->TargetInfo = SATELLITE_HEALTH;
        Data.Info->Time       = 130.0f;
    }
    
    { // LEVEL 04a
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_PlayerShield, "1" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  500000;
        Data.Info->ScoreRating_Threshold[ 2 ] = 1000000;
        Data.Info->ScoreRating_Threshold[ 3 ] = 2000000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 4000000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_Shield;
        Data.Info->TargetID   = EntityType_Miner_Stationary;
        Data.Info->TargetInfo = 4;
        Data.Info->Time       = 140.0f;
    }
    
    { // LEVEL 08
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_Collector, "2" );
        Data.Info->ScoreRating_Threshold[ 0 ] =   50000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  125000;
        Data.Info->ScoreRating_Threshold[ 2 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 3 ] =  500000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 1000000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_Collector;
        Data.Info->TargetID   = EntityType_Loader;
        Data.Info->TargetInfo = LOADER_HEALTH;
        Data.Info->Time       = 165.0f;
    }
    
    { // LEVEL 09
        OVERWORLD_DATA Data = NewLevel( AppState, Level_Bound, LevelID_ObservationDeck, "0" );
        Data.Info->ScoreRating_Threshold[ 0 ] =  100000;
        Data.Info->ScoreRating_Threshold[ 1 ] =  250000;
        Data.Info->ScoreRating_Threshold[ 2 ] =  500000;
        Data.Info->ScoreRating_Threshold[ 3 ] =  750000;
        Data.Info->ScoreRating_Threshold[ 4 ] = 1250000;
        Data.Info->BackgroundID = TextureID_Background_01;
        Data.Info->WeaponID   = ItemID_Blaster;
        Data.Info->GadgetID   = ItemID_GrappleBeam;
        Data.Info->TargetID   = EntityType_ObservationDeck;
        Data.Info->TargetInfo = OBSERVATION_DECK_HEALTH;
        Data.Info->Time       = 180.0f;
    }
    
    {
        AddUnlockToLevel( AppState, LevelID_Blimp,        LevelID_Market   );
        AddUnlockToLevel( AppState, LevelID_Blimp,        LevelID_Turret );
        AddUnlockToLevel( AppState, LevelID_Blimp,        LevelID_TurretShield );
        AddUnlockToLevel( AppState, LevelID_Blimp,        LevelID_Worm  );
        
        AddUnlockToLevel( AppState, LevelID_TurretShield, LevelID_MegaMiner );
        AddUnlockToLevel( AppState, LevelID_TurretShield, LevelID_BombLauncher );
        AddUnlockToLevel( AppState, LevelID_TurretShield, LevelID_GravityWell  );
        AddUnlockToLevel( AppState, LevelID_TurretShield, LevelID_AutoTurret );
        
        AddUnlockToLevel( AppState, LevelID_Turret,       LevelID_MegaMiner );
        AddUnlockToLevel( AppState, LevelID_Turret,       LevelID_BombLauncher );
        AddUnlockToLevel( AppState, LevelID_Turret,       LevelID_GravityWell  );
        AddUnlockToLevel( AppState, LevelID_Turret,       LevelID_AutoTurret );
        
        AddUnlockToLevel( AppState, LevelID_Worm,         LevelID_MegaMiner );
        AddUnlockToLevel( AppState, LevelID_Worm,         LevelID_BombLauncher );
        AddUnlockToLevel( AppState, LevelID_Worm,         LevelID_GravityWell  );
        AddUnlockToLevel( AppState, LevelID_Worm,         LevelID_AutoTurret );
        
        AddUnlockToLevel( AppState, LevelID_MegaMiner,    LevelID_PlayerShield );
        AddUnlockToLevel( AppState, LevelID_MegaMiner,    LevelID_Collector  );
        
        AddUnlockToLevel( AppState, LevelID_BombLauncher, LevelID_PlayerShield );
        AddUnlockToLevel( AppState, LevelID_BombLauncher, LevelID_Collector  );
        
        AddUnlockToLevel( AppState, LevelID_GravityWell,  LevelID_PlayerShield );
        AddUnlockToLevel( AppState, LevelID_GravityWell,  LevelID_Collector  );
        
        AddUnlockToLevel( AppState, LevelID_PlayerShield, LevelID_ObservationDeck );
        AddUnlockToLevel( AppState, LevelID_Collector,    LevelID_ObservationDeck );
    }
    
    { // Init Gravity Well Particles
        for( int32 iParticle = 0; iParticle < OVERWORLD_GRAVITY_WELL_PARTICLE_MAX_COUNT; iParticle++ ) {
            GRAVITY_WELL_PARTICLE * Particle = OverworldT->GravityWell_Particle + iParticle;
            
            Particle->Speed      = PLAYER_ROTATE_SPEED_SLOW;
            Particle->TargetTime = RandomF32InRange( 1.0f, 5.0f );
            Particle->Timer      = RandomF32InRange( 0.0f, Particle->TargetTime );
            Particle->Radians    = RandomF32() * PI * 2.0f;
            Particle->Dist       = RandomF32InRange( OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_INNER, OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_OUTER );
            Particle->tSpeed     = RandomF32InRange( 0.75f, 1.25f );
            Particle->iColor     = ( uint8 )RandomU32InRange( 0, 3 );
        }
    }
    
#if 0
    for( int32 iLevel = 0; iLevel < OverworldS->nLevel; iLevel++ ) {
        OVERWORLD_LEVEL * Level = OverworldS->Level + iLevel;
        Level->IsAvailable = true;
    }
#endif
}