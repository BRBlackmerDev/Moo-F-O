
internal BUMPER
BumperC( int32 WaveGuid, vec2 Position ) {
    BUMPER Result = {};
    Result.WaveGuid  = WaveGuid;
    Result.Position  = Position;
    Result.Direction = ToDirection2D( RandomF32() * TAU );
    Result.Velocity  = Result.Direction * BUMPER_BOUNCE_SPEED;
    Result.Radius    = BUMPER_RADIUS;
    Result.Health    = 1;
    
    return Result;
}

internal BUMPER
BumperC_Stationary( int32 WaveGuid, vec2 Position ) {
    BUMPER Result = {};
    Result.WaveGuid  = WaveGuid;
    Result.Position  = Position;
    Result.Radius    = BUMPER_STATIONARY_RADIUS;
    Result.Health    = 1;
    Result.IsStationary = true;
    
    return Result;
}

internal void
AddBumper( APP_STATE * AppState, BUMPER Bumper ) {
    BUMPER_STATE * BumperS = &AppState->BumperS;
    if( BumperS->nBumper < BUMPER_MAX_COUNT ) {
        BumperS->Bumper[ BumperS->nBumper++ ] = Bumper;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BUMPER to BUMPER_STATE: %u", BUMPER_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal ENTITY_VALID_RESULT
DoesBumperIntersectBumper( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, int32 iSkip, flo32 RadiusA ) {
    BUMPER_STATE * BumperS = &AppState->BumperS;
    
    ENTITY_VALID_RESULT Result = {};
    
    for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
        if( iBumper != iSkip ) {
            BUMPER * Bumper = BumperS->Bumper + iBumper;
            
            flo32 RadiusB = Bumper->Radius;
            flo32 Radius  = RadiusA + RadiusB;
            DoesRayIntersectCircleExt( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), Circ( Bumper->Position, Radius ) );
            
            if( ( BestIntersect->IsValid ) && ( !BestIntersect->WasProcessed ) ) {
                BestIntersect->WasProcessed = true;
                Result.IsValid = true;
                Result.iEntity = iBumper;
            }
        }
    }
    
    return Result;
}

