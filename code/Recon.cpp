
internal RECON
ReconC( int32 WaveGuid, vec2 Position, flo32 Radians ) {
    RECON Result = {};
    Result.WaveGuid = WaveGuid;
    Result.bPos     = Position;
    Result.Position = Position;
    Result.Health   = RECON_HEALTH;
    Result.Radians  = Radians;
    Result.Timer    = RECON_DEATH_ANIM_TARGET_TIME;
    
    vec2 xAxis = ToDirection2D( Radians );
    vec2 yAxis = GetPerp( xAxis );
    
    vec2 PartOffset = Vec2( RECON_SIDE_LENGTH, RECON_SIDE_LENGTH ) * 0.25f;
    vec2 Pos0[ RECON_PART_MAX_COUNT ] = {};
    Pos0[ 0 ] = Result.Position - xAxis * PartOffset.x - yAxis * PartOffset.y;
    Pos0[ 1 ] = Result.Position + xAxis * PartOffset.x - yAxis * PartOffset.y;
    Pos0[ 2 ] = Result.Position                        + yAxis * PartOffset.y;
    
    for( int32 iPart = 0; iPart < RECON_PART_MAX_COUNT; iPart++ ) {
        Result.Part[ iPart ].Position = Pos0[ iPart ];
        Result.Part[ iPart ].Radians  = Radians;
    }
    
    return Result;
}

