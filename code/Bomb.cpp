
#if 0
internal BOMBER
BomberC( int32 WaveGuid, vec2 Position, int32 nBomb = 1 ) {
    BOMBER Result = {};
    Result.WaveGuid    = WaveGuid;
    Result.Position    = Position;
    Result.Radius      = BOMBER_RADIUS;
    Result.Health      = BOMBER_HEALTH;
    
    Result.nBomb       = nBomb;
    Assert( ( nBomb > 0 ) && ( nBomb <= BOMBER_BOMB_MAX_COUNT ) );
    flo32 Segment = TAU / ( flo32 )nBomb;
    for( int32 iBomb = 0; iBomb < nBomb; iBomb++ ) {
        Result.Bomb_Radians[ iBomb ]  = ( flo32 )iBomb * Segment;
    }
    
    return Result;
}

internal BOMBER
BomberC_Autonomous( int32 WaveGuid, vec2 Position, int32 nBomb = 1 ) {
    BOMBER Result = {};
    Result.WaveGuid    = WaveGuid;
    Result.Position    = Position;
    Result.Radius      = BOMBER_RADIUS;
    Result.Health      = BOMBER_HEALTH;
    
    Result.Launch_TargetTime = RandomF32InRange( 2.0f, 5.0f ) + BOMBER_LAUNCH_WARMUP_TARGET_TIME;
    
    Result.nBomb       = nBomb;
    Assert( ( nBomb > 0 ) && ( nBomb <= BOMBER_BOMB_MAX_COUNT ) );
    flo32 Segment = TAU / ( flo32 )nBomb;
    for( int32 iBomb = 0; iBomb < nBomb; iBomb++ ) {
        Result.Bomb_Radians[ iBomb ]  = ( flo32 )iBomb * Segment;
    }
    
    Result.CanMove = true;
    
    return Result;
}

