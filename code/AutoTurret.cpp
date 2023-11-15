
internal AUTO_TURRET
AutoTurretC( int32 WaveGuid, vec2 Position ) {
    AUTO_TURRET Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Radians  = 0.0f;
    
    return Result;
}

internal void
AddAutoTurret( APP_STATE * AppState, AUTO_TURRET AutoTurret ) {
    AUTO_TURRET_STATE * AutoTurretS = &AppState->AutoTurretS;
    if( AutoTurretS->nAutoTurret < AUTO_TURRET_MAX_COUNT ) {
        AutoTurretS->AutoTurret[ AutoTurretS->nAutoTurret++ ] = AutoTurret;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add AUTO_TURRET to AUTO_TURRET_STATE: %u", AUTO_TURRET_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateAutoTurret( APP_STATE * AppState, flo32 dT ) {
    AUTO_TURRET_STATE * AutoTurretS = &AppState->AutoTurretS;
    PLAYER_STATE      * Player      = &AppState->Player;
    TURRET_STATE      * TurretS     = &AppState->TurretS;
    CHASER_STATE      * ChaserS     = &AppState->ChaserS;
    SWARM_STATE       * SwarmS      = &AppState->SwarmS;
    BAY_DOOR_STATE    * BayDoorS    = &AppState->BayDoorS;
    
    AutoTurretS->Powerup_Timer = MaxValue( AutoTurretS->Powerup_Timer - dT, 0.0f );
    
    
    { // Check if Turret damage-boost AutoTurret
        for( uint32 iAutoTurret = 0; iAutoTurret < AutoTurretS->nAutoTurret; iAutoTurret++ ) {
            AUTO_TURRET * AutoTurret = AutoTurretS->AutoTurret + iAutoTurret;
            
            boo32 DoDamage     = false;
            boo32 DoDrop       = false;
            vec2  DoDamage_Pos = {};
            
            
#if 0            
            { // TURRET
                for( int32 iTurret = 0; ( !DoDamage ) && ( iTurret < TurretS->nTurret ); iTurret++ ) {
                    TURRET Turret = TurretS->Turret[ iTurret ];
                    
                    {
                        flo32 Radius   = ( AUTO_TURRET_RADIUS + Turret.Radius - 0.1f );
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( AutoTurret->Position - Turret.Position );
                        
                        if( DistSq <= RadiusSq ) {
                            DoDamage     = true;
                            DoDamage_Pos = Turret.Position;
                        }
                    }
                    
                    ENTITY_SHIELD Shield = Turret.Shield;
                    if( ( !DoDamage ) && ( Shield.HasShield ) ) {
                        { // Shield Arc
                            flo32 Radius_Inner = Turret.Radius + Shield.Dist - AUTO_TURRET_RADIUS + 0.1f;
                            Radius_Inner = MaxValue( Radius_Inner, 0.0f );
                            
                            flo32 Radius_Outer = Turret.Radius + Shield.Dist + Shield.Width + AUTO_TURRET_RADIUS - 0.1f;
                            
                            flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
                            flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
                            flo32 DistSq         = GetLengthSq( AutoTurret->Position - Turret.Position );
                            if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                                flo32 MinRadians = Shield.bRadians;
                                flo32 MaxRadians = MinRadians + Shield.Radians;
                                
                                flo32 InRadians = ToRadians( AutoTurret->Position - Turret.Position );
                                if( InRadians < MinRadians ) {
                                    InRadians += TAU;
                                }
                                
                                if( ( InRadians >= MinRadians ) && ( InRadians <= MaxRadians ) ) {
                                    memset( Turret.Shield_SegmentT, 0, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
                                    
                                    flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
                                    int32 iSegment   = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t );
                                    int32 MinSegment = MaxValue( iSegment - 1, 0 );
                                    
                                    for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                        int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
                                        Turret.Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
                                    }
                                    
                                    DoDamage     = true;
                                    DoDamage_Pos = Turret.Position;
                                }
                            }
                        }
                        
                        if( !DoDamage ) { // End Capsules
                            flo32 Radius = AUTO_TURRET_RADIUS - 0.1f;
                            vec2 DirA = ToDirection2D( Shield.bRadians                  );
                            vec2 DirB = ToDirection2D( Shield.bRadians + Shield.Radians );
                            
                            flo32 OffsetA = Turret.Radius + Shield.Dist;
                            flo32 OffsetB = OffsetA + Shield.Width;
                            
                            CAPSULE2 CapA = Capsule2( Turret.Position + DirA * OffsetA, Turret.Position + DirA * OffsetB, Radius );
                            CAPSULE2 CapB = Capsule2( Turret.Position + DirB * OffsetA, Turret.Position + DirB * OffsetB, Radius );
                            
                            boo32 IsInCapsuleA = IsInCapsule( AutoTurret->Position, CapA );
                            boo32 IsInCapsuleB = IsInCapsule( AutoTurret->Position, CapB );
                            if( ( IsInCapsuleA ) || ( IsInCapsuleB ) ) {
                                memset( Turret.Shield_SegmentT, 0, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
                                
                                int32 iSegment = 0;
                                if( IsInCapsuleB ) {
                                    flo32 t = Clamp01( Shield.Radians / TAU );
                                    iSegment = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t ) - 1;
                                }
                                
                                int32 MinSegment = MaxValue( iSegment - 1, 0 );
                                
                                for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                    int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
                                    Turret.Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
                                }
                                
                                DoDamage     = true;
                                DoDamage_Pos = Turret.Position;
                            }
                        }
                    }
                }
            }
#endif
            
#if 0            
            { // MINIBOSS
                for( int32 iMiniBoss = 0; ( !DoDamage ) && ( iMiniBoss < BayDoorS->nMiniBoss ); iMiniBoss++ ) {
                    MINIBOSS * MiniBoss = BayDoorS->MiniBoss + iMiniBoss;
                    
                    {
                        flo32 Radius   = ( AUTO_TURRET_RADIUS + MINIBOSS_RADIUS - 0.1f );
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( AutoTurret->Position - MiniBoss->Position );
                        
                        if( DistSq <= RadiusSq ) {
                            DoDamage     = true;
                            DoDamage_Pos = MiniBoss->Position;
                            
                            vec2 Dir = GetNormal( AutoTurret->Position - MiniBoss->Position );
                            MiniBoss->Velocity = Dir * -MINIBOSS_DAMAGE_BOOST_SPEED;
                            
                            if( MiniBoss->iTarget == ( int32 )iAutoTurret ) {
                                MiniBoss->iTarget_Timer += 1.5f;
                            }
                        }
                    }
                    
                    ENTITY_SHIELD Shield = MiniBoss->Shield;
                    if( ( !DoDamage ) && ( Shield.HasShield ) ) {
                        { // Shield Arc
                            flo32 Radius_Inner = MINIBOSS_RADIUS + Shield.Dist - AUTO_TURRET_RADIUS + 0.1f;
                            Radius_Inner = MaxValue( Radius_Inner, 0.0f );
                            
                            flo32 Radius_Outer = MINIBOSS_RADIUS + Shield.Dist + Shield.Width + AUTO_TURRET_RADIUS - 0.1f;
                            
                            flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
                            flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
                            flo32 DistSq         = GetLengthSq( AutoTurret->Position - MiniBoss->Position );
                            if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                                flo32 MinRadians = Shield.bRadians;
                                flo32 MaxRadians = MinRadians + Shield.Radians;
                                
                                flo32 InRadians = ToRadians( AutoTurret->Position - MiniBoss->Position );
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
                                    
                                    DoDamage     = true;
                                    DoDamage_Pos = MiniBoss->Position;
                                    
                                    vec2 Dir = GetNormal( AutoTurret->Position - MiniBoss->Position );
                                    MiniBoss->Velocity = Dir * -MINIBOSS_DAMAGE_BOOST_SPEED;
                                    
                                    if( MiniBoss->iTarget == ( int32 )iAutoTurret ) {
                                        MiniBoss->iTarget_Timer += 1.5f;
                                    }
                                }
                            }
                        }
                        
                        if( !DoDamage ) { // End Capsules
                            flo32 Radius = AUTO_TURRET_RADIUS - 0.1f;
                            vec2 DirA = ToDirection2D( Shield.bRadians                  );
                            vec2 DirB = ToDirection2D( Shield.bRadians + Shield.Radians );
                            
                            flo32 OffsetA = MINIBOSS_RADIUS + Shield.Dist;
                            flo32 OffsetB = OffsetA + Shield.Width;
                            
                            CAPSULE2 CapA = Capsule2( MiniBoss->Position + DirA * OffsetA, MiniBoss->Position + DirA * OffsetB, Radius );
                            CAPSULE2 CapB = Capsule2( MiniBoss->Position + DirB * OffsetA, MiniBoss->Position + DirB * OffsetB, Radius );
                            
                            boo32 IsInCapsuleA = IsInCapsule( AutoTurret->Position, CapA );
                            boo32 IsInCapsuleB = IsInCapsule( AutoTurret->Position, CapB );
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
                                
                                DoDamage     = true;
                                DoDamage_Pos = MiniBoss->Position;
                                
                                vec2 Dir = GetNormal( AutoTurret->Position - MiniBoss->Position );
                                MiniBoss->Velocity = Dir * -MINIBOSS_DAMAGE_BOOST_SPEED;
                                
                                if( MiniBoss->iTarget == ( int32 )iAutoTurret ) {
                                    MiniBoss->iTarget_Timer += 1.5f;
                                }
                            }
                        }
                    }
                }
            }
