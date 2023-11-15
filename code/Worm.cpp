
internal void
Worm_InitAttack( WORM * Worm ) {
    Worm->DoAttack            = false;
    Worm->DoAttack_Timer      = 0.0f;
    Worm->DoAttack_TargetTime = RandomF32InRange( 2.0f, 4.0f );
    Worm->DoAttack_iAttack    = -1;
    
    if( Worm->Class == WormClass_Small ) {
        Worm->DoAttack_TargetTime = RandomF32InRange( 1.5f, 2.5f );
    }
}

internal WORM
WormC_Large( int32 WaveGuid, vec2 bPos, vec2 bDir ) {
    WORM Result = {};
    Result.WaveGuid     = WaveGuid;
    Result.Class        = WormClass_Large;
    Result.bPos         = bPos;
    Result.Health       = WORM_HEALTH_LARGE;
    Result.Radius       = WORM_RADIUS_LARGE;
    
    Result.Spawn_MaxLength = 12.0f;
    
    Result.Body_Dir     = GetNormal( bDir );
    Result.nBody        = 4;
    Result.Body_Pos[ 0 ] = bPos;
    Result.Body_Pos[ 1 ] = bPos;
    Result.Body_Pos[ 2 ] = bPos;
    Result.Body_Pos[ 3 ] = bPos;
    
    Result.Tag_PosA  = bPos - Result.Body_Dir * ( Result.Radius - 0.2f );
    Result.Tag_PosB  = Result.Tag_PosA - Result.Body_Dir * 1.0f;
    
    Worm_InitAttack( &Result );
    
    return Result;
}

internal WORM
WormC_Small( int32 WaveGuid, vec2 bPos, vec2 bDir ) {
    WORM Result = {};
    Result.WaveGuid     = WaveGuid;
    Result.Class        = WormClass_Small;
    Result.bPos         = bPos;
    Result.Health       = WORM_HEALTH_SMALL;
    Result.Radius       = WORM_RADIUS_SMALL;
    
    Result.Spawn_MaxLength = 4.0f;
    
    Result.Body_Dir     = GetNormal( bDir );
    Result.nBody        = 3;
    Result.Body_Pos[ 0 ] = bPos;
    Result.Body_Pos[ 1 ] = bPos;
    Result.Body_Pos[ 2 ] = bPos;
    Result.Body_Pos[ 3 ] = bPos;
    Result.CanSpawnPoints           = true;
    Result.CanSpawnScoreMultipliers = true;
    
    Result.Tag_PosA  = bPos - Result.Body_Dir * ( Result.Radius - 0.2f );
    Result.Tag_PosB  = Result.Tag_PosA - Result.Body_Dir * 1.0f;
    
    Worm_InitAttack( &Result );
    
    return Result;
}

