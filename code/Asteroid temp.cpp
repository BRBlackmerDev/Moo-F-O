
internal void
AddObservationDeckWindowSmashParticles( APP_STATE * AppState, vec2 bPos ) {
    int32 nParticles = 512;
    
    flo32 bRadians = PI * 1.5f;
    flo32 Pos_Range = OBSERVATION_DECK_WINDOW_RADIANS;
    flo32 Dir_Range = PI * 0.75f;
    flo32 Radius_Inner = OBSERVATION_DECK_RADIUS * 0.85f;
    flo32 Radius_Outer = OBSERVATION_DECK_RADIUS - 0.35f;
    
    for( int32 iParticle = 0; iParticle < nParticles; iParticle++ ) {
        PARTICLE Particle = {};
        
        flo32 Pos_Radians = bRadians + RandomF32InRange( -0.5f, 0.5f ) * Pos_Range;
        flo32 Radius      = RandomF32InRange( Radius_Inner, Radius_Outer );
        flo32 Dir_Radians = bRadians + RandomF32InRange( -0.5f, 0.5f ) * Dir_Range;
        flo32 Speed       = RandomF32InRange( 12.0f, 24.0f );
        
        Particle.Position = bPos + ToDirection2D( Pos_Radians ) * Radius;
        Particle.Velocity = ToDirection2D( Dir_Radians ) * Speed;
        Particle.Friction = RandomF32InRange( 0.4f, 0.8f );
        
        flo32 xDim = RandomF32InRange( 0.1f, 0.2f );
        flo32 yDim = RandomF32InRange( 0.1f, 0.2f );
        Particle.Dim      = Vec2( xDim, yDim );
        
        Particle.Radians  = RandomF32() * TAU;
        Particle.dRadians = RandomF32InRange( -1.0f, 1.0f ) * ( TAU / 64.0f );
        
        Particle.TargetTime = ( flo32 )RandomS32InRange( 60, 300 ) / 60.0f;
        
        flo32 tGray = RandomF32InRange( 0.2f, 0.5f );
        Particle.Color      = COLOR_GRAY( tGray );
        
        AddParticle( AppState, Particle );
    }
}

internal ASTEROID
AsteroidC( ASTEROID_STATE * AsteroidS, int32 WaveGuid, ASTEROID_CLASS Class, vec2 Position, vec2 Direction, flo32 Speed, uint32 Flags = ( ASTEROID_FLAG__CAN_SPAWN_CHILDREN |
                                                                                                                                         ASTEROID_FLAG__CAN_USE_ZERO_POINT_ENERGY | ASTEROID_FLAG__ENABLE_DEFAULT_POINT_SPAWN ) ) {
    ASTEROID_CLASS_RADIUS;
    ASTEROID_CLASS_HEALTH;
    
    ASTEROID Result = {};
    Result.WaveGuid     = WaveGuid;
    Result.Guid         = AsteroidS->CurrentGuid++;
    Result.TextureID    = ( TEXTURE_ID )( TextureID_Asteroid00 + RandomS32InRange( 0, ASTEROID_TEXTURE_ID_MAX_COUNT - 1 ) );
    Result.Class        = Class;
    Result.Position     = Position;
    Result.Direction    = Direction;
    Result.Speed        = Speed;
    Result.Radius       = AsteroidClassRadius[ Class ];
    Result.Health       = AsteroidClassHealth[ Class ];
    Result.Radians      = RandomF32() * TAU;
    Result.dRadians     = RandomF32InRange( -1.0f, 1.0f ) * ( TAU / 6.0f );
    Result.Flags        = Flags;
    Result.tColor       = RandomF32InRange( 0.75f, 1.25f );
    
    return Result;
}