#endif
            
            { // YACHT
                for( int32 iYacht = 0; ( !DoDamage ) && ( iYacht < BayDoorS->nYacht ); iYacht++ ) {
                    YACHT * Yacht = BayDoorS->Yacht + iYacht;
                    
                    if( ( Yacht->Health > 0 ) && ( Yacht->Damage_IFrameTimer <= 0.0f ) ) {
                        flo32 Radius   = ( AUTO_TURRET_RADIUS + YACHT_RADIUS - 0.1f );
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( AutoTurret->Position - Yacht->Position );
                        
                        if( DistSq <= RadiusSq ) {
                            DoDamage     = true;
                            DoDrop       = false;
                            DoDamage_Pos = Yacht->Position;
                            
                            vec2 Dir = GetNormal( AutoTurret->Position - Yacht->Position );
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
            
            if( DoDamage ) {
                AutoTurret->ShowDamage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                
                if( ( AutoTurret->ZeroPoint_IsActive ) && ( DoDrop ) ) {
                    AutoTurret->ZeroPoint_IsActive = false;
                    Player->ZeroPoint_IsActive     = false;
                }
                
                if( AutoTurret->ZeroPoint_IsActive ) {
                    Player->DoDamage_Velocity = GetNormal( AutoTurret->Position - DoDamage_Pos ) * PLAYER_DAMAGE_BOOST_SPEED;
                    Player->DoDamage_Timer    = PLAYER_DAMAGE_BOOST_TARGET_TIME;
                } else {
                    AutoTurret->Velocity = GetNormal( AutoTurret->Position - DoDamage_Pos ) * AUTO_TURRET_DAMAGE_BOOST_SPEED;
                    AutoTurret->Radians            = RandomF32() * TAU;
                    AutoTurret->Fire_Anim_Radians  = AutoTurret->Radians;
                }
            }
        }
    }
    
    for( uint32 iAutoTurret = 0; iAutoTurret < AutoTurretS->nAutoTurret; iAutoTurret++ ) {
        AUTO_TURRET * AutoTurret = AutoTurretS->AutoTurret + iAutoTurret;
        
        AutoTurret->DrawCollision    = false;
        AutoTurret->ShowDamage_Timer = MaxValue( AutoTurret->ShowDamage_Timer - dT, 0.0f );
        
        if( !Player->ZeroPoint_IsActive ) {
            AutoTurret->ZeroPoint_IsActive = false;
        }
        if( Player->IsDead ) {
            AutoTurret->ZeroPoint_ShowUI   = false;
        }
        
        for( uint32 iRepel = iAutoTurret + 1; iRepel < AutoTurretS->nAutoTurret; iRepel++ ) {
            AUTO_TURRET * Repel = AutoTurretS->AutoTurret + iRepel;
            
            vec2 V = Repel->Position - AutoTurret->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = AUTO_TURRET_REPEL_RADIUS + AUTO_TURRET_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * AUTO_TURRET_REPEL_SPEED;
                
                if( !AutoTurret->ZeroPoint_IsActive ) {
                    AutoTurret->Repel -= Repel_Vector;
                }
                if( !Repel->ZeroPoint_IsActive ) {
                    Repel->Repel  += Repel_Vector;
                }
            }
        }
        
        {
            flo32 Radius   = AUTO_TURRET_ZERO_POINT_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            flo32 DistSq   = GetLengthSq( Player->Position - AutoTurret->Position );
            AutoTurret->FireRange_ShowUI = ( DistSq <= RadiusSq );
        }
        
        {
            if( !AutoTurret->ZeroPoint_IsActive ) {
                vec2 Accel = ( -AutoTurret->Velocity + AutoTurret->Repel ) * AUTO_TURRET_FRICTION;
                
                vec2 dPos = Accel * ( 0.5f * dT * dT ) + AutoTurret->Velocity * dT;
                // TODO: Collision!
                AutoTurret->Position += dPos;
                AutoTurret->Velocity += Accel * dT;
                AutoTurret->Repel = {};
                
                AutoTurret->Position = ClampToRect( AutoTurret->Position, AppState->World_Bound );
            }
        }
        
        if( AutoTurret->ZeroPoint_IsActive ) {
            AutoTurret->Velocity = {};
            AutoTurret->Repel    = {};
            
            AutoTurret->Radians = WrapRadians( AutoTurret->Radians - ( Player->Rotate_Speed * dT ) );
            AutoTurret->Fire_Anim_Radians = WrapRadians( AutoTurret->Fire_Anim_Radians - ( Player->Rotate_Speed * dT ) );
            
            AutoTurret->ZeroPoint_Radians = WrapRadians( AutoTurret->ZeroPoint_Radians - ( Player->Rotate_Speed * dT ) );
            
            AutoTurret->Position = Player->Position + ToDirection2D( AutoTurret->ZeroPoint_Radians ) * AutoTurret->ZeroPoint_Dist;
        }
        
        boo32       CanFire           = false;
        boo32       HasTarget         = false;
        ENTITY_TYPE HasTarget_Type    = {};
        int32       HasTarget_iEntity = -1;
        
        AutoTurret->Fire_Timer -= dT;
        //if( ( !AutoTurret->ZeroPoint_IsActive ) && ( AutoTurret->Fire_Timer <= 0.0f ) ) {
        //CanFire = true;
        //}
        if( AutoTurret->Fire_Timer <= 0.0f ) {
            CanFire = true;
        }
        if( AutoTurret->Fire_Timer <= -dT ) {
            AutoTurret->Fire_Timer = 0.0f;
        }
        
        AutoTurret->Fire_Anim_Timer = MaxValue( AutoTurret->Fire_Anim_Timer - dT, 0.0f );
        
        vec2 A = AutoTurret->Position;
        vec2 B = {};
        
        if( CanFire ) {
            flo32 MinDist   = AUTO_TURRET_FIRE_RADIUS;
            flo32 HalfRange = AUTO_TURRET_FIRE_RADIANS_HALF_RANGE;
            if( AutoTurretS->Powerup_Timer > 0.0f ) {
                MinDist   *= AutoTurretS->Powerup_Rate;
                HalfRange *= AutoTurretS->Powerup_Rate;
            }
            
            flo32 MinDistSq = MinDist * MinDist;
            
            { // TURRET
                for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
                    TURRET * Turret = TurretS->Turret + iTurret;
                    
                    if( Turret->Health > 0 ) {
                        vec2  Dir     = Turret->Position - AutoTurret->Position;
                        flo32 Radians = ToRadians( Dir );
                        
                        flo32 Diff = GetSmallestDiffInRadians( AutoTurret->Radians, Radians );
                        if( fabsf( Diff ) <= HalfRange ) {
                            flo32 DistSq = GetLengthSq( Dir );
                            if( DistSq < MinDistSq ) {
                                MinDistSq = DistSq;
                                
                                HasTarget         = true;
                                HasTarget_Type    = EntityType_Turret;
                                HasTarget_iEntity = iTurret;
                                
                                B = Turret->Position;
                            }
                        }
                    }
                }
            }
            
            { // CHASER
                for( int32 iChaser = 0; iChaser < ChaserS->nChaser; iChaser++ ) {
                    CHASER * Chaser = ChaserS->Chaser + iChaser;
                    
                    if( Chaser->Health > 0 ) {
                        vec2  Dir     = Chaser->Position - AutoTurret->Position;
                        flo32 Radians = ToRadians( Dir );
                        
                        flo32 Diff = GetSmallestDiffInRadians( AutoTurret->Radians, Radians );
                        if( fabsf( Diff ) <= HalfRange ) {
                            flo32 DistSq = GetLengthSq( Dir );
                            if( DistSq < MinDistSq ) {
                                MinDistSq = DistSq;
                                
                                HasTarget         = true;
                                HasTarget_Type    = EntityType_Chaser;
                                HasTarget_iEntity = iChaser;
                                
                                B = Chaser->Position;
                            }
                        }
                    }
                }
            }
            
            { // SWARM
                for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
                    SWARM * Swarm = SwarmS->Swarm + iSwarm;
                    
                    if( Swarm->Health > 0 ) {
                        vec2  Dir     = Swarm->Position - AutoTurret->Position;
                        flo32 Radians = ToRadians( Dir );
                        
                        flo32 Diff = GetSmallestDiffInRadians( AutoTurret->Radians, Radians );
                        if( fabsf( Diff ) <= HalfRange ) {
                            flo32 DistSq = GetLengthSq( Dir );
                            if( DistSq < MinDistSq ) {
                                MinDistSq = DistSq;
                                
                                HasTarget         = true;
                                HasTarget_Type    = EntityType_Swarm;
                                HasTarget_iEntity = iSwarm;
                                
                                B = Swarm->Position;
                            }
                        }
                    }
                }
            }
            
            { // PERSONAL_POD
                for( int32 iPersonalPod = 0; iPersonalPod < BayDoorS->nPersonalPod; iPersonalPod++ ) {
                    PERSONAL_POD * PersonalPod = BayDoorS->PersonalPod + iPersonalPod;
                    
                    if( PersonalPod->Health > 0 ) {
                        vec2  Dir     = PersonalPod->Position - AutoTurret->Position;
                        flo32 Radians = ToRadians( Dir );
                        
                        flo32 Diff = GetSmallestDiffInRadians( AutoTurret->Radians, Radians );
                        if( fabsf( Diff ) <= HalfRange ) {
                            flo32 DistSq = GetLengthSq( Dir );
                            if( DistSq < MinDistSq ) {
                                MinDistSq = DistSq;
                                
                                HasTarget         = true;
                                HasTarget_Type    = EntityType_PersonalPod;
                                HasTarget_iEntity = iPersonalPod;
                                
                                B = PersonalPod->Position;
                            }
                        }
                    }
                }
            }
            
            { // TRANSPORT
                for( int32 iTransport = 0; iTransport < BayDoorS->nTransport; iTransport++ ) {
                    TRANSPORT * Transport = BayDoorS->Transport + iTransport;
                    
                    if( Transport->Health > 0 ) {
                        vec2  Dir     = Transport->Position - AutoTurret->Position;
                        flo32 Radians = ToRadians( Dir );
                        
                        flo32 Diff = GetSmallestDiffInRadians( AutoTurret->Radians, Radians );
                        if( fabsf( Diff ) <= HalfRange ) {
                            flo32 DistSq = GetLengthSq( Dir );
                            if( DistSq < MinDistSq ) {
                                MinDistSq = DistSq;
                                
                                HasTarget         = true;
                                HasTarget_Type    = EntityType_Transport;
                                HasTarget_iEntity = iTransport;
                                
                                B = Transport->Position;
                            }
                        }
                    }
                }
            }
            
#if 0            
            { // MINIBOSS
                for( int32 iMiniBoss = 0; iMiniBoss < BayDoorS->nMiniBoss; iMiniBoss++ ) {
                    MINIBOSS * MiniBoss = BayDoorS->MiniBoss + iMiniBoss;
                    
                    if( MiniBoss->Health > 0 ) {
                        vec2  Dir     = MiniBoss->Position - AutoTurret->Position;
                        flo32 Radians = ToRadians( Dir );
                        
                        flo32 Diff = GetSmallestDiffInRadians( AutoTurret->Radians, Radians );
                        if( fabsf( Diff ) <= HalfRange ) {
                            flo32 DistSq = GetLengthSq( Dir );
                            if( DistSq < MinDistSq ) {
                                MinDistSq = DistSq;
                                
                                HasTarget         = true;
                                HasTarget_Type    = EntityType_MiniBoss;
                                HasTarget_iEntity = iMiniBoss;
                                
                                B = MiniBoss->Position;
                            }
                        }
                    }
                }
            }
#endif
        }
        
        if( ( CanFire ) && ( HasTarget ) ) {
            {
                flo32 TargetTime = AUTO_TURRET_FIRE_COOLDOWN_TARGET_TIME;
                if( AutoTurretS->Powerup_Timer > 0.0f ) {
                    TargetTime /= AutoTurretS->Powerup_Rate;
                }
                AutoTurret->Fire_Timer     += TargetTime;
                AutoTurret->Fire_Anim_Timer = AUTO_TURRET_FIRE_ANIM_TARGET_TIME;
            }
            
            boo32 HitIsValid = true;
            
#if 0            
            { // Is Shield blocking the auto-turret laser?
                for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
                    TURRET * Turret = TurretS->Turret + iTurret;
                    
                    if( Turret->Health > 0 ) {
                        ENTITY_SHIELD Shield = Turret->Shield;
                        if( Shield.HasShield ) {
                            flo32 Radius = Turret->Radius + Shield.Dist + Shield.Width - 0.1f;
                            
                            // TODO: Check collision of inner radius and end capsules?
                            
                            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                            DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Turret->Position, Radius ) );
                            if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                                flo32 MinRadians = Shield.bRadians;
                                flo32 MaxRadians = MinRadians + Shield.Radians;
                                
                                flo32 InRadians = ToRadians( BestIntersect.P - Turret->Position );
                                if( InRadians < MinRadians ) {
                                    InRadians += TAU;
                                }
                                
                                if( ( InRadians >= MinRadians ) && ( InRadians <= MaxRadians ) ) {
                                    // TODO: Get closest chaser for shield hit
                                    flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
                                    int32 iSegment   = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t );
                                    int32 MinSegment = MaxValue( iSegment - 1, 0 );
                                    
                                    for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                        int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
                                        Turret->Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
                                    }
                                    
                                    AutoTurret->Fire_Anim_Position = BestIntersect.P;
                                    AutoTurret->Fire_Anim_Radians  = ToRadians( BestIntersect.P - AutoTurret->Position );
                                    
                                    HitIsValid = false;
                                }
                            }
                        }
                    }
                }
                