internal void
AddWorm( APP_STATE * AppState, WORM Worm ) {
    WORM_STATE * WormS = &AppState->WormS;
    if( WormS->nWorm < WORM_MAX_COUNT ) {
        WormS->Worm[ WormS->nWorm++ ] = Worm;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add WORM to WORM_STATE: %u", WORM_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateWorm( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE * Player = &AppState->Player;
    WORM_STATE * WormS = &AppState->WormS;
    
    if( WormS->SpawnLarge_IsActive ) {
        WormS->SpawnLarge_Cooldown_Timer = MaxValue( WormS->SpawnLarge_Cooldown_Timer - dT, 0.0f );
        
        if( WormS->nWorm > 0 ) {
            WORM Worm = WormS->Worm[ 0 ];
            Assert( Worm.Class == WormClass_Large );
            
            if( Worm.Health <= 75 ) {
                WormS->SpawnLarge_Cooldown_Timer = 0.0f;
            }
        }
        
        if( WormS->SpawnLarge_Cooldown_Timer <= 0.0f ) {
            WormS->SpawnLarge_Cooldown_Timer = 60.0f;
            
            flo32 Offset = 10.0f;
            vec2 Dir[ 2 ] = {};
            vec2 Pos[ 2 ] = {};
            
            Dir[ 0 ] = Vec2( 1.0f, 0.0f );
            Pos[ 0 ] = GetLC( AppState->World_Bound ) + Dir[ 0 ] * Offset;
            
            Dir[ 1 ] = Vec2( -1.0f, 0.0f );
            Pos[ 1 ] = GetRC( AppState->World_Bound ) + Dir[ 1 ] * Offset;
            
            vec2 Dir0 = Dir[ WormS->SpawnLarge_nWormsSpawned ];
            vec2 Pos0 = Pos[ WormS->SpawnLarge_nWormsSpawned ];
            
            ENTITY Entity = {};
            Entity.WaveGuid = -1;
            Entity.Type     = EntityType_Worm;
            Entity.nEntity  = 1;
            Entity.Position = Pos0;
            Entity.Radius   = WORM_RADIUS_LARGE;
            Entity.Shield.bRadians  = Dir0.x;
            Entity.Shield.Radians   = Dir0.y;
            AddEntitySpawn( AppState, Entity );
            
            WormS->SpawnLarge_nWormsSpawned += 1;
            if( WormS->SpawnLarge_nWormsSpawned >= 2 ) {
                WormS->SpawnLarge_IsActive = false;
            }
        }
    }
    
    if( WormS->Spawn_IsActive ) {
        int32 nSpawn = 0;
        
        for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
            WORM Worm = WormS->Worm[ iWorm ];
            if( Worm.Class == WormClass_Small ) {
                nSpawn += 1;
            }
        }
        
        if( nSpawn < WormS->Spawn_MaxWorm ) {
            if( nSpawn <= 2 ) {
                WormS->Spawn_Cooldown_Timer = MinValue( WormS->Spawn_Cooldown_Timer, WormS->Spawn_Cooldown_TargetTime * 0.5f );
            }
            
            WormS->Spawn_Cooldown_Timer = MaxValue( WormS->Spawn_Cooldown_Timer - dT, 0.0f );
            
            if( WormS->Spawn_Cooldown_Timer <= 0.0f ) {
                WormS->Spawn_Cooldown_Timer = WormS->Spawn_Cooldown_TargetTime;
                
                rect Bound = AddRadius( AppState->World_Bound, -6.0f );
                vec2 Pos   = GetRandomPoint( Bound );
                vec2 Dir   = RandomDirection2D();
                
                boo32 IsValid = true;
                for( int32 iWorm = 0; ( IsValid ) && ( iWorm < WormS->nWorm ); iWorm++ ) {
                    WORM Worm = WormS->Worm[ iWorm ];
                    if( Worm.Class == WormClass_Large ) {
                        flo32 Radius = 12.0f;
                        vec2 bPosA = Worm.bPos + Worm.Body_Dir * Worm.Spawn_Length;
                        vec2 bPosB = Worm.bPos + Worm.Body_Dir * Worm.Spawn_MaxLength;
                        
                        if( ( IsInCircle( Pos, Circ( bPosA, Radius ) ) ) || ( IsInCircle( Pos, Circ( bPosB, Radius ) ) ) ) {
                            IsValid = false;
                        }
                    }
                }
                
                if( IsValid ) {
                    vec2 Dest  = Pos - Dir * 4.0f;
                    if( ( Dest.x < Bound.Left ) || ( Dest.x > Bound.Right ) ) {
                        Dir.x = -Dir.x;
                    }
                    if( ( Dest.y < Bound.Bottom ) || ( Dest.y > Bound.Top ) ) {
                        Dir.y = -Dir.y;
                    }
                    Dest = Pos - Dir * 4.0f;
                    
                    ENTITY Entity = {};
                    Entity.WaveGuid = -1;
                    Entity.Type     = EntityType_Worm;
                    Entity.nEntity  = 1;
                    Entity.Position = Dest;
                    Entity.Radius   = WORM_RADIUS_SMALL;
                    Entity.Shield.bRadians  = Dir.x;
                    Entity.Shield.Radians   = Dir.y;
                    AddEntitySpawn( AppState, Entity );
                    
                    //AddWorm( AppState, WormC_Small( -1, Dest, Dir ) );
                } else {
                    WormS->Spawn_Cooldown_Timer = 0.25f;
                }
                
                
                if( WormS->Spawn_nAdditionalSmallWormsAtStart > 0 ) {
                    WormS->Spawn_Cooldown_Timer = 0.0f;
                    WormS->Spawn_nAdditionalSmallWormsAtStart -= 1;
                }
            }
        }
    }
    
    for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
        WORM * Worm = WormS->Worm + iWorm;
        
#if 0
        {
            vec2 bPos = Worm->bPos + Worm->Body_Dir * 16.0f;
            vec2 V    = GetPerp( Worm->Body_Dir ) * WORM_MOVEMENT_X_OFFSET;
            
            vec2 A   = bPos - V;
            vec2 B   = bPos + V;
            CAPSULE2 Cap = Capsule2( A, B, WORM_MOVEMENT_Y_OFFSET + Worm->Radius );
            Worm->Cap = Cap;
        }
#endif
        
        { // Update Attack
            Worm->ShowDamage_Timer = MaxValue( Worm->ShowDamage_Timer - dT, 0.0f );
            
            if( Worm->Health > 0 ) {
                if( Worm->DoAttack ) {
                    vec2 bPos = Worm->Position + Vec2( 0.1f, -0.6f ) * Worm->Radius;
                    bPos = Worm->Position;
                    vec2 bDir = GetNormal( Player->Position - bPos );
                    
                    switch( Worm->DoAttack_iAttack ) {
                        case 0: { // Single Red Fireball
                            AddFireball( AppState, FireballC_Small( -1, FireballType_Points, bPos, bDir ) );
                            Worm->DoAttack_Counter -= 1;
                        } break;
                        
                        case 1: { // 5 Red Fireballs in Burst Pattern
                            int32 nFireball = 5;
                            flo32 bRadians  = RandomF32() * TAU;
                            flo32 Radians   = TAU / ( flo32 )nFireball;
                            for( int32 iFireball = 0; iFireball < nFireball; iFireball++ ) {
                                flo32 Rad = bRadians + Radians * ( flo32 )iFireball;
                                vec2  Dir = ToDirection2D( Rad );
                                vec2  Pos = bPos + Dir * 0.5f;
                                
                                AddFireball( AppState, FireballC_Small( -1, FireballType_Points, Pos, Dir ) );
                            }
                            Worm->DoAttack_Counter -= 1;
                        } break;
                        
                        case 2: { // 5 Green Fireballs in Wave Pattern
                            int32 nFireball  = 5;
                            flo32 MaxRadians = PI / 3.0f;
                            flo32 Radians    = MaxRadians / ( flo32 )nFireball;
                            flo32 bRadians   = ToRadians( bDir ) - Radians * ( ( flo32 )( nFireball - 1 ) * 0.5f );
                            
                            for( int32 iFireball = 0; iFireball < nFireball; iFireball++ ) {
                                flo32 Rad = bRadians + Radians * ( flo32 )iFireball;
                                vec2  Dir = ToDirection2D( Rad );
                                vec2  Pos = bPos + Dir * 0.5f;
                                
                                AddFireball( AppState, FireballC_Small( -1, FireballType_Multiplier, Pos, Dir ) );
                            }
                            Worm->DoAttack_Counter -= 1;
                        } break;
                        
                        case 3: { // 1 Large Green Fireball
                            AddFireball( AppState, FireballC_Large( -1, FireballType_Multiplier, bPos, bDir ) );
                            Worm->DoAttack_Counter -= 1;
                        } break;
                        
                        case 4: { // Stream of Red Fireballs at Player
                            Worm->DoAttack_TargetTime = ( 6.0f / 60.0f );
                            Worm->DoAttack_Timer += dT;
                            if( Worm->DoAttack_Timer >= Worm->DoAttack_TargetTime ) {
                                Worm->DoAttack_Timer = 0.0f;
                                
                                Worm->DoAttack_Counter -= 1;
                                AddFireball( AppState, FireballC_Small( -1, FireballType_Points, bPos, bDir ) );
                            }
                        } break;
                        
                        case 5: { // Stream of Red Fireballs in Spiral Pattern
                            Worm->DoAttack_TargetTime = ( 6.0f / 60.0f );
                            Worm->DoAttack_Timer += dT;
                            if( Worm->DoAttack_Timer >= Worm->DoAttack_TargetTime ) {
                                Worm->DoAttack_Timer = 0.0f;
                                
                                flo32 t       = Clamp01( ( flo32 )Worm->DoAttack_Counter / 20.0f );
                                flo32 Radians = ( TAU * 1.25f ) * t;
                                vec2  Dir     = ToDirection2D( Radians );
                                
                                Worm->DoAttack_Counter -= 1;
                                AddFireball( AppState, FireballC_Small( -1, FireballType_Points, bPos, Dir ) );
                            }
                        } break;
                    }
                    
                    if( Worm->DoAttack_Counter <= 0 ) {
                        Worm_InitAttack( Worm );
                    }
                } else {
                    Worm->DoAttack_Timer += dT;
                    if( ( Worm->DoAttack_iAttack < 0 ) &&  ( Worm->DoAttack_Timer >= ( Worm->DoAttack_TargetTime - WORM_ATTACK_WARMUP_TARGET_TIME ) ) ) {
                        
                        int32 Select = RandomS32InRange( 1, 5 );
                        
                        if( Worm->Class == WormClass_Small ) {
                            Select = 0;
                        }
                        
                        Worm->DoAttack_iAttack = Select;
                        if( Select >= 4 ) {
                            Worm->DoAttack_Counter = 20;
                        } else {
                            Worm->DoAttack_Counter = 1;
                        }
                    }
                    if( Worm->DoAttack_Timer >= Worm->DoAttack_TargetTime ) {
                        //Worm->DoAttack_Timer = 0.0f;
                        Worm->DoAttack = true;
                    }
                }
            }
        }
        
        { // Update Spawn
            flo32 Speed    = 0.0f;
            flo32 Friction = 3.0f;
            if( Worm->Spawn_Length < Worm->Spawn_MaxLength ) {
                Speed    = 12.0f;
                Friction = 4.0f;
                
                if( Worm->Class == WormClass_Small ) {
                    Speed    = 6.0f;
                    Friction = 4.0f;
                }
            }
            
            flo32 Accel   = ( Speed - Worm->Spawn_Velocity ) * Friction;
            flo32 dLength = ( Accel * 0.5f * dT * dT ) + Worm->Spawn_Velocity * dT;
            
            Worm->Spawn_Length   += dLength;
            Worm->Spawn_Velocity += Accel * dT;
        }
        
        { // Update Movement
            flo32 Speed    = 1.0f / 4.0f;
            
            if( Worm->Class == WormClass_Small ) {
                Speed = 1.0f / 5.0f;
            }
            
            if( Worm->Health < 0 ) {
                Speed = 0.0f;
            }
            
            flo32 Friction = WORM_FRICTION;
            flo32 ddMove   = ( Speed - Worm->dMove ) * Friction;
            
            Worm->tMove += ddMove * ( 0.5f * dT * dT ) + Worm->dMove * dT;
            Worm->dMove += ddMove * dT;
            
            if( Worm->tMove >= 1.0f ) {
                Worm->tMove -= 1.0f;
            }
            
            flo32 Radians = Worm->tMove * TAU;
            flo32 xOffset = WORM_MOVEMENT_X_OFFSET * sinf( Radians );
            flo32 yOffset = WORM_MOVEMENT_Y_OFFSET * cosf( Radians * 2.0f + PI * 0.5f );
            
            flo32 tSpawn = Clamp01( Worm->Spawn_Length / Worm->Spawn_MaxLength );
            yOffset *= tSpawn;
            
            if( Worm->Class == WormClass_Small ) {
                xOffset *= WORM_MOVEMENT_RATIO_FOR_SMALL_WORM;
                yOffset *= WORM_MOVEMENT_RATIO_FOR_SMALL_WORM;
            }
            
            // NOTE: yAxis is parallel to the (general) direction of the worm's body
            // NOTE: xAxis is perpendicular
            
            vec2  yAxis = Worm->Body_Dir;
            vec2  xAxis = -GetPerp( yAxis );
            
            //flo32 tSpawn = Clamp01( Worm->Spawn_Length / Worm->Spawn_MaxLength );
            //flo32 tSpeed = lerp( 10.0f, tSpawn, 0.0f );
            //flo32 dLength = MinValue( tSpeed * dT, ( Worm->Spawn_MaxLength - Worm->Spawn_Length ) * 0.5f );
            //Worm->Spawn_Length = MinValue( Worm->Spawn_Length + dLength, Worm->Spawn_MaxLength );
            vec2  bPos  = Worm->bPos + yAxis * Worm->Spawn_Length;
            
            vec2 PrevPos = Worm->Position;
            Worm->Position = bPos + xAxis * xOffset + yAxis * yOffset;
            
            {
                Worm->Tag_PosA  = Worm->Position + GetNormal( Worm->Body_Pos[ 0 ] - Worm->Position ) * ( Worm->Radius - 0.2f );
                Worm->Tag_PosB += ( -Worm->Body_Dir * 2.0f ) * dT;
                Worm->Tag_PosB  = Worm->Tag_PosA + GetNormal( Worm->Tag_PosB - Worm->Tag_PosA ) * 1.5f;
            }
        }
        
        { // Update Body
            vec2  PrevPos = Worm->Position;
            flo32 tSpawn  = Clamp01( Worm->Spawn_Length / Worm->Spawn_MaxLength );
            flo32 Speed   = lerp( 0.0f, tSpawn, 6.0f );
            
            for( int32 iBody = 0; iBody < Worm->nBody; iBody++ ) {
                vec2  Vel = ( -Worm->Body_Dir * Speed );
                
                if( Worm->Class == WormClass_Small ) {
                    vec2 bDir = GetNormal( Worm->bPos - Worm->Body_Pos[ iBody ] );
                    Vel += ( bDir * ( 1.5f * ( flo32 )iBody ) );
                }
                
                //vec2   Pos = Worm->Body_Pos[ iBody ] + Vel * dT;
                //flo32 tPos = dot( Pos - Worm->Position, -Worm->Body_Dir );
                
                //if( tPos < Worm->Spawn_Length ) {
                //Worm->Body_Pos[ iBody ] = Pos;
                //}
                
                Worm->Body_Pos[ iBody ] += Vel * dT;
            }
            
            flo32 Ratio = ( 4.0f / 3.5f );
            if( Worm->Class == WormClass_Small ) {
                Ratio = 1.0f;
            }
            for( int32 iBody = 0; iBody < 4; iBody++ ) {
                flo32 Radius   = Worm->Radius * Ratio;
                flo32 RadiusSq = Radius * Radius;
                vec2  V        = Worm->Body_Pos[ iBody ] - PrevPos;
                flo32 DistSq   = GetLengthSq( V );
                if( DistSq > RadiusSq ) {
                    Worm->Body_Pos[ iBody ] = PrevPos + GetNormal( V ) * Radius;
                    
                    PrevPos = Worm->Body_Pos[ iBody ];
                }
            }
        }
        
        { // Kill Small Worm if Collide with Large Worm
            if( ( Worm->Health > 0 ) && ( Worm->Class == WormClass_Small ) ) {
                for( int32 iWormA = 0; iWormA < WormS->nWorm; iWormA++ ) {
                    WORM WormA = WormS->Worm[ iWormA ];
                    if( ( WormA.Health > 0 ) && ( WormA.Class == WormClass_Large ) ) {
                        flo32 Radius   = Worm->Radius + WormA.Radius;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Worm->Position - WormA.Position );
                        if( DistSq <= RadiusSq ) {
                            Worm->Health = 0;
                        }
                    }
                }
            }
        }
        
        { // Update Worm Death
            if( Worm->Health <= 0 ) {
                if( ( Worm->Death_iBody <= 0 ) && ( Worm->Death_Timer <= 0.0f ) ) {
                    if( Worm->CanSpawnScoreMultipliers ) {
                        int32 nScoreMultiplier = 3;
                        for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                            SCORE_MULTIPLIER ScoreMultiplier = NewScoreMultiplier( Worm->Position, AppState->World_Bound, false );
                            ScoreMultiplier.Velocity *= 5.0f;
                            
                            AddScoreMultiplier( AppState, ScoreMultiplier );
                        }
                    }
                    
                    int32 nIter = ( Worm->Class == WormClass_Small ) ? 1 : 2;
                    for( int32 iIter = 0; iIter < nIter; iIter++ ) {
                        AddBigEntityDeathParticles( AppState, Worm->Position, Worm->Radius * 1.5f, COLOR_RED );
                        AddBigEntityDeathParticles( AppState, Worm->Position, Worm->Radius * 0.8f, WORM_BASE_COLOR );
                    }
                    
                    if( Worm->CanSpawnPoints ) {
                        switch( Worm->Class ) {
                            case WormClass_Small: {
                                AddKillToScore( AppState, EntityType_Worm, Worm->Position, WORM_POINTS_SMALL );
                            } break;
                            
                            case WormClass_Large: {
                                //AddKillToScore( AppState, EntityType_Worm, Worm->Position, WORM_POINTS_LARGE );
                            } break;
                        }
                    }
                }
                
                if( Worm->Death_iBody < Worm->nBody ) {
                    Worm->Death_Timer += dT;
                    if( Worm->Death_Timer >= WORM_DEATH_CYCLE_TARGET_TIME ) {
                        Worm->Death_Timer = 0.0f;
                        
                        vec2 Pos = Worm->Body_Pos[ Worm->Death_iBody ];
                        
                        if( Worm->CanSpawnScoreMultipliers ) {
                            int32 nScoreMultiplier = 2;
                            for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                                SCORE_MULTIPLIER ScoreMultiplier = NewScoreMultiplier( Pos, AppState->World_Bound, false );
                                ScoreMultiplier.Velocity *= 5.0f;
                                
                                AddScoreMultiplier( AppState, ScoreMultiplier );
                            }
                        }
                        
                        if( Worm->Class == WormClass_Large ) {
                            AddEntityDeathParticles( AppState, Pos, WORM_BASE_COLOR );
                        }
                        
                        AddEntityDeathParticles( AppState, Pos, WORM_BASE_COLOR );
                        AddEntityDeathParticles( AppState, Pos, COLOR_RED );
                        
                        Worm->Death_iBody += 1;
                    }
                }
            }
        }
    }
}