internal void
AddAsteroid( APP_STATE * AppState, ASTEROID Asteroid ) {
    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
    if( AsteroidS->nAsteroid < ASTEROID_MAX_COUNT ) {
        AsteroidS->Asteroid[ AsteroidS->nAsteroid++ ] = Asteroid;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add ASTEROID to ASTEROID_STATE: %u", ASTEROID_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateAsteroid( APP_STATE * AppState, flo32 dT ) {
    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
    PLAYER_STATE   * Player    = &AppState->Player;
    
    if( AsteroidS->Spawn_IsActive ) {
        AsteroidS->Spawn_Timer += dT;
        
        int32 MaxAsteroid = ASTEROID_MAX_COUNT;
        if( AsteroidS->Spawn_MaxAsteroid > -1 ) {
            MaxAsteroid = ( uint32 )AsteroidS->Spawn_MaxAsteroid;
        }
        
        if( ( AsteroidS->nAsteroid < MaxAsteroid ) && ( AsteroidS->Spawn_Timer >= AsteroidS->Spawn_TargetTime ) ) {
            AsteroidS->Spawn_Timer = 0.0f;
            
            ASTEROID_CLASS_RADIUS;
            
            ASTEROID_CLASS Class  = {};
            uint32         Flags  = 0;
            {
                int32 Sum = 0;
                int32 Range[ AsteroidType_Count * AsteroidClass_Count + 1 ] = {};
                
                for( uint32 iType = 0; iType < AsteroidType_Count; iType++ ) {
                    for( uint32 iClass = 0; iClass < AsteroidClass_Count; iClass++ ) {
                        Sum += AsteroidS->Numer_TypeClass[ iType ][ iClass ];
                        
                        uint32 Index = ( iType * AsteroidClass_Count + iClass ) + 1;
                        Range[ Index ] = Sum;
                    }
                }
                
                Assert( Sum > 0 );
                int32 Select = RandomS32InRange( 0, Sum - 1 );
                for( uint32 iType = 0; iType < AsteroidType_Count; iType++ ) {
                    for( uint32 iClass = 0; iClass < AsteroidClass_Count; iClass++ ) {
                        if( AsteroidS->Numer_TypeClass[ iType ][ iClass ] > 0 ) {
                            uint32 Index = ( iType * AsteroidClass_Count + iClass );
                            int32  Lo    = Range[ Index     ];
                            int32  Hi    = Range[ Index + 1 ];
                            
                            if( ( Select >= Lo ) && ( Select < Hi ) ) {
                                Class = ( ASTEROID_CLASS )iClass;
                                
                                switch( iType ) {
                                    case AsteroidType_Default: {
                                        Flags |= ASTEROID_FLAG__CAN_SPAWN_CHILDREN;
                                        Flags |= ASTEROID_FLAG__CAN_USE_ZERO_POINT_ENERGY;
                                        Flags |= ASTEROID_FLAG__ENABLE_DEFAULT_POINT_SPAWN;
                                    } break;
                                    
                                    case AsteroidType_BombOnly: {
                                        Flags |= ASTEROID_FLAG__BOMB_ONLY;
                                    } break;
                                    
                                    case AsteroidType_LaserOnly: {
                                        Flags |= ASTEROID_FLAG__LASER_ONLY;
                                    } break;
                                    
                                    case AsteroidType_SwarmOnly: {
                                        Flags |= ASTEROID_FLAG__SWARM_ONLY;
                                    } break;
                                    
                                    default: {
                                        InvalidCodePath;
                                    } break;
                                }
                            }
                        }
                    }
                }
            }
            
            flo32 Radius = AsteroidClassRadius[ Class ];
            
            rect Spawn_Bound = AddRadius( AppState->World_Bound, Radius + 1.0f );
            
            vec2  Pos   = GetRandomPointOnPerimeter( Spawn_Bound, AsteroidS->Spawn_SideIsEnabled[ 0 ], AsteroidS->Spawn_SideIsEnabled[ 1 ], AsteroidS->Spawn_SideIsEnabled[ 2 ],
                                                    AsteroidS->Spawn_SideIsEnabled[ 3 ] );
            vec2  Dest  = GetRandomPoint( AsteroidS->Spawn_Bound );
            Dest = Player->Position;
            
            vec2  Dir   = GetNormal( Dest - Pos );
            flo32 Speed = 4.0f;
            
            ESCORT_STATE * EscortS = &AppState->EscortS;
            if( EscortS->nEscort > 0 ) {
                ESCORT Escort = EscortS->Escort[ 0 ];
                
                flo32 Denom = ( flo32 )ESCORT_BONUS_POINTS_MAX_INDICATOR;
                flo32 tA = Clamp01( ( flo32 )Escort.BonusPoints_AtIndicator[ 0 ] / Denom ) * 0.5f;
                flo32 tB = Clamp01( ( flo32 )Escort.BonusPoints_AtIndicator[ 1 ] / Denom ) * 0.5f;
                
                flo32 WorldT = Clamp01( ( Pos.x - AppState->World_Bound.Left ) / GetWidth( AppState->World_Bound ) );
                
                if( Pos.x < 0.0f ) {
                    Speed = lerp( 2.0f, ( tA + tB ) * (   WorldT          * 2.0f ), 4.0f );
                } else {
                    Speed = lerp( 4.0f, ( tA + tB ) * ( ( WorldT - 0.5f ) * 2.0f ), 10.0f );
                }
            }
            
            ASTEROID Asteroid = AsteroidC( AsteroidS, -1, Class, Pos, Dir, Speed, Flags );
            AddAsteroid( AppState, Asteroid );
        }
    }
    
    {
        ESCORT_STATE * EscortS = &AppState->EscortS;
        if( EscortS->nEscort > 0 ) {
            
            for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                
                vec2 Velocity = Asteroid->Direction * Asteroid->Speed;
                Velocity += Vec2( -1.0f, 0.0f ) * dT;
                
                Asteroid->Direction = GetNormal( Velocity );
                Asteroid->Speed     = GetLength( Velocity );
            }
        }
    }
    
    {
        uint32 NewCount = 0;
        for( uint32 iExclude = 0; iExclude < AsteroidS->Collide_nExclude; iExclude++ ) {
            UINT32_PAIR Exclude = AsteroidS->Collide_iExclude[ iExclude ];
            
            ASTEROID * AsteroidA = 0;
            ASTEROID * AsteroidB = 0;
            
            for( int32 Iter = 0; Iter < AsteroidS->nAsteroid; Iter++ ) {
                ASTEROID * Asteroid = AsteroidS->Asteroid + Iter;
                if( ( uint32 )Asteroid->Guid == Exclude.elem[ 0 ] ) {
                    AsteroidA = Asteroid;
                }
                if( ( uint32 )Asteroid->Guid == Exclude.elem[ 1 ] ) {
                    AsteroidB = Asteroid;
                }
            }
            
            boo32 DoRemove = true;
            if( ( AsteroidA ) && ( AsteroidB ) ) {
                flo32 Radius   = AsteroidA->Radius + AsteroidB->Radius;
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( AsteroidA->Position - AsteroidB->Position );
                if( DistSq <= RadiusSq ) {
                    DoRemove = false;
                }
            }
            
            if( !DoRemove ) {
                AsteroidS->Collide_iExclude[ NewCount++ ] = Exclude;
            }
        }
        AsteroidS->Collide_nExclude = NewCount;
    }
    
    for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
        ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
        
        Asteroid->Damage_Timer = MaxValue( Asteroid->Damage_Timer - dT, 0.0f );
        
        vec2 dPos = Asteroid->Direction * ( Asteroid->Speed * dT );
        
        if( !Player->ZeroPoint_IsActive ) {
            Asteroid->ZeroPoint_IsActive = false;
        }
        if( Player->IsDead ) {
            Asteroid->ZeroPoint_ShowUI   = false;
        }
        
        if( Asteroid->ZeroPoint_IsActive ) {
            flo32 Rotate_Speed = Player->Rotate_Speed;
            
            if( Player->ZeroPoint_Fast_CooldownTimer > 0.0f ) {
                Rotate_Speed = PLAYER_ROTATE_SPEED_FAST;
            }
            
            Asteroid->ZeroPoint_Radians = WrapRadians( Asteroid->ZeroPoint_Radians - Rotate_Speed * dT );
            
            vec2 Dir = ToDirection2D( Asteroid->ZeroPoint_Radians );
            
            Asteroid->Position  = Player->Position + Dir * Asteroid->ZeroPoint_Dist;
            Asteroid->Direction = -GetPerp( ToDirection2D( Asteroid->ZeroPoint_Radians ) );
            Asteroid->Speed     = Rotate_Speed * Asteroid->ZeroPoint_Dist;
            Asteroid->dRadians  = RandomF32InRange( -1.0f, 1.0f ) * ( TAU / 24.0f );
        } else {
            Asteroid->Position += dPos;
            Asteroid->Radians   = WrapRadians( Asteroid->Radians + Asteroid->dRadians * dT );
        }
    }
    
    //DISPLAY_VALUE( uint32, AsteroidS->nAsteroid );
}

