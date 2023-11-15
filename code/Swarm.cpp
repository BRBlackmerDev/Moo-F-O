
internal SWARM
SwarmC( int32 WaveGuid, vec2 Position ) {
    SWARM Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Radius   = SWARM_RADIUS;
    Result.Health   = 1;
    
    return Result;
}

internal void
AddSwarm( APP_STATE * AppState, SWARM Swarm ) {
    SWARM_STATE * SwarmS = &AppState->SwarmS;
    if( SwarmS->nSwarm < SWARM_MAX_COUNT ) {
        SwarmS->Swarm[ SwarmS->nSwarm++ ] = Swarm;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SWARM to SWARM_STATE: %u", SWARM_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateSwarm( APP_STATE * AppState, flo32 dT ) {
    SWARM_STATE  * SwarmS = &AppState->SwarmS;
    PLAYER_STATE * Player = &AppState->Player;
    
    { // Swarm Spawn
        SWARM_SPAWN_SETTINGS * Spawn = &SwarmS->Spawn;
        
        if( Spawn->IsActive ) {
            int32 nActiveSwarm = SwarmS->nSwarm;
            ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
            for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
                ENTITY EntitySpawn = EntitySpawnS->EntitySpawn[ iEntitySpawn ];
                if( EntitySpawn.Type == EntityType_Swarm ) {
                    nActiveSwarm += 1;
                }
            }
            
            switch( Spawn->Style ) {
                case SwarmSpawnStyle_Group: {
                    if( ( Spawn->SpawnCounter <= 0 ) && ( nActiveSwarm <= Spawn->nRespawnAt ) ) {
                        Spawn->SpawnCounter = Spawn->nMaxSpawn - nActiveSwarm;
                        Spawn->Delay_Timer  = Spawn->Delay_TargetTime;
                        Spawn->Position     = Player->Position;
                    }
                    
                    if( Spawn->SpawnCounter > 0 ) {
                        Spawn->Delay_Timer += dT;
                        if( Spawn->Delay_Timer >= Spawn->Delay_TargetTime ) {
                            Spawn->SpawnCounter -= 1;
                            
                            flo32 Outer = Spawn->OuterRadius;
                            flo32 Inner = Spawn->InnerRadius;
                            boo32 IsValid = false;
                            vec2  Pos     = {};
                            
                            while( !IsValid ) {
                                flo32 Radians = RandomF32() * TAU;
                                vec2  Dir     = ToDirection2D( Radians );
                                flo32 Dist    = RandomF32InRange( Inner, Outer );
                                
                                Pos = Spawn->Position + Dir * Dist;
                                if( IsInRect( Pos, AppState->World_Bound ) ) {
                                    IsValid = true;
                                }
                            }
                            
                            AddEntitySpawn( AppState, GenValIdentity( Spawn->WaveGuid, EntityType_Swarm, Pos ) );
                        }
                    }
                } break;
                
                case SwarmSpawnStyle_Stream: {
                    InvalidCodePath;
                    
                    
#if 0                    
                    uint32 MaxSwarm = Spawn->nSpawn;
                    flo32  Delay    = Spawn->Delay;
                    
                    MARKET_STATE_SAVE_DATA * MarketS = &AppState->MarketS.SaveData;
                    PRODUCT_ENTRY  Product = MarketS->Product[ EntityType_Swarm ];
                    if( ( Product.Flags & PRODUCT_ENTRY_FLAG__IS_AVAILABLE ) && ( Product.Percent != 1.0f ) ) {
                        MaxSwarm = ( uint32 )( ( flo32 )MaxSwarm * Product.Percent );
                        Delay    = Delay / Product.Percent;
                    }
                    
                    if( ( SwarmS->nSwarm <= MaxSwarm ) && ( Spawn->Timer >= Spawn->Delay ) ) {
                        flo32 Outer = Spawn->OuterRadius;
                        flo32 Inner = Spawn->InnerRadius;
                        boo32 IsValid = false;
                        vec2  Pos     = {};
                        
                        while( !IsValid ) {
                            flo32 Radians = RandomF32() * TAU;
                            vec2  Dir     = ToDirection2D( Radians );
                            flo32 Dist    = RandomF32InRange( Inner, Outer );
                            
                            Pos = Player->Position + Dir * Dist;
                            if( IsInRect( Pos, AppState->World_Bound ) ) {
                                IsValid = true;
                            }
                        }
                        
                        AddEntitySpawn( AppState, GenValIdentity( Spawn->WaveGuid, EntityType_Swarm, Pos ) );
                        
                        Spawn->Timer = 0.0f;
                    }
#endif
                    
                } break;
                
                case SwarmSpawnStyle_Point_Level05: {
                    // 126 / 4 = 32.5 * 32 = 1000ish
                    flo32 Spawn_TargetTime = 3.0f;
                    int32 Spawn_MaxCount   = 64;
                    
                    if( nActiveSwarm < 4 ) {
                        SwarmS->Spawn_Timer = MinValue( SwarmS->Spawn_Timer, 1.0f );
                    }
                    
                    SwarmS->Spawn_Timer = MaxValue( SwarmS->Spawn_Timer - dT, 0.0f );
                    if( SwarmS->Spawn_Timer <= 0.0f ) {
                        if( nActiveSwarm < Spawn_MaxCount ) {
                            SwarmS->Spawn_Timer = Spawn_TargetTime;
                            
                            int32 nToSpawn = RandomS32InRange( 16, 24 );
                            nToSpawn = MinValue( nToSpawn, Spawn_MaxCount - nActiveSwarm );
                            
                            Spawn->SpawnCounter = nToSpawn;
                            Spawn->Delay_Timer  = Spawn->Delay_TargetTime;
                        } else {
                            SwarmS->Spawn_Timer = 1.0f;
                        }
                    }
                    
                    //if( ( Spawn->SpawnCounter <= 0 ) && ( nActiveSwarm <= Spawn->nRespawnAt ) ) {
                    //Spawn->SpawnCounter = Spawn->nMaxSpawn - nActiveSwarm;
                    //Spawn->Delay_Timer  = Spawn->Delay_TargetTime;
                    //}
                    
                    int32 nPoint      = 2;
                    vec2   Point[ 2 ] = {};
                    Point[ 0 ] = GetT( AppState->World_Bound, Vec2( 0.9f, 0.1f ) );
                    Point[ 1 ] = GetT( AppState->World_Bound, Vec2( 0.9f, 0.9f ) );
                    
                    if( Spawn->SpawnCounter > 0 ) {
                        Spawn->Delay_Timer += dT;
                        if( Spawn->Delay_Timer >= Spawn->Delay_TargetTime ) {
                            Spawn->SpawnCounter -= 1;
                            
                            flo32 Outer = Spawn->OuterRadius;
                            flo32 Inner = Spawn->InnerRadius;
                            boo32 IsValid = false;
                            
                            int32 Select  = RandomS32InRange( 0, nPoint - 1 );
                            vec2  Pos     = Point[ Select ];
                            
                            while( !IsValid ) {
                                flo32 Radians = RandomF32() * TAU;
                                vec2  Dir     = ToDirection2D( Radians );
                                flo32 Dist    = RandomF32InRange( 0.0f, 1.0f );
                                
                                Pos += Dir * Dist;
                                if( IsInRect( Pos, AppState->World_Bound ) ) {
                                    IsValid = true;
                                }
                            }
                            
                            AddEntitySpawn( AppState, GenValIdentity( Spawn->WaveGuid, EntityType_Swarm, Pos ) );
                        }
                    }
                } break;
            }
        }
    }
    
    for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
        SWARM * Swarm = SwarmS->Swarm + iSwarm;
        
        flo32 dCloakT = 1.0f / ( 20.0f / 60.0f ) * dT;
        Swarm->CloakT = MinValue( Swarm->CloakT + dCloakT, 1.0f );
        
        { // ASTEROID
            ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
            for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                
                if( !( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) {
                    flo32 Dist = GetLength( Swarm->Position - Asteroid->Position ) - Asteroid->Radius;
                    Swarm->CloakT = MinValue( Swarm->CloakT, Clamp01( Dist / 2.0f ) );
                }
            }
        }
        
        { // PLAYER BULLET
            BULLET_STATE * BulletS = &AppState->BulletS;
            for( int32 iBullet = 0; iBullet < BulletS->nBullet; iBullet++ ) {
                BULLET * Bullet = BulletS->Bullet + iBullet;
                
                vec2 A = Bullet->PrevPos;
                vec2 B = Bullet->Position + Bullet->Direction * BULLET_HEIGHT;
                
                if( Bullet->Health > 0 ) {
                    flo32 Dist = DistPointToEdge( Swarm->Position, A, B );
                    Swarm->CloakT = MinValue( Swarm->CloakT, Clamp01( Dist / 2.0f ) );
                }
            }
        }
        
        for( int32 iRepel = iSwarm + 1; iRepel < SwarmS->nSwarm; iRepel++ ) {
            SWARM * Repel = SwarmS->Swarm + iRepel;
            
            vec2 V = Repel->Position - Swarm->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = SWARM_REPEL_RADIUS + SWARM_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * SWARM_REPEL_SPEED;
                
                Swarm->Repel -= Repel_Vector;
                Repel->Repel += Repel_Vector;
            }
        }
        
        vec2 Dir   = GetNormal( Player->Position - Swarm->Position );
        vec2 Accel = ( Dir * SWARM_SPEED - Swarm->Velocity + Swarm->Repel ) * SWARM_FRICTION;
        if( Player->GravityWell_IsActive ) {
            Accel += GetPerp( Dir ) * ( Player->Rotate_Speed * SWARM_FRICTION * 12.0f );
        }
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Swarm->Velocity * dT;
        // TODO: Collision!
        Swarm->Position += dPos;
        Swarm->Velocity += Accel * dT;
        
        Swarm->Repel = {};
    }
}