internal void
AddBomber( APP_STATE * AppState, BOMBER Bomber ) {
    BOMB_STATE * BombS = &AppState->BombS;
    if( BombS->nBomber < BOMBER_MAX_COUNT ) {
        BombS->Bomber[ BombS->nBomber++ ] = Bomber;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BOMBER to BOMB_STATE: %u", BOMBER_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}
#endif

internal BOMB
BombC( int32 WaveGuid, vec2 Position, vec2 Direction ) {
    BOMB Result = {};
    Result.WaveGuid    = WaveGuid;
    Result.Position    = Position;
    Result.Velocity    = Direction * BOMB_SPEED;
    Result.Radius      = BOMB_RADIUS;
    
    return Result;
}

internal void
AddBomb( APP_STATE * AppState, BOMB Bomb ) {
    BOMB_STATE * BombS = &AppState->BombS;
    if( BombS->nBomb < BOMB_MAX_COUNT ) {
        BombS->Bomb[ BombS->nBomb++ ] = Bomb;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BOMB to BOMB_STATE: %u", BOMB_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

#if 0
internal void
UpdateBomber( APP_STATE * AppState, flo32 dT ) {
    BOMB_STATE   * BombS   = &AppState->BombS;
    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
    PLAYER_STATE   * Player    = &AppState->Player;
    
    for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
        BOMBER * Bomber = BombS->Bomber + iBomber;
        
        if( Bomber->CanMove ) {
            Bomber->Move_Timer += dT;
            if( Bomber->Move_Timer >= Bomber->Move_TargetTime ) {
                Bomber->Move_Timer      = 0.0f;
                Bomber->Move_TargetTime = RandomF32InRange( 1.5f, 3.0f );
                
                flo32 t = RandomF32();
                Bomber->Move_Speed = lerp( 0.5f, t * t, 2.5f );
                
                rect  Bound   = AddRadius( AppState->World_Bound, -8.0f );
                flo32 Radians = WrapRadians( Bomber->Move_Radians + RandomF32InRange( -PI / 3.0f, PI / 3.0f ) );
                vec2  Dir     = ToDirection2D( Radians );
                vec2  Dest    = Bomber->Position + Dir * ( Bomber->Move_Speed * Bomber->Move_TargetTime );
                if( ( Dest.x < Bound.Left ) || ( Dest.x > Bound.Right ) ) {
                    Dir.x = -Dir.x;
                }
                if( ( Dest.y < Bound.Bottom ) || ( Dest.y > Bound.Top ) ) {
                    Dir.y = -Dir.y;
                }
                
                Dest = Bomber->Position + Dir * ( Bomber->Move_Speed * Bomber->Move_TargetTime );
                if( !IsInRect( Dest, Bound ) ) {
                    Dest = ClampToRect( Dest, Bound );
                    Dir  = GetNormal( Dest - Bomber->Position );
                }
                
                Bomber->Move_Radians = ToRadians( Dir );
            }
            
            {
                vec2 Dir   = ToDirection2D( Bomber->Move_Radians );
                vec2 Accel = ( Dir * Bomber->Move_Speed - Bomber->Velocity + Bomber->Repel ) * BOMBER_FRICTION;
                
                vec2 dPos = Accel * ( 0.5f * dT * dT ) + Bomber->Velocity * dT;
                // TODO: Collision!
                Bomber->Position += dPos;
                Bomber->Velocity += Accel * dT;
                
                Bomber->Repel = {};
            }
        }
        
        Bomber->AtRadians = ToRadians( Player->Position - Bomber->Position );
        
        Bomber->Launch_Timer += dT;
        if( Bomber->Launch_Timer >= Bomber->Launch_TargetTime ) {
            Bomber->Launch_Timer      = 0.0f;
            Bomber->Launch_TargetTime = RandomF32InRange( 2.0f, 5.0f ) + BOMBER_LAUNCH_WARMUP_TARGET_TIME;
            
            for( int32 iBomb = 0; iBomb < Bomber->nBomb; iBomb++ ) {
                flo32 Radians = Bomber->AtRadians + Bomber->Bomb_Radians[ iBomb ];
                vec2  Dir     = ToDirection2D( Radians );
                vec2  Pos     = Bomber->Position + Dir * 1.8f;
                
                AddBomb( AppState, BombC( Bomber->WaveGuid, Pos, Dir ) );
            }
        }
    }
}
#endif

internal void
UpdateBomb( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE * Player  = &AppState->Player;
    BOMB_STATE * BombS = &AppState->BombS;
    
    for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
        BOMB * Bomb = BombS->Bomb + iBomb;
        
        if( Bomb->IsTriggered ) {
            Bomb->Light_Timer += ( dT * 4.0f );
        } else {
            Bomb->Light_Timer += dT;
        }
        if( Bomb->Light_Timer >= BOMB_LIGHT_CYCLE_TARGET_TIME ) {
            Bomb->Light_Timer -= ( BOMB_LIGHT_CYCLE_TARGET_TIME * 2.0f );
        }
        
        if( !Bomb->IsTriggered ) {
            flo32 Radius   = BOMB_DETONATE_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            flo32 DistSq   = GetLengthSq( Player->Position - Bomb->Position );
            if( DistSq <= RadiusSq ) {
                Bomb->IsTriggered = true;
                Bomb->Light_Timer = 0.0f;
            }
        }
        
        if( Bomb->IsTriggered ) {
            Bomb->Timer += dT;
            if( ( !Bomb->IsDetonated ) && ( Bomb->Timer >= BOMB_DETONATE_TARGET_TIME ) ) {
                Bomb->IsDetonated = true;
                Bomb->DoDamage    = true;
                Bomb->Timer       = 0.0f;
            }
        }
        
        Bomb->IsTractorBeamed = false;
        if( !Bomb->IsDetonated ) {
            if( Player->TractorBeam_IsActive ) {
                flo32 dSpeed      = BOMB_SPEED;
                vec2  DirToPlayer = GetNormal( Player->Position - Bomb->Position );
                
                Bomb->IsTractorBeamed = true;
                Bomb->IsTractorBeamed_Radians = ToRadians( DirToPlayer );
                
                Bomb->Velocity += DirToPlayer * ( dSpeed * dT );
                if( GetLength( Bomb->Velocity ) >= BOMB_SPEED ) {
                    Bomb->Velocity = GetNormal( Bomb->Velocity ) * BOMB_SPEED;
                }
            }
            
            Bomb->Position += Bomb->Velocity * dT;
        }
    }
    
    for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
        BOMB * Bomb = BombS->Bomb + iBomb;
        if( Bomb->DoDamage ) {
            
#if 0            
            { // DRIFTER
                DRIFTER_STATE * DrifterS = &AppState->DrifterS;
                
                for( int32 iDrifter = 0; iDrifter < DrifterS->nDrifter; iDrifter++ ) {
                    DRIFTER * Drifter = DrifterS->Drifter + iDrifter;
                    
                    flo32 Radius   = ( BOMB_DAMAGE_RADIUS + Drifter->Radius );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bomb->Position - Drifter->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        Drifter->Health = 0;
                    }
                }
            }
#endif
            
#if 0
            { // BOMB
                for( uint32 iTargetBomb = 0; iTargetBomb < BombS->nBomb; iTargetBomb++ ) {
                    BOMB * TargetBomb = BombS->Bomb + iTargetBomb;
                    
                    if( ( iTargetBomb != iBomb ) && ( !TargetBomb->IsDetonated ) ) {
                        flo32 Margin = 0.0f;
                        flo32 Radius   = ( BOMB_DAMAGE_RADIUS + TargetBomb->Radius + Margin );
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Bomb->Position - TargetBomb->Position );
                        
                        if( DistSq <= RadiusSq ) {
                            TargetBomb->IsTriggered = true;
                            TargetBomb->Timer       = MaxValue( TargetBomb->Timer, BOMB_DETONATE_TARGET_TIME - BOMB_DETONATE_BOMB_TARGET_TIME );
                        }
                    }
                }
            }
#endif
            
#if 0            
            { // ASTEROID
                ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
                
                for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                    ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                    
                    flo32 Margin = 0.0f;
                    flo32 Radius   = ( BOMB_DAMAGE_RADIUS + Asteroid->Radius + Margin );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bomb->Position - Asteroid->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        if( !( Asteroid->Flags & ASTEROID_FLAG__LASER_ONLY ) ) {
                            Asteroid->Health = 0;
                        } else {
                            Asteroid->Flags &= ( ~ASTEROID_FLAG__BOMB_ONLY );
                        }
                        
                        vec2 Velocity = Asteroid->Direction * Asteroid->Speed;
                        Velocity     += GetNormal( Asteroid->Position - Bomb->Position ) * 4.0f;
                        
                        Asteroid->Direction = GetNormal( Velocity );
                        Asteroid->Speed     = GetLength( Velocity );
                    }
                }
            }
