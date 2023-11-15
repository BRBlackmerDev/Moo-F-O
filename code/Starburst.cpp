
//----------
// STAR
//----------

internal STAR
StarC( int32 WaveGuid, vec2 Position, boo32 DoExplode = true ) {
    vec2  Direction = ToDirection2D( RandomF32() * TAU );
    flo32 Speed     = RandomF32InRange( 1.0f, 2.0f );
    flo32 Dist      = RandomF32InRange( 0.0f, 1.0f );
    vec2  Velocity  = Direction * Speed;
    if( DoExplode ) {
        Velocity = Direction * ( Speed * 10.0f );
    }
    
    STAR Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Velocity = Velocity;
    Result.Health   = 1;
    
    return Result;
}

internal void
AddStar( APP_STATE * AppState, STAR Star ) {
    STARBURST_STATE * StarburstS = &AppState->StarburstS;
    if( StarburstS->nStar < STAR_MAX_COUNT ) {
        StarburstS->Star[ StarburstS->nStar++ ] = Star;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add STAR to STARBURST_STATE: %u", STAR_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
Star_AddSparkParticles( APP_STATE * AppState, vec2 PosA, vec2 PosB ) {
    vec2 V = PosB - PosA;
    
    PARTICLE Particle = {};
    Particle.Position   = ( PosB + PosA ) * 0.5f;
    Particle.Dim        = Vec2( GetLength( V ), 0.075f );
    Particle.Radians    = ToRadians( V );
    Particle.TargetTime = ( 4.0f / 60.0f );
    Particle.Color      = COLOR_CYAN;
    AddParticle( AppState, Particle );
    
    flo32 RadiansA = Particle.Radians + ( PI * 0.3f );
    flo32 RadiansB = Particle.Radians - ( PI * 0.3f );
    vec2  DirA     = ToDirection2D( RadiansA );
    vec2  DirB     = ToDirection2D( RadiansB );
    
    vec2  Spark_Dim = Vec2( Particle.Dim.x * 0.5f, 0.05f );
    
    PARTICLE ParticleA = {};
    ParticleA.Position   = PosB + DirA * ( Spark_Dim.x * 0.5f );
    ParticleA.Dim        = Spark_Dim;
    ParticleA.Radians    = RadiansA;
    ParticleA.TargetTime = ( 7.0f / 60.0f );
    ParticleA.Color      = Vec4( 0.0f, 0.8f, 0.8f, 1.0f );
    AddParticle( AppState, ParticleA );
    
    PARTICLE ParticleB = {};
    ParticleB.Position   = PosB + DirB * ( Spark_Dim.x * 0.5f );
    ParticleB.Dim        = Spark_Dim;
    ParticleB.Radians    = RadiansB;
    ParticleB.TargetTime = ( 7.0f / 60.0f );
    ParticleB.Color      = Vec4( 0.0f, 0.8f, 0.8f, 1.0f );
    AddParticle( AppState, ParticleB );
}

internal void
UpdateStar( APP_STATE * AppState, flo32 dT ) {
    STARBURST_STATE * StarburstS = &AppState->StarburstS;
    SWARM_STATE     * SwarmS     = &AppState->SwarmS;
    TURRET_STATE    * TurretS    = &AppState->TurretS;
    PLAYER_STATE    * Player     = &AppState->Player;
    
    for( uint32 iStar = 0; iStar < StarburstS->nStar; iStar++ ) {
        STAR * Star = StarburstS->Star + iStar;
        
        for( uint32 iRepel = iStar + 1; iRepel < StarburstS->nStar; iRepel++ ) {
            STAR * Repel = StarburstS->Star + iRepel;
            
            vec2 V = Repel->Position - Star->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = STAR_REPEL_RADIUS + STAR_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * STAR_REPEL_SPEED;
                
                Star->Repel -= Repel_Vector;
                Repel->Repel  += Repel_Vector;
            }
        }
        
        vec2 Dir   = GetNormal( Player->Position - Star->Position );
        vec2 Accel = ( Dir * STAR_SPEED - Star->Velocity + Star->Repel ) * STAR_FRICTION;
        if( Player->GravityWell_IsActive ) {
            Accel += GetPerp( Dir ) * ( Player->Rotate_Speed * STAR_FRICTION * 6.0f );
        }
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Star->Velocity * dT;
        // TODO: Collision!
        Star->Position += dPos;
        Star->Velocity += Accel * dT;
        
        Star->Repel = {};
    }
    
    for( uint32 iStar = 0; iStar < StarburstS->nStar; iStar++ ) {
        STAR * Star = StarburstS->Star + iStar;
        
        { // SWARM
            for( int32 iSwarmA = 0; ( Star->Health > 0 ) && ( iSwarmA < SwarmS->nSwarm ); iSwarmA++ ) {
                SWARM * SwarmA = SwarmS->Swarm + iSwarmA;
                if( SwarmA->Health > 0 ) {
                    flo32 DistSqA   = GetLengthSq( SwarmA->Position - Star->Position );
                    flo32 RadiusA   = STAR_RADIUS + SWARM_RADIUS;
                    flo32 RadiusSqA = RadiusA * RadiusA;
                    if( DistSqA <= RadiusSqA ) {
                        for( int32 iSwarmB = 0; iSwarmB < SwarmS->nSwarm; iSwarmB++ ) {
                            SWARM * SwarmB = SwarmS->Swarm + iSwarmB;
                            
                            if( SwarmB->Health > 0 ) {
                                flo32 DistSqB   = GetLengthSq( SwarmB->Position - Star->Position );
                                flo32 RadiusB   = STAR_EXPLODE_RADIUS + SWARM_RADIUS;
                                flo32 RadiusSqB = RadiusB * RadiusB;
                                if( DistSqB <= RadiusSqB ) {
                                    int32 SwarmB_Damage = MinValue( SwarmB->Health, 1 );
                                    SwarmB->Health -= SwarmB_Damage;
                                    
                                    Star_AddSparkParticles( AppState, Star->Position, SwarmB->Position );
                                }
                            }
                        }
                        
                        int32 Star_Damage = MinValue( Star->Health, 1 );
                        Star->Health -= Star_Damage;
                    }
                }
            }
        }
        
        { // TURRET
            for( int32 iTurret = 0; ( Star->Health > 0 ) && ( iTurret < TurretS->nTurret ); iTurret++ ) {
                TURRET * Turret = TurretS->Turret + iTurret;
                
                flo32 DistSq   = GetLengthSq( Turret->Position - Star->Position );
                flo32 Radius   = Turret->Radius + STAR_RADIUS;
                flo32 RadiusSq = Radius * Radius;
                if( DistSq <= RadiusSq ) {
                    int32 Star_Damage = MinValue( Star->Health, ( int32 )1 );
                    Star->Health -= Star_Damage;
                    
                    int32 Target_Damage = MinValue( Turret->Health, ( int32 )5 );
                    Turret->Health -= Target_Damage;
                    
                    Star_AddSparkParticles( AppState, Star->Position, Turret->Position );
                }
            }
        }
    }
    
    //DISPLAY_VALUE( uint32, StarburstS->nStar );
}

internal void
DrawStar( RENDER_PASS * Pass, STAR Star, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec2 DimA = Vec2( STAR_RADIUS, STAR_RADIUS ) * 2.0f * Scale;
    vec2 DimB = DimA * 0.8f;
    
    orect BoundA = ORectCD( Star.Position, DimA, PI * 0.25f );
    orect BoundB = ORectCD( Star.Position, DimB, PI * 0.25f );
    vec4 ColorA = ToColor( 235, 90,  125 ) * Alpha;
    vec4 ColorB = ToColor( 255, 110, 145 ) * Alpha;
    
    DrawORect( Pass, BoundA, ColorA );
    DrawORect( Pass, BoundB, ColorB );
}

internal void
DrawStar( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player  = Draw->Player;
    STARBURST_STATE * StarburstS = Draw->StarburstS;
    
    for( uint32 iStar = 0; iStar < StarburstS->nStar; iStar++ ) {
        STAR Star = StarburstS->Star[ iStar ];
        
        DrawStar( Pass, Star );
    }
}

internal void
FinalizeStar( APP_STATE * AppState ) {
    SCORE_STATE     * ScoreS     = &AppState->ScoreS;
    STARBURST_STATE * StarburstS = &AppState->StarburstS;
    
    uint32 NewCount = 0;
    for( uint32 iStar = 0; iStar < StarburstS->nStar; iStar++ ) {
        STAR Star = StarburstS->Star[ iStar ];
        
        if( Star.Health > 0 ) {
            StarburstS->Star[ NewCount++ ] = Star;
        } else {
            AddKillToScore( AppState, EntityType_Star, Star.Position );
            AddScoreMultiplier( AppState, Star_NewScoreMultiplier( Star.Position, GetNormal( -Star.Velocity ) ) );
            
            AddEntityDeathParticles( AppState, Star.Position, ToColor( 255, 110, 145 ) );
            AddKillToWave( AppState, Star.WaveGuid );
        }
    }
    StarburstS->nStar = NewCount;
}

//----------
// STARBURST
//----------

internal STARBURST
StarburstC( int32 WaveGuid, vec2 Position ) {
    STARBURST Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Health   = 1;
    
    return Result;
}

internal void
AddStarburst( APP_STATE * AppState, STARBURST Starburst ) {
    STARBURST_STATE * StarburstS = &AppState->StarburstS;
    if( StarburstS->nStarburst < STARBURST_MAX_COUNT ) {
        StarburstS->Starburst[ StarburstS->nStarburst++ ] = Starburst;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add STARBURST to STARBURST_STATE: %u", STARBURST_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateStarburst( APP_STATE * AppState, flo32 dT ) {
    STARBURST_STATE * StarburstS = &AppState->StarburstS;
    PLAYER_STATE    * Player     = &AppState->Player;
    
    for( uint32 iStarburst = 0; iStarburst < StarburstS->nStarburst; iStarburst++ ) {
        STARBURST * Starburst = StarburstS->Starburst + iStarburst;
        
        for( uint32 iRepel = iStarburst + 1; iRepel < StarburstS->nStarburst; iRepel++ ) {
            STARBURST * Repel = StarburstS->Starburst + iRepel;
            
            vec2 V = Repel->Position - Starburst->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = STARBURST_REPEL_RADIUS + STARBURST_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * STARBURST_REPEL_SPEED;
                
                Starburst->Repel -= Repel_Vector;
                Repel->Repel  += Repel_Vector;
            }
        }
        
        vec2 Dir   = GetNormal( Player->Position - Starburst->Position );
        vec2 Accel = ( Dir * STARBURST_SPEED - Starburst->Velocity + Starburst->Repel ) * STARBURST_FRICTION;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Starburst->Velocity * dT;
        // TODO: Collision!
        Starburst->Position += dPos;
        Starburst->Velocity += Accel * dT;
        
        Starburst->Repel = {};
    }
}

internal void
DrawStarburst( RENDER_PASS * Pass, STARBURST Starburst, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec2 DimA = Vec2( STARBURST_RADIUS, STARBURST_RADIUS ) * 2.0f * Scale;
    vec2 DimB = DimA * 0.8f;
    
    orect BoundA = ORectCD( Starburst.Position, DimA, PI * 0.25f );
    orect BoundB = ORectCD( Starburst.Position, DimB, PI * 0.25f );
    vec4 ColorA = ToColor( 235, 90,  125 ) * Alpha;
    vec4 ColorB = ToColor( 255, 110, 145 ) * Alpha;
    
    DrawORect( Pass, BoundA, ColorA );
    DrawORect( Pass, BoundB, ColorB );
}

internal void
DrawStarburst( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player  = Draw->Player;
    STARBURST_STATE * StarburstS = Draw->StarburstS;
    
    for( uint32 iStarburst = 0; iStarburst < StarburstS->nStarburst; iStarburst++ ) {
        STARBURST Starburst = StarburstS->Starburst[ iStarburst ];
        
        DrawStarburst( Pass, Starburst );
    }
}

internal void
FinalizeStarburst( APP_STATE * AppState ) {
    SCORE_STATE     * ScoreS     = &AppState->ScoreS;
    STARBURST_STATE * StarburstS = &AppState->StarburstS;
    
    uint32 NewCount = 0;
    for( uint32 iStarburst = 0; iStarburst < StarburstS->nStarburst; iStarburst++ ) {
        STARBURST Starburst = StarburstS->Starburst[ iStarburst ];
        
        if( Starburst.Health > 0 ) {
            StarburstS->Starburst[ NewCount++ ] = Starburst;
        } else {
            AddEntityDeathParticles( AppState, Starburst.Position, ToColor( 255, 110, 145 ) );
            
            AddStar( AppState, StarC( Starburst.WaveGuid, Starburst.Position ) );
            AddStar( AppState, StarC( Starburst.WaveGuid, Starburst.Position ) );
            AddStar( AppState, StarC( Starburst.WaveGuid, Starburst.Position ) );
            
            AddKillToWave( AppState, Starburst.WaveGuid );
        }
    }
    StarburstS->nStarburst = NewCount;
}