internal void
DrawSwarm( RENDER_PASS * Pass, SWARM Swarm, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 Radians = ( Swarm.Velocity.x / SWARM_SPEED ) * ( PI * 0.03125f );
    
    flo32 HalfRadius = Swarm.Radius * 0.5f;
    flo32 Offset     = ( sqrtf( HalfRadius * HalfRadius + HalfRadius * HalfRadius ) + ( ( 1.0f - Swarm.CloakT ) * Swarm.Radius * 0.6f ) ) * Scale;
    
    vec2  xAxis = ToDirection2D( Radians );
    vec2  yAxis = GetPerp( xAxis );
    
    vec2  Dim   = Vec2( Swarm.Radius, Swarm.Radius ) * Scale;
    
    vec2 PosA = Swarm.Position - xAxis * Offset;
    vec2 PosB = Swarm.Position - yAxis * Offset;
    vec2 PosC = Swarm.Position + xAxis * Offset;
    vec2 PosD = Swarm.Position + yAxis * Offset;
    
    orect BoundA = ORectCD( PosA, Dim, Radians + PI * 0.25f );
    orect BoundB = ORectCD( PosB, Dim, Radians + PI * 0.25f );
    orect BoundC = ORectCD( PosC, Dim, Radians + PI * 0.25f );
    orect BoundD = ORectCD( PosD, Dim, Radians + PI * 0.25f );
    
    flo32 Alph = lerp( 0.25f, Swarm.CloakT, 1.0f );
    vec4 Color = SWARM_BASE_COLOR * Alph;
    
    DrawORect( Pass, BoundA, Color * Alpha );
    DrawORect( Pass, BoundB, Color * Alpha );
    DrawORect( Pass, BoundC, Color * Alpha );
    DrawORect( Pass, BoundD, Color * Alpha );
}