internal void
AddRecon( APP_STATE * AppState, RECON Recon ) {
    RECON_STATE * ReconS = &AppState->ReconS;
    if( ReconS->nRecon < RECON_MAX_COUNT ) {
        ReconS->Recon[ ReconS->nRecon++ ] = Recon;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add RECON to RECON_STATE: %u", RECON_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateRecon( APP_STATE * AppState, flo32 dT ) {
    RECON_STATE * ReconS = &AppState->ReconS;
    
    for( int32 iRecon = 0; iRecon < ReconS->nRecon; iRecon++ ) {
        RECON * Recon = ReconS->Recon + iRecon;
        
        if( Recon->Health > 0 ) {
            Recon->PosX_Timer += dT;
            if( Recon->PosX_Timer >= Recon->PosX_TargetTime ) {
                Recon->PosX_Timer      = 0.0f;
                Recon->PosX_TargetTime = RandomF32InRange( 3.0f, 5.0f );
                Recon->PosX_Rate       = TAU / ( RandomF32InRange( 2.0f, 8.0f ) );
            }
            
            Recon->PosY_Timer += dT;
            if( Recon->PosY_Timer >= Recon->PosY_TargetTime ) {
                Recon->PosY_Timer      = 0.0f;
                Recon->PosY_TargetTime = RandomF32InRange( 3.0f, 5.0f );
                Recon->PosY_Rate       = TAU / ( RandomF32InRange( 2.0f, 8.0f ) );
            }
            
            Recon->PosX_Radians = WrapRadians( Recon->PosX_Radians + Recon->PosX_Rate * dT );
            Recon->PosY_Radians = WrapRadians( Recon->PosY_Radians + Recon->PosY_Rate * dT );
            
            vec2 xAxis = ToDirection2D( Recon->Radians );
            vec2 yAxis = GetPerp( xAxis );
            
            Recon-> Position    = Recon->bPos;
            Recon->Position.x += cosf( Recon->PosX_Radians ) * RECON_POSITION_OFFSET;
            Recon->Position.y += cosf( Recon->PosY_Radians ) * RECON_POSITION_OFFSET;
            
            int32 PrevHealth = Recon->Health;
            { // Collision with Enemy Bullet
                vec2 Dim = Vec2( RECON_SIDE_LENGTH, RECON_SIDE_LENGTH ) * 0.5f;
                
                vec2 A = Recon->Position + xAxis * -Dim.x + yAxis * -Dim.y;
                vec2 B = Recon->Position + xAxis *  Dim.x + yAxis * -Dim.y;
                vec2 C = Recon->Position +                  yAxis *  Dim.y;
                
                ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
                for( int32 iEnemyBullet = 0; ( Recon->Health > 0 ) && ( iEnemyBullet < EnemyBulletS->nEnemyBullet ); iEnemyBullet++ ) {
                    ENEMY_BULLET * EnemyBullet = EnemyBulletS->EnemyBullet + iEnemyBullet;
                    
                    if( EnemyBullet->Health > 0 ) {
                        flo32 Radius   = ENEMY_BULLET_RADIUS - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = DistSqPointToTri( EnemyBullet->Position, A, B, C );
                        if( DistSq <= RadiusSq ) {
                            EnemyBullet->Health         = MaxValue( EnemyBullet->Health - 1, 0 );
                            EnemyBullet->DoHitPointDrop = true;
                            
                            Recon->Health               = MaxValue( Recon->Health       - 1, 0 );
                        }
                    }
                }
            }
            
            vec2 PartOffset = Vec2( RECON_SIDE_LENGTH, RECON_SIDE_LENGTH ) * 0.25f;
            vec2 Pos0[ RECON_PART_MAX_COUNT ] = {};
            Pos0[ 0 ] = Recon->Position - xAxis * PartOffset.x - yAxis * PartOffset.y;
            Pos0[ 1 ] = Recon->Position + xAxis * PartOffset.x - yAxis * PartOffset.y;
            Pos0[ 2 ] = Recon->Position                        + yAxis * PartOffset.y;
            
            for( int32 iPart = 0; iPart < RECON_PART_MAX_COUNT; iPart++ ) {
                RECON_PART * Part = Recon->Part + iPart;
                
                Part->Position = Pos0[ iPart ];
                
                Part->Blink_TimerA += dT;
                if( Part->Blink_TimerA >= Part->Blink_TargetTimeA ) {
                    Part->Blink_TargetTimeA =  ( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
                    Part->Blink_TimerA      = -( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
                }
                
                Part->Blink_TimerB += dT;
                if( Part->Blink_TimerB >= Part->Blink_TargetTimeB ) {
                    Part->Blink_TargetTimeB =  ( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
                    Part->Blink_TimerB      = -( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
                }
            }
            
            if( Recon->Health != PrevHealth ) {
                ReconS->BonusObjective_nReconKills += 1;
                
                for( int32 iPart = 0; iPart < RECON_PART_MAX_COUNT; iPart++ ) {
                    RECON_PART * Part = Recon->Part + iPart;
                    
                    flo32 Speed   = RandomF32InRange( 3.0f, 4.0f );
                    flo32 Radians = ToRadians( Part->Position - Recon->Position );
                    Radians += RandomF32InRange( -1.0f, 1.0f ) * ( PI / 64.0f );
                    
                    Part->Velocity = ToDirection2D( Radians ) * Speed;
                    
                    flo32 dRadians = TAU / RandomF32InRange( 14.0f, 16.0f );
                    if( RandomS32InRange( 0, 1 ) == 1 ) {
                        dRadians = -dRadians;
                    }
                    Part->dRadians = dRadians;
                    
                    Part->Timer = ( flo32 )RandomS32InRange( 8, 30 ) / 60.0f;
                    
                    Part->Blink_TimerA = -1.0f;
                    Part->Blink_TimerB = -1.0f;
                }
            }
        } else {
            Recon->Health = -1;
            
            {
                flo32 PrevTimer = Recon->Timer;
                Recon->Timer -= dT;
                
                if( ( PrevTimer >= 0.0f ) && ( Recon->Timer < 0.0f ) ) {
                    AddEntityDeathParticles( AppState, Recon->Position, RECON_BLINK_LIGHT_COLOR );
                    AddEntityDeathParticles( AppState, Recon->Position, RECON_BLINK_LIGHT_COLOR );
                }
                
                if( Recon->Timer > 0.0f ) {
                    Recon->Health = 0;
                }
            }
            
            for( int32 iPart = 0; iPart < RECON_PART_MAX_COUNT; iPart++ ) {
                RECON_PART * Part = Recon->Part + iPart;
                
                Part->Position += Part->Velocity * dT;
                Part->Radians   = WrapRadians( Part->Radians + Part->dRadians * dT );
                
                flo32 PrevTimer = Part->Timer;
                Part->Timer    -= dT;
                
                if( ( PrevTimer >= 0.0f ) && ( Part->Timer < 0.0f ) ) {
                    AddEntityDeathParticles( AppState, Part->Position, RECON_BLINK_LIGHT_COLOR );
                    AddEntityDeathParticles( AppState, Part->Position, RECON_BLINK_LIGHT_COLOR );
                }
                
                if( Part->Timer > 0.0f ) {
                    Recon->Health = 0;
                }
            }
        }
    }
}

internal void
DrawRecon( RENDER_PASS * Pass, RECON Recon, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec2 ScaA   = Vec2( RECON_SIDE_LENGTH, RECON_SIDE_LENGTH ) * ( 0.5f * Scale );
    vec2 ScaB   = ScaA * Vec2( 0.50f, 0.50f );
    vec4 ColorA = RECON_BASE_COLOR;
    vec4 ColorB = COLOR_GRAY( 0.15f );
    
    if( Recon.Timer >= 0.0f ) {
        if( Recon.Health <= 0 ) {
            flo32 t = 1.0f - Clamp01( Recon.Timer / RECON_DEATH_ANIM_TARGET_TIME );
            ColorA = lerp( ColorA, t * 0.5f, TowardWhite( RECON_BLINK_LIGHT_COLOR, 0.5f ) );
        }
        DrawTriangle( Pass, Recon.Position, ScaA, Recon.Radians, ColorA );
    }
    
    for( int32 iPart = 0; iPart < 3; iPart++ ) {
        RECON_PART Part = Recon.Part[ iPart ];
        
        if( Part.Timer >= 0.0f ) {
            vec4 Color = ColorB;
            
            if( Recon.Health <= 0 ) {
                flo32 t = 1.0f - Clamp01( Part.Timer / ( RECON_DEATH_ANIM_TARGET_TIME * 2.0f ) );
                Color = lerp( Color, t * 0.8f, TowardWhite( RECON_BLINK_LIGHT_COLOR, 0.5f ) );
            }
            
            vec2 Part_Pos = Recon.Position + ( Part.Position - Recon.Position ) * Scale;
            DrawTriangle( Pass, TextureID_Recon, Part_Pos, ScaB, Part.Radians, Color );
            
            vec2 xAxis = ToDirection2D( Part.Radians );
            vec2 yAxis = GetPerp( xAxis );
            
            vec2 DimA = ScaB * Vec2( 0.2f, -0.65f );
            vec2 DimB = ScaB * Vec2( 0.4f, -0.65f );
            
            vec2 A = Part_Pos + xAxis * DimA.x + yAxis * DimA.y;
            vec2 B = Part_Pos + xAxis * DimB.x + yAxis * DimB.y;
            
            vec4 ColorC = COLOR_BLACK;
            vec4 ColorD = COLOR_BLACK;
            
            if( Part.Blink_TimerA >= 0.0f ) {
                ColorC = RECON_BLINK_LIGHT_COLOR;
            }
            if( Part.Blink_TimerB >= 0.0f ) {
                ColorD = RECON_BLINK_LIGHT_COLOR;
            }
            
            DrawPoint( Pass, A, Vec2( 0.10f, 0.10f ) * Scale, ColorC * 0.75f );
            DrawPoint( Pass, B, Vec2( 0.10f, 0.10f ) * Scale, ColorD * 0.75f );
        }
    }
}

internal void
DrawRecon( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    RECON_STATE * ReconS = Draw->ReconS;
    
    for( int32 iRecon = 0; iRecon < ReconS->nRecon; iRecon++ ) {
        RECON Recon = ReconS->Recon[ iRecon ];
        
        DrawRecon( Pass, Recon );
    }
}

internal void
FinalizeRecon( APP_STATE * AppState ) {
    RECON_STATE * ReconS = &AppState->ReconS;
    
    uint32 NewCount = 0;
    for( int32 iRecon = 0; iRecon < ReconS->nRecon; iRecon++ ) {
        RECON Recon = ReconS->Recon[ iRecon ];
        
        if( Recon.Health > -1 ) {
            ReconS->Recon[ NewCount++ ] = Recon;
        } else {
            AddKillToWave( AppState, Recon.WaveGuid );
            
            AddEntityToKilledCountForLevel( AppState, EntityType_Recon, 1 );
        }
    }
    ReconS->nRecon = NewCount;
}