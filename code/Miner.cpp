
internal LASER
LaserC( vec2 Position, flo32 Radians ) {
    LASER Result = {};
    Result.Position     = Position;
    Result.Radians      = Radians;
    Result.MaxLength    = MINER_STATIONARY_LASER_LENGTH;
    
    Result.Fire_TargetTime  = 4.0f;
    Result.Reset_TargetTime = Result.Fire_TargetTime + 2.0f;
    Result.Timer            = Result.Fire_TargetTime - MINER_LASER_WARMUP_TARGET_TIME;
    
    return Result;
}

internal void
AddLaser( APP_STATE * AppState, LASER Laser ) {
    MINER_STATE * MinerS = &AppState->MinerS;
    if( MinerS->nLaser < LASER_MAX_COUNT ) {
        MinerS->Laser[ MinerS->nLaser++ ] = Laser;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add LASER to MINER_STATE: %u", LASER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal MINER
MinerC_Stationary( int32 WaveGuid, vec2 Position, flo32 RotateSpeed, int32 nLaser = 1 ) {
    MINER Result = {};
    Result.WaveGuid     = WaveGuid;
    Result.IsAutonomous = false;
    Result.Position     = Position;
    Result.Radius       = MINER_RADIUS_STATIONARY;
    Result.Health       = MINER_HEALTH;
    Result.RotateSpeed  = RotateSpeed;
    
    Result.Fire_TargetTime = MINER_STATIONARY_LASER_INIT_TARGET_TIME + MINER_LASER_WARMUP_TARGET_TIME;
    Result.Laser_MaxLength = MINER_STATIONARY_LASER_LENGTH;
    
    Result.nLaser       = nLaser;
    Assert( ( nLaser > 0 ) && ( nLaser <= MINER_LASER_MAX_COUNT ) );
    flo32 Segment = TAU / ( flo32 )nLaser;
    for( int32 iLaser = 0; iLaser < nLaser; iLaser++ ) {
        MINER_LASER * Laser = Result.Laser + iLaser;
        flo32 Adjust = RandomF32InRange( -1.0f, 1.0f ) * ( PI / 64.0f );
        Laser->Radians  = ( ( flo32 )iLaser * Segment ) + Adjust;
    }
    
    return Result;
}

internal MINER
MinerC_Autonomous( int32 WaveGuid, vec2 Position, int32 nLaser = 1 ) {
    MINER Result = {};
    Result.WaveGuid       = WaveGuid;
    Result.IsAutonomous   = true;
    Result.Position       = Position;
    Result.Radius         = MINER_RADIUS_AUTONOMOUS;
    Result.Health         = MINER_HEALTH;
    Result.RotateSpeed    = MINER_ROTATE_SPEED_TARGET_PLAYER;
    
    Result.AtRadians       = RandomF32() * TAU;
    Result.Fire_TargetTime = RandomF32InRange( 2.0f, 5.0f ) + MINER_LASER_WARMUP_TARGET_TIME;
    Result.Laser_MaxLength = MINER_AUTONOMOUS_LASER_LENGTH;
    
    Result.nLaser         = nLaser;
    Assert( ( nLaser > 0 ) && ( nLaser <= MINER_LASER_MAX_COUNT ) );
    flo32 Segment = TAU / ( flo32 )nLaser;
    for( int32 iLaser = 0; iLaser < nLaser; iLaser++ ) {
        MINER_LASER * Laser = Result.Laser + iLaser;
        flo32 Adjust = RandomF32InRange( -1.0f, 1.0f ) * ( PI / 64.0f );
        Laser->Radians  = ( ( flo32 )iLaser * Segment ) + Adjust;
    }
    
    return Result;
}

internal void
AddMiner( APP_STATE * AppState, MINER Miner ) {
    MINER_STATE * MinerS = &AppState->MinerS;
    if( MinerS->nMiner < MINER_MAX_COUNT ) {
        MinerS->Miner[ MinerS->nMiner++ ] = Miner;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add MINER to MINER_STATE: %u", MINER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal MEGA_MINER
MegaMinerC( int32 WaveGuid, vec2 Position, flo32 Radians ) {
    MEGA_MINER Result = {};
    Result.WaveGuid       = WaveGuid;
    Result.Position       = Position;
    Result.Radius         = MEGA_MINER_RADIUS;
    Result.Health         = MEGA_MINER_HEALTH;
    
    Result.AtRadians      = Radians;
    Result.RotateSpeed    = MEGA_MINER_ROTATE_SPEED;
    
    return Result;
}

internal void
AddMegaMiner( APP_STATE * AppState, MEGA_MINER MegaMiner ) {
    MINER_STATE * MinerS = &AppState->MinerS;
    if( MinerS->nMegaMiner < MEGA_MINER_MAX_COUNT ) {
        MinerS->MegaMiner[ MinerS->nMegaMiner++ ] = MegaMiner;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add MEGA_MINER to MINER_STATE: %u", MEGA_MINER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
PlayMinerLaserAudio( APP_STATE * AppState ) {
    int32         nActive          = 0;
    AUDIO_ENTRY * MinerLaser_Audio = 0;
    
    MINER_STATE * MinerS = &AppState->MinerS;
    for( int32 iMiner = 0; ( nActive <= 0 ) && ( iMiner < MinerS->nMiner ); iMiner++ ) {
        MINER Miner = MinerS->Miner[ iMiner ];
        if( Miner.Laser[ 0 ].IsActive ) {
            nActive += 1;
        }
    }
    
    AUDIO_SYSTEM * AudioSystem = AppState->AudioSystem;
    for( int32 iAudio = 0; ( !MinerLaser_Audio ) && ( iAudio < AudioSystem->nAudio ); iAudio++ ) {
        AUDIO_ENTRY * Audio = AudioSystem->Audio + iAudio;
        if( Audio->AudioID == AudioID_Miner_Laser ) {
            MinerLaser_Audio = Audio;
        }
    }
    
    if( ( nActive > 0 ) && ( !MinerLaser_Audio ) ) {
        // PlayAudio( AppState, AudioID_Miner_Laser, MINER_LASER_VOLUME, 0.75f, true );
    }
    if( ( nActive <= 0 ) && ( MinerLaser_Audio ) ) {
        StopAudio( AudioSystem, AudioID_Miner_Laser );
    }
}

internal void
UpdateMiner( APP_STATE * AppState, flo32 dT ) {
    MINER_STATE    * MinerS    = &AppState->MinerS;
    PLAYER_STATE   * Player    = &AppState->Player;
    
    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
        MINER * Miner = MinerS->Miner + iMiner;
        
        Miner->Damage_Timer = MaxValue( Miner->Damage_Timer - dT, 0.0f );
        
        if( Miner->IsAutonomous ) {
            Miner->Move_Timer += dT;
            if( Miner->Move_Timer >= Miner->Move_TargetTime ) {
                Miner->Move_Timer      = 0.0f;
                Miner->Move_TargetTime = RandomF32InRange( 0.5f, 1.0f );
                
                flo32 t = RandomF32();
                Miner->Move_Speed = lerp( 1.0f, t, 5.0f );
                
                rect  Bound   = AddRadius( AppState->World_Bound, -8.0f );
                flo32 Radians = WrapRadians( Miner->Move_Radians + RandomF32InRange( -PI / 3.0f, PI / 3.0f ) );
                vec2  Dir     = ToDirection2D( Radians );
                vec2  Dest    = Miner->Position + Dir * ( Miner->Move_Speed * Miner->Move_TargetTime );
                if( ( Dest.x < Bound.Left ) || ( Dest.x > Bound.Right ) ) {
                    Dir.x = -Dir.x;
                }
                if( ( Dest.y < Bound.Bottom ) || ( Dest.y > Bound.Top ) ) {
                    Dir.y = -Dir.y;
                }
                
                Dest = Miner->Position + Dir * ( Miner->Move_Speed * Miner->Move_TargetTime );
                
                Dest = Player->Position;
                Dir  = GetNormal( Dest - Miner->Position );
                
                if( !IsInRect( Dest, Bound ) ) {
                    //Dest = ClampToRect( Dest, Bound );
                }
                
                Miner->Move_Radians = ToRadians( Dir );
            }
            
            {
                vec2 Dir   = ToDirection2D( Miner->Move_Radians );
                vec2 Accel = ( Dir * Miner->Move_Speed - Miner->Velocity + Miner->Repel ) * MINER_FRICTION;
                
                vec2 dPos = Accel * ( 0.5f * dT * dT ) + Miner->Velocity * dT;
                // TODO: Collision!
                Miner->Position += dPos;
                Miner->Velocity += Accel * dT;
                
                Miner->Repel = {};
            }
            
            Miner->DestRadians = ToRadians( Player->Position - Miner->Position );
            
            flo32 Dest = Miner->DestRadians;
            flo32 At   = Miner->AtRadians;
            if( Dest != At ) {
                flo32 Diff = GetSmallestDiffInRadians( At, Dest );
                
                flo32 dRadiansHi = ( Miner->RotateSpeed * 2.0f * dT );
                flo32 dRadiansLo = -dRadiansHi;
                if( ( Diff < dRadiansLo ) || ( Diff > dRadiansHi ) ) {
                    At += Clamp( dRadiansLo, Diff, dRadiansHi );
                } else {
                    At  = Dest;
                }
                
                Miner->AtRadians = WrapRadians( At );
            }
            
            if( Miner->Health > 0 ) {
                Miner->Fire_Timer += dT;
                if( Miner->Fire_Timer >= Miner->Fire_TargetTime ) {
                    Miner->Fire_IsActive   = !Miner->Fire_IsActive;
                    Miner->Fire_Timer      = 0.0f;
                    
                    if( Miner->Fire_IsActive ) {
                        Miner->Fire_TargetTime = MINER_LASER_FIRE_TARGET_TIME;
                    } else {
                        Miner->Fire_TargetTime = RandomF32InRange( 2.0f, 5.0f ) + MINER_LASER_WARMUP_TARGET_TIME;
                    }
                    
                    for( int32 iLaser = 0; iLaser < Miner->nLaser; iLaser++ ) {
                        MINER_LASER * Laser = Miner->Laser + iLaser;
                        Laser->IsActive = Miner->Fire_IsActive;
                    }
                }
            } else if( Miner->DoLaserOnlyKill ) {
                Miner->DoLaserOnlyKill_Timer += dT;
            }
        } else {
            Miner->AtRadians = WrapRadians( Miner->AtRadians + Miner->RotateSpeed * dT );
            
            Miner->Fire_Timer = MinValue( Miner->Fire_Timer + dT, Miner->Fire_TargetTime );
            if( Miner->Fire_Timer >= Miner->Fire_TargetTime ) {
                Miner->Fire_IsActive = true;
                for( int32 iLaser = 0; iLaser < Miner->nLaser; iLaser++ ) {
                    MINER_LASER * Laser = Miner->Laser + iLaser;
                    Laser->IsActive = true;
                }
            }
        }
    }
    
    for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
        MEGA_MINER * MegaMiner = MinerS->MegaMiner + iMegaMiner;
        
        {
            vec2  Pos[ MEGA_MINER_EXHAUST_MAX_COUNT ] = {};
            flo32 Rad[ MEGA_MINER_EXHAUST_MAX_COUNT ] = {};
            
            Pos[ 0 ] = MegaMiner->Position + ToDirection2D( MegaMiner->AtRadians + PI * 0.4f ) * ( MegaMiner->Radius * 0.85f );
            Pos[ 1 ] = MegaMiner->Position + ToDirection2D( MegaMiner->AtRadians - PI * 0.4f ) * ( MegaMiner->Radius * 0.85f );
            Pos[ 2 ] = MegaMiner->Position + ToDirection2D( MegaMiner->AtRadians + PI * 0.975f ) * ( MegaMiner->Radius * 0.875f );
            Pos[ 3 ] = MegaMiner->Position + ToDirection2D( MegaMiner->AtRadians - PI * 0.975f ) * ( MegaMiner->Radius * 0.875f );
            
            Rad[ 0 ] = MegaMiner->AtRadians + PI * 0.9f;
            Rad[ 1 ] = MegaMiner->AtRadians - PI * 0.9f;
            Rad[ 2 ] = MegaMiner->AtRadians + PI * 0.95f;
            Rad[ 3 ] = MegaMiner->AtRadians - PI * 0.95f;
            
            for( int32 iExhaust = 0; iExhaust < MEGA_MINER_EXHAUST_MAX_COUNT; iExhaust++ ) {
                MegaMiner->Exhaust_Timer[ iExhaust ] += dT;
                if( MegaMiner->Exhaust_Timer[ iExhaust ] >= MegaMiner->Exhaust_TargetTime[ iExhaust ] ) {
                    MegaMiner->Exhaust_Timer[ iExhaust ]     -= MegaMiner->Exhaust_TargetTime[ iExhaust ];
                    MegaMiner->Exhaust_TargetTime[ iExhaust ] = ( flo32 )RandomS32InRange( 4, 8 ) / 60.0f;
                    
                    PARTICLE Particle = {};
                    Particle.TextureID = TextureID_Dome;
                    
                    flo32 Radians    = Rad[ iExhaust ] + ( RandomF32InRange( -1.0f, 1.0f ) * ( PI / 6.0f ) );
                    vec2  Dir        = ToDirection2D( Radians );
                    flo32 Speed      = RandomF32InRange( 3.0f, 5.0f );
                    flo32 TargetTime = ( flo32 )RandomS32InRange( 30, 50 ) / 60.0f;
                    
                    Particle.Position  = Pos[ iExhaust ];
                    Particle.Velocity  = Dir * Speed;
                    Particle.Friction  = 2.0f;
                    Particle.Dim       = Vec2( 0.25f, 0.25f ) * RandomF32InRange( 0.8f, 1.2f );
                    Particle.dDim      = Vec2( 1.5f,  1.5f  ) * ( RandomF32InRange( 0.9f, 1.1f ) / TargetTime );
                    
                    Particle.TargetTime = TargetTime;
                    
                    flo32 tGray = RandomF32InRange( 0.25f, 0.75f );
                    Particle.Color      = COLOR_GRAY( tGray ) * RandomF32InRange( 0.6f, 0.8f );
                    Particle.dColor     = Vec4( 0.0f, 0.0f, 0.0f, -1.0f / TargetTime );
                    
                    AddParticle( AppState, Particle );
                }
            }
        }
        
        {
            MegaMiner->Drill_Timer += dT;
            if( MegaMiner->Drill_Timer >= MEGA_MINER_DRILL_ANIM_TARGET_TIME ) {
                MegaMiner->Drill_Timer -= MEGA_MINER_DRILL_ANIM_TARGET_TIME;
            }
        }
        
        {
            flo32 dRadians = ( TAU / 0.125f );
            MegaMiner->Rumble_Radians = WrapRadians( MegaMiner->Rumble_Radians + dRadians * dT );
        }
        
        MegaMiner->ShowDamage_Timer = MaxValue( MegaMiner->ShowDamage_Timer - dT, 0.0f );
        MegaMiner->DestRadians      = ToRadians( Player->Position - MegaMiner->Position );
        
        flo32 Dest = MegaMiner->DestRadians;
        flo32 At   = MegaMiner->AtRadians;
        if( Dest != At ) {
            flo32 Diff = GetSmallestDiffInRadians( At, Dest );
            
            flo32 dRadiansHi = ( MegaMiner->RotateSpeed * 2.0f * dT );
            flo32 dRadiansLo = -dRadiansHi;
            if( ( Diff < dRadiansLo ) || ( Diff > dRadiansHi ) ) {
                At += Clamp( dRadiansLo, Diff, dRadiansHi );
            } else {
                At  = Dest;
            }
            
            MegaMiner->AtRadians = WrapRadians( At );
        }
        
        flo32 Speed    = MEGA_MINER_SPEED;
        flo32 Friction = MEGA_MINER_FRICTION;
        
        vec2 Dir   = GetNormal( Player->Position - MegaMiner->Position );
        vec2 Accel = ( Dir * Speed - MegaMiner->Velocity ) * Friction;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + MegaMiner->Velocity * dT;
        // TODO: Collision!
        MegaMiner->Position += dPos;
        MegaMiner->Velocity += Accel * dT;
    }
    
    { // Mega-Miner Drill Damage Miner
        for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
            MEGA_MINER * MegaMiner = MinerS->MegaMiner + iMegaMiner;
            
            vec2 yAxis = GetNormal( ToDirection2D( MegaMiner->AtRadians ) );
            vec2 xAxis = -GetPerp( yAxis );
            
            vec2 bPos  = MegaMiner->Position + yAxis * MEGA_MINER_DRILL_OFFSET;
            vec2 Scale = MEGA_MINER_DRILL_SCALE;
            vec2 A     = bPos + xAxis * -Scale.x + yAxis * -Scale.y;
            vec2 B     = bPos                    + yAxis *  Scale.y;
            vec2 C     = bPos + xAxis *  Scale.x + yAxis * -Scale.y;
            
            for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                MINER * Miner = MinerS->Miner + iMiner;
                if( Miner->Health > 0 ) {
                    flo32 Radius   = ( Miner->Radius * 0.9f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = DistSqPointToTri( Miner->Position, A, B, C );
                    if( DistSq <= RadiusSq ) {
                        Miner->LaserDamage_Timer += ( dT * 2.0f );
                        while( Miner->LaserDamage_Timer >= MINER_LASER_DAMAGE_TARGET_TIME ) {
                            Miner->Health             = MaxValue( Miner->Health - 1, 0 );
                            Miner->LaserDamage_Timer -= MINER_LASER_DAMAGE_TARGET_TIME;
                        }
                        
                        vec2 Dir = GetNormal( Miner->Position - MegaMiner->Position );
                        vec2 Pos = ClosestPointToTriPerimeter( Miner->Position, A, B, C );
                        AddMinerLaserHitParticles( AppState, Pos, Dir );
                    }
                }
            }
        }
    }
    
    { // LASER
        for( int32 iLaser = 0; iLaser < MinerS->nLaser; iLaser++ ) {
            LASER * Laser = MinerS->Laser + iLaser;
            
            {
                vec2 Dir = ToDirection2D( Laser->Radians );
                Laser->Dest = Laser->Position + Dir * Laser->MaxLength;
            }
            
            Laser->Timer += dT;
            if( Laser->Timer >= Laser->Fire_TargetTime ) {
                Laser->IsActive = true;
            } else {
                Laser->IsActive = false;
            }
            
            if( Laser->Timer >= Laser->Reset_TargetTime ) {
                Laser->Timer = 0.0f;
            }
        }
    }
}

internal void
DrawMiner( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, MINER Miner, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    {
        vec4 Color  = MINER_STATIONARY_BASE_COLOR;
        vec4 ColorA = TowardBlack( Color, 0.7f );
        vec4 ColorB = TowardBlack( Color, 0.6f );
        vec4 ColorC = TowardBlack( Color, 0.85f );
        vec4 ColorD = TowardBlack( Color, 0.9f );
        
        if( Miner.IsAutonomous ) {
            Color = MINER_AUTONOMOUS_BASE_COLOR;
            ColorA = TowardBlack( Color, 0.8f );
            ColorB = TowardBlack( Color, 0.7f );
            ColorC = TowardBlack( Color, 0.9f );
            ColorD = TowardBlack( Color, 0.9f );
        }
        
        //vec4 ColorB = COLOR_GRAY( 0.055f );
        //vec4 ColorC = COLOR_GRAY( 0.03f  );
        
        // colorA : base bound
        // colorC : barrel thin
        // colorB : barrel wide
        // colorD : laser base, laser pivot
        
        {
            flo32 tDamage = Clamp01( Miner.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
            vec4 ColorE = lerp( Color, 0.25f, COLOR_WHITE );
            ColorA = lerp( ColorA, tDamage, ColorE );
        }
        
        
        DrawRect( Pass, TextureID_RoundedDome, RectCR( Miner.Position, Miner.Radius * Scale ), ColorA * Alpha );
        //DrawCircle( Pass, Miner.Position, Miner.Radius * Scale, ColorA * Alpha );
        
        if( Miner.nLaser > 2 ) {
            DrawRect( Pass, TextureID_Dome, RectCR( Miner.Position, Miner.Radius * Scale * 0.6f ), ColorD * Alpha );
            //DrawCircle( Pass, Miner.Position, Miner.Radius * Scale * 0.6f, ColorD * Alpha );
        } else {
            DrawRect( Pass, TextureID_Dome, RectCR( Miner.Position, Miner.Radius * Scale * 0.5f ), ColorD * Alpha );
            //DrawCircle( Pass, Miner.Position, Miner.Radius * Scale * 0.5f, ColorD * Alpha );
        }
        
        boo32 DoWarmUpLaser  = false;
        flo32 DoWarmUpLaserT = 0.35f;
        flo32 TargetTimeA = Miner.Fire_TargetTime - MINER_LASER_WARMUP_TARGET_TIME;
        if( ( !Miner.Fire_IsActive ) && ( Miner.Fire_Timer >= TargetTimeA ) ) {
            flo32 Numer = ( Miner.Fire_Timer - TargetTimeA );
            flo32 Denom = ( MINER_LASER_WARMUP_TARGET_TIME - ( 20.0f / 60.0f ) );
            flo32 t = Clamp01( Numer / Denom );
            t *= t;
            
            vec4 ColorE = lerp( ColorB, 0.3f,  COLOR_WHITE );
            vec4 ColorF = lerp( ColorC, 0.15f, COLOR_WHITE );
            
            ColorB = lerp( ColorB, t, ColorE );
            ColorC = lerp( ColorC, t, ColorF );
            
            DoWarmUpLaser  = true;
            DoWarmUpLaserT = t;
        }
        
        for( int32 iLaser = 0; iLaser < Miner.nLaser; iLaser++ ) {
            MINER_LASER Laser = Miner.Laser[ iLaser ];
            
            flo32 Radians = Miner.AtRadians + Laser.Radians;
            vec2  Dir     = ToDirection2D( Radians );
            
            orect BoundB = ORectCD( Miner.Position, Vec2( 1.6f, 0.6f ) * ( Miner.Radius * Scale ), Radians );
            if( Miner.nLaser > 1 ) {
                BoundB = ORectTD( Miner.Position, Vec2( 0.0f, 0.5f ), Vec2( 0.8f, 0.6f ) * ( Miner.Radius * Scale ), Radians );
            }
            orect BoundC = ORectBCD( Miner.Position, Vec2( 0.325f, 0.925f ) * ( Miner.Radius * Scale ), Radians );
            
            DrawORect( Pass, BoundC, ColorC * Alpha );
            DrawORect( Pass, TextureID_Barrel_Rot90, BoundB, ColorB * Alpha );
            
            if( iLaser == 0 ) {
                vec2 Pos = Miner.Position + Dir * ( Miner.Radius * 0.6f * Scale );
                vec2 Sca = Vec2( 0.0625f, 0.1125f ) * ( Miner.Radius * Scale );
                DrawTriangle( Pass, Pos, Sca, Radians - PI * 0.5f, Vec4( 0.75f, 0.0f, 0.0f, 1.0f ) * Alpha );
            }
            
            if( ( Miner.Health > 0 ) && ( ( DoWarmUpLaser ) || ( Laser.IsActive ) ) ) {
                vec2 A   = Miner.Position + Dir * Miner.Radius * 0.9f;
                vec2 B   = Miner.Position + Dir * Miner.Laser_MaxLength;
                DrawLine( Pass, A, B, Vec4( DoWarmUpLaserT, 0.0f, 0.0f, DoWarmUpLaserT ) );
            }
            
            if( Laser.IsActive ) {
                vec2  bPos   = GetT( BoundC, Vec2( 0.5f, 1.0f ) );
                flo32 Length = GetLength( Laser.Dest - bPos );
                flo32 Margin = MINER_LASER_WIDTH * 0.75f;
                flo32 Width  = MINER_LASER_WIDTH + Margin * 2.0f;
                
                if( Scale > 1.0f ) {
                    Length += lerp( 0.0f, Scale - 1.0f, 4.0f );
                    
                    Margin *= Scale * 1.5f;
                    Width  *= Scale * 1.5f;
                }
                
                orect BoundD = ORectBCD( bPos, Vec2( Width, Length ), Radians );
                DrawORect( Pass, TextureID_MinerLaser_Body, BoundD, COLOR_WHITE * Alpha );
                
                {
                    orect BoundE = ORectBCD( GetT( BoundD, Vec2( 0.5f, 0.0f ) ), Vec2( Width, Margin ), Radians + PI );
                    DrawORect( Pass, TextureID_MinerLaser_End, BoundE, COLOR_WHITE * Alpha );
                }
                
                {
                    orect BoundE = ORectBCD( GetT( BoundD, Vec2( 0.5f, 1.0f ) ), Vec2( Width, Margin ), Radians );
                    DrawORect( Pass, TextureID_MinerLaser_End, BoundE, COLOR_WHITE * Alpha );
                }
                
                if( ( Miner.DoLaserOnlyKill ) && ( Miner.DoLaserOnlyKill_iLaser == ( int32 )iLaser ) ) {
                    flo32 Denom = MINER_LASER_ONLY_KILL_TARGET_TIME - ( 2.0f / 60.0f );
                    flo32 t     = Clamp01( Miner.DoLaserOnlyKill_Timer / Denom );
                    
                    uint32 MaxSegment = 8;
                    flo32  xOffset  = MINER_LASER_WIDTH;
                    flo32  yOffset  = ( Length + Miner.Radius ) / ( flo32 )MaxSegment;
                    
                    uint32 nSegment = ( uint32 )( ( flo32 )MaxSegment * t + 1.0f );
                    
                    vec2   yAxis = Dir * -yOffset;
                    vec2   xAxis = GetPerp( GetNormal( yAxis ) ) * xOffset;
                    
                    vec2 AtPos = bPos + Dir * ( Length + 0.5f );
                    for( uint32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                        vec2 A = AtPos + xAxis;
                        vec2 B = AtPos - xAxis + yAxis;
                        
                        orect Bound = ORectYAxis( A, B, 0.35f );
                        DrawORect( Pass, Bound, COLOR_WHITE );
                        
                        AtPos +=  yAxis;
                        xAxis  = -xAxis;
                    }
                }
            }
        }
        
        DrawCircle( Pass, Miner.Position, Miner.Radius * Scale * 0.35f, ( TowardWhite( ColorD, 0.025f ) ) * Alpha );
        
        {
            if( ( Miner.DoLaserOnlyKill ) && ( Miner.DoLaserOnlyKill_Timer > ( MINER_LASER_ONLY_KILL_TARGET_TIME - ( 2.0f / 60.0f ) ) ) ) {
                DrawCircle( Pass, Miner.Position, Miner.Radius * 1.1f, COLOR_WHITE );
            }
        }
        
        if( Miner.Health > 0 ) {
            DrawEntityHealth( Pass, AppState, Camera, Miner.Position, Miner.Health, COLOR_WHITE );
        }
    }
}

internal void
DrawMegaMiner( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, MEGA_MINER MegaMiner, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    {
        vec4 Color_Body = TowardBlack( MEGA_MINER_BASE_COLOR, 0.3f );
        
        if( MegaMiner.ShowDamage_Timer > 0.0f ) {
            flo32 HalfTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 0.5f;
            uint32 Cycle = ( uint32 )( MegaMiner.ShowDamage_Timer / HalfTime );
            if( ( Cycle % 2 ) == 1 ) {
                Color_Body = TowardWhite( Color_Body, 0.25f );
            } else {
                Color_Body = TowardWhite( Color_Body, 0.0f );
            }
        }
        
        flo32 tRumble       = cosf( MegaMiner.Rumble_Radians );
        flo32 tRadians      = cosf( MegaMiner.Rumble_Radians - PI * 0.5f );
        flo32 AtRadians     = MegaMiner.AtRadians + tRadians * ( TAU / 512.0f );
        vec2  Rumble_Offset = ToDirection2D( AtRadians + PI * 0.5f ) * ( MegaMiner.Radius * 0.02f * tRumble );
        vec2 bPos = MegaMiner.Position + Rumble_Offset;
        
        flo32 Radius = MegaMiner.Radius * Scale;
        
        vec2 DirA = ToDirection2D( AtRadians );
        vec2 PosA = bPos + DirA * Radius * 1.0f;
        vec2 ScaA = Vec2( Radius + 0.25f, Radius * 0.5f - 0.25f );
        vec2 ScaB = ScaA * Vec2( 0.65f, 0.6f );
        
        DrawTriangle( Pass, PosA, ScaA, AtRadians - PI * 0.5f, COLOR_RED * Alpha );
        DrawORect( Pass, ORectTCD( PosA + DirA * -ScaA.y, Vec2( ScaA.x * 2.0f, 0.25f ), AtRadians ), COLOR_RED * Alpha );
        
        
        //DrawRect( Pass, TextureID_MegaMiner_Body, RectCR( bPos, MegaMiner.Radius        * Scale, AtRadians ), ColorA * Alpha );
        //DrawRect( Pass, TextureID_Dome, RectCR( bPos, MegaMiner.Radius * 0.9f * Scale ), ColorB * Alpha );
        //DrawCircle( Pass, bPos, MegaMiner.Radius        * Scale, ColorA * Alpha );
        //DrawCircle( Pass, bPos, MegaMiner.Radius * 0.9f * Scale, ColorB * Alpha );
        
        //DrawTriangle( Pass, PosA, ScaB, AtRadians - PI * 0.5f, COLOR_RED );
        //DrawORect( Pass, ORectTCD( PosA + DirA * -ScaB.y, Vec2( ScaB.x * 1.9f, 0.15f ), AtRadians ), ColorC );
        
        {
            int32 nThread = 8;
            
            vec2 yAxis = GetNormal( ToDirection2D( MegaMiner.AtRadians ) );
            vec2 xAxis = -GetPerp( yAxis );
            
            vec2 Drill_Pos = MegaMiner.Position + yAxis * Radius;
            vec2 Drill_Sca = Vec2( 1.083333f, 0.416667f ) * Radius;
            vec2 A         = Drill_Pos + xAxis * -Drill_Sca.x + yAxis * -Drill_Sca.y;
            vec2 B         = Drill_Pos                        + yAxis *  Drill_Sca.y;
            vec2 C         = Drill_Pos + xAxis *  Drill_Sca.x + yAxis * -Drill_Sca.y;
            
            for( int32 iThread = 0; iThread < nThread; iThread++ ) {
                flo32 t       = Clamp01( ( flo32 )  iThread       / ( flo32 )nThread );
                flo32 t0      = Clamp01( MegaMiner.Drill_Timer / MEGA_MINER_DRILL_ANIM_TARGET_TIME ) / ( flo32 )nThread;
                vec2  P       = lerp( B, t + t0, C );
                flo32 Radians = MegaMiner.AtRadians + PI * 0.495f;
                
                RAY2_INTERSECT Intersect = Ray2IntersectInit();
                DoesRayIntersectEdge( &Intersect, Ray2( P,  ToDirection2D( Radians ) * Radius * 3.0f ), B, A );
                
                if( ( Intersect.IsValid ) && ( Intersect.t <= 1.0f ) ) {
                    vec4 Color = COLOR_WHITE;
                    flo32 tTarget = 0.95f;
                    if( ( t + t0 ) >= tTarget ) {
                        flo32 Alph = 1.0f - Clamp01( ( t + t0 - tTarget ) / ( 1.0f - tTarget ) );
                        Color *= Alph;
                    }
                    
                    DrawORect( Pass, TextureID_MinerLaser_Body, ORectTD( P, Vec2( 0.5f, 0.0f ), Vec2( Radius * 0.2f, GetLength( P - Intersect.P ) ), Radians - PI * 0.5f ), Color * Alpha );
                    //DrawLine( Pass, P, Intersect.P, Color );
                }
            }
        }
        
        DrawORect( Pass, TextureID_MegaMiner_Body, ORectCR( bPos, Radius, AtRadians ), Color_Body * Alpha );
        
        vec2 Window_Pos = bPos + ToDirection2D( AtRadians ) * ( Radius * 0.675f  );
        DrawORect( Pass, TextureID_Dome, ORectTD( Window_Pos, Vec2( 1.0f, 0.5f ), Vec2( 0.25f, 0.15f ) * Radius, AtRadians ), COLOR_GRAY( 0.1f ) * Alpha );
        DrawORect( Pass, TextureID_Dome, ORectTD( Window_Pos, Vec2( 1.0f, 0.5f ), Vec2( 0.1f, 0.075f ) * Radius, AtRadians ), COLOR_GRAY( 0.065f ) * Alpha );
        
        {
            vec2 Dim = Vec2( 0.2f, 0.1f ) * Radius;
            flo32 RadiansA = AtRadians + PI * 0.4f;
            flo32 RadiansB = AtRadians - PI * 0.4f;
            
            DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( bPos + ToDirection2D( RadiansA ) * ( Radius * 0.85f ), Dim, RadiansA + PI * 0.5f ), COLOR_GRAY( 0.15f ) * Alpha );
            DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( bPos + ToDirection2D( RadiansB ) * ( Radius * 0.85f ), Dim, RadiansB - PI * 0.5f ), COLOR_GRAY( 0.15f ) * Alpha );
        }
        
        {
            vec2  Dim     = Vec2( 0.1f, 0.5f ) * Radius;
            flo32 Radians = AtRadians + PI;
            
            DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( bPos + ToDirection2D( Radians ) * ( Radius * 0.875f ), Dim, Radians ), COLOR_GRAY( 0.15f ) * Alpha );
        }
        
        if( MegaMiner.Health > 0 ) {
            DrawEntityHealth( Pass, AppState, Camera, bPos, MegaMiner.Health, COLOR_WHITE );
        }
    }
}

internal void
DrawLaser( RENDER_PASS * Pass, LASER Laser ) {
    
    boo32 DoWarmUpLaser  = false;
    flo32 DoWarmUpLaserT = 0.35f;
    
    flo32 TargetTimeA   = Laser.Fire_TargetTime - MINER_LASER_WARMUP_TARGET_TIME;
    if( Laser.Timer >= TargetTimeA ) {
        flo32 Numer = ( Laser.Timer - TargetTimeA );
        flo32 Denom = ( MINER_LASER_WARMUP_TARGET_TIME - ( 20.0f / 60.0f ) );
        flo32 t = Clamp01( Numer / Denom );
        t *= t;
        
        DoWarmUpLaser  = true;
        DoWarmUpLaserT = t;
    }
    
    if( DoWarmUpLaser ) {
        vec2 A   = Laser.Position;
        vec2 B   = Laser.Position + ToDirection2D( Laser.Radians ) * Laser.MaxLength;
        DrawLine( Pass, A, B, Vec4( DoWarmUpLaserT, 0.0f, 0.0f, DoWarmUpLaserT ) );
    }
    
    vec2  bPos    = Laser.Position;
    flo32 Radians = Laser.Radians;
    
    if( Laser.IsActive ) {
        flo32 Length = GetLength( Laser.Dest - bPos );
        
        orect BoundD = ORectBCD( bPos, Vec2( MINER_LASER_WIDTH, Length ), Radians );
        DrawORect( Pass, BoundD, COLOR_RED );
    }
}

internal void
DrawMiner( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    MINER_STATE * MinerS = Draw->MinerS;
    
    for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
        MEGA_MINER MegaMiner = MinerS->MegaMiner[ iMegaMiner ];
        
        DrawMegaMiner( Pass, AppState, Camera, MegaMiner );
    }
    
    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
        MINER Miner = MinerS->Miner[ iMiner ];
        
        DrawMiner( Pass, AppState, Camera, Miner );
    }
}