#if 0                
                for( int32 iMiniBoss = 0; iMiniBoss < BayDoorS->nMiniBoss; iMiniBoss++ ) {
                    MINIBOSS * MiniBoss = BayDoorS->MiniBoss + iMiniBoss;
                    
                    if( MiniBoss->Health > 0 ) {
                        ENTITY_SHIELD Shield = MiniBoss->Shield;
                        if( Shield.HasShield ) {
                            flo32 Radius = MINIBOSS_RADIUS + Shield.Dist + Shield.Width - 0.1f;
                            
                            // TODO: Check collision of inner radius and end capsules?
                            
                            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                            DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( MiniBoss->Position, Radius ) );
                            if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                                flo32 MinRadians = Shield.bRadians;
                                flo32 MaxRadians = MinRadians + Shield.Radians;
                                
                                flo32 InRadians = ToRadians( BestIntersect.P - MiniBoss->Position );
                                if( InRadians < MinRadians ) {
                                    InRadians += TAU;
                                }
                                
                                if( ( InRadians >= MinRadians ) && ( InRadians <= MaxRadians ) ) {
                                    // TODO: Get closest chaser for shield hit
                                    flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
                                    int32 iSegment   = ( int32 )( ( flo32 )MINIBOSS_SHIELD_SEGMENT_MAX_COUNT * t );
                                    int32 MinSegment = MaxValue( iSegment - 1, 0 );
                                    
                                    for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                                        int32 iSeg = ( MinSegment + Iter ) % MINIBOSS_SHIELD_SEGMENT_MAX_COUNT; 
                                        MiniBoss->Shield_SegmentT[ iSeg ] = MINIBOSS_SHIELD_MAX_T;
                                    }
                                    
                                    AutoTurret->Fire_Anim_Position = BestIntersect.P;
                                    AutoTurret->Fire_Anim_Radians  = ToRadians( BestIntersect.P - AutoTurret->Position );
                                    
                                    HitIsValid = false;
                                }
                            }
                        }
                    }
                }
