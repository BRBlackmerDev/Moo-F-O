

internal void
UpdateZeroPoint( APP_STATE * AppState, boo32 DoPressed, boo32 DoReleased, flo32 dT ) {
    PLAYER_STATE           * Player      = &AppState->Player;
    AUTO_TURRET_STATE      * AutoTurretS = &AppState->AutoTurretS;
    ASTEROID_STATE         * AsteroidS   = &AppState->AsteroidS;
    
    if( Player->ZeroPoint_IsEnabled ) {
        Player->ZeroPoint_ShowUI = false;
        
        if( Player->ZeroPoint_IsActive ) {
            flo32 Rotate_Speed = Player->Rotate_Speed;
            
            if( ( Player->ZeroPoint_IsAsteroid ) && ( Player->ZeroPoint_Fast_CooldownTimer > 0.0f ) ) {
                Rotate_Speed = PLAYER_ROTATE_SPEED_FAST;
            }
            Player->ZeroPoint_Radians = WrapRadians( Player->ZeroPoint_Radians - Rotate_Speed * dT );
            
            Player->ZeroPoint_ShowUI = true;
            if( DoReleased ) {
                Player->ZeroPoint_ShowUI          = false;
                Player->ZeroPoint_IsActive        = false;
                Player->ZeroPoint_IsAsteroid      = false;
                Player->ZeroPoint_IsSecurityDrone = false;
            }
        } else {
            flo32       IsClosest_Dist    = FLT_MAX;
            ENTITY_TYPE IsClosest_Type    = {};
            int32       IsClosest_iEntity = -1;
            
            { // ASTEROID
                for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                    ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                    
                    if( Asteroid->Class > AsteroidClass_Tiny ) {
                        Asteroid->ZeroPoint_ShowUI = false;
                        
                        flo32 Radius   = ASTEROID_ZERO_POINT_RADIUS;
                        if( Asteroid->Class >= AsteroidClass_Large ) {
                            Radius += 1.0f;
                        }
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Player->Position - Asteroid->Position );
                        if( DistSq <= RadiusSq ) {
                            if( Asteroid->Flags & ASTEROID_FLAG__CAN_USE_ZERO_POINT_ENERGY ) {
                                flo32 Dist = GetLength( Player->Position - Asteroid->Position ) - PLAYER_RADIUS - Asteroid->Radius;
                                
                                if( Dist <= IsClosest_Dist ) {
                                    IsClosest_Dist    = Dist;
                                    IsClosest_Type    = EntityType_Asteroid;
                                    IsClosest_iEntity = iAsteroid;
                                }
                            }
                        }
                    }
                }
            }
            
            { // AUTO TURRET
                for( uint32 iAutoTurret = 0; iAutoTurret < AutoTurretS->nAutoTurret; iAutoTurret++ ) {
                    AUTO_TURRET * AutoTurret = AutoTurretS->AutoTurret + iAutoTurret;
                    AutoTurret->ZeroPoint_ShowUI = false;
                    
                    flo32 Radius   = AUTO_TURRET_ZERO_POINT_RADIUS;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - AutoTurret->Position );
                    if( DistSq <= RadiusSq ) {
                        flo32 Dist = GetLength( Player->Position - AutoTurret->Position ) - PLAYER_RADIUS - AUTO_TURRET_RADIUS;
                        
                        if( Dist <= IsClosest_Dist ) {
                            IsClosest_Dist    = Dist;
                            IsClosest_Type    = EntityType_AutoTurret;
                            IsClosest_iEntity = iAutoTurret;
                        }
                    }
                }
            }
            
#if 0            
            { // EXPLOSIVE
                EXPLOSIVE_STATE        * ExplosiveS  = &AppState->ExplosiveS;
                for( uint32 iExplosive = 0; iExplosive < ExplosiveS->nExplosive; iExplosive++ ) {
                    EXPLOSIVE * Explosive = ExplosiveS->Explosive + iExplosive;
                    Explosive->ZeroPoint_ShowUI = false;
                    
                    flo32 Radius   = EXPLOSIVE_ZERO_POINT_RADIUS;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Explosive->Position );
                    if( DistSq <= RadiusSq ) {
                        flo32 Dist = GetLength( Player->Position - Explosive->Position ) - PLAYER_RADIUS - EXPLOSIVE_RADIUS;
                        
                        if( Dist <= IsClosest_Dist ) {
                            IsClosest_Dist    = Dist;
                            IsClosest_Type    = EntityType_Explosive;
                            IsClosest_iEntity = iExplosive;
                        }
                    }
                }
            }