internal void
DrawWormBody( RENDER_PASS * Pass, WORM Worm, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 tColor = Clamp01( Worm.Spawn_Length / Worm.Spawn_MaxLength );
    tColor = ( tColor * 0.95f + 0.05f );
    
    flo32 tDamage = Clamp01( Worm.ShowDamage_Timer / WORM_SHOW_DAMAGE_TARGET_TIME );
    
    {
        vec4 Color[ 4 ] = {
            ToColor( 20, 20, 20 ),
            ToColor( 15, 15, 15 ),
            ToColor( 10, 10, 10 ),
            ToColor(  7,  7,  7 ),
        };
        
        for( int32 iBody = Worm.nBody - 1; iBody >= Worm.Death_iBody; iBody-- ) {
            flo32 tBody = 1.0f - Clamp01( ( flo32 )iBody / ( flo32 )Worm.nBody );
            
            vec4 ColorA = Color[ iBody ];
            ColorA.rgb *= tColor;
            
            ColorA = TowardWhite( ColorA, tDamage * 0.15f * tBody );
            
            //DrawCircle( Pass, Worm.Body_Pos[ iBody ], Worm.Radius * ( 0.95f - 0.05f * ( flo32 )iBody ), ColorA );
            
            vec2  Pos    = Worm.Body_Pos[ iBody ];
            flo32 Radius = Worm.Radius * ( 0.95f - 0.05f * ( flo32 )iBody );
            rect  Bound  = RectCR( Pos, Radius );
            
            DrawRect( Pass, TextureID_Worm_Body, Bound, ColorA );
        }
    }
}

