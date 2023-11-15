
internal PERSONAL_POD
PersonalPodC( BAY_DOOR_SPAWN * BayDoorSpawn, flo32 InitSpeed ) {
    PERSONAL_POD Result = {};
    Result.WaveGuid   = BayDoorSpawn->WaveGuid;
    
    flo32 tX = RandomF32();
    flo32 tY = 1.0f;
    
    Result.Position   = GetT( BayDoorSpawn->Src, Vec2( tX, tY ) );
    Result.Velocity   = -BayDoorSpawn->Src.yAxis * InitSpeed;
    Result.Friction   = 0.1f;
    
    orect Dest = BayDoorSpawn->Dest;
    Result.Dest       = Dest.Center - Dest.xAxis * ( Dest.HalfDim.x * ( tX * 2.0f - 1.0f ) );
    Result.Health     = 1;
    
    return Result;
}

internal void
AddPersonalPod( APP_STATE * AppState, PERSONAL_POD PersonalPod ) {
    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
    if( BayDoorS->nPersonalPod < PERSONAL_POD_MAX_COUNT ) {
        BayDoorS->PersonalPod[ BayDoorS->nPersonalPod++ ] = PersonalPod;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add PERSONAL_POD to BAY_DOOR_STATE: %u", PERSONAL_POD_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal TRANSPORT
TransportC( BAY_DOOR_SPAWN * BayDoorSpawn, flo32 InitSpeed ) {
    TRANSPORT Result = {};
    Result.WaveGuid   = BayDoorSpawn->WaveGuid;
    
    flo32 tX = RandomF32();
    flo32 tY = 1.0f;
    
    orect Src = BayDoorSpawn->Src;
    Result.Position = GetT( BayDoorSpawn->Src, Vec2( tX, tY ) );
    Result.Velocity = -Src.yAxis * InitSpeed;
    Result.Dest     = BayDoorSpawn->Dest.Center;
    
    Result.Radius   = TRANSPORT_RADIUS;
    Result.Health   = TRANSPORT_HEALTH;
    
    return Result;
}

internal void
AddTransport( APP_STATE * AppState, TRANSPORT Transport ) {
    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
    if( BayDoorS->nTransport < TRANSPORT_MAX_COUNT ) {
        BayDoorS->Transport[ BayDoorS->nTransport++ ] = Transport;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add TRANSPORT to BAY_DOOR_STATE: %u", TRANSPORT_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

#if 0
internal MINIBOSS
MiniBossC( BAY_DOOR_SPAWN * BayDoorSpawn, flo32 InitSpeed ) {
    MINIBOSS Result = {};
    Result.WaveGuid   = BayDoorSpawn->WaveGuid;
    
    orect Src = BayDoorSpawn->Src;
    Result.Position   = Src.Center + Src.yAxis * ( MINIBOSS_RADIUS * 0.4f );
    Result.Velocity   = -Src.yAxis * InitSpeed;
    
    Result.Health     = 150;
    
    Result.iTarget            = RandomS32InRange( 0, AUTO_TURRET_MAX_COUNT ) - 1;
    Result.iTarget_TargetTime = RandomF32InRange( 7.0f, 10.0f );
    
    ENTITY_SHIELD Shield = {};
    Shield.HasShield = true;
    Shield.Radians   = PI * 1.25f;
    Shield.Dist      = 1.0f;
    Shield.Width     = 1.0f;
    Result.Shield    = Shield;
    
    return Result;
}

internal void
AddMiniBoss( APP_STATE * AppState, MINIBOSS MiniBoss ) {
    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
    if( BayDoorS->nMiniBoss < MINIBOSS_MAX_COUNT ) {
        BayDoorS->MiniBoss[ BayDoorS->nMiniBoss++ ] = MiniBoss;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add MINIBOSS to BAY_DOOR_STATE: %u", MINIBOSS_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}
#endif

internal YACHT
YachtC( BAY_DOOR_SPAWN * BayDoorSpawn, flo32 InitSpeed ) {
    YACHT Result = {};
    Result.WaveGuid   = BayDoorSpawn->WaveGuid;
    
    flo32 tX = RandomF32();
    flo32 tY = 1.0f;
    
    orect Src = BayDoorSpawn->Src;
    Result.Position   = GetT( BayDoorSpawn->Src, Vec2( tX, tY ) );
    Result.Velocity   = -Src.yAxis * InitSpeed;
    Result.Dest       = {};
    
    Result.Health        = YACHT_HEALTH;
    
    return Result;
}

internal void
AddYacht( APP_STATE * AppState, YACHT Yacht ) {
    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
    if( BayDoorS->nYacht < YACHT_MAX_COUNT ) {
        BayDoorS->Yacht[ BayDoorS->nYacht++ ] = Yacht;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add YACHT to BAY_DOOR_STATE: %u", YACHT_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal BAY_DOOR_SPAWN
BayDoorSpawnC( orect Src ) {
    BAY_DOOR_SPAWN Result = {};
    Result.WaveGuid = -1;
    Result.Src      = Src;
    
    return Result;
}

internal void
AddBayDoorSpawn( APP_STATE * AppState, BAY_DOOR_SPAWN BayDoorSpawn ) {
    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
    if( BayDoorS->nBayDoorSpawn < BAY_DOOR_SPAWN_MAX_COUNT ) {
        BayDoorS->BayDoorSpawn[ BayDoorS->nBayDoorSpawn++ ] = BayDoorSpawn;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BAY_DOOR_SPAWN to BAY_DOOR_STATE: %u", BAY_DOOR_SPAWN_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateRingForYachtExit( YACHT_EXIT_RING_DATA * Ring, flo32 dT, boo32 DoExit, flo32 Exit_Timer, vec2 RotateSpeed, vec3 Radius ) {
    flo32 InitRadius = Radius.z;
    if( DoExit ) {
        { // Update Yacht Exit Rotate Speed
            flo32 MinRotateSpeed = RotateSpeed.x;
            flo32 MaxRotateSpeed = RotateSpeed.y;
            
            if( Ring->RotateSpeed < MinRotateSpeed ) {
                Ring->RotateSpeed += ( MinRotateSpeed * dT );
                Ring->RotateSpeed  = MinValue( Ring->RotateSpeed, MinRotateSpeed );
                
                if( Exit_Timer >= ( 2.0f / 60.0f ) ) {
                    Ring->RotateSpeed = MaxValue( Ring->RotateSpeed, MinRotateSpeed );
                }
            } else {
                if( Exit_Timer >= ( 2.0f / 60.0f ) ) {
                    Ring->RotateSpeed += ( Ring->RotateSpeed * dT * 4.0f );
                    Ring->RotateSpeed  = Clamp( Ring->RotateSpeed, MinRotateSpeed, MaxRotateSpeed );
                } else {
                    Ring->RotateSpeed -= ( Ring->RotateSpeed * dT * 2.0f );
                    Ring->RotateSpeed  = Clamp( Ring->RotateSpeed, MinRotateSpeed, MaxRotateSpeed );
                }
            }
        }
        
        { // Update Yacht Exit Radius
            flo32 MinRadius = Radius.x;
            flo32 MaxRadius = Radius.y;
            
            Ring->Radius = MaxValue( Ring->Radius, InitRadius );
            if( Ring->Radius < MinRadius ) {
                Ring->Radius += ( dT * 0.5f );
                Ring->Radius  = MinValue( Ring->Radius, MinRadius );
                
                if( Exit_Timer >= ( 2.0f / 60.0f ) ) {
                    Ring->Radius = MaxValue( Ring->Radius, MinRadius );
                }
            } else {
                if( Exit_Timer >= ( 2.0f / 60.0f ) ) {
                    Ring->Radius += ( Ring->Radius * dT * 0.5f );
                    Ring->Radius  = Clamp( Ring->Radius, MinRadius, MaxRadius );
                } else {
                    Ring->Radius -= ( Ring->Radius * dT );
                    Ring->Radius  = Clamp( Ring->Radius, MinRadius, MaxRadius );
                }
            }
            Ring->Radius = MaxValue( Ring->Radius, InitRadius );
        }
        
        { // Update Yacht Exit tColor
            if( Exit_Timer >= ( 2.0f / 60.0f ) ) {
                Ring->tColor += ( dT * 0.333333f );
            } else {
                Ring->tColor -= ( Ring->tColor * dT * 2.0f );
            }
            Ring->tColor = Clamp01( Ring->tColor );
        }
        
        Ring->Alpha = MinValue( Ring->Alpha + dT * 0.5f, 1.0f );
    } else {
        Ring->Alpha = MaxValue( Ring->Alpha - dT * 0.5f, 0.0f );
        
        Ring->RotateSpeed -= ( Ring->RotateSpeed * dT * 2.0f );
        Ring->RotateSpeed  = MaxValue( Ring->RotateSpeed, 0.0f );
        
        Ring->Radius -= ( Ring->Radius * dT );
        Ring->Radius  = MaxValue( Ring->Radius, InitRadius );
        
        Ring->tColor -= ( Ring->tColor * dT * 2.0f );
        Ring->tColor  = MaxValue( Ring->tColor, 0.0f );
    }
    
    Ring->Radians     = WrapRadians( Ring->Radians + Ring->RotateSpeed * dT );
}

internal void
UpdateBayDoors( APP_STATE * AppState, flo32 dT ) {
    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
    PLAYER_STATE   * Player   = &AppState->Player;
    
    { // Bay Door Spawn Animation
        boo32 IsActive[ BAY_DOOR_SPAWN_DEST_MAX_COUNT ] = {};
        for( int32 iBayDoorSpawn = 0; iBayDoorSpawn < BayDoorS->nBayDoorSpawn; iBayDoorSpawn++ ) {
            BAY_DOOR_SPAWN BayDoorSpawn = BayDoorS->BayDoorSpawn[ iBayDoorSpawn ];
            
            if( ( BayDoorSpawn.Stage != BayDoorEventStage_Wait ) && ( BayDoorSpawn.Stage != BayDoorEventStage_CloseBayDoors ) && ( BayDoorSpawn.Stage != BayDoorEventStage_Complete ) && ( BayDoorSpawn.Stage != BayDoorEventStage_EndLevel ) ) {
                IsActive[ BayDoorSpawn.iDest ] = true;
            }
        }
        
        flo32 dAlpha = 1.0f / 0.5f;
        for( int32 iDest = 0; iDest < BAY_DOOR_SPAWN_DEST_MAX_COUNT; iDest++ ) {
            if( IsActive[ iDest ] ) {
                BayDoorS->BayDoorSpawn_DestAlpha[ iDest ] = MinValue( BayDoorS->BayDoorSpawn_DestAlpha[ iDest ] + dAlpha * dT, 1.0f );
            } else {
                BayDoorS->BayDoorSpawn_DestAlpha[ iDest ] = MaxValue( BayDoorS->BayDoorSpawn_DestAlpha[ iDest ] - dAlpha * dT, 0.0f );
            }
        }
        
        flo32 dRadians = TAU / 2.0f;
        BayDoorS->BayDoorSpawn_Radians = WrapRadians( BayDoorS->BayDoorSpawn_Radians + dRadians * dT );
    }
    
    if( BayDoorS->SpawnPersonalPod_IsActive ) {
        int32 nAvailable                                 = 0;
        int32 iAvailable[ BAY_DOOR_SPAWN_SRC_MAX_COUNT ] = {};
        for( int32 iBayDoorSpawn = 0; iBayDoorSpawn < BAY_DOOR_SPAWN_SRC_MAX_COUNT; iBayDoorSpawn++ ) {
            BAY_DOOR_SPAWN BayDoorSpawn = BayDoorS->BayDoorSpawn[ iBayDoorSpawn ];
            if( ( BayDoorSpawn.Stage == BayDoorEventStage_Wait ) || ( BayDoorSpawn.Stage == BayDoorEventStage_Complete ) ) {
                iAvailable[ nAvailable++ ] = iBayDoorSpawn;
            }
        }
        
        if( ( BayDoorS->SpawnTransport_IsActive ) && ( BayDoorS->SpawnTransport_iStage < BAY_DOOR_SPAWN_TRANSPORT_YACHT_SPAWN_COUNT ) ) {
            BayDoorS->SpawnTransport_Timer += dT;
            if( BayDoorS->SpawnTransport_Timer >= BAY_DOOR_SPAWN_TRANSPORT_COOLDOWN_TARGET_TIME ) {
                BayDoorS->SpawnTransport_Timer = 0.0f;
                
                if( nAvailable > 1 ) {
                    BayDoorS->SpawnTransport_Timer    = 0.0f;
                    
                    int32 iBayDoor_Select         = RandomS32InRange( 0, nAvailable - 1 );
                    int32 iSrc                    = iAvailable[ iBayDoor_Select ];
                    iAvailable[ iBayDoor_Select ] = iAvailable[ --nAvailable ];
                    
                    int32 iDest  = RandomS32InRange( 0, BAY_DOOR_SPAWN_DEST_MAX_COUNT - 1 );
                    
                    boo32 DoYacht = BayDoorS->SpawnTransport_DoYacht[ BayDoorS->SpawnTransport_iStage++ ];
                    
                    if( DoYacht ) {
                        int32 nYacht = 1;
                        flo32 Spawn_TargetTime = ( 60.0f / 60.0f );
                        
                        BAY_DOOR_SPAWN Spawn = {};
                        Spawn.WaveGuid         = -1;
                        Spawn.Type             = BayDoorEventType_Yacht;
                        Spawn.Stage            = BayDoorEventStage_OpenBayDoors;
                        Spawn.TargetTime       = ( flo32 )nYacht * Spawn_TargetTime;
                        Spawn.iSrc             = iSrc;
                        Spawn.iDest            = iDest;
                        Spawn.Src              = BayDoorS->BayDoorSpawn_Src [ iSrc  ];
                        Spawn.Dest             = BayDoorS->BayDoorSpawn_Dest[ iDest ];
                        Spawn.Spawn_TargetTime = Spawn_TargetTime;
                        
                        BayDoorS->BayDoorSpawn[ iSrc ] = Spawn;
                    } else {
                        int32 nTransport      = 8;
                        flo32 Spawn_TargetTime = ( 16.0f / 60.0f );
                        
                        BAY_DOOR_SPAWN Spawn = {};
                        Spawn.WaveGuid         = -1;
                        Spawn.Type             = BayDoorEventType_Transport;
                        Spawn.Stage            = BayDoorEventStage_OpenBayDoors;
                        Spawn.TargetTime       = ( flo32 )nTransport * Spawn_TargetTime;
                        Spawn.iSrc             = iSrc;
                        Spawn.iDest            = iDest;
                        Spawn.Src              = BayDoorS->BayDoorSpawn_Src [ iSrc  ];
                        Spawn.Dest             = BayDoorS->BayDoorSpawn_Dest[ iDest ];
                        Spawn.Spawn_TargetTime = Spawn_TargetTime;
                        
                        BayDoorS->BayDoorSpawn[ iSrc ] = Spawn;
                    }
                } else {
                    BayDoorS->SpawnTransport_Timer = BAY_DOOR_SPAWN_TRANSPORT_COOLDOWN_TARGET_TIME - 1.0f;
                }
            }
        }
        
        if( BayDoorS->SpawnPersonalPod_iStage < BAY_DOOR_SPAWN_PERSONAL_POD_SPAWN_COUNT ) {
            BayDoorS->SpawnPersonalPod_Timer += dT;
            if( BayDoorS->SpawnPersonalPod_Timer >= BAY_DOOR_SPAWN_PERSONAL_POD_COOLDOWN_TARGET_TIME ) {
                if( nAvailable > 1 ) {
                    BayDoorS->SpawnPersonalPod_Timer = 0.0f;
                    
                    int32 Select = RandomS32InRange( 0, nAvailable - 1 );
                    int32 iSrc   = iAvailable[ Select ];
                    iAvailable[ Select ] = iAvailable[ --nAvailable ];
                    
                    int32 iDest  = RandomS32InRange( 0, BAY_DOOR_SPAWN_DEST_MAX_COUNT - 1 );
                    int32 nPersonalPod = BayDoorS->SpawnPersonalPod_nPersonalPod[ BayDoorS->SpawnPersonalPod_iStage++ ];
                    
                    BAY_DOOR_SPAWN * BayDoorSpawn = BayDoorS->BayDoorSpawn + iSrc;
                    flo32 TargetTime[ 2 ] = {
                        BAY_DOOR_SPAWN_PERSONAL_POD_TARGET_TIME_SLOW,
                        BAY_DOOR_SPAWN_PERSONAL_POD_TARGET_TIME_FAST,
                    };
                    
                    int32 Spawn_Select     = RandomS32InRange( 0, 1 );
                    flo32 Spawn_TargetTime = TargetTime[ Spawn_Select ];
                    
                    BAY_DOOR_SPAWN Spawn = {};
                    Spawn.WaveGuid         = -1;
                    Spawn.Type             = BayDoorEventType_PersonalPod;
                    Spawn.Stage            = BayDoorEventStage_OpenBayDoors;
                    Spawn.TargetTime       = ( flo32 )nPersonalPod * Spawn_TargetTime;
                    Spawn.iSrc             = iSrc;
                    Spawn.iDest            = iDest;
                    Spawn.Src              = BayDoorS->BayDoorSpawn_Src [ iSrc  ];
                    Spawn.Dest             = BayDoorS->BayDoorSpawn_Dest[ iDest ];
                    Spawn.Spawn_TargetTime = Spawn_TargetTime;
                    
                    BayDoorS->BayDoorSpawn[ iSrc ] = Spawn;
                } else {
                    BayDoorS->SpawnPersonalPod_Timer = BAY_DOOR_SPAWN_PERSONAL_POD_COOLDOWN_TARGET_TIME - 1.0f;
                }
            }
        }
        
        { // Mini Spawns
            flo32 TargetTime = BAY_DOOR_SPAWN_PERSONAL_POD_COOLDOWN_TARGET_TIME * 2.5f;
            
            if( ( BayDoorS->SpawnPersonalPod_iStage < BAY_DOOR_SPAWN_PERSONAL_POD_SPAWN_COUNT ) && ( nAvailable > 2 ) ) {
                BayDoorS->SpawnPersonalPod_MiniSpawnTimer += dT;
                if( BayDoorS->SpawnPersonalPod_MiniSpawnTimer >= TargetTime ) {
                    BayDoorS->SpawnPersonalPod_MiniSpawnTimer = 0.0f;
                    
                    int32 Select = RandomS32InRange( 0, nAvailable - 1 );
                    int32 iSrc   = iAvailable[ Select ];
                    iAvailable[ Select ] = iAvailable[ --nAvailable ];
                    
                    int32 iDest  = RandomS32InRange( 0, BAY_DOOR_SPAWN_DEST_MAX_COUNT - 1 );
                    int32 nPersonalPod = 12;
                    
                    BAY_DOOR_SPAWN * BayDoorSpawn = BayDoorS->BayDoorSpawn + iSrc;
                    
                    flo32 Spawn_TargetTime = BAY_DOOR_SPAWN_PERSONAL_POD_TARGET_TIME_SLOW;
                    
                    BAY_DOOR_SPAWN Spawn = {};
                    Spawn.WaveGuid         = -1;
                    Spawn.Type             = BayDoorEventType_PersonalPod;
                    Spawn.Stage            = BayDoorEventStage_OpenBayDoors;
                    Spawn.TargetTime       = ( flo32 )nPersonalPod * Spawn_TargetTime;
                    Spawn.iSrc             = iSrc;
                    Spawn.iDest            = iDest;
                    Spawn.Src              = BayDoorS->BayDoorSpawn_Src [ iSrc  ];
                    Spawn.Dest             = BayDoorS->BayDoorSpawn_Dest[ iDest ];
                    Spawn.Spawn_TargetTime = Spawn_TargetTime;
                    
                    BayDoorS->BayDoorSpawn[ iSrc ] = Spawn;
                }
            }
        }
    }
    
    for( int32 iBayDoorSpawn = 0; iBayDoorSpawn < BayDoorS->nBayDoorSpawn; iBayDoorSpawn++ ) {
        BAY_DOOR_SPAWN * BayDoorSpawn = BayDoorS->BayDoorSpawn + iBayDoorSpawn;
        
        flo32 Anim_Timer = BayDoorSpawn->BayDoors_AnimT * PERSONAL_POD_BAY_DOORS_TARGET_TIME;
        
        switch( BayDoorSpawn->Stage ) {
            case BayDoorEventStage_Wait: {
                // DO NOTHING
            } break;
            
            case BayDoorEventStage_OpenBayDoors: {
                Anim_Timer = Clamp( Anim_Timer + dT, 0.0f, PERSONAL_POD_BAY_DOORS_TARGET_TIME );
                
                if( Anim_Timer >= PERSONAL_POD_BAY_DOORS_TARGET_TIME ) {
                    BayDoorSpawn->Stage = BayDoorEventStage_SpawnEntities;
                }
            } break;
            
            case BayDoorEventStage_SpawnEntities: {
                switch( BayDoorSpawn->Type ) {
                    case BayDoorEventType_PersonalPod: {
                        BayDoorSpawn->Spawn_Timer += dT;
                        if( BayDoorSpawn->Spawn_Timer >= BayDoorSpawn->Spawn_TargetTime ) {
                            BayDoorSpawn->Spawn_Timer = 0.0f;
                            AddPersonalPod( AppState, PersonalPodC( BayDoorSpawn, BAY_DOOR_SPAWN_PERSONAL_POD_INIT_SPEED ) );
                        }
                        
                        BayDoorSpawn->Timer += dT;
                        if( BayDoorSpawn->Timer >= BayDoorSpawn->TargetTime ) {
                            BayDoorSpawn->Stage = BayDoorEventStage_CloseBayDoors;
                        }
                    } break;
                    
                    case BayDoorEventType_Transport: {
                        //AddTransport( AppState, TransportC( BayDoorSpawn, BAY_DOOR_SPAWN_TRANSPORT_INIT_SPEED ) );
                        //BayDoorSpawn->Stage = BayDoorEventStage_ClearBayDoors;
                        
                        BayDoorSpawn->Spawn_Timer += dT;
                        if( BayDoorSpawn->Spawn_Timer >= BayDoorSpawn->Spawn_TargetTime ) {
                            BayDoorSpawn->Spawn_Timer = 0.0f;
                            AddTransport( AppState, TransportC( BayDoorSpawn, BAY_DOOR_SPAWN_TRANSPORT_INIT_SPEED ) );
                        }
                        
                        BayDoorSpawn->Timer += dT;
                        if( BayDoorSpawn->Timer >= BayDoorSpawn->TargetTime ) {
                            BayDoorSpawn->Stage = BayDoorEventStage_CloseBayDoors;
                        }
                    } break;
                    
#if 0                    
                    case BayDoorEventType_MiniBoss: {
                        AddMiniBoss( AppState, MiniBossC( BayDoorSpawn, BAY_DOOR_SPAWN_MINIBOSS_INIT_SPEED ) );
                        BayDoorSpawn->Stage = BayDoorEventStage_ClearBayDoors;
                    } break;
#endif
                    
                    case BayDoorEventType_Yacht: {
                        BayDoorSpawn->Timer += dT;
                        if( BayDoorSpawn->Timer >= BayDoorSpawn->TargetTime ) {
                            AddYacht( AppState, YachtC( BayDoorSpawn, BAY_DOOR_SPAWN_YACHT_INIT_SPEED ) );
                            BayDoorSpawn->Stage = BayDoorEventStage_CloseBayDoors;
                        }
                    } break;
                    
                    default: {
                        InvalidCodePath;
                    } break;
                }
            } break;
            
            case BayDoorEventStage_ClearBayDoors: {
                flo32 TargetTime_Transport = 8.0f;
                flo32 TargetTime_MiniBoss  = 3.0f;
                
                flo32 TargetTime = ( BayDoorSpawn->Type == BayDoorEventType_Transport ) ? TargetTime_Transport : TargetTime_MiniBoss;
                
                BayDoorSpawn->Timer += dT;
                if( BayDoorSpawn->Timer >= TargetTime ) {
                    BayDoorSpawn->Stage = BayDoorEventStage_CloseBayDoors;
                }
            } break;
            
            case BayDoorEventStage_CloseBayDoors: {
                Anim_Timer = Clamp( Anim_Timer - dT, 0.0f, PERSONAL_POD_BAY_DOORS_TARGET_TIME );
                
                if( Anim_Timer <= 0.0f ) {
                    BayDoorSpawn->Stage = BayDoorEventStage_Complete;
                }
            } break;
            
            case BayDoorEventStage_Complete: {
                // DO NOTHING
            } break;
            
            case BayDoorEventStage_EndLevel: {
                // DO NOTHING
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        BayDoorSpawn->BayDoors_AnimT = Clamp01( Anim_Timer / PERSONAL_POD_BAY_DOORS_TARGET_TIME );
    }
    
    flo32 ColorDT_PersonalPod = ( 1.0f / (  12.0f / 60.0f ) ) * dT;
    flo32 ColorDT_Transport   = ( 1.0f / (  36.0f / 60.0f ) ) * dT;
    flo32 ColorDT_Yacht       = ( 1.0f / (  40.0f / 60.0f ) ) * dT;
    flo32 ColorDT_MiniBoss    = ( 1.0f / ( 100.0f / 60.0f ) ) * dT;
    
    flo32 dFriction = ( 1.0f / 3.0f ) * dT;
    
    for( int32 iPersonalPod = 0; iPersonalPod < BayDoorS->nPersonalPod; iPersonalPod++ ) {
        PERSONAL_POD * PersonalPod = BayDoorS->PersonalPod + iPersonalPod;
        
        PersonalPod->Friction += dFriction;
        PersonalPod->ColorT = Clamp01( PersonalPod->ColorT + ColorDT_PersonalPod );
        
        vec2 Accel = {};
        if( IsInRect( PersonalPod->Position, AppState->World_Bound ) ) {
            vec2 Dir = GetNormal( PersonalPod->Dest - PersonalPod->Position );
            Accel = ( Dir * PERSONAL_POD_SPEED - PersonalPod->Velocity ) * PersonalPod->Friction;
        }
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + PersonalPod->Velocity * dT;
        
        PersonalPod->Position += dPos;
        PersonalPod->Velocity += Accel * dT;
    }
    
    for( int32 iTransport = 0; iTransport < BayDoorS->nTransport; iTransport++ ) {
        TRANSPORT * Transport = BayDoorS->Transport + iTransport;
        
        Transport->ColorT = Clamp01( Transport->ColorT + ColorDT_Transport );
        
        vec2 Accel = {};
        if( IsInRect( Transport->Position, AppState->World_Bound ) ) {
            vec2 Dir = GetNormal( Transport->Dest - Transport->Position );
            Accel = ( Dir * TRANSPORT_SPEED - Transport->Velocity ) * TRANSPORT_FRICTION;
        }
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Transport->Velocity * dT;
        
        Transport->Position += dPos;
        Transport->Velocity += Accel * dT;
    }
    
#if 0    
    for( int32 iMiniBoss = 0; iMiniBoss < BayDoorS->nMiniBoss; iMiniBoss++ ) {
        MINIBOSS * MiniBoss = BayDoorS->MiniBoss + iMiniBoss;
        
        { // Update Shield_SegmentT
            flo32 Temp[ MINIBOSS_SHIELD_SEGMENT_MAX_COUNT ] = {};
            memcpy( Temp, MiniBoss->Shield_SegmentT, sizeof( flo32 ) * MINIBOSS_SHIELD_SEGMENT_MAX_COUNT );
            
            for( int32 iSegment = 0; iSegment < MINIBOSS_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
                int32 iSegmentA =   iSegment;
                int32 iSegmentB = ( iSegment + 1 ) % MINIBOSS_SHIELD_SEGMENT_MAX_COUNT;
                
                flo32 SegmentTA = MiniBoss->Shield_SegmentT[ iSegmentA ];
                flo32 SegmentTB = MiniBoss->Shield_SegmentT[ iSegmentB ];
                
                flo32 tA = MaxValue( ( SegmentTB - SegmentTA ) * 0.65f, 0.0f );
                flo32 tB = MaxValue( ( SegmentTA - SegmentTB ) * 0.65f, 0.0f );
                
                Temp[ iSegmentA ] = MinValue( Temp[ iSegmentA ] + tA, MINIBOSS_SHIELD_MAX_T );
                Temp[ iSegmentB ] = MinValue( Temp[ iSegmentB ] + tB, MINIBOSS_SHIELD_MAX_T );
            }
            
            memcpy( MiniBoss->Shield_SegmentT, Temp, sizeof( flo32 ) * MINIBOSS_SHIELD_SEGMENT_MAX_COUNT );
        }
        
        for( int32 iRepel = iMiniBoss + 1; iRepel < BayDoorS->nMiniBoss; iRepel++ ) {
            MINIBOSS * Repel = BayDoorS->MiniBoss + iRepel;
            
            vec2 V = Repel->Position - MiniBoss->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = MINIBOSS_REPEL_RADIUS + MINIBOSS_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * MINIBOSS_REPEL_SPEED;
                
                MiniBoss->Repel -= Repel_Vector;
                Repel->Repel  += Repel_Vector;
            }
        }
        
        MiniBoss->iTarget_Timer += dT;
        if( MiniBoss->iTarget_Timer >= MiniBoss->iTarget_TargetTime ) {
            MiniBoss->iTarget_Timer      = 0.0f;
            MiniBoss->iTarget_TargetTime = RandomF32InRange( 7.0f, 10.0f );
            
            if( MiniBoss->iTarget > -1 ) {
                MiniBoss->iTarget = -1;
            } else {
                MiniBoss->iTarget = RandomS32InRange( 0, 1 );
            }
        }
        
        vec2 ToPos = Player->Position;
        if( MiniBoss->iTarget > -1 ) {
            AUTO_TURRET_STATE * AutoTurretS = &AppState->AutoTurretS;
            AUTO_TURRET         AutoTurret  = AutoTurretS->AutoTurret[ MiniBoss->iTarget ];
            ToPos = AutoTurret.Position;
        }
        
        flo32 Radians = ToRadians( ToPos - MiniBoss->Position );
        
        ENTITY_SHIELD * Shield = &MiniBoss->Shield;
        Shield->bRadians = WrapRadians( Radians - Shield->Radians * 0.5f );
        
        flo32 t = dT * 4.0f;
        for( int32 iSegment = 0; iSegment < MINIBOSS_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
            MiniBoss->Shield_SegmentT[ iSegment ] = MaxValue( MiniBoss->Shield_SegmentT[ iSegment ] - t, 0.0f );
        }
        
        MiniBoss->ColorT = Clamp01( MiniBoss->ColorT + ColorDT_MiniBoss );
        
        vec2 Dir   = GetNormal( ToPos - MiniBoss->Position );
        vec2 Accel = ( Dir * MINIBOSS_SPEED - MiniBoss->Velocity + MiniBoss->Repel ) * MINIBOSS_FRICTION;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + MiniBoss->Velocity * dT;
        
        MiniBoss->Position += dPos;
        MiniBoss->Velocity += Accel * dT;
        
        MiniBoss->Repel = {};
    }
#endif
    
    { // Update Yacht Exit
        vec2 YachtExit_bPos  = {};
        BayDoorS->YachtExit_tFlash = MaxValue( BayDoorS->YachtExit_tFlash - ( 8.0f * dT ) * 2.0f, 0.0f );
        if( BayDoorS->YachtExit_tFlash > 0.0f ) {
            AddYachtExitParticles( AppState, YachtExit_bPos, 4, 1.0f, 1.0f );
        }
        
        // Find yacht that is closest to exiting
        YACHT * Yacht = 0;
        for( int32 iYacht = 0; iYacht < BayDoorS->nYacht; iYacht++ ) {
            YACHT * Y = BayDoorS->Yacht + iYacht;
            if( ( Y->Health > 0 ) && ( ( !Yacht ) || ( Y->Exit_Timer > Yacht->Exit_Timer ) ) ) {
                Yacht = Y;
            }
        }
        
        boo32 DoExit     = false;
        flo32 Exit_Timer = 0.0f;
        if( Yacht ) {
            DoExit     = true;
            Exit_Timer = Yacht->Exit_Timer;
        }
        
        vec2 InnerRing_Speed = Vec2( TAU / 4.0f, TAU / 0.25f );
        vec2 MidRing_Speed   = Vec2( TAU / 6.0f, TAU / 0.75f );
        UpdateRingForYachtExit( &BayDoorS->YachtExit_InnerRing, dT, DoExit, Exit_Timer, InnerRing_Speed, Vec3( 1.25f, 2.0f, 0.25f ) );
        UpdateRingForYachtExit( &BayDoorS->YachtExit_MidRing,   dT, DoExit, Exit_Timer, MidRing_Speed, Vec3( 2.0f,  2.25f, 0.35f ) );
        
        { // Update particles
            YACHT_EXIT_RING_DATA Ring = BayDoorS->YachtExit_MidRing;
            if( Ring.Alpha >= 0.7f ) {
                flo32 tSpeed = Clamp01( ( Ring.RotateSpeed - MidRing_Speed.x ) / ( MidRing_Speed.y - MidRing_Speed.x ) );
                flo32 Numer = lerp( 6.0f, tSpeed, 2.0f );
                
                flo32 TargetTime = ( Numer / 60.0f );
                
                BayDoorS->YachtExit_ParticleTimer += dT;
                if( BayDoorS->YachtExit_ParticleTimer >= TargetTime ) {
                    BayDoorS->YachtExit_ParticleTimer = 0.0f;
                    
                    AddYachtExitParticles( AppState, YachtExit_bPos, 1, 0.1f, 1.0f );
                }
            }
        }
    }
    
    for( int32 iYacht = 0; iYacht < BayDoorS->nYacht; iYacht++ ) {
        YACHT * Yacht = BayDoorS->Yacht + iYacht;
        
        Yacht->ColorT = Clamp01( Yacht->ColorT + ColorDT_Yacht );
        Yacht->Damage_IFrameTimer = MaxValue( Yacht->Damage_IFrameTimer - dT, 0.0f );
        
        Yacht->ShowDamage_Timer = MaxValue( Yacht->ShowDamage_Timer - dT, 0.0f );
        Yacht->Light_Timer += dT;
        if( Yacht->Exit_Timer > 0.0f ) {
            Yacht->Light_Timer += ( dT * 2.0f );
        }
        if( Yacht->Light_Timer >= YACHT_LIGHT_TARGET_TIME ) {
            Yacht->Light_Timer -= YACHT_LIGHT_TARGET_TIME;
        }
        
        rect Dest_Bound = RectCD( Yacht->Dest, Vec2( 1.0f, 1.0f ) );
        
        vec2  Dir      = {};
        flo32 Friction = YACHT_FRICTION;
        if( ( Yacht->Health > 0 ) && ( Yacht->Exit_Timer <= 0.0f ) ) {
            Dir = GetNormal( Yacht->Dest - Yacht->Position );
            
            if( Yacht->Exit_Timer <= 0.1f ) {
                Friction = 1.5f;
            }
            
            if( IsInRect( Yacht->Position, Dest_Bound ) ) {
                Yacht->Exit_Timer += dT;
            }
        } else {
            Yacht->Exit_Timer += dT;
        }
        
        vec2 Accel = ( Dir * YACHT_SPEED - Yacht->Velocity ) * Friction;
        vec2 dPos  = Accel * ( 0.5f * dT * dT ) + Yacht->Velocity * dT;
        
        Yacht->Position += dPos;
        Yacht->Velocity += Accel * dT;
        
        if( Yacht->Exit_Timer > 0.0f ) {
            vec2 V    = Yacht->Dest - Yacht->Position;
            vec2 Norm = GetNormal( V );
            
            flo32 Speed   = 0.5f;
            flo32 Epsilon = Speed * dT;
            flo32 Dist    = GetLength( V );
            
            if( Dist <= Epsilon ) {
                Yacht->Position  = Yacht->Dest;
                Yacht->Velocity  = {};
            } else {
                Yacht->Position += Norm * Epsilon;
            }
        }
    }
    
    //DISPLAY_VALUE( uint32, BayDoorS->nPersonalPod );
    //DISPLAY_VALUE( uint32, BayDoorS->nTransport );
}

internal void
DrawPersonalPod( RENDER_PASS * Pass, PERSONAL_POD PersonalPod, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4  ColorA  = lerp( COLOR_BLACK, PersonalPod.ColorT, COLOR_GRAY( 0.25f ) ) * Alpha;
    vec4  ColorB  = lerp( COLOR_BLACK, PersonalPod.ColorT, COLOR_GRAY( 0.175f ) ) * Alpha;
    
    vec2  Dir     = GetNormal( PersonalPod.Velocity );
    flo32 Radians = ToRadians( Dir );
    vec2  Sca     = PERSONAL_POD_RADIUS * ( 0.5f * Scale );
    vec2  PosA    = PersonalPod.Position + Dir * ( Sca.x * 0.4f );
    vec2  PosB    = PersonalPod.Position - Dir * ( Sca.x * 0.6f );
    flo32 RadiusA = Sca.y * 0.6f;
    
    //DrawEllipse( Pass, PersonalPod.Position, Sca, Radians, ColorA );
    //DrawCircle ( Pass, PosA, RadiusA, ColorB );
    
    //orect BoundC = ORectCD( PosB, Vec2( 0.75f, 0.75f ), 0.0f );
    //BoundC.HalfDim *= 3.0f;
    //flo32 Denom = 7.0f;
    //flo32 t = Clamp01( ( PersonalPod.Position.y / 24.0f ) * 0.5f + 0.5f );
    //DrawORect( Pass, TextureID_Glow, BoundC, Vec4( t, t, t, 0.0f ) );
    
    orect BoundA = ORectCD( PersonalPod.Position, Sca * 2.0f, Radians );
    orect BoundB = ORectCR( PosA, RadiusA, Radians );
    
    DrawORect( Pass, TextureID_RoundedDome, BoundA, ColorA );
    DrawORect( Pass, TextureID_Dome,        BoundB, ColorB );
}

internal void
DrawTransport( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, TRANSPORT Transport, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4 ColorA = lerp( COLOR_BLACK, Transport.ColorT, COLOR_GRAY( 0.18f ) ) * Alpha;
    vec4 ColorB = lerp( COLOR_BLACK, Transport.ColorT, COLOR_GRAY( 0.135f ) ) * Alpha;
    
    vec2  Dir     = GetNormal( Transport.Velocity );
    flo32 Radians = ToRadians( Dir );
    vec2  Sca     = Vec2( 0.2f, 0.45f ) * ( TRANSPORT_RADIUS * Scale );
    vec2  PosA    = Transport.Position + Dir * ( TRANSPORT_RADIUS * 0.55f * Scale );
    
    //DrawCircle ( Pass, Transport.Position, TRANSPORT_RADIUS * Scale, ColorA );
    //DrawEllipse( Pass, PosA, Sca, Radians, ColorB );
    
    orect BoundA = ORectCR( Transport.Position, TRANSPORT_RADIUS * Scale, Radians );
    orect BoundB = ORectCD( PosA, Sca * 2.0f, Radians );
    
    DrawORect( Pass, TextureID_RoundedDome, BoundA, ColorA );
    DrawORect( Pass, TextureID_Dome,        BoundB, ColorB );
    
    //orect Bound = ORectBCDY( Transport.Position, TRANSPORT_DIM, GetNormal( -Transport.Velocity ) );
    //DrawORect( Pass, Bound, ColorA );
    
    if( Transport.Health > 0 ) {
        vec2 Pos = Transport.Position - Dir * ( Transport.Radius * 0.2f * Scale );
        DrawEntityHealth( Pass, AppState, Camera, Pos, Transport.Health, COLOR_WHITE );
    }
}

internal void
DrawMiniBoss( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, MINIBOSS MiniBoss, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    {
        vec4 ColorA = lerp( COLOR_BLACK, MiniBoss.ColorT, Vec4( 0.4f, 0.0f, 0.0f, 1.0f ) );
        
        DrawCircle( Pass, MiniBoss.Position, MINIBOSS_RADIUS, ColorA );
        
        if( MiniBoss.Health > 0 ) {
            Assert( Font );
            Assert( Camera );
            
            char Str[ 4 ] = {};
            sprintf( Str, "%u", MiniBoss.Health );
            DrawString( Pass, Font, Str, MiniBoss.Position, TextAlign_Center, WORLD_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE );
        }
    }
    
    ENTITY_SHIELD Shield = MiniBoss.Shield;
    if( Shield.HasShield ) {
        flo32 Radius_Inner   = ( MINIBOSS_RADIUS + Shield.Dist                ) * Scale;
        flo32 Radius_Outer   = ( MINIBOSS_RADIUS + Shield.Dist + Shield.Width ) * Scale;
        
        vec4 ColorA = Vec4( 0.0f, 0.35f, 0.35f, 1.0f );
        vec4 ColorB = COLOR_CYAN;
        
        int32 nSegment = ( int32 )( ( flo32 )MINIBOSS_SHIELD_SEGMENT_MAX_COUNT * Clamp01( Shield.Radians / TAU ) );
        
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            flo32 Segment_Radians = Shield.bRadians + ( ( flo32 )iSegment / ( flo32 )nSegment ) * Shield.Radians;
            vec2  Segment_Offset  = ToDirection2D( Segment_Radians ) * ( ( Radius_Outer + Radius_Inner ) * 0.5f );
            vec2  Segment_Scale   = {};
            if( ( iSegment % 2 ) == 0 ) {
                Segment_Scale.x = ( TAU / ( flo32 )MINIBOSS_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Outer * 1.01f;
            } else {
                Segment_Scale.x = ( TAU / ( flo32 )MINIBOSS_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Inner * 1.01f;
            }
            Segment_Scale.y = ( Radius_Outer - Radius_Inner ) * 0.5f;
            
            flo32 t = MiniBoss.Shield_SegmentT[ iSegment ];
            vec4 Color = {};
            if( t > 1.0f ) {
                flo32 tA = Clamp01( ( t - 1.0f ) / ( MINIBOSS_SHIELD_MAX_T - 1.0f ) );
                Color = lerp( ColorB, tA, COLOR_WHITE );
            } else {
                Color = lerp( ColorA, MiniBoss.Shield_SegmentT[ iSegment ], ColorB );
                
                flo32 a = lerp( 0.5f, t, 1.0f );
                Color *= a;
            }
            
            flo32 tOffset = lerp( 1.0f, t / MINIBOSS_SHIELD_MAX_T, 0.985f );
            flo32 tScaleY = lerp( 1.0f, t / MINIBOSS_SHIELD_MAX_T, 0.9f );
            
            Segment_Offset  *= tOffset;
            Segment_Scale.y *= tScaleY;
            
            if( ( iSegment % 2 ) == 0 ) {
                DrawTriangle( Pass, MiniBoss.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color * Alpha );
            } else {
                DrawTriangle( Pass, MiniBoss.Position + Segment_Offset, Segment_Scale, Segment_Radians - PI * 0.5f, Color * Alpha );
            }
        }
    }
}

internal void
DrawYacht( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, YACHT Yacht, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 RadiusA = YACHT_RADIUS * Scale;
    flo32 RadiusB = RadiusA * 0.65f;
    
    {
        vec4 bColorA = YACHT_BASE_COLOR;
        vec4 bColorB = TowardBlack( bColorA, 0.35f );
        
        vec4 ColorA = TowardBlack( bColorA, 1.0f - Yacht.ColorT );
        vec4 ColorB = TowardBlack( bColorB, 1.0f - Yacht.ColorT );
        
        {
            flo32 TargetTime = YACHT_SHOW_DAMAGE_TARGET_TIME - ENTITY_SHOW_DAMAGE_TARGET_TIME;
            if( Yacht.ShowDamage_Timer > TargetTime ) {
                flo32 t = Clamp01( ( Yacht.ShowDamage_Timer - TargetTime ) / ( YACHT_SHOW_DAMAGE_TARGET_TIME - TargetTime ) );
                ColorA = TowardWhite( ColorA, t * 0.6f );
                ColorB = TowardWhite( ColorB, t * 0.6f );
            }
        }
        
        flo32 t = Clamp01( Yacht.Exit_Timer / ( YACHT_EXIT_TARGET_TIME - ( 20.0f / 60.0f ) ) );
        ColorA = TowardWhite( ColorA, t * 0.8f );
        ColorB = TowardWhite( ColorB, t * 0.8f );
        
        DrawRect( Pass, TextureID_RoundedDome, RectCR( Yacht.Position, RadiusA ), ColorB * Alpha );
        DrawRect( Pass, TextureID_Dome,        RectCR( Yacht.Position, RadiusB ), ColorA * Alpha );
    }
    
    {
        vec4  bColor = COLOR_GRAY( 0.2f );
        int32 bLight = ( int32 )( Yacht.Light_Timer / YACHT_LIGHT_CYCLE_TARGET_TIME );
        
        int32 nLight = 12;
        for( int32 iLight = 0; iLight < nLight; iLight++ ) {
            flo32 t       = ( flo32 )iLight / ( flo32 )nLight;
            flo32 Radians = TAU * -t;
            flo32 Dist    = RadiusA * 0.85f;
            flo32 Radius  = RadiusA * 0.0675f;
            
            vec2  Pos     = Yacht.Position + ToDirection2D( Radians ) * Dist;
            
            vec4  Color   = bColor;
            if( Yacht.ShowDamage_Timer > 0.0f ) {
                int32 iCycle = ( int32 )( Yacht.ShowDamage_Timer / YACHT_SHOW_DAMAGE_CYCLE_TARGET_TIME );
                if( ( iCycle % 2 ) == 0 ) {
                    Color = Vec4( 0.8f, 0.0f, 0.0f, 1.0f );
                }
            } else {
                flo32 tColor = ( ( flo32 )( ( bLight + iLight ) % 4 ) ) / 3.0f;
                Color = lerp( Color, tColor, COLOR_WHITE );
            }
            
            DrawCircle( Pass, Pos, Radius, Color * Alpha );
        }
    }
    
    //DrawCircle( Pass, Yacht.Position, RadiusA, ColorA );
    //DrawCircle( Pass, Yacht.Position, RadiusB, ColorB );
    
    if( Yacht.Health > 0 ) {
        DrawEntityHealth( Pass, AppState, Camera, Yacht.Position, Yacht.Health, COLOR_BLACK );
    }
}

internal void
DrawBayDoors( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    BAY_DOOR_STATE * BayDoorS = Draw->BayDoorS;
    
    { // Draw Spawn Dest Art
        for( int32 iDest = 0; iDest < BAY_DOOR_SPAWN_DEST_MAX_COUNT; iDest++ ) {
            flo32 Alph = BayDoorS->BayDoorSpawn_DestAlpha[ iDest ];
            if( Alph > 0.0f ) {
                orect Bound = BayDoorS->BayDoorSpawn_Dest[ iDest ];
                Bound.HalfDim.y *= 3.0f;
                Bound.HalfDim.x -= 1.0f;
                
                vec2 P = GetT( Bound, Vec2( 0.0f, 0.0f ) );
                vec2 Q = GetT( Bound, Vec2( 1.0f, 0.0f ) );
                
                vec2  Dim        = Vec2( 3.0f, 3.0f );
                int32 nIndicator = ( int32 )( GetLength( Q - P ) / Dim.x );
                flo32 Radians    = ToRadians( Bound.xAxis );
                
                vec2  Dir   = GetNormal( Q - P );
                vec2  AtPos = ( P + Q ) * 0.5f - ( Dir * ( Dim.x * ( flo32 )nIndicator * 0.5f ) ) + Dir * ( Dim.x * 0.5f );
                
                orect Outer = ORectCD( ( P + Q ) * 0.5f, Dim * Vec2( ( flo32 )nIndicator, 1.0f ), Radians );
                DrawORect( Pass, Outer, COLOR_GRAY( 0.1f ) * 0.5f * Alph );
                
                flo32 tRadians = cosf( BayDoorS->BayDoorSpawn_Radians );
                vec2  Offset   = Bound.yAxis * 0.25f;
                
                
                for( int32 iIndicator = 0; iIndicator < nIndicator; iIndicator++ ) {
                    flo32 tRad = tRadians;
                    if( ( iIndicator % 2 ) == 0 ) {
                        tRad = -tRad;
                    }
                    
                    orect R     = ORectCD( AtPos + Offset * tRad, Dim * 0.35f, Radians );
                    flo32 tGray = 0.35f + 0.15f * tRad;
                    DrawORect( Pass, TextureID_BayDoors_SpawnDest, R, COLOR_GRAY( tGray ) * Alph );
                    
                    AtPos += ( Dir * Dim.x );
                }
            }
        }
    }
    
    { // Draw Yacht Exit
        {
            vec2 bPos = {};
            
            { // Mid Rects
                YACHT_EXIT_RING_DATA Ring = BayDoorS->YachtExit_MidRing;
                
                int32 nRect  = 10;
                flo32 Radius = Ring.Radius;
                
                // NOTE : tWidth  is the  t  of  MidRing.Radius  from  MidRing.InitRadius  to  MidRing.MinRadius
                flo32 tWidth = Clamp01( ( Ring.Radius - 0.35f ) / ( 1.65f ) );
                
                flo32 Width  = lerp( 0.65f, tWidth, 1.65f );
                vec2  Dim    = Vec2( Width, Width );
                
                for( int32 iRect = 0; iRect < nRect; iRect++ ) {
                    flo32 RadiansA = Ring.Radians + ( TAU / ( flo32 )nRect ) * ( flo32 )iRect;
                    flo32 RadiansB = ( ( PI ) / ( flo32 )nRect ) * ( flo32 )iRect;
                    vec2  Dir     = ToDirection2D( RadiansA );
                    vec2  Pos     = bPos + Dir * Radius;
                    
                    vec4  ColorA   = COLOR_GRAY( 0.075f );
                    vec4  ColorB   = Vec4( 0.2f, 0.2f, 0.1f, 1.0f );
                    if( ( iRect % 2 ) == 0 ) {
                        ColorA = COLOR_GRAY( 0.08f );
                        ColorB = Vec4( 0.25f, 0.25f, 0.15f, 1.0f );
                    }
                    
                    vec4 Color = lerp( ColorA, Ring.tColor, ColorB );
                    
                    if( BayDoorS->YachtExit_tFlash > 0.0f ) {
                        Color = lerp( Color, Clamp01( BayDoorS->YachtExit_tFlash ), COLOR_WHITE );
                    }
                    
                    Color *= Ring.Alpha;
                    orect Bound   = ORectCD( Pos, Dim, RadiansB );
                    DrawORect( Pass, Bound, Color );
                }
            }
            
            { // Inner Rect
                YACHT_EXIT_RING_DATA Ring = BayDoorS->YachtExit_InnerRing;
                
                flo32 Radius = Ring.Radius;
                vec4  Color  = lerp( COLOR_GRAY( 0.05f ), Ring.tColor, Vec4( 0.6f, 0.6f, 0.4f, 1.0f ) );
                
                if( BayDoorS->YachtExit_tFlash > 0.0f ) {
                    Color = lerp( Color, Clamp01( BayDoorS->YachtExit_tFlash ), COLOR_WHITE );
                }
                
                Color *= Ring.Alpha;
                orect Bound  = ORectCR( bPos, Radius, Ring.Radians );
                DrawORect( Pass, Bound, Color );
            }
        }
    }
    
    for( int32 iTransport = 0; iTransport < BayDoorS->nTransport; iTransport++ ) {
        TRANSPORT Transport = BayDoorS->Transport[ iTransport ];
        
        DrawTransport( Pass, AppState, Camera, Transport );
    }
    
    for( int32 iPersonalPod = 0; iPersonalPod < BayDoorS->nPersonalPod; iPersonalPod++ ) {
        PERSONAL_POD PersonalPod = BayDoorS->PersonalPod[ iPersonalPod ];
        
        DrawPersonalPod( Pass, PersonalPod );
    }
    
    for( int32 iYacht = 0; iYacht < BayDoorS->nYacht; iYacht++ ) {
        YACHT Yacht = BayDoorS->Yacht[ iYacht ];
        
        DrawYacht( Pass, AppState, Camera, Yacht );
    }
}

internal void
DrawBayDoorSpawn( RENDER_PASS * Pass, BAY_DOOR_SPAWN BayDoorSpawn, vec3 Light_Direction ) {
    //DrawORectOutline( Pass, BayDoorSpawn.Src,  COLOR_GREEN );
    //DrawORectOutline( Pass, BayDoorSpawn.Dest, COLOR_GREEN );
    
    flo32 Bottom          = 0.0f;
    flo32 Top             = 8.0f;
    
    { // HANGAR DOORS
        orect A = BayDoorSpawn.Src;
        A.HalfDim *= Vec2( 1.8f, 1.5f );
        
        flo32 Radians = ToRadians( A.xAxis );
        
        flo32 tGray = 0.1f;
        vec4 ColorA = COLOR_GRAY( tGray );
        
        vec4 tLight = {};
        
        {
            vec2 bPos = A.Center + A.yAxis * A.HalfDim.y;
            
            vec2 V = ToDirection2D( Radians - PI * 0.5f );
            vec3 N = GetNormal( Vec3( V, 100.0f ) );
            tLight = COLOR_GRAY( Clamp01( dot( N, -Light_Direction ) ) );
            
            orect B = ORectBCDY( bPos, A.HalfDim * Vec2( 5.0f, 6.0f ), A.yAxis );
            DrawORect( Pass, B, ColorA );
        }
        
        {
            vec2 Tri_Scale = Vec2( A.HalfDim.y * 2.0f, A.HalfDim.x * 0.75f );
            
            vec2 Tri_OffsetX = A.xAxis * ( A.HalfDim.x + Tri_Scale.y );
            vec2 Tri_OffsetY = A.yAxis * ( A.HalfDim.y );
            
            vec2 Tri_PosA = A.Center + Tri_OffsetX + Tri_OffsetY;
            vec2 Tri_PosB = A.Center - Tri_OffsetX + Tri_OffsetY;
            
            vec2 xAxis = ToDirection2D( Radians - PI * 0.5f );
            vec2 yAxis = GetPerp( xAxis );
            vec2 VertA = Tri_PosA + xAxis * Tri_Scale.x - yAxis * Tri_Scale.y;
            vec2 VertB = Tri_PosA                       + yAxis * Tri_Scale.y;
            vec2 VertC = Tri_PosA - xAxis * Tri_Scale.x - yAxis * Tri_Scale.y;
            
            xAxis = -xAxis;
            yAxis = -yAxis;
            
            vec2 VertD = Tri_PosB + xAxis * Tri_Scale.x - yAxis * Tri_Scale.y;
            vec2 VertE = Tri_PosB                       + yAxis * Tri_Scale.y;
            vec2 VertF = Tri_PosB - xAxis * Tri_Scale.x - yAxis * Tri_Scale.y;
            
            //DrawTriangleOutline( Pass, Tri_PosA, Tri_Scale, Radians - PI * 0.5f, COLOR_RED );
            //DrawTriangleOutline( Pass, Tri_PosB, Tri_Scale, Radians + PI * 0.5f, COLOR_RED );
            
            //DrawPoint( Pass, VertA, Vec2( 0.5f, 0.5f ), COLOR_RED   );
            //DrawPoint( Pass, VertB, Vec2( 0.5f, 0.5f ), COLOR_GREEN );
            //DrawPoint( Pass, VertC, Vec2( 0.5f, 0.5f ), COLOR_BLUE  );
            
            vec3 VA = Vec3( VertA, Bottom );
            vec3 VB = Vec3( VertB, Bottom );
            vec3 VC = Vec3( VertC, Top    );
            vec3 WA = Vec3( VertD, Bottom );
            vec3 WB = Vec3( VertE, Bottom );
            vec3 WC = Vec3( VertF, Top    );
            
            vec3 N = GetNormal( Cross( VB - VA, VC - VA ) );
            tLight = COLOR_GRAY( Clamp01( dot( N, -Light_Direction ) ) );
            
            DrawTriangle( Pass, Tri_PosA, Tri_Scale, Radians - PI * 0.5f, ColorA * tLight );
            
            N = GetNormal( Cross( WB - WA, WC - WA ) );
            DrawTriangle( Pass, Tri_PosB, Tri_Scale, Radians + PI * 0.5f, ColorA * tLight );
            
            N = GetNormal( Cross( VC - VA, WA - VA ) );
            tLight = COLOR_GRAY( Clamp01( dot( N, -Light_Direction ) ) );
        }
        
        {
            orect B = A;
            orect C = A;
            
            flo32 Dim     = A.HalfDim.x * 0.3f;
            flo32 HalfDim = Dim * 0.5f;
            flo32 DimInv  = A.HalfDim.x - Dim;
            
            vec2 Offset = A.xAxis * ( DimInv + HalfDim );
            
            B.Center += Offset;
            C.Center -= Offset;
            
            B.HalfDim.x = HalfDim;
            C.HalfDim.x = HalfDim;
            
            {
                vec2 P = GetT( C, Vec2( 0.0f, 1.0f ) );
                vec2 Q = GetT( B, Vec2( 1.0f, 1.0f ) );
                
                orect D = {};
                D.xAxis     = B.xAxis;
                D.yAxis     = B.yAxis;
                D.HalfDim.x = GetLength( Q - P ) * 0.5f;
                D.HalfDim.y = B.HalfDim.y;
                D.Center    = ( P + Q ) * 0.5f + D.yAxis * D.HalfDim.y;
                
                
                flo32 tAnimA = Clamp( BayDoorSpawn.BayDoors_AnimT, 0.0f, 0.95f );
                flo32 tAnimB = Clamp01( tAnimA * 2.0f );
                
                orect E = D;
                E.Center    -= E.yAxis * ( E.HalfDim.y * 2.0f );
                E.HalfDim.x *= 0.8f;
                E.Center    -= E.yAxis * ( 0.5f * tAnimB );
                E            = ORectAddDim( E, 0.0f, 0.0f, 0.0f, 1.25f );
                
                DrawORect( Pass, TextureID_BayDoors_Interior, E, COLOR_GRAY( 0.15f * tAnimA ) );
                
                orect F = ORectAddDim( E, 0.0f, 0.0f, 0.0f, -( E.HalfDim.y * 2.0f - 0.5f ) );
                DrawORect( Pass, F, ColorA * tLight );
                
                orect G = F;
                G.HalfDim.x *= 0.97f;
                G.HalfDim.y *= 0.8f;
                
                DrawORect( Pass, G, COLOR_BLACK );
                
                flo32 Length = G.HalfDim.x * 2.0f;
                
                if( ( BayDoorSpawn.Type != BayDoorEventType_Yacht ) && ( BayDoorSpawn.Stage != BayDoorEventStage_Wait ) && ( BayDoorSpawn.Stage != BayDoorEventStage_CloseBayDoors ) && ( BayDoorSpawn.Stage != BayDoorEventStage_Complete ) && ( BayDoorSpawn.Stage != BayDoorEventStage_EndLevel ) ) {
                    flo32 MaxTargetTime = 12.0f;
                    flo32 tA            = Clamp01( BayDoorSpawn.TargetTime / MaxTargetTime );
                    flo32 tB            = Clamp01( BayDoorSpawn.Timer / BayDoorSpawn.TargetTime );
                    
                    if( BayDoorSpawn.Type == BayDoorEventType_Transport ) {
                        tA = 1.0f;
                    }
                    
                    DISPLAY_VALUE( flo32, BayDoorSpawn.TargetTime );
                    DISPLAY_VALUE( flo32, tA );
                    DISPLAY_VALUE( flo32, tB );
                    
                    flo32 LengthA       = Length  * tA;
                    flo32 LengthB       = LengthA * tB;
                    
                    orect H = ORectAddDim( G, -( Length - LengthA ), -G.HalfDim.y, 0.0f, 0.0f );
                    orect I = ORectAddDim( G, -( Length - LengthA + LengthB ), 0.0f, 0.0f, -G.HalfDim.y );
                    
                    vec4 Color = Vec4( 0.5f, 0.5f, 0.0f, 1.0f );
                    if( BayDoorSpawn.Spawn_TargetTime == BAY_DOOR_SPAWN_PERSONAL_POD_TARGET_TIME_FAST ) {
                        Color = Vec4( 0.0f, 0.5f, 0.0f, 1.0f );
                    }
                    if( BayDoorSpawn.Type == BayDoorEventType_Transport ) {
                        Color = COLOR_GRAY( 0.5f );
                    }
                    
                    //DrawORect( Pass, H, Color );
                    DrawORect( Pass, I, Color );
                }
                
                DrawORect( Pass, D, ColorA * tLight );
            }
            
            DrawORect( Pass, B, ColorA * tLight );
            DrawORect( Pass, C, ColorA * tLight );
            
            {
                flo32 Slat_Dim     = DimInv / 3.0f;
                flo32 Slat_HalfDim = Slat_Dim * 0.5f;
                
                vec2 bPosA = B.Center - B.xAxis * ( B.HalfDim.x - Slat_HalfDim );
                vec2 bPosB = C.Center + C.xAxis * ( C.HalfDim.x - Slat_HalfDim );
                
                orect D = B;
                D.Center    = bPosA;
                D.HalfDim.x = Slat_HalfDim;
                
                orect SlatA = D;
                orect SlatB = D;
                orect SlatC = D;
                
                flo32 Anim_T = Clamp( BayDoorSpawn.BayDoors_AnimT, 0.0f, 0.95f );
                flo32 tXA = 1.0f - Anim_T;
                flo32 tXB = 1.0f - Anim_T;
                flo32 tXC = 1.0f - Anim_T;
                flo32 tYA = Anim_T;
                flo32 tYB = Anim_T;
                flo32 tYC = Anim_T;
                
                flo32 Slat_OffsetY = 0.025f;
                
                vec2 OffsetXA = SlatA.xAxis * ( SlatA.HalfDim.x * 2.0f * -tXA );
                vec2 OffsetXB = SlatB.xAxis * ( SlatB.HalfDim.x * 2.0f * -tXB );
                vec2 OffsetXC = SlatC.xAxis * ( SlatC.HalfDim.x * 2.0f * -tXC );
                vec2 OffsetYA = SlatA.yAxis * ( Slat_OffsetY         * -tYA );
                vec2 OffsetYB = SlatB.yAxis * ( Slat_OffsetY         * -tYB );
                vec2 OffsetYC = SlatC.yAxis * ( Slat_OffsetY         * -tYC );
                
                SlatA.Center = bPosA        + OffsetXA + OffsetYA;
                SlatB.Center = SlatA.Center + OffsetXB + OffsetYB;
                SlatC.Center = SlatB.Center + OffsetXC + OffsetYC;
                
                vec4 ColorB = lerp( ColorA, Anim_T, COLOR_GRAY( tGray + 0.01f ) );
                vec4 ColorC = lerp( ColorA, Anim_T, COLOR_GRAY( tGray + 0.03f ) );
                vec4 ColorD = lerp( ColorA, Anim_T, COLOR_GRAY( tGray + 0.05f ) );
                
                DrawORect( Pass, SlatA, ColorB * tLight );
                DrawORect( Pass, SlatB, ColorC * tLight );
                DrawORect( Pass, SlatC, ColorD * tLight );
                
                orect SlatD = SlatA;
                orect SlatE = SlatB;
                orect SlatF = SlatC;
                
                SlatD.Center = bPosB        - OffsetXA + OffsetYA;
                SlatE.Center = SlatD.Center - OffsetXB + OffsetYB;
                SlatF.Center = SlatE.Center - OffsetXC + OffsetYC;
                
                DrawORect( Pass, SlatD, ColorB * tLight );
                DrawORect( Pass, SlatE, ColorC * tLight );
                DrawORect( Pass, SlatF, ColorD * tLight );
            }
        }
    }
}

internal void
DrawBayDoorSpawn( RENDER_PASS * Pass, DRAW_STATE * Draw, vec3 Light_Direction ) {
    BAY_DOOR_STATE * BayDoorS = Draw->BayDoorS;
    
    for( int32 iBayDoorSpawn = 0; iBayDoorSpawn < BayDoorS->nBayDoorSpawn; iBayDoorSpawn++ ) {
        BAY_DOOR_SPAWN BayDoorSpawn = BayDoorS->BayDoorSpawn[ iBayDoorSpawn ];
        
        DrawBayDoorSpawn( Pass, BayDoorSpawn, Light_Direction );
    }
    
    for( int32 iBayDoorSpawn_Dest = 0; iBayDoorSpawn_Dest < BAY_DOOR_SPAWN_DEST_MAX_COUNT; iBayDoorSpawn_Dest++ ) {
        orect Dest = BayDoorS->BayDoorSpawn_Dest[ iBayDoorSpawn_Dest ];
        //DrawORectOutline( Pass, Dest, COLOR_GREEN );
    }
}

internal void
FinalizeBayDoors( APP_STATE * AppState ) {
    SCORE_STATE  * ScoreS  = &AppState->ScoreS;
    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
    
    { // PERSONAL_POD
        rect Remove_Bound = AddRadius( AppState->World_Bound, 2.0f );
        
        uint32 NewCount = 0;
        for( int32 iPersonalPod = 0; iPersonalPod < BayDoorS->nPersonalPod; iPersonalPod++ ) {
            PERSONAL_POD PersonalPod = BayDoorS->PersonalPod[ iPersonalPod ];
            
            if( PersonalPod.Health == 0 ) {
                AddKillToScore( AppState, EntityType_PersonalPod, PersonalPod.Position );
                
                POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_PersonalPod );
                int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                
                vec2 Pos = ClampToRect( PersonalPod.Position, AppState->World_Bound );
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                    AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                }
                
                PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Tiny );
            }
            if( ( !IsInRect( PersonalPod.Position, Remove_Bound ) ) && ( PersonalPod.Health > 0 ) ) {
                AddEntityToMissedCountForLevel( AppState, EntityType_PersonalPod, 1 );
            }
            
            if( ( IsInRect( PersonalPod.Position, Remove_Bound ) ) && ( PersonalPod.Health > 0 ) ) {
                BayDoorS->PersonalPod[ NewCount++ ] = PersonalPod;
            }
        }
        BayDoorS->nPersonalPod = NewCount;
    }
    
    { // TRANSPORT
        rect Remove_Bound = AddRadius( AppState->World_Bound, 8.0f );
        
        uint32 NewCount = 0;
        for( int32 iTransport = 0; iTransport < BayDoorS->nTransport; iTransport++ ) {
            TRANSPORT Transport = BayDoorS->Transport[ iTransport ];
            
            if( Transport.Health == 0 ) {
                AddKillToScore( AppState, EntityType_Transport, Transport.Position );
                
                POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Transport );
                int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                
                vec2 Pos = ClampToRect( Transport.Position, AppState->World_Bound );
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                    AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                }
                
                AddEntityDeathParticles( AppState, Transport.Position, TRANSPORT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Transport.Position, TRANSPORT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Transport.Position, TRANSPORT_BASE_COLOR );
                AddKillToWave( AppState, Transport.WaveGuid );
                
                PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Tiny );
            }
            if( ( !IsInRect( Transport.Position, Remove_Bound ) ) && ( Transport.Health > 0 ) ) {
                AddEntityToMissedCountForLevel( AppState, EntityType_Transport, 1 );
            }
            
            if( ( IsInRect( Transport.Position, Remove_Bound ) ) && ( Transport.Health > 0 ) ) {
                BayDoorS->Transport[ NewCount++ ] = Transport;
            } else {
                AddKillToWave( AppState, Transport.WaveGuid );
            }
        }
        BayDoorS->nTransport = NewCount;
    }
    
#if 0    
    { // MINIBOSS
        uint32 NewCount = 0;
        for( int32 iMiniBoss = 0; iMiniBoss < BayDoorS->nMiniBoss; iMiniBoss++ ) {
            MINIBOSS MiniBoss = BayDoorS->MiniBoss[ iMiniBoss ];
            
            if( MiniBoss.Health > 0 ) {
                BayDoorS->MiniBoss[ NewCount++ ] = MiniBoss;
            } else {
                AddKillToScore( AppState, EntityType_MiniBoss, MiniBoss.Position );
                AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( MiniBoss.Position, 175 ) );
                AddEntityDeathParticles( AppState, MiniBoss.Position, COLOR_GRAY( 0.8f ) );
                AddKillToWave( AppState, MiniBoss.WaveGuid );
            }
        }
        BayDoorS->nMiniBoss = NewCount;
    }
#endif
    
    { // YACHT
        rect Remove_Bound = AddRadius( AppState->World_Bound, 8.0f );
        
        uint32 NewCount = 0;
        for( int32 iYacht = 0; iYacht < BayDoorS->nYacht; iYacht++ ) {
            YACHT Yacht = BayDoorS->Yacht[ iYacht ];
            
            flo32 Speed = GetLength( Yacht.Velocity );
            
            boo32 DoRemove = false;
            if( ( Yacht.Health == 0 ) && ( Speed <= 1.0f ) ) {
                AddKillToScore( AppState, EntityType_Yacht, Yacht.Position );
                
                POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Yacht );
                int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                
                vec2 Pos = ClampToRect( Yacht.Position, AppState->World_Bound );
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                    AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                }
                
                AddEntityDeathParticles( AppState, Yacht.Position, YACHT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Yacht.Position, YACHT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Yacht.Position, YACHT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Yacht.Position, COLOR_WHITE );
                AddEntityDeathParticles( AppState, Yacht.Position, COLOR_RED );
                AddKillToWave( AppState, Yacht.WaveGuid );
                
                BayDoorS->BonusObjective_nYachtKills += 1;
                
                DoRemove = true;
                
                PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Small );
            }
            if( ( Yacht.Health > 0 ) && ( Yacht.Exit_Timer >= YACHT_EXIT_TARGET_TIME ) ) {
                AddEntityToMissedCountForLevel( AppState, EntityType_Yacht, 1 );
                
                BayDoorS->BonusObjective_nYachtMisses += 1;
                BayDoorS->YachtExit_tFlash             = 2.0f;
                
                DoRemove = true;
            }
            
            if( DoRemove ) {
                AddKillToWave( AppState, Yacht.WaveGuid );
            } else {
                BayDoorS->Yacht[ NewCount++ ] = Yacht;
            }
        }
        BayDoorS->nYacht = NewCount;
    }
}