internal void
DrawAsteroid( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, ASTEROID Asteroid, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    { // Draw Zero Point
        if( Asteroid.ZeroPoint_IsActive ) {
            vec4  Color  = PLAYER_GADGET_BASE_COLOR;
            flo32 Radius = ( Asteroid.Radius + 0.1f ) * 1.125f;
            
            rect Bound = RectCR( Asteroid.Position, Radius );
            DrawRect( Pass, TextureID_ZeroPoint_Base_Active, Bound, Color );
        } else if( Asteroid.ZeroPoint_ShowUI ) {
            vec4  Color  = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.5f );
            flo32 Radius = ( Asteroid.Radius + 0.1f ) * 1.0625f;
            
            rect Bound = RectCR( Asteroid.Position, Radius );
            DrawRect( Pass, TextureID_ZeroPoint_Base_Inactive, Bound, Color );
        }
    }
    
    vec4 Color_Health = COLOR_WHITE;
    
    vec4 ColorA = TowardBlack( ASTEROID_DEFAULT_BASE_COLOR, 0.55f );
    if( Asteroid.Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD ) {
        ColorA = TowardBlack( ASTEROID_DEFAULT_BASE_COLOR, 0.15f );
    }
    
    if( Asteroid.Flags & ASTEROID_FLAG__BOMB_ONLY ) {
        ColorA = Vec4( 0.1f, 0.05f, 0.125f, 1.0f );
    }
    if( Asteroid.Flags & ASTEROID_FLAG__LASER_ONLY ) {
        ColorA = Vec4( 0.125f, 0.0f, 0.025f, 1.0f );
        if( Asteroid.Health == 0 ) {
            ColorA = COLOR_WHITE;
        }
    }
    if( Asteroid.Flags & ASTEROID_FLAG__SWARM_ONLY ) {
        ColorA = TowardBlack( ASTEROID_SWARM_ONLY_BASE_COLOR, 0.55f );
        if( Asteroid.Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD ) {
            ColorA = TowardBlack( ASTEROID_SWARM_ONLY_BASE_COLOR, 0.15f );
        }
    }
    
    ColorA.rgb *= Asteroid.tColor;
    
    {
        flo32 tDamage = Clamp01( Asteroid.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
        ColorA = lerp( ColorA, tDamage, TowardWhite( ColorA, 0.5f ) );
    }
    
    //DrawCircle( Pass, Asteroid.Position, Asteroid.Radius * Scale, COLOR_GREEN );
    
    orect Bound = ORectCR( Asteroid.Position, Asteroid.Radius * Scale, Asteroid.Radians );
    DrawORect( Pass, Asteroid.TextureID, Bound, ColorA * Alpha );
    
    //DrawCircleOutline( Pass, Asteroid.Position, Asteroid.Radius * Scale, COLOR_RED );
    
    if( Asteroid.Health > 0 ) {
        DrawEntityHealth( Pass, AppState, Camera, Asteroid.Position, Asteroid.Health, Color_Health );
        
        //sprintf( Str, "%d", ( int32 )Asteroid.Speed );
        //DrawString( Pass, Font, Str, Asteroid.Position + Vec2( 0.0f, -0.6f ), TextAlign_Center, WORLD_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE );
    }
}

internal void
DrawAsteroid( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    ASTEROID_STATE * AsteroidS = Draw->AsteroidS;
    
    for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
        ASTEROID Asteroid = AsteroidS->Asteroid[ iAsteroid ];
        DrawAsteroid( Pass, AppState, Camera, Asteroid );
    }
}