internal void
FinalizeMiner( APP_STATE * AppState ) {
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    MINER_STATE * MinerS = &AppState->MinerS;
    
    { // MINER
        uint32 NewCount = 0;
        for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
            MINER Miner = MinerS->Miner[ iMiner ];
            
            boo32 DoKeep = ( Miner.Health > 0 ) || ( ( Miner.DoLaserOnlyKill ) && ( Miner.DoLaserOnlyKill_Timer < MINER_LASER_ONLY_KILL_TARGET_TIME ) );
            
            if( DoKeep ) {
                MinerS->Miner[ NewCount++ ] = Miner;
            } else {
                if( Miner.IsAutonomous ) {
                    AddKillToScore( AppState, EntityType_Miner, Miner.Position );
                } else {
                    AddEntityToKilledCountForLevel( AppState, EntityType_Miner_Stationary, 1 );
                }
                
                OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
                POINTS_ENTRY                Points     = OverworldT->Points[ EntityType_Miner ];
                
                AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( Miner.Position, Points.nScoreMultiplier ) );
                
                vec4 bColor = MINER_STATIONARY_BASE_COLOR;
                if( Miner.IsAutonomous ) {
                    bColor = MINER_AUTONOMOUS_BASE_COLOR;
                } else {
                    AddBigEntityDeathParticles( AppState, Miner.Position, Miner.Radius, bColor );
                }
                
                AddEntityDeathParticles( AppState, Miner.Position, bColor );
                AddEntityDeathParticles( AppState, Miner.Position, bColor );
                AddEntityDeathParticles( AppState, Miner.Position, bColor );
                AddEntityDeathParticles( AppState, Miner.Position, COLOR_WHITE, false );
                AddKillToWave( AppState, Miner.WaveGuid );
                
                MinerS->BonusObjective_nMinerKills += 1;
                MinerS->BonusObjective_DoNext       = true;
                
                PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Medium );
            }
        }
        MinerS->nMiner = NewCount;
        
        PlayMinerLaserAudio( AppState );
    }
    
    { // MEGA_MINER
        uint32 NewCount = 0;
        for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
            MEGA_MINER MegaMiner = MinerS->MegaMiner[ iMegaMiner ];
            
            boo32 DoKeep = ( MegaMiner.Health > 0 );
            
            if( DoKeep ) {
                MinerS->MegaMiner[ NewCount++ ] = MegaMiner;
            } else {
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, COLOR_WHITE, false );
                
                AddEntityDeathParticles( AppState, MegaMiner.Position, MEGA_MINER_BASE_COLOR );
                AddEntityDeathParticles( AppState, MegaMiner.Position, MEGA_MINER_BASE_COLOR );
                
                AddKillToWave( AppState, MegaMiner.WaveGuid );
                AddEntityToKilledCountForLevel( AppState, EntityType_MegaMiner, 1 );
                
                MinerS->BonusObjective_nMegaMinerKills += 1;
                
                PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Large );
            }
        }
        MinerS->nMegaMiner = NewCount;
    }
}