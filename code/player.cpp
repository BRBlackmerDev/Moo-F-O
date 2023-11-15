

internal boo32
IsInRoundedORect( vec2 P, orect Bound, flo32 Radius ) {
    boo32 Result = IsInORect( P, Bound );
    if( !Result ) {
        vec2  Q        = ClosestPointToORectPerimeter( P, Bound );
        flo32 DistSq   = GetLengthSq( Q - P );
        flo32 RadiusSq = Radius * Radius;
        
        Result = ( DistSq <= RadiusSq );
    }
    return Result;
}

internal void
UpdatePlayerInput( APP_STATE * AppState, MOUSE_STATE * Mouse, flo32 dT ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->dPos = {};
    
    MOUSE_BUTTON_ID ID_Fire   = MouseButton_Left;
    MOUSE_BUTTON_ID ID_Shield = MouseButton_Right;
    
    if( Player->IsDead ) {
        
    } else {
        boo32 Fire_IsActive = false;
        if( IsDown( Mouse, ID_Fire ) ) {
            Fire_IsActive = true;
        }
        
        Player->Rotate_Speed            = PLAYER_ROTATE_SPEED_SLOW;
        Player->Fire_CooldownTargetTime = PLAYER_FIRE_COOLDOWN_TARGET_TIME_SLOW;
        if( Fire_IsActive ) {
            Player->Rotate_Speed            = PLAYER_ROTATE_SPEED_FAST;
            Player->Fire_CooldownTargetTime = PLAYER_FIRE_COOLDOWN_TARGET_TIME_FAST;
            
            Player->ZeroPoint_Fast_CooldownTimer = ( 4.0f / 60.0f );
        } else {
            Player->ZeroPoint_Fast_CooldownTimer = MaxValue( Player->ZeroPoint_Fast_CooldownTimer - dT, 0.0f );
        }
        
        if( Player->Powerup_Timer_PlayerBullets > 0.0f ) {
            Assert( Player->Powerup_Rate_PlayerBullets >= 1.0f );
            
            flo32 Rotate_Rate = ( ( Player->Powerup_Rate_PlayerBullets - 1.0f ) * 0.5f ) + 1.0f;
            Player->Rotate_Speed            *= Rotate_Rate;
            Player->Fire_CooldownTargetTime /= Player->Powerup_Rate_PlayerBullets;
        }
        
        { // Update Zero Point
            boo32 DoPressed  = WasPressed ( Mouse, ID_Shield );
            boo32 DoReleased = WasReleased( Mouse, ID_Shield );
            
            if( Player->ZeroPoint_IsSecurityDrone ) {
                // it should take the same time to get to full speed whether fast or slow
                // if fast is pressed (slow -> fast), then timer is set part way
                // if fast is released(fast -> slow), then timer is over full
                
                if( WasPressed( Mouse, ID_Fire ) ) {
                    Player->ZeroPoint_IsSecurityDrone_tSpeed *= ( PLAYER_ROTATE_SPEED_SLOW / PLAYER_ROTATE_SPEED_FAST );
                }
                if( WasReleased( Mouse, ID_Fire ) ) {
                    Player->ZeroPoint_IsSecurityDrone_tSpeed *= ( PLAYER_ROTATE_SPEED_FAST / PLAYER_ROTATE_SPEED_SLOW );
                }
                
                flo32 dSpeed = ( 1.0f / SECURITY_DRONE_ZERO_POINT_TARGET_TIME_TO_FULL_SPEED ) * dT;
                flo32 Diff   = 1.0f - Player->ZeroPoint_IsSecurityDrone_tSpeed;
                if( Diff < -dSpeed ) {
                    Player->ZeroPoint_IsSecurityDrone_tSpeed -= dSpeed;
                } else if( Diff > dSpeed ) {
                    Player->ZeroPoint_IsSecurityDrone_tSpeed += dSpeed;
                } else {
                    Player->ZeroPoint_IsSecurityDrone_tSpeed  = 1.0f;
                }
                
                Player->Rotate_Speed *= ( Player->ZeroPoint_IsSecurityDrone_tSpeed * SECURITY_DRONE_ZERO_POINT_ADJUST_SPEED_RATIO );
            }
            
            UpdateZeroPoint( AppState, DoPressed, DoReleased, dT );
        }
        
        if( Player->Collector_IsEnabled ) {
            if( WasPressed( Mouse, ID_Shield ) ) {
                Player->Collector_IsActive = false;
            }
            if( WasReleased( Mouse, ID_Shield ) ) {
                Player->Collector_IsActive = true;
            }
            
            flo32 dRadians = TAU * dT;
            for( int32 iCollector = 0; iCollector < SpitType_Count; iCollector++ ) {
                Player->Collector_Flash_Timer[ iCollector ] = MaxValue( Player->Collector_Flash_Timer[ iCollector ] - dT, 0.0f );
                Player->Collector_Radians[ iCollector ] = WrapRadians( Player->Collector_Radians[ iCollector ] - dRadians );
            }
        }
        
        if( Player->GravityWell_IsEnabled ) {
            if( WasPressed( Mouse, ID_Shield ) ) {
                Player->GravityWell_IsActive = true;
            }
            if( WasReleased( Mouse, ID_Shield ) ) {
                Player->GravityWell_IsActive = false;
            }
        }
        
        if( Player->Shield_IsEnabled ) {
            if( WasPressed( Mouse, ID_Shield ) ) {
                Player->Shield_IsActive = true;
            }
            if( WasReleased( Mouse, ID_Shield ) ) {
                Player->Shield_IsActive = false;
            }
        }
        
        if( Player->TractorBeam_IsEnabled ) {
            if( WasPressed( Mouse, ID_Shield ) ) {
                Player->TractorBeam_IsActive = true;
            }
            if( WasReleased( Mouse, ID_Shield ) ) {
                Player->TractorBeam_IsActive = false;
            }
            
            flo32 MinRadius = TRACTOR_BEAM_RING_MIN_RADIUS;
            flo32 MaxRadius = TRACTOR_BEAM_RING_MAX_RADIUS;
            flo32 dRadius   = ( MinRadius - MaxRadius ) / 1.25f;
            Player->TractorBeam_dRadius = lerp( dRadius * 0.5f, Player->TractorBeam_MaxAlpha, dRadius );
            
            for( int32 iRing = 0; iRing < TRACTOR_BEAM_RING_MAX_COUNT; iRing++ ) {
                TRACTOR_BEAM_RING * Ring = Player->TractorBeam_Ring + iRing;
                Ring->Radius += Player->TractorBeam_dRadius * dT;
                
                if( Ring->Radius <= MinRadius ) {
                    Ring->Radius = MaxRadius;
                    
                    if( Player->TractorBeam_IsActive ) {
                        Ring->DoShow = true;
                    }
                }
            }
            
            if( Player->TractorBeam_MaxAlpha <= 0.0f ) {
                Player->TractorBeam_Ring[ 0 ].Radius = lerp( MinRadius, 1.0f / 3.0f, MaxRadius );
                Player->TractorBeam_Ring[ 1 ].Radius = lerp( MinRadius, 2.0f / 3.0f, MaxRadius );
                Player->TractorBeam_Ring[ 2 ].Radius = lerp( MinRadius, 3.0f / 3.0f, MaxRadius );
                Player->TractorBeam_Ring[ 0 ].DoShow = false;
                Player->TractorBeam_Ring[ 1 ].DoShow = false;
                Player->TractorBeam_Ring[ 2 ].DoShow = true;
            }
            
            if( Player->TractorBeam_IsActive ) {
                flo32 dAlpha = ( 1.0f / 0.25f );
                Player->TractorBeam_MaxAlpha = MinValue( Player->TractorBeam_MaxAlpha + dAlpha * dT, 1.0f );
                
                //Player->dRadius
                //flo32 Rate = TAU / 1.5f;
                //Player->TractorBeam_AnimRadians = WrapRadians( Player->TractorBeam_AnimRadians + Rate * dT );
            } else {
                flo32 dAlpha = ( 1.0f / 0.5f );
                Player->TractorBeam_MaxAlpha = MaxValue( Player->TractorBeam_MaxAlpha - dAlpha * dT, 0.0f );
            }
        }
        
        vec2 World_HalfDim = GetDim( AppState->World_Bound ) * 0.5f;
        
        Player->dPos  = {};
        if( ( Player->DoDamage_Timer <= 0.0f ) && ( Player->DoBumper_Timer <= 0.0f ) ) {
            Player->dPos  = ( Mouse->dPos / AppState->UI_HalfDim ) * World_HalfDim;
        }
        Player->dPos += Player->DoDamage_Velocity * dT;
        Player->DoDamage_Velocity -= Player->DoDamage_Velocity * ( PLAYER_DAMAGE_BOOST_FRICTION * dT );
        Player->dPos += Player->DoBumper_Velocity * dT;
        Player->DoBumper_Velocity -= Player->DoBumper_Velocity * ( PLAYER_DAMAGE_BOOST_FRICTION * dT );
        
        // TODO: Collision!!!
        Player->Position += Player->dPos;
        Player->Position  = ClampToRect( Player->Position, AppState->World_Bound );
        
        { // SATELLITE COLLISION
            SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
            if( ( SatelliteS->nSatellite > 0 ) && ( SatelliteS->ShieldDown_Timer <= 0.0f ) ) {
                rect Bound = AppState->World_Bound;
                Bound.Right = SatelliteS->Shield_Bound.Left - ( PLAYER_RADIUS - 0.2f );
                Player->Position = ClampToRect( Player->Position, Bound );
            }
        }
        
        { // OBSERVATION_DECK COLLISION
            BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
            OBSERVATION_DECK   ObservationDeck = BoardRoomS->ObservationDeck;
            
            if( ( BoardRoomS->State_IsActive ) && ( ObservationDeck.IsPresent ) ) {
                flo32 Radius = ObservationDeck.Radius + ( PLAYER_RADIUS - 0.2f );
                if( ObservationDeck.Shield_IsActive ) {
                    Radius += ObservationDeck.Shield_Dist;
                }
                
                circ C = Circ( ObservationDeck.Position, Radius );
                if( IsInCircle( Player->Position, C ) ) {
                    vec2 Norm = GetNormal( Player->Position - C.Center );
                    Player->Position = C.Center + Norm * C.Radius;
                }
                
                rect Bound = AppState->World_Bound;
                Player->Position = ClampToRect( Player->Position, Bound );
            }
        }
    }
    
    DISPLAY_VALUE( vec2,  Player->Position );
    //DISPLAY_VALUE( vec2,  Player->dPos     );
}

internal void
DamagePlayer( APP_STATE * AppState, uint32 Damage, ENTITY_TYPE Type, uint8 * EntityData, uint32 EntityData_Size ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    uint32 Dmg = MinValue( Player->Health, Damage );
    Player->Health -= Dmg;
    
    flo32 MaxTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 3.0f;
    MaxTime = PLAYER_NO_DAMAGE_TARGET_TIME - ( 4.0f / 60.0f );
    Player->ShowDamage_Timer = MaxTime - 0.0001f;
    // TODO: Why is the math like this? What was I trying to do?
    
    Player->NoDamage_Timer        = PLAYER_NO_DAMAGE_TARGET_TIME;
    Player->SecondaryDamage_Timer = 0.0f;
    
    Player->LastDamage_Timer    = PLAYER_LAST_DAMAGE_TARGET_TIME;
    Player->LastDamage_Type     = Type;
    memcpy( Player->LastDamage_Buffer, EntityData, EntityData_Size );
    
    Player->ScreenFlash_Type       = ScreenFlashType_Player_Damage;
    Player->ScreenFlash_TargetTime = SCREEN_FLASH_TARGET_TIME__PLAYER_DAMAGE;
    Player->ScreenFlash_Timer      = Player->ScreenFlash_TargetTime;
    
    PlaySoundEffect( AppState, AudioID_Player_Damage, 2, PLAYER_DAMAGE_VOLUME, 1.0f );
}

internal void
SecondaryDamageToPlayer( APP_STATE * AppState, ENTITY_TYPE Type, uint8 * EntityData, uint32 EntityData_Size ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    Player->ShowDamage_Timer      = PLAYER_NO_DAMAGE_SECONDARY_TARGET_TIME - ( 4.0f / 60.0f );
    Player->NoDamage_Timer        = PLAYER_NO_DAMAGE_SECONDARY_TARGET_TIME;
    Player->SecondaryDamage_Timer = PLAYER_SECONDARY_DAMAGE_TARGET_TIME;
    
    Player->LastDamage_Timer    = PLAYER_LAST_DAMAGE_TARGET_TIME;
    Player->LastDamage_Type     = Type;
    memcpy( Player->LastDamage_Buffer, EntityData, EntityData_Size );
    
    Player->ScreenFlash_Type       = ScreenFlashType_Player_SecondaryDamage;
    Player->ScreenFlash_TargetTime = SCREEN_FLASH_TARGET_TIME__PLAYER_SECONDARY_DAMAGE;
    Player->ScreenFlash_Timer      = Player->ScreenFlash_TargetTime;
    
    PlaySoundEffect( AppState, AudioID_Player_SecondaryDamage, 2, PLAYER_SECONDARY_DAMAGE_VOLUME, 1.0f );
}

