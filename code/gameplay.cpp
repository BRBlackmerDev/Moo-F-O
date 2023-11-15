

//----------
// SAVE/LOAD GAME
//----------

#define VERSION__SAVEGAME_FILE  ( 1 )
internal void
SaveGame( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    // filetag
    // version
    // AtLevel
    // nLevels
    // for each level
    //  save OVERWORLD_LEVEL struct
    
    {
        char * FileTag  = FILETAG__SAVEGAME_FILE;
        uint32 Version  = VERSION__SAVEGAME_FILE;
        char * SaveDir  = 0;
        char * FileName = "MyGame";
        // TODO: Allow different game files!
        
        OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
        Assert( OverworldS->nLevel > 0 );
        
        MEMORY  _Output = SubArena( TempMemory );
        MEMORY * Output = &_Output;
        
        WriteEntityFileHeader( Output, FileTag, Version );
        
        _writem( Output, SAVE_GAME, AppState->SaveGame );
        //_writem( Output, OVERWORLD_STATE_SAVE_DATA, AppState->SaveGame.OverworldSave );
        //_writem( Output, MARKET_STATE_SAVE_DATA,    AppState->MarketS.SaveData );
        
        boo32 IsValid = OutputFile( Platform, Output, SaveDir, FileName, FileTag );
        Assert( IsValid );
        
        _PopSize( TempMemory, Output->Size );
    }
    
#if 0    
    {
        char * FileTag  = "POWERUPS";
        uint32 Version  = 0;
        char * SaveDir  = LEVEL_SAVE_DIRECTORY;
        char * FileName = "MyGame";
        
        MEMORY  _Output = SubArena( TempMemory );
        MEMORY * Output = &_Output;
        
        WriteEntityFileHeader( Output, FileTag, Version );
        
        _writem( Output, uint64, AppState->Market_TotalPoints );
        for( int32 iPowerup = 0; iPowerup < PowerupType_Count; iPowerup++ ) {
            _writem( Output, POWERUP_ENTRY, PowerupSave->PowerupEntry[ iPowerup ] );
        }
        
        boo32 IsValid = OutputFile( Platform, Output, SaveDir, FileName, FileTag );
        Assert( IsValid );
        
        _PopSize( TempMemory, Output->size );
    }
#endif
}

internal OVERWORLD_LEVEL
ReadOverworldLevel( uint32 Version, uint8 ** Ptr ) {
    OVERWORLD_LEVEL Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            Result = _read( ptr, OVERWORLD_LEVEL );
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
LoadGame( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    {
        char * SaveDir  = 0;
        char * FileTag  = FILETAG__SAVEGAME_FILE;
        char * FileName = "MyGame";
        
        FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
        if( File.Contents ) {
            uint8 * ptr = ( uint8 * )File.Contents;
            
            uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
            
            uint32 CheckSize = sizeof( ENTITY_FILE_HEADER )
                + sizeof( SAVE_GAME );
            Assert( CheckSize == File.Size );
            
            AppState->SaveGame = _read( ptr, SAVE_GAME );
            //AppState->SaveGame.OverworldSave = _read( ptr, OVERWORLD_STATE_SAVE_DATA );
            //AppState->MarketS.SaveData    = _read( ptr, MARKET_STATE_SAVE_DATA );
            
#if 0            
            OVERWORLD_STATE_SAVE_DATA Overworld_SaveData = _read( ptr, OVERWORLD_STATE_SAVE_DATA );
            AppState->MarketS.SaveData                   = _read( ptr, MARKET_STATE_SAVE_DATA );
            
            OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
            OverworldS->AtLevel = MinValue( Overworld_SaveData.AtLevel, OverworldS->nLevel );
            for( int32 iLevelA = 0; iLevelA < Overworld_SaveData.nLevel; iLevelA++ ) {
                OVERWORLD_LEVEL LevelA = Overworld_SaveData.Level[ iLevelA ];
                
                boo32 WasFound = false;
                for( int32 iLevelB = 0; ( !WasFound ) && ( iLevelB < OverworldS->nLevel ); iLevelB++ ) {
                    OVERWORLD_LEVEL * LevelB = OverworldS->Level + iLevelB;
                    
                    if( MatchString( LevelB->FileName, LevelA.FileName ) ) {
                        WasFound = true;
                        
                        ( *LevelB ) = LevelA;
                    }
                }
            }
#endif
            
            
            _PopSize( TempMemory, File.Size );
        }
    }
    
#if 0    
    {
        char * SaveDir  = LEVEL_SAVE_DIRECTORY;
        char * FileTag  = "POWERUPS";
        char * FileName = "MyGame";
        
        uint8 * Ptr       = 0;
        uint32  CheckSize = sizeof( ENTITY_FILE_HEADER ) + sizeof( uint64 ) + sizeof( POWERUP_ENTRY ) * PowerupType_Count;
        
        FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
        if( File.Contents ) {
            Ptr = ( uint8 * )File.Contents;
            uint32 Version = VerifyEntityHeaderAndGetVersion( &Ptr, FileTag );
        }
        
        if( ( Ptr ) && ( CheckSize == File.Size ) ) {
            AppState->Market_TotalPoints = _read( Ptr, uint64 );
            for( int32 iPowerup = 0; iPowerup < PowerupType_Count; iPowerup++ ) {
                PowerupSave->PowerupEntry[ iPowerup ] = _read( Ptr, POWERUP_ENTRY );
            }
            
            _PopSize( TempMemory, File.Size );
        } else {
            GenPowerupStats( AppState );
        }
    }
#endif
}

//----------
// GAMEPLAY FUNCTIONS
//----------

internal void
ResetGameData( APP_STATE * AppState ) {
    memset( &AppState->SaveGame, 0, sizeof( SAVE_GAME ) );
}

internal void
InitOverworld( APP_STATE * AppState ) {
    OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
    OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
    
    OverworldS->AtLevel = LevelID_Blimp;
    
    OVERWORLD_LEVEL * Level = OverworldS->Level + OverworldS->AtLevel;
    OVERWORLD_MAP   * Map   = OverworldT->Map   + OverworldS->AtLevel;
    Level->IsAvailable = true;
    
    OverworldT->Player_Position = GetCenter( Map->Bound );
}

internal void
InitGameData( APP_STATE * AppState ) {
    InitPoints   ( AppState );
    //InitMarket   ( AppState );
    InitOverworld( AppState );
    InitPowerups ( AppState, 120.0f );
}

internal void
ResetLevelMemory( APP_STATE * AppState ) {
    //ResetCollision( AppState );
    
    uint8 * AddrA = ( uint8 * )&AppState->Reset_LevelReset_Start;
    uint8 * AddrB = ( uint8 * )&AppState->Reset_LevelReset_End;
    uint32 Reset_Size = ( uint32 )( AddrB - AddrA );
    memset( AddrA, 0, Reset_Size );
}

internal void
InitPlayerForLevel( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->MaxHealth = PLAYER_HEALTH;
    Player->Health    = PLAYER_HEALTH;
    Player->Facing_Radians = PI * 0.5f;
}

internal void
InitScoreForLevel( APP_STATE * AppState ) {
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    ScoreS->Multiplier = 1.0f;
    
    //BUMPER_STATE * BumperS = &AppState->BumperS;
    //BumperS->BasePoints = 1;
}

internal void
InitCowForOverworld( APP_STATE * AppState ) {
    OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
    OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
    OVERWORLD_MAP             * Map        = OverworldT->Map + OverworldS->AtLevel;
    
    TITLE_MENU_COW * Cow = &AppState->Overworld_Cow;
    Cow->IsInitialized = true;
    Cow->Position      = {};
    Cow->Destination   = Cow->Position;
    Cow->AtTheta       = RandomF32() * TAU;
    Cow->AtPhi         = RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.4f );
    Cow->DestPhi       = Cow->AtPhi;
    Cow->Direction     = Vec3( cosf( Cow->AtPhi ) * cosf( Cow->AtTheta ), sinf( Cow->AtPhi ), -sinf( Cow->AtTheta ) );
    Cow->Velocity      = Cow->Direction * COW_SPEED_FOR_OVERWORLD_MAP;
}

