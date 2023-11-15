
internal BOMB_LAUNCHER
InitBombLauncher( vec2 Position, vec2 Direction ) {
    BOMB_LAUNCHER Result = {};
    Result.Position    = Position;
    Result.Radius      = BOMB_LAUNCHER_RADIUS;
    Result.Health      = BOMB_LAUNCHER_HEALTH;
    
    Result.Launch_TargetTime = BOMB_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME;
    
    Result.Anim_Direction = Direction;
    
    return Result;
}

internal void
UpdateBombLauncher( APP_STATE * AppState, flo32 dT ) {
    BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
    PLAYER_STATE        * Player        = &AppState->Player;
    
    for( int32 iBombLauncher = 0; iBombLauncher < BombLauncherS->nBombLauncher; iBombLauncher++ ) {
        BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iBombLauncher;
        
        BombLauncher->Damage_Timer = MaxValue( BombLauncher->Damage_Timer - dT, 0.0f );
        BombLauncher->Radians = ToRadians( Player->Position - BombLauncher->Position );
        
        switch( BombLauncher->Stage ) {
            case BombLauncherStage_Wait: {
                // DO NOTHING
            } break;
            
            case BombLauncherStage_EnableBombLauncher: {
                BombLauncher->Anim_Timer += dT;
                
                if( BombLauncher->Anim_Timer >= BOMB_LAUNCHER_SPAWN_ANIM_TARGET_TIME ) {
                    BombLauncher->Stage = BombLauncherStage_SpawnBombs;
                }
            } break;
            
            case BombLauncherStage_SpawnBombs: {
                BombLauncher->Launch_Timer += dT;
                if( BombLauncher->Launch_Timer >= BombLauncher->Launch_TargetTime ) {
                    BombLauncher->Launch_Timer      = 0.0f;
                    BombLauncher->Launch_TargetTime = RandomF32InRange( 2.5f, 4.0f ) + BOMB_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME;
                    
                    vec2 Dir = ToDirection2D( BombLauncher->Radians );
                    vec2 Pos = BombLauncher->Position + Dir * BOMB_LAUNCHER_RADIUS;
                    
                    AddBomb( AppState, BombC( BombLauncher->WaveGuid, Pos, Dir ) );
                }
                
                if( BombLauncher->CanMove ) {
                    BombLauncher->CanMove_Timer += dT;
                    if( BombLauncher->CanMove_Timer >= BOMB_LAUNCHER_MOVE_DELAY_TARGET_TIME ) {
                        flo32 Speed = 10.0f;
                        flo32 dY    = BombLauncher->CanMove_Dir * Speed * dT;
                        BombLauncher->Position.y += dY;
                        
                        flo32 yRange = 18.0f;
                        
                        if( ( BombLauncher->Position.y < -yRange ) || ( BombLauncher->Position.y > yRange ) ) {
                            BombLauncher->Position.y = Clamp( BombLauncher->Position.y, -yRange, yRange );
                            BombLauncher->CanMove_Timer = 0.0f;
                            BombLauncher->CanMove_Dir  *= -1.0f;
                        }
                    }
                }
            } break;
            
            case BombLauncherStage_IsDestroyed: {
                // Do nothing.
            } break;
        }
    }
    
}

