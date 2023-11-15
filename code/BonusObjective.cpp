
internal void
UpdateBonusObjective( APP_STATE * AppState, flo32 dT ) {
    SCORE_STATE     * ScoreS       = &AppState->ScoreS;
    END_LEVEL       * EndLevel     = &AppState->EndLevel;
    OVERWORLD_LEVEL * CurrentLevel = &AppState->CurrentLevel.Level;
    
    if( ( !EndLevel->Show ) && ( !ScoreS->DoBonusObjective ) ) {
        boo32 DoBonusObjective = false;
        
        switch( AppState->CurrentLevel.LevelID ) {
            case LevelID_Blimp: {
                BLIMP_STATE * BlimpS = &AppState->BlimpS;
                if( BlimpS->BonusObjective_nBlimpKills >= 1 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_Turret: {
                RECON_STATE * ReconS = &AppState->ReconS;
                if( ReconS->nRecon <= 0 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_TurretShield: {
                TURRET_STATE * TurretS = &AppState->TurretS;
                if( TurretS->BonusObjective_nMegaTurretKills >= 1 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_BombLauncher: {
                BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
                
                if( BombLauncherS->BonusObjective_nBombLauncherKills >= BOMB_LAUNCHER_MAX_COUNT ) {
                    DoBonusObjective = true;
                } else {
                    if( ( BombLauncherS->BonusObjective_nBombLauncherSpawns > 0 ) && ( BombLauncherS->BonusObjective_nBombLauncherKills >= BombLauncherS->BonusObjective_nBombLauncherSpawns ) ) {
                        BombLauncherS->BonusObjective_MaxBombLauncherToSpawn = MinValue( BombLauncherS->BonusObjective_MaxBombLauncherToSpawn + 1, BOMB_LAUNCHER_MAX_COUNT );
                    }
                    BombLauncherS->BonusObjective_SpawnCooldownTimer += dT;
                    if( BombLauncherS->BonusObjective_SpawnCooldownTimer >= 1.0f ) {
                        while( BombLauncherS->BonusObjective_nBombLauncherSpawns < BombLauncherS->BonusObjective_MaxBombLauncherToSpawn ) {
                            Assert( BombLauncherS->BonusObjective_nBombLauncherSpawns < BOMB_LAUNCHER_MAX_COUNT );
                            
                            int32 nSelect = 0;
                            int32 iSelect[ BOMB_LAUNCHER_MAX_COUNT ] = {};
                            for( int32 iBombLauncher = 0; iBombLauncher < BOMB_LAUNCHER_MAX_COUNT; iBombLauncher++ ) {
                                BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iBombLauncher;
                                if( BombLauncher->Stage == BombLauncherStage_Wait ) {
                                    iSelect[ nSelect++ ] = iBombLauncher;
                                }
                            }
                            
                            int32 Select = 0;
                            if( nSelect > 1 ) {
                                Select = RandomS32InRange( 0, nSelect - 1 );
                            }
                            
                            BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iSelect[ Select ];
                            BombLauncher->Stage = BombLauncherStage_EnableBombLauncher;
                            
                            BombLauncherS->BonusObjective_nBombLauncherSpawns += 1;
                        }
                    }
                }
            } break;
            
            case LevelID_AutoTurret: {
                BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
                if( BayDoorS->BonusObjective_nYachtKills >= 3 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_PlayerShield: {
                MINER_STATE * MinerS = &AppState->MinerS;
                
                if( MinerS->BonusObjective_nMinerKills >= 4 ) {
                    DoBonusObjective = true;
                } else if( MinerS->BonusObjective_DoNext ) {
                    flo32 TargetTime = 2.0f;
                    MinerS->BonusObjective_DoNext_Timer += dT;
                    
                    if( MinerS->BonusObjective_DoNext_Timer >= TargetTime ) {
                        MinerS->BonusObjective_DoNext       = false;
                        MinerS->BonusObjective_DoNext_Timer = 0.0f;
                        
                        int32 nLaser = MinerS->BonusObjective_nMinerKills + 1;
                        AddEntitySpawn( AppState, GenValIdentity( -1, EntityType_Miner_Stationary, Vec2( 0.0f, 0.0f ), nLaser ) );
                    }
                }
            } break;
            
            case LevelID_MegaMiner: {
                MINER_STATE * MinerS = &AppState->MinerS;
                if( MinerS->BonusObjective_nMegaMinerKills >= 1 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_GravityWell: {
                SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
                if( SatelliteS->BonusObjective_nSatelliteKills >= 1 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_Worm: {
                WORM_STATE * WormS = &AppState->WormS;
                if( WormS->BonusObjective_nLargeWormKills >= 2 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_Collector: {
                SPITTER_STATE * SpitterS = &AppState->SpitterS;
                if( SpitterS->BonusObjective_nLoaderKills >= 1 ) {
                    DoBonusObjective = true;
                }
            } break;
            
            case LevelID_ObservationDeck: {
                BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
                OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                
                if( ObservationDeck->Health <= -1 ) {
                    DoBonusObjective = true;
                }
            } break;
        }
        
        if( DoBonusObjective ) {
            ScoreS->DoBonusObjective = true;
            ClearActiveWaveArrayToEndLevel( AppState );
        }
    }
}