#endif
            }
#endif
            
            if( HitIsValid ) {
                switch( HasTarget_Type ) {
#if 0
                    case EntityType_Turret: {
                        TURRET * Turret = TurretS->Turret + HasTarget_iEntity;
                        
                        int32 Turret_Damage = MinValue( Turret->Health, 1 );
                        Turret->Health -= Turret_Damage;
                        
                        AutoTurret->Fire_Anim_Position = Turret->Position;
                        AutoTurret->Fire_Anim_Radians  = ToRadians( Turret->Position - AutoTurret->Position );
                    } break;
                    
                    case EntityType_Chaser: {
                        CHASER * Chaser = ChaserS->Chaser + HasTarget_iEntity;
                        
                        int32 Chaser_Damage = MinValue( Chaser->Health, 1 );
                        Chaser->Health -= Chaser_Damage;
                        
                        AutoTurret->Fire_Anim_Position = Chaser->Position;
                        AutoTurret->Fire_Anim_Radians  = ToRadians( Chaser->Position - AutoTurret->Position );
                    } break;
                    
                    case EntityType_Swarm: {
                        SWARM * Swarm = SwarmS->Swarm + HasTarget_iEntity;
                        
                        int32 Swarm_Damage = MinValue( Swarm->Health, 1 );
                        Swarm->Health -= Swarm_Damage;
                        
                        AutoTurret->Fire_Anim_Position = Swarm->Position;
                        AutoTurret->Fire_Anim_Radians  = ToRadians( Swarm->Position - AutoTurret->Position );
                    } break;
#endif
                    
                    case EntityType_PersonalPod: {
                        PERSONAL_POD * PersonalPod = BayDoorS->PersonalPod + HasTarget_iEntity;
                        
                        int32 PersonalPod_Damage = MinValue( PersonalPod->Health, 1 );
                        PersonalPod->Health -= PersonalPod_Damage;
                        
                        AddPersonalPodDeathParticles( AppState, PersonalPod->Velocity, PersonalPod->Position, GetNormal( PersonalPod->Position - AutoTurret->Position ) );
                        
                        AutoTurret->Fire_Anim_Position = PersonalPod->Position;
                        AutoTurret->Fire_Anim_Radians  = ToRadians( PersonalPod->Position - AutoTurret->Position );
                    } break;
                    
                    case EntityType_Transport: {
                        TRANSPORT * Transport = BayDoorS->Transport + HasTarget_iEntity;
                        
                        int32 Transport_Damage = MinValue( Transport->Health, 1 );
                        Transport->Health -= Transport_Damage;
                        
                        vec2 N = GetNormal( AutoTurret->Position - Transport->Position );
                        vec2 P = Transport->Position + N * ( Transport->Radius * lerp( 0.8f, RandomF32(), 1.0f ) );
                        
                        AutoTurret->Fire_Anim_Position = P;
                        AutoTurret->Fire_Anim_Radians  = ToRadians( Transport->Position - AutoTurret->Position );
                        
                        AutoTurret->DrawCollision   = true;
                        AutoTurret->DrawCollision_P = P;
                        AutoTurret->DrawCollision_N = N;
                    } break;
                    
#if 0                    
                    case EntityType_MiniBoss: {
                        MINIBOSS * MiniBoss = BayDoorS->MiniBoss + HasTarget_iEntity;
                        
                        int32 MiniBoss_Damage = MinValue( MiniBoss->Health, 1 );
                        MiniBoss->Health -= MiniBoss_Damage;
                        
                        AutoTurret->Fire_Anim_Position = MiniBoss->Position;
                        AutoTurret->Fire_Anim_Radians  = ToRadians( MiniBoss->Position - AutoTurret->Position );
                    } break;
#endif
                    
                    default: {
                        InvalidCodePath;
                    } break;
                }
            }
        }
    }
}

