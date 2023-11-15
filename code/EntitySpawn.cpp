
internal int32
CountEntitySpawnsOfType( APP_STATE * AppState, ENTITY_TYPE Type ) {
    ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
    
    int32 Result = 0;
    
    for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
        ENTITY Entity = EntitySpawnS->EntitySpawn[ iEntitySpawn ];
        
        if( Entity.Type == Type ) {
            Result += ( int32 )CountTotalEntitiesInEntity( Type );
        }
    }
    
    return Result;
}

internal void
SpawnEntity( APP_STATE * AppState, ENTITY Entity ) {
    switch( Entity.Type ) {
        case EntityType_Turret: {
            uint32 Health = ( uint32 )Entity.Radius * 10;
            AddTurret( AppState, TurretC( Entity.WaveGuid, Entity.Position, Entity.Radius, Health, Entity.Shield ) );
        } break;
        
        case EntityType_Chaser: {
            AddChaser( AppState, ChaserC( Entity.WaveGuid, Entity.Position ) );
        } break;
        
        case EntityType_Swarm: {
            AddSwarm( AppState, SwarmC( Entity.WaveGuid, Entity.Position ) );
        } break;
        
        case EntityType_Drifter: {
            AddDrifter( AppState, DrifterC( Entity.WaveGuid, Entity.Position ) );
        } break;
        
        case EntityType_Looper: {
            AddLooper( AppState, LooperC( Entity.WaveGuid, Entity.Position ) );
        } break;
        
#if 0        
        case EntityType_Starburst: {
            AddStarburst( AppState, StarburstC( Entity.WaveGuid, Entity.Position ) );
        } break;
#endif
        
        case EntityType_Miner: {
            uint32 nLaser = Entity.nEntity;
            AddMiner( AppState, MinerC_Autonomous( Entity.WaveGuid, Entity.Position, nLaser ) );
        } break;
        
        case EntityType_Miner_Stationary: {
            int32 nLaser = Entity.nEntity;
            AddMiner( AppState, MinerC_Stationary( Entity.WaveGuid, Entity.Position, -MINER_STATIONARY_ROTATE_SPEED, nLaser ) );
        } break;
        
#if 0        
        case EntityType_Bomber: {
            uint32 nBomb = Entity.nEntity;
            AddBomber( AppState, BomberC_Autonomous( Entity.WaveGuid, Entity.Position, nBomb ) );
        } break;
#endif
        
#if 0        
        case EntityType_Bumper: {
            AddBumper( AppState, BumperC( Entity.WaveGuid, Entity.Position ) );
        } break;
#endif
        
#if 0        
        case EntityType_Bank: {
            AddBank( AppState, BankC( Entity.WaveGuid, Entity.Position ) );
        } break;
#endif
        
#if 0        
        case EntityType_Bumper_Stationary: {
            AddBumper( AppState, BumperC_Stationary( Entity.WaveGuid, Entity.Position ) );
        } break;
#endif
        
        case EntityType_MegaTurret: {
            int32 Health = Entity.nEntity;
            AddMegaTurret( AppState, TurretC( Entity.WaveGuid, Entity.Position, Entity.Radius, Health, Entity.Shield ) );
        } break;
        
        case EntityType_Worm: {
            vec2 Dir = Vec2( Entity.Shield.bRadians, Entity.Shield.Radians );
            if( Entity.Radius == WORM_RADIUS_SMALL ) {
                AddWorm( AppState, WormC_Small( Entity.WaveGuid, Entity.Position, Dir ) );
            } else {
                AddWorm( AppState, WormC_Large( Entity.WaveGuid, Entity.Position, Dir ) );
            }
        } break;
        
        case EntityType_Gunner: {
            AddGunner( AppState, GunnerC( Entity.Position ) );
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
}

internal void
UpdateEntitySpawn( APP_STATE * AppState, flo32 dT ) {
    ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
    
    for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
        EntitySpawnS->EntitySpawn_Timer[ iEntitySpawn ] += dT;
    }
    //DISPLAY_VALUE( uint32, EntitySpawnS->nEntitySpawn );
}

internal void
DrawEntitySpawn( RENDER_PASS * Pass, ENTITY Entity, flo32 Timer, flo32 Alpha ) {
    flo32 TargetTime = ENTITY_SPAWN_TARGET_TIME - ( 10.0f / 60.0f );
    flo32 t          = Clamp01( Timer / TargetTime );
    flo32 tRad       = lerp( 0.8f, t, 1.2f );
    flo32 Radius     = Entity.Radius * tRad * 1.35f;
    
    flo32 Alph  = t * 0.4f;
    vec4  Color = COLOR_WHITE;
    
    switch( Entity.Type ) {
        case EntityType_Turret:{
            if( Entity.Shield.HasShield ) {
                flo32 Radius_Outer = Entity.Radius + Entity.Shield.Dist + Entity.Shield.Width * 1.5f;
                
                vec4 NoShield_Color = TowardBlack( TURRET_BASE_COLOR, 0.6f  ) * Alph;
                vec4 Shield_Color   = TowardBlack( TURRET_BASE_COLOR, 0.25f ) * Alph;
                if( fabs( Entity.Shield.RotateSpeed ) == PLAYER_ROTATE_SPEED_FAST ) {
                    NoShield_Color = TowardBlack( MEGA_TURRET_BASE_COLOR, 0.35f ) * Alph;
                    Shield_Color   = TowardBlack( MEGA_TURRET_BASE_COLOR, 0.0f  ) * Alph;
                }
                if( Entity.Shield.Radians >= TAU ) {
                    DrawCircle( Pass, Entity.Position, lerp( Radius, t, Radius_Outer ), Shield_Color * Alpha );
                } else {
                    DrawCircleSegment( Pass, Entity.Position, lerp( Radius, t, Radius_Outer ), Entity.Shield.bRadians + Entity.Shield.Radians, TAU - Entity.Shield.Radians, false, NoShield_Color * Alpha );
                    
                    DrawCircleSegment( Pass, Entity.Position, lerp( Radius, t, Radius_Outer ), Entity.Shield.bRadians, Entity.Shield.Radians, false, Shield_Color * Alpha );
                }
            }
            
            Color = TowardWhite( TURRET_BASE_COLOR, 0.2f );
        } break;
        
        case EntityType_Chaser: {
            Color = TowardWhite( CHASER_BASE_COLOR, 0.3f );
        } break;
        
        case EntityType_Swarm: {
            Color = TowardWhite( SWARM_BASE_COLOR, 0.2f );
        } break;
        
        case EntityType_Drifter: {
            Color = TowardWhite( DRIFTER_BASE_COLOR, 0.1f );
        } break;
        
        case EntityType_Looper: {
            Color = TowardWhite( LOOPER_BASE_COLOR, 0.3f );
        } break;
        
        case EntityType_Miner: {
            Color = TowardWhite( MINER_AUTONOMOUS_BASE_COLOR, 0.2f );
        } break;
        
        case EntityType_Miner_Stationary: {
            Color = TowardWhite( MINER_STATIONARY_BASE_COLOR, 0.2f );
        } break;
        
        case EntityType_Bumper: {
            Color = BUMPER_BASE_COLOR;
        } break;
        
        case EntityType_Bank: {
            Color = BANK_BASE_COLOR;
        } break;
        
        case EntityType_Bumper_Stationary: {
            Color = BUMPER_STATIONARY_BASE_COLOR;
        } break;
        
        case EntityType_MegaTurret: {
            if( Entity.Shield.HasShield ) {
                if( Entity.Shield.HasShield ) {
                    flo32 Radius_Outer = Entity.Radius + Entity.Shield.Dist + Entity.Shield.Width * 1.5f;
                    
                    vec4 NoShield_Color = TowardBlack( TURRET_BASE_COLOR, 0.6f  ) * Alph;
                    vec4 Shield_Color   = TowardBlack( TURRET_BASE_COLOR, 0.25f ) * Alph;
                    if( fabs( Entity.Shield.RotateSpeed ) == PLAYER_ROTATE_SPEED_FAST ) {
                        NoShield_Color = TowardBlack( MEGA_TURRET_BASE_COLOR, 0.35f ) * Alph;
                        Shield_Color   = TowardBlack( MEGA_TURRET_BASE_COLOR, 0.0f  ) * Alph;
                    }
                    DrawCircleSegment( Pass, Entity.Position, lerp( Radius, t, Radius_Outer ), Entity.Shield.bRadians + Entity.Shield.Radians, TAU - Entity.Shield.Radians, false, NoShield_Color * Alpha );
                    
                    DrawCircleSegment( Pass, Entity.Position, lerp( Radius, t, Radius_Outer ), Entity.Shield.bRadians, Entity.Shield.Radians, false, Shield_Color * Alpha );
                }
            }
            
            Color = MEGA_TURRET_BASE_COLOR;
        } break;
        
        case EntityType_Worm: {
            Color = TowardWhite( WORM_BASE_COLOR, 0.2f );
        } break;
        
        case EntityType_Gunner: {
            Color = TowardWhite( GUNNER_BASE_COLOR, 0.2f );
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    Color *= Alph;
    
    rect Bound = RectCR( Entity.Position, Radius );
    DrawRect( Pass, TextureID_EntitySpawn_Indicator, Bound, Color * Alpha );
}

internal void
DrawEntitySpawn( RENDER_PASS * Pass, DRAW_STATE * Draw, flo32 Alpha = 1.0f ) {
    ENTITY_SPAWN_STATE * EntitySpawnS = Draw->EntitySpawnS;
    for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
        ENTITY Entity = EntitySpawnS->EntitySpawn      [ iEntitySpawn ];
        flo32  Timer  = EntitySpawnS->EntitySpawn_Timer[ iEntitySpawn ];
        DrawEntitySpawn( Pass, Entity, Timer, Alpha );
    }
}

internal void
FinalizeEntitySpawn( APP_STATE * AppState ) {
    ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
    
    uint32 NewCount = 0;
    for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
        if( EntitySpawnS->EntitySpawn_Timer[ iEntitySpawn ] >= ENTITY_SPAWN_TARGET_TIME ) {
            ENTITY Entity = EntitySpawnS->EntitySpawn[ iEntitySpawn ];
            SpawnEntity( AppState, Entity );
        } else {
            EntitySpawnS->EntitySpawn      [ NewCount ] = EntitySpawnS->EntitySpawn      [ iEntitySpawn ];
            EntitySpawnS->EntitySpawn_Timer[ NewCount ] = EntitySpawnS->EntitySpawn_Timer[ iEntitySpawn ];
            NewCount++;
        }
    }
    EntitySpawnS->nEntitySpawn = NewCount;
}