internal void
UpdateBumper( APP_STATE * AppState, flo32 dT ) {
    BUMPER_STATE * BumperS = &AppState->BumperS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    if( BumperS->Spawn_IsActive ) {
        BumperS->Spawn_Timer += dT;
        if( BumperS->Spawn_Timer >= BUMPER_STATIONARY_SPAWN_TARGET_TIME ) {
            BumperS->Spawn_Timer = 0.0f;
            
            flo32 Dist     = BUMPER_STATIONARY_RADIUS * 2.0f + 4.2f;
            flo32 HalfDist = Dist     * 0.5f;
            flo32 QuarDist = HalfDist * 0.5f;
            flo32 Offset   = sqrtf( ( HalfDist * HalfDist ) + ( QuarDist * QuarDist ) );
            
            flo32 Radians  = TAU / 3.0f;
            flo32 bRadians = RandomF32() * TAU;
            
            rect  Bound = RectCD( Vec2( 0.0f, 0.0f ), Vec2( 60.0f, 34.0f ) );
            vec2  bPos  = GetRandomPoint( Bound );
            for( int32 iBumper = 0; iBumper < 3; iBumper++ ) {
                vec2 Pos = bPos + ToDirection2D( bRadians + Radians * ( flo32 )iBumper ) * Offset;
                
                ENTITY Entity = GenValIdentity( -1, EntityType_Bumper_Stationary, Pos );
                AddEntitySpawn( AppState, Entity );
            }
        }
    }
    
    for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
        BUMPER * Bumper = BumperS->Bumper + iBumper;
        
        Bumper->LastBump_Timer = MaxValue( Bumper->LastBump_Timer - dT, 0.0f );
        
        if( Bumper->IsStationary ) {
            Bumper->Exit_Timer += dT;
            if( Bumper->Exit_Timer >= BUMPER_EXIT_TARGET_TIME ) {
                Bumper->Health     = 0;
                Bumper->DoExitAnim = true;
            }
        }
        
        if( Bumper->Health > 0 ) { // COLLISION
            { // PLAYER
                flo32 DistSq   = GetLengthSq( Player->Position - Bumper->Position );
                flo32 Radius   = ( PLAYER_RADIUS + Bumper->Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                if( DistSq <= RadiusSq ) {
                    Player->DoBumper_Timer    = PLAYER_BUMPER_BOOST_TARGET_TIME;
                    Player->DoBumper_Velocity = GetNormal( Player->Position - Bumper->Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    
                    if( !Bumper->IsStationary ) {
                        Bumper->Velocity  += GetNormal( Bumper->Position - Player->Position ) * BUMPER_BOUNCE_SPEED;
                        Bumper->Direction  = GetNormal( Bumper->Velocity );
                        
                        AddKillToScore( AppState, EntityType_Bumper_Bounce, Bumper->Position, BumperS->BasePoints++ );
                    }
                    
                    Bumper->LastBump_Timer = BUMPER_BOUNCE_TARGET_TIME;
                }
            }
            
            { // BUMPER
                for( int32 iCollide = iBumper + 1; iCollide < BumperS->nBumper; iCollide++ ) {
                    BUMPER * Collide = BumperS->Bumper + iCollide;
                    
                    flo32 Radius   = Bumper->Radius + Collide->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Collide->Position - Bumper->Position );
                    if( DistSq <= RadiusSq ) {
                        vec2 Dir = GetNormal( Bumper->Position - Collide->Position );
                        if( ( Bumper->IsStationary ) || ( Collide->IsStationary ) ) {
                            Assert( !( ( Bumper->IsStationary ) && ( Collide->IsStationary ) ) );
                            
                            boo32 DoFlip = false;
                            
                            BUMPER * BumperA = Bumper;
                            BUMPER * BumperB = Collide;
                            if( BumperA->IsStationary ) {
                                DoFlip = true;
                                _swap( BUMPER *, BumperA, BumperB );
                            }
                            
                            RAY2 Ray = Ray2( BumperA->PrevPos,  BumperA->PrevdPos );
                            circ Cir = Circ( BumperB->Position, BumperA->Radius + BumperB->Radius );
                            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                            DoesRayIntersectCircleExt( &BestIntersect, Ray, Cir );
                            
                            if( BestIntersect.IsValid ) {
                                BumperA->Position = BestIntersect.P;
                                Dir               = BestIntersect.N;
                                
                                if( DoFlip ) {
                                    Dir = -Dir;
                                }
                            } else {
                                BumperA->Position = BumperB->Position + Dir * Radius;
                            }
                        }
                        
                        if( !Bumper->IsStationary ) {
                            Bumper->Velocity       -= dot( Bumper->Velocity, Dir ) * Dir;
                            Bumper->Velocity       += Dir * BUMPER_BOUNCE_SPEED;
                            Bumper->Direction       = GetNormal( Bumper->Velocity );
                            AddKillToScore( AppState, EntityType_Bumper_Bounce, Bumper->Position, BumperS->BasePoints++ );
                        }
                        Bumper->LastBump_Timer  = BUMPER_BOUNCE_TARGET_TIME;
                        
                        if( !Collide->IsStationary ) {
                            Collide->Velocity      -= dot( Collide->Velocity, Dir ) * Dir;
                            Collide->Velocity      -= Dir * BUMPER_BOUNCE_SPEED;
                            Collide->Direction      = GetNormal( Collide->Velocity );
                            AddKillToScore( AppState, EntityType_Bumper_Bounce, Collide->Position, BumperS->BasePoints++ );
                        }
                        Collide->LastBump_Timer = BUMPER_BOUNCE_TARGET_TIME;
                        
                    }
                }
            }
            
            { // ASTEROID
                ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
                for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                    ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                    
                    flo32 Radius   = Bumper->Radius + Asteroid->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Asteroid->Position - Bumper->Position );
                    if( DistSq <= RadiusSq ) {
                        vec2 Dir = GetNormal( Asteroid->Position - Bumper->Position );
                        
                        if( !Bumper->IsStationary ) {
                            Bumper->Velocity      -= dot( Bumper->Velocity, Dir ) * Dir;
                            Bumper->Velocity      -= Dir * BUMPER_BOUNCE_SPEED;
                            Bumper->Direction      = GetNormal( Bumper->Velocity );
                            AddKillToScore( AppState, EntityType_Bumper_Bounce, Bumper->Position, BumperS->BasePoints++ );
                        }
                        Bumper->LastBump_Timer = BUMPER_BOUNCE_TARGET_TIME;
                        
                        vec2 Velocity = Asteroid->Direction * Asteroid->Speed;
                        Velocity -= dot( Velocity, Dir ) * Dir;
                        Velocity += Dir * ( BUMPER_BOUNCE_SPEED / Asteroid->Radius );
                        
                        Asteroid->Direction = GetNormal( Velocity );
                        Asteroid->Speed     = GetLength( Velocity );
                        
                        Asteroid->Health    = MaxValue( Asteroid->Health - 2, 0 );
                        
                        if( Asteroid->ZeroPoint_IsActive ) {
                            Asteroid->ZeroPoint_IsActive = false;
                            Player->ZeroPoint_IsActive   = false;
                            Player->ZeroPoint_IsAsteroid = false;
                        }
                    }
                }
            }
            
            { // MINER
                MINER_STATE * MinerS = &AppState->MinerS;
                for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                    MINER * Miner = MinerS->Miner + iMiner;
                    
                    flo32 Radius   = Bumper->Radius + Miner->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Miner->Position - Bumper->Position );
                    if( DistSq <= RadiusSq ) {
                        vec2 Dir = GetNormal( Miner->Position - Bumper->Position );
                        
                        if( !Bumper->IsStationary ) {
                            Bumper->Velocity      -= dot( Bumper->Velocity, Dir ) * Dir;
                            Bumper->Velocity      -= Dir * BUMPER_BOUNCE_SPEED;
                            Bumper->Direction      = GetNormal( Bumper->Velocity );
                            AddKillToScore( AppState, EntityType_Bumper_Bounce, Bumper->Position, BumperS->BasePoints++ );
                        }
                        Bumper->LastBump_Timer = BUMPER_BOUNCE_TARGET_TIME;
                        
                        Miner->Velocity -= dot( Miner->Velocity, Dir ) * Dir;
                        Miner->Velocity += Dir * BUMPER_BOUNCE_SPEED;
                    }
                }
            }
            
            { // BOMBER
                BOMB_STATE * BombS = &AppState->BombS;
                for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
                    BOMBER * Bomber = BombS->Bomber + iBomber;
                    
                    flo32 Radius   = Bumper->Radius + Bomber->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bomber->Position - Bumper->Position );
                    if( DistSq <= RadiusSq ) {
                        vec2 Dir = GetNormal( Bomber->Position - Bumper->Position );
                        
                        if( !Bumper->IsStationary ) {
                            Bumper->Velocity      -= dot( Bumper->Velocity, Dir ) * Dir;
                            Bumper->Velocity      -= Dir * BUMPER_BOUNCE_SPEED;
                            Bumper->Direction      = GetNormal( Bumper->Velocity );
                            AddKillToScore( AppState, EntityType_Bumper_Bounce, Bumper->Position, BumperS->BasePoints++ );
                        }
                        Bumper->LastBump_Timer = BUMPER_BOUNCE_TARGET_TIME;
                        
                        Bomber->Velocity -= dot( Bomber->Velocity, Dir ) * Dir;
                        Bomber->Velocity += Dir * BUMPER_BOUNCE_SPEED;
                    }
                }
            }
            
            { // EXPLOSIVE
                EXPLOSIVE_STATE * ExplosiveS = &AppState->ExplosiveS;
                for( uint32 iExplosive = 0; iExplosive < ExplosiveS->nExplosive; iExplosive++ ) {
                    EXPLOSIVE * Explosive = ExplosiveS->Explosive + iExplosive;
                    
                    if( Explosive->Health > 0 ) {
                        flo32 Radius   = Bumper->Radius + EXPLOSIVE_RADIUS;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Explosive->Position - Bumper->Position );
                        if( DistSq <= RadiusSq ) {
                            vec2 Dir = GetNormal( Explosive->Position - Bumper->Position );
                            
                            if( !Bumper->IsStationary ) {
                                Bumper->Velocity      -= dot( Bumper->Velocity, Dir ) * Dir;
                                Bumper->Velocity      -= Dir * BUMPER_BOUNCE_SPEED;
                                Bumper->Direction      = GetNormal( Bumper->Velocity );
                                AddKillToScore( AppState, EntityType_Bumper_Bounce, Bumper->Position, BumperS->BasePoints++ );
                            }
                            Bumper->LastBump_Timer = BUMPER_BOUNCE_TARGET_TIME;
                            
                            if( !Explosive->ZeroPoint_IsActive ) {
                                vec2 Velocity = Explosive->Direction * Explosive->Speed;
                                Velocity     -= dot( Velocity, Dir ) * Dir;
                                Velocity     += Dir * BUMPER_BOUNCE_SPEED;
                                
                                Explosive->Direction = GetNormal( Velocity );
                                Explosive->Speed     = GetLength( Velocity );
                                
                                Explosive->DoArmed   = true;
                            }
                            
                        }
                    }
                }
            }
            
            { // WORLD_BOUND
                rect Bound = AddRadius( AppState->World_Bound, -Bumper->Radius );
                if( !IsInRect( Bumper->Position, Bound ) ) {
                    vec2 Dir = Vec2( 0.0f, 0.0f );
                    if( Bumper->Position.x < Bound.Left ) {
                        Dir.x =  1.0f;
                    }
                    if( Bumper->Position.x > Bound.Right ) {
                        Dir.x = -1.0f;
                    }
                    if( Bumper->Position.y < Bound.Bottom ) {
                        Dir.y =  1.0f;
                    }
                    if( Bumper->Position.y > Bound.Top ) {
                        Dir.y = -1.0f;
                    }
                    
                    Bumper->Position       = ClampToRect( Bumper->Position, Bound );
                    Bumper->Velocity      -= dot( Bumper->Velocity, Dir ) * Dir;
                    Bumper->Velocity      += Dir * BUMPER_BOUNCE_SPEED;
                    Bumper->Direction      = GetNormal( Bumper->Velocity );
                    Bumper->LastBump_Timer = BUMPER_BOUNCE_TARGET_TIME;
                    
                    //AddKillToScore( AppState, EntityType_Bumper_Bounce, Bumper->Position, BumperS->BasePoints++ );
                    
                    Assert( !Bumper->IsStationary );
                }
            }
        }
        
        vec2 Accel = ( Bumper->Direction * BUMPER_SPEED - Bumper->Velocity ) * BUMPER_FRICTION;
        vec2 dPos  = Accel * ( 0.5f * dT * dT ) + Bumper->Velocity * dT;
        Bumper->PrevdPos  = dPos;
        Bumper->PrevPos   = Bumper->Position;
        
        Bumper->Position += dPos;
        Bumper->Velocity += Accel * dT;
    }
}