#endif
            
            { // SECURITY_DRONE
                BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
                OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                
                if( ( BoardRoomS->State_IsActive ) && ( ObservationDeck->Health >= 0 ) ) {
                    SECURITY_DRONE * SecurityDrone = &BoardRoomS->SecurityDrone;
                    
                    SecurityDrone->ZeroPoint_ShowUI = false;
                    if( SecurityDrone->Stage == SecurityDroneStage_Stunned ) {
                        flo32 Radius   = SECURITY_DRONE_ZERO_POINT_RADIUS;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Player->Position - SecurityDrone->Position );
                        if( DistSq <= RadiusSq ) {
                            flo32 Dist = GetLength( Player->Position - SecurityDrone->Position ) - PLAYER_RADIUS - SecurityDrone->Radius;
                            
                            if( Dist <= IsClosest_Dist ) {
                                IsClosest_Dist    = Dist;
                                IsClosest_Type    = EntityType_SecurityDrone;
                                IsClosest_iEntity = 0;
                            }
                        }
                    }
                }
            }
            
            if( IsClosest_iEntity > -1 ) {
                Player->ZeroPoint_ShowUI = true;
                
                switch( IsClosest_Type ) {
                    case EntityType_Asteroid: {
                        ASTEROID * Asteroid = AsteroidS->Asteroid + IsClosest_iEntity;
                        
                        Asteroid->ZeroPoint_ShowUI = true;
                        Player->ZeroPoint_Pos      = Asteroid->Position;
                        
                        if( DoPressed ) {
                            vec2 Dir = Asteroid->Position - Player->Position;
                            
                            Player->ZeroPoint_IsActive   = true;
                            Player->ZeroPoint_DrawBeam   = true;
                            Player->ZeroPoint_Radians    = ToRadians( Dir );
                            Player->ZeroPoint_Dist       = GetLength( Dir ) - PLAYER_RADIUS - Asteroid->Radius;
                            Player->ZeroPoint_IsAsteroid = true;
                            
                            Asteroid->ZeroPoint_IsActive = true;
                            Asteroid->ZeroPoint_Radians  = ToRadians( Dir );
                            Asteroid->ZeroPoint_Dist     = GetLength( Dir );
                        }
                    } break;
                    
                    case EntityType_AutoTurret: {
                        AUTO_TURRET * AutoTurret = AutoTurretS->AutoTurret + IsClosest_iEntity;
                        
                        AutoTurret->ZeroPoint_ShowUI = true;
                        Player->ZeroPoint_Pos        = AutoTurret->Position;
                        
                        if( DoPressed ) {
                            vec2 Dir = AutoTurret->Position - Player->Position;
                            
                            Player->ZeroPoint_IsActive   = true;
                            Player->ZeroPoint_Radians    = ToRadians( Dir );
                            Player->ZeroPoint_Dist       = GetLength( Dir ) - PLAYER_RADIUS - AUTO_TURRET_RADIUS;
                            Player->ZeroPoint_DrawBeam   = ( Player->ZeroPoint_Dist > 0.0f );
                            
                            AutoTurret->ZeroPoint_IsActive = true;
                            AutoTurret->ZeroPoint_Radians  = ToRadians( Dir );
                            AutoTurret->ZeroPoint_Dist     = GetLength( Dir );
                        }
                    } break;
                    
#if 0                    
                    case EntityType_Explosive: {
                        EXPLOSIVE * Explosive = ExplosiveS->Explosive + IsClosest_iEntity;
                        
                        Explosive->ZeroPoint_ShowUI = true;
                        Player->ZeroPoint_Pos       = Explosive->Position;
                        
                        if( DoPressed ) {
                            vec2 Dir = Explosive->Position - Player->Position;
                            
                            Player->ZeroPoint_IsActive = true;
                            Player->ZeroPoint_Radians  = ToRadians( Dir );
                            Player->ZeroPoint_Dist     = GetLength( Dir ) - PLAYER_RADIUS - EXPLOSIVE_RADIUS;
                            Player->ZeroPoint_DrawBeam = ( Player->ZeroPoint_Dist > 0.0f );
                            
                            Explosive->ZeroPoint_IsActive = true;
                            Explosive->ZeroPoint_Radians  = ToRadians( Dir );
                            Explosive->ZeroPoint_Dist     = GetLength( Dir );
                        }
                    } break;
#endif
                    
                    case EntityType_SecurityDrone: {
                        BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
                        Assert( BoardRoomS->State_IsActive );
                        
                        SECURITY_DRONE * SecurityDrone = &BoardRoomS->SecurityDrone;
                        
                        SecurityDrone->ZeroPoint_ShowUI = true;
                        Player->ZeroPoint_Pos           = SecurityDrone->Position;
                        
                        if( DoPressed ) {
                            vec2 Dir = SecurityDrone->Position - Player->Position;
                            
                            Player->ZeroPoint_IsActive = true;
                            Player->ZeroPoint_Radians  = ToRadians( Dir );
                            Player->ZeroPoint_Dist     = GetLength( Dir ) - PLAYER_RADIUS - SECURITY_DRONE_RADIUS;
                            Player->ZeroPoint_DrawBeam = ( Player->ZeroPoint_Dist > 0.0f );
                            
                            SecurityDrone->ZeroPoint_IsActive = true;
                            SecurityDrone->ZeroPoint_Radians  = ToRadians( Dir );
                            SecurityDrone->ZeroPoint_Dist     = GetLength( Dir );
                            
                            Player->ZeroPoint_IsSecurityDrone = true;
                            
                            vec2 V = -GetPerp( GetNormal( SecurityDrone->Position - Player->Position ) );
                            V *= ( Player->Rotate_Speed * SecurityDrone->ZeroPoint_Dist * SECURITY_DRONE_ZERO_POINT_ADJUST_SPEED_RATIO );
                            
                            flo32 t = dot( SecurityDrone->Velocity, V ) / GetLengthSq( V );
                            Player->ZeroPoint_IsSecurityDrone_tSpeed = t;
                            
                            Player->Rotate_Speed *= t;
                        }
                    } break;
                    
                    default: {
                        InvalidCodePath;
                    } break;
                }
            }
        }
    }
}