internal void
DrawBombLauncher( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, BOMB_LAUNCHER BombLauncher, boo32 DrawMount, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 Anim_Height = BOMB_LAUNCHER_SPAWN_ANIM_HEIGHT;
    vec2  PosA        = BombLauncher.Position;
    vec2  PosB        = BombLauncher.Position - BombLauncher.Anim_Direction * Anim_Height;
    vec2  Center      = ( PosA + PosB ) * 0.5f;
    
    if( BombLauncher.Stage < BombLauncherStage_IsDestroyed ) {
        if( DrawMount ) { // Draw Mount
            vec2  DimA   = Vec2( ( BOMB_LAUNCHER_RADIUS * 0.5f ) * 2.0f, Anim_Height + 0.2f * 2.0f );
            vec2  DimB   = Vec2( 0.20f, Anim_Height );
            
            orect BoundA = ORectCDY( Center, DimA, BombLauncher.Anim_Direction );
            orect BoundB = ORectCDY( Center, DimB, BombLauncher.Anim_Direction );
            
            DrawORect( Pass, BoundA, COLOR_GRAY( 0.1f ) );
            DrawORect( Pass, BoundB, COLOR_BLACK );
        }
        
        {
            vec4 Color_Base  = BOMB_LAUNCHER_BASE_COLOR;
            vec4 Color_BaseA = TowardBlack( Color_Base, 0.8f );
            vec4 Color_BaseB = TowardBlack( Color_Base, 0.35f );
            
            vec4 Color_Barrel = BOMB_LAUNCHER_BASE_COLOR;
            vec4 Color_Launch = TowardWhite( BOMB_LAUNCHER_BASE_COLOR, 0.1f );
            vec4 Color_Stand  = TowardBlack( BOMB_LAUNCHER_BASE_COLOR, 0.55f );
            
            {
                flo32 tDamage = Clamp01( BombLauncher.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
                Color_Base = lerp( Color_Base, tDamage, TowardWhite( Color_Base, 0.25f ) );
            }
            
            flo32 AnimT = Clamp01( BombLauncher.Anim_Timer / BOMB_LAUNCHER_SPAWN_ANIM_TARGET_TIME );
            vec2  Pos   = lerp( PosB, AnimT, PosA );
            vec2  Dir = ToDirection2D( BombLauncher.Radians );
            
            DrawRect( Pass, TextureID_RoundedDome, RectCR( Pos, BOMB_LAUNCHER_RADIUS        * Scale ), Color_Base  * Alpha );
            DrawRect( Pass, TextureID_Dome,        RectCR( Pos, BOMB_LAUNCHER_RADIUS * 0.7f * Scale ), Color_BaseA * Alpha );
            DrawRect( Pass, TextureID_Dome,        RectCR( Pos, BOMB_LAUNCHER_RADIUS * 0.6f * Scale ), Color_BaseB * Alpha );
            
            vec2  Pos_Barrel   = Pos - Dir * ( BOMB_LAUNCHER_RADIUS * 0.8f ) * Scale;
            vec2  Dim_Barrel   = Vec2( 1.8f, 0.9f ) * BOMB_LAUNCHER_RADIUS   * Scale;
            orect Bound_Barrel = ORectTD( Pos_Barrel, Vec2( 0.0f, 0.5f ), Dim_Barrel, BombLauncher.Radians );
            
            vec2  Pos_LaunchA  = Pos - Dir * ( BOMB_LAUNCHER_RADIUS * 0.75f ) * Scale;
            vec2  Pos_LaunchB  = Pos + Dir * ( BOMB_LAUNCHER_RADIUS * 0.5f  ) * Scale;
            
            flo32 Launch_TargetTime = ( BombLauncher.Launch_TargetTime - BOMB_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME );
            flo32 tLaunch = 1.0f - Clamp01( BombLauncher.Launch_Timer / Launch_TargetTime );
            if( ( Alpha >= 1.0f ) && ( BombLauncher.Launch_Timer >= Launch_TargetTime ) ) {
                tLaunch = Clamp01( ( BombLauncher.Launch_Timer - Launch_TargetTime ) / BOMB_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME );
                Color_Launch = lerp( Color_Launch, tLaunch, TowardWhite( Color_Launch, 0.25f ) );
            }
            vec2  Pos_Launch   = lerp( Pos_LaunchA, tLaunch, Pos_LaunchB );
            
            DrawORect( Pass, ORectTD( GetT( Bound_Barrel, Vec2( 0.85f, 1.0f ) ), Vec2( 1.0f, 1.0f ), Dim_Barrel * Vec2( 0.8f, 0.5f ), BombLauncher.Radians - ( PI / 14.0f ) ), Color_Stand * Alpha );
            DrawORect( Pass, ORectTD( GetT( Bound_Barrel, Vec2( 0.85f, 0.0f ) ), Vec2( 1.0f, 0.0f ), Dim_Barrel * Vec2( 0.8f, 0.5f ), BombLauncher.Radians + ( PI / 14.0f ) ), Color_Stand * Alpha );
            
            DrawORect( Pass, TextureID_Barrel_Rot90, Bound_Barrel, Color_Barrel * Alpha );
            DrawORect( Pass, TextureID_SatelliteDish, ORectTD( Pos_Barrel, Vec2( 0.0f, 0.5f ), Dim_Barrel * Vec2( 0.2f, 1.0f ), BombLauncher.Radians + PI ), Color_Barrel * Alpha );
            DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Pos_Barrel, Vec2( 0.0f, 0.5f ), Dim_Barrel * Vec2( 0.05f, 1.1f ), BombLauncher.Radians ), TowardBlack( Color_Barrel, 0.2f ) * Alpha );
            DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Pos_Launch, Vec2( 0.0f, 0.5f ), Dim_Barrel * Vec2( 0.15f, 1.08f ), BombLauncher.Radians ), Color_Launch * Alpha );
            
            if( BombLauncher.Health > 0 ) {
                vec2 Pos_Text = GetT( Bound_Barrel, Vec2( 0.25f, 0.5f ) );
                
                DrawORect( Pass, TextureID_Barrel_Rot90, ORectCD( Pos_Text, Dim_Barrel * Vec2( 0.05f, 1.2f ), BombLauncher.Radians ), TowardBlack( BOMB_LAUNCHER_BASE_COLOR, 0.75f ) );
                DrawRect( Pass, TextureID_Dome, RectCR( Pos_Text, 0.6f ), TowardBlack( BOMB_LAUNCHER_BASE_COLOR, 0.75f ) );
                
                DrawEntityHealth( Pass, AppState, Camera, Pos_Text, BombLauncher.Health, COLOR_WHITE );
            }
        }
    }
}