internal void
DrawBumper( RENDER_PASS * Pass, BUMPER Bumper, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 Sca  = 1.0f;
    flo32 Alph = 1.0f;
    if( ( Bumper.IsStationary ) && ( Bumper.Exit_Timer >= BUMPER_EXIT_TARGET_TIME ) ) {
        flo32 t = Clamp01( ( Bumper.Exit_Timer - BUMPER_EXIT_TARGET_TIME ) / BUMPER_EXIT_ANIM_TARGET_TIME );
        Sca  = lerp( 1.0f, t, 0.2f );
        Alph = lerp( 1.0f, t, 0.0f );
    }
    
    flo32 t = Clamp01( Bumper.LastBump_Timer / BUMPER_BOUNCE_TARGET_TIME );
    vec4 ColorA = BUMPER_BASE_COLOR;
    vec4 ColorB = ToColor( 200, 255, 200 );
    vec4 ColorC = ToColor(   0,  25,   0 )  * Alpha;
    
    if( Bumper.IsStationary ) {
        ColorA = BUMPER_STATIONARY_BASE_COLOR * Alph;
        ColorB = ToColor( 255, 255, 200 ) * Alph;
        ColorC = ToColor(  25,  25,   0 ) * Alph * Alpha;
    }
    vec4 Color  = lerp( ColorA, t, ColorB ) * Alpha;
    
    flo32 RadiusA = Bumper.Radius        * Scale * Sca;
    flo32 RadiusB = Bumper.Radius * 0.7f * Scale * Sca;
    flo32 RadiusC = Bumper.Radius * 0.3f * Scale * Sca;
    
    DrawCircle( Pass, Bumper.Position, RadiusA, Color );
    DrawCircle( Pass, Bumper.Position, RadiusB, ColorC );
    
    if( Bumper.IsStationary ) {
        flo32 Radians = ( 1.0f - Clamp01( Bumper.Exit_Timer / BUMPER_EXIT_TARGET_TIME ) ) * TAU;
        DrawCircleSegment( Pass, Bumper.Position, RadiusB, PI * 0.25f, Radians, true, ToColor( 150, 150, 0 ) );
    }
    DrawCircle( Pass, Bumper.Position, RadiusC, Color );
}

