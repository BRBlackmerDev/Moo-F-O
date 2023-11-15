
internal void
UpdateEndLevel( KEYBOARD_STATE * Keyboard, MOUSE_STATE * Mouse, APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE * Player   = &AppState->Player;
    END_LEVEL    * EndLevel = &AppState->EndLevel;
    
    if( EndLevel->Show ) {
        EndLevel->FadeIn_Timer += dT;
        
        { // SHOW STATS
            flo32 Offset = GetHeight( AppState->World_Bound ) / 3.0f;
            if( Player->Position.x >= ( AppState->World_Bound.Right - Offset ) ) {
                EndLevel->ShowStats       = true;
                EndLevel->ShowStats_Timer = 0.0f;
            } else {
                EndLevel->ShowStats        = false;
                flo32 ShowStats_TargetTime = ( 48.0f / 60.0f );
                EndLevel->ShowStats_Timer += dT;
                if( EndLevel->ShowStats_Timer >= ShowStats_TargetTime ) {
                    EndLevel->ShowStats_Timer -= ( ShowStats_TargetTime * 2.0f );
                }
            }
        }
        
        if( EndLevel->EndLevelAction_Timer < ( END_LEVEL_BUTTON_TARGET_TIME - ( 12.0f / 60.0f ) ) ) {
            if( ( EndLevel->EndLevelAction == EndLevelAction_ExitToOverworld ) && ( WasReleased( Mouse, MouseButton_Left ) ) ) {
                EndLevel->EndLevelAction = EndLevelAction_Null;
            }
            if( ( EndLevel->EndLevelAction == EndLevelAction_ReplayLevel ) && ( WasReleased( Mouse, MouseButton_Right ) ) ) {
                EndLevel->EndLevelAction = EndLevelAction_Null;
            }
            if( ( EndLevel->EndLevelAction == EndLevelAction_ExitToDesktop ) && ( WasReleased( Keyboard, KeyCode_Escape ) ) ) {
                EndLevel->EndLevelAction = EndLevelAction_Null;
            }
        }
        if( EndLevel->EndLevelAction == EndLevelAction_Null ) {
            EndLevel->EndLevelAction_Timer = 0.0f;
            if( WasPressed( Mouse, MouseButton_Left ) ) {
                EndLevel->EndLevelAction = EndLevelAction_ExitToOverworld;
            }
            if( WasPressed( Mouse, MouseButton_Right ) ) {
                EndLevel->EndLevelAction = EndLevelAction_ReplayLevel;
            }
            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                EndLevel->EndLevelAction = EndLevelAction_ExitToDesktop;
            }
        }
        if( EndLevel->EndLevelAction == EndLevelAction_Null ) {
            flo32 dVolume = 1.0f / 0.5f;
            ChangeVolumeOfCurrentMusicTrack( AppState, dVolume * dT );
            
            flo32 dAlpha  = 1.0f / 0.5f;
            AppState->FadeOut_Alpha = Clamp01( AppState->FadeOut_Alpha + dAlpha * dT );
        } else {
            EndLevel->EndLevelAction_Timer += dT;
            
            // NOTE : This line is if the player presses a button while the end level screen is fading in, then we fill the timer to make the end level screen full alpha.
            EndLevel->FadeIn_Timer          = ( END_LEVEL_FADE_IN_DELAY_TARGET_TIME + END_LEVEL_FADE_IN_TARGET_TIME );
            
            if( ( EndLevel->EndLevelAction == EndLevelAction_ExitToOverworld ) || ( EndLevel->EndLevelAction == EndLevelAction_ExitToDesktop ) ) {
                flo32 FadeOut_TargetTime = ( 30.0f / 60.0f );
                if( EndLevel->EndLevelAction_Timer >= ( END_LEVEL_BUTTON_TARGET_TIME - FadeOut_TargetTime ) ) {
                    flo32 dVolume = 1.0f / FadeOut_TargetTime;
                    ChangeVolumeOfCurrentMusicTrack( AppState, -dVolume * dT );
                    
                    flo32 dAlpha  = 1.0f / 0.5f;
                    AppState->FadeOut_Alpha = Clamp01( AppState->FadeOut_Alpha - dAlpha * dT );
                }
            }
        }
        
        
#if 0        
        boo32 CanReplay = ( !EndLevel->DoExitToOverworld )
            && ( !EndLevel->DoExitToDesktop );
        boo32 CanExitToOverworld = ( !EndLevel->DoReplay )
            && ( !EndLevel->DoExitToDesktop );
        boo32 CanExitToDesktop = ( !EndLevel->DoReplay )
            && ( !EndLevel->DoExitToOverworld );
        
        if( ( CanExitToOverworld ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
            EndLevel->DoExitToOverworld = true;
            
            CanReplay        = false;
            CanExitToDesktop = false;
            
            EndLevel->FadeIn_Timer = ( END_LEVEL_FADE_IN_DELAY_TARGET_TIME + END_LEVEL_FADE_IN_TARGET_TIME );
        }
        if( WasReleased( Mouse, MouseButton_Left ) ) {
            EndLevel->DoExitToOverworld       = false;
            EndLevel->DoExitToOverworld_Timer = 0.0f;
        }
        
        if( ( CanReplay ) && ( WasPressed( Mouse, MouseButton_Right ) ) ) {
            EndLevel->DoReplay = true;
            
            CanExitToDesktop   = false;
            
            EndLevel->FadeIn_Timer = ( END_LEVEL_FADE_IN_DELAY_TARGET_TIME + END_LEVEL_FADE_IN_TARGET_TIME );
        }
        if( WasReleased( Mouse, MouseButton_Right ) ) {
            EndLevel->DoReplay       = false;
            EndLevel->DoReplay_Timer = 0.0f;
        }
        
        if( ( CanExitToDesktop ) && ( WasPressed( Keyboard, KeyCode_Escape ) ) ) {
            EndLevel->DoExitToDesktop = true;
            
            EndLevel->FadeIn_Timer = ( END_LEVEL_FADE_IN_DELAY_TARGET_TIME + END_LEVEL_FADE_IN_TARGET_TIME );
        }
        if( WasReleased( Keyboard, KeyCode_Escape ) ) {
            EndLevel->DoExitToDesktop       = false;
            EndLevel->DoExitToDesktop_Timer = 0.0f;
        }
        
        if( EndLevel->DoExitToOverworld ) {
            EndLevel->DoExitToOverworld_Timer += dT;
        }
        if( EndLevel->DoReplay ) {
            EndLevel->DoReplay_Timer += dT;
        }
        if( EndLevel->DoExitToDesktop ) {
            EndLevel->DoExitToDesktop_Timer += dT;
        }
#endif
    } else {
        SCORE_STATE * ScoreS = &AppState->ScoreS;
        if( ScoreS->Level_DisplayTimer_DoUpdate ) {
            ScoreS->Level_DisplayTimer = MaxValue( ScoreS->Level_DisplayTimer - dT, 0.0f );
        }
        ScoreS->Level_PlayTimer += dT;
        
        AppState->FadeOut_Alpha = 1.0f;
    }
}

