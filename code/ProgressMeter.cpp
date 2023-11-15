
internal void
UpdateProgressMeter( APP_STATE * AppState, flo32 dT ) {
    END_LEVEL * EndLevel = &AppState->EndLevel;
    if( !EndLevel->Show ) {
        PROGRESS_STATE   * ProgressS    = &AppState->ProgressS;
        PROGRESS_STAGE   * Stage        =  ProgressS->Stage;
        PROGRESS_STAGE   * Bonus        = &ProgressS->Bonus;
        PROGRESS_DISPLAY * Display      =  ProgressS->Display;
        PROGRESS_DISPLAY * BonusDisplay = &ProgressS->BonusDisplay;
        
        vec2 Dim_Small   = Vec2(  40.0f, UI_PANEL_HEIGHT * 0.6f );
        vec2 Dim_Medium  = Vec2(  80.0f, UI_PANEL_HEIGHT * 0.6f );
        vec2 Dim_Large   = Vec2( 120.0f, UI_PANEL_HEIGHT * 0.6f );
        vec2 Dim_XLarge  = Vec2( 180.0f, UI_PANEL_HEIGHT * 0.6f );
        vec2 Dim_Display = Vec2(  88.0f, UI_PANEL_HEIGHT - 8.0f );
        
        flo32 MarginX         = 8.0f;
        flo32 Advance_Small   = Dim_Small.x   + MarginX;
        flo32 Advance_Medium  = Dim_Medium.x  + MarginX;
        flo32 Advance_Large   = Dim_Large.x   + MarginX;
        flo32 Advance_XLarge  = Dim_XLarge.x  + MarginX;
        flo32 Advance_Display = Dim_Display.x + MarginX;
        
        vec2 Panel_bPos = Vec2( AppState->App_HalfDim.x, AppState->App_Bound.Top - ( UI_PANEL_HEIGHT * 0.5f ) );
        if( MatchString( AppState->CurrentLevel_FileName, "Level01a" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 1;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = BLIMP_HEALTH;
                
                Stage[ 0 ].IsActive = true;
            }
            
            if( !Bonus->IsActive ) {
                BLIMP_STATE * BlimpS = &AppState->BlimpS;
                if( BlimpS->nBlimp >= 1 ) {
                    Bonus->IsActive = true;
                }
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 114.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                BLIMP_STATE * BlimpS = &AppState->BlimpS;
                if( BlimpS->BonusObjective_nBlimpKills >= 1 ) {
                    Bonus->tCompletion = 1.0f;
                    Bonus->IsComplete  = true;
                } else {
                    if( BlimpS->nBlimp >= 1 ) {
                        BLIMP * Blimp = BlimpS->Blimp + 0;
                        
                        int32 Health       = Blimp->Health;
                        int32 TargetHealth = BLIMP_HEALTH;
                        
                        Bonus->tCompletion = 1.0f - Clamp01( ( flo32 )Health / ( flo32 )TargetHealth );
                        ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                        ProgressS->BonusDisplay.sValue = Health;
                        
                        if( Bonus->tCompletion >= 1.0f ) {
                            Bonus->IsComplete = true;
                        }
                    }
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level01b" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 2;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Stage  [ 1 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = LEVEL01B_BONUS_OBJECTIVE_RECON_COUNT;
                
                Stage[ 0 ].IsActive = true;
                Bonus->IsActive     = true;
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                SCORE_STATE * ScoreS = &AppState->ScoreS;
                SCORE_ENTITY_KILL_COUNT KillCount = ScoreS->EntityKillCount[ EntityType_Turret ];
                
                int32 nKills       = KillCount.nTotal - Stage[ 0 ].bCounter;
                int32 nTargetKills = 8;
                
                Stage[ 0 ].tCompletion  = Clamp01( ( flo32 )nKills / ( flo32 )nTargetKills );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Counter;
                ProgressS->Display[ 0 ].sValue = nTargetKills - nKills;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                    
                    Stage[ 1 ].IsActive = true;
                    Stage[ 1 ].bCounter = nTargetKills;
                }
            }
            if( ( Stage[ 1 ].IsActive ) && ( !Stage[ 1 ].IsComplete ) ) {
                SCORE_STATE * ScoreS = &AppState->ScoreS;
                SCORE_ENTITY_KILL_COUNT KillCount = ScoreS->EntityKillCount[ EntityType_Turret ];
                
                int32 nKills       = KillCount.nTotal - Stage[ 1 ].bCounter;
                int32 nTargetKills = 8;
                
                Stage[ 1 ].tCompletion  = Clamp01( ( flo32 )nKills / ( flo32 )nTargetKills );
                ProgressS->Display[ 1 ].Type   = ProgressDisplayType_Counter;
                ProgressS->Display[ 1 ].sValue = nTargetKills - nKills;
                
                if( Stage[ 1 ].tCompletion >= 1.0f ) {
                    Stage[ 1 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                RECON_STATE * ReconS = &AppState->ReconS;
                
                int32 nKills       = ReconS->BonusObjective_nReconKills;
                int32 nTargetKills = LEVEL01B_BONUS_OBJECTIVE_RECON_COUNT;
                
                Bonus->tCompletion = Clamp01( ( flo32 )nKills / ( flo32 )nTargetKills );
                ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                ProgressS->BonusDisplay.sValue = nTargetKills - nKills;
                
                if( Bonus->tCompletion >= 1.0f ) {
                    Bonus->IsComplete = true;
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level02a" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 2;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Small   );  AtPos.x += Advance_Small;
                Stage  [ 1 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = MEGA_TURRET_HEALTH;
                
                Stage[ 0 ].IsActive = true;
            }
            
            if( !Bonus->IsActive ) {
                TURRET_STATE * TurretS = &AppState->TurretS;
                if( TurretS->nMegaTurret >= 1 ) {
                    Bonus->IsActive = true;
                }
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                SCORE_STATE * ScoreS = &AppState->ScoreS;
                SCORE_ENTITY_KILL_COUNT KillCount = ScoreS->EntityKillCount[ EntityType_Turret ];
                
                int32 nKills       = KillCount.nTotal - Stage[ 0 ].bCounter;
                int32 nTargetKills = 2;
                
                Stage[ 0 ].tCompletion  = Clamp01( ( flo32 )nKills / ( flo32 )nTargetKills );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Counter;
                ProgressS->Display[ 0 ].sValue = nTargetKills - nKills;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                    
                    Stage[ 1 ].IsActive = true;
                    
                    InitPowerups( AppState, 128.0f );
                }
            }
            if( ( Stage[ 1 ].IsActive ) && ( !Stage[ 1 ].IsComplete ) ) {
                Stage[ 1 ].Timer += dT;
                flo32 TargetTime  = 128.0f;
                
                Stage[ 1 ].tCompletion = Clamp01( Stage[ 1 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 1 ].Timer;
                
                if( Stage[ 1 ].tCompletion >= 1.0f ) {
                    Stage[ 1 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                TURRET_STATE * TurretS = &AppState->TurretS;
                if( TurretS->nMegaTurret >= 1 ) {
                    TURRET * MegaTurret = TurretS->MegaTurret + 0;
                    
                    int32 Health       = MegaTurret->Health;
                    int32 TargetHealth = MEGA_TURRET_HEALTH;
                    
                    Bonus->tCompletion = 1.0f - Clamp01( ( flo32 )Health / ( flo32 )TargetHealth );
                    ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                    ProgressS->BonusDisplay.sValue = Health;
                    
                    if( Bonus->tCompletion >= 1.0f ) {
                        Bonus->IsComplete = true;
                    }
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level02b" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 1;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = BOMB_LAUNCHER_MAX_COUNT;
                
                Stage[ 0 ].IsActive = true;
                Bonus->IsActive     = true;
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 126.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
                
                int32 nKills       = BombLauncherS->BonusObjective_nBombLauncherKills - 2;
                int32 nTargetKills = BOMB_LAUNCHER_MAX_COUNT - 2;
                
                Bonus->tCompletion = Clamp01( ( flo32 )nKills / ( flo32 )nTargetKills );
                ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                ProgressS->BonusDisplay.sValue = nTargetKills - nKills;
                
                if( Bonus->tCompletion >= 1.0f ) {
                    Bonus->IsComplete = true;
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level03a" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 1;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = 3;
                
                Stage[ 0 ].IsActive = true;
                Bonus->IsActive     = true;
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
                
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = BayDoorS->SpawnPersonalPod_TargetTimeForLevel;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    int32 nEntities = CountTotalEntitiesOnScreen( AppState );
                    if( nEntities > 0 ) {
                        ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Wait;
                        ProgressS->Display[ 0 ].Timer += dT;
                    } else {
                        Stage[ 0 ].IsComplete = true;
                    }
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
                
                int32 nMisses      = BayDoorS->BonusObjective_nYachtMisses;
                int32 nKills       = BayDoorS->BonusObjective_nYachtKills;
                int32 nTargetKills = 3;
                
                Bonus->tCompletion = Clamp01( ( flo32 )nKills / ( flo32 )nTargetKills );
                if( nMisses >= 3 ) {
                    Bonus->IsFailed = true;
                }
                
                ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                ProgressS->BonusDisplay.sValue = nTargetKills - nKills;
                
                if( Bonus->tCompletion >= 1.0f ) {
                    Bonus->IsComplete = true;
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level04a" ) ) {
            vec2 DimA = Vec2( 240.0f, UI_PANEL_HEIGHT * 0.3f );
            vec2 DimB = Vec2(  48.0f, UI_PANEL_HEIGHT * 0.3f );
            vec2 DimC = Vec2(  16.0f, UI_PANEL_HEIGHT * 0.1f );
            
            DimB.x = ( DimA.x - ( DimC.x * 4.0f + MarginX * 7.0f ) ) * 0.25f;
            
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos  = Panel_bPos + Vec2( -400.0f, 0.0f );
                vec2 AtPosA = AtPos + Vec2( 0.0f,  UI_PANEL_HEIGHT * 0.2f );
                vec2 AtPosB = AtPos + Vec2( 0.0f, -UI_PANEL_HEIGHT * 0.2f );
                
                ProgressS->nStage = 9;
                Stage  [ 0 ].Bound  = RectLCD( AtPosA, DimA );  AtPosA.x += DimA.x + MarginX;
                
                Stage  [ 1 ].Bound  = RectLCD( AtPosB, DimC );  AtPosB.x += DimC.x + MarginX;
                Stage  [ 2 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                Stage  [ 3 ].Bound  = RectLCD( AtPosB, DimC );  AtPosB.x += DimC.x + MarginX;
                Stage  [ 4 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                Stage  [ 5 ].Bound  = RectLCD( AtPosB, DimC );  AtPosB.x += DimC.x + MarginX;
                Stage  [ 6 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                Stage  [ 7 ].Bound  = RectLCD( AtPosB, DimC );  AtPosB.x += DimC.x + MarginX;
                Stage  [ 8 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                
                AtPos.x += DimA.x + MarginX;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                Display[ 1 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = 4;
                
                Stage[ 0 ].IsActive = true;
                Stage[ 1 ].IsActive = true;
                Bonus->IsActive     = true;
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 140.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            
            flo32 Stage_TargetTime[ 16 ] = { FLT_MAX,
                8.0f, 20.0f, 12.0f, 20.0f, 12.0f, 20.0f, 12.0f, 24.0f
            };
            
            for( int32 iStage = 1; iStage < ProgressS->nStage; iStage++ ) {
                if( ( Stage[ iStage ].IsActive ) && ( !Stage[ iStage ].IsComplete ) ) {
                    Stage[ iStage ].Timer += dT;
                    flo32 TargetTime  = Stage_TargetTime[ iStage ];
                    
                    Stage[ iStage ].tCompletion = Clamp01( Stage[ iStage ].Timer / TargetTime );
                    if( ( iStage % 2 ) == 0 ) {
                        ProgressS->Display[ 1 ].Type   = ProgressDisplayType_Timer;
                        ProgressS->Display[ 1 ].fValue = TargetTime - Stage[ iStage ].Timer;
                    } else {
                        ProgressS->Display[ 1 ].Type   = ProgressDisplayType_Hidden;
                    }
                    
                    if( Stage[ iStage ].tCompletion >= 1.0f ) {
                        Stage[ iStage     ].IsComplete = true;
                        Stage[ iStage + 1 ].IsActive   = true;
                    }
                }
            }
            
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                MINER_STATE * MinerS = &AppState->MinerS;
                
                int32 nKills       = MinerS->BonusObjective_nMinerKills;
                int32 nTargetKills = 4;
                
                Bonus->tCompletion = Clamp01( ( flo32 )nKills / ( flo32 )nTargetKills );
                
                ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                ProgressS->BonusDisplay.sValue = nTargetKills - nKills;
                
                if( Bonus->tCompletion >= 1.0f ) {
                    Bonus->IsComplete = true;
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level04b" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 1;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = MEGA_MINER_HEALTH;
                
                Stage[ 0 ].IsActive = true;
            }
            
            if( !Bonus->IsActive ) {
                MINER_STATE * MinerS = &AppState->MinerS;
                if( MinerS->nMegaMiner >= 1 ) {
                    Bonus->IsActive = true;
                }
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 148.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                MINER_STATE * MinerS = &AppState->MinerS;
                if( MinerS->BonusObjective_nMegaMinerKills >= 1 ) {
                    Bonus->tCompletion = 1.0f;
                    Bonus->IsComplete  = true;
                } else {
                    if( MinerS->nMegaMiner >= 1 ) {
                        MEGA_MINER * MegaMiner = MinerS->MegaMiner + 0;
                        
                        int32 Health       = MegaMiner->Health;
                        int32 TargetHealth = MEGA_MINER_HEALTH;
                        
                        Bonus->tCompletion = 1.0f - Clamp01( ( flo32 )Health / ( flo32 )TargetHealth );
                        ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                        ProgressS->BonusDisplay.sValue = Health;
                        
                        if( Bonus->tCompletion >= 1.0f ) {
                            Bonus->IsComplete = true;
                        }
                    }
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level05" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 1;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = SATELLITE_HEALTH;
                
                Stage[ 0 ].IsActive = true;
                Bonus->IsActive     = true;
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 126.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
                if( SatelliteS->BonusObjective_nSatelliteKills >= 1 ) {
                    Bonus->tCompletion = 1.0f;
                    Bonus->IsComplete  = true;
                } else {
                    if( SatelliteS->nSatellite >= 1 ) {
                        SATELLITE * Satellite = SatelliteS->Satellite + 0;
                        
                        int32 Health       = Satellite->Health;
                        int32 TargetHealth = SATELLITE_HEALTH;
                        
                        Bonus->tCompletion = 1.0f - Clamp01( ( flo32 )Health / ( flo32 )TargetHealth );
                        ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                        ProgressS->BonusDisplay.sValue = Health;
                        
                        if( Bonus->tCompletion >= 1.0f ) {
                            Bonus->IsComplete = true;
                        }
                    }
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level07" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 1;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = WORM_HEALTH_LARGE * 2;
                
                Stage[ 0 ].IsActive = true;
            }
            
            if( !Bonus->IsActive ) {
                WORM_STATE * WormS = &AppState->WormS;
                if( WormS->nWorm >= 1 ) {
                    Bonus->IsActive = true;
                }
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 152.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                WORM_STATE * WormS = &AppState->WormS;
                
                int32 Health       = WORM_HEALTH_LARGE * WormS->BonusObjective_nLargeWormKills;
                int32 TargetHealth = WORM_HEALTH_LARGE * 2;
                
                for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
                    WORM Worm = WormS->Worm[ iWorm ];
                    if( Worm.Class == WormClass_Large ) {
                        Health += ( WORM_HEALTH_LARGE - Worm.Health );
                    }
                }
                
                Bonus->tCompletion = Clamp01( ( flo32 )Health / ( flo32 )TargetHealth );
                ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                ProgressS->BonusDisplay.sValue = TargetHealth - Health;
                
                if( Bonus->tCompletion >= 1.0f ) {
                    Bonus->IsComplete = true;
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level08" ) ) {
            vec2 DimA = Vec2( 240.0f, UI_PANEL_HEIGHT * 0.3f );
            vec2 DimB = {};
            DimB.x = ( DimA.x - MarginX * 3.0f ) / 4.0f;
            DimB.y = UI_PANEL_HEIGHT * 0.3f;
            
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos  = Panel_bPos + Vec2( -400.0f, 0.0f );
                vec2 AtPosA = AtPos + Vec2( 0.0f,  UI_PANEL_HEIGHT * 0.2f );
                vec2 AtPosB = AtPos + Vec2( 0.0f, -UI_PANEL_HEIGHT * 0.2f );
                
                ProgressS->nStage = 5;
                Stage  [ 0 ].Bound  = RectLCD( AtPosA, DimA );  AtPosA.x += DimA.x + MarginX;
                
                Stage  [ 1 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                Stage  [ 2 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                Stage  [ 3 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                Stage  [ 4 ].Bound  = RectLCD( AtPosB, DimB );  AtPosB.x += DimB.x + MarginX;
                
                AtPos.x += DimA.x + MarginX;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                Display[ 1 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = LOADER_HEALTH;
                
                Stage[ 0 ].IsActive = true;
                Stage[ 1 ].IsActive = true;
            }
            
            if( !Bonus->IsActive ) {
                SPITTER_STATE * SpitterS = &AppState->SpitterS;
                if( SpitterS->nLoader >= 1 ) {
                    Bonus->IsActive = true;
                }
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 40.0f * 4.0f + 2.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            
            flo32 Stage_TargetTime[ 16 ] = { FLT_MAX,
                42.0f, 40.0f, 40.0f, 40.0f,
            };
            
            for( int32 iStage = 1; iStage < ProgressS->nStage; iStage++ ) {
                if( ( Stage[ iStage ].IsActive ) && ( !Stage[ iStage ].IsComplete ) ) {
                    Stage[ iStage ].Timer += dT;
                    flo32 TargetTime  = Stage_TargetTime[ iStage ];
                    
                    Stage[ iStage ].tCompletion = Clamp01( Stage[ iStage ].Timer / TargetTime );
                    
                    ProgressS->Display[ 1 ].Type   = ProgressDisplayType_Timer;
                    ProgressS->Display[ 1 ].fValue = TargetTime - Stage[ iStage ].Timer;
                    
                    if( Stage[ iStage ].tCompletion >= 1.0f ) {
                        Stage[ iStage     ].IsComplete = true;
                        Stage[ iStage + 1 ].IsActive   = true;
                    }
                }
            }
            
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                SPITTER_STATE * SpitterS = &AppState->SpitterS;
                if( SpitterS->BonusObjective_nLoaderKills >= 1 ) {
                    Bonus->tCompletion = 1.0f;
                    Bonus->IsComplete  = true;
                } else {
                    if( SpitterS->nLoader >= 1 ) {
                        LOADER * Loader = SpitterS->Loader + 0;
                        
                        int32 Health       = Loader->Health;
                        int32 TargetHealth = LOADER_HEALTH;
                        
                        Bonus->tCompletion = 1.0f - Clamp01( ( flo32 )Health / ( flo32 )TargetHealth );
                        ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                        ProgressS->BonusDisplay.sValue = Health;
                        
                        if( Bonus->tCompletion >= 1.0f ) {
                            Bonus->IsComplete = true;
                        }
                    }
                }
            }
        }
        if( MatchString( AppState->CurrentLevel_FileName, "Level09" ) ) {
            if( !ProgressS->IsInitialized ) {
                ProgressS->IsInitialized = true;
                
                vec2 AtPos = Panel_bPos + Vec2( -400.0f, 0.0f );
                
                ProgressS->nStage = 1;
                Stage  [ 0 ].Bound  = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                Display[ 0 ].Bound  = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display + 200.0f;
                Bonus->Bound        = RectLCD( AtPos, Dim_Medium  );  AtPos.x += Advance_Medium;
                BonusDisplay->Bound = RectLCD( AtPos, Dim_Display );  AtPos.x += Advance_Display;
                
                BonusDisplay->Type   = ProgressDisplayType_Counter;
                BonusDisplay->sValue = OBSERVATION_DECK_HEALTH;
                
                Stage[ 0 ].IsActive = true;
                Bonus->IsActive     = true;
            }
            
            if( ( Stage[ 0 ].IsActive ) && ( !Stage[ 0 ].IsComplete ) ) {
                Stage[ 0 ].Timer += dT;
                flo32 TargetTime  = 180.0f;
                
                Stage[ 0 ].tCompletion = Clamp01( Stage[ 0 ].Timer / TargetTime );
                ProgressS->Display[ 0 ].Type   = ProgressDisplayType_Timer;
                ProgressS->Display[ 0 ].fValue = TargetTime - Stage[ 0 ].Timer;
                
                if( Stage[ 0 ].tCompletion >= 1.0f ) {
                    Stage[ 0 ].IsComplete = true;
                }
            }
            if( ( Bonus->IsActive ) && ( !Bonus->IsComplete ) ) {
                BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
                OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                
                int32 Health       = ObservationDeck->Health + 1;
                int32 TargetHealth = OBSERVATION_DECK_HEALTH + 1;
                
                Bonus->tCompletion = 1.0f - Clamp01( ( flo32 )Health / ( flo32 )TargetHealth );
                if( ObservationDeck->Health > 0 ) {
                    ProgressS->BonusDisplay.Type   = ProgressDisplayType_Counter;
                    ProgressS->BonusDisplay.sValue = ObservationDeck->Health;
                } else {
                    ProgressS->BonusDisplay.Type   = ProgressDisplayType_Message;
                    strcpy( ProgressS->BonusDisplay.Message, "!!!" );
                }
                
                if( Bonus->tCompletion >= 1.0f ) {
                    Bonus->IsComplete = true;
                }
            }
        }
    }
}

internal void
DrawProgressMeter( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    PROGRESS_STATE * ProgressS = Draw->ProgressS;
    
    int32 nStage = ProgressS->nStage;
    ProgressS->Stage[ nStage++ ] = ProgressS->Bonus;
    
    int32 nDisplay = 0;
    for( int32 iStage = 0; iStage < nStage; iStage++ ) {
        PROGRESS_STAGE Stage = ProgressS->Stage[ iStage ];
        vec4 OutlineColor    = COLOR_GRAY( 0.1f );
        vec4 BackgroundColor = COLOR_GRAY( 0.15f );
        vec4 CompleteColor   = Vec4( 0.5f, 0.5f, 0.0f, 1.0f );
        
        if( ( Stage.IsActive ) && ( !Stage.IsComplete ) ) {
            OutlineColor    = Vec4( 0.0f, 0.45f, 0.0f, 1.0f );
            BackgroundColor = COLOR_GRAY( 0.2f );
            CompleteColor   = Vec4( 0.85f, 0.85f, 0.0f, 1.0f );
        }
        if( Stage.IsComplete ) {
            OutlineColor    = Vec4( 0.35f, 0.35f, 0.0f, 1.0f );
        }
        
        rect BoundA = Stage.Bound;
        rect BoundB = AddRadius( BoundA, -2.0f );
        
        if( GetHeight( BoundA ) <= UI_PANEL_HEIGHT * 0.1f ) {
            BoundB = BoundA;
        }
        
        rect BoundC = BoundB;
        BoundC.Right = lerp( BoundC.Left, Stage.tCompletion, BoundC.Right );
        
        DrawRect( Pass, BoundA, OutlineColor );
        DrawRect( Pass, BoundB, BackgroundColor );
        DrawRect( Pass, BoundC, CompleteColor );
        
        if( Stage.IsFailed ) {
            orect A = ORectPP( GetBL( BoundB ), GetTR( BoundB ), 2.0f );
            orect B = ORectPP( GetTL( BoundB ), GetBR( BoundB ), 2.0f );
            
            A.HalfDim.y *= 0.9f;
            B.HalfDim.y *= 0.9f;
            
            DrawORect( Pass, A, Vec4( 0.75f, 0.0f, 0.0f, 1.0f ) );
            DrawORect( Pass, B, Vec4( 0.75f, 0.0f, 0.0f, 1.0f ) );
        }
        
        if( ( Stage.IsActive ) && ( !Stage.IsComplete ) ) {
            nDisplay += 1;
        }
    }
    
    // NOTE: This weird code is here so that the Bonus Stage and Display can use the same drawing code as the other stages, but still meet its weird requirements. The Bonus Stage might not always be active, but the Display will always be active and drawn.
    if( !ProgressS->Bonus.IsActive ) {
        nDisplay++;
    }
    ProgressS->Display[ nDisplay - 1 ] = ProgressS->BonusDisplay;
    
    for( int32 iDisplay = 0; iDisplay < nDisplay; iDisplay++ ) {
        PROGRESS_DISPLAY Display = ProgressS->Display[ iDisplay ];
        
        vec4 OutlineColor    = COLOR_GRAY( 0.05f );
        vec4 BackgroundColor = COLOR_GRAY( 0.075f );
        
        rect BoundA = Display.Bound;
        rect BoundB = AddRadius( BoundA, -2.0f );
        
        if( Display.Type != ProgressDisplayType_Hidden ) {
            DrawRect( Pass, BoundA, OutlineColor );
            DrawRect( Pass, BoundB, BackgroundColor );
            
            char Str[ 16 ] = {};
            vec4 Color     = COLOR_WHITE;
            
            switch( Display.Type ) {
                case ProgressDisplayType_Wait: {
                    sprintf( Str, "-WAIT-" );
                    flo32 Radians = Display.Timer * ( TAU / 2.2f );
                    flo32 t       = cosf( Radians ) * 0.5f + 0.5f;
                    flo32 Alpha   = lerp( 0.25f, t, 1.0f );
                    Color *= Alpha;
                } break;
                
                case ProgressDisplayType_Counter: {
                    sprintf( Str, "%d", Display.sValue );
                } break;
                
                case ProgressDisplayType_Timer: {
                    int32 Minutes = ( ( int32 )Display.fValue / 60 );
                    flo32 Seconds = Display.fValue;
                    while( Seconds >= 60.0f ) {
                        Seconds -= 60.0f;
                    }
                    
                    Assert( Minutes < 10 );
                    sprintf( Str, "%1u:%04.1f", Minutes, Seconds );
                } break;
                
                case ProgressDisplayType_Message: {
                    sprintf( Str, "%s", Display.Message );
                } break;
            }
            
            DrawString( Pass, AppState->FontList + FontID_1_50, Str, GetCenter( Display.Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), Color );
        }
    }
}