internal void
DrawBumper( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BUMPER_STATE * BumperS = Draw->BumperS;
    
    for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
        BUMPER Bumper = BumperS->Bumper[ iBumper ];
        
        DrawBumper( Pass, Bumper );
    }
}

internal void
FinalizeBumper( APP_STATE * AppState ) {
    BUMPER_STATE * BumperS = &AppState->BumperS;
    
    uint32 NewCount = 0;
    for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
        BUMPER Bumper = BumperS->Bumper[ iBumper ];
        
        if( Bumper.IsStationary ) {
            if( Bumper.Health > 0 ) {
                BumperS->Bumper[ NewCount++ ] = Bumper;
            } else {
                if( Bumper.DoExitAnim ) {
                    AddEntityToMissedCountForLevel( AppState, EntityType_Bumper_Stationary, 1 );
                } else {
                    AddKillToScore( AppState, EntityType_Bumper_Stationary, Bumper.Position );
                    
                    POINTS_ENTRY * PointsEntry = GetPointsEntryForEntity( AppState, EntityType_Bumper_Stationary );
                    int32 nScoreMultiplier = PointsEntry->nScoreMultiplier;
                    
                    vec2 Pos = ClampToRect( Bumper.Position, AppState->World_Bound );
                    for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                        AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                    }
                    
                    AddEntityDeathParticles( AppState, Bumper.Position, ToColor( 100, 100, 0 ) );
                    AddEntityDeathParticles( AppState, Bumper.Position, ToColor( 100, 100, 0 ) );
                    AddEntityDeathParticles( AppState, Bumper.Position, COLOR_WHITE );
                }
                
                AddKillToWave( AppState, Bumper.WaveGuid );
            }
        } else {
            if( Bumper.Health > 0 ) {
                BumperS->Bumper[ NewCount++ ] = Bumper;
            } else {
                AddEntityDeathParticles( AppState, Bumper.Position, ToColor( 0, 100, 0 ) );
                AddEntityDeathParticles( AppState, Bumper.Position, ToColor( 0, 100, 0 ) );
                AddEntityDeathParticles( AppState, Bumper.Position, COLOR_WHITE );
                
                POINTS_ENTRY * PointsEntry = GetPointsEntryForEntity( AppState, EntityType_Bumper );
                int32 nScoreMultiplier = PointsEntry->nScoreMultiplier;
                AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( Bumper.Position, nScoreMultiplier ) );
                
                AddKillToScore( AppState, EntityType_Bumper, Bumper.Position );
                AddKillToWave( AppState, Bumper.WaveGuid );
                
                BumperS->BonusObjective_nBumperKills += 1;
            }
        }
    }
    BumperS->nBumper = NewCount;
}