internal void
DrawBarForBonusMultiplier( RENDER_PASS * Pass, rect Bound, flo32 Outline_Width, flo32 Percent, boo32 DoFlash = false, flo32 Flash_Radians = 0.0f ) {
    vec4 Background_Color = COLOR_GRAY( 0.05f );
    vec4 Outline_Color    = lerp( COLOR_GRAY( 0.2f ), Percent, COLOR_WHITE );
    vec4 Bar_Color        = COLOR_YELLOW;
    
    if( DoFlash ) {
        flo32 tFlash = cosf( Flash_Radians ) * 0.5f + 0.5f;
        flo32 tBlend = GetT01( Percent, 0.4f, 0.9f );
        Bar_Color = lerp( Bar_Color, tBlend, TowardWhite( Bar_Color, tFlash ) );
    }
    
    rect Inner_Bound = AddRadius( Bound, -Outline_Width );
    rect Bar_Bound   = Inner_Bound;
    Bar_Bound.Left = lerp( Bar_Bound.Right, Percent, Bar_Bound.Left );
    
    DrawRect( Pass, Bound,       Outline_Color );
    DrawRect( Pass, Inner_Bound, Background_Color );
    DrawRect( Pass, Bar_Bound,   Bar_Color );
}

internal void
DrawScoreRating( RENDER_PASS * Pass, int32 Rating, vec2 Center, vec2 Dim, flo32 OffsetBetween ) {
    int32 MaxRating = SCORE_RATING_MAX_RATING;
    
    vec2 AtPos = Center;
    AtPos.x -= ( ( Dim.x + OffsetBetween ) * 2.0f );
    
    vec4 Outline_Color = COLOR_BLACK;
    vec4 Inner_Color   = COLOR_GRAY( 0.75f );
    
    vec2 Scale = Dim * 0.5f;
    
    for( int32 iRating = 0; iRating < MaxRating; iRating++ ) {
        if( iRating <= ( Rating - 1 ) ) {
            DrawTriangle( Pass, AtPos, Scale, 0.0f, Inner_Color );
        } else {
            DrawTriangle( Pass, AtPos, Scale, 0.0f, Outline_Color );
            DrawTriangleOutline( Pass, AtPos, Scale, 0.0f, Inner_Color );
        }
        
        AtPos.x += ( Dim.x + OffsetBetween );
    }
}