internal void
DrawWormHead( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, WORM Worm, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 tColor = Clamp01( Worm.Spawn_Length / Worm.Spawn_MaxLength );
    tColor = ( tColor * 0.75f + 0.25f );
    
    if( ( Alpha < 1.0f ) || ( Worm.Health > 0 ) ) {
        flo32 tDamage = Clamp01( Worm.ShowDamage_Timer / WORM_SHOW_DAMAGE_TARGET_TIME );
        
        vec4 ColorA = WORM_BASE_COLOR;
        vec4 Color  = lerp( ColorA, tDamage, TowardWhite( WORM_BASE_COLOR, 0.25f ) );
        Color.rgb *= tColor;
        
        flo32 tGrayB  = 0.05f * tColor;
        
        //DrawCircle( Pass, Worm.Position, Worm.Radius, Color );
        
        rect Bound = RectCR( Worm.Position, Worm.Radius * Scale );
        DrawRect( Pass, TextureID_Worm_Body, Bound, Color * Alpha );
        
        {
            flo32 TargetTime = Worm.DoAttack_TargetTime - WORM_ATTACK_WARMUP_TARGET_TIME;
            
            if( Worm.DoAttack_iAttack > -1 ) {
                flo32 Radius  = Worm.Radius * 0.4f * Scale;
                flo32 tAttack = Clamp01( ( Worm.DoAttack_Timer - TargetTime ) / ( Worm.DoAttack_TargetTime - TargetTime ) );
                
                vec4 bColor = FIREBALL_POINTS_BASE_COLOR;
                if( ( Worm.DoAttack_iAttack == 2 ) || ( Worm.DoAttack_iAttack == 3 ) ) {
                    bColor = FIREBALL_MULTIPLIER_BASE_COLOR;
                }
                
                vec4 ColorC = lerp( COLOR_GRAY( tGrayB ), lerp( 0.35f, tAttack, 0.5f ), TowardBlack( bColor, 0.2f ) );
                vec4 ColorD = bColor * Clamp01( tAttack - 0.25f );
                //DrawCircle( Pass, Worm.Position, Radius, COLOR_GRAY( tGrayB ) );
                
                DrawRect( Pass, TextureID_Collector_Flash, RectCR( Worm.Position, Radius        ), ColorC * Alpha );
                DrawRect( Pass, TextureID_Glow,            RectCR( Worm.Position, Radius * 0.8f ), ColorD * Alpha );
                
                int32 nTeeth = 16;
                for( int32 iTeeth = 0; iTeeth < nTeeth; iTeeth++ ) {
                    flo32 t   = Clamp01( ( flo32 )iTeeth / ( flo32 )nTeeth );
                    vec2  Dir = ToDirection2D( t * TAU );
                    vec2  Pos = Worm.Position + Dir * Radius;
                    vec2  Sca = Vec2( TAU / ( flo32 )nTeeth, 0.4f ) * ( Radius * 0.5f );
                    
                    DrawTriangleBC( Pass, Pos, Sca, t * TAU + PI * 0.5f, COLOR_GRAY( 0.15f ) * Alpha );
                }
            } else {
                DrawRect( Pass, TextureID_Collector_Flash, RectCR( Worm.Position, Worm.Radius * 0.05f * Scale ), COLOR_GRAY( tGrayB ) * Alpha );
            }
        }
    }
    
    if( Worm.Health > 0 ) {
        vec2 PosA = Worm.Tag_PosA;
        vec2 PosB = Worm.Tag_PosB;
        
        vec4 Tag_Color      = lerp( COLOR_BLACK, tColor, COLOR_GRAY( 0.1f  ) );
        vec4 TagChain_Color = lerp( COLOR_BLACK, tColor, COLOR_GRAY( 0.05f ) );
        
        orect Bound = ORectTD( PosA, Vec2( 0.0f, 0.5f ), Vec2( GetLength( PosB - PosA ), 0.15f ), ToRadians( PosB - PosA ) );
        
        DrawORect( Pass, TextureID_Worm_TagChain, Bound, TagChain_Color );
        DrawRect ( Pass, TextureID_Worm_Tag, RectCD( PosB, WORM_TAG_DIM ), Tag_Color );
        
        DrawEntityHealth( Pass, AppState, Camera, PosB, Worm.Health, COLOR_WHITE * tColor );
    }
}

