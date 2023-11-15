
//----------
// ENTITY
//----------

internal void
AddEntity( EDITOR_STATE * Editor, ENTITY Entity ) {
    EDITOR__WAVE_STATE * WaveS       = &Editor->WaveS;
    WAVE               * CurrentWave = WaveS->Wave + WaveS->CurrentWave;
    
    if( CurrentWave->nEntity < WAVE_ENTITY_MAX_COUNT ) {
        CurrentWave->Entity[ CurrentWave->nEntity++ ] = Entity;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add ENTITY to the current wave: %u", WAVE_ENTITY_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

//----------
// WAVE
//----------

internal int32
CountTotalEntitiesOnScreen( APP_STATE * AppState ) {
    int32 nEntities = 0;
    
    nEntities += AppState->TurretS.nTurret;
    nEntities += AppState->ChaserS.nChaser;
    nEntities += AppState->SwarmS.nSwarm;
    nEntities += AppState->DrifterS.nDrifter;
    nEntities += AppState->LooperS.nLooper;
    //nEntities += AppState->StarburstS.nStarburst;
    //nEntities += AppState->StarburstS.nStar;
    nEntities += AppState->BayDoorS.nPersonalPod;
    nEntities += AppState->BayDoorS.nTransport;
    //nEntities += AppState->BayDoorS.nMiniBoss;
    nEntities += AppState->BayDoorS.nYacht;
    nEntities += AppState->MinerS.nMiner;
    //nEntities += AppState->BombS.nBomber;
    
    return nEntities;
}

internal uint32
CountTotalEntitiesInWave( WAVE * Wave ) {
    uint32 nTotalEntitiesInWave = 0;
    
    switch( Wave->Type ) {
        case WaveType_SpawnEntities: {
            for( uint32 iEntity = 0; iEntity < Wave->nEntity; iEntity++ ) {
                ENTITY * Entity = Wave->Entity + iEntity;
                nTotalEntitiesInWave += CountTotalEntitiesInEntity( Entity->Type, Entity->nEntity );
            }
        } break;
        
        case WaveType_OpenBayDoors: {
            BAY_DOOR_EVENT Event = Wave->BayDoorEvent;
            if( Event.Type == BayDoorEventType_Transport ) {
                nTotalEntitiesInWave += CountTotalEntitiesInEntity( EntityType_Transport );
            }
            if( Event.Type == BayDoorEventType_MiniBoss ) {
                nTotalEntitiesInWave += CountTotalEntitiesInEntity( EntityType_MiniBoss );
            }
        } break;
    }
    
    return nTotalEntitiesInWave;
}

internal WAVE * 
Editor_NewWave( EDITOR_STATE * Editor ) {
    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
    WAVE               * Wave  = 0;
    if( WaveS->nWave < WAVE_MAX_COUNT ) {
        WAVE PrevWave = WaveS->Wave[ WaveS->CurrentWave ];
        
        WaveS->CurrentWave = WaveS->nWave++;
        
        Wave = WaveS->Wave + WaveS->CurrentWave;
        
        ( *Wave ) = PrevWave;
        Wave->Guid      = WaveS->CurrentGuid++;
        Wave->nNextWave = 0;
        Wave->nKillWave = 0;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add WAVE to WAVE_STATE: %u", WAVE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
    
    return Wave;
}

internal void
InitWaveEditor( EDITOR_STATE * Editor ) {
    { // End Level
        WAVE * Wave = Editor_NewWave( Editor );
        Wave->Type = WaveType_EndLevel;
        Wave->Exit = ExitType_NoExit;
    }
    
    { // Blank Wave
        WAVE * Wave = Editor_NewWave( Editor );
        Wave->Type                    = WaveType_SpawnEntities;
        Wave->Exit                    = ExitType_TargetTime;
        Wave->Exit_TargetTime         = 10.0f;
    }
    
    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
    WaveS->CurrentWave = 1;
}

internal void
AddWave( APP_STATE * AppState, WAVE Wave ) {
    WAVE_STATE * WaveS = &AppState->WaveS;
    if( WaveS->nWave < WAVE_MAX_COUNT ) {
        WaveS->Wave[ WaveS->nWave++ ] = Wave;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add WAVE to WAVE_STATE: %u", WAVE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
AddWaveToActive( APP_STATE * AppState, int32 iWave ) {
    WAVE_STATE * WaveS = &AppState->WaveS;
    if( WaveS->nActiveWave < ACTIVE_WAVE_MAX_COUNT ) {
        
        WaveS->ActiveWave              [ WaveS->nActiveWave ] = WaveS->Wave[ iWave ];
        WaveS->ActiveWave_IsInitialized[ WaveS->nActiveWave ] = false;
        WaveS->nActiveWave++;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add WAVE to WAVE_STATE: %u", ACTIVE_WAVE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
InitCameraForLevel( APP_STATE * AppState ) {
    rect View_Bound = AppState->UI_Bound;
    
#if ENABLE_DEBUG_REPLAY
    PANEL * Panel = &AppState->Debug;
    if( Panel->Show ) {
        View_Bound.Right = Panel->Bound.Left;
    }
#endif
    
    ZoomCameraToWorldBound( &AppState->Camera, AppState->UI_Bound, View_Bound, AppState->World_Bound );
    
    { // Adjust Camera for UI
        CAMERA_STATE * Camera = &AppState->Camera;
        flo32 t     = UI_PANEL_HEIGHT / AppState->UI_Dim.y;
        flo32 ToAdd = t * GetHeight( AppState->World_Bound );
        
        Camera->Pos.y      += ToAdd * 0.5f;
        Camera->Dim.y      += ToAdd;
        Camera->HalfDim     = Camera->Dim * 0.5f;
        Camera->Scale_Ratio = Camera->Dim.y / WORLD_BOUND_BASE_Y_UNITS_FOR_TEXT_SCALE;
    }
    
    { // Adjust Camera for Camera Shake
        CAMERA_STATE * Camera      = &AppState->Camera;
        CAMERA_SHAKE * CameraShake = &AppState->CameraShake;
        
        vec2 tPos = lerp( CameraShake->P, CameraShake->t, CameraShake->Q );
        Camera->Pos += CameraShake->Offset * tPos * CameraShake->Strength;
    }
}

internal void
ClearActiveWaveArrayToEndLevel( APP_STATE * AppState ) {
    WAVE_STATE * WaveS = &AppState->WaveS;
    
    WAVE Wave = {};
    Wave.Type = WaveType_EndLevel;
    Wave.Exit = ExitType_NoExit;
    
    WaveS->nActiveWave                   = 1;
    WaveS->ActiveWave_IsInitialized[ 0 ] = false;
    WaveS->ActiveWave[ 0 ]               = Wave;
}

internal void
UpdateWave( APP_STATE * AppState, flo32 dT ) {
    WAVE_STATE * WaveS = &AppState->WaveS;
    
    for( int32 iActiveWave = 0; iActiveWave < WaveS->nActiveWave; iActiveWave++ ) {
        WAVE * ActiveWave = WaveS->ActiveWave + iActiveWave;
        
        if( !WaveS->ActiveWave_IsInitialized[ iActiveWave ] ) {
            ActiveWave->InDelay_Timer += dT;
            if( ActiveWave->InDelay_Timer >= ActiveWave->InDelay_TargetTime ) {
                WaveS->ActiveWave_IsInitialized[ iActiveWave ] = true;
                
                switch( ActiveWave->Type ) {
                    case WaveType_EndLevel: {
                        END_LEVEL * EndLevel = &AppState->EndLevel;
                        EndLevel->Show              = true;
                        
                        PLAYER_STATE * Player = &AppState->Player;
                        Player->ZeroPoint_ShowUI   = false;
                        Player->ZeroPoint_IsActive = false;
                        Player->ZeroPoint_DrawBeam = false;
                        EndLevel->ShowLevelComplete = ( Player->Health > 0 );
                        
                        KillAllEnemies( AppState, true );
                        
                        WaveS->nActiveWave = 1;
                        WaveS->ActiveWave_IsInitialized[ 0 ] = true;
                        WaveS->ActiveWave[ 0 ]               = *( ActiveWave );
                    } break;
                    
                    case WaveType_SpawnEntities: {
                        for( uint32 iEntity = 0; iEntity < ActiveWave->nEntity; iEntity++ ) {
                            ENTITY Entity = ActiveWave->Entity[ iEntity ];
                            
#if 0                            
                            MARKET_STATE_SAVE_DATA * MarketS = &AppState->MarketS.SaveData;
                            PRODUCT_ENTRY  Product = MarketS->Product[ Entity.Type ];
                            if( Product.Flags & PRODUCT_ENTRY_FLAG__IS_AVAILABLE ) {
                                Entity.nEntity *= ( int32 )Product.Percent;
                            }
#endif
                            
                            if( ( Entity.nEntity > 1 ) && ( Entity.Type != EntityType_Miner ) && ( Entity.Type != EntityType_Bomber ) ) {
                                AddEntityGroupSpawn( AppState, ToEntityGroupSpawn( Entity ) );
                            } else {
                                AddEntitySpawn( AppState, Entity );
                            }
                        }
                    } break;
                    
                    case WaveType_OpenBayDoors: {
                        BAY_DOOR_EVENT Event = ActiveWave->BayDoorEvent;
                        
                        Assert( Event.iSrc  < BAY_DOOR_SPAWN_MAX_COUNT      );
                        Assert( Event.iDest < BAY_DOOR_SPAWN_DEST_MAX_COUNT );
                        
                        BAY_DOOR_STATE * BayDoorS     = &AppState->BayDoorS;
                        BAY_DOOR_SPAWN * BayDoorSpawn = BayDoorS->BayDoorSpawn + Event.iSrc;
                        Assert( ( BayDoorSpawn->Stage == BayDoorEventStage_Wait ) || ( BayDoorSpawn->Stage == BayDoorEventStage_Complete ) );
                        
                        BAY_DOOR_SPAWN Spawn = {};
                        Spawn.WaveGuid   = ActiveWave->Guid;
                        Spawn.Type       = Event.Type;
                        Spawn.Stage      = BayDoorEventStage_OpenBayDoors;
                        Spawn.TargetTime = Event.TargetTime;
                        Spawn.Src        = BayDoorSpawn->Src;
                        Spawn.Dest       = BayDoorS->BayDoorSpawn_Dest[ Event.iDest ];
                        
                        ( *BayDoorSpawn ) = Spawn;
                    } break;
                    
                    case WaveType_SetAsteroidSettings: {
                        ASTEROID_SETTINGS_EVENT Event = ActiveWave->AsteroidSettingsEvent;
                        
                        ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
                        AsteroidS->Spawn_Timer       = 0.0f;
                        AsteroidS->Spawn_TargetTime  = ( flo32 )Event.Spawn_nFrames / 60.0f;
                        
                        AsteroidS->Spawn_MaxAsteroid = Event.Spawn_MaxAsteroid;
                        
                        int32 Denom = 0;
                        
                        for( uint32 iType = 0; iType < AsteroidType_Count; iType++ ) {
                            for( uint32 iClass = 0; iClass < AsteroidClass_Count; iClass++ ) {
                                int32 Numer = Event.Numer_TypeClass[ iType ][ iClass ];
                                AsteroidS->Numer_TypeClass[ iType ][ iClass ] = Numer;
                                
                                Denom += Numer;
                            }
                        }
                        
                        for( int32 iSide = 0; iSide < 4; iSide++ ) {
                            AsteroidS->Spawn_SideIsEnabled[ iSide ] = Event.Spawn_SideIsEnabled[ iSide ];
                        }
                        AsteroidS->Spawn_Bound = Event.Spawn_Bound;
                        
                        if( Denom > 0 ) {
                            AsteroidS->Spawn_IsActive = true;
                        } else {
                            AsteroidS->Spawn_IsActive = false;
                        }
                    } break;
                    
                    case WaveType_SetExplosiveSettings: {
                        EXPLOSIVE_SETTINGS_EVENT Event = ActiveWave->ExplosiveSettingsEvent;
                        
#if 0                        
                        EXPLOSIVE_STATE * ExplosiveS = &AppState->ExplosiveS;
                        ExplosiveS->Spawn_Timer      = 0.0f;
                        ExplosiveS->Spawn_TargetTime = ( flo32 )Event.Spawn_nFrames / 60.0f;
                        
                        if( Event.Spawn_nFrames > 0 ) {
                            ExplosiveS->Spawn_IsActive = true;
                        } else {
                            ExplosiveS->Spawn_IsActive = false;
                        }
#endif
                    } break;
                    
                    case WaveType_SetSwarmSettings: {
                        SWARM_SETTINGS_EVENT Event = ActiveWave->SwarmSettingsEvent;
                        
                        SWARM_STATE          * SwarmS = &AppState->SwarmS;
                        SWARM_SPAWN_SETTINGS * Spawn  = &SwarmS->Spawn;
                        
                        Spawn->WaveGuid    = ActiveWave->Guid;
                        Spawn->IsActive    = Event.IsActive;
                        Spawn->Style       = Event.Style;
                        Spawn->nMaxSpawn   = Event.nMaxSpawn;
                        Spawn->nRespawnAt  = Event.nRespawnAt;
                        Spawn->InnerRadius = Event.InnerRadius;
                        Spawn->OuterRadius = Event.OuterRadius;
                        Spawn->Delay_TargetTime = ( ( flo32 )Event.Delay_TargetTime_nFrames / 60.0f );
                    } break;
                    
                    case WaveType_SetTurretSettings: {
                        TURRET_SETTINGS_EVENT Event = ActiveWave->TurretSettingsEvent;
                        
                        TURRET_STATE * TurretS = &AppState->TurretS;
                        TurretS->Spawn_Cooldown_TargetTime = ( flo32 )Event.Spawn_nFrames / 60.0f;
                        
                        TurretS->Spawn_MaxTurret = Event.Spawn_MaxTurret;
                        
                        int32 Denom = 0;
                        
                        for( uint32 iType = 0; iType < TurretSettings_Speed_Count; iType++ ) {
                            for( uint32 iClass = 0; iClass < TurretSettings_Radians_Count; iClass++ ) {
                                int32 Numer = Event.Numer_SpeedRadians[ iType ][ iClass ];
                                TurretS->Numer_SpeedRadians[ iType ][ iClass ] = Numer;
                                
                                Denom += Numer;
                            }
                        }
                        
                        if( Denom > 0 ) {
                            TurretS->Spawn_IsActive = true;
                        } else {
                            TurretS->Spawn_IsActive = false;
                        }
                    } break;
                    
                    case WaveType_EnableBombLauncher: {
                        BOMB_LAUNCHER_EVENT Event = ActiveWave->BombLauncherEvent;
                        
                        Assert( Event.iSrc < BOMB_LAUNCHER_MAX_COUNT );
                        
                        BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
                        BombLauncherS->BonusObjective_MaxBombLauncherToSpawn = MinValue( BombLauncherS->BonusObjective_MaxBombLauncherToSpawn + 1, BOMB_LAUNCHER_MAX_COUNT );
                    } break;
                    
                    case WaveType_EnableMegaTurret: {
                        TURRET_STATE * TurretS = &AppState->TurretS;
                        TurretS->SpawnMegaTurret_IsActive = true;
                        TurretS->SpawnMegaTurret_Health   = MEGA_TURRET_HEALTH;
                        TurretS->SpawnMegaTurret_Timer    = MEGA_TURRET_SPAWN_TARGET_TIME;
                    } break;
                    
                    case WaveType_EnableBlimp: {
                        rect MoveDest_Bound = AddRadius( AppState->World_Bound, -0.5f );
                        vec2 Dim = Vec2( 5.0f, 5.0f );
                        
                        BLIMP_STATE * BlimpS = &AppState->BlimpS;
                        BlimpS->MoveDest[ 0 ] = RectCD( GetBL( MoveDest_Bound ), Dim );
                        BlimpS->MoveDest[ 1 ] = RectCD( GetTL( MoveDest_Bound ), Dim );
                        BlimpS->MoveDest[ 2 ] = RectCD( GetTR( MoveDest_Bound ), Dim );
                        BlimpS->MoveDest[ 3 ] = RectCD( GetBR( MoveDest_Bound ), Dim );
                        BlimpS->nMoveDest = 4;
                        
                        int32 Move_iDest = RandomS32InRange( 0, BlimpS->nMoveDest - 1 );
                        
                        vec2 StartPos[ 4 ] = {};
                        StartPos[ 0 ] = GetBL( AppState->World_Bound ) + Vec2( -8.0f, -8.0f );
                        StartPos[ 1 ] = GetTL( AppState->World_Bound ) + Vec2( -8.0f,  8.0f );
                        StartPos[ 2 ] = GetTR( AppState->World_Bound ) + Vec2(  8.0f,  8.0f );
                        StartPos[ 3 ] = GetBR( AppState->World_Bound ) + Vec2(  8.0f, -8.0f );
                        
                        AddBlimp( AppState, BlimpC( -1, Move_iDest, StartPos[ Move_iDest ] ) );
                    } break;
                    
                    case WaveType_EnableMegaMiner: {
                        AddMegaMiner( AppState, MegaMinerC( -1, GetBC( AppState->World_Bound ) + Vec2( 0.0f, -6.0f ), PI * 0.5f ) );
                    } break;
                    
                    case WaveType_SetWormSettings: {
                        WORM_SETTINGS_EVENT Event = ActiveWave->WormSettingsEvent;
                        
                        WORM_STATE * WormS = &AppState->WormS;
                        WormS->Spawn_Cooldown_TargetTime = ( flo32 )Event.Spawn_nFrames / 60.0f;
                        WormS->Spawn_MaxWorm             = Event.Spawn_MaxWorm;
                        
                        if( WormS->Spawn_MaxWorm > 0 ) {
                            WormS->Spawn_IsActive = true;
                        } else {
                            WormS->Spawn_IsActive = false;
                        }
                    } break;
                    
                    case WaveType_EnableLoader: {
                        rect  Bound   = AddRadius( AppState->World_Bound, 6.0f );
                        vec2  Pos     = GetRandomPointOnPerimeter( Bound );
                        flo32 Radians = ToRadians( Pos - GetCenter( AppState->World_Bound ) );
                        AddLoader( AppState, LoaderC( Pos, Radians ) );
                    } break;
                    
                    case WaveType_SetSpitterSettings: {
                        SPITTER_SETTINGS_EVENT Event = ActiveWave->SpitterSettingsEvent;
                        
                        int32 nActive          = 0;
                        int32 Spiral_nSpitters = 0;
                        
                        SPITTER_STATE * SpitterS = &AppState->SpitterS;
                        for( int32 iSpitter = 0; iSpitter < SpitterS->nSpitter; iSpitter++ ) {
                            SPITTER * Spitter = SpitterS->Spitter + iSpitter;
                            Spitter->IsActive = Event.Group_IsActive[ Spitter->GroupID ];
                            
                            if( Spitter->IsActive ) {
                                nActive++;
                            }
                            if( ( Spitter->GroupID == SpitterGroupID_Spiral ) && ( Spitter->IsActive ) ) {
                                Spiral_nSpitters++;
                            }
                        }
                        
                        SpitterS->Spiral_nSpitters = Spiral_nSpitters;
                        SpitterS->Barrel_IsActive  = ( nActive > 0 );
                        SpitterS->Spiral_nToFire   = Event.nSpiralSpittersToFire;
                    } break;
                    
                    case WaveType_EnableLevelTimer: {
                        SCORE_STATE * ScoreS = &AppState->ScoreS;
                        ScoreS->Level_DisplayTimer_DoUpdate = true;
                    } break;
                    
                    default: {
                        InvalidCodePath;
                    } break;
                }
            }
        }
    }
    
    for( int32 iActiveWave = 0; iActiveWave < WaveS->nActiveWave; iActiveWave++ ) {
        if( WaveS->ActiveWave_IsInitialized[ iActiveWave ] ) {
            WAVE * ActiveWave = WaveS->ActiveWave + iActiveWave;
            ActiveWave->Exit_Timer += dT;
            
            if( ActiveWave->DoOutDelay ) {
                ActiveWave->OutDelay_Timer += dT;
            }
            
            //DISPLAY_VALUE( int32, ActiveWave->nKills );
            //DISPLAY_VALUE( int32, ActiveWave->nTotalEntitiesInWave );
        }
    }
    //DISPLAY_VALUE( uint32, WaveS->nActiveWave );
    
    for( uint32 iEntityGroupSpawn = 0; iEntityGroupSpawn < WaveS->nEntityGroupSpawn; iEntityGroupSpawn++ ) {
        ENTITY_GROUP_SPAWN * EntityGroupSpawn = WaveS->EntityGroupSpawn + iEntityGroupSpawn;
        
        EntityGroupSpawn->Timer += dT;
        if( EntityGroupSpawn->Timer >= EntityGroupSpawn->TargetTime ) {
            EntityGroupSpawn->Timer = 0.0f;
            EntityGroupSpawn->Counter--;
            
            ENTITY Entity = GenValIdentity( EntityGroupSpawn->WaveGuid, EntityGroupSpawn->Type, EntityGroupSpawn->Position );
            AddEntitySpawn( AppState, Entity );
        }
    }
}

internal void
FinalizeWave( APP_STATE * AppState, MEMORY * TempMemory ) {
    WAVE_STATE * WaveS = &AppState->WaveS;
    
    { // ENTITY GROUP SPAWN
        uint32 NewCount = 0;
        for( uint32 iEntityGroupSpawn = 0; iEntityGroupSpawn < WaveS->nEntityGroupSpawn; iEntityGroupSpawn++ ) {
            ENTITY_GROUP_SPAWN EntityGroupSpawn = WaveS->EntityGroupSpawn[ iEntityGroupSpawn ];
            
            if( EntityGroupSpawn.Counter > 0 ) {
                WaveS->EntityGroupSpawn[ NewCount++ ] = EntityGroupSpawn;
            }
        }
        WaveS->nEntityGroupSpawn = NewCount;
    }
    
    boo32 * DoRemove = _PushArray_Clear( TempMemory, boo32, ACTIVE_WAVE_MAX_COUNT );
    
    int32 nActiveWave = WaveS->nActiveWave;
    for( int32 iActiveWave = 0; iActiveWave < nActiveWave; iActiveWave++ ) {
        if( WaveS->ActiveWave_IsInitialized[ iActiveWave ] ) {
            WAVE * ActiveWave = WaveS->ActiveWave + iActiveWave;
            
            boo32 DoNextWave = false;
            
            switch( ActiveWave->Exit ) {
                case ExitType_Instant: {
                    DoNextWave = true;
                } break;
                
                case ExitType_TargetTime: {
                    if( ActiveWave->Exit_Timer >= ActiveWave->Exit_TargetTime ) {
                        DoNextWave = true;
                    }
                } break;
                
                case ExitType_NoExit: {
                    // DO NOTHING!
                } break;
                
                case ExitType_KillAllEntitiesInWave: {
                    if( ( ActiveWave->Exit_Timer >= ( ENTITY_SPAWN_TARGET_TIME + 1.0f ) ) && ( ActiveWave->nKills >= ActiveWave->nTotalEntitiesInWave ) ) {
                        DoNextWave = true;
                    }
                } break;
                
                case ExitType_KillAllEntitiesOnScreen: {
                    if( ActiveWave->Exit_Timer >= ( ENTITY_SPAWN_TARGET_TIME + 1.0f ) ) {
                        int32 nEntities = CountTotalEntitiesOnScreen( AppState );
                        
                        if( nEntities <= 0 ) {
                            DoNextWave = true;
                        }
                    }
                } break;
                
                case ExitType_BayDoorEventIsComplete: {
                    BAY_DOOR_EVENT Event = ActiveWave->BayDoorEvent;
                    
                    BAY_DOOR_STATE * BayDoorS     = &AppState->BayDoorS;
                    BAY_DOOR_SPAWN   BayDoorSpawn = BayDoorS->BayDoorSpawn[ Event.iSrc ];
                    
                    DoNextWave = ( BayDoorSpawn.Stage == BayDoorEventStage_Complete );
                } break;
                
                case ExitType_AllBayDoorEventsAreComplete: {
                    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
                    
                    DoNextWave = true;
                    for( int32 iBayDoorSpawn = 0; iBayDoorSpawn < BayDoorS->nBayDoorSpawn; iBayDoorSpawn++ ) {
                        BAY_DOOR_SPAWN BayDoorSpawn = BayDoorS->BayDoorSpawn[ iBayDoorSpawn ];
                        if( ( BayDoorSpawn.Stage != BayDoorEventStage_Wait ) && ( BayDoorSpawn.Stage != BayDoorEventStage_Complete ) ) {
                            Assert( BayDoorSpawn.Stage != BayDoorEventStage_EndLevel );
                            
                            DoNextWave = false;
                        }
                    }
                } break;
                
                case ExitType_BayDoorSpawnIsComplete: {
                    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
                    
                    Assert( BayDoorS->SpawnPersonalPod_IsActive );
                    Assert( BayDoorS->SpawnTransport_IsActive );
                    
                    if( ( BayDoorS->SpawnPersonalPod_iStage >= BAY_DOOR_SPAWN_PERSONAL_POD_SPAWN_COUNT ) && ( BayDoorS->SpawnTransport_iStage >= BAY_DOOR_SPAWN_TRANSPORT_YACHT_SPAWN_COUNT ) ) {
                        DoNextWave = true;
                    }
                } break;
                
                case ExitType_KillAllTurrets: {
                    TURRET_STATE * TurretS = &AppState->TurretS;
                    
                    int32 nEntity = TurretS->nTurret;
                    nEntity += CountEntitySpawnsOfType( AppState, EntityType_Turret );
                    
                    if( nEntity <= 0 ) {
                        DoNextWave = true;
                    }
                } break;
                
                default: {
                    InvalidCodePath;
                } break;
            }
            
            if( DoNextWave ) {
                ActiveWave->DoOutDelay = true;
            }
            
            if( ( DoNextWave ) && ( ActiveWave->OutDelay_Timer >= ActiveWave->OutDelay_TargetTime ) ) {
                if( ActiveWave->nRepeatWave == 0 ) {
                    for( uint32 iNextWave = 0; iNextWave < ActiveWave->nNextWave; iNextWave++ ) {
                        if( ActiveWave->NextWave[ iNextWave ] > -1 ) {
                            AddWaveToActive( AppState, ActiveWave->NextWave[ iNextWave ] );
                        }
                    }
                    
                    DoRemove[ iActiveWave ] = true;
                    
                    for( int32 iKillWave = 0; iKillWave < ActiveWave->nKillWave; iKillWave++ ) {
                        for( int32 iCheckRemove = 0; iCheckRemove < nActiveWave; iCheckRemove++ ) {
                            WAVE * CheckRemove = WaveS->ActiveWave + iCheckRemove;
                            if( CheckRemove->Guid == ActiveWave->KillWave_Guid[ iKillWave ] ) {
                                DoRemove[ iCheckRemove ] = true;
                            }
                        }
                    }
                } else {
                    if( ActiveWave->nRepeatWave > 0 ) {
                        ActiveWave->nRepeatWave--;
                    }
                    
                    ActiveWave->nKills         = 0;
                    ActiveWave->Exit_Timer     = 0.0f;
                    ActiveWave->InDelay_Timer  = 0.0f;
                    ActiveWave->DoOutDelay     = false;
                    ActiveWave->OutDelay_Timer = 0.0f;
                    
                    WaveS->ActiveWave_IsInitialized[ iActiveWave ] = false;
                }
            }
        }
    }
    
    uint32 NewCount = 0;
    for( int32 iActiveWave = 0; iActiveWave < WaveS->nActiveWave; iActiveWave++ ) {
        if( !DoRemove[ iActiveWave ] ) {
            WaveS->ActiveWave              [ NewCount ] = WaveS->ActiveWave              [ iActiveWave ];
            WaveS->ActiveWave_IsInitialized[ NewCount ] = WaveS->ActiveWave_IsInitialized[ iActiveWave ];
            NewCount++;
        }
    }
    WaveS->nActiveWave = NewCount;
    
    _PopArray( TempMemory, boo32, ACTIVE_WAVE_MAX_COUNT );
}

//----------
// LEVEL
//----------

#if 0
internal void
GAME_LoadLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__LEVEL;
    
    WAVE_STATE * WaveS  = &AppState->WaveS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.Contents ) {
        uint8 * ptr = ( uint8 * )File.Contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        uint32 CurrentGuid  = _read( ptr, int32 );
        WaveS->StartingWave = _read( ptr, uint32 );
        
        LEVEL_SETTINGS Level_Settings = _read( ptr, LEVEL_SETTINGS );
        
        flo32  World_YUnits   = Level_Settings.World_YUnits;
        AppState->World_Bound = GetWorldBound( AppState->App_Dim, World_YUnits );
        
        WaveS->nWave = ( int32 )_read( ptr, uint32 );
        for( int32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WAVE Src = ReadWave( Version, &ptr );
            WaveS->Wave[ iWave ] = Src;
        }
        
        strcpy( AppState->CurrentLevel.Level.FileName, FileName );
        
        _PopSize( TempMemory, File.Size );
    }
}
#endif