internal void
DrawScoreThresholdColonScoreRating( RENDER_PASS * Pass, FONT * Font, int32 Rating, int32 Rating_Threshold, vec2 Colon_Pos, flo32 RatingX, flo32 ValueX, vec2 Rating_Dim, flo32 Rating_Offset, vec4 Color ) {
    DrawString( Pass, Font, ":", Colon_Pos, TextAlign_Center, Color );
    
    char Str[ 32 ] = {};
    sprintf( Str, "%d", Rating_Threshold );
    DrawString( Pass, Font, Str, Vec2( ValueX, Colon_Pos.y ), TextAlign_CenterRight, Color );
    
    DrawScoreRating( Pass, Rating, Vec2( RatingX, Colon_Pos.y ), Rating_Dim, Rating_Offset );
}

internal void
DrawEndLevel( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    END_LEVEL * EndLevel = Draw->EndLevel;
    
    PLAYER_STATE              * Player       = &AppState->Player;
    SCORE_STATE               * ScoreS       = Draw->ScoreS;
    OVERWORLD_STATE_SAVE_DATA * OverworldS   = &AppState->SaveGame.OverworldSave;
    OVERWORLD_LEVEL             CurrentLevel = OverworldS->Level[ OverworldS->AtLevel ];
    
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
    int32 nScoreMultiplier = 1 + ScoreMultiplierS->nScoreMultiplierSpawned;
    // NOTE: This value starts at 1 because the player starts with 1 score multiplier.
    
    if( ( EndLevel->Show ) && ( EndLevel->FadeIn_Timer >= END_LEVEL_FADE_IN_DELAY_TARGET_TIME ) ) {
        flo32 Alpha = Clamp01( ( EndLevel->FadeIn_Timer - END_LEVEL_FADE_IN_DELAY_TARGET_TIME ) / END_LEVEL_FADE_IN_TARGET_TIME );
        
        DrawRect( Pass, AppState->UI_Bound, Vec4( 0.0f, 0.0f, 0.0f, 0.5f ) * Alpha );
        
        {
            vec2 Text_Pos   = Vec2( 960.0f, 810.0f );
            vec4 Text_Color = COLOR_WHITE;
            if( EndLevel->ShowLevelComplete ) {
                DrawString( Pass, AppState->FontList + FontID_7_00, "LEVEL COMPLETE", Text_Pos, TextAlign_Center, Vec2( 1.0f, 1.0f ), Text_Color * Alpha );
            } else {
                DrawString( Pass, AppState->FontList + FontID_7_00, "TRY AGAIN", Text_Pos, TextAlign_BottomCenter, Vec2( 1.0f, 1.0f ), Text_Color * Alpha );
            }
        }
        
        if( ScoreS->DoBonusObjective ) {
            vec2 Text_Pos   = Vec2( 960.0f, 730.0f );
            vec4 Text_Color = BONUS_OBJECTIVE_TEXT_BASE_COLOR;
            
            DrawString( Pass, AppState->FontList + FontID_3_00, "2X OBJECTIVE ACHIEVED", Text_Pos, TextAlign_Center, Vec2( 1.0f, 1.0f ), Text_Color * Alpha );
        }
        
        flo32 Score = ScoreS->Score;
        if( ScoreS->DoBonusObjective ) {
            Score *= 2.0f;
        }
        
        if( Score > CurrentLevel.HighScore ) {
            vec4 Color = Vec4( 0.8f, 0.8f, 0.0f, 1.0f ) * Alpha;
            DrawString( Pass, AppState->FontList + FontID_2_00, "NEW HIGH SCORE!", Vec2( 960.0f, 548.0f ), TextAlign_Center, Vec2( 1.0f, 1.0f ), Color );
        }
        
        {
            flo32 X       = 960.0f;
            flo32 XMargin =  28.0f;
            
            flo32 NewHigh_X     = 686.0f;
            vec4  NewHigh_Color = TowardBlack( COLOR_YELLOW, 0.15f );
            
            { // SCORE
                flo32 Y = 486.0f;
                
                DrawString( Pass, AppState->FontList + FontID_3_00, ":", Vec2( X, Y ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                DrawString( Pass, AppState->FontList + FontID_3_00, "Score", Vec2( X - XMargin, Y ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                
                char Str[ 32 ] = {};
                sprintf( Str, "%u", ( uint32 )Score );
                DrawString( Pass, AppState->FontList + FontID_3_00, Str, Vec2( X + XMargin, Y ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                
                if( ( uint32 )Score > CurrentLevel.HighScore ) {
                    DrawString( Pass, AppState->FontList + FontID_1_50, "NEW HIGH", Vec2( NewHigh_X, Y ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), NewHigh_Color * Alpha );
                }
            }
            
            { // MULTIPLIER
                flo32 Y = 432.0f;
                
                DrawString( Pass, AppState->FontList + FontID_3_00, ":", Vec2( X, Y ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                DrawString( Pass, AppState->FontList + FontID_2_50, "Multiplier", Vec2( X - XMargin, Y ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                
                char Str[ 32 ] = {};
                sprintf( Str, "%u", ( uint32 )ScoreS->Multiplier );
                DrawString( Pass, AppState->FontList + FontID_3_00, Str, Vec2( X + XMargin, Y ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                
                if( ( uint32 )ScoreS->Multiplier > CurrentLevel.HighMultiplier ) {
                    DrawString( Pass, AppState->FontList + FontID_1_50, "NEW HIGH", Vec2( NewHigh_X, Y ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), NewHigh_Color * Alpha );
                }
            }
            
            { // BONUS MULTIPLIER
                flo32 Y = 378.0f;
                
                { // Avg Bonus
                    DrawString( Pass, AppState->FontList + FontID_3_00, ":", Vec2( X, Y ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                    DrawString( Pass, AppState->FontList + FontID_2_50, "Avg. Bonus", Vec2( X - XMargin, Y ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                    
                    flo32 AvgBonus = ScoreS->BonusMultiplier_Total / ScoreS->BonusMultiplier_TotalTime;
                    flo32 Percent  = GetT01( AvgBonus, 1.0f, BONUS_MULTIPLIER_MAX_VALUE );
                    
                    vec2 Bonus_Dim   = Vec2( 240.0f, 22.0f );
                    rect Bonus_Bound = RectTD( Vec2( X + XMargin, Y ), Vec2( 0.0f, 0.5f ), Bonus_Dim );
                    
                    DrawBarForBonusMultiplier( Pass, Bonus_Bound, 2.0f, Percent );
                    
                    if( AvgBonus > CurrentLevel.HighAvgBonus ) {
                        DrawString( Pass, AppState->FontList + FontID_1_50, "NEW HIGH", Vec2( NewHigh_X, Y ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), NewHigh_Color * Alpha );
                    }
                }
            }
        }
        
        { // EXIT TO OVERWORLD
            vec2 P   = Vec2( 864.0f, 162.0f );
            vec2 Dim = Vec2( 228.0f, 40.0f );
            rect R   = RectRCD( P + Vec2( -10.0f, 0.0f ), Dim );
            DrawRect       ( Pass, R, COLOR_GRAY( 0.02f ) * Alpha );
            DrawRectOutline( Pass, R, COLOR_GRAY( 0.1f  ) * Alpha );
            DrawString( Pass, AppState->FontList + FontID_2_00, "Left Mouse", GetCenter( R ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
            
            flo32 t = 0.0f;
            if( EndLevel->EndLevelAction == EndLevelAction_ExitToOverworld ) {
                t = Clamp01( EndLevel->EndLevelAction_Timer / END_LEVEL_BUTTON_TARGET_TIME );
            }
            rect  S = R;
            S.Right = lerp( S.Left, t, S.Right );
            
            DrawRect( Pass, S, COLOR_YELLOW * Alpha );
            
            vec4 TextColor = COLOR_WHITE;
            if( t > 0.0f ) {
                TextColor = COLOR_YELLOW;
            }
            DrawString( Pass, AppState->FontList + FontID_2_00, ": To Overworld Map", P, TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), TextColor * Alpha );
        }
        
        { // REPLAY
            vec2 P   = Vec2( 864.0f, 108.0f );
            vec2 Dim = Vec2( 228.0f, 36.0f );
            rect R   = RectRCD( P + Vec2( -10.0f, 0.0f ), Dim );
            DrawRect       ( Pass, R, COLOR_GRAY( 0.02f ) * Alpha );
            DrawRectOutline( Pass, R, COLOR_GRAY( 0.1f  ) * Alpha );
            DrawString( Pass, AppState->FontList + FontID_2_00, "Right Mouse", GetCenter( R ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
            
            flo32 t = 0.0f;
            if( EndLevel->EndLevelAction == EndLevelAction_ReplayLevel ) {
                t = Clamp01( EndLevel->EndLevelAction_Timer / END_LEVEL_BUTTON_TARGET_TIME );
            }
            rect  S = R;
            S.Right = lerp( S.Left, t, S.Right );
            
            DrawRect( Pass, S, COLOR_YELLOW * Alpha );
            
            vec4 TextColor = COLOR_WHITE;
            if( t > 0.0f ) {
                TextColor = COLOR_YELLOW;
            }
            DrawString( Pass, AppState->FontList + FontID_2_00, ": Replay Level", P, TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), TextColor * Alpha );
        }
        
        { // EXIT TO DESKTOP
            vec2 P   = Vec2( 864.0f, 54.0f );
            vec2 Dim = Vec2( 228.0f, 36.0f );
            rect R   = RectRCD( P + Vec2( -10.0f, 0.0f ), Dim );
            DrawRect       ( Pass, R, COLOR_GRAY( 0.02f ) * Alpha );
            DrawRectOutline( Pass, R, COLOR_GRAY( 0.1f  ) * Alpha );
            DrawString( Pass, AppState->FontList + FontID_2_00, "Escape", GetCenter( R ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
            
            flo32 t = 0.0f;
            if( EndLevel->EndLevelAction == EndLevelAction_ExitToDesktop ) {
                t = Clamp01( EndLevel->EndLevelAction_Timer / END_LEVEL_BUTTON_TARGET_TIME );
            }
            rect  S = R;
            S.Right = lerp( S.Left, t, S.Right );
            
            DrawRect( Pass, S, COLOR_YELLOW * Alpha );
            
            vec4 TextColor = COLOR_WHITE;
            if( t > 0.0f ) {
                TextColor = COLOR_YELLOW;
            }
            DrawString( Pass, AppState->FontList + FontID_2_00, ": Exit to Desktop", P, TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), TextColor * Alpha );
        }
        
        { // ENTITY TOTALS AND KILLS
            if( EndLevel->ShowStats ) {
                flo32 Panel_Width = 490.0f;
                vec2  Panel_bPos  = Vec2( 1900.0f, 648.0f ) + Vec2( -Panel_Width, 0.0f );
                
                flo32 Entry_Width   = Panel_Width;
                flo32 Entry_Height  = Panel_Width * 0.1f;
                flo32 Line_Height   = Entry_Height * 0.7f;
                flo32 Header_Height = Entry_Height * 0.5f;
                vec2  Icon_Dim      = Vec2( Entry_Height, Entry_Height );
                vec2  Scale         = Vec2( 0.15f, 0.1f ) * Icon_Dim;
                flo32 AddedGap      = 8.0f;
                
                ENTITY_TYPE Entity_DrawOrder[] = {
                    EntityType_Drifter,
                    EntityType_Looper,
                    EntityType_Chaser,
                    EntityType_EnemyBullet,
                    EntityType_Turret,
                    EntityType_Gunner,
                    EntityType_Swarm,
                    EntityType_Asteroid,
                    EntityType_Asteroid_SwarmOnly,
                    EntityType_PersonalPod,
                    EntityType_Transport,
                    EntityType_Yacht,
                    EntityType_Miner,
                    EntityType_Bomb,
                    EntityType_BombLauncher,
                    EntityType_Missile,
                    EntityType_MissileLauncher,
                    EntityType_Fireball,
                    EntityType_Worm,
                    EntityType_Spit,
                    EntityType_Barrel,
                    EntityType_Recon,
                    EntityType_Blimp,
                    EntityType_MegaTurret,
                    EntityType_MegaMiner,
                    EntityType_Miner_Stationary,
                    EntityType_Loader,
                    EntityType_ObservationDeck,
                };
                int32 nEntity = _ArrayCount( Entity_DrawOrder );
                
                uint32 nEntry = 0;
                for( int32 iType = 0; iType < nEntity; iType++ ) {
                    ENTITY_TYPE Type = Entity_DrawOrder[ iType ];
                    SCORE_ENTITY_KILL_COUNT EntityKillCount = ScoreS->EntityKillCount[ Type ];
                    
                    if( EntityKillCount.nTotal > 0 ) {
                        nEntry += 1;
                    }
                }
                
                rect Bound = RectTLD( Panel_bPos, Vec2( Entry_Width, Entry_Height * ( flo32 )nEntry + Line_Height * 5.0f + Header_Height * 2.0f + AddedGap * 2.0f ) );
                
                DrawRect( Pass,            Bound,          COLOR_GRAY( 0.1f  ) );
                DrawRect( Pass, AddRadius( Bound, -4.0f ), COLOR_GRAY( 0.05f ) );
                
                vec2 AtPos = GetT( Bound, Vec2( 0.04f, 1.0f ) );
                AtPos.y -= Header_Height;
                
                {
                    FONT * Font = AppState->FontList + FontID_1_75;
                    
                    char Str[ 128 ] = {};
                    
                    { // Game Timer
                        int32 Minutes = ( ( int32 )ScoreS->Level_PlayTimer / 60 );
                        int32 Seconds = ( ( int32 )ScoreS->Level_PlayTimer % 60 );
                        
                        sprintf( Str, "Time: %2d:%02d", Minutes, Seconds );
                        DrawString( Pass, Font, Str, AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        
                        AtPos.y -= ( Line_Height + AddedGap );
                    }
                    
                    flo32 SlashX          = AtPos.x + 240.0f;
                    flo32 OffsetFromSlash = 20.0f;
                    
                    { // MULTIPLIER
                        DrawString( Pass, Font, "Multiplier:", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        DrawString( Pass, Font, "/", Vec2( SlashX, AtPos.y ), TextAlign_TopCenter, COLOR_WHITE );
                        
                        sprintf( Str, "%u", ( uint32 )ScoreS->Multiplier );
                        DrawString( Pass, Font, Str, Vec2( SlashX - OffsetFromSlash, AtPos.y ), TextAlign_TopRight, COLOR_WHITE );
                        
                        sprintf( Str, "%u", nScoreMultiplier );
                        DrawString( Pass, Font, Str, Vec2( SlashX + OffsetFromSlash, AtPos.y ), TextAlign_TopLeft, COLOR_WHITE );
                        
                        AtPos.y -= Line_Height;
                    }
                    
                    { // AVG BONUS
                        flo32 AvgBonus = ScoreS->BonusMultiplier_Total / ScoreS->BonusMultiplier_TotalTime;
                        
                        DrawString( Pass, Font, "Avg. Bonus:", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        DrawString( Pass, Font, "/", Vec2( SlashX, AtPos.y ), TextAlign_TopCenter, COLOR_WHITE );
                        
                        sprintf( Str, "%.02f", AvgBonus );
                        DrawString( Pass, Font, Str, Vec2( SlashX - OffsetFromSlash, AtPos.y ), TextAlign_TopRight, COLOR_WHITE );
                        
                        sprintf( Str, "%.02f", ( flo32 )BONUS_MULTIPLIER_MAX_VALUE );
                        DrawString( Pass, Font, Str, Vec2( SlashX + OffsetFromSlash, AtPos.y ), TextAlign_TopLeft, COLOR_WHITE );
                        
                        AtPos.y -= ( Line_Height + AddedGap );
                    }
                    
                    { // Time at Full Bonus
                        int32 Minutes = ( ( int32 )ScoreS->BonusMultiplier_FullTime / 60 );
                        int32 Seconds = ( ( int32 )ScoreS->BonusMultiplier_FullTime % 60 );
                        flo32 Percent = ScoreS->BonusMultiplier_FullTime / ScoreS->BonusMultiplier_TotalTime * 100.0f;
                        
                        sprintf( Str, " Time at Full Bonus: %2d:%02d  (%.02f%%)", Minutes, Seconds, Percent );
                        DrawString( Pass, AppState->FontList + FontID_1_50, Str, AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_GRAY( 0.75f ) );
                        
                        AtPos.y -= Line_Height;
                    }
                    
                }
                
                AtPos.y -= Line_Height * 0.5f;
                {
                    
                    //DrawString( Pass, AppState->FontList + FontID_, "Enemy Kills", GetTL( Bound ) + Vec2( 20.0f, -8.0f ), TextAlign_TopLeft, Vec2( 1.35f, 1.35f ), COLOR_WHITE );
                    DrawString( Pass, AppState->FontList + FontID_1_35, "Enemy Kills", AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                    
                    AtPos.y -= Line_Height;
                }
                
                FONT * Font  = AppState->FontList + FontID_1_35;
                vec4   Color = COLOR_WHITE * Alpha;
                
                for( int32 iType = 0; iType < nEntity; iType++ ) {
                    ENTITY_TYPE Type = Entity_DrawOrder[ iType ];
                    SCORE_ENTITY_KILL_COUNT EntityKillCount = ScoreS->EntityKillCount[ Type ];
                    
                    if( EntityKillCount.nTotal > 0 ) {
                        flo32 SlashX          = AtPos.x + 160.0f;
                        flo32 OffsetFromSlash = 12.0f;
                        
                        rect Icon_Bound = RectTLD( AtPos, Icon_Dim );
                        DrawEntityIcon( Pass, Type, AddRadius( Icon_Bound, 4.0f ), true );
                        
                        vec2  bPos = GetT( Icon_Bound, Vec2( 0.0f, 0.5f ) ) + Vec2( Icon_Dim.x * 1.1666667f, 0.0f );
                        flo32 Type_AdvanceX = Entry_Width * 0.3f;
                        
                        char Str[ 128 ] = {};
                        DrawString( Pass, AppState->FontList + FontID_1_50, ":", bPos + Vec2( 6.0f, 0.0f ), TextAlign_Center, Color );
                        DrawString( Pass, Font, "/", Vec2( SlashX, bPos.y ), TextAlign_Center, Color );
                        
                        sprintf( Str, "%u", EntityKillCount.nKilled );
                        DrawString( Pass, Font, Str, Vec2( SlashX - OffsetFromSlash, bPos.y ), TextAlign_CenterRight, Color );
                        
                        sprintf( Str, "%u", EntityKillCount.nTotal );
                        DrawString( Pass, Font, Str, Vec2( SlashX + OffsetFromSlash, bPos.y ), TextAlign_CenterLeft, Color );
                        
                        if( Type == EntityType_Spit ) {
                            SPITTER_STATE * SpitterS = &AppState->SpitterS;
                            sprintf( Str, "(3x = %u)", SpitterS->nSpitBonusTotal );
                            DrawString( Pass, Font, Str, Vec2( SlashX + 80.0f, bPos.y ), TextAlign_CenterLeft, Color );
                        }
                        
                        AtPos.y -= Entry_Height;
                    }
                }
            } else {
                flo32 X = 1820.0f;
                DrawString( Pass, AppState->FontList + FontID_2_50, "Show Stats", Vec2( X, 540.0f ), TextAlign_CenterRight, COLOR_WHITE * Alpha );
                
                vec4 Arrow_Color = COLOR_WHITE * 0.2f;
                if( EndLevel->ShowStats_Timer >= 0.0f ) {
                    Arrow_Color = COLOR_WHITE;
                }
                DrawTriangleBC( Pass, Vec2( X - 208.0f, 500.0f ), Vec2( 15.0f, 20.0f ), -PI * 0.5f, Arrow_Color * Alpha );
                
                rect Bound = Rect( X - 222.0f, 380.0f, X - 220.0f, 568.0f );
                DrawRect( Pass, Bound, COLOR_WHITE * 0.2f * Alpha );
            }
        }
        
        { // SCORE RATING
            vec2 bPos = Vec2( 960.0f, 648.0f );
            
            OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
            OVERWORLD_LEVEL * Level      = OverworldS->Level + OverworldS->AtLevel;
            OVERWORLD_MAP   * Map        = OverworldT->Map   + OverworldS->AtLevel;
            
            boo32 ScoreRating_WasFound = false;
            int32 ScoreRating          = 0;
            for( int32 iScoreRating = 0; ( !ScoreRating_WasFound ) && ( iScoreRating < SCORE_RATING_MAX_RATING ); iScoreRating++ ) {
                if( ( int32 )Score < Map->Info.ScoreRating_Threshold[ iScoreRating ] ) {
                    ScoreRating          = iScoreRating;
                    ScoreRating_WasFound = true;
                }
            }
            if( !ScoreRating_WasFound ) {
                Assert( ( int32 )Score >= Map->Info.ScoreRating_Threshold[ SCORE_RATING_MAX_RATING - 1 ] );
                ScoreRating = SCORE_RATING_MAX_RATING;
            }
            
            //Level->HighScore      = MaxValue( Level->HighScore,      ( uint32 )Score              );
            //Level->HighMultiplier = MaxValue( Level->HighMultiplier, ( uint32 )ScoreS->Multiplier );
            //Level->ScoreRating    = MaxValue( Level->ScoreRating,              ScoreRating );
            //if( ScoreS->DoBonusObjective ) {
            //Level->BonusObjectiveWasAcquired = true;
            //}
            
            int32 Rating    = ScoreRating;
            int32 MaxRating = 5;
            
            vec4 Outline_Color = COLOR_BLACK;
            vec4 Inner_Color   = COLOR_GRAY( 0.75f );
            
            vec2  Scale   = Vec2( 48.0f, 40.0f );
            flo32 OffsetX = ( Scale.x * 2.0f ) + 2.0f;
            
            {
                
                vec2  AtPos   = bPos;
                AtPos.x -= ( OffsetX * 2.0f );
                
                //if( Level.BonusObjectiveWasAcquired ) {
                //AtPos.x -= ( OffsetX * 0.5f );
                //}
                
                for( int32 iRating = 0; iRating < MaxRating; iRating++ ) {
                    if( iRating <= ( Rating - 1 ) ) {
                        DrawTriangle( Pass, AtPos, Scale, 0.0f, Inner_Color );
                    } else {
                        DrawTriangle( Pass, AtPos, Scale, 0.0f, Outline_Color );
                        DrawTriangleOutline( Pass, AtPos, Scale, 0.0f, Inner_Color );
                    }
                    
                    AtPos.x += OffsetX;
                }
            }
            
            {
                vec2 AtPos = Vec2( 300.0f, 540.0f );
                
                flo32 OffsetFromColon = 28.0f;
                vec2  Rating_Dim      = Vec2( 30.0f, 24.0f );
                flo32 Rating_Offset   = 1.0f;
                flo32 RatingX         = AtPos.x - ( OffsetFromColon + Rating_Dim.x * 2.5f + Rating_Offset * 2.0f );
                flo32 PaddingX        = 20.0f;
                flo32 Text_Offset     = 172.0f;
                flo32 AdvanceY        = 52.0f;
                
                DISPLAY_VALUE( int32, Rating );
                for( int32 iScoreRating = 0; iScoreRating < SCORE_RATING_MAX_RATING; iScoreRating++ ) {
                    if( Rating == iScoreRating ) {
                        rect Bound  = RectCD( AtPos, Vec2( 0.0f, 44.0f ) );
                        Bound.Left  = RatingX - ( Rating_Dim.x * 2.5f + PaddingX );
                        Bound.Right = Bound.Right + Text_Offset + PaddingX;
                        Bound.Top  += 2.0f;
                        DrawRect( Pass, Bound, COLOR_GRAY( 0.05f ) );
                    }
                    
                    DrawScoreThresholdColonScoreRating( Pass, AppState->FontList + FontID_2_00, iScoreRating + 1, Map->Info.ScoreRating_Threshold[ iScoreRating ], AtPos, RatingX, AtPos.x + Text_Offset, Rating_Dim, Rating_Offset, COLOR_WHITE );
                    
                    AtPos.y -= AdvanceY;
                }
            }
            
            //if( Level.BonusObjectiveWasAcquired ) {
            //vec4 ColorC = Vec4( 1.0f, 1.0f, 0.5f, 1.0f );
            //DrawString( Pass, AppState->FontList + FontID_, "2X", AtPos, TextAlign_Center, WORLD_TEXT_SCALE * 1.25f, ColorC );
            //}
        }
        
#if 0        
        { // SCORE LOG
            ENTITY_TYPE_LABELS;
            
            char Str[ 128 ] = {};
            
            flo32 AdvanceY = 16.0f;
            flo32 t        = ( Player->Position.x - AppState->World_Bound.Left ) / GetWidth( AppState->World_Bound );
            
            vec2 AtPos = GetT( AppState->App_Bound, Vec2( 0.08f, 0.88f ) );
            AtPos.y += ( AdvanceY * ( flo32 )ScoreS->nScoreLog * t );
            
            flo32 TotalPointsMissed = 0.0f;
            
            for( int32 iScoreLog = 0; iScoreLog < ScoreS->nScoreLog; iScoreLog++ ) {
                SCORE_LOG ScoreLog = ScoreS->ScoreLog[ iScoreLog ];
                
                sprintf( Str, "%.3s:  %8.2f  %4u  %4.2f : %3u, %8.2f", EntityTypeLabels[ ScoreLog.Type ], ScoreLog.Points, ( uint32 )ScoreLog.AtScoreMultiplier, ScoreLog.AtBonusMultiplier, ScoreLog.nScoreMultiplierOnScreen, ScoreLog.PointsMissed );
                //DrawString( Pass, AppState->FontList + FontID_, Str, AtPos, TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                
                AtPos.y -= AdvanceY;
                
                TotalPointsMissed += ScoreLog.PointsMissed;
            }
            
            sprintf( Str, "Points Missed : %.2f", TotalPointsMissed );
            //DrawString( Pass, AppState->FontList + FontID_, Str, AtPos, TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
        }
#endif
    }
}

internal void
FinalizeEndLevel( APP_STATE * AppState ) {
    END_LEVEL * EndLevel = &AppState->EndLevel;
    
    if( EndLevel->Show ) {
#if 0
        if( ( EndLevel->DoExitToOverworld ) && ( EndLevel->DoExitToOverworld_Timer >= END_LEVEL_BUTTON_TARGET_TIME ) ) {
            EndLevel->DoExitToOverworld       = false;
            EndLevel->DoExitToOverworld_Timer = 0.0f;
            
            AppState->DoExitToOverworld = true;
        }
        if( ( EndLevel->DoReplay ) && ( EndLevel->DoReplay_Timer >= END_LEVEL_BUTTON_TARGET_TIME ) ) {
            EndLevel->DoReplay       = false;
            EndLevel->DoReplay_Timer = 0.0f;
            
            AppState->DoReplayLevel = true;
        }
        if( ( EndLevel->DoExitToDesktop ) && ( EndLevel->DoExitToDesktop_Timer >= END_LEVEL_BUTTON_TARGET_TIME ) ) {
            EndLevel->DoExitToDesktop       = false;
            EndLevel->DoExitToDesktop_Timer = 0.0f;
            
            AppState->DoExitToDesktop = true;
        }
#endif
        if( ( EndLevel->EndLevelAction != EndLevelAction_Null ) && ( EndLevel->EndLevelAction_Timer >= END_LEVEL_BUTTON_TARGET_TIME ) ) {
            AppState->EndLevelAction       = EndLevel->EndLevelAction;
            
            EndLevel->EndLevelAction       = EndLevelAction_Null;
            EndLevel->EndLevelAction_Timer = 0.0f;
        }
    }
}