internal void
DrawBombLauncher( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    BOMB_LAUNCHER_STATE * BombLauncherS = Draw->BombLauncherS;
    
    for( int32 iBombLauncher = 0; iBombLauncher < BombLauncherS->nBombLauncher; iBombLauncher++ ) {
        BOMB_LAUNCHER BombLauncher = BombLauncherS->BombLauncher[ iBombLauncher ];
        
        DrawBombLauncher( Pass, AppState, Camera, BombLauncher, true );
    }
}

internal void
FinalizeBombLauncher( APP_STATE * AppState ) {
    SCORE_STATE         * ScoreS        = &AppState->ScoreS;
    BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
    
    int32 iAlt[ BOMB_LAUNCHER_MAX_COUNT ] = { 1, 0, 3, 2 };
    
    for( int32 iBombLauncher = 0; iBombLauncher < BombLauncherS->nBombLauncher; iBombLauncher++ ) {
        BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iBombLauncher;
        
        if( ( BombLauncher->Stage != BombLauncherStage_IsDestroyed ) && ( BombLauncher->Health == 0 ) ) {
            BombLauncher->Stage = BombLauncherStage_IsDestroyed;
            
            BOMB_LAUNCHER * Alt = BombLauncherS->BombLauncher + iAlt[ iBombLauncher ];
            Alt->CanMove       = true;
            Alt->CanMove_Timer = 0.0f;
            Alt->CanMove_Dir   = ( RandomS32InRange( 0, 1 ) == 0 ) ? 1.0f : -1.0f;
            
            //AddKillToScore( AppState, EntityType_BombLauncher, BombLauncher->Position );
            //AddKillToWave ( AppState, BombLauncher->WaveGuid );
            //MARKET_STATE_SAVE_DATA * MarketS = &AppState->MarketS.SaveData;
            //POINTS_ENTRY             Points  = MarketS->Points[ EntityType_BombLauncher ];
            //AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( BombLauncher->Position, Points.nScoreMultiplier ) );
            
            AddBigEntityDeathParticles( AppState, BombLauncher->Position, BombLauncher->Radius, BOMB_LAUNCHER_BASE_COLOR );
            AddBigEntityDeathParticles( AppState, BombLauncher->Position, BombLauncher->Radius, BOMB_LAUNCHER_BASE_COLOR );
            AddBigEntityDeathParticles( AppState, BombLauncher->Position, BombLauncher->Radius, COLOR_WHITE, false );
            
            BombLauncherS->BonusObjective_nBombLauncherKills += 1;
            BombLauncherS->BonusObjective_SpawnCooldownTimer  = 0.0f;
        }
    }
}