internal void
FinalizeAsteroid( APP_STATE * AppState ) {
    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
    
    {
        flo32 Margin = 0.2f;
        
        for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
            ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
            
            for( int32 iCollide = iAsteroid + 1; ( Asteroid->Health > 0 ) && ( iCollide < AsteroidS->nAsteroid ); iCollide++ ) {
                ASTEROID * Collide = AsteroidS->Asteroid + iCollide;
                
                if( Collide->Health > 0 ) {
                    boo32 DoCollide = true;
                    
                    {
                        Assert( Asteroid->Guid > -1 );
                        Assert( Collide->Guid  > -1 );
                        
                        uint32 GuidA = ( uint32 )Asteroid->Guid;
                        uint32 GuidB = ( uint32 )Collide->Guid;
                        if( GuidA > GuidB ) {
                            _swap( uint32, GuidA, GuidB );
                        }
                        
                        for( uint32 iExclude = 0; ( DoCollide ) && ( iExclude < AsteroidS->Collide_nExclude ); iExclude++ ) {
                            UINT32_PAIR Exclude = AsteroidS->Collide_iExclude[ iExclude ];
                            if( ( Exclude.elem[ 0 ] == GuidA ) && ( Exclude.elem[ 1 ] == GuidB ) ) {
                                DoCollide = false;
                            }
                        }
                    }
                    
                    if( DoCollide ) {
                        // NOTE: If an Asteroid is Zero-Pointed and collides with an Asteroid out-of-bounds, then the collision is invalid and the colliding Asteroid should be removed.
                        // NOTE: If an Asteroid is Zero-Pointed out-of-bounds and collides with an Asteroid in-bounds, the collision is still valid.
                        rect Asteroid_Bound = AddRadius( AppState->World_Bound, Asteroid->Radius - 0.5f );
                        rect Collide_Bound  = AddRadius( AppState->World_Bound, Collide->Radius  - 0.5f );
                        boo32 Asteroid_IsValid = IsInRect( Asteroid->Position, Asteroid_Bound );
                        boo32 Collide_IsValid  = IsInRect( Collide->Position,  Collide_Bound );
                        
                        flo32 Collide_Radius   = ( Asteroid->Radius + Collide->Radius ) * 0.9f;
                        flo32 Collide_RadiusSq = Collide_Radius * Collide_Radius;
                        flo32 Collide_DistSq   = GetLengthSq( Asteroid->Position - Collide->Position );
                        
                        if( Collide_DistSq <= Collide_RadiusSq ) {
                            if( ( Asteroid->ZeroPoint_IsActive ) && ( !Collide_IsValid ) ) {
                                DoCollide = false;
                                Collide->Health = 0;
                                Collide->Flags  = 0;
                            }
                            if( ( Collide->ZeroPoint_IsActive ) && ( !Asteroid_IsValid ) ) {
                                DoCollide = false;
                                Asteroid->Health = 0;
                                Asteroid->Flags  = 0;
                            }
                        }
                    }
                    
                    if( DoCollide ) {
                        flo32 Collide_Radius   = ( Asteroid->Radius + Collide->Radius ) * 0.9f;
                        flo32 Collide_RadiusSq = Collide_Radius * Collide_Radius;
                        flo32 Collide_DistSq   = GetLengthSq( Asteroid->Position - Collide->Position );
                        
                        if( Collide_DistSq <= Collide_RadiusSq ) {
                            int32 DamageLo[ AsteroidClass_Count ] = { 2,  8, 30, 75  };
                            int32 DamageHi[ AsteroidClass_Count ] = { 3, 15, 60, 150 };
                            
                            if( !( ( Asteroid->Flags & ASTEROID_FLAG__BOMB_ONLY ) || ( Asteroid->Flags & ASTEROID_FLAG__LASER_ONLY ) ||
                                  ( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) ) {
                                int32 Damage = DamageLo[ Collide->Class ];
                                if( Collide->Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD ) {
                                    Damage = DamageHi[ Collide->Class ];
                                    
                                    int32 nScoreMultiplier = ( int32 )Collide->Class + 1;
                                    vec2 Pos = ClampToRect( ( Asteroid->Position + Collide->Position ) * 0.5f, AppState->World_Bound );
                                    for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                                        AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                                    }
                                }
                                
                                Asteroid->Health       = MaxValue( Asteroid->Health - Damage, 0 );
                                Asteroid->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                            }
                            
                            if( !( ( Collide->Flags & ASTEROID_FLAG__BOMB_ONLY ) || ( Collide->Flags & ASTEROID_FLAG__LASER_ONLY ) ||
                                  ( Collide->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) ) {
                                int32 Damage = DamageLo[ Asteroid->Class ];
                                if( Asteroid->Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD ) {
                                    Damage = DamageHi[ Asteroid->Class ];
                                    
                                    
                                    int32 nScoreMultiplier = ( int32 )Asteroid->Class + 1;
                                    vec2 Pos = ClampToRect( ( Asteroid->Position + Collide->Position ) * 0.5f, AppState->World_Bound );
                                    for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                                        AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                                    }
                                }
                                
                                Collide->Health        = MaxValue( Collide->Health - Damage, 0 );
                                Asteroid->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                            }
                            
                            vec2 Dir = GetNormal( Collide->Position - Asteroid->Position );
                            
                            flo32 mA = Asteroid->Radius;
                            vec2  vA = Asteroid->Direction * Asteroid->Speed;
                            
                            flo32 mB = Collide->Radius;
                            vec2  vB = Collide->Direction * Collide->Speed;
                            
                            flo32 v1 = dot( vA, Dir );
                            flo32 v2 = dot( vB, Dir );
                            
                            flo32 vAf = ( ( mA * v1 ) + ( mB * v2 ) + mB * ( v2 - v1 ) ) / ( mA + mB );
                            flo32 vBf = ( ( mA * v1 ) + ( mB * v2 ) + mA * ( v1 - v2 ) ) / ( mA + mB );
                            
                            vA += Dir * ( vAf - v1 );
                            vB += Dir * ( vBf - v2 );
                            
                            flo32 DotA      = dot( GetPerp( Asteroid->Direction ), Dir );
                            flo32 DotB      = dot( GetPerp( Collide->Direction  ), Dir );
                            flo32 tSpeedA   = ( Collide->Speed  / ASTEROID_HI_DAMAGE_SPEED_THRESHOLD );
                            flo32 tSpeedB   = ( Asteroid->Speed / ASTEROID_HI_DAMAGE_SPEED_THRESHOLD );
                            flo32 tRadiansA = ( Collide->Radius  / Asteroid->Radius ) * 0.6f;
                            flo32 tRadiansB = ( Asteroid->Radius / Collide->Radius  ) * 0.6f;
                            flo32 dRadiansA = Asteroid->dRadians - ( Collide->dRadians  * tRadiansA );
                            flo32 dRadiansB = Collide->dRadians  - ( Asteroid->dRadians * tRadiansB );
                            dRadiansA += ( tSpeedA * DotA * ( Collide->Radius / Asteroid->Radius ) * ( TAU / 4.5f ) );
                            dRadiansB -= ( tSpeedB * DotB * ( Asteroid->Radius / Collide->Radius ) * ( TAU / 4.5f ) );
                            
                            {
                                vec4  ColorA  = GetAsteroidBaseColor( *Asteroid );
                                vec4  ColorB  = GetAsteroidBaseColor( *Collide  );
                                
                                int32 nParticlesA = ( int32 )( tRadiansA * tSpeedA ) + 1;
                                int32 nParticlesB = ( int32 )( tRadiansB * tSpeedB ) + 1;
                                
                                vec2 PosA = Asteroid->Position + Dir * Asteroid->Radius;
                                vec2 PosB = Collide->Position  - Dir * Collide->Radius;
                                vec2 Pos  = ( PosA + PosB ) * 0.5f;
                                
                                for( int32 iParticle = 0; iParticle < nParticlesA; iParticle++ ){
                                    AddBulletHitParticles( AppState, Pos, Collide->Direction,   Dir, ColorA );
                                }
                                for( int32 iParticle = 0; iParticle < nParticlesB; iParticle++ ){
                                    AddBulletHitParticles( AppState, Pos, Asteroid->Direction, -Dir, ColorB );
                                }
                            }
                            
                            if( !Asteroid->ZeroPoint_IsActive ) {
                                Asteroid->Direction = GetNormal( vA );
                                Asteroid->Speed     = GetLength( vA );
                                Asteroid->dRadians  = dRadiansA;
                            }
                            
                            if( !Collide->ZeroPoint_IsActive ) {
                                Collide->Direction  = GetNormal( vB );
                                Collide->Speed      = GetLength( vB );
                                Collide->dRadians   = dRadiansB;
                            }
                            
                            {
                                UINT32_PAIR Exclude = {};
                                Exclude.elem[ 0 ] = Asteroid->Guid;
                                Exclude.elem[ 1 ] = Collide->Guid;
                                if( Exclude.elem[ 0 ] > Exclude.elem[ 1 ] ) {
                                    _swap( uint32, Exclude.elem[ 0 ], Exclude.elem[ 1 ] );
                                }
                                
                                Assert( AsteroidS->Collide_nExclude < ASTEROID_COLLIDE_EXCLUDE_MAX_COUNT );
                                AsteroidS->Collide_iExclude[ AsteroidS->Collide_nExclude++ ] = Exclude;
                            }
                        }
                    }
                }
            }
            
            { // MINER
                if( Asteroid->Health > 0 ) {
                    MINER_STATE * MinerS = &AppState->MinerS;
                    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                        MINER * Miner = MinerS->Miner + iMiner;
                        
                        flo32 Radius   = ( Asteroid->Radius * 0.9f ) + Miner->Radius - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Miner->Position - Asteroid->Position );
                        if( DistSq <= RadiusSq ) {
                            if( !( ( Asteroid->Flags & ASTEROID_FLAG__BOMB_ONLY ) || ( Asteroid->Flags & ASTEROID_FLAG__LASER_ONLY ) ||
                                  ( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) ) {
                                Asteroid->Health = 0;
                            }
                            
                            int32 DamageLo[ AsteroidClass_Count ] = { 0,  1,  5, 10 };
                            int32 DamageHi[ AsteroidClass_Count ] = { 2, 13, 24, 35 };
                            
                            int32 Damage = DamageLo[ Asteroid->Class ];
                            if( Asteroid->Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD ) {
                                Damage = DamageHi[ Asteroid->Class ];
                                
                                int32 nScoreMultiplier = ( int32 )Asteroid->Class + 1;
                                vec2 Pos = ClampToRect( ( Asteroid->Position + Miner->Position ) * 0.5f, AppState->World_Bound );
                                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                                    AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                                }
                            }
                            
                            Miner->Health       = MaxValue( Miner->Health - Damage, 0 );
                            Miner->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                            
                            vec2 Dir = GetNormal( Asteroid->Position - Miner->Position );
                            
                            flo32 mA = Asteroid->Radius;
                            vec2  vA = Asteroid->Direction * Asteroid->Speed;
                            
                            flo32 mB = Miner->Radius;
                            vec2  vB = Miner->Velocity;
                            
                            flo32 v1 = dot( vA, Dir );
                            flo32 v2 = dot( vB, Dir );
                            
                            flo32 vAf = ( ( mA * v1 ) + ( mB * v2 ) + mB * ( v2 - v1 ) ) / ( mA + mB );
                            flo32 vBf = ( ( mA * v1 ) + ( mB * v2 ) + mA * ( v1 - v2 ) ) / ( mA + mB );
                            
                            vA += Dir * ( vAf - v1 );
                            vB += Dir * ( vBf - v2 );
                            
                            vA +=  Dir * 1.0f;
                            vB += -Dir * 0.5f;
                            
                            flo32 DotA      = dot( GetPerp( Asteroid->Direction ), Dir );
                            flo32 tRadiansA = ( Miner->Radius  / Asteroid->Radius ) * 0.6f;
                            flo32 dRadiansA = ( Asteroid->dRadians - ( DotA * ( TAU / 4.5f ) ) ) * tRadiansA;
                            
                            Asteroid->Direction = GetNormal( vA );
                            Asteroid->Speed     = GetLength( vA ) + 1.0f;
                            Asteroid->dRadians  = dRadiansA;
                            
                            Miner->Velocity = vB;
                            
                            { // Particles
                                vec2  Pos  = ( Asteroid->Position + Miner->Position ) * 0.5f;
                                vec2  Norm = GetNormal( Asteroid->Position - Miner->Position );
                                boo32 DoHi = ( Asteroid->Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD );
                                
                                vec4 bColor = MINER_STATIONARY_BASE_COLOR;
                                if( Miner->IsAutonomous ) {
                                    bColor = MINER_AUTONOMOUS_BASE_COLOR;
                                }
                                
                                AddEntityDeathParticles( AppState, Pos, ASTEROID_DEFAULT_BASE_COLOR );
                                AddEntityDeathParticles( AppState, Pos, bColor );
                                
                                if( DoHi ) {
                                    AddEntityDeathParticles( AppState, Pos, bColor );
                                }
                            }
                        }
                    }
                }
            }
            
            { // MEGA_MINER
                if( Asteroid->Health > 0 ) {
                    MINER_STATE * MinerS = &AppState->MinerS;
                    for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
                        MEGA_MINER * MegaMiner = MinerS->MegaMiner + iMegaMiner;
                        
                        { // Collide with Mega-Miner Drill
                            vec2 yAxis = GetNormal( ToDirection2D( MegaMiner->AtRadians ) );
                            vec2 xAxis = -GetPerp( yAxis );
                            
                            vec2 bPos  = MegaMiner->Position + yAxis * MEGA_MINER_DRILL_OFFSET;
                            vec2 Scale = MEGA_MINER_DRILL_SCALE;
                            vec2 A     = bPos + xAxis * -Scale.x + yAxis * -Scale.y;
                            vec2 B     = bPos                    + yAxis *  Scale.y;
                            vec2 C     = bPos + xAxis *  Scale.x + yAxis * -Scale.y;
                            
                            flo32 Radius   = ( Asteroid->Radius * 0.9f );
                            flo32 RadiusSq = Radius * Radius;
                            flo32 DistSq   = DistSqPointToTri( Asteroid->Position, A, B, C );
                            if( DistSq <= RadiusSq ) {
                                Asteroid->Health = 0;
                                
                                vec2 Pos = ClosestPointToTriPerimeter( Asteroid->Position, A, B, C );
                                AddEntityDeathParticles( AppState, Pos, ASTEROID_DEFAULT_BASE_COLOR );
                                AddEntityDeathParticles( AppState, Pos, COLOR_WHITE );
                                
                                if( Asteroid->Class > AsteroidClass_Tiny ) {
                                    vec2 P = ClampToRect( Pos, AppState->World_Bound );
                                    AddScoreMultiplier( AppState, NewScoreMultiplier( P, AppState->World_Bound, true ) );
                                    
                                }
                            }
                        }
                        
                        if( Asteroid->Health > 0 ) {
                            flo32 Radius   = ( Asteroid->Radius * 0.9f ) + MegaMiner->Radius - 0.1f;
                            flo32 RadiusSq = Radius * Radius;
                            flo32 DistSq   = GetLengthSq( MegaMiner->Position - Asteroid->Position );
                            if( DistSq <= RadiusSq ) {
                                if( !( ( Asteroid->Flags & ASTEROID_FLAG__BOMB_ONLY ) || ( Asteroid->Flags & ASTEROID_FLAG__LASER_ONLY ) ||
                                      ( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) ) {
                                    Asteroid->Health = 0;
                                }
                                
                                int32 DamageLo[ AsteroidClass_Count ] = { 0,  1,  2,  5 };
                                int32 DamageHi[ AsteroidClass_Count ] = { 1,  7, 15, 25 };
                                
                                int32 Damage = DamageLo[ Asteroid->Class ];
                                if( Asteroid->Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD ) {
                                    Damage = DamageHi[ Asteroid->Class ];
                                    
                                    int32 nScoreMultiplier = ( int32 )Asteroid->Class + 1;
                                    vec2 Pos = ClampToRect( ( Asteroid->Position + MegaMiner->Position ) * 0.5f, AppState->World_Bound );
                                    for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                                        AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                                    }
                                }
                                
                                MegaMiner->Health = MaxValue( MegaMiner->Health - Damage, 0 );
                                
                                if( Damage == 1 ) {
                                    MegaMiner->ShowDamage_Timer = ( 24.0f / 60.0f );
                                } else if( Damage > 1 ) {
                                    MegaMiner->ShowDamage_Timer = ( 60.0f / 60.0f );
                                }
                                
                                vec2 Dir = GetNormal( Asteroid->Position - MegaMiner->Position );
                                
                                flo32 mA = Asteroid->Radius;
                                vec2  vA = Asteroid->Direction * Asteroid->Speed;
                                
                                flo32 mB = MegaMiner->Radius * 2.0f;
                                vec2  vB = MegaMiner->Velocity;
                                
                                flo32 v1 = dot( vA, Dir );
                                flo32 v2 = dot( vB, Dir );
                                
                                flo32 vAf = ( ( mA * v1 ) + ( mB * v2 ) + mB * ( v2 - v1 ) ) / ( mA + mB );
                                flo32 vBf = ( ( mA * v1 ) + ( mB * v2 ) + mA * ( v1 - v2 ) ) / ( mA + mB );
                                
                                vA += Dir * ( vAf - v1 );
                                vB += Dir * ( vBf - v2 );
                                
                                vA +=  Dir * 1.0f;
                                vB += -Dir * 0.5f;
                                
                                flo32 DotA      = dot( GetPerp( Asteroid->Direction ), Dir );
                                flo32 tRadiansA = ( MegaMiner->Radius  / Asteroid->Radius ) * 0.6f;
                                flo32 dRadiansA = ( Asteroid->dRadians - ( DotA * ( TAU / 4.5f ) ) ) * tRadiansA;
                                
                                Asteroid->Direction = GetNormal( vA );
                                Asteroid->Speed     = GetLength( vA ) + 1.0f;
                                Asteroid->dRadians  = dRadiansA;
                                
                                MegaMiner->Velocity = vB;
                                
                                { // Particles
                                    vec2  Pos  = ( Asteroid->Position + MegaMiner->Position ) * 0.5f;
                                    vec2  Norm = GetNormal( Asteroid->Position - MegaMiner->Position );
                                    boo32 DoHi = ( Asteroid->Speed >= ASTEROID_HI_DAMAGE_SPEED_THRESHOLD );
                                    
                                    AddEntityDeathParticles( AppState, Pos, ASTEROID_DEFAULT_BASE_COLOR );
                                    AddEntityDeathParticles( AppState, Pos, MEGA_MINER_BASE_COLOR );
                                    if( DoHi ) {
                                        AddEntityDeathParticles( AppState, Pos, MEGA_MINER_BASE_COLOR );
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            { // SWARM
                if( ( Asteroid->Health > 0 ) && ( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) {
                    SWARM_STATE * SwarmS = &AppState->SwarmS;
                    for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
                        SWARM * Swarm = SwarmS->Swarm + iSwarm;
                        
                        flo32 Radius   = ( Asteroid->Radius * 0.9f ) + Swarm->Radius - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Swarm->Position - Asteroid->Position );
                        if( DistSq <= RadiusSq ) {
                            Asteroid->Health       = MaxValue( Asteroid->Health - 1, 0 );
                            Asteroid->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                            
                            Swarm->Health = 0;
                            
                            vec2 Pos   = Swarm->Position;
                            vec2 Dir   = GetNormal( Swarm->Velocity );
                            vec2 Norm  = GetNormal( Swarm->Position - Asteroid->Position );
                            vec4 Color = SWARM_BASE_COLOR;
                            
                            AddBulletHitParticles( AppState, Pos, Dir, Norm, Color );
                            AddBulletHitParticles( AppState, Pos, Dir, Norm, Color );
                            AddBulletHitParticles( AppState, Pos, Dir, Norm, Color );
                            AddBulletHitParticles( AppState, Pos, Dir, Norm, COLOR_WHITE );
                        }
                    }
                }
            }
            
            { // SATELLITE
                if( Asteroid->Health > 0 ) {
                    SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
                    if( SatelliteS->nSatellite > 0 ) {
                        rect Bound = AddRadius( SatelliteS->Shield_Bound, Asteroid->Radius - 0.2f );
                        if( IsInRect( Asteroid->Position, Bound ) ) {
                            Asteroid->Position = ClosestPointToRectPerimeter( Asteroid->Position, Bound );
                            Asteroid->Direction -= ( 2.0f * dot( Asteroid->Direction, SatelliteS->Shield_BounceDir ) ) * SatelliteS->Shield_BounceDir;
                            Asteroid->dRadians  *= 0.6f;
                            
                            ASTEROID_CLASS_RADIUS;
                            flo32 tRadius = Asteroid->Radius / AsteroidClassRadius[ AsteroidClass_Large ];
                            flo32 tSpeed  = Clamp01( Asteroid->Speed / ASTEROID_HI_DAMAGE_SPEED_THRESHOLD );
                            
                            SATELLITE_SHIELD_HIT ShieldHit = {};
                            ShieldHit.Pos   = ClosestPointToRectPerimeter( Asteroid->Position, SatelliteS->Shield_Bound );
                            ShieldHit.Scale = RandomF32InRange( 1.5f, 2.0f ) * ( tRadius * tSpeed );
                            ShieldHit.Timer = SATELLITE_SHIELD_HIT_TARGET_TIME;
                            
                            SatelliteS->ShieldHit[ SatelliteS->iShieldHit ] = ShieldHit;
                            SatelliteS->iShieldHit = ( SatelliteS->iShieldHit + 1 ) % SATELLITE_SHIELD_HIT_MAX_COUNT;
                        }
                    }
                }
            }
        }
    }
    
    { // Spawn New Asteroids
        int32 nAsteroid = AsteroidS->nAsteroid;
        for( int32 iAsteroid = 0; iAsteroid < nAsteroid; iAsteroid++ ) {
            ASTEROID Asteroid = AsteroidS->Asteroid[ iAsteroid ];
            if( ( Asteroid.Health == 0 ) && ( Asteroid.Class > AsteroidClass_Tiny ) && ( Asteroid.Flags & ASTEROID_FLAG__CAN_SPAWN_CHILDREN ) ) {
                ASTEROID_CLASS Class  = ( ASTEROID_CLASS )( Asteroid.Class - 1 );
                vec2           Pos    = Asteroid.Position;
                flo32          Speed  = Asteroid.Speed + 1.0f;
                
                vec2 DirA = Asteroid.Direction;
                vec2 DirB = GetPerp( DirA );
                
                vec2 Dir[] = {
                    GetNormal( DirA + DirB ),
                    GetNormal( DirA - DirB ),
                };
                
                ASTEROID AsteroidA = AsteroidC( AsteroidS, -1, Class, Pos, Dir[ 0 ], Speed );
                ASTEROID AsteroidB = AsteroidC( AsteroidS, -1, Class, Pos, Dir[ 1 ], Speed );
                
                AsteroidA.dRadians = ( Asteroid.dRadians * 0.5f ) + RandomF32() * ( TAU / 8.0f );
                AsteroidB.dRadians = ( Asteroid.dRadians * 0.5f ) - RandomF32() * ( TAU / 8.0f );
                
                AddAsteroid( AppState, AsteroidA );
                AddAsteroid( AppState, AsteroidB );
            }
        }
        
        // NOTE: Because of the Asteroid child-collision chain-reaction thing, we have to check overlaps and add exclusions after all new Asteroids are generated. For only the new Asteroids.
        int32 BeginCheckAt = nAsteroid;
        for( int32 iAsteroid = BeginCheckAt; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
            ASTEROID Asteroid = AsteroidS->Asteroid[ iAsteroid ];
            Assert( Asteroid.Health > 0 );
            
            for( int32 iCheck = 0; iCheck < AsteroidS->nAsteroid; iCheck++ ) {
                if( iAsteroid != iCheck ) {
                    ASTEROID Check = AsteroidS->Asteroid[ iCheck ];
                    if( Check.Health > 0 ) {
                        flo32 Radius   = Asteroid.Radius + Check.Radius;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Asteroid.Position - Check.Position );
                        if( DistSq <= RadiusSq ) {
                            UINT32_PAIR Exclude = {};
                            Exclude.elem[ 0 ] = Asteroid.Guid;
                            Exclude.elem[ 1 ] = Check.Guid;
                            if( Exclude.elem[ 0 ] > Exclude.elem[ 1 ] ) {
                                _swap( uint32, Exclude.elem[ 0 ], Exclude.elem[ 1 ] );
                            }
                            
                            Assert( AsteroidS->Collide_nExclude < ASTEROID_COLLIDE_EXCLUDE_MAX_COUNT );
                            AsteroidS->Collide_iExclude[ AsteroidS->Collide_nExclude++ ] = Exclude;
                        }
                    }
                }
            }
        }
        //DISPLAY_VALUE( uint32, AsteroidS->Collide_nExclude );
    }
    
    {
        uint32 NewCount = 0;
        for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
            ASTEROID Asteroid = AsteroidS->Asteroid[ iAsteroid ];
            
            rect Kill_Bound = AddRadius( AppState->World_Bound, Asteroid.Radius + 2.0f );
            
            if( Asteroid.Health == 0 ) {
                if( ( Asteroid.Class == AsteroidClass_Tiny ) && ( Asteroid.Flags & ASTEROID_FLAG__ENABLE_DEFAULT_POINT_SPAWN ) ) {
                    AddKillToScore( AppState, EntityType_Asteroid, Asteroid.Position );
                    
                    POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Asteroid );
                    int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                    
                    vec2 Pos = ClampToRect( Asteroid.Position, AppState->World_Bound );
                    for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                        AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound ) );
                    }
                }
            }
            
            if( ( Asteroid.Flags & ASTEROID_FLAG__BOMB_ONLY ) && ( Asteroid.Health == 0 ) ) {
                uint32 nScoreMultiplier[ AsteroidClass_Count ] = { 1, 5, 10, 25 };
                
                AddKillToScore( AppState, EntityType_Asteroid_BombOnly, Asteroid.Position );
                AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( Asteroid.Position, nScoreMultiplier[ Asteroid.Class ] ) );
            }
            if( ( Asteroid.Flags & ASTEROID_FLAG__LASER_ONLY ) && ( Asteroid.Health == 0 ) ) {
                uint32 nScoreMultiplier[ AsteroidClass_Count ] = { 1, 5, 20, 50 };
                
                AddKillToScore( AppState, EntityType_Asteroid_LaserOnly, Asteroid.Position );
                AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( Asteroid.Position, nScoreMultiplier[ Asteroid.Class ] ) );
            }
            if( ( Asteroid.Flags & ASTEROID_FLAG__SWARM_ONLY ) && ( Asteroid.Health == 0 ) ) {
                uint32 nScoreMultiplier[ AsteroidClass_Count ] = { 1, 5, 10, 25 };
                
                AddKillToScore( AppState, EntityType_Asteroid_SwarmOnly, Asteroid.Position );
                AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( Asteroid.Position, nScoreMultiplier[ Asteroid.Class ] ) );
            }
            
            boo32 DoKeep = ( Asteroid.Health > 0 ) && 
                ( ( IsInRect( Asteroid.Position, Kill_Bound ) ) || ( Asteroid.ZeroPoint_IsActive ) );
            
            if( DoKeep ) {
                AsteroidS->Asteroid[ NewCount++ ] = Asteroid;
            } else {
                if( Asteroid.ZeroPoint_IsActive ) {
                    PLAYER_STATE * Player = &AppState->Player;
                    Player->ZeroPoint_IsActive   = false;
                    Player->ZeroPoint_IsAsteroid = false;
                }
                
                AddAsteroidDeathParticles( AppState, Asteroid );
            }
        }
        AsteroidS->nAsteroid = NewCount;
    }
}