internal void
DrawSwarm( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    SWARM_STATE * SwarmS = Draw->SwarmS;
    
    for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
        SWARM Swarm = SwarmS->Swarm[ iSwarm ];
        
        DrawSwarm( Pass, Swarm );
    }
}

internal void
FinalizeSwarm( APP_STATE * AppState ) {
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    SWARM_STATE * SwarmS = &AppState->SwarmS;
    
    uint32 NewCount = 0;
    for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
        SWARM Swarm = SwarmS->Swarm[ iSwarm ];
        
        if( Swarm.Health > 0 ) {
            SwarmS->Swarm[ NewCount++ ] = Swarm;
        } else {
            AddKillToScore( AppState, EntityType_Swarm, Swarm.Position );
            
            //uint32 nScoreMultiplier = 1;
            //if( RandomF32() >= 0.8f ) {
            //nScoreMultiplier += 1;
            //}
            
            //for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
            //AddScoreMultiplier( AppState, NewScoreMultiplier( Swarm.Position, AppState->World_Bound, true ) );
            //}
            
            //AddEntityDeathParticles( AppState, Swarm.Position, Vec4( 0.8f, 1.0f, 1.0f, 1.0f ) );
            AddKillToWave( AppState, Swarm.WaveGuid );
            
            PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Tiny );
        }
    }
    SwarmS->nSwarm = NewCount;
}