internal void
DrawWorm( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    WORM_STATE * WormS = Draw->WormS;
    
    for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
        WORM Worm = WormS->Worm[ iWorm ];
        DrawWormBody( Pass, Worm );
    }
    
    for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
        WORM Worm = WormS->Worm[ iWorm ];
        DrawWormHead( Pass, AppState, Camera, Worm );
    }
}

internal void
FinalizeWorm( APP_STATE * AppState ) {
    WORM_STATE  * WormS  = &AppState->WormS;
    
    { // WORM
        uint32 NewCount = 0;
        for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
            WORM Worm = WormS->Worm[ iWorm ];
            
            boo32 DoKeep = ( Worm.Death_iBody < Worm.nBody );
            
            if( DoKeep ) {
                WormS->Worm[ NewCount++ ] = Worm;
            } else {
                AddKillToWave( AppState, Worm.WaveGuid );
                
                WormS->BonusObjective_nWormKills += 1;
                if( Worm.Class == WormClass_Large ) {
                    WormS->BonusObjective_nLargeWormKills += 1;
                }
            }
        }
        WormS->nWorm = NewCount;
    }
    
    DISPLAY_VALUE( int32, WormS->BonusObjective_nWormKills );
    DISPLAY_VALUE( int32, WormS->BonusObjective_nLargeWormKills );
}