#endif
            
            { // MINER
                MINER_STATE * MinerS = &AppState->MinerS;
                
                for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                    MINER * Miner = MinerS->Miner + iMiner;
                    
                    flo32 Margin = 0.0f;
                    flo32 Radius   = ( BOMB_DAMAGE_RADIUS + Miner->Radius + Margin );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bomb->Position - Miner->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        Miner->Health = 0;
                    }
                }
            }
            
#if 0            
            { // BOMBER
                for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
                    BOMBER * Bomber = BombS->Bomber + iBomber;
                    
                    flo32 Margin = 0.0f;
                    flo32 Radius   = ( BOMB_DAMAGE_RADIUS + Bomber->Radius + Margin );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bomb->Position - Bomber->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        Bomber->Health = 0;
                    }
                }
            }
#endif
            
#if 0            
            { // BUMPER
                BUMPER_STATE * BumperS = &AppState->BumperS;
                for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
                    BUMPER * Bumper = BumperS->Bumper + iBumper;
                    
                    flo32 Margin = 0.0f;
                    flo32 Radius   = ( BOMB_DAMAGE_RADIUS + Bumper->Radius + Margin );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bomb->Position - Bumper->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        Bumper->Health = 0;
                    }
                }
            }
#endif
            
            { // TURRET
                TURRET_STATE * TurretS = &AppState->TurretS;
                for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
                    TURRET * Turret = TurretS->Turret + iTurret;
                    
                    {
                        flo32 Radius   = ( BOMB_DAMAGE_RADIUS + Turret->Radius );
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Bomb->Position - Turret->Position );
                        
                        if( DistSq <= RadiusSq ) {
                            Turret->Health = 0;
                        }
                    }
                    
                    ENTITY_SHIELD Shield = Turret->Shield;
                    if( ( Turret->Health > 0 ) && ( Shield.HasShield ) ) {
                        Assert( Shield.Radians == TAU );
                        
                        flo32 Radius   = BOMB_DAMAGE_RADIUS;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistToSegmentPos = ( Turret->Radius + Shield.Dist + Shield.Width - 0.1f );
                        
                        for( int32 iSegment = 0; iSegment < TURRET_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
                            flo32 t   = Clamp01( ( flo32 )iSegment / ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT );
                            vec2  Pos = Turret->Position + ToDirection2D( Shield.bRadians + TAU * t ) * DistToSegmentPos;
                            flo32 DistSq = GetLengthSq( Pos - Bomb->Position );
                            if( DistSq <= RadiusSq ) {
                                Turret->Shield_SegmentT[ iSegment ] = 2.0f;
                            }
                        }
                    }
                }
            }
            
            { // ENEMY_BULLET
                ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
                for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
                    ENEMY_BULLET * EnemyBullet = EnemyBulletS->EnemyBullet + iEnemyBullet;
                    
                    flo32 Margin = 0.0f;
                    flo32 Radius   = ( BOMB_DAMAGE_RADIUS + ENEMY_BULLET_RADIUS + Margin );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bomb->Position - EnemyBullet->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        EnemyBullet->Health             = 0;
                        EnemyBullet->DoExplodePointDrop = true;
                    }
                }
            }
            
            { // BOMB_LAUNCHER
                BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
                for( int32 iBombLauncher = 0; iBombLauncher < BombLauncherS->nBombLauncher; iBombLauncher++ ) {
                    BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iBombLauncher;
                    
                    if( ( BombLauncher->Stage == BombLauncherStage_EnableBombLauncher ) || ( BombLauncher->Stage == BombLauncherStage_SpawnBombs ) ) {
                        
                        flo32 Anim_Height = BOMB_LAUNCHER_SPAWN_ANIM_HEIGHT;
                        vec2  PosA        = BombLauncher->Position;
                        vec2  PosB        = BombLauncher->Position - BombLauncher->Anim_Direction * Anim_Height;
                        flo32 Anim_T      = Clamp01( BombLauncher->Anim_Timer / BOMB_LAUNCHER_SPAWN_ANIM_TARGET_TIME );
                        vec2  Pos         = lerp( PosB, Anim_T, PosA );
                        
                        flo32 Margin = 0.0f;
                        flo32 Radius   = ( BOMB_DAMAGE_RADIUS + BombLauncher->Radius + Margin );
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Bomb->Position - Pos );
                        
                        if( DistSq <= RadiusSq ) {
                            BombLauncher->Health = 0;
                        }
                    }
                }
            }
        }
    }
}