internal void
CheckIfDamagePlayer( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    // TODO: Currently, this picks the first checked enemy that damages the player. Should it be changed to find the enemy that actually first contacts the player? This probably won't be an issue. I can't imagine that many enemies contacting the player in a single frame.
    
    boo32 CanDamagePlayer = ( Player->NoDamage_Timer <= 0.0f );
    
    boo32       DoKillEntity         = false;
    ENTITY_TYPE DoKillEntity_Type    = {};
    uint32      DoKillEntity_iEntity = 0;
    
    if( CanDamagePlayer ) {
        uint32 PrevHealth = Player->Health;
        
        { // ENEMY BULLET
            ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
            
            for( int32 iEnemyBullet = 0; ( CanDamagePlayer ) && ( iEnemyBullet < EnemyBulletS->nEnemyBullet ); iEnemyBullet++ ) {
                ENEMY_BULLET EnemyBullet = EnemyBulletS->EnemyBullet[ iEnemyBullet ];
                
                flo32 Radius   = ( PLAYER_RADIUS + ENEMY_BULLET_RADIUS - 0.25f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - EnemyBullet.Position );
                
                if( DistSq <= RadiusSq ) {
                    DamagePlayer( AppState, 1, EntityType_EnemyBullet, ( uint8 * )&EnemyBullet, sizeof( ENEMY_BULLET ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - EnemyBullet.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
        { // TURRET
            TURRET_STATE * TurretS = &AppState->TurretS;
            
            for( int32 iMegaTurret = 0; ( CanDamagePlayer ) && ( iMegaTurret < TurretS->nMegaTurret ); iMegaTurret++ ) {
                TURRET MegaTurret = TurretS->MegaTurret[ iMegaTurret ];
                
                flo32 TargetTime = ( MEGA_TURRET_EXIT_TARGET_TIME ) + 1.0f;
                if( MegaTurret.Exit_Timer < TargetTime ) {
                    {
                        flo32 Radius   = ( PLAYER_RADIUS + MegaTurret.Radius - 0.1f );
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Player->Position - MegaTurret.Position );
                        
                        if( DistSq <= RadiusSq ) {
                            DamagePlayer( AppState, 1, EntityType_MegaTurret, ( uint8 * )&MegaTurret, sizeof( TURRET ) );
                            
                            Player->DoDamage_Velocity = GetNormal( Player->Position - MegaTurret.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                            Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                            
                            CanDamagePlayer = false;
                        }
                    }
                    
                    ENTITY_SHIELD Shield = MegaTurret.Shield;
                    if( ( CanDamagePlayer ) && ( Shield.HasShield ) ) {
                        { // Shield Arc
                            flo32 Radius_Inner = MegaTurret.Radius + Shield.Dist - PLAYER_RADIUS + 0.1f;
                            Radius_Inner = MaxValue( Radius_Inner, 0.0f );
                            
                            flo32 Radius_Outer = MegaTurret.Radius + Shield.Dist + Shield.Width + PLAYER_RADIUS - 0.1f;
                            
                            flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
                            flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
                            flo32 DistSq         = GetLengthSq( Player->Position - MegaTurret.Position );
                            if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                                flo32 MinRadians = Shield.bRadians;
                                flo32 MaxRadians = MinRadians + Shield.Radians;
                                
                                flo32 InRadians = ToRadians( Player->Position - MegaTurret.Position );
                                if( InRadians < MinRadians ) {
                                    InRadians += TAU;
                                }
                                
                                if( ( InRadians >= MinRadians ) && ( InRadians <= MaxRadians ) ) {
                                    memset( MegaTurret.Shield_SegmentT, 0, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
                                    
                                    flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
                                    int32 iSegment   = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t );
                                    int32 MinSegment = MaxValue( iSegment - 1, 0 );
                                    
                                    for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                        int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
                                        MegaTurret.Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
                                    }
                                    
                                    DamagePlayer( AppState, 1, EntityType_MegaTurret, ( uint8 * )&MegaTurret, sizeof( TURRET ) );
                                    
                                    Player->DoDamage_Velocity = GetNormal( Player->Position - MegaTurret.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                                    
                                    CanDamagePlayer = false;
                                }
                            }
                        }
                        
                        if( CanDamagePlayer ) { // End Capsules
                            flo32 Radius = PLAYER_RADIUS - 0.1f;
                            vec2 DirA = ToDirection2D( Shield.bRadians                  );
                            vec2 DirB = ToDirection2D( Shield.bRadians + Shield.Radians );
                            
                            flo32 OffsetA = MegaTurret.Radius + Shield.Dist;
                            flo32 OffsetB = OffsetA + Shield.Width;
                            
                            CAPSULE2 CapA = Capsule2( MegaTurret.Position + DirA * OffsetA, MegaTurret.Position + DirA * OffsetB, Radius );
                            CAPSULE2 CapB = Capsule2( MegaTurret.Position + DirB * OffsetA, MegaTurret.Position + DirB * OffsetB, Radius );
                            
                            boo32 IsInCapsuleA = IsInCapsule( Player->Position, CapA );
                            boo32 IsInCapsuleB = IsInCapsule( Player->Position, CapB );
                            if( ( IsInCapsuleA ) || ( IsInCapsuleB ) ) {
                                memset( MegaTurret.Shield_SegmentT, 0, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
                                
                                int32 iSegment = 0;
                                if( IsInCapsuleB ) {
                                    flo32 t = Clamp01( Shield.Radians / TAU );
                                    iSegment = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t ) - 1;
                                }
                                
                                int32 MinSegment = MaxValue( iSegment - 1, 0 );
                                
                                for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                    int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
                                    MegaTurret.Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
                                }
                                
                                DamagePlayer( AppState, 1, EntityType_MegaTurret, ( uint8 * )&MegaTurret, sizeof( TURRET ) );
                                
                                Player->DoDamage_Velocity = GetNormal( Player->Position - MegaTurret.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                                Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                                
                                CanDamagePlayer = false;
                            }
                        }
                    }
                }
            }
            
            for( int32 iTurret = 0; ( CanDamagePlayer ) && ( iTurret < TurretS->nTurret ); iTurret++ ) {
                TURRET Turret = TurretS->Turret[ iTurret ];
                
                {
                    flo32 Radius   = ( PLAYER_RADIUS + Turret.Radius - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Turret.Position );
                    
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_Turret, ( uint8 * )&Turret, sizeof( TURRET ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Turret.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
                
                ENTITY_SHIELD Shield = Turret.Shield;
                if( ( CanDamagePlayer ) && ( Shield.HasShield ) ) {
                    boo32 DoDamage = CheckShieldCollisionWithCircle( Shield, Turret.Shield_SegmentT, Turret.Position, Turret.Radius, Player->Position, PLAYER_RADIUS, true );
                    if( DoDamage ) {
                        CanDamagePlayer = false;
                        
                        DamagePlayer( AppState, 1, EntityType_Turret, ( uint8 * )&Turret, sizeof( TURRET ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Turret.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    }
                }
            }
        }
        
        { // CHASER
            CHASER_STATE * ChaserS = &AppState->ChaserS;
            
            for( int32 iChaser = 0; ( CanDamagePlayer ) && ( iChaser < ChaserS->nChaser ); iChaser++ ) {
                CHASER Chaser = ChaserS->Chaser[ iChaser ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Chaser.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Chaser.Position );
                
                if( DistSq <= RadiusSq ) {
                    DamagePlayer( AppState, 1, EntityType_Chaser, ( uint8 * )&Chaser, sizeof( CHASER ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Chaser.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
        { // SWARM
            SWARM_STATE * SwarmS = &AppState->SwarmS;
            
            for( int32 iSwarm = 0; ( CanDamagePlayer ) && ( iSwarm < SwarmS->nSwarm ); iSwarm++ ) {
                SWARM Swarm = SwarmS->Swarm[ iSwarm ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Swarm.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Swarm.Position );
                
                if( DistSq <= RadiusSq ) {
                    if( Player->SecondaryDamage_Timer <= PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME ) {
                        SecondaryDamageToPlayer( AppState, EntityType_Swarm, ( uint8 * )&Swarm, sizeof( SWARM ) );
                        
                        DoKillEntity         = true;
                        DoKillEntity_Type    = EntityType_Swarm;
                        DoKillEntity_iEntity = iSwarm;
                    } else {
                        DamagePlayer( AppState, 1, EntityType_Swarm, ( uint8 * )&Swarm, sizeof( SWARM ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Swarm.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
        
        { // DRIFTER
            DRIFTER_STATE * DrifterS = &AppState->DrifterS;
            
            for( int32 iDrifter = 0; ( CanDamagePlayer ) && ( iDrifter < DrifterS->nDrifter ); iDrifter++ ) {
                DRIFTER Drifter = DrifterS->Drifter[ iDrifter ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Drifter.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Drifter.Position );
                
                if( DistSq <= RadiusSq ) {
                    DamagePlayer( AppState, 1, EntityType_Drifter, ( uint8 * )&Drifter, sizeof( DRIFTER ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Drifter.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
        { // LOOPER
            LOOPER_STATE * LooperS = &AppState->LooperS;
            
            for( int32 iLooper = 0; ( CanDamagePlayer ) && ( iLooper < LooperS->nLooper ); iLooper++ ) {
                LOOPER Looper = LooperS->Looper[ iLooper ];
                
                flo32 bRadians = ( 1.0f - Clamp01( Looper.Loop_Timer / LOOPER_LOOP_CYCLE_TARGET_TIME ) ) * TAU - PI;
                vec2  bPos     = Looper.Position + ToDirection2D( bRadians ) * Looper.Loop_Radius;
                rect  Looper_Bound = RectCR( bPos, LOOPER_COLLISION_RADIUS );
                
                flo32 Dist = DistPointToRect( Player->Position, Looper_Bound );
                boo32 DoDamage = ( IsInRect( Player->Position, Looper_Bound ) )
                    || ( Dist <= PLAYER_RADIUS );
                if( DoDamage ) {
                    DamagePlayer( AppState, 1, EntityType_Looper, ( uint8 * )&Looper, sizeof( LOOPER ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - bPos ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
#if 0        
        { // STARBURST
            STARBURST_STATE * StarburstS = &AppState->StarburstS;
            
            for( uint32 iStarburst = 0; ( CanDamagePlayer ) && ( iStarburst < StarburstS->nStarburst ); iStarburst++ ) {
                STARBURST Starburst = StarburstS->Starburst[ iStarburst ];
                
                orect Starburst_Bound = ORectCR( Starburst.Position, STARBURST_COLLISION_RADIUS, PI * 0.25f );
                
                flo32 Dist     = DistPointToORect( Player->Position, Starburst_Bound );
                boo32 DoDamage = ( IsInORect( Player->Position, Starburst_Bound ) )
                    || ( Dist <= PLAYER_RADIUS );
                if( DoDamage ) {
                    DamagePlayer( AppState, 1, EntityType_Starburst, ( uint8 * )&Starburst, sizeof( STARBURST ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Starburst.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
        { // STAR
            STARBURST_STATE * StarburstS = &AppState->StarburstS;
            
            for( uint32 iStar = 0; ( CanDamagePlayer ) && ( iStar < StarburstS->nStar ); iStar++ ) {
                STAR Star = StarburstS->Star[ iStar ];
                
                flo32 Radius   = ( PLAYER_RADIUS + STAR_RADIUS - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Star.Position );
                
                if( DistSq <= RadiusSq ) {
                    if( Player->SecondaryDamage_Timer <= PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME ) {
                        SecondaryDamageToPlayer( AppState, EntityType_Star, ( uint8 * )&Star, sizeof( STAR ) );
                        
                        DoKillEntity         = true;
                        DoKillEntity_Type    = EntityType_Star;
                        DoKillEntity_iEntity = iStar;
                    } else {
                        DamagePlayer( AppState, 1, EntityType_Star, ( uint8 * )&Star, sizeof( STAR ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Star.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
        
        { // MINIBOSS
            BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
            
            for( int32 iMiniBoss = 0; ( CanDamagePlayer ) && ( iMiniBoss < BayDoorS->nMiniBoss ); iMiniBoss++ ) {
                MINIBOSS * MiniBoss = BayDoorS->MiniBoss + iMiniBoss;
                
                {
                    flo32 Radius   = ( PLAYER_RADIUS + MINIBOSS_RADIUS - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - MiniBoss->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_MiniBoss, ( uint8 * )&MiniBoss, sizeof( MINIBOSS ) );
                        
                        vec2 Dir = GetNormal( Player->Position - MiniBoss->Position );
                        
                        Player->DoDamage_Velocity = Dir * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                        
                        MiniBoss->Velocity = Dir * -MINIBOSS_DAMAGE_BOOST_SPEED;
                        
                        if( MiniBoss->iTarget == -1 ) {
                            MiniBoss->iTarget_Timer += 1.5f;
                        }
                    }
                }
                
                ENTITY_SHIELD Shield = MiniBoss->Shield;
                if( ( CanDamagePlayer ) && ( Shield.HasShield ) ) {
                    { // Shield Arc
                        flo32 Radius_Inner = MINIBOSS_RADIUS + Shield.Dist - PLAYER_RADIUS + 0.1f;
                        Radius_Inner = MaxValue( Radius_Inner, 0.0f );
                        
                        flo32 Radius_Outer = MINIBOSS_RADIUS + Shield.Dist + Shield.Width + PLAYER_RADIUS - 0.1f;
                        
                        flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
                        flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
                        flo32 DistSq         = GetLengthSq( Player->Position - MiniBoss->Position );
                        if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                            flo32 MinRadians = Shield.bRadians;
                            flo32 MaxRadians = MinRadians + Shield.Radians;
                            
                            flo32 InRadians = ToRadians( Player->Position - MiniBoss->Position );
                            if( InRadians < MinRadians ) {
                                InRadians += TAU;
                            }
                            
                            if( ( InRadians >= MinRadians ) && ( InRadians <= MaxRadians ) ) {
                                memset( MiniBoss->Shield_SegmentT, 0, sizeof( flo32 ) * MINIBOSS_SHIELD_SEGMENT_MAX_COUNT );
                                
                                flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
                                int32 iSegment   = ( int32 )( ( flo32 )MINIBOSS_SHIELD_SEGMENT_MAX_COUNT * t );
                                int32 MinSegment = MaxValue( iSegment - 1, 0 );
                                
                                for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                    int32 iSeg = ( MinSegment + Iter ) % MINIBOSS_SHIELD_SEGMENT_MAX_COUNT; 
                                    MiniBoss->Shield_SegmentT[ iSeg ] = MINIBOSS_SHIELD_MAX_T;
                                }
                                
                                DamagePlayer( AppState, 1, EntityType_MiniBoss, ( uint8 * )&MiniBoss, sizeof( MINIBOSS ) );
                                
                                vec2 Dir = GetNormal( Player->Position - MiniBoss->Position );
                                
                                Player->DoDamage_Velocity = Dir * PLAYER_DAMAGE_BOOST_SPEED;
                                Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                                
                                CanDamagePlayer = false;
                                
                                MiniBoss->Velocity = Dir * -MINIBOSS_DAMAGE_BOOST_SPEED;
                                
                                if( MiniBoss->iTarget == -1 ) {
                                    MiniBoss->iTarget_Timer += 1.5f;
                                }
                            }
                        }
                    }
                    
                    if( CanDamagePlayer ) { // End Capsules
                        flo32 Radius = PLAYER_RADIUS - 0.1f;
                        vec2 DirA = ToDirection2D( Shield.bRadians                  );
                        vec2 DirB = ToDirection2D( Shield.bRadians + Shield.Radians );
                        
                        flo32 OffsetA = MINIBOSS_RADIUS + Shield.Dist;
                        flo32 OffsetB = OffsetA + Shield.Width;
                        
                        CAPSULE2 CapA = Capsule2( MiniBoss->Position + DirA * OffsetA, MiniBoss->Position + DirA * OffsetB, Radius );
                        CAPSULE2 CapB = Capsule2( MiniBoss->Position + DirB * OffsetA, MiniBoss->Position + DirB * OffsetB, Radius );
                        
                        boo32 IsInCapsuleA = IsInCapsule( Player->Position, CapA );
                        boo32 IsInCapsuleB = IsInCapsule( Player->Position, CapB );
                        if( ( IsInCapsuleA ) || ( IsInCapsuleB ) ) {
                            memset( MiniBoss->Shield_SegmentT, 0, sizeof( flo32 ) * MINIBOSS_SHIELD_SEGMENT_MAX_COUNT );
                            
                            int32 iSegment = 0;
                            if( IsInCapsuleB ) {
                                flo32 t = Clamp01( Shield.Radians / TAU );
                                iSegment = ( int32 )( ( flo32 )MINIBOSS_SHIELD_SEGMENT_MAX_COUNT * t ) - 1;
                            }
                            
                            int32 MinSegment = MaxValue( iSegment - 1, 0 );
                            
                            for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                int32 iSeg = ( MinSegment + Iter ) % MINIBOSS_SHIELD_SEGMENT_MAX_COUNT; 
                                MiniBoss->Shield_SegmentT[ iSeg ] = MINIBOSS_SHIELD_MAX_T;
                            }
                            
                            DamagePlayer( AppState, 1, EntityType_MiniBoss, ( uint8 * )&MiniBoss, sizeof( MINIBOSS ) );
                            
                            vec2 Dir = GetNormal( Player->Position - MiniBoss->Position );
                            
                            Player->DoDamage_Velocity = Dir * PLAYER_DAMAGE_BOOST_SPEED;
                            Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                            
                            CanDamagePlayer = false;
                            
                            MiniBoss->Velocity = Dir * -MINIBOSS_DAMAGE_BOOST_SPEED;
                            
                            if( MiniBoss->iTarget == -1 ) {
                                MiniBoss->iTarget_Timer += 1.5f;
                            }
                        }
                    }
                }
            }
        }
#endif
        
        { // ASTEROID
            ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
            
            flo32 Margin[ AsteroidClass_Count ] = { 0.1f, 0.2f, 0.3f, 0.4f };
            
            for( int32 iAsteroid = 0; ( CanDamagePlayer ) && ( iAsteroid < AsteroidS->nAsteroid ); iAsteroid++ ) {
                ASTEROID Asteroid = AsteroidS->Asteroid[ iAsteroid ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Asteroid.Radius - Margin[ Asteroid.Class ] );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Asteroid.Position );
                
                if( DistSq <= RadiusSq ) {
                    // NOTE: I only allow Asteroids to damage the Player if more than half a unit of the Asteroid radius lies within the world bound. This is to prevent Asteroids from damaging the Player as the spawn from offscreen. This will cause an issue if the Asteroid is leaving the screen. I can fix it by flagging Asteroids that have and have not been onscreen, but I'm going to leave it as is for now.
                    
                    rect Valid_Bound = AddRadius( AppState->World_Bound, Asteroid.Radius - 0.5f );
                    if( IsInRect( Asteroid.Position, Valid_Bound ) ) {
                        if( Asteroid.Class > AsteroidClass_Tiny ) {
                            DamagePlayer( AppState, 1, EntityType_Asteroid, ( uint8 * )&Asteroid, sizeof( ASTEROID ) );
                            
                            Player->DoDamage_Velocity = GetNormal( Player->Position - Asteroid.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                            Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                            
                            CanDamagePlayer = false;
                        } else {
                            if( Player->SecondaryDamage_Timer <= PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME ) {
                                SecondaryDamageToPlayer( AppState, EntityType_Asteroid, ( uint8 * )&Asteroid, sizeof( ASTEROID ) );
                            } else {
                                DamagePlayer( AppState, 1, EntityType_Asteroid, ( uint8 * )&Asteroid, sizeof( ASTEROID ) );
                                
                                Player->DoDamage_Velocity = GetNormal( Player->Position - Asteroid.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                                Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                                
                                CanDamagePlayer = false;
                            }
                        }
                    }
                    
                    DoKillEntity         = true;
                    DoKillEntity_Type    = EntityType_Asteroid;
                    DoKillEntity_iEntity = iAsteroid;
                }
            }
        }
        
        { // MINER
            MINER_STATE * MinerS = &AppState->MinerS;
            
            for( int32 iMiner = 0; ( CanDamagePlayer ) && ( iMiner < MinerS->nMiner ); iMiner++ ) {
                MINER Miner = MinerS->Miner[ iMiner ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Miner.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Miner.Position );
                
                if( DistSq <= RadiusSq ) {
                    for( int32 iLaser = 0; iLaser < Miner.nLaser; iLaser++ ) {
                        Miner.Laser[ iLaser ].IsActive = false;
                    }
                    
                    DamagePlayer( AppState, 1, EntityType_Miner, ( uint8 * )&Miner, sizeof( MINER ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Miner.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
                
                for( int32 iLaser = 0; ( CanDamagePlayer ) && ( iLaser < Miner.nLaser ); iLaser++ ) {
                    MINER_LASER Laser = Miner.Laser[ iLaser ];
                    
                    if( Laser.IsActive ) {
                        CAPSULE2 Cap = Capsule2( Miner.Position, Laser.Dest, PLAYER_RADIUS - 0.25f + MINER_LASER_WIDTH * 0.5f );
                        if( IsInCapsule( Player->Position, Cap ) ) {
                            for( int32 Iter = 0; Iter < Miner.nLaser; Iter++ ) {
                                if( Iter != iLaser ) {
                                    Miner.Laser[ Iter ].IsActive = false;
                                }
                            }
                            
                            DamagePlayer( AppState, 1, EntityType_Miner, ( uint8 * )&Miner, sizeof( MINER ) );
                            
                            vec2 Dir = GetNormal( GetPerp( Laser.Dest - Miner.Position ) );
                            if( dot( Player->Position - Miner.Position, Dir ) < 0.0f ) {
                                Dir = -Dir;
                            }
                            
                            Player->DoDamage_Velocity = Dir * PLAYER_DAMAGE_BOOST_SPEED;
                            Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                            
                            CanDamagePlayer = false;
                        }
                    }
                }
            }
            
            for( int32 iLaser = 0; ( CanDamagePlayer ) && ( iLaser < MinerS->nLaser ); iLaser++ ) {
                LASER Laser = MinerS->Laser[ iLaser ];
                
                if( Laser.IsActive ) {
                    CAPSULE2 Cap = Capsule2( Laser.Position, Laser.Dest, PLAYER_RADIUS - 0.25f + MINER_LASER_WIDTH * 0.5f );
                    if( IsInCapsule( Player->Position, Cap ) ) {
                        DamagePlayer( AppState, 1, EntityType_Laser, ( uint8 * )&Laser, sizeof( LASER ) );
                        
                        vec2 Dir = GetNormal( GetPerp( Laser.Dest - Laser.Position ) );
                        if( dot( Player->Position - Laser.Position, Dir ) < 0.0f ) {
                            Dir = -Dir;
                        }
                        
                        Player->DoDamage_Velocity = Dir * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
        
        { // MEGA_MINER
            MINER_STATE * MinerS = &AppState->MinerS;
            
            for( int32 iMegaMiner = 0; ( CanDamagePlayer ) && ( iMegaMiner < MinerS->nMegaMiner ); iMegaMiner++ ) {
                MEGA_MINER MegaMiner = MinerS->MegaMiner[ iMegaMiner ];
                {
                    flo32 Radius   = ( PLAYER_RADIUS + MegaMiner.Radius - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - MegaMiner.Position );
                    
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_MegaMiner, ( uint8 * )&MegaMiner, sizeof( MEGA_MINER ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - MegaMiner.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
                
                if( CanDamagePlayer ) { // Drill Collision
                    vec2 yAxis = GetNormal( ToDirection2D( MegaMiner.AtRadians ) );
                    vec2 xAxis = -GetPerp( yAxis );
                    
                    vec2 bPos  = MegaMiner.Position + yAxis * MEGA_MINER_DRILL_OFFSET;
                    vec2 Scale = MEGA_MINER_DRILL_SCALE;
                    vec2 A     = bPos + xAxis * -Scale.x + yAxis * -Scale.y;
                    vec2 B     = bPos                    + yAxis *  Scale.y;
                    vec2 C     = bPos + xAxis *  Scale.x + yAxis * -Scale.y;
                    
                    flo32 Radius   = ( PLAYER_RADIUS * 0.85f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = DistSqPointToTri( Player->Position, A, B, C );
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_MegaMiner, ( uint8 * )&MegaMiner, sizeof( MEGA_MINER ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - MegaMiner.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
        
#if 0        
        { // BOMBER
            BOMB_STATE * BombS = &AppState->BombS;
            
            for( int32 iBomber = 0; ( CanDamagePlayer ) && ( iBomber < BombS->nBomber ); iBomber++ ) {
                BOMBER Bomber = BombS->Bomber[ iBomber ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Bomber.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Bomber.Position );
                
                if( DistSq <= RadiusSq ) {
                    DamagePlayer( AppState, 1, EntityType_Bomber, ( uint8 * )&Bomber, sizeof( BOMBER ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Bomber.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
#endif
        
        { // BOMB
            BOMB_STATE * BombS = &AppState->BombS;
            
            for( int32 iBomb = 0; ( CanDamagePlayer ) && ( iBomb < BombS->nBomb ); iBomb++ ) {
                BOMB Bomb = BombS->Bomb[ iBomb ];
                
                if( Bomb.IsDetonated ) {
                    flo32 Radius   = BOMB_DAMAGE_RADIUS - 0.5f;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Bomb.Position );
                    
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_Bomb, ( uint8 * )&Bomb, sizeof( BOMB ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Bomb.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
        
        { // BOMB_LAUNCHER
            BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
            
            for( int32 iBombLauncher = 0; ( CanDamagePlayer ) && ( iBombLauncher < BombLauncherS->nBombLauncher ); iBombLauncher++ ) {
                BOMB_LAUNCHER BombLauncher = BombLauncherS->BombLauncher[ iBombLauncher ];
                
                flo32 Radius   = ( PLAYER_RADIUS + BombLauncher.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - BombLauncher.Position );
                
                boo32 CanHit = ( BombLauncher.Health > 0 )
                    && ( ( BombLauncher.Stage == BombLauncherStage_EnableBombLauncher )
                        || ( BombLauncher.Stage == BombLauncherStage_SpawnBombs ) );
                
                if( ( CanHit ) && ( DistSq <= RadiusSq ) ) {
                    DamagePlayer( AppState, 1, EntityType_BombLauncher, ( uint8 * )&BombLauncher, sizeof( BOMB_LAUNCHER ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - BombLauncher.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
#if 0        
        { // BANK
            BANK_STATE * BankS = &AppState->BankS;
            
            for( int32 iBank = 0; ( CanDamagePlayer ) && ( iBank < BankS->nBank ); iBank++ ) {
                BANK Bank = BankS->Bank[ iBank ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Bank.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Bank.Position );
                
                if( DistSq <= RadiusSq ) {
                    DamagePlayer( AppState, 1, EntityType_Bank, ( uint8 * )&Bank, sizeof( BANK ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Bank.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
#endif
        
        { // BLIMP
            BLIMP_STATE * BlimpS = &AppState->BlimpS;
            
            for( int32 iBlimp = 0; ( CanDamagePlayer ) && ( iBlimp < BlimpS->nBlimp ); iBlimp++ ) {
                BLIMP Blimp = BlimpS->Blimp[ iBlimp ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Blimp.Radius - 0.5f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Blimp.Position );
                
                if( DistSq <= RadiusSq ) {
                    DamagePlayer( AppState, 1, EntityType_Blimp, ( uint8 * )&Blimp, sizeof( BLIMP ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Blimp.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
        { // BAY_DOOR
            BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
            
            for( int32 iYacht = 0; ( CanDamagePlayer ) && ( iYacht < BayDoorS->nYacht ); iYacht++ ) {
                YACHT * Yacht = BayDoorS->Yacht + iYacht;
                
                if( ( Yacht->Health > 0 ) && ( Yacht->Damage_IFrameTimer <= 0.0f ) ) {
                    flo32 Radius   = ( PLAYER_RADIUS + YACHT_RADIUS - 0.5f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Yacht->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_Yacht, ( uint8 * )Yacht, sizeof( YACHT ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Yacht->Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                        
                        vec2 Dir = GetNormal( Player->Position - Yacht->Position );
                        Yacht->Velocity   = Dir * -YACHT_DAMAGE_BOOST_SPEED;
                        Yacht->Exit_Timer = -YACHT_DAMAGE_COOLDOWN_TARGET_TIME;
                        
                        Yacht->Health = MaxValue( Yacht->Health - 1, 0 );
                        
                        Yacht->Damage_IFrameTimer = YACHT_DAMAGE_IFRAME_TARGET_TIME;
                        Yacht->ShowDamage_Timer   = YACHT_SHOW_DAMAGE_TARGET_TIME;
                        
                        AddKillToScore( AppState, EntityType_Yacht_Hit, Yacht->Position + Dir * YACHT_RADIUS );
                        
                        POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Yacht_Hit );
                        int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                        vec2 Pos = ClampToRect( Yacht->Position, AppState->World_Bound );
                        for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                            AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                        }
                    }
                }
            }
        }
        
#if 0        
        { // ESCORT
            ESCORT_STATE * EscortS = &AppState->EscortS;
            
            for( int32 iEscort = 0; ( CanDamagePlayer ) && ( iEscort < EscortS->nEscort ); iEscort++ ) {
                ESCORT Escort = EscortS->Escort[ iEscort ];
                
                if( CanDamagePlayer ) { // Engine Collision
                    vec2 yAxis = Vec2( -1.0f, 0.0f );
                    vec2 xAxis = -GetPerp( yAxis );
                    
                    vec2 bPos  = Escort.Position + yAxis * ( ESCORT_ENGINE_OFFSET + ESCORT_ENGINE_MIN_SCALE_Y * 0.5f );
                    vec2 Sca   = Vec2( 4.0f, ESCORT_ENGINE_MIN_SCALE_Y ) * 0.5f * 0.8f;
                    vec2 A     = bPos + xAxis * -Sca.x * 0.6f + yAxis * -Sca.y * 0.4f;
                    vec2 B     = bPos + yAxis *  Sca.y;
                    vec2 C     = bPos + xAxis *  Sca.x * 0.6f + yAxis * -Sca.y * 0.4f;
                    
                    flo32 Radius   = ( PLAYER_RADIUS * 0.85f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = DistSqPointToTri( Player->Position, A, B, C );
                    if( DistSq <= RadiusSq ) {
                        if( Player->SecondaryDamage_Timer <= PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME ) {
                            SecondaryDamageToPlayer( AppState, EntityType_Escort, ( uint8 * )&Escort, sizeof( ESCORT ) );
                        } else { 
                            DamagePlayer( AppState, 1, EntityType_Escort, ( uint8 * )&Escort, sizeof( ESCORT ) );
                            
                            Player->DoDamage_Velocity = GetNormal( Player->Position - Escort.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                            Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                            
                            CanDamagePlayer = false;
                        }
                    }
                }
            }
        }
#endif
        
        { // MISSILE
            MISSILE_STATE * MissileS = &AppState->MissileS;
            
            for( int32 iMissile = 0; ( CanDamagePlayer ) && ( iMissile < MissileS->nMissile ); iMissile++ ) {
                MISSILE Missile = MissileS->Missile[ iMissile ];
                
                flo32 Radius   = ( PLAYER_RADIUS + MISSILE_RADIUS - 0.1f );
                
                vec2 A = Missile.Position;
                vec2 B = A - GetNormal( Missile.Velocity ) * MISSILE_LENGTH;
                CAPSULE2 Cap = Capsule2( A, B, Radius );
                
                if( IsInCapsule( Player->Position, Cap ) ) {
                    DamagePlayer( AppState, 1, EntityType_Missile, ( uint8 * )&Missile, sizeof( MISSILE ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Missile.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
        
        { // WORM
            WORM_STATE * WormS = &AppState->WormS;
            
            for( int32 iWorm = 0; ( CanDamagePlayer ) && ( iWorm < WormS->nWorm ); iWorm++ ) {
                WORM Worm = WormS->Worm[ iWorm ];
                
                if( Worm.Health > 0 ) {
                    flo32 Radius   = ( PLAYER_RADIUS + Worm.Radius - 0.25f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Worm.Position );
                    
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_Worm, ( uint8 * )&Worm, sizeof( WORM ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Worm.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
        
        { // FIREBALL
            FIREBALL_STATE * FireballS = &AppState->FireballS;
            
            for( int32 iFireball = 0; ( CanDamagePlayer ) && ( iFireball < FireballS->nFireball ); iFireball++ ) {
                FIREBALL Fireball = FireballS->Fireball[ iFireball ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Fireball.Radius - 0.25f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Fireball.Position );
                
                if( DistSq <= RadiusSq ) {
                    if( ( Fireball.Radius == FIREBALL_RADIUS_LARGE ) || ( Player->SecondaryDamage_Timer > PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME ) ) {
                        DamagePlayer( AppState, 1, EntityType_Fireball, ( uint8 * )&Fireball, sizeof( FIREBALL ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Fireball.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    } else {
                        SecondaryDamageToPlayer( AppState, EntityType_Fireball, ( uint8 * )&Fireball, sizeof( FIREBALL ) );
                    }
                    
                    DoKillEntity         = true;
                    DoKillEntity_Type    = EntityType_Fireball;
                    DoKillEntity_iEntity = iFireball;
                }
            }
        }
        
        { // SPIT
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            for( int32 iSpit = 0; ( CanDamagePlayer ) && ( iSpit < SpitterS->nSpit ); iSpit++ ) {
                SPIT Spit = SpitterS->Spit[ iSpit ];
                
                if( !Spit.IsSterilized ) {
                    flo32 Radius   = ( PLAYER_RADIUS + Spit.Radius - 0.5f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Spit.Position );
                    
                    if( DistSq <= RadiusSq ) {
                        if( Player->SecondaryDamage_Timer <= PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME ) {
                            SecondaryDamageToPlayer( AppState, EntityType_Spit, ( uint8 * )&Spit, sizeof( SPIT ) );
                            
                            DoKillEntity         = true;
                            DoKillEntity_Type    = EntityType_Spit;
                            DoKillEntity_iEntity = iSpit;
                        } else {
                            DamagePlayer( AppState, 1, EntityType_Spit, ( uint8 * )&Spit, sizeof( SPIT ) );
                            
                            Player->DoDamage_Velocity = GetNormal( Player->Position - Spit.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                            Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                            
                            CanDamagePlayer = false;
                        }
                    }
                }
            }
        }
        
        { // LOADER
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            for( int32 iLoader = 0; ( CanDamagePlayer ) && ( iLoader < SpitterS->nLoader ); iLoader++ ) {
                LOADER * Loader = SpitterS->Loader + iLoader;
                
                if( Loader->Health > 0 ) {
                    flo32 Radius = PLAYER_RADIUS - 0.1f;
                    orect Bound  = ORectCD( Loader->Position, LOADER_DIM, Loader->Radians );
                    
                    if( IsInRoundedORect( Player->Position, Bound, Radius ) ) {
                        DamagePlayer( AppState, 1, EntityType_Loader, ( uint8 * )Loader, sizeof( LOADER ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Loader->Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                        
                        Loader->Timer = -LOADER_DAMAGE_PLAYER_COOLDOWN_TARGET_TIME;
                    }
                }
            }
        }
        
        
#if 0        
        { // DAMAGE_ZONE
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            for( int32 iDamageZone = 0; ( CanDamagePlayer ) && ( iDamageZone < SpitterS->nDamageZone ); iDamageZone++ ) {
                DAMAGE_ZONE DamageZone = SpitterS->DamageZone[ iDamageZone ];
                
                if( ( DamageZone.IsEnabled ) && ( DamageZone.Timer >= 0.0f ) && ( !DamageZone.IsInactive ) ) {
                    flo32 Radius = PLAYER_RADIUS - 0.5f;
                    
                    if( IsInRoundedORect( Player->Position, DamageZone.Bound, Radius ) ) {
                        DamagePlayer( AppState, 1, EntityType_DamageZone, ( uint8 * )&DamageZone, sizeof( DAMAGE_ZONE ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - DamageZone.Bound.Center ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
#endif
        
        { // SECURITY_DRONE
            BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
            OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
            
            if( ( BoardRoomS->State_IsActive ) && ( ObservationDeck->IsPresent ) ) {
                if( ( CanDamagePlayer ) && ( ObservationDeck->Health >= 0 ) ) {
                    SECURITY_DRONE SecurityDrone = BoardRoomS->SecurityDrone;
                    
                    flo32 Radius   = ( PLAYER_RADIUS + SecurityDrone.Radius - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - SecurityDrone.Position );
                    
                    if( DistSq <= RadiusSq ) {
                        for( int32 iLaser = 0; iLaser < SecurityDrone.nLaser; iLaser++ ) {
                            SecurityDrone.Laser_IsActive[ iLaser ] = false;
                        }
                        
                        DamagePlayer( AppState, 1, EntityType_SecurityDrone, ( uint8 * )&SecurityDrone, sizeof( SECURITY_DRONE ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - SecurityDrone.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                    
                    if( ( CanDamagePlayer ) && ( SecurityDrone.Laser_IsActive[ 0 ] ) && ( SecurityDrone.Timer >= 0.0f ) ) {
                        for( int32 iLaser = 0; iLaser < SecurityDrone.nLaser; iLaser++ ) {
                            vec2 Dest = SecurityDrone.Laser_Dest[ iLaser ];
                            
                            CAPSULE2 Cap = Capsule2( SecurityDrone.Position, Dest, PLAYER_RADIUS - 0.25f + MINER_LASER_WIDTH * 0.5f );
                            if( IsInCapsule( Player->Position, Cap ) ) {
                                for( int32 Iter = 0; Iter < SecurityDrone.nLaser; Iter++ ) {
                                    if( Iter != iLaser ) {
                                        SecurityDrone.Laser_IsActive[ Iter ] = false;
                                    }
                                }
                                
                                DamagePlayer( AppState, 1, EntityType_SecurityDrone, ( uint8 * )&SecurityDrone, sizeof( SECURITY_DRONE ) );
                                
                                vec2 Dir = GetNormal( GetPerp( Dest - SecurityDrone.Position ) );
                                if( dot( Player->Position - SecurityDrone.Position, Dir ) < 0.0f ) {
                                    Dir = -Dir;
                                }
                                
                                Player->DoDamage_Velocity = Dir * PLAYER_DAMAGE_BOOST_SPEED;
                                Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                                
                                CanDamagePlayer = false;
                            }
                        }
                    }
                }
                
                if( ( CanDamagePlayer ) && ( ObservationDeck->Barrier_IsActive ) && ( ObservationDeck->IsPresent ) && ( ObservationDeck->Health > -1 ) ) {
                    int32 nBarrier = OBSERVATION_DECK_BARRIER_COUNT;
                    
                    flo32 Radius_Inner = ObservationDeck->Radius + ObservationDeck->Barrier_Dist;
                    flo32 Radius_Outer = Radius_Inner + OBSERVATION_DECK_BARRIER_WIDTH;
                    
                    Radius_Inner -= ( PLAYER_RADIUS - 0.1f );
                    Radius_Outer += ( PLAYER_RADIUS - 0.1f );
                    
                    flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
                    flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
                    
                    vec2 Edge_Origin[ OBSERVATION_DECK_BARRIER_COUNT ][ 2 ] = {};
                    vec2 Edge_Dest  [ OBSERVATION_DECK_BARRIER_COUNT ][ 2 ] = {};
                    vec2 Edge_Dir   [ OBSERVATION_DECK_BARRIER_COUNT ][ 2 ] = {};
                    
                    for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                        flo32 Radians    = ObservationDeck->Barrier_Radians + ( TAU * 0.25f * ( flo32 )iBarrier ) - OBSERVATION_DECK_BARRIER_RADIANS * 0.5f;
                        flo32 MinRadians_Inner = Radians;
                        flo32 MaxRadians_Inner = MinRadians_Inner + OBSERVATION_DECK_BARRIER_RADIANS;
                        
                        flo32 RadiansPerSegment = ( OBSERVATION_DECK_BARRIER_RADIANS / ( flo32 )OBSERVATION_DECK_BARRIER_SEGMENT_COUNT );
                        flo32 MinRadians_Outer = MinRadians_Inner + ( RadiansPerSegment * 0.5f );
                        flo32 MaxRadians_Outer = MaxRadians_Inner - ( RadiansPerSegment * 0.5f );
                        
                        vec2 A = ObservationDeck->Position + ToDirection2D( MinRadians_Inner ) * Radius_Inner;
                        vec2 B = ObservationDeck->Position + ToDirection2D( MinRadians_Outer ) * Radius_Outer;
                        vec2 C = ObservationDeck->Position + ToDirection2D( MaxRadians_Outer ) * Radius_Outer;
                        vec2 D = ObservationDeck->Position + ToDirection2D( MaxRadians_Inner ) * Radius_Inner;
                        
                        Edge_Origin[ iBarrier ][ 0 ] = A;
                        Edge_Origin[ iBarrier ][ 1 ] = C;
                        Edge_Dest  [ iBarrier ][ 0 ] = B;
                        Edge_Dest  [ iBarrier ][ 1 ] = D;
                        Edge_Dir   [ iBarrier ][ 0 ] = GetPerp( B - A );
                        Edge_Dir   [ iBarrier ][ 1 ] = GetPerp( D - C );
                    }
                    
                    flo32 DistSq = GetLengthSq( Player->Position - ObservationDeck->Position );
                    if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                        for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                            vec2  VA   = Player->Position - Edge_Origin[ iBarrier ][ 0 ];
                            vec2  VB   = Player->Position - Edge_Origin[ iBarrier ][ 1 ];
                            
                            flo32 DotA = dot( VA, Edge_Dir[ iBarrier ][ 0 ] );
                            flo32 DotB = dot( VB, Edge_Dir[ iBarrier ][ 1 ] );
                            
                            CAPSULE2 CapA = Capsule2( Edge_Origin[ iBarrier ][ 0 ], Edge_Dest[ iBarrier ][ 0 ], PLAYER_RADIUS - 0.1f );
                            CAPSULE2 CapB = Capsule2( Edge_Origin[ iBarrier ][ 1 ], Edge_Dest[ iBarrier ][ 1 ], PLAYER_RADIUS - 0.1f );
                            
                            boo32 IsDotAB  = ( ( DotA >= 0.0f ) && ( DotB >= 0.0f ) );
                            boo32 IsInCapA = IsInCapsule( Player->Position, CapA );
                            boo32 IsInCapB = IsInCapsule( Player->Position, CapB );
                            
                            if( ( IsDotAB ) || ( IsInCapA ) || ( IsInCapB ) ) {
                                flo32 t   = ( flo32 )iBarrier / ( flo32 )nBarrier;
                                flo32 Rad = ObservationDeck->Barrier_Radians + TAU * t;
                                flo32 Dis = ObservationDeck->Radius + ObservationDeck->Barrier_Dist;
                                vec2  Pos = ObservationDeck->Position + ToDirection2D( Rad ) * Dis;
                                
                                OBSERVATION_DECK_BARRIER ObservationDeckBarrier = {};
                                ObservationDeckBarrier.Position = Pos;
                                ObservationDeckBarrier.Radians  = Rad;
                                DamagePlayer( AppState, 1, EntityType_ObservationDeckBarrier, ( uint8 * )&ObservationDeckBarrier, sizeof( OBSERVATION_DECK_BARRIER ) );
                                
                                vec2 Dir = {};
                                if( IsInCapA ) {
                                    Dir = GetNormal( GetPerp( CapA.P - CapA.Q ) );
                                } else if( IsInCapB ) {
                                    Dir = GetNormal( GetPerp( CapB.P - CapB.Q ) );
                                } else {
                                    flo32 MidRadius = ( Radius_Inner + Radius_Outer ) * 0.5f;
                                    flo32 Dist      = GetLength( Player->Position - ObservationDeck->Position );
                                    
                                    Dir = GetNormal( Player->Position - ObservationDeck->Position );
                                    if( Dist < MidRadius ) {
                                        Dir = -Dir;
                                    }
                                }
                                
                                Player->DoDamage_Velocity = Dir * PLAYER_DAMAGE_BOOST_SPEED;
                                Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                                
                                CanDamagePlayer = false;
                            }
                        }
                    }
                }
                
                if( CanDamagePlayer ) {
                    for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                        MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                        if( ( CanDamagePlayer ) && ( MissileLauncher->Health > 0 ) ) {
                            flo32 Radius = MissileLauncher->Radius + PLAYER_RADIUS - 0.1f;
                            flo32 RadiusSq = Radius * Radius;
                            flo32 DistSq   = GetLengthSq( Player->Position - MissileLauncher->Position );
                            
                            if( DistSq <= RadiusSq ) {
                                DamagePlayer( AppState, 1, EntityType_MissileLauncher, ( uint8 * )MissileLauncher, sizeof( MISSILE_LAUNCHER ) );
                                
                                Player->DoDamage_Velocity = GetNormal( Player->Position - MissileLauncher->Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                                Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                                
                                CanDamagePlayer = false;
                            }
                        }
                    }
                }
            }
        }
        
#if 0        
        { // BOLT
            BOLT_STATE * BoltS = &AppState->BoltS;
            
            for( int32 iBolt = 0; ( CanDamagePlayer ) && ( iBolt < BoltS->nBolt ); iBolt++ ) {
                BOLT Bolt = BoltS->Bolt[ iBolt ];
                
                flo32 Radius   = ( PLAYER_RADIUS + Bolt.Radius - 0.1f );
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Player->Position - Bolt.Position );
                
                if( DistSq <= RadiusSq ) {
                    DamagePlayer( AppState, 1, EntityType_Bolt, ( uint8 * )&Bolt, sizeof( BOLT ) );
                    
                    Player->DoDamage_Velocity = GetNormal( Player->Position - Bolt.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                    
                    CanDamagePlayer = false;
                }
            }
        }
#endif
        
        { // GUNNER
            GUNNER_STATE * GunnerS = &AppState->GunnerS;
            
            for( int32 iGunner = 0; ( CanDamagePlayer ) && ( iGunner < GunnerS->nGunner ); iGunner++ ) {
                GUNNER Gunner = GunnerS->Gunner[ iGunner ];
                
                if( Gunner.Health > 0 ) {
                    flo32 Radius   = ( PLAYER_RADIUS + Gunner.Radius - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Gunner.Position );
                    
                    if( DistSq <= RadiusSq ) {
                        DamagePlayer( AppState, 1, EntityType_Gunner, ( uint8 * )&Gunner, sizeof( GUNNER ) );
                        
                        Player->DoDamage_Velocity = GetNormal( Player->Position - Gunner.Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                        
                        CanDamagePlayer = false;
                    }
                }
            }
        }
        
        if( Player->Health != PrevHealth ) {
            if( ( DoKillEntity ) && ( DoKillEntity_Type == EntityType_Asteroid ) ) {
                ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
                ASTEROID       * Asteroid  = AsteroidS->Asteroid + DoKillEntity_iEntity;
                
                if( !( ( Asteroid->Flags & ASTEROID_FLAG__BOMB_ONLY ) || ( Asteroid->Flags & ASTEROID_FLAG__LASER_ONLY ) ||
                      ( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) ) {
                    Asteroid->Health = 0;
                    Asteroid->Flags  = 0;
                }
            }
            
            KillAllEnemies( AppState );
        } else if( DoKillEntity ) {
            switch( DoKillEntity_Type ) {
                case EntityType_Swarm: {
                    SWARM_STATE * SwarmS = &AppState->SwarmS;
                    SWARM         Swarm  = SwarmS->Swarm[ DoKillEntity_iEntity ];
                    
                    AddEntityDeathParticles( AppState, Swarm.Position, SWARM_BASE_COLOR );
                    
                    AddKillToWave( AppState, Swarm.WaveGuid );
                    SwarmS->Swarm[ DoKillEntity_iEntity ] = SwarmS->Swarm[ --SwarmS->nSwarm ];
                } break;
                
#if 0                
                case EntityType_Star: {
                    STARBURST_STATE * StarburstS = &AppState->StarburstS;
                    STAR              Star       = StarburstS->Star[ DoKillEntity_iEntity ];
                    
                    // TODO: Change Star death particles to sparks!
                    AddEntityDeathParticles( AppState, Star.Position, ToColor( 255, 110, 145 ) );
                    AddKillToWave( AppState, Star.WaveGuid );
                    StarburstS->Star[ DoKillEntity_iEntity ] = StarburstS->Star[ --StarburstS->nStar ];
                } break;
#endif
                
                case EntityType_Asteroid: {
                    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
                    ASTEROID       * Asteroid  = AsteroidS->Asteroid + DoKillEntity_iEntity;
                    
                    Asteroid->Health = 0;
                    Asteroid->Flags  = 0;
                } break;
                
                case EntityType_Missile: {
                    MISSILE_STATE * MissileS = &AppState->MissileS;
                    MISSILE         Missile  = MissileS->Missile[ DoKillEntity_iEntity ];
                    
                    AddMissileDeathParticles( AppState, Missile.Position );
                    AddKillToWave( AppState, Missile.WaveGuid );
                    MissileS->Missile[ DoKillEntity_iEntity ] = MissileS->Missile[ --MissileS->nMissile ];
                } break;
                
                case EntityType_Fireball: {
                    FIREBALL_STATE * FireballS = &AppState->FireballS;
                    FIREBALL         Fireball  = FireballS->Fireball[ DoKillEntity_iEntity ];
                    
                    vec4 bColor = FIREBALL_POINTS_BASE_COLOR;
                    if( Fireball.Type == FireballType_Multiplier ) {
                        bColor = FIREBALL_MULTIPLIER_BASE_COLOR;
                    }
                    
                    if( Fireball.Radius == FIREBALL_RADIUS_LARGE ) {
                        AddBigEntityDeathParticles( AppState, Fireball.Position, Fireball.Radius, bColor );
                        AddBigEntityDeathParticles( AppState, Fireball.Position, Fireball.Radius, bColor );
                    } else {
                        AddEntityDeathParticles( AppState, Fireball.Position, bColor );
                        AddEntityDeathParticles( AppState, Fireball.Position, bColor );
                    }
                    
                    AddKillToWave( AppState, Fireball.WaveGuid );
                    FireballS->Fireball[ DoKillEntity_iEntity ] = FireballS->Fireball[ --FireballS->nFireball ];
                } break;
                
                case EntityType_Spit: {
                    SPITTER_STATE * SpitterS = &AppState->SpitterS;
                    SPIT            Spit     = SpitterS->Spit[ DoKillEntity_iEntity ];
                    
                    SPIT_BASE_COLORS;
                    AddEntityDeathParticles( AppState, Spit.Position, SpitBaseColors[ Spit.Type ] );
                    SpitterS->Spit[ DoKillEntity_iEntity ] = SpitterS->Spit[ --SpitterS->nSpit ];
                } break;
                
                default: {
                    InvalidCodePath;
                } break;
            }
        }
    }
}

internal void
UpdatePlayer( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    Player->LastDamage_Timer  = MaxValue( Player->LastDamage_Timer  - dT, 0.0f );
    Player->ScreenFlash_Timer = MaxValue( Player->ScreenFlash_Timer - dT, 0.0f );
    
    if( Player->IsDead ) {
        Player->IsDead_Timer += dT;
    } else {
        Player->DoDamage_Timer        = MaxValue( Player->DoDamage_Timer        - dT, 0.0f );
        Player->ShowDamage_Timer      = MaxValue( Player->ShowDamage_Timer      - dT, 0.0f );
        Player->NoDamage_Timer        = MaxValue( Player->NoDamage_Timer        - dT, 0.0f );
        Player->SecondaryDamage_Timer = MaxValue( Player->SecondaryDamage_Timer - dT, 0.0f );
        
        Player->ShowShieldPowerup_Timer     = MaxValue( Player->ShowShieldPowerup_Timer     - dT, 0.0f );
        Player->Powerup_Timer_PlayerBullets = MaxValue( Player->Powerup_Timer_PlayerBullets - dT, 0.0f );
        
        Player->DoBumper_Timer        = MaxValue( Player->DoBumper_Timer        - dT, 0.0f );
        
        Player->Facing_Radians = WrapRadians( Player->Facing_Radians - ( Player->Rotate_Speed * dT ) );
        
        Player->Fire_CooldownTimer = MaxValue( Player->Fire_CooldownTimer - dT, 0.0f );
        //if( ( Player->Rotate_Speed >= PLAYER_ROTATE_SPEED_FAST ) && ( Player->Fire_CooldownTimer <= 0.0f ) ) {
        if( Player->Fire_CooldownTimer <= 0.0f ) {
            Player->Fire_CooldownTimer += Player->Fire_CooldownTargetTime;
            
            vec2 Direction = ToDirection2D( Player->Facing_Radians );
            vec2 BulletP   = Player->Position;
            
            if( !Player->Shield_IsActive ) {
                AddBullet( AppState, BulletC( BulletP, Direction ) );
                
                if( Player->Rotate_Speed == PLAYER_ROTATE_SPEED_SLOW ) {
                    flo32 Rate = RandomF32InRange( 0.8f, 1.2f );
                    PlaySoundEffect( AppState, AudioID_Bullet_Slow, 8, PLAYER_BULLET_VOLUME, Rate );
                }
                if( Player->Rotate_Speed == PLAYER_ROTATE_SPEED_FAST ) {
                    flo32 Rate = 1.0f;
                    // PlayAudio( AppState, AudioID_Bullet_Fast, PLAYER_BULLET_VOLUME, Rate );
                }
                
                //flo32 RadiansA = Player->Facing_Radians - ( PI / 6.0f );
                //flo32 RadiansB = Player->Facing_Radians + ( PI / 6.0f );
                //AddBullet( AppState, BulletC( BulletP, ToDirection2D( RadiansA ) ) );
                //AddBullet( AppState, BulletC( BulletP, ToDirection2D( RadiansB ) ) );
            }
        }
        
        { // Update Gravity Well Particles
            flo32 MaxTargetTime = ( 120.0f / 60.0f );
            if( ( Player->GravityWell_IsActive ) && ( Player->GravityWell_IsEnabled ) ) {
                Player->GravityWell_Timer += dT;
            } else {
                Player->GravityWell_Timer  = 0.0f;
            }
            Player->GravityWell_Timer = Clamp( Player->GravityWell_Timer, 0.0f, MaxTargetTime );
            Player->nGravityWell_Particle = ( int32 )( Clamp01( Player->GravityWell_Timer / MaxTargetTime ) * ( flo32 )GRAVITY_WELL_PARTICLE_MAX_COUNT );
            
            
            for( int32 iParticle = 0; iParticle < GRAVITY_WELL_PARTICLE_MAX_COUNT; iParticle++ ) {
                GRAVITY_WELL_PARTICLE * Particle = Player->GravityWell_Particle + iParticle;
                
                {
                    vec2 Dir = ToDirection2D( Particle->Radians );
                    flo32 tDist = lerp( 0.25f, Clamp01( Particle->Dist / 14.0f ), 0.1f );
                    
                    Particle->Dist -= dot( Player->dPos, Dir ) * tDist;
                    Particle->Dist  = MaxValue( Particle->Dist, 0.0f );
                }
                
                if( Particle->Timer < Particle->TargetTime ) {
                    flo32 Speed = ( Player->GravityWell_IsActive ? Player->Rotate_Speed : 0.0f );
                    Particle->Speed += ( Speed - Particle->Speed ) * dT;
                    
                    Particle->Radians  = WrapRadians( Particle->Radians - ( Particle->Speed * dT / ( Particle->Dist * 0.4f ) ) );
                    Particle->Dist    -= ( Particle->Dist * 0.25f * dT );
                    
                    Particle->Timer += dT;
                } else if( ( Player->GravityWell_IsActive ) && ( iParticle < Player->nGravityWell_Particle ) ) {
                    Particle->Speed      = Player->Rotate_Speed;
                    Particle->TargetTime = RandomF32InRange( 1.0f, 2.0f );
                    Particle->Timer      = 0.0f;
                    Particle->Radians    = RandomF32() * PI * 2.0f;
                    Particle->Dist       = RandomF32InRange( 2.5f, 14.0f );
                }
            }
        }
        
        if( Player->Shield_IsEnabled ) {
            UpdateShieldSegmentT( Player->Shield_SegmentT, dT );
        }
        
        { // ASTEROID COLLISION
            if( Player->Shield_IsActive ) {
                ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
                for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                    ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                    
                    flo32 DistSq   = GetLengthSq( Player->Position - Asteroid->Position );
                    flo32 Radius   = ( PLAYER_SHIELD_RADIUS + Asteroid->Radius - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    if( DistSq <= RadiusSq ) {
                        vec2 Norm = GetNormal( Player->Position - Asteroid->Position );
                        
                        Player->DoBumper_Timer    = PLAYER_BUMPER_BOOST_TARGET_TIME;
                        Player->DoBumper_Velocity = Norm * PLAYER_DAMAGE_BOOST_SPEED;
                        
                        flo32 DotA      = dot( GetPerp( Asteroid->Direction ), GetNormal( Asteroid->Position - Player->Position ) );
                        flo32 dRadiansA = ( Asteroid->dRadians - ( DotA * ( TAU / 4.5f ) ) ) * 0.6f;
                        
                        vec2 Velocity = Asteroid->Direction * Asteroid->Speed;
                        Velocity -= dot( Velocity, Norm ) * Norm;
                        Velocity -= Norm * BUMPER_BOUNCE_SPEED;
                        
                        Asteroid->Direction = GetNormal( Velocity );
                        Asteroid->Speed     = GetLength( Velocity );
                        Asteroid->dRadians  = dRadiansA;
                        
                        flo32 InRadians = ToRadians( Asteroid->Position - Player->Position );
                        HitShield( Player->Shield, Player->Shield_SegmentT, InRadians );
                    }
                }
            }
        }
        
        { // MINER COLLISION
            if( Player->Shield_IsActive ) {
                MINER_STATE * MinerS = &AppState->MinerS;
                for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                    MINER * Miner = MinerS->Miner + iMiner;
                    Assert( !Miner->IsAutonomous );
                    
                    flo32 DistSq   = GetLengthSq( Player->Position - Miner->Position );
                    flo32 Radius   = ( PLAYER_SHIELD_RADIUS + Miner->Radius - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    if( DistSq <= RadiusSq ) {
                        vec2 Norm = GetNormal( Player->Position - Miner->Position );
                        
                        Player->DoBumper_Timer    = PLAYER_BUMPER_BOOST_TARGET_TIME;
                        Player->DoBumper_Velocity = Norm * PLAYER_DAMAGE_BOOST_SPEED;
                        
                        flo32 InRadians = ToRadians( Miner->Position - Player->Position );
                        HitShield( Player->Shield, Player->Shield_SegmentT, InRadians );
                    }
                }
            }
        }
        
        { // SCORE_MULTIPLIER COLLISION
            if( Player->Shield_IsActive ) {
                SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < ScoreMultiplierS->nScoreMultiplier; iScoreMultiplier++ ) {
                    SCORE_MULTIPLIER * ScoreMultiplier = ScoreMultiplierS->ScoreMultiplier + iScoreMultiplier;
                    
                    flo32 DistSq   = GetLengthSq( Player->Position - ScoreMultiplier->Position );
                    flo32 Radius   = ( PLAYER_SHIELD_RADIUS + SCORE_MULTIPLIER_RADIUS - 0.1f );
                    flo32 RadiusSq = Radius * Radius;
                    if( DistSq <= RadiusSq ) {
                        //Player->DoBumper_Timer    = PLAYER_BUMPER_BOOST_TARGET_TIME;
                        //Player->DoBumper_Velocity = GetNormal( Player->Position - ScoreMultiplier->Position ) * PLAYER_DAMAGE_BOOST_SPEED;
                        
                        ScoreMultiplier->Velocity += GetNormal( ScoreMultiplier->Position - Player->Position ) * BUMPER_BOUNCE_SPEED;
                        
                        ScoreMultiplier->Direction = GetNormal( ScoreMultiplier->Velocity );
                        //ScoreMultiplier->Speed     = GetLength( ScoreMultiplier->Velocity );
                        
                        flo32 InRadians = ToRadians( ScoreMultiplier->Position - Player->Position );
                        HitShield( Player->Shield, Player->Shield_SegmentT, InRadians );
                    }
                }
            }
        }
        
        { // Update Collector
            Player->Powerup_Timer_SecondaryGadgets = MaxValue( Player->Powerup_Timer_SecondaryGadgets - dT, 0.0f );
            
            flo32 dRate = 1.0f / ( 6.0f / 60.0f );
            if( Player->Powerup_Timer_SecondaryGadgets > 0.0f ) {
                Player->Powerup_Rate_SecondaryGadgets += dRate * dT;
            } else {
                Player->Powerup_Rate_SecondaryGadgets -= dRate * dT;
            }
            Player->Powerup_Rate_SecondaryGadgets = Clamp( 1.0f, Player->Powerup_Rate_SecondaryGadgets, 2.0f );
            
            if( Player->Collector_IsActive ) {
                flo32 Collector_Radius = PLAYER_COLLECTOR_RADIUS;
                Collector_Radius *= Player->Powerup_Rate_SecondaryGadgets;
                
                int32 nCollector = SpitType_Count;
                
                flo32 bRadians = Player->Facing_Radians + TAU / ( ( flo32 )nCollector * 2.0f );
                flo32  Radians = TAU / ( flo32 )nCollector;
                
                for( int32 iCollector = 0; iCollector < nCollector; iCollector++ ) {
                    flo32 * Radians0      = Player->Collector_Anim_Radians     [ iCollector ];
                    flo32 * Timer0        = Player->Collector_Anim_Timer       [ iCollector ];
                    flo32 * Dist0         = Player->Collector_Anim_Dist        [ iCollector ];
                    boo32 * IsSterilized0 = Player->Collector_Anim_IsSterilized[ iCollector ];
                    {
                        flo32 dRadians = ( ( TAU / 1.5f ) * dT );
                        
                        for( int32 iAnim = 0; iAnim < PLAYER_COLLECTOR_ANIM_MAX_COUNT; iAnim++ ) {
                            Radians0[ iAnim ] = WrapRadians( Radians0[ iAnim ] - dRadians );
                            Timer0  [ iAnim ] = MaxValue( Timer0[ iAnim ] - dT, 0.0f );
                        }
                    }
                    
                    {
                        flo32 Rad = bRadians + Radians * ( flo32 )iCollector;
                        vec2  Pos = Player->Position + ToDirection2D( Rad ) * ( PLAYER_RADIUS + Collector_Radius + PLAYER_COLLECTOR_DIST_FROM_PLAYER );
                        
                        { // Collect Spit
                            SPITTER_STATE * SpitterS = &AppState->SpitterS;
                            for( int32 iSpit = 0; iSpit < SpitterS->nSpit; iSpit++ ) {
                                SPIT * Spit = SpitterS->Spit + iSpit;
                                
                                if( ( iCollector == Spit->Type ) || ( Spit->IsSterilized ) ) {
                                    flo32 DistSq   = GetLengthSq( Pos - Spit->Position );
                                    flo32 Radius   = Collector_Radius + Spit->Radius - 0.1f;
                                    flo32 RadiusSq = Radius * Radius;
                                    if( DistSq <= RadiusSq ) {
                                        Spit->Health = 0;
                                        Player->Collector_Flash_Timer[ iCollector ] = PLAYER_COLLECTOR_FLASH_TARGET_TIME;
                                        
                                        flo32 R = ToRadians( Spit->Position - Pos );
                                        Radians0     [ Player->Collector_Anim_AtIndex[ iCollector ] ] = R;
                                        Timer0       [ Player->Collector_Anim_AtIndex[ iCollector ] ] = PLAYER_COLLECTOR_ANIM_TARGET_TIME;
                                        Dist0        [ Player->Collector_Anim_AtIndex[ iCollector ] ] = sqrtf( DistSq );
                                        IsSterilized0[ Player->Collector_Anim_AtIndex[ iCollector ] ] = Spit->IsSterilized;
                                        Player->Collector_Anim_AtIndex[ iCollector ] = ( Player->Collector_Anim_AtIndex[ iCollector ] + 1 ) % PLAYER_COLLECTOR_ANIM_MAX_COUNT;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

internal void
DrawPlayer( RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, APP_STATE * AppState, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player = Draw->Player;
    SCORE_STATE  * ScoreS = &AppState->ScoreS;
    
    if( Player->IsDead ) {
        
    } else {
#if 0
        {
            vec4 Color = COLOR_GREEN;
            if( Player->DoCollision ) {
                Color = COLOR_RED;
            }
            
            vec2 A = Player->Position;
            vec2 B = Player->Position + ToDirection2D( Player->DoCollision_Radians ) * 100.0f;
            
            DrawLine( Pass_Game, A, B, Color );
            DrawPoint( Pass_Game, Player->DoCollision_Pos, Vec2( 0.1f, 0.1f ), COLOR_GREEN );
            
            vec2 C = Player->DoCollision_Pos + Player->DoCollision_Norm * 2.0f;
            DrawLine( Pass_Game, Player->DoCollision_Pos, C, COLOR_CYAN );
        }
#endif
        { // Draw Player
            //DrawCircleOutline( Pass_Game, Player->Position, 12.0f, COLOR_RED );
            //DrawCircleOutline( Pass_Game, Player->Position, 3.0f, COLOR_RED );
            
            { // Draw Zero Point
                // NOTE: if Zero-Point is Enabled
                if( Player->ZeroPoint_IsEnabled ) {
                    if( Player->ZeroPoint_IsActive ) {
                        vec4  Color  = PLAYER_GADGET_BASE_COLOR;
                        flo32 Radius = ( PLAYER_RADIUS + 0.1f ) * 1.125f;
                        
                        rect Bound = RectCR( Player->Position, Radius );
                        DrawRect( Pass_Game, TextureID_ZeroPoint_Base_Active, Bound, Color );
                    } else if( Player->ZeroPoint_ShowUI ) {
                        vec4  Color  = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.5f );
                        flo32 Radius = ( PLAYER_RADIUS + 0.1f ) * 1.0625f;
                        
                        rect Bound = RectCR( Player->Position, Radius );
                        DrawRect( Pass_Game, TextureID_ZeroPoint_Base_Inactive, Bound, Color );
                    }
                    
#if 0                    
                    vec4  Color  = Vec4( 0.0f, 0.5f, 0.0f, 1.0f );
                    flo32 Radius = PLAYER_RADIUS * 1.15f;
                    
                    if( Player->ZeroPoint_IsActive ) {
                        Color  = Vec4( 0.0f, 1.0f, 0.0f, 1.0f );
                        Radius = PLAYER_RADIUS * 1.25f;
                        Radius = PLAYER_RADIUS + AUTO_TURRET_LASER_WIDTH;
                    }
                    
                    {
                        flo32 Dist    = PLAYER_RADIUS * 2.5f;
                        vec2  N       = GetNormal( Player->ZeroPoint_Pos - Player->Position );
                        vec2  P       = Player->Position + N * Dist;
                        
                        flo32 Theta   = asinf( PLAYER_RADIUS / Dist );
                        flo32 Radians = ToRadians( Player->Position - P );
                        flo32 TangentDist = sqrtf( Dist * Dist - PLAYER_RADIUS * PLAYER_RADIUS );
                        
                        vec2 A = P + ToDirection2D( Radians + Theta ) * TangentDist;
                        vec2 B = P + ToDirection2D( Radians - Theta ) * TangentDist;
                        
                        //DrawPoint( Pass_Game, P, Vec2( 0.25f, 0.25f ), COLOR_MAGENTA );
                        //DrawPoint( Pass_Game, A, Vec2( 0.25f, 0.25f ), COLOR_RED  );
                        //DrawPoint( Pass_Game, B, Vec2( 0.25f, 0.25f ), COLOR_BLUE );
                        
                        vec2  Scale    = Vec2( TangentDist, TangentDist ) / sqrtf( 2.0f );
                        
                        vec2  CenterA  = ( P + A ) * 0.5f;
                        vec2  CenterB  = ( P + B ) * 0.5f;
                        
                        flo32 RadiansA = Radians + Theta + PI * 0.25f;
                        flo32 RadiansB = Radians - Theta + PI * 1.25f;
                        
                        orect BoundA = ORectCD( CenterA, Scale, RadiansA );
                        orect BoundB = ORectCD( CenterB, Scale, RadiansB );
                        
                        //DrawORect( Pass_Game, TextureID_ZeroPoint_Arrow, BoundA, Color );
                        //DrawORect( Pass_Game, TextureID_ZeroPoint_Arrow, BoundB, Color );
                        
                        rect BoundC = RectCR( Player->Position, Radius );
                        DrawRect( Pass_Game, TextureID_ZeroPoint_Base_Active, BoundC, Color );
                    }
#endif
                    
                    
                    //DrawCircle( Pass_Game, Player->Position, Radius, Color );
                }
            }
            
            flo32 Alpha = 1.0f;
            vec4  Color = COLOR_WHITE;
            
            { // Show Damage
                if( Player->ShowDamage_Timer > 0.0f ) {
                    flo32 HalfTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 0.5f;
                    uint32 Cycle = ( uint32 )( Player->ShowDamage_Timer / HalfTime );
                    if( ( Cycle % 2 ) == 1 ) {
                        Alpha = 0.15f;
                    }
                }
            }
            
            Color *= Alpha;
            
#if 0
            flo32 BonusMultiplier_Dim = lerp( 0.05f, ScoreS->BonusMultiplier, 0.125f );
            DrawCircle( Pass_Game, Player->Position, PLAYER_RADIUS, COLOR_GRAY( 0.1f ) );
            { // Draw Bonus Multiplier
                vec4 BonusColor = lerp( Vec4( 0.75f, 0.75f, 0.0f, 1.0f ), 1.0f - ScoreS->BonusMultiplier, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
                if( ScoreS->BonusMultiplier >= 0.7f ) {
                    flo32 tCycle = cosf( ScoreS->BonusMultiplier_Radians ) * 0.5f + 0.5f;
                    BonusColor = lerp( COLOR_WHITE, tCycle, BonusColor );
                }
                
                
                int32 MaxSegment = 256;
                int32   nSegment = ( uint32 )( ScoreS->BonusMultiplier * ( flo32 )MaxSegment );
                for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                    flo32 Radians = ( ( flo32 )iSegment / ( flo32 )MaxSegment ) * PI * 2.0f;
                    vec2  Offset  = ToDirection2D( Radians ) * ( PLAYER_RADIUS * 0.5f );
                    vec2  Scale   = Vec2( ( PI * 2.0f ) / ( flo32 )MaxSegment, PLAYER_RADIUS * 0.5f );
                    DrawTriangle( Pass_Game, Player->Position + Offset, Scale, Radians + PI * 0.5f, BonusColor * Alpha );
                }
            }
            DrawCircle( Pass_Game, Player->Position, PLAYER_RADIUS - BonusMultiplier_Dim, Color );
#endif
            
            {
                //rect Bound = RectCR( Player->Position, PLAYER_RADIUS );
                //DrawRect( Pass_Game, TextureID_RoundedDome, Bound, Color );
                
                DrawORect( Pass_Game, TextureID_Player_Avatar, ORectCR( Player->Position, PLAYER_RADIUS, Player->Facing_Radians ), Color );
                
                //DrawCircle( Pass_Game, Player->Position, PLAYER_RADIUS, Color );
            }
            
            { // JETPACK GLOW
                vec2 Dir  = ToDirection2D( Player->Facing_Radians );
                vec2 Perp = GetPerp( Dir );
                
                flo32 xOffset = PLAYER_RADIUS * 0.675f;
                flo32 yOffset = PLAYER_RADIUS * 0.7f;
                
                vec2 Pos0 = Player->Position - ( Dir * yOffset ) + ( Perp * xOffset );
                vec2 Pos1 = Player->Position - ( Dir * yOffset ) - ( Perp * xOffset );
                
                flo32 xScale  = PLAYER_RADIUS * 0.05f;
                flo32 yScale0 = PLAYER_RADIUS * RandomF32InRange( 0.05f, 0.2f );
                flo32 yScale1 = PLAYER_RADIUS * RandomF32InRange( 0.05f, 0.2f ) * 0.5f;
                
                if( Player->Rotate_Speed == PLAYER_ROTATE_SPEED_FAST ) {
                    yScale0 *= 1.5f;
                }
                
                flo32 Radians = Player->Facing_Radians + PI * 0.5f;
                DrawTriangleBC( Pass_Game, Pos0, Vec2( xScale, yScale0 ), Radians, Color );
                DrawTriangleBC( Pass_Game, Pos1, Vec2( xScale, yScale1 ), Radians, Color );
            }
            
            //vec2  Gun_Dir   = ToDirection2D( Player->Facing_Radians );
            //vec2  Gun_Dim   = Vec2( 0.2f, 1.0f );
            //orect Gun_Bound = ORectBCD( Player->Position + Gun_Dir * 0.2f, Gun_Dim, Player->Facing_Radians );
            
            //DrawORect( Pass_Game, Gun_Bound, COLOR_RED );
        }
        
        { // Draw Zero-Point Beam
            if( ( Player->ZeroPoint_IsActive ) && ( Player->ZeroPoint_DrawBeam ) ) {
                vec2  Dir     = ToDirection2D( Player->ZeroPoint_Radians );
                vec2  Pos     = Player->Position + Dir * ( PLAYER_RADIUS + 0.1f );
                vec2  Dim     = Vec2( AUTO_TURRET_LASER_WIDTH * 4.0f, Player->ZeroPoint_Dist - 0.1f );
                
                orect Bound = ORectBCD( Pos, Dim, Player->ZeroPoint_Radians );
                DrawORect( Pass_Game, TextureID_AutoTurretLaser_Body, Bound, COLOR_WHITE );
            }
        }
        
        { // Draw Gravity Well
            for( int32 iParticle = 0; iParticle < GRAVITY_WELL_PARTICLE_MAX_COUNT; iParticle++ ) {
                GRAVITY_WELL_PARTICLE Particle = Player->GravityWell_Particle[ iParticle ];
                
                vec2 Offset = ToDirection2D( Particle.Radians ) * Particle.Dist;
                
                flo32 Alpha = 1.0f;
                
                flo32 FadeIn_TargetTime  = ( 10.0f / 60.0f );
                flo32 FadeOut_TargetTime = ( 14.0f / 60.0f );
                if( Particle.Timer <= FadeIn_TargetTime ) {
                    Alpha = Clamp01( Particle.Timer / FadeIn_TargetTime );
                }
                flo32 TargetTimeA = ( Particle.TargetTime - FadeOut_TargetTime );
                if( Particle.Timer >= TargetTimeA ) {
                    Alpha = 1.0f - Clamp01( ( Particle.Timer - TargetTimeA ) / FadeOut_TargetTime );
                }
                
                flo32 tDist = Clamp01( ( Particle.Dist - 1.5f ) / ( 8.0f - 1.5f ) );
                Alpha = MinValue( Alpha, lerp( 1.0f, tDist * tDist, 0.25f ) );
                
                flo32 tColor = Clamp01( ( Particle.Dist - 1.0f ) / ( 4.0f - 1.0f ) );
                vec4   Color = lerp( Vec4( 0.5f, 1.0f, 0.5f, 1.0f ), tColor * tColor, COLOR_WHITE );
                Color *= Alpha;
                
                flo32 tDim  = Clamp01( ( Particle.Dist - 1.0f ) / ( 4.0f - 1.0f ) );
                flo32  Dim0 = lerp( 0.1f, tDim * tDim, 0.05f );
                vec2   Dim  = Vec2( Dim0, Dim0 );
                
                Dim.y *= ( Particle.Speed * 2.0f + 1.0f );
                
                orect Bound = ORectTD( Player->Position + Offset, Vec2( 0.5f, 0.0f ), Dim, Particle.Radians );
                DrawORect( Pass_Game, Bound, Color );
                
                //DrawPoint( Pass_Game, Player->Position + Offset, Vec2( Dim, Dim ), Color );
            }
        }
        
        { // Draw Shield
            if( Player->Shield_IsActive ) {
                ENTITY_SHIELD Shield = Player->Shield;
                Assert( Shield.HasShield );
                
                flo32 Scale = 1.0f;
                flo32 Alpha = 1.0f;
                
                flo32 Radius_Inner   = ( PLAYER_RADIUS + Shield.Dist                ) * Scale;
                flo32 Radius_Outer   = ( PLAYER_RADIUS + Shield.Dist + Shield.Width ) * Scale;
                
                vec4 ColorA = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.5f );
                vec4 ColorB = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.0f );
                vec4 ColorC = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.25f );
                vec4 ColorD = TowardWhite( PLAYER_GADGET_BASE_COLOR, 0.25f );
                
                int32 nSegment = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * Clamp01( Shield.Radians / TAU ) );
                int32 nDrawSegment = nSegment;
                if( Shield.Radians < TAU ) {
                    nDrawSegment += 1;
                }
                
                for( int32 iSegment = 0; iSegment < nDrawSegment; iSegment++ ) {
                    flo32 Segment_Radians = Shield.bRadians + ( ( flo32 )iSegment / ( flo32 )nSegment ) * Shield.Radians;
                    vec2  Segment_Offset  = ToDirection2D( Segment_Radians ) * ( ( Radius_Outer + Radius_Inner ) * 0.5f );
                    vec2  Segment_Scale   = {};
                    if( ( iSegment % 2 ) == 0 ) {
                        Segment_Scale.x = ( TAU / ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Outer * 1.01f;
                    } else {
                        Segment_Scale.x = ( TAU / ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Inner * 1.01f;
                    }
                    Segment_Scale.y = ( Radius_Outer - Radius_Inner ) * 0.5f;
                    
                    flo32 t = Player->Shield_SegmentT[ iSegment ];
                    vec4 Color_Glow = {};
                    vec4 Color_Ring = {};
                    if( t > 1.0f ) {
                        flo32 tA = Clamp01( ( t - 1.0f ) / ( TURRET_SHIELD_MAX_T - 1.0f ) );
                        
                        Color_Glow = lerp( ColorB, tA, COLOR_WHITE );
                        Color_Ring = lerp( ColorD, tA, COLOR_WHITE );
                    } else {
                        Color_Glow = lerp( ColorA, Player->Shield_SegmentT[ iSegment ], ColorB );
                        Color_Ring = lerp( ColorC, Player->Shield_SegmentT[ iSegment ], ColorD );
                    }
                    
                    flo32 tOffset = lerp( 1.0f, t / TURRET_SHIELD_MAX_T, 0.985f );
                    flo32 tScaleY = lerp( 1.0f, t / TURRET_SHIELD_MAX_T, 0.9f );
                    
                    Segment_Offset  *= tOffset;
                    Segment_Scale.y *= tScaleY;
                    
                    if( iSegment == 0 ) {
                        MODEL_ID ModelID = MeshID_Triangle;
                        if( Shield.Radians < TAU ) {
                            ModelID = MeshID_Turret_Shield_EndCap;
                        }
                        DrawModel( Pass_Game, ModelID, TextureID_Turret_Shield_Glow, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Glow * Alpha );
                        DrawModel( Pass_Game, ModelID, TextureID_Turret_Shield_Ring, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Ring * Alpha );
                    } else if( iSegment == nSegment ) {
                        Segment_Scale.x = -Segment_Scale.x;
                        DrawModel( Pass_Game, MeshID_Turret_Shield_EndCap, TextureID_Turret_Shield_Glow, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Glow * Alpha );
                        DrawModel( Pass_Game, MeshID_Turret_Shield_EndCap, TextureID_Turret_Shield_Ring, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Ring * Alpha );
                    } else if( ( iSegment % 2 ) == 0 ) {
                        DrawTriangle      ( Pass_Game, TextureID_Turret_Shield_Glow, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Glow * Alpha );
                        DrawTriangle      ( Pass_Game, TextureID_Turret_Shield_Ring, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Ring * Alpha );
                    } else {
                        DrawTriangleFlipUV( Pass_Game, TextureID_Turret_Shield_Glow, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians - PI * 0.5f, Color_Glow * Alpha );
                        DrawTriangleFlipUV( Pass_Game, TextureID_Turret_Shield_Ring, Player->Position + Segment_Offset, Segment_Scale, Segment_Radians - PI * 0.5f, Color_Ring * Alpha );
                    }
                    
                    //DrawCircleOutline( Pass, Turret.Position, Radius_Inner + Shield.Width, COLOR_RED );
                    //DrawCircleOutline( Pass, Turret.Position, Radius_Inner, COLOR_RED );
                    
                    //DrawCircleSegmentOutline( Pass, Turret.Position, Radius_Inner, Shield.bRadians, Shield.Radians, false, COLOR_RED );
                    //DrawCircleSegmentOutline( Pass, Turret.Position, Radius_Outer, Shield.bRadians, Shield.Radians, false, COLOR_RED );
                }
            }
        }
        
        { // Draw Tractor Beam
            if( Player->TractorBeam_MaxAlpha > 0.0f ) {
                flo32 MinRadius = TRACTOR_BEAM_RING_MIN_RADIUS;
                flo32 MaxRadius = TRACTOR_BEAM_RING_MAX_RADIUS;
                
                flo32 TargetA = 0.2f;
                flo32 TargetB = 0.6f;
                
                for( int32 iRing = 0; iRing < TRACTOR_BEAM_RING_MAX_COUNT; iRing++ ) {
                    TRACTOR_BEAM_RING Ring = Player->TractorBeam_Ring[ iRing ];
                    if( Ring.DoShow ) {
                        flo32 tRadius = GetT01( Ring.Radius, MinRadius, MaxRadius );
                        flo32 Alph    = 1.0f;
                        
                        if( tRadius < TargetA ) {
                            Alph  = Clamp01( tRadius / TargetA );
                            Alph *= Alph;
                        } else if( tRadius > TargetB ) {
                            Alph  = 1.0f -  Clamp01( ( tRadius - TargetB ) / ( 1.0f - TargetB ) );
                            Alph *= Alph;
                        }
                        
                        vec4  ColorA   = PLAYER_GADGET_BASE_COLOR * Alph * Player->TractorBeam_MaxAlpha;
                        vec4  ColorB   = TowardWhite( PLAYER_GADGET_BASE_COLOR, 0.5f ) * Alph * Player->TractorBeam_MaxAlpha;
                        
                        //DrawCircleOutline( Pass_Game, Player->Position, Ring.Radius, Color );
                        DrawRect( Pass_Game, TextureID_TractorBeam_Glow, RectCR( Player->Position, Ring.Radius * 1.1f ), ColorA );
                        DrawRect( Pass_Game, TextureID_TractorBeam_Ring, RectCR( Player->Position, Ring.Radius * 1.1f ), ColorB );
                    }
                }
                
#if 0                
                int32 nRing = 6;
                for( int32 iRing = 0; iRing < nRing; iRing++ ) {
                    flo32 Radians = WrapRadians( Player->TractorBeam_AnimRadians + ( flo32 )iRing * ( TAU / ( flo32 )nRing ) );
                    flo32 t      =  Clamp01( Radians / TAU ) * 2.0f;
                    flo32 Radius = lerp( 3.0f, t, 1.25f );
                    
                    flo32 Alph    = 1.0f;
                    flo32 TargetA = 0.6f;
                    flo32 TargetB = 0.85f;
                    
                    if( t < TargetA ) {
                        Alph  =         Clamp01( t / TargetA );
                        Alph *= Alph;
                    } else if( t > TargetB ) {
                        Alph  = 1.0f -  Clamp01( ( t - TargetB ) / ( 1.0f - TargetB ) );
                    }
                    
                    DrawCircleOutline( Pass_Game, Player->Position, Radius, Vec4( 0.0f, 0.75f, 0.0f, 1.0f ) * Alph );
                }
#endif
            }
        }
        
        { // Draw Collector
            if( Player->Collector_IsActive ) {
                flo32 Collector_Radius = PLAYER_COLLECTOR_RADIUS;
                Collector_Radius *= Player->Powerup_Rate_SecondaryGadgets;
                
                SPIT_BASE_COLORS;
                SPIT_ALT_COLORS;
                
                int32 nCollector = SpitType_Count;
                
                flo32 bRadians = Player->Facing_Radians + TAU / ( ( flo32 )nCollector * 2.0f );
                flo32  Radians = TAU / ( flo32 )nCollector;
                
                flo32 Transparent = 0.75f;
                
                for( int32 iCollector = 0; iCollector < nCollector; iCollector++ ) {
                    flo32 t = Clamp01( Player->Collector_Flash_Timer[ iCollector ] / PLAYER_COLLECTOR_FLASH_TARGET_TIME );
                    
                    flo32 Rad = bRadians + Radians * ( flo32 )iCollector;
                    vec2  Pos = Player->Position + ToDirection2D( Rad ) * ( PLAYER_RADIUS + Collector_Radius + PLAYER_COLLECTOR_DIST_FROM_PLAYER );
                    
                    {
                        vec4  Color = SpitAltColors[ iCollector ];
                        flo32 ToAdd = lerp( 0.0f, t, 0.6f );
                        Color.r = MinValue( Color.r + ToAdd, 1.0f );
                        Color.g = MinValue( Color.g + ToAdd, 1.0f );
                        Color.b = MinValue( Color.b + ToAdd, 1.0f );
                        
                        flo32 Radians_Inner = Player->Collector_Radians[ iCollector ] + ( Radians * ( flo32 )iCollector );
                        
                        orect Bound = ORectCR( Pos, Collector_Radius, Radians_Inner );
                        DrawORect( Pass_Game, TextureID_Collector, Bound, Color * Transparent );
                        
                        flo32 Alph = lerp( 0.0f, t, 1.0f );
                        DrawORect( Pass_Game, TextureID_Collector_Flash, Bound, Color * Alph * Transparent );
                    }
                    
                    {
                        flo32 * Radians0      = Player->Collector_Anim_Radians     [ iCollector ];
                        flo32 * Timer0        = Player->Collector_Anim_Timer       [ iCollector ];
                        flo32 * Dist0         = Player->Collector_Anim_Dist        [ iCollector ];
                        boo32 * IsSterilized0 = Player->Collector_Anim_IsSterilized[ iCollector ];
                        
                        for( int32 iAnim = 0; iAnim < PLAYER_COLLECTOR_ANIM_MAX_COUNT; iAnim++ ) {
                            if( Timer0[ iAnim ] > 0.0f ) {
                                flo32 tAnim = Clamp01( Timer0[ iAnim ] / PLAYER_COLLECTOR_ANIM_TARGET_TIME );
                                flo32 Dist  = lerp( 0.1f, tAnim, Dist0[ iAnim ] );
                                vec2  Dir   = ToDirection2D( Radians0[ iAnim ] );
                                vec2  P     = Pos + Dir * Dist;
                                
                                flo32 Alph  = 1.0f;
                                flo32 Sca   = tAnim;
                                
                                SPIT Spit = SpitC( P, Vec2( 0.0f, 0.0f ), ( SPIT_TYPE )iCollector, IsSterilized0[ iAnim ] );
                                DrawSpit( Pass_Game, Spit, Alph, Sca );
                            }
                        }
                    }
                }
            }
        }
        
        { // Draw Shield Powerup
            if( Player->ShowShieldPowerup_Timer > 0.0f ) {
                flo32 Radius   = PLAYER_RADIUS * 1.2f;
                flo32 bRadians = Player->Facing_Radians * 2.0f;
                
                int32 nSegment        = 6;
                flo32 Segment_Radians = TAU / ( flo32 )nSegment;
                
                vec4 Color0[ 3 ] = {
                    COLOR_RED,
                    COLOR_BLUE,
                    COLOR_YELLOW,
                };
                
                flo32 Alph = 0.5f;
                {
                    flo32 CycleTime  = ( 15.0f / 60.0f );
                    int32 nCycle     = 6;
                    flo32 TargetTime = CycleTime * ( flo32 )nCycle;
                    
                    if( Player->ShowShieldPowerup_Timer <= TargetTime ) {
                        flo32 CycleHalfTime = CycleTime * 0.5f;
                        int32 Cycle = ( int32 )( Player->ShowShieldPowerup_Timer / CycleHalfTime );
                        if( ( Cycle % 2 ) == 0 ) {
                            Alph = 0.1f;
                        }
                    }
                }
                
                for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                    flo32 Radians = ( ( flo32 )iSegment * Segment_Radians ) + bRadians;
                    vec4  Color   = Color0[ iSegment % 3 ] * Alph;
                    DrawCircleSegment( Pass_Game, Player->Position, Radius, Radians, Segment_Radians, false, Color );
                }
            }
        }
        
        { // Draw Secondary Damage
            if( Player->SecondaryDamage_Timer > 0.0f ) {
                flo32 t       = Clamp01( Player->SecondaryDamage_Timer / PLAYER_SECONDARY_DAMAGE_TARGET_TIME );
                flo32 Radians = t * ( TAU * 5.5f );
                flo32 tAlpha  = cosf( Radians ) * 0.5f + 0.5f;
                flo32 Alph    = lerp( 0.25f, tAlpha, 0.5f );
                if( Player->SecondaryDamage_Timer < PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME ) {
                    Alph *= ( Player->SecondaryDamage_Timer / PLAYER_SECONDARY_DAMAGE_TOLERANCE_TARGET_TIME );
                }
                rect  Bound   = RectCR( Player->Position, 2.5f );
                
                DrawRect( Pass_Game, TextureID_Glow, Bound, COLOR_RED * Alph );
            }
        }
    }
}

internal void
DrawPlayerLastDamage( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player = Draw->Player;
    
    if( Player->LastDamage_Timer > 0.0f ) {
        flo32 t       = 1.0f - Clamp01( Player->LastDamage_Timer / PLAYER_LAST_DAMAGE_TARGET_TIME );
        flo32 Radians = lerp( -PI * 0.125f, t, PI * 4.75f );
        
        flo32 Alpha = 0.25f;
        flo32 TargetTime = ( 12.0f / 60.0f );
        if( Player->LastDamage_Timer <= TargetTime ) {
            Alpha *= Clamp01( Player->LastDamage_Timer / TargetTime );
        }
        
        switch( Player->LastDamage_Type ) {
            case EntityType_Turret: {
                TURRET * Turret = ( TURRET * )&Player->LastDamage_Buffer;
                Turret->Health       = 0;
                Turret->Damage_Timer = 0.0f;
                Turret->Fire_Timer   = 0.0f;
                
                Assert( Turret->Radius <= 4.0f );
                
                flo32 Sca0[ 4 ] = { 1.5f, 1.4f, 1.25f, 1.15f };
                flo32 Sca       = Sca0[ ( ( int32 )Turret->Radius - 1 ) ];
                
                if( Turret->Shield.HasShield ) {
                    Sca = 1.2f;
                }
                
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, Sca );
                DrawTurret( Pass, 0, 0, Turret[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Chaser: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.25f );
                
                CHASER * Chaser = ( CHASER * )&Player->LastDamage_Buffer;
                Chaser->Health = 0;
                
                DrawChaser( Pass, Chaser[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Swarm: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.5f );
                
                SWARM * Swarm = ( SWARM * )&Player->LastDamage_Buffer;
                Swarm->CloakT = 0.75f;
                
                DrawSwarm( Pass, Swarm[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Drifter: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.35f );
                
                DRIFTER * Drifter = ( DRIFTER * )&Player->LastDamage_Buffer;
                Drifter->Health = 0;
                
                DrawDrifter( Pass, Drifter[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Looper: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.35f );
                
                LOOPER * Looper = ( LOOPER * )&Player->LastDamage_Buffer;
                Looper->Health = 0;
                
                DrawLooper( Pass, Looper[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_EnemyBullet: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.5f );
                DrawEnemyBullet( Pass, ( ( ENEMY_BULLET * )&Player->LastDamage_Buffer )[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Asteroid: {
                ASTEROID * Asteroid = ( ASTEROID * )&Player->LastDamage_Buffer;
                Asteroid->Health             = 0;
                Asteroid->ZeroPoint_IsActive = false;
                Asteroid->ZeroPoint_ShowUI   = false;
                Asteroid->Damage_Timer       = 0.0f;
                
                if( !( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) {
                    Asteroid->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME * 0.125f;
                }
                
                flo32 Scale_Hi[ AsteroidClass_Count ] = { 1.4f, 1.3f, 1.2f, 1.2f };
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, Scale_Hi[ Asteroid->Class ] );
                
                Alpha *= 2.0f;
                
                DrawAsteroid( Pass, 0, 0, Asteroid[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Miner: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.3f );
                
                MINER * Miner = ( MINER * )&Player->LastDamage_Buffer;
                Miner->Health       = 0;
                Miner->Damage_Timer = 0.0f;
                Miner->Fire_Timer   = 0.0f;
                
                Alpha *= 1.5f;
                
                DrawMiner( Pass, 0, 0, Miner[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Bomber: {
                // do nothing
            } break;
            
            case EntityType_Bomb: {
                BOMB * Bomb = ( BOMB * )&Player->LastDamage_Buffer;
                Bomb->Timer = 0.0f;
                
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.1f );
                DrawBomb( Pass, Bomb[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_BombLauncher: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.5f );
                
                BOMB_LAUNCHER * BombLauncher = ( BOMB_LAUNCHER * )&Player->LastDamage_Buffer;
                BombLauncher->Health       = 0;
                BombLauncher->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME * 0.5f;
                
                DrawBombLauncher( Pass, 0, 0, BombLauncher[ 0 ], false, Alpha, Scale );
            } break;
            
            case EntityType_Blimp: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.25f );
                
                BLIMP * Blimp = ( BLIMP * )&Player->LastDamage_Buffer;
                Blimp->Health       = 0;
                Blimp->Damage_Timer = 0.0f;
                
                DrawBlimp( Pass, 0, 0, Blimp[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Yacht: {
                YACHT * Yacht = ( YACHT * )&Player->LastDamage_Buffer;
                Yacht->Health           = 0;
                Yacht->ShowDamage_Timer = 0.0f;
                Yacht->Exit_Timer       = 0.0f;
                Yacht->ColorT           = 1.0f;
                
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.25f );
                DrawYacht( Pass, 0, 0, Yacht[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_MegaTurret: {
                TURRET * Turret = ( TURRET * )&Player->LastDamage_Buffer;
                Turret->Health       = 0;
                Turret->Damage_Timer = 0.0f;
                Turret->Fire_Timer   = 0.0f;
                
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.2f );
                DrawMegaTurret( Pass, 0, 0, Turret[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_MegaMiner: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.25f );
                
                MEGA_MINER * MegaMiner = ( MEGA_MINER * )&Player->LastDamage_Buffer;
                MegaMiner->Health           = 0;
                MegaMiner->ShowDamage_Timer = 0.0f;
                
                DrawMegaMiner( Pass, 0, 0, MegaMiner[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Escort: {
                // do nothing
            } break;
            
            case EntityType_Rogue: {
                // do nothing
            } break;
            
            case EntityType_Missile: {
                MISSILE * Missile = ( MISSILE * )&Player->LastDamage_Buffer;
                Missile->Exhaust_Timer = 0.0f;
                
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.35f );
                DrawMissile( Pass, Missile[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Worm: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.25f );
                
                WORM * Worm = ( WORM * )&Player->LastDamage_Buffer;
                Worm->Health           = 0;
                Worm->ShowDamage_Timer = 0.0f;
                Worm->DoAttack_Timer   = 0.0f;
                Worm->Spawn_Length     = Worm->Spawn_MaxLength;
                
                Alpha *= 1.35f;
                
                DrawWormHead( Pass, 0, 0, Worm[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Fireball: {
                FIREBALL * Fireball = ( FIREBALL * )&Player->LastDamage_Buffer;
                Fireball->Health = 0;
                
                flo32 Sca = 1.35f;
                if( Fireball->Radius == FIREBALL_RADIUS_LARGE ) {
                    Sca = 1.25f;
                }
                
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, Sca );
                DrawFireball( Pass, 0, 0, Fireball[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Laser: {
                // do nothing
            } break;
            
            case EntityType_Spit: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.5f );
                
                SPIT * Spit = ( SPIT * )&Player->LastDamage_Buffer;
                Spit->Health = 0;
                
                DrawSpit( Pass, Spit[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_Loader: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.4f );
                
                LOADER * Loader = ( LOADER * )&Player->LastDamage_Buffer;
                Loader->Health                = 0;
                Loader->Damage_Cooldown_Timer = MaxValue( Loader->Damage_Cooldown_Timer, 0.0f );
                
                Alpha *= 1.5f;
                
                DrawLoader( Pass, 0, 0, Loader[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_DamageZone: {
                // do nothing
            } break;
            
            case EntityType_SecurityDrone: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.35f );
                
                SECURITY_DRONE * SecurityDrone = ( SECURITY_DRONE * )&Player->LastDamage_Buffer;
                SecurityDrone->Health             = 0;
                SecurityDrone->Damage_Timer       = 0.0f;
                SecurityDrone->Fire_Timer         = 0.0f;
                SecurityDrone->Boost_TargetTime   = FLT_MAX;
                SecurityDrone->Boost_Timer        = 0.0f;
                SecurityDrone->ZeroPoint_IsActive = false;
                SecurityDrone->ZeroPoint_ShowUI   = false;
                
                DrawSecurityDrone( Pass, 0, 0, SecurityDrone[ 0 ], false, Alpha, Scale );
            } break;
            
#if 0            
            case EntityType_Bolt: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.5f );
                DrawBolt( Pass, ( ( BOLT * )&Player->LastDamage_Buffer )[ 0 ], Alpha, Scale );
            } break;
#endif
            
            case EntityType_Gunner: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.5f );
                
                GUNNER * Gunner = ( GUNNER * )&Player->LastDamage_Buffer;
                Gunner->Health       = 0;
                Gunner->Fire_Timer   = 0.0f;
                Gunner->Damage_Timer = 0.0f;
                
                DrawGunner( Pass, 0, 0, Gunner[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_MissileLauncher: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.5f );
                
                MISSILE_LAUNCHER * MissileLauncher = ( MISSILE_LAUNCHER * )&Player->LastDamage_Buffer;
                MissileLauncher->Health       = 0;
                MissileLauncher->Damage_Timer = 0.0f;
                
                Alpha *= 1.5f;
                
                DrawMissileLauncher( Pass, 0, 0, MissileLauncher[ 0 ], Alpha, Scale );
            } break;
            
            case EntityType_ObservationDeckBarrier: {
                flo32 Scale = lerp( 1.0f, cosf( Radians ) * 0.5f + 0.5f, 1.075f );
                
                OBSERVATION_DECK_BARRIER * ObservationDeckBarrier = ( OBSERVATION_DECK_BARRIER * )&Player->LastDamage_Buffer;
                
                DrawObservationDeckBarrier( Pass, ObservationDeckBarrier[ 0 ], Alpha, Scale );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
    }
}

internal void
FinalizePlayer( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    if( !Player->IsDead ) {
        if( Player->Health <= 0 ) {
            Player->IsDead       = true;
            Player->IsDead_Timer = 0.0f;
            
            Player->ZeroPoint_IsActive        = false;
            Player->ZeroPoint_ShowUI          = false;
            Player->ZeroPoint_DrawBeam        = false;
            Player->ZeroPoint_IsAsteroid      = false;
            Player->ZeroPoint_IsSecurityDrone = false;
            
            Player->Shield_IsActive      = false;
            Player->TractorBeam_IsActive = false;
            Player->Collector_IsActive   = false;
            
            POWERUP_STATE * PowerupS = &AppState->PowerupS;
            PowerupS->Spawn_IsActive = false;
            
            ClearActiveWaveArrayToEndLevel( AppState );
        }
    }
}