internal void
DrawAutoTurret( RENDER_PASS * Pass, AUTO_TURRET AutoTurret, flo32 Powerup_Rate = 1.0f, flo32 Scale = 1.0f, flo32 Alpha = 1.0f, boo32 ShowFireRange = true ) {
    if( AutoTurret.ZeroPoint_IsActive ) {
        vec4  Color  = PLAYER_GADGET_BASE_COLOR;
        flo32 Radius = ( AUTO_TURRET_RADIUS + 0.1f ) * 1.125f;
        
        rect Bound = RectCR( AutoTurret.Position, Radius );
        DrawRect( Pass, TextureID_ZeroPoint_Base_Active, Bound, Color );
    } else if( AutoTurret.ZeroPoint_ShowUI ) {
        vec4  Color  = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.5f );
        flo32 Radius = ( AUTO_TURRET_RADIUS + 0.1f ) * 1.0625f;
        
        rect Bound = RectCR( AutoTurret.Position, Radius );
        DrawRect( Pass, TextureID_ZeroPoint_Base_Inactive, Bound, Color );
    }
    
    if( ShowFireRange ) {
        vec4  ColorA  = Vec4( 0.0f, 0.035f, 0.0f, 0.035f );
        
        if( ( AutoTurret.ZeroPoint_ShowUI ) || ( AutoTurret.ZeroPoint_IsActive ) ) {
            ColorA = Vec4( 0.0f, 0.085f, 0.0f, 0.085f );
        }
        
        flo32 HalfRange = AUTO_TURRET_FIRE_RADIANS_HALF_RANGE * Powerup_Rate;
        flo32 Radius    = AUTO_TURRET_FIRE_RADIUS             * Powerup_Rate;
        
        flo32 Radians = AutoTurret.Radians - HalfRange;
        DrawCircleSegment( Pass, AutoTurret.Position, Radius, Radians, HalfRange * 2.0f, false, ColorA );
    }
    
    vec4 bColorA = AUTO_TURRET_BASE_COLOR;
    vec4 bColorB = MINER_AUTONOMOUS_BASE_COLOR;
    
    vec4 ColorA = Vec4( 0.0f, 0.25f, 0.1f, 1.0f );
    vec4 ColorB = Vec4( 0.0f, 0.35f, 0.1f, 1.0f );
    vec4 ColorC = Vec4( 0.25f, 0.15f, 0.0f, 1.0f );
    
    //DrawCircle( Pass, AutoTurret.Position, AUTO_TURRET_RADIUS,        ColorA );
    //DrawCircle( Pass, AutoTurret.Position, AUTO_TURRET_RADIUS * 0.4f, ColorB );
    
    {
        flo32 tDamage = Clamp01( AutoTurret.ShowDamage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
        vec4  ColorD  = TowardWhite( bColorA, lerp( 0.0f, tDamage, 0.75f ) );
        rect  Bound   = RectCR( AutoTurret.Position, AUTO_TURRET_RADIUS * Scale );
        DrawRect( Pass, TextureID_AutoTurret, Bound, ColorD * Alpha );
    }
    
    flo32 bRadius = AUTO_TURRET_RADIUS * Scale;
    {
        vec2  Dim     = Vec2( 0.25f, 1.0f ) * bRadius;
        orect Bound   = ORectBCD( AutoTurret.Position, Dim, AutoTurret.Fire_Anim_Radians );
        
        DrawORect( Pass, TextureID_Barrel, Bound, TowardBlack( bColorB, 0.6f ) * Alpha );
        DrawRect ( Pass, TextureID_RoundedDome, RectCR( AutoTurret.Position, bRadius * 0.25f ), TowardBlack( bColorB, 0.65f ) * Alpha );
    }
    
    if( AutoTurret.Fire_Anim_Timer > 0.0f ) {
        if( AutoTurret.DrawCollision ) {
            {
                orect A = ORectCD( AutoTurret.DrawCollision_P, Vec2( BULLET_WIDTH, BULLET_WIDTH ) * 2.0f, RandomF32() * TAU );
                DrawORect( Pass, A, COLOR_WHITE );
            }
            
            {
                flo32 bRadians = ToRadians( AutoTurret.DrawCollision_N );
                flo32 RadiansA = bRadians + PI * RandomF32InRange( 0.35f, 0.45f ) - PI * 0.5f;
                flo32 RadiansB = bRadians - PI * RandomF32InRange( 0.35f, 0.45f ) - PI * 0.5f;
                
                vec2  DimA = Vec2( BULLET_WIDTH * 0.75f, BULLET_HEIGHT * RandomF32InRange( 0.35f, 0.75f ) ) * 0.5f;
                vec2  DimB = Vec2( BULLET_WIDTH * 0.75f, BULLET_HEIGHT * RandomF32InRange( 0.35f, 0.75f ) ) * 0.5f;
                
                vec2  Direction = ToDirection2D( AutoTurret.Fire_Anim_Radians );
                flo32 AdjHeight = dot( Direction, GetPerp( AutoTurret.DrawCollision_N ) ) * ( BULLET_HEIGHT * 0.2f );
                DimA.y += AdjHeight;
                DimB.y -= AdjHeight;
                
                DrawTriangleBC( Pass, AutoTurret.DrawCollision_P, DimA, RadiansA, COLOR_WHITE );
                DrawTriangleBC( Pass, AutoTurret.DrawCollision_P, DimB, RadiansB, COLOR_WHITE );
            }
        }
        
        flo32 Margin  = AUTO_TURRET_LASER_WIDTH * 1.25f;
        vec2  N       = GetNormal( AutoTurret.Fire_Anim_Position - AutoTurret.Position );
        vec2  bPos    = AutoTurret.Position + N * AUTO_TURRET_RADIUS;
        vec2  V       = AutoTurret.Fire_Anim_Position - bPos;
        vec2  Dim     = Vec2( AUTO_TURRET_LASER_WIDTH + Margin * 2.0f, GetLength( V ) );
        flo32 Radians = ToRadians( V );
        orect Bound = ORectBCD( bPos, Dim, Radians );
        DrawORect( Pass, TextureID_AutoTurretLaser_Body, Bound, COLOR_WHITE );
        
        {
            orect BoundE = ORectBCD( bPos, Vec2( AUTO_TURRET_LASER_WIDTH + Margin * 2.0f, Margin ), Radians + PI );
            DrawORect( Pass, TextureID_AutoTurretLaser_End, BoundE, COLOR_WHITE );
        }
        
        {
            orect BoundE = ORectBCD( AutoTurret.Fire_Anim_Position, Vec2( AUTO_TURRET_LASER_WIDTH + Margin * 2.0f, Margin ), Radians );
            DrawORect( Pass, TextureID_AutoTurretLaser_End, BoundE, COLOR_WHITE );
        }
    }
}

internal void
DrawAutoTurret( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    AUTO_TURRET_STATE * AutoTurretS = Draw->AutoTurretS;
    
    flo32 Powerup_Rate = 1.0f;
    if( AutoTurretS->Powerup_Timer > 0.0f ) {
        Powerup_Rate = AutoTurretS->Powerup_Rate;
    }
    
    for( uint32 iAutoTurret = 0; iAutoTurret < AutoTurretS->nAutoTurret; iAutoTurret++ ) {
        AUTO_TURRET AutoTurret = AutoTurretS->AutoTurret[ iAutoTurret ];
        
        DrawAutoTurret( Pass, AutoTurret, Powerup_Rate );
    }
}

internal void
FinalizeAutoTurret( APP_STATE * AppState ) {
    AUTO_TURRET_STATE * AutoTurretS = &AppState->AutoTurretS;
    
}