internal void
DrawBomber( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, BOMBER Bomber, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    {
        vec4 ColorA = COLOR_GRAY( 0.08f  );
        vec4 ColorB = COLOR_GRAY( 0.055f );
        vec4 ColorC = COLOR_GRAY( 0.03f  );
        
        DrawCircle( Pass, Bomber.Position, Bomber.Radius * Scale, ColorA * Alpha );
        
        {
            flo32 TargetTime = Bomber.Launch_TargetTime - BOMBER_LAUNCH_WARMUP_TARGET_TIME;
            if( Bomber.Launch_Timer >= TargetTime ) {
                flo32 Numer = ( Bomber.Launch_Timer - TargetTime );
                flo32 Denom = ( BOMBER_LAUNCH_WARMUP_TARGET_TIME - ( 20.0f / 60.0f ) );
                flo32 t     = Clamp01( Numer / Denom );
                t *= t;
                
                ColorC = lerp( ColorC, t, Vec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
            }
        }
        
        flo32 Height = 1.8f;
        if( Bomber.nBomb == 1 ) {
            Height = 1.7f;
        }
        
        for( int32 iBomb = 0; iBomb < Bomber.nBomb; iBomb++ ) {
            flo32 Radians = Bomber.AtRadians + Bomber.Bomb_Radians[ iBomb ];
            orect BoundC  = ORectBCD( Bomber.Position, Vec2( 1.7f, Height ) * Scale, Radians );
            DrawORect( Pass, BoundC, ColorC * Alpha );
        }
        
        vec2 Offset = {};
        if( Bomber.nBomb == 1 ) {
            Offset = ToDirection2D( Bomber.AtRadians ) * -0.1f;
        }
        DrawCircle( Pass, Bomber.Position + Offset, Bomber.Radius * Scale * 0.8f, ColorB * Alpha );
        
        for( int32 iBomb = 0; iBomb < Bomber.nBomb; iBomb++ ) {
            flo32 Radians = Bomber.AtRadians + Bomber.Bomb_Radians[ iBomb ];
            orect BoundD  = ORectBCD( Bomber.Position, Vec2( 0.4f, 1.7f ) * Scale, Radians );
            DrawORect( Pass, BoundD, ColorC * Alpha );
        }
        
        DrawCircle( Pass, Bomber.Position + Offset, Bomber.Radius * Scale * 0.4f, COLOR_GRAY( 0.03f ) * Alpha );
        
        if( Bomber.Health > 0 ) {
            Assert( Font );
            Assert( Camera );
            
            char Str[ 4 ] = {};
            sprintf( Str, "%u", Bomber.Health );
            DrawString( Pass, Font, Str, Bomber.Position, TextAlign_Center, WORLD_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE );
        }
    }
}

internal void
DrawBomb( RENDER_PASS * Pass, BOMB Bomb, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 tTimer = Clamp01( Bomb.Timer / ( BOMB_DETONATE_TARGET_TIME - ( 12.0f / 60.0f ) ) );
    
    vec4 Color_Base  = lerp( BOMB_BASE_COLOR, tTimer, TowardWhite( BOMB_BASE_COLOR, 0.35f ) );
    vec4 Color_Light = COLOR_BLACK;
    vec4 Color_Ring  = COLOR_BLACK;
    vec4 Color_Glow  = Vec4( 1.0f, 0.25f, 0.25f, 1.0f ) * 0.1f;
    
    if( Bomb.IsDetonated ) {
        flo32 dRadius = ( Clamp01( Bomb.Timer / BOMB_DAMAGE_TARGET_TIME ) * 0.2f + 0.1f );
        flo32  Radius = ( BOMB_DAMAGE_RADIUS + dRadius ) * Scale;
        
        DrawCircle( Pass, Bomb.Position, Radius, COLOR_WHITE * ( 0.8f * Alpha ) );
        
        if( Alpha < 1.0f ) {
            DrawRect( Pass, TextureID_RoundedDome, RectCR( Bomb.Position, BOMB_RADIUS * 0.8f  * Scale  ), Color_Base * ( Alpha * 3.0f ) );
            DrawRect( Pass, TextureID_Dome,        RectCR( Bomb.Position, BOMB_RADIUS * 0.25f * Scale  ), Color_Ring * ( Alpha * 3.0f ) );
        }
    } else {
        flo32 Radius = BOMB_RADIUS * Scale;
        
        if( Bomb.Light_Timer >= 0.0f ) {
            Color_Glow  = Vec4( 1.0f, 0.25f, 0.25f, 1.0f ) * 0.65f;
            Color_Light = Vec4( 0.75f, 0.0f, 0.0f, 1.0f );
        }
        if( Bomb.IsTriggered ) {
            Color_Ring = COLOR_GRAY( 0.5f );
        }
        
        DrawRect( Pass, TextureID_Glow,        RectCR( Bomb.Position, Radius * 1.1f   ), Color_Glow );
        DrawRect( Pass, TextureID_RoundedDome, RectCR( Bomb.Position, Radius * 0.8f   ), Color_Base );
        DrawRect( Pass, TextureID_Dome,        RectCR( Bomb.Position, Radius * 0.25f  ), Color_Ring );
        DrawRect( Pass, TextureID_Dome,        RectCR( Bomb.Position, Radius * 0.175f ), Color_Light );
        
        if( Bomb.IsTriggered ) {
            DrawCircleOutline( Pass, Bomb.Position, BOMB_DAMAGE_RADIUS, COLOR_RED );
            DrawCircle       ( Pass, Bomb.Position, BOMB_DAMAGE_RADIUS, Vec4( 0.025f, 0.0f, 0.0f, 0.025f ) );
        }
        
#if 0        
        if( Bomb.IsTractorBeamed ) {
            vec2 Dir = ToDirection2D( Bomb.IsTractorBeamed_Radians );
            vec2 Pos = Bomb.Position + Dir * 0.9f;
            vec2 Sca = Vec2( 0.15f, 0.2f );
            
            DrawTriangle( Pass, Pos, Sca, Bomb.IsTractorBeamed_Radians - PI * 0.5f, Vec4( 0.0f, 0.65f, 0.65f, 1.0f ) );
        }
#endif
        
        //DrawCircleOutline( Pass, Bomb.Position, BOMB_DETONATE_RADIUS, COLOR_RED );
    }
}

#if 0
internal void
DrawBomber( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    BOMB_STATE * BombS = Draw->BombS;
    
    for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
        BOMBER Bomber = BombS->Bomber[ iBomber ];
        
        DrawBomber( Pass, Font, Camera, Bomber );
    }
}
#endif