internal void
StartGame( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    
#if START_IN_BLANK_LEVEL
    MOUSE_STATE * Mouse = &Platform->Mouse;
    Mouse->ResetPos     = true;
    Mouse->DoHideCursor = true;
    
    ResetLevelMemory( AppState );
    
    AppState->Mode        = AppMode_Game;
    AppState->World_Bound = GetWorldBound( AppState->App_Dim, 48.0f );
    
    InitPoints( AppState );
    
    InitPlayerForLevel( AppState );
    InitScoreForLevel ( AppState );
    InitCameraForLevel( AppState );
    
    PLAYER_STATE * Player = &AppState->Player;
    Player->TractorBeam_IsEnabled = true;
    flo32 MinRadius = TRACTOR_BEAM_RING_MIN_RADIUS;
    flo32 MaxRadius = TRACTOR_BEAM_RING_MAX_RADIUS;
    Player->TractorBeam_Ring[ 0 ].Radius = lerp( MinRadius, 1.0f / 3.0f, MaxRadius );
    Player->TractorBeam_Ring[ 1 ].Radius = lerp( MinRadius, 2.0f / 3.0f, MaxRadius );
    Player->TractorBeam_Ring[ 2 ].Radius = lerp( MinRadius, 3.0f / 3.0f, MaxRadius );
    Player->TractorBeam_Ring[ 2 ].DoShow = true;
    //Player->ZeroPoint_IsEnabled = true;
    //Player->Collector_IsEnabled = true;
    //Player->Position         = Vec2( 0.0f, -12.0f );
    //Player->Shield_IsEnabled = true;
    //Player->Shield.HasShield = true;
    //Player->Shield.bRadians  = 0.0f;
    //Player->Shield.Radians   = TAU;
    //Player->Shield.Dist      = 0.75f;
    //Player->Shield.Width     = 0.75f;
    
#if 0
    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
    AsteroidS->Spawn_IsActive    = true;
    AsteroidS->Spawn_TargetTime  = 1.0f;
    AsteroidS->Spawn_MaxAsteroid = -1;
    AsteroidS->Numer_TypeClass[ AsteroidType_Default ][ AsteroidClass_Large  ] = 1;
    AsteroidS->Numer_TypeClass[ AsteroidType_Default ][ AsteroidClass_Medium ] = 1;
    AsteroidS->Numer_TypeClass[ AsteroidType_Default ][ AsteroidClass_Small  ] = 1;
    AsteroidS->Spawn_SideIsEnabled[ 0 ] = true;
    AsteroidS->Spawn_SideIsEnabled[ 1 ] = true;
    AsteroidS->Spawn_SideIsEnabled[ 2 ] = true;
    AsteroidS->Spawn_SideIsEnabled[ 3 ] = true;
    AsteroidS->Spawn_Bound = AppState->World_Bound;
#endif
    
    
#else
    AppState->Mode = AppMode_Overworld;
    
    if( Platform->DoesFileExist( 0, "MyGame", FILETAG__SAVEGAME_FILE ) ) {
        LoadGame( Platform, AppState, TempMemory );
        
        OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
        OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
        //OverworldS->AtLevel         = 10;
        OVERWORLD_MAP             * Map        = OverworldT->Map + OverworldS->AtLevel;
        OverworldT->Player_Position = GetCenter( Map->Bound );
        
        InitPoints( AppState );
    } else {
        InitGameData( AppState );
    }
#endif
    
    AppState->InGameMenu_ShowMenu    = false;
    AppState->InGameMenu_ShowConfirm = false;
}

internal uint64
OutputClockElapsedToDebugConsole( uint64 Clock_Start, uint64 Clock_End, char * Label = 0 ) {
    uint64 Elapsed = Clock_End - Clock_Start;
    
    char Str[ 128 ] = {};
    if( Label ) {
        sprintf( Str, "%s: %llu\n", Label, Elapsed );
    } else {
        sprintf( Str, "%llu\n", Elapsed );
    }
    CONSOLE_STRING( Str );
    
    uint64 Result = Clock_End;
    return Result;
}

internal void
StartLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, LEVEL_ID LevelID ) {
    Platform->Mouse.DoHideCursor    = true;
    
    uint64 Clock = __rdtsc();
    
    ResetLevelMemory( AppState );
    
    Clock = OutputClockElapsedToDebugConsole( Clock, __rdtsc(), "Clear Memory." );
    
    AppState->Mode = AppMode_Game;
    
    //GAME_LoadLevel( Platform, AppState, TempMemory, LEVEL_SAVE_DIRECTORY, FileName );
    
    {
        ASSET_LEVEL Level = AppState->LevelList[ LevelID ];
        
        WAVE_STATE * WaveS = &AppState->WaveS;
        WaveS->StartingWave = Level.StartingWave;
        WaveS->nWave        = Level.nWave;
        for( int32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WaveS->Wave[ iWave ] = Level.Wave[ iWave ];
        }
        
        AppState->World_Bound = GetWorldBound( AppState->UI_Dim, Level.World_YUnits );
        AppState->CurrentLevel.LevelID = LevelID;
    }
    
    Clock = OutputClockElapsedToDebugConsole( Clock, __rdtsc(), "Load Asset Data." );
    
    LEVEL_INFO Level_Info       = AppState->CurrentLevel.Info;
    
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    ScoreS->Level_DisplayTimer_DoUpdate = true;
    ScoreS->Level_DisplayTimer          = Level_Info.Time;
    
    flo32 Level_TargetTime = Level_Info.Time;
    
    switch( LevelID ) {
        case LevelID_Blimp: {
            // Do Nothing.
        } break;
        
        case LevelID_Turret: {
            ScoreS->Level_DisplayTimer_DoUpdate = false;
            ScoreS->Level_DisplayTimer          = -1.0f;
            
            // NOTE : This is a generic default value because this level doesn't end using a timer.
            Level_TargetTime = 120.0f;
            
            int32 nReconX = LEVEL01B_BONUS_OBJECTIVE_RECON_X_COUNT;
            int32 nReconY = LEVEL01B_BONUS_OBJECTIVE_RECON_Y_COUNT;
            
            rect  Bound   = AddRadius( AppState->World_Bound, 0.75f );
            
            flo32 OffsetX = GetWidth ( Bound ) / ( flo32 )( nReconX + 1 );
            flo32 OffsetY = GetHeight( Bound ) / ( flo32 )( nReconY );
            
            vec2  bPos    [ 4 ] = { GetBL( Bound ), GetTL( Bound ) + Vec2( OffsetX * 0.5f, 0.0f ), GetTR( Bound ), GetBR( Bound ) + Vec2( -OffsetX * 0.5f, 0.0f ) };
            vec2  bDir    [ 4 ] = { Vec2( 0.0f, 1.0f ), Vec2( 1.0f, 0.0f ), Vec2( 0.0f, -1.0f ), Vec2( -1.0f, 0.0f ) };
            int32 bRecon  [ 4 ] = { nReconY, nReconX, nReconY, nReconX };
            flo32 bOffset [ 4 ] = { OffsetY, OffsetX, OffsetY, OffsetX };
            flo32 bRadians[ 4 ] = { PI * 0.5f, 0.0f, -PI * 0.5f, -PI };
            
            for( int32 iSide = 0; iSide < 4; iSide++ ) {
                vec2  Pos     = bPos    [ iSide ];
                vec2  Dir     = bDir    [ iSide ];
                int32 nRecon  = bRecon  [ iSide ];
                flo32 Offset  = bOffset [ iSide ];
                flo32 Radians = bRadians[ iSide ];
                
                for( int32 iRecon = 0; iRecon < nRecon; iRecon++ ) {
                    vec2 P = Pos + Dir * ( Offset * ( ( flo32 )iRecon + 0.5f ) );
                    AddRecon( AppState, ReconC( -1, P, Radians ) );
                }
            }
        } break;
        
        case LevelID_TurretShield: {
            ScoreS->Level_DisplayTimer_DoUpdate = false;
            
            TURRET_STATE * TurretS = &AppState->TurretS;
            TurretS->Spawn_xSpacing = 12.0f;
            TurretS->BonusObjective_nMegaTurretPossibleKills = 1;
        } break;
        
        case LevelID_BombLauncher: {
            PLAYER_STATE * Player = &AppState->Player;
            Player->TractorBeam_IsEnabled = true;
            
            flo32 X = 38.0f;
            flo32 Y = 13.5f;
            
            BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
            BombLauncherS->BombLauncher[ 0 ] = InitBombLauncher( Vec2( -X, -Y ), Vec2(  1.0f, 0.0f ) );
            BombLauncherS->BombLauncher[ 1 ] = InitBombLauncher( Vec2( -X,  Y ), Vec2(  1.0f, 0.0f ) );
            BombLauncherS->BombLauncher[ 2 ] = InitBombLauncher( Vec2(  X,  Y ), Vec2( -1.0f, 0.0f ) );
            BombLauncherS->BombLauncher[ 3 ] = InitBombLauncher( Vec2(  X, -Y ), Vec2( -1.0f, 0.0f ) );
            BombLauncherS->nBombLauncher = BOMB_LAUNCHER_MAX_COUNT;
            
            BombLauncherS->BonusObjective_SpawnCooldownTimer     = -1.0f;
            BombLauncherS->BonusObjective_MaxBombLauncherToSpawn = 2;
            
            TURRET_STATE * TurretS = &AppState->TurretS;
            TurretS->Spawn_xSpacing = 11.0f;
        } break;
        
        case LevelID_AutoTurret: {
            PLAYER_STATE * Player = &AppState->Player;
            Player->ZeroPoint_IsEnabled = true;
            
            AddAutoTurret( AppState, AutoTurretC( -1, Vec2( -16.0f,  4.0f ) ) );
            AddAutoTurret( AppState, AutoTurretC( -1, Vec2( 12.0f, -6.0f ) ) );
            
            AddBayDoorSpawn( AppState, BayDoorSpawnC( ORectCD( Vec2( -30.0f, 19.0f ), Vec2( 6.0f, 2.0f ), PI * 0.0625f ) ) );
            AddBayDoorSpawn( AppState, BayDoorSpawnC( ORectCD( Vec2( 0.0f,  21.0f ), Vec2( 10.0f, 4.0f ) ) ) );
            AddBayDoorSpawn( AppState, BayDoorSpawnC( ORectCD( Vec2( 40.0f, 0.0f ), Vec2(  12.0f, 6.0f ), -PI * 0.5f ) ) );
            AddBayDoorSpawn( AppState, BayDoorSpawnC( ORectCD( Vec2( 28.0f, -20.0f ), Vec2( 8.0f, 2.0f ), PI * 1.0625f ) ) );
            AddBayDoorSpawn( AppState, BayDoorSpawnC( ORectCD( Vec2( -35.0f, -19.0f ), Vec2(  6.0f, 3.0f ), PI * 0.75f ) ) );
            
            BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
            BayDoorS->BayDoorSpawn_Src [ 0 ] = ORectCD( Vec2( -30.0f,  19.0f ), Vec2( 6.0f, 2.0f ), PI * 0.0625f );
            BayDoorS->BayDoorSpawn_Src [ 1 ] = ORectCD( Vec2(   0.0f,  21.0f ), Vec2( 10.0f, 4.0f ) );
            BayDoorS->BayDoorSpawn_Src [ 2 ] = ORectCD( Vec2(  40.0f,   0.0f ), Vec2(  12.0f, 6.0f ), -PI * 0.5f );
            BayDoorS->BayDoorSpawn_Src [ 3 ] = ORectCD( Vec2(  28.0f, -20.0f ), Vec2( 8.0f, 2.0f ), PI * 1.0625f );
            BayDoorS->BayDoorSpawn_Src [ 4 ] = ORectCD( Vec2( -35.0f, -19.0f ), Vec2(  6.0f, 3.0f ), PI * 0.75f );
            BayDoorS->BayDoorSpawn_Dest[ 0 ] = ORectCD( Vec2( -44.0f,   2.0f ), Vec2( 24.0f, 2.0f ), PI * 0.5f );
            BayDoorS->BayDoorSpawn_Dest[ 1 ] = ORectCD( Vec2(  -9.0f, -26.0f ), Vec2( 34.0f, 2.0f ), PI );
            BayDoorS->BayDoorSpawn_Dest[ 2 ] = ORectCD( Vec2(  32.0f,  26.0f ), Vec2( 18.0f, 2.0f ), 0 );
            
            BayDoorS->SpawnPersonalPod_Timer = BAY_DOOR_SPAWN_PERSONAL_POD_COOLDOWN_TARGET_TIME - 4.0f;
            
            { // Set Personal Pod Spawns
                BayDoorS->SpawnPersonalPod_IsActive      = true;
                
                int32 nSpawn     = BAY_DOOR_SPAWN_PERSONAL_POD_SPAWN_COUNT;
                int32 nHalfSpawn = nSpawn / 2;
                Assert( ( nSpawn % 2 ) == 0 );
                
                int32 nMinPersonalPod   = 40;
                int32 nMaxPersonalPod   = 140;
                int32 nPersonalPodSpawn = ( nMinPersonalPod + nMaxPersonalPod ) * nHalfSpawn;
                
                for( int32 iSpawn = 0; iSpawn < nHalfSpawn; iSpawn++ ) {
                    int32 nPersonalPodA = RandomS32InRange( nMinPersonalPod, nMaxPersonalPod );
                    int32 nPersonalPodB = ( nMaxPersonalPod + nMinPersonalPod ) - nPersonalPodA;
                    
                    BayDoorS->SpawnPersonalPod_nPersonalPod[ ( iSpawn * 2 )     ] = nPersonalPodA;
                    BayDoorS->SpawnPersonalPod_nPersonalPod[ ( iSpawn * 2 ) + 1 ] = nPersonalPodB;
                }
                
                int32 AtIndex = 9;
                BubbleSort( BayDoorS->SpawnPersonalPod_nPersonalPod,                    AtIndex );
                BubbleSort( BayDoorS->SpawnPersonalPod_nPersonalPod + AtIndex, nSpawn - AtIndex );
                
#if 0            
                for( int32 iSpawn = 0; iSpawn < nSpawn; iSpawn++ ) {
                    int32 Select = RandomS32InRange( iSpawn, nSpawn - 1 );
                    
                    if( iSpawn != Select ) {
                        _swap( int32, BayDoorS->SpawnPersonalPod_nPersonalPod[ iSpawn ], BayDoorS->SpawnPersonalPod_nPersonalPod[ Select ] );
                    }
                }
#endif
                
                {
                    int32 nPersonalPod = BayDoorS->SpawnPersonalPod_nPersonalPod[ nSpawn - 1 ];
                    flo32 TargetTime   = ( flo32 )nPersonalPod * BAY_DOOR_SPAWN_PERSONAL_POD_TARGET_TIME_SLOW;
                    BayDoorS->SpawnPersonalPod_TargetTimeForLevel = ( BAY_DOOR_SPAWN_PERSONAL_POD_SPAWN_COUNT - 1 ) * BAY_DOOR_SPAWN_PERSONAL_POD_COOLDOWN_TARGET_TIME + TargetTime + PERSONAL_POD_BAY_DOORS_TARGET_TIME * 2.0f;
                }
            }
            
            { // Set Transport and Yacht Spawns
                BayDoorS->SpawnTransport_IsActive   = true;
                int32 nTransportSpawn = 5;
                int32 nYachtSpawn     = 5;
                
                int32 nSpawn = BAY_DOOR_SPAWN_TRANSPORT_YACHT_SPAWN_COUNT;
                Assert( ( nTransportSpawn + nYachtSpawn ) == nSpawn );
                
                BayDoorS->SpawnTransport_DoYacht[ --nSpawn ] = true;
                nYachtSpawn -= 1;
                
                for( int32 iSpawn = 0; iSpawn < nSpawn; iSpawn++ ) {
                    boo32 DoYacht = ( RandomS32InRange( 0, 1 ) == 1 );
                    
                    if( (  DoYacht ) && ( nYachtSpawn <= 0 ) ) {
                        DoYacht = false;
                    }
                    if( ( !DoYacht ) && ( nTransportSpawn <= 0 ) ) {
                        DoYacht = true;
                    }
                    
                    if( DoYacht ) {
                        Assert( nYachtSpawn > 0 );
                        nYachtSpawn -= 1;
                    } else {
                        Assert( nTransportSpawn > 0 );
                        nTransportSpawn -= 1;
                    }
                    
                    BayDoorS->SpawnTransport_DoYacht[ iSpawn ] = DoYacht;
                }
            }
            
            Level_TargetTime = BayDoorS->SpawnPersonalPod_TargetTimeForLevel;
            ScoreS->Level_DisplayTimer = Level_TargetTime;
        } break;
        
        case LevelID_PlayerShield: {
            MINER_STATE * MinerS = &AppState->MinerS;
            MinerS->BonusObjective_DoNext = true;
            
            PLAYER_STATE * Player = &AppState->Player;
            Player->Shield_IsEnabled = true;
            Player->Position         = Vec2( 0.0f, -12.0f );
            Player->Shield.HasShield = true;
            Player->Shield.bRadians  = 0.0f;
            Player->Shield.Radians   = TAU;
            Player->Shield.Dist      = 0.5f;
            Player->Shield.Width     = 1.0f;
        } break;
        
        case LevelID_MegaMiner: {
            PLAYER_STATE * Player = &AppState->Player;
            Player->ZeroPoint_IsEnabled   = true;
        } break;
        
        case LevelID_GravityWell: {
            PLAYER_STATE * Player = &AppState->Player;
            Player->GravityWell_IsEnabled = true;
            
            vec2 Pos = GetT( AppState->World_Bound, Vec2( 1.0f, 0.5f ) ) + Vec2( -5.0f, 0.0f );
            AddSatellite( AppState, SatelliteC( -1, Pos ) );
            
            SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
            SatelliteS->Shield_BounceDir = Vec2( -1.0f, 0.0f );
            SatelliteS->Shield_Bound         = AppState->World_Bound;
            SatelliteS->Shield_Bound.Left    = Pos.x - SATELLITE_SHIELD_WORLD_BOUND_OFFSET;
            SatelliteS->Shield_Bound.Right  += 20.0f;
            SatelliteS->Shield_Bound.Top    += 20.0f;
            SatelliteS->Shield_Bound.Bottom -= 20.0f;
            SatelliteS->Shield_bBound        = SatelliteS->Shield_Bound;
            
            SatelliteS->Move_Bound  = RectCD( Pos, Vec2( 0.0f, SATELLITE_MOVE_INIT_OFFSET ) );
        } break;
        
        case LevelID_Worm: {
            WORM_STATE * WormS = &AppState->WormS;
            WormS->SpawnLarge_IsActive       = true;
            WormS->SpawnLarge_Cooldown_Timer = 2.0f;
            
            WormS->Spawn_nAdditionalSmallWormsAtStart = 1;
        } break;
        
        case LevelID_Collector: {
            PLAYER_STATE * Player = &AppState->Player;
            Player->Collector_IsEnabled = true;
            Player->Collector_IsActive  = true;
            
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            SpitterS->Barrel_TargetTime = lerp( 1.0f, RandomF32_2Dice(), 5.0f );
            
            SpitterS->SpitBonus_IsActive   = true;
            SpitterS->SpitBonus_Type       = ( SPIT_TYPE )RandomS32InRange( 0, 2 );
            SpitterS->SpitBonus_NextType   = ( SPIT_TYPE )RandomS32InRange( 0, 2 );
            SpitterS->SpitBonus_TargetTime = SPITTER_SPIT_BONUS_CYCLE_TARGET_TIME;
            
            //SpitterS->AtPhase       = -1;
            //SpitterS->AtPhase_Timer = FLT_MAX;
            
            
            AddSpitter( AppState, SpitterC( SpitterType_Spiral, SpitterGroupID_Spiral, GetT( AppState->World_Bound, Vec2( 0.5f, 0.5f ) ), RandomDirection2D() ) );
            AddSpitter( AppState, SpitterC( SpitterType_Spiral, SpitterGroupID_Spiral, GetT( AppState->World_Bound, Vec2( 0.3f, 0.65f ) ), RandomDirection2D() ) );
            AddSpitter( AppState, SpitterC( SpitterType_Spiral, SpitterGroupID_Spiral, GetT( AppState->World_Bound, Vec2( 0.75f, 0.3f ) ), RandomDirection2D() ) );
            SpitterS->Spiral_nSpitters  = SpitterS->nSpitter;
            SpitterS->Spiral_TargetTime = 15.0f;
            SpitterS->Spiral_Timer      = SpitterS->Spiral_TargetTime - 8.0f;
            
            {
                flo32 SyncTime     = 12.0f;
                flo32 TimePerCycle = ( 12.0f / 60.0f ) * 3.0f + SPITTER_WARMUP_TARGET_TIME_SINGLE;
                flo32 nCycleA      = 2.0f;
                flo32 nCycleB      = 3.0f;
                
                flo32 bGapTimeA = ( SyncTime - ( nCycleA * TimePerCycle ) ) / nCycleA;
                flo32 bGapTimeB = ( SyncTime - ( nCycleB * TimePerCycle ) ) / nCycleB;
                
                flo32 Offset = ( PLAYER_RADIUS + PLAYER_COLLECTOR_RADIUS + PLAYER_COLLECTOR_DIST_FROM_PLAYER ) * 0.5f * sqrtf( 3.0f );
                vec2  bPos = GetT( AppState->World_Bound, Vec2( 0.1f, 0.2f ) );
                vec2  PosA = bPos + Vec2( 0.0f,  Offset );
                vec2  PosB = bPos + Vec2( 0.0f, -Offset );
                
                PosB = GetT( AppState->World_Bound, Vec2( 0.1f, 0.15f ) );
                
                //AddSpitter( AppState, SpitterC( SpitterType_Double, SpitterGroupID_Double, PosA, Vec2(  1.0f, 0.0f ), bGapTimeA ) );
                AddSpitter( AppState, SpitterC( SpitterType_Double, SpitterGroupID_Double, PosB, Vec2(  1.0f, 0.0f ), bGapTimeB ) );
            }
            
            {
                flo32 SyncTime     = 18.0f;
                flo32 TimePerCycle = ( 12.0f / 60.0f ) * 3.0f + SPITTER_WARMUP_TARGET_TIME_SINGLE;
                flo32 nCycleA      = 3.0f;
                flo32 nCycleB      = 5.0f;
                
                flo32 bGapTimeA = ( SyncTime - ( nCycleA * TimePerCycle ) ) / nCycleA;
                flo32 bGapTimeB = ( SyncTime - ( nCycleB * TimePerCycle ) ) / nCycleB;
                
                flo32 Offset = ( PLAYER_RADIUS + PLAYER_COLLECTOR_RADIUS + PLAYER_COLLECTOR_DIST_FROM_PLAYER ) * 0.5f * sqrtf( 3.0f );
                vec2  bPos = GetT( AppState->World_Bound, Vec2( 0.9f, 0.8f ) );
                vec2  PosA = bPos + Vec2( 0.0f,  Offset );
                vec2  PosB = bPos + Vec2( 0.0f, -Offset );
                
                PosB = GetT( AppState->World_Bound, Vec2( 0.9f, 0.75f ) );
                
                //AddSpitter( AppState, SpitterC( SpitterType_Double, SpitterGroupID_Double, PosA, Vec2( -1.0f, 0.0f ), bGapTimeA ) );
                AddSpitter( AppState, SpitterC( SpitterType_Double, SpitterGroupID_Double, PosB, Vec2( -1.0f, 0.0f ), bGapTimeB ) );
            }
            
            
            AddSpitter( AppState, SpitterC( SpitterType_Single, SpitterGroupID_Single_Corner, GetT( AppState->World_Bound, Vec2( 0.1f, 0.8f ) ), Vec2(  1.0f, 0.0f ) ) );
            AddSpitter( AppState, SpitterC( SpitterType_Single, SpitterGroupID_Single_Corner, GetT( AppState->World_Bound, Vec2( 0.9f, 0.2f ) ), Vec2( -1.0f, 0.0f ) ) );
            
            
            AddSpitter( AppState, SpitterC( SpitterType_Single, SpitterGroupID_Single_Middle, GetT( AppState->World_Bound, Vec2( 0.1f, 0.475f ) ), Vec2(  1.0f, 0.0f ) ) );
            AddSpitter( AppState, SpitterC( SpitterType_Single, SpitterGroupID_Single_Middle, GetT( AppState->World_Bound, Vec2( 0.9f, 0.525f ) ), Vec2( -1.0f, 0.0f ) ) );
            
            
            AddSpitter( AppState, SpitterC( SpitterType_Short, SpitterGroupID_Vertical_Corner, GetT( AppState->World_Bound, Vec2( 0.2f, 0.1f ) ), Vec2(  0.0f,  1.0f ) ) );
            AddSpitter( AppState, SpitterC( SpitterType_Short, SpitterGroupID_Vertical_Corner, GetT( AppState->World_Bound, Vec2( 0.8f, 0.9f ) ), Vec2(  0.0f, -1.0f ) ) );
            AddSpitter( AppState, SpitterC( SpitterType_Short, SpitterGroupID_Vertical_Middle, GetT( AppState->World_Bound, Vec2( 0.4f, 0.9f ) ), Vec2(  0.0f, -1.0f ) ) );
            AddSpitter( AppState, SpitterC( SpitterType_Short, SpitterGroupID_Vertical_Middle, GetT( AppState->World_Bound, Vec2( 0.6f, 0.1f ) ), Vec2(  0.0f,  1.0f ) ) );
            
            flo32 StartLevel_Delay = 5.0f;
            // This value syncs these three things:
            //  1) A Damage Zone activates.
            //  2) The Spit Bonus advances to the next type.
            //  3) The Spitters completely spawn in.
            // The value is equal to the Delay Wave at the start of the level, plus the SPITTER_ACTIVE_TARGET_TIME. NOTE: This means that the SPITTER_ACTIVE_TARGET_TIME should be rounded to a whole second value, because the Wave Editor cannot do sub-second Wave times.
            
            { // DAMAGE ZONES
                int32 nDamageZone = 4;
                vec2 Pos[ 4 ] ={
                    GetT( AppState->World_Bound, Vec2( 0.4f,  0.1f ) ),
                    GetT( AppState->World_Bound, Vec2( 0.1f, 0.65f ) ),
                    GetT( AppState->World_Bound, Vec2( 0.6f,  0.9f ) ),
                    GetT( AppState->World_Bound, Vec2( 0.9f, 0.35f ) ),
                };
                
                flo32 Radians[ 4 ] = {
                    0.0f, PI * 0.5f, 0.0f, PI * 0.5f,
                };
                
                int32 StartingDamageZone = RandomS32InRange( 0, nDamageZone - 1 );
                for( int32 iDamageZone = 0; iDamageZone < 4; iDamageZone++ ) {
                    int32 Index = ( StartingDamageZone + iDamageZone ) % nDamageZone;
                    
                    AddDamageZone( AppState, DamageZoneC( ORectCD( Pos[ Index ], DAMAGE_ZONE_DIM, Radians[ Index ] ) ) );
                }
                
                // NOTE : I'm moving the Damage Zone Activation Time up by 3.0 seconds, so that this is the first thing the Player sees when starting the level and gives them a little time to process.
                for( int32 iDamageZone = 0; iDamageZone < SpitterS->nDamageZone; iDamageZone++ ) {
                    DAMAGE_ZONE * DamageZone = SpitterS->DamageZone + iDamageZone;
                    DamageZone->Timer = -( StartLevel_Delay - 3.0f + DAMAGE_ZONE_ACTIVE_TARGET_TIME * ( flo32 )( iDamageZone ) );
                    
                    DamageZone->IsEnabled = true;
                }
            }
            
            SpitterS->SpitBonus_Timer = SPITTER_SPIT_BONUS_CYCLE_TARGET_TIME - StartLevel_Delay;
        } break;
        
        case LevelID_ObservationDeck: {
            PLAYER_STATE * Player = &AppState->Player;
            Player->ZeroPoint_IsEnabled = true;
            
            {
                BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
                BoardRoomS->State_IsActive           = true;
                BoardRoomS->Spawn_HitObservationDeck = true;
                
                {
                    flo32 Dist  = SECURITY_DRONE_SPAWN_SPEED * 3.5f;
                    flo32 Y     = AppState->World_Bound.Bottom - Dist;
                    BoardRoomS->SecurityDrone = SecurityDroneC( Vec2( 0.0f, Y ), Vec2( 0.0f, -14.0f ) );
                }
                
                OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                ObservationDeck->IsPresent = true;
                ObservationDeck->Health   = OBSERVATION_DECK_HEALTH;
                ObservationDeck->Radius   = OBSERVATION_DECK_RADIUS;
                flo32 Y = AppState->World_Bound.Top - 8.0f + OBSERVATION_DECK_RADIUS;
                ObservationDeck->Position = Vec2( 0.0f, Y );
                
                ObservationDeck->Shield_IsActive = true;
                ObservationDeck->Shield_MaxDist  = OBSERVATION_DECK_SHIELD_INIT_DIST;
                ObservationDeck->Shield_Dist     = ObservationDeck->Shield_MaxDist;
                
                // NOTE: These values will appear to give the barrier a little delay before each action.
                ObservationDeck->Barrier_Dist        = -( ( OBSERVATION_DECK_BARRIER_DIST / 2.5f ) * ( 140.0f / 60.0f ) + OBSERVATION_DECK_BARRIER_WIDTH );
                ObservationDeck->Barrier_RotateSpeed = -( OBSERVATION_DECK_BARRIER_ROTATE_SPEED ) * 0.5f;
                ObservationDeck->Barrier_Radians     = 0.0f;
                
                for( int32 Iter = 0; Iter < SPAWN_LAYOUT_PATTERN_MAX_COUNT; Iter++ ) {
                    BoardRoomS->SpawnLayout_Index[ Iter ] = Iter;
                }
                BoardRoomS->iSpawnLayout_Index = SPAWN_LAYOUT_PATTERN_MAX_COUNT;
                
                {
                    vec2 Room_Dim = Vec2( OBSERVATION_DECK_RADIUS * 2.0f, OBSERVATION_DECK_RADIUS * 0.5f ) * OBSERVATION_DECK_WINDOW_TEXTURE_SCALE;
                    rect Room_Bound = RectTD( ObservationDeck->Position + Vec2( 0.0f, -OBSERVATION_DECK_RADIUS ), Vec2( 0.5f, 0.0f ), Room_Dim );
                    
                    Room_Dim *= 0.5f;
                    Room_Bound = RectCD( GetCenter( Room_Bound ), Room_Dim );
                    Room_Bound = AddDim( Room_Bound, Room_Dim.x * 0.15f, Room_Dim.y * 0.25f, Room_Dim.x * 0.15f, Room_Dim.y * 0.15f );
                    Room_Dim = GetDim( Room_Bound );
                    
                    vec2  Table_Center = GetT( Room_Bound, Vec2( 0.5f, 0.45f ) );
                    vec2  Table_Radius = Vec2( 0.25f, 0.15f ) * Room_Dim;
                    flo32 Table_yScale = Table_Radius.y / Table_Radius.x;
                    
                    flo32 bRadians = RandomF32() * TAU;
                    flo32 RadiansPerSegment = TAU / ( flo32 )BOARD_MEMBER_MAX_COUNT;
                    
                    BoardRoomS->nBoardMember = BOARD_MEMBER_MAX_COUNT;
                    for( int32 iBoardMember = 0; iBoardMember < BoardRoomS->nBoardMember; iBoardMember++ ) {
                        flo32 Radians = bRadians + RandomF32InRange( -0.35f, 0.35f ) * RadiansPerSegment;
                        vec2  V       = ToDirection2D( Radians ) * ( Table_Radius.x * RandomF32InRange( 1.0f, 1.2f ) );
                        V.y *= Table_yScale;
                        
                        vec2 Pos = Table_Center + V;
                        
                        Pos.y += ( 0.2f + 0.4f ); // half torso + leg length
                        BoardRoomS->BoardMember[ iBoardMember ] = BoardMemberC( Pos );
                        
                        bRadians += RadiansPerSegment;
                    }
                }
                
                //AddMissileLauncher( AppState, MissileLauncherC( -1, GetT( AppState->World_Bound, Vec2( 0.15f, 0.9f ) ), -PI * 0.5f ) );
                //AddMissileLauncher( AppState, MissileLauncherC( -1, GetT( AppState->World_Bound, Vec2( 0.85f, 0.9f ) ), -PI * 0.5f ) );
                
                {
                    flo32 RadiansA = ( TAU * 0.75f ) + ( OBSERVATION_DECK_BARRIER_RADIANS + ( TAU / 64.0f ) ) * 0.5f;
                    flo32 RadiansB = ( TAU * 0.75f ) - ( OBSERVATION_DECK_BARRIER_RADIANS + ( TAU / 64.0f ) ) * 0.5f;
                    
                    flo32 RadiusA  = ObservationDeck->Radius + 14.0f;
                    flo32 RadiusB  = ObservationDeck->Radius +  6.0f;
                    
                    vec2  bPos = ObservationDeck->Position + Vec2( 0.0f, -16.0f );
                    vec2  PA   = ObservationDeck->Position + ToDirection2D( RadiansA ) * RadiusA;
                    vec2  PB   = ObservationDeck->Position + ToDirection2D( RadiansA ) * RadiusB;
                    vec2  QA   = ObservationDeck->Position + ToDirection2D( RadiansB ) * RadiusA;
                    vec2  QB   = ObservationDeck->Position + ToDirection2D( RadiansB ) * RadiusB;
                    
                    ObservationDeck->MissileLauncher_Pos[ 0 ][ 0 ] = PA;
                    ObservationDeck->MissileLauncher_Pos[ 0 ][ 1 ] = PB;
                    
                    ObservationDeck->MissileLauncher_Pos[ 1 ][ 0 ] = QA;
                    ObservationDeck->MissileLauncher_Pos[ 1 ][ 1 ] = QB;
                    
                    ObservationDeck->MissileLauncher_Radians[ 0 ] = RadiansA;
                    ObservationDeck->MissileLauncher_Radians[ 1 ] = RadiansB;
                    
                    ObservationDeck->MissileLauncher[ 0 ] = MissileLauncherC( -1, PA, -PI * 0.5f );
                    ObservationDeck->MissileLauncher[ 1 ] = MissileLauncherC( -1, QA, -PI * 0.5f );
                    
                    ObservationDeck->MissileLauncher[ 0 ].Launch_Timer = OBSERVATION_DECK_MISSILE_LAUNCHER_TARGET_TIME * 0.5f;
                }
                
                {
                    vec2  InitPos     = ObservationDeck->Position + Vec2( 0.0f, -OBSERVATION_DECK_RADIUS * 0.95f );
                    flo32 InitRadians = RandomF32() * TAU;
                    flo32 Range       = PI * 1.1f;
                    flo32 MaxLevel    = 1.0f;
                    
                    vec2 Window_Dim = Vec2( OBSERVATION_DECK_RADIUS * 2.0f, OBSERVATION_DECK_RADIUS * 0.5f ) * OBSERVATION_DECK_WINDOW_TEXTURE_SCALE;
                    rect Window_Bound = RectTD( ObservationDeck->Position + Vec2( 0.0f, -OBSERVATION_DECK_RADIUS ), Vec2( 0.5f, 0.0f ), Window_Dim );
                    
                    Window_Dim *= 0.5f;
                    Window_Bound = RectCD( GetCenter( Window_Bound ), Window_Dim );
                    Window_Bound = AddDim( Window_Bound, Window_Dim.x * 0.1f, 0.0f, Window_Dim.x * 0.1f, Window_Dim.y * 0.15f );
                    
#define WINDOW_CRACK_BRANCH_MAX_COUNT  ( 128 )
                    int32 nBranch = 0;
                    WINDOW_CRACK_BRANCH BranchQueue[ WINDOW_CRACK_BRANCH_MAX_COUNT ] = {};
                    BranchQueue[ nBranch++ ] = { InitPos, InitRadians, InitRadians, MaxLevel };  InitRadians += ( PI * 0.5f + RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.2f ) );
                    BranchQueue[ nBranch++ ] = { InitPos, InitRadians, InitRadians, MaxLevel };  InitRadians += ( PI * 0.5f + RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.2f ) );
                    BranchQueue[ nBranch++ ] = { InitPos, InitRadians, InitRadians, MaxLevel };  InitRadians += ( PI * 0.5f + RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.2f ) );
                    BranchQueue[ nBranch++ ] = { InitPos, InitRadians, InitRadians, MaxLevel };  InitRadians += ( PI * 0.5f + RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.2f ) );
                    
                    
                    while( nBranch > 0 ) {
                        WINDOW_CRACK_BRANCH Branch = BranchQueue[ 0 ];
                        for( int32 iBranch = 0; iBranch < ( nBranch - 1 ); iBranch++ ) {
                            BranchQueue[ iBranch ] = BranchQueue[ iBranch + 1 ];
                        }
                        nBranch -= 1;
                        
                        vec2  LastPos     = Branch.AtPos;
                        flo32 LastRadians = Branch.AtRadians;
                        flo32 bRadians    = Branch.bRadians;
                        flo32 AtLevel     = Branch.AtLevel;
                        
                        int32 nSegments = RandomS32InRange( 2, 5 );
                        nSegments = MinValue( nSegments, WINDOW_CRACK_SEGMENT_MAX_COUNT - ObservationDeck->WindowCrack_nSegments );
                        
                        boo32 DoNewBranch = true;
                        flo32 MinRadians  = bRadians - Range * 0.5f;
                        flo32 MaxRadians  = bRadians + Range * 0.5f;
                        
                        for( int32 iSegment = 0; iSegment < nSegments; iSegment++ ) {
                            flo32 Length  = RandomF32InRange( 0.8f, 1.2f ) * 0.5f;
                            flo32 Radians = LastRadians + RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.1f ); 
                            vec2  Pos     = LastPos + ToDirection2D( Radians ) * Length;
                            
                            if( ( Radians >= MinRadians ) && ( Radians <= MaxRadians ) && ( IsInRect( Pos, Window_Bound ) ) ) {
                                WINDOW_CRACK_SEGMENT WindowCrack = {};
                                WindowCrack.P      = LastPos;
                                WindowCrack.Q      = Pos;
                                WindowCrack.tLevel = AtLevel;
                                ObservationDeck->WindowCrack[ ObservationDeck->WindowCrack_nSegments++ ] = WindowCrack;
                                Assert( ObservationDeck->WindowCrack_nSegments <= WINDOW_CRACK_SEGMENT_MAX_COUNT );
                                
                                LastPos     = Pos;
                                LastRadians = Radians;
                                
                                MaxLevel = MaxValue( AtLevel, MaxLevel );
                                
                                AtLevel += 1.0f;
                            } else {
                                nSegments   = iSegment;
                                DoNewBranch = false;
                            }
                        }
                        
                        if( DoNewBranch ) {
                            int32 nNewBranch = RandomS32InRange( 2, 3 );
                            for( int32 iNewBranch = 0; iNewBranch < nNewBranch; iNewBranch++ ) {
                                WINDOW_CRACK_BRANCH NewBranch = {};
                                NewBranch.AtPos     = LastPos;
                                NewBranch.AtRadians = LastRadians + RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.75f );
                                NewBranch.bRadians  = bRadians;
                                NewBranch.AtLevel   = AtLevel + 1.0f;
                                
                                BranchQueue[ nBranch++ ] = NewBranch;
                                Assert( nBranch < WINDOW_CRACK_BRANCH_MAX_COUNT );
                            }
                        }
                        
                        if( ObservationDeck->WindowCrack_nSegments >= WINDOW_CRACK_SEGMENT_MAX_COUNT ) {
                            nBranch = 0;
                        }
                    }
                    
                    ObservationDeck->WindowCrack_MaxSegments = ObservationDeck->WindowCrack_nSegments;
                    ObservationDeck->WindowCrack_nSegments   = 0;
                    
                    for( int32 iSegment = 0; iSegment < ObservationDeck->WindowCrack_MaxSegments; iSegment++ ) {
                        WINDOW_CRACK_SEGMENT * Segment = ObservationDeck->WindowCrack + iSegment;
                        Segment->tLevel = 1.0f - Clamp01( Segment->tLevel / MaxLevel );
                    }
                }
                
                // NOTE : TEMP!
                //ObservationDeck->Shield_IsActive  = false;
                //ObservationDeck->Barrier_IsActive = true;
                //ObservationDeck->Barrier_Dist     = OBSERVATION_DECK_BARRIER_DIST;
                //ObservationDeck->Health          = 0;
                //ScoreS->Level_DisplayTimer    = 15.0f;
            } break;
        }
    }
    
    
    Clock = OutputClockElapsedToDebugConsole( Clock, __rdtsc(), "Init Level data." );
    
    InitPowerups( AppState, Level_TargetTime );
    InitScoreForLevel ( AppState );
    InitPlayerForLevel( AppState );
    InitCameraForLevel( AppState );
    
    Clock = OutputClockElapsedToDebugConsole( Clock, __rdtsc(), "Init other data." );
    
    WAVE_STATE * WaveS = &AppState->WaveS;
    AddWaveToActive( AppState, WaveS->StartingWave );
    
    AppState->InGameMenu_ShowMenu    = false;
    AppState->InGameMenu_ShowConfirm = false;
}