internal void
DrawBomb( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BOMB_STATE * BombS = Draw->BombS;
    
    for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
        BOMB Bomb = BombS->Bomb[ iBomb ];
        
        DrawBomb( Pass, Bomb );
    }
}



internal void
FinalizeBomb( APP_STATE * AppState ) {
    SCORE_STATE  * ScoreS = &AppState->ScoreS;
    BOMB_STATE * BombS = &AppState->BombS;
    
    {
        for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
            BOMB * Bomb = BombS->Bomb + iBomb;
            if( Bomb->IsDetonated ) {
                Bomb->DoDamage = false;
            }
        }
    }
    
#if 0
    {
        uint32 NewCount = 0;
        for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
            BOMBER Bomber = BombS->Bomber[ iBomber ];
            
            if( Bomber.Health > 0 ) {
                BombS->Bomber[ NewCount++ ] = Bomber;
            } else {
                AddKillToScore( AppState, EntityType_Bomber, Bomber.Position );
                AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( Bomber.Position, 50 ) );
                
                AddEntityDeathParticles( AppState, Bomber.Position, COLOR_GRAY( 0.2f ) );
                AddEntityDeathParticles( AppState, Bomber.Position, COLOR_RED );
                AddEntityDeathParticles( AppState, Bomber.Position, COLOR_WHITE );
                AddKillToWave( AppState, Bomber.WaveGuid );
            }
        }
        BombS->nBomber = NewCount;
    }
#endif
    
    {
        uint32 NewCount = 0;
        for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
            BOMB Bomb = BombS->Bomb[ iBomb ];
            
            boo32 DoKill = false;
            
            rect  Bound = AddRadius( AppState->World_Bound, BOMB_DAMAGE_RADIUS - 2.0f );
            if( !IsInRect( Bomb.Position, Bound ) ) {
                DoKill = true;
                
                AddEntityToMissedCountForLevel( AppState, EntityType_Bomb, 1 );
            }
            
            if( ( Bomb.IsDetonated ) && ( Bomb.Timer >= BOMB_DAMAGE_TARGET_TIME ) ) {
                DoKill = true;
                
                AddKillToScore( AppState, EntityType_Bomb, Bomb.Position );
            }
            
            if( !DoKill ) {
                BombS->Bomb[ NewCount++ ] = Bomb;
            }
        }
        BombS->nBomb = NewCount;
    }
}