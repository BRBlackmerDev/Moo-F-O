
//----------
// SPIT
//----------

internal SPIT
SpitC( vec2 Position, vec2 Direction, SPIT_TYPE Type, boo32 IsSterilized = false ) {
    SPIT Result = {};
    Result.IsSterilized = IsSterilized;
    Result.Type         = Type;
    Result.iTexture     = RandomS32InRange( 0, ASTEROID_TEXTURE_ID_MAX_COUNT - 1 );
    Result.Position     = Position;
    Result.Velocity     = Direction * SPIT_SPEED;
    Result.Radius       = SPIT_RADIUS;
    Result.Health       = 1;
    Result.Radians      = RandomF32() * TAU;
    Result.dRadians     = RandomF32InRange( -1.0f, 1.0f ) * ( TAU / 2.0f );
    
    return Result;
}

internal void
AddSpit( APP_STATE * AppState, SPIT Spit ) {
    SPITTER_STATE * SpitterS = &AppState->SpitterS;
    if( SpitterS->nSpit < SPIT_MAX_COUNT ) {
        SpitterS->Spit[ SpitterS->nSpit++ ] = Spit;
        
        SpitterS->nSpitTotal += 1;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SPIT to SPITTER_STATE: %u", SPIT_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

//----------
// BARREL
//----------

internal BARREL
BarrelC( vec2 Position, vec2 Direction ) {
    BARREL Result = {};
    Result.Position  = Position;
    Result.Health    = BARREL_HEALTH;
    Result.Radians   = RandomF32() * TAU;
    
    flo32 Speed      = RandomF32InRange( 10.0f, 14.0f );
    Result.Velocity  = Direction * Speed;
    
    flo32 Range      = TAU / 4.0f;
    Result.dRadians  = RandomF32InRange( -Range, Range );
    
    return Result;
}

internal void
AddBarrel( APP_STATE * AppState, BARREL Barrel ) {
    SPITTER_STATE * SpitterS = &AppState->SpitterS;
    if( SpitterS->nBarrel < BARREL_MAX_COUNT ) {
        SpitterS->Barrel[ SpitterS->nBarrel++ ] = Barrel;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BARREL to SPITTER_STATE: %u", BARREL_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

//----------
// DAMAGE_ZONE
//----------

internal DAMAGE_ZONE
DamageZoneC( orect Bound ) {
    DAMAGE_ZONE Result = {};
    Result.Bound = Bound;
    
    return Result;
}

internal void
AddDamageZone( APP_STATE * AppState, DAMAGE_ZONE DamageZone ) {
    SPITTER_STATE * SpitterS = &AppState->SpitterS;
    if( SpitterS->nDamageZone < DAMAGE_ZONE_MAX_COUNT ) {
        SpitterS->DamageZone[ SpitterS->nDamageZone++ ] = DamageZone;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add DAMAGE_ZONE to SPITTER_STATE: %u", DAMAGE_ZONE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

//----------
// LOADER
//----------

internal LOADER
LoaderC( vec2 Position, flo32 Radians ) {
    LOADER Result = {};
    Result.Position  = Position;
    Result.Radians   = Radians;
    Result.Health    = LOADER_HEALTH;
    
    return Result;
}

internal void
AddLoader( APP_STATE * AppState, LOADER Loader ) {
    SPITTER_STATE * SpitterS = &AppState->SpitterS;
    if( SpitterS->nLoader < LOADER_MAX_COUNT ) {
        SpitterS->Loader[ SpitterS->nLoader++ ] = Loader;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add LOADER to SPITTER_STATE: %u", LOADER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

//----------
// SPITTER
//----------

internal SPITTER
SpitterC( SPITTER_TYPE Type, int32 GroupID, vec2 Position, vec2 Direction, flo32 DoSpit_Reset_TargetTime = 0.0f ) {
    SPITTER Result = {};
    Result.Type      = Type;
    Result.GroupID   = GroupID;
    Result.Position  = Position;
    Result.Direction = Direction;
    
    Result.DoSpit_Reset_TargetTime = DoSpit_Reset_TargetTime;
    
    return Result;
}

internal void
AddSpitter( APP_STATE * AppState, SPITTER Spitter ) {
    SPITTER_STATE * SpitterS = &AppState->SpitterS;
    if( SpitterS->nSpitter < SPITTER_MAX_COUNT ) {
        SpitterS->Spitter[ SpitterS->nSpitter++ ] = Spitter;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SPITTER to SPITTER_STATE: %u", SPITTER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateSpitter( APP_STATE * AppState, flo32 dT ) {
    SPITTER_STATE * SpitterS = &AppState->SpitterS;
    PLAYER_STATE  * Player   = &AppState->Player;
    
    { // Update Spit Bonus
        if( SpitterS->SpitBonus_IsActive ) {
            SpitterS->SpitBonus_Bound = RectTD( GetT( AppState->World_Bound, Vec2( 0.05f, 0.98f ) ), Vec2( 0.0f, 1.0f ), Vec2( 10.0f, 2.5f ) );
            
            SpitterS->SpitBonus_Timer += dT;
            
            if( SpitterS->SpitBonus_Timer >= SpitterS->SpitBonus_TargetTime ) {
                SpitterS->SpitBonus_Timer = 0.0f;
                
                SpitterS->SpitBonus_Type       = SpitterS->SpitBonus_NextType;
                SpitterS->SpitBonus_NextType   = ( SPIT_TYPE )RandomS32InRange( 0, 2 );
                SpitterS->SpitBonus_TargetTime = SPITTER_SPIT_BONUS_CYCLE_TARGET_TIME;
                
                AUDIO_ID AudioID = ( AUDIO_ID )( ( int32 )AudioID_SpitBonus_Red + ( int32 )SpitterS->SpitBonus_Type );
                // PlayAudio( AppState, AudioID, SPIT_BONUS_VOLUME );
            }
        }
    }
    
    { // Update Spiral Spitter State
        if( SpitterS->Spiral_nSpitters > 0 ) {
            SpitterS->Spiral_Timer += dT;
            if( SpitterS->Spiral_Timer >= SpitterS->Spiral_TargetTime ) {
                SpitterS->Spiral_Timer = 0.0f;
                
                Assert( SpitterS->Spiral_nSpitters == 3 );
                
                int32 nToSelect = SpitterS->Spiral_nToFire;
                if( nToSelect > 0 ) {
                    int32 nAvailable = SpitterS->Spiral_nSpitters;
                    int32 Index[ 3 ] = { 0, 1, 2 };
                    int32 Type [ 3 ] = { 0, 1, 2 };
                    
                    for( int32 iToSelect = 0; iToSelect < nToSelect; iToSelect++ ) {
                        int32 SelectA = RandomS32InRange( 0, nAvailable - 1 );
                        int32 SelectB = RandomS32InRange( 0, nAvailable - 1 );
                        
                        int32     iIndex = Index[ SelectA ];
                        SPIT_TYPE iType  = ( SPIT_TYPE )Type[ SelectB ];
                        
                        Index[ iIndex ] = Index[ nAvailable - 1 ];
                        Type [ iType  ] = Type [ nAvailable - 1 ];
                        nAvailable -= 1;
                        
                        SPITTER * Spitter = SpitterS->Spitter + iIndex;
                        Spitter->DoSpit         = true;
                        Spitter->DoSpit_Type    = iType;
                        Spitter->DoSpit_Counter = SPITTER_SPIT_COUNT_SPIRAL;
                        Spitter->DoSpit_Timer   = -SPITTER_WARMUP_TARGET_TIME_SPIRAL;
                    }
                }
            }
        }
    }
    
    { // Update Spawn Barrel
        if( SpitterS->Barrel_IsActive ) {
            SpitterS->Barrel_Timer += dT;
            if( SpitterS->Barrel_Timer >= SpitterS->Barrel_TargetTime ) {
                SpitterS->Barrel_Timer      = 0.0f;
                SpitterS->Barrel_TargetTime = lerp( 1.0f, RandomF32_2Dice(), 5.0f );
                
                int32 nScoreMultiplier = 1;
                flo32 t                = RandomF32();
                if( t >= 0.75f ) {
                    nScoreMultiplier = 2;
                }
                
                int32 IsAvailable[ SPITTER_MAX_COUNT ] = {};
                int32  nAvailable                      = 0;
                
                for( int32 iSpitter = 0; iSpitter < SpitterS->nSpitter; iSpitter++ ) {
                    SPITTER Spitter = SpitterS->Spitter[ iSpitter ];
                    if( Spitter.IsActive ) {
                        IsAvailable[ nAvailable++ ] = iSpitter;
                    }
                }
                
                if( nAvailable > 0 ) {
                    int32   Select  = IsAvailable[ RandomS32InRange( 0, nAvailable - 1 ) ];
                    SPITTER Spitter = SpitterS->Spitter[ Select ];
                    vec2 Pos = ClampToRect( Spitter.Position, AppState->World_Bound );
                    for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                        //AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                        
                        AddBarrel( AppState, BarrelC( Pos, RandomDirection2D() ) );
                    }
                } else {
                    SpitterS->Barrel_TargetTime = 1.0f;
                }
            }
        }
    }
    
    { // Update Spit
        for( int32 iSpit = 0; iSpit < SpitterS->nSpit; iSpit++ ) {
            SPIT * Spit = SpitterS->Spit + iSpit;
            
            vec2 dPos = Spit->Velocity * dT;
            Spit->Position += dPos;
            
            Spit->Radians   = WrapRadians( Spit->Radians + Spit->dRadians * dT );
        }
    }
    
    { // Update Barrel
        for( int32 iBarrel = 0; iBarrel < SpitterS->nBarrel; iBarrel++ ) {
            BARREL * Barrel = SpitterS->Barrel + iBarrel;
            
            for( int32 iRepel = iBarrel + 1; iRepel < SpitterS->nBarrel; iRepel++ ) {
                BARREL * Repel = SpitterS->Barrel + iRepel;
                
                vec2 V = Repel->Position - Barrel->Position;
                
                flo32 DistSq   = GetLengthSq( V );
                flo32 Radius   = BARREL_REPEL_RADIUS + BARREL_REPEL_RADIUS;
                flo32 RadiusSq = Radius * Radius;
                if( DistSq <= RadiusSq ) {
                    vec2 Repel_Vector = GetNormal( V ) * BARREL_REPEL_SPEED;
                    
                    Barrel->Repel -= Repel_Vector;
                    Repel->Repel  += Repel_Vector;
                }
            }
            
            {
                rect Bound = AddRadius( AppState->World_Bound, -0.5f );
                if( Barrel->Position.x <= Bound.Left ) {
                    Barrel->Repel.x += BARREL_REPEL_SPEED;
                }
                if( Barrel->Position.x >= Bound.Right ) {
                    Barrel->Repel.x -= BARREL_REPEL_SPEED;
                }
                if( Barrel->Position.y <= Bound.Bottom ) {
                    Barrel->Repel.y += BARREL_REPEL_SPEED;
                }
                if( Barrel->Position.y >= Bound.Top ) {
                    Barrel->Repel.y -= BARREL_REPEL_SPEED;
                }
            }
            
            {
                SCORE_STATE * ScoreS = &AppState->ScoreS;
                flo32 TractorBeamT = ScoreS->BonusMultiplier;
                flo32 Speed        = BARREL_TRACTOR_BEAM_SPEED * TractorBeamT;
                vec2  DirToPlayer  = GetNormal( Player->Position - Barrel->Position );
                
                Barrel->Velocity += DirToPlayer * ( Speed * dT );
            }
            
            Barrel->Radians += Barrel->dRadians * dT;
            
            flo32 Friction = 2.0f;
            vec2 Accel = ( -Barrel->Velocity + Barrel->Repel ) * Friction;
            
            vec2 dPos = Accel * ( 0.5f * dT * dT ) + Barrel->Velocity * dT;
            
            Barrel->Position += dPos;
            Barrel->Velocity += Accel * dT;
            
            Barrel->Repel = {};
        }
    }
    
    { // Update DamageZone
        for( int32 iDamageZone = 0; iDamageZone < SpitterS->nDamageZone; iDamageZone++ ) {
            DAMAGE_ZONE * DamageZone = SpitterS->DamageZone + iDamageZone;
            
            flo32 dRadians = ( TAU / 2.0f ) * dT;
            DamageZone->IsInactive_Radians = WrapRadians( DamageZone->IsInactive_Radians + dRadians );
            
            { // Update Enabled
                if( DamageZone->IsEnabled ) {
                    DamageZone->IsEnabled_Timer = MinValue( DamageZone->IsEnabled_Timer + dT, DAMAGE_ZONE_ENABLE_TARGET_TIME );
                } else {
                    DamageZone->IsEnabled_Timer = MaxValue( DamageZone->IsEnabled_Timer - dT, 0.0f );
                }
            }
            
            DamageZone->Timer += dT;
            if( DamageZone->Timer >= DAMAGE_ZONE_ACTIVE_TARGET_TIME ) {
                DamageZone->Timer = -( DAMAGE_ZONE_ACTIVE_TARGET_TIME * ( flo32 )( SpitterS->nDamageZone - 1 ) );
                DamageZone->IsInactive = false;
            }
        }
    }
    
    { // Update Loader
        for( int32 iLoader = 0; iLoader < SpitterS->nLoader; iLoader++ ) {
            LOADER * Loader = SpitterS->Loader + iLoader;
            
            {
                vec2  Pos[ LOADER_EXHAUST_MAX_COUNT ] = {};
                flo32 Rad[ LOADER_EXHAUST_MAX_COUNT ] = {};
                
                orect Bound = ORectCD( Loader->Position, LOADER_DIM, Loader->Radians );
                
                Pos[ 0 ] = GetT( Bound, Vec2( 0.0f, 0.9f ) );
                Pos[ 1 ] = GetT( Bound, Vec2( 0.0f, 0.1f ) );
                Pos[ 2 ] = GetT( Bound, Vec2( 0.8f, 0.15f ) );
                Pos[ 3 ] = GetT( Bound, Vec2( 0.8f, 0.85f ) );
                
                Rad[ 0 ] = Loader->Radians + PI;
                Rad[ 1 ] = Loader->Radians + PI;
                Rad[ 2 ] = Loader->Radians + PI + PI * 0.125f;
                Rad[ 3 ] = Loader->Radians + PI - PI * 0.125f;
                
                for( int32 iExhaust = 0; iExhaust < LOADER_EXHAUST_MAX_COUNT; iExhaust++ ) {
                    Loader->Exhaust_Timer[ iExhaust ] += dT;
                    if( Loader->Exhaust_Timer[ iExhaust ] >= Loader->Exhaust_TargetTime[ iExhaust ] ) {
                        Loader->Exhaust_Timer[ iExhaust ]     -= Loader->Exhaust_TargetTime[ iExhaust ];
                        Loader->Exhaust_TargetTime[ iExhaust ] = ( flo32 )RandomS32InRange( 4, 8 ) / 60.0f;
                        
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
                        if( iExhaust >= 2 ) {
                            Particle.Dim      *= 0.5f;
                            Particle.Velocity *= 0.5f;
                        }
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
                flo32 dRadians = ( TAU / 0.125f );
                Loader->Rumble_Radians = WrapRadians( Loader->Rumble_Radians + dRadians * dT );
            }
            
            Loader->Anim_Timer += dT;
            if( Loader->Anim_Timer >= LOADER_ANIM_TARGET_TIME ) {
                Loader->Anim_Timer = 0.0f;
            }
            
            Loader->Damage_Cooldown_Timer = MinValue( Loader->Damage_Cooldown_Timer + dT, 2.0f );
            
            { // Check Damage Zones
                orect Bound = ORectCD( Loader->Position, LOADER_DIM, Loader->Radians );
                
                for( int32 iDamageZone = 0; iDamageZone < SpitterS->nDamageZone; iDamageZone++ ) {
                    DAMAGE_ZONE * DamageZone = SpitterS->DamageZone + iDamageZone;
                    
                    if( ( DamageZone->Timer >= 0.0f ) && ( !DamageZone->IsInactive ) && ( DoesORectIntersectORect( Bound, DamageZone->Bound ) ) ) {
                        Loader->Health = MaxValue( Loader->Health - 1, 0 );
                        Loader->Timer                 = -LOADER_DAMAGE_ZONE_COOLDOWN_TARGET_TIME;
                        Loader->Damage_Cooldown_Timer = -LOADER_DAMAGE_ZONE_COOLDOWN_TARGET_TIME;
                        
                        DamageZone->IsInactive = true;
                        
                        int32 nScoreMultiplier = 10;
                        vec2 Pos = ClampToRect( Loader->Position, AppState->World_Bound );
                        for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                            AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                        }
                    }
                }
            }
            
            { // Barrel Collision
                vec2  Dim    = LOADER_DIM + Vec2( -0.1f, -0.1f );
                orect BoundA = ORectCD( Loader->Position, Dim, Loader->Radians );
                
                for( int32 iBarrel = 0; iBarrel < SpitterS->nBarrel; iBarrel++ ) {
                    BARREL * Barrel = SpitterS->Barrel + iBarrel;
                    
                    orect BoundB = ORectCD( Barrel->Position, BARREL_DIM, Barrel->Radians );
                    if( DoesORectIntersectORect( BoundA, BoundB ) ) {
                        Barrel->Health = 0;
                    }
                }
            }
            
            Loader->Timer += dT;
            
            vec2  Dir      = {};
            flo32 Friction = LOADER_MOVE_FRICTION;
            if( Loader->Timer < 0.0f ) {
                Friction = 4.0f;
            }
            
            if( Loader->Timer >= 0.0f ) {
                if( Loader->DoMove ) {
                    if( Loader->Timer >= LOADER_MOVE_WARMUP_TARGET_TIME ) {
                        Dir = ToDirection2D( Loader->Radians );
                    }
                    
                    {
                        flo32 dRadiansMax     = LOADER_ROTATION_SPEED_SLOW * dT;
                        vec2  FacingDir       = ToDirection2D( Loader->Radians );
                        vec2  DirToPlayer     = GetNormal( Player->Position - Loader->Position );
                        flo32 RadiansToPlayer = ToRadians( DirToPlayer );
                        flo32 dRadians        = GetSmallestDiffInRadians( Loader->Radians, RadiansToPlayer );
                        
                        if( fabsf( dRadians ) <= LOADER_MOVE_RADIAN_HALF_RANGE ) {
                            dRadians = Clamp( dRadians, -dRadiansMax, dRadiansMax );
                            
                            Loader->Radians += dRadians;
                        } else {
                            Loader->DoMove = false;
                            Loader->Timer  = 0.0f;
                        }
                    }
                } else {
                    if( Loader->Timer >= LOADER_ROTATION_WARMUP_TARGET_TIME ) {
                        flo32 dRadiansMax     = LOADER_ROTATION_SPEED_FAST * dT;
                        vec2  FacingDir       = ToDirection2D( Loader->Radians );
                        vec2  DirToPlayer     = GetNormal( Player->Position - Loader->Position );
                        flo32 RadiansToPlayer = ToRadians( DirToPlayer );
                        flo32 dRadians        = GetSmallestDiffInRadians( Loader->Radians, RadiansToPlayer );
                        
                        if( fabsf( dRadians ) > LOADER_MOVE_RADIAN_HALF_RANGE ) {
                            dRadians = Clamp( dRadians, -dRadiansMax, dRadiansMax );
                            
                            Loader->Radians += dRadians;
                        } else {
                            Loader->DoMove = true;
                            Loader->Timer  = 0.0f;
                        }
                    }
                }
            }
            
            flo32 Speed = LOADER_MOVE_SPEED_SLOW;
            if( Player->Collector_IsActive ) {
                Speed = LOADER_MOVE_SPEED_FAST;
            }
            vec2 Accel = ( Dir * Speed - Loader->Velocity ) * Friction;
            
            vec2 dPos = Accel * ( 0.5f * dT * dT ) + Loader->Velocity * dT;
            // TODO: Collision!
            
            Loader->Position += dPos;
            Loader->Velocity += Accel * dT;
        }
    }
    
    { // Update Spitter
        for( int32 iSpitter = 0; iSpitter < SpitterS->nSpitter; iSpitter++ ) {
            SPITTER * Spitter = SpitterS->Spitter + iSpitter;
            
            { // Update Active
                if( Spitter->IsActive ) {
                    Spitter->Timer = MinValue( Spitter->Timer + dT, SPITTER_ACTIVE_TARGET_TIME );
                } else {
                    Spitter->Timer = MaxValue( Spitter->Timer - dT, 0.0f );
                }
            }
            
            { // Update Attack
                if( ( Spitter->IsActive ) && ( Spitter->Timer >= SPITTER_ACTIVE_TARGET_TIME ) ) {
                    if( !Spitter->DoSpit ) {
                        switch( Spitter->Type ) {
                            case SpitterType_Single:
                            case SpitterType_Double: {
                                Spitter->DoSpit_Timer += dT;
                                if( Spitter->DoSpit_Timer >= Spitter->DoSpit_TargetTime ) {
                                    Spitter->DoSpit         = true;
                                    Spitter->DoSpit_Timer  -= ( Spitter->DoSpit_TargetTime + SPITTER_WARMUP_TARGET_TIME_SINGLE );
                                    Spitter->DoSpit_Counter = SPITTER_SPIT_COUNT_SINGLE;
                                    
                                    int32 Numer[ SpitType_Count ] = { 2, 2, 1 };
                                    int32 ToAdd = RandomSelectByOdds( Numer, SpitType_Count );
                                    
                                    Spitter->DoSpit_Type = ( SPIT_TYPE )( ( Spitter->DoSpit_Type + ToAdd ) % SpitType_Count );
                                }
                            } break;
                            
                            case SpitterType_Spiral: {
                                flo32 dRadians = ( TAU / 18.0f ) * dT;
                                flo32  Radians = ToRadians( Spitter->Direction );
                                Spitter->Direction = ToDirection2D( Radians + dRadians );
                            } break;
                            
                            case SpitterType_Short: {
                                Spitter->DoSpit_Timer += dT;
                                if( Spitter->DoSpit_Timer >= Spitter->DoSpit_TargetTime ) {
                                    Spitter->DoSpit         = true;
                                    Spitter->DoSpit_Timer  -= ( Spitter->DoSpit_TargetTime + SPITTER_WARMUP_TARGET_TIME_SHORT );
                                    Spitter->DoSpit_Counter = SPITTER_SPIT_COUNT_SHORT;
                                    
                                    int32 Numer[ SpitType_Count ] = { 2, 2, 1 };
                                    int32 ToAdd = RandomSelectByOdds( Numer, SpitType_Count );
                                    
                                    Spitter->DoSpit_Type = ( SPIT_TYPE )( ( Spitter->DoSpit_Type + ToAdd ) % SpitType_Count );
                                }
                            } break;
                        }
                    }
                    
                    if( Spitter->DoSpit ) {
                        vec2 bPos = Spitter->Position;
                        vec2 bDir = Spitter->Direction;
                        
                        switch( Spitter->Type ) {
                            case SpitterType_Single: {
                                Spitter->DoSpit_TargetTime = ( 12.0f / 60.0f );
                                Spitter->DoSpit_Timer += dT;
                                if( Spitter->DoSpit_Timer >= Spitter->DoSpit_TargetTime ) {
                                    Spitter->DoSpit_Timer = 0.0f;
                                    
                                    AddSpit( AppState, SpitC( bPos, bDir, Spitter->DoSpit_Type ) );
                                    Spitter->DoSpit_Counter -= 1;
                                    
                                    if( Spitter->DoSpit_Counter <= 0 ) {
                                        Spitter->DoSpit            = false;
                                        Spitter->DoSpit_TargetTime = RandomF32InRange( 2.0f, 4.0f );
                                    }
                                }
                            } break;
                            
                            case SpitterType_Double: {
                                Spitter->DoSpit_TargetTime = ( 12.0f / 60.0f );
                                Spitter->DoSpit_Timer += dT;
                                if( Spitter->DoSpit_Timer >= Spitter->DoSpit_TargetTime ) {
                                    Spitter->DoSpit_Timer = 0.0f;
                                    
                                    AddSpit( AppState, SpitC( bPos, bDir, Spitter->DoSpit_Type ) );
                                    Spitter->DoSpit_Counter -= 1;
                                    
                                    if( Spitter->DoSpit_Counter <= 0 ) {
                                        Spitter->DoSpit            = false;
                                        Spitter->DoSpit_TargetTime = Spitter->DoSpit_Reset_TargetTime;
                                    }
                                }
                            } break;
                            
                            case SpitterType_Spiral: {
                                Spitter->DoSpit_TargetTime = ( 6.0f / 60.0f );
                                
                                flo32 dRadians = ( ( TAU * -1.25f ) / ( Spitter->DoSpit_TargetTime * SPITTER_SPIT_COUNT_SPIRAL ) ) * dT;
                                
                                if( Spitter->DoSpit_Timer < 0.0f ) {
                                    dRadians = ( TAU / 18.0f ) * dT;
                                }
                                
                                flo32  Radians = ToRadians( Spitter->Direction );
                                Spitter->Direction = ToDirection2D( Radians + dRadians );
                                
                                Spitter->DoSpit_Timer += dT;
                                if( Spitter->DoSpit_Timer >= Spitter->DoSpit_TargetTime ) {
                                    Spitter->DoSpit_Timer    = 0.0f;
                                    Spitter->DoSpit_Counter -= 1;
                                    AddSpit( AppState, SpitC( bPos, Spitter->Direction, Spitter->DoSpit_Type ) );
                                }
                                
                                if( Spitter->DoSpit_Counter <= 0 ) {
                                    Spitter->DoSpit = false;
                                }
                            } break;
                            
                            case SpitterType_Short: {
                                Spitter->DoSpit_TargetTime = ( 12.0f / 60.0f );
                                Spitter->DoSpit_Timer += dT;
                                if( Spitter->DoSpit_Timer >= Spitter->DoSpit_TargetTime ) {
                                    Spitter->DoSpit_Timer = 0.0f;
                                    
                                    AddSpit( AppState, SpitC( bPos, bDir, Spitter->DoSpit_Type ) );
                                    Spitter->DoSpit_Counter -= 1;
                                    
                                    if( Spitter->DoSpit_Counter <= 0 ) {
                                        Spitter->DoSpit            = false;
                                        Spitter->DoSpit_TargetTime = RandomF32InRange( 0.5f, 1.5f );
                                    }
                                }
                            } break;
                        }
                    }
                }
            }
        }
    }
    
    DISPLAY_VALUE( uint32, SpitterS->nSpitter );
    DISPLAY_VALUE( uint32, SpitterS->nSpit );
    
    DISPLAY_VALUE( int32, SpitterS->nSpitTotal );
    DISPLAY_VALUE( int32, SpitterS->nSpitBonusTotal );
}

internal void
DrawSpit( RENDER_PASS * Pass, SPIT Spit, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    SPIT_BASE_COLORS;
    
    //DrawCircle( Pass, Spit.Position, Spit.Radius * Scale, SpitBaseColors[ Spit.Type ] * Alpha );
    
    vec4 Color = SpitBaseColors[ Spit.Type ];
    if( Spit.IsSterilized ) {
        Color = COLOR_WHITE;
    }
    
    DrawORect( Pass, TextureID_Spit, ORectCR( Spit.Position, Spit.Radius * Scale, Spit.Radians ), Color * Alpha );
}

internal void
DrawBarrel( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, BARREL Barrel, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    orect Bound = ORectCD( Barrel.Position, BARREL_DIM * Scale, Barrel.Radians );
    vec4  Color = BARREL_BASE_COLOR;
    
    DrawORect( Pass, TextureID_Barrel, Bound, Color );
    
    if( Barrel.Health > 0 ) {
        DrawEntityHealth( Pass, AppState, Camera, Barrel.Position, Barrel.Health, COLOR_GRAY( 0.9f ) );
    }
}

internal void
DrawDamageZone( RENDER_PASS * Pass, DAMAGE_ZONE DamageZone, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    int32 nSegment = 5;
    vec2  Dim      = DamageZone.Bound.HalfDim * 2.0f;
    
    vec4 ColorA = COLOR_GRAY( 0.05f );
    vec4 ColorB = COLOR_GRAY( 0.2f );
    vec4 ColorC = Vec4( 0.0f, 0.75f, 0.75f, 1.0f );
    
    {
        flo32 t = Clamp01( DamageZone.IsEnabled_Timer / DAMAGE_ZONE_ENABLE_TARGET_TIME );
        
        ColorA = lerp( COLOR_GRAY( 0.025f ), t, ColorA );
        ColorB = lerp( COLOR_GRAY( 0.05f  ), t, ColorB );
    }
    
    DrawORect( Pass, AddRadius( DamageZone.Bound, 0.05f ), ColorB );
    
    if( ( DamageZone.IsEnabled ) && ( !DamageZone.IsInactive ) && ( DamageZone.Timer >= -DAMAGE_ZONE_WARMUP_TARGET_TIME ) ) {
        flo32 t = Clamp01( ( DamageZone.Timer + DAMAGE_ZONE_WARMUP_TARGET_TIME - ( 20.0f / 60.0f ) ) / DAMAGE_ZONE_WARMUP_TARGET_TIME );
        
        if( DamageZone.Timer < 0.0f ) {
            ColorA = lerp( ColorA, t, COLOR_GRAY( 0.5f ) );
        }
        
        ColorB = lerp( ColorB, t, COLOR_GRAY( 0.75f ) );
    }
    if( ( DamageZone.IsEnabled ) && ( DamageZone.IsInactive ) ) {
        flo32 t = cosf( DamageZone.IsInactive_Radians ) * 0.5f - 0.5f;
        ColorA = lerp( COLOR_GRAY( 0.10f ), t, COLOR_GRAY( 0.15f ) );
    }
    
    DrawORect( Pass, DamageZone.Bound, ColorA );
    
    vec2 Pos0[ 5 ] = {};
    
    {
        flo32 Segment_WidthA  = Dim.y * 0.025f;
        flo32 Segment_OffsetA = ( Dim.y - Segment_WidthA ) / ( ( flo32 )( nSegment - 1 ) );
        vec2  OffsetA        = DamageZone.Bound.yAxis * Segment_OffsetA;
        
        vec2  bPos    = GetT( DamageZone.Bound, Vec2( 0.0f, 0.0f ) );
        vec2  DimA    = Vec2( Dim.x, Segment_WidthA );
        flo32 Radians = ToRadians( DamageZone.Bound.xAxis );
        
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            vec2  Pos   = bPos + OffsetA * ( flo32 )iSegment;
            orect Bound = ORectTD( Pos, Vec2( 0.0f, 0.0f ), DimA, Radians );
            
            DrawORect( Pass, Bound, ColorB );
            
            Pos0[ iSegment ] = Pos;
        }
    }
    
    if( ( DamageZone.IsEnabled ) && ( !DamageZone.IsInactive ) && ( DamageZone.Timer > 0.0f ) ) {
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            int32 nPoint = 20;
            
            vec2  bPos    = Pos0[ iSegment ];
            vec2  OffsetA = DamageZone.Bound.xAxis * ( Dim.x / ( flo32 )nPoint );
            
            vec2  A = bPos;
            
            for( int32 iPoint = 0; iPoint < nPoint; iPoint++ ) {
                vec2  Pos = bPos + OffsetA * ( flo32 )( iPoint + 1 );
                flo32 t   = 0.0f;
                if( iPoint < ( nPoint - 1 ) ) {
                    t = RandomF32InRange( -1.0f, 1.0f );
                }
                vec2  B   = Pos + DamageZone.Bound.yAxis * ( t * 0.35f );
                
                orect Bound = ORectCD( ( A + B ) * 0.5f, Vec2( GetLength( B - A ), 0.15f ), ToRadians( B - A ) );
                DrawORect( Pass, Bound, ColorC );
                //DrawLine( Pass, A, B, ColorC );
                
                A = B;
            }
        }
    }
    
#if 0    
    if( ( DamageZone.IsEnabled ) && ( !DamageZone.IsInactive ) && ( DamageZone.Timer >= -DAMAGE_ZONE_WARMUP_TARGET_TIME ) ) {
        vec4 Color = Vec4( 0.5f, 0.0f, 0.0f, 0.5f );
        if( DamageZone.Timer < 0.0f ) {
            flo32 t    = 1.0f - Clamp01( DamageZone.Timer / -DAMAGE_ZONE_WARMUP_TARGET_TIME );
            flo32 Alph = lerp( 0.0f, t, 0.25f );
            Color = Vec4( Alph, Alph, Alph, Alph );
        }
        
        DrawORect( Pass, DamageZone.Bound, Color );
    }
#endif
    
#if 0
    {
        flo32 t     = Clamp01( DamageZone.IsEnabled_Timer / DAMAGE_ZONE_ENABLE_TARGET_TIME );
        vec4  Color = Vec4( t, 0.0f, 0.0f, t );
        DrawORectOutline( Pass, DamageZone.Bound, Color );
    }
#endif
}

internal void
DrawLoader( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, LOADER Loader, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec2 Loader_Dim = LOADER_DIM;
    
    flo32 tRumble       = cosf( Loader.Rumble_Radians );
    flo32 tRadians      = cosf( Loader.Rumble_Radians - PI * 0.5f );
    flo32 Radians       = Loader.Radians + tRadians * ( TAU / 512.0f );
    vec2  Rumble_Offset = ToDirection2D( Radians + PI * 0.5f ) * ( Loader_Dim.x * 0.01f * tRumble );
    vec2  bPos = Loader.Position + Rumble_Offset;
    
    Loader_Dim *= Scale;
    
    orect BoundA = ORectCD( bPos, Loader_Dim, Radians );
    
    if( Loader.Damage_Cooldown_Timer > 0.0f ) {
        flo32 DamageT      = 1.0f - Clamp01( Loader.Damage_Cooldown_Timer / 2.0f );
        vec2  DamageOffset = BoundA.xAxis * Loader_Dim.x * -0.1f;
        
        flo32 AnimT   = 1.0f - Clamp01( Loader.Anim_Timer / LOADER_ANIM_TARGET_TIME );
        
        int32 nSegment = 6;
        
        flo32 ScaX = ( Loader_Dim.y * 0.9f ) / ( flo32 )( nSegment * 2 + 1 );
        flo32 ScaY = Loader_Dim.x * 0.07f;
        
        vec2  OffsetX   = BoundA.yAxis * ScaX;
        vec2  OffsetY   = BoundA.xAxis * -ScaY;
        vec2  Spike_Pos = GetT( BoundA, Vec2( 1.03f, 0.5f ) );
        
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            vec2 P = Spike_Pos + ( OffsetX * ( ( flo32 )iSegment + AnimT - 0.25f ) );
            vec2 Q = Spike_Pos - ( OffsetX * ( ( flo32 )iSegment + AnimT - 0.25f ) );
            
            if( iSegment == 0 ) {
                P += OffsetY * ( 1.0f - AnimT );
                Q += OffsetY * ( 1.0f - AnimT );
            }
            if( iSegment == ( nSegment - 1 ) ) {
                P += OffsetY * AnimT;
                Q += OffsetY * AnimT;
            }
            
            P += DamageOffset * DamageT;
            Q += DamageOffset * DamageT;
            
            DrawTriangle( Pass, P, Vec2( ScaX, ScaY ), Loader.Radians - PI * 0.5f, COLOR_RED * Alpha );
            DrawTriangle( Pass, Q, Vec2( ScaX, ScaY ), Loader.Radians - PI * 0.5f, COLOR_RED * Alpha );
        }
    }
    
    vec4 Body_Color = LOADER_BASE_COLOR;
    if( Loader.Damage_Cooldown_Timer < 0.0f ) {
        Body_Color = TowardBlack( Body_Color, 0.5f );
    }
    DrawORect( Pass, TextureID_Loader_Body, BoundA, Body_Color * Alpha );
    //DrawORect( Pass, BoundB, ColorB );
    
    
    vec2 Dome_Pos = bPos;
    {
        vec2 Dome_Dim = Loader_Dim * Vec2( 0.2f, 0.65f );
        Dome_Pos = bPos + ToDirection2D( Radians ) * 0.2f * Scale;
        
        vec4 Window_Color = COLOR_GRAY( 0.135f );
        vec4 Dome_Color   = TowardBlack( Body_Color, 0.2f );
        
        DrawORect( Pass, TextureID_HalfDome, ORectTD( Dome_Pos, Vec2( 1.0f, 0.5f ), Dome_Dim * Vec2( 2.0f, 1.0f ), Radians ), Dome_Color * Alpha );
        DrawORect( Pass, TextureID_HalfDome, ORectTD( Dome_Pos, Vec2( 1.0f, 0.5f ), Dome_Dim * Vec2( 1.25f, 1.0f ), Radians + PI ), TowardBlack( Dome_Color, 0.05f ) * Alpha );
        DrawORect( Pass, TextureID_Dome, ORectCD( Dome_Pos, Dome_Dim,        Radians ), Dome_Color * Alpha );
        DrawORect( Pass, TextureID_Dome, ORectCD( Dome_Pos, Dome_Dim - Vec2( Dome_Dim.x, Dome_Dim.x ) * 0.2f, Radians ), Window_Color * Alpha );
        
        //DrawEllipse( Pass, Pos, Sca, Radians, Color );
        
    }
    
    if( Loader.Damage_Cooldown_Timer < 0.0f ) {
        vec4 ColorC = Vec4( 0.0f, 0.75f, 0.75f, 1.0f );
        
        vec2 Pos0[ 5 ] = {};
        
        vec2  Dim      = Loader_Dim;
        int32 nSegment = 5;
        orect R        = BoundA;
        
        {
            flo32 Segment_OffsetA = ( Dim.y / ( flo32 )( nSegment - 1 ) );
            vec2  OffsetA         = R.yAxis * Segment_OffsetA;
            
            vec2  Spark_Pos = GetT( R, Vec2( 0.0f, 0.0f ) );
            for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                Pos0[ iSegment ] = Spark_Pos + OffsetA * ( flo32 )iSegment;
            }
        }
        
        flo32 tRandom = Clamp01( ( Loader.Damage_Cooldown_Timer + LOADER_DAMAGE_ZONE_COOLDOWN_TARGET_TIME ) / LOADER_DAMAGE_ZONE_COOLDOWN_TARGET_TIME );
        
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            int32 nPoint = 20;
            
            vec2  Point_bPos    = Pos0[ iSegment ];
            vec2  OffsetA = R.xAxis * ( Dim.x / ( flo32 )nPoint );
            
            vec2  A = Point_bPos;
            
            for( int32 iPoint = 0; iPoint < nPoint; iPoint++ ) {
                vec2  Pos = Point_bPos + OffsetA * ( flo32 )( iPoint + 1 );
                flo32 t   = 0.0f;
                if( iPoint < ( nPoint - 1 ) ) {
                    t = RandomF32InRange( -1.0f, 1.0f );
                }
                vec2  B   = Pos + R.yAxis * ( t * 0.35f );
                
                orect Bound = ORectCD( ( A + B ) * 0.5f, Vec2( GetLength( B - A ), 0.15f ), ToRadians( B - A ) );
                
                flo32 tDraw = RandomF32();
                if( tDraw > tRandom ) {
                    DrawORect( Pass, Bound, ColorC );
                }
                
                A = B;
            }
        }
    }
    
#if 0    
    {
        vec2 DirA = ToDirection2D( Loader.Radians + LOADER_MOVE_RADIAN_HALF_RANGE );
        vec2 DirB = ToDirection2D( Loader.Radians - LOADER_MOVE_RADIAN_HALF_RANGE );
        vec2 A    = Loader.Position;
        vec2 B    = A + DirA * 100.0f;
        vec2 C    = A + DirB * 100.0f;
        DrawLine( Pass, A, B, COLOR_RED );
        DrawLine( Pass, A, C, COLOR_RED );
    }
#endif
    
    { // EXHAUST PIPES
        vec2 Pipe_Dim = Loader_Dim * Vec2( 0.15f, 0.1f );
        
        vec2 Pipe_PosA = GetT( BoundA, Vec2( 0.1f, 0.95f ) );
        vec2 Pipe_PosB = GetT( BoundA, Vec2( 0.1f, 0.05f ) );
        
        orect Pipe_BoundA = ORectTD( Pipe_PosA, Vec2( 1.0f, 1.0f ), Pipe_Dim, Radians );
        orect Pipe_BoundB = ORectTD( Pipe_PosB, Vec2( 1.0f, 0.0f ), Pipe_Dim, Radians );
        
        vec4  Pipe_Color  = COLOR_GRAY( 0.1f );
        
        DrawORect( Pass, TextureID_Barrel_Rot90, Pipe_BoundA, Pipe_Color * Alpha );
        DrawORect( Pass, TextureID_Barrel_Rot90, Pipe_BoundB, Pipe_Color * Alpha );
        DrawORect( Pass, TextureID_SatelliteDish, ORectTD( Pipe_PosA, Vec2( 0.0f, 1.0f ), Vec2( Pipe_Dim.y, Pipe_Dim.y ) * Vec2( 0.5f, 1.0f ), Radians ), Pipe_Color * Alpha );
        DrawORect( Pass, TextureID_SatelliteDish, ORectTD( Pipe_PosB, Vec2( 0.0f, 0.0f ), Vec2( Pipe_Dim.y, Pipe_Dim.y ) * Vec2( 0.5f, 1.0f ), Radians ), Pipe_Color * Alpha );
    }
    
    { // EXHAUST VENTS
        vec2 Vent_Dim = Loader_Dim * Vec2( 0.1f, 0.075f );
        
        vec2 Vent_PosA = GetT( BoundA, Vec2( 0.8f, 0.15f ) );
        vec2 Vent_PosB = GetT( BoundA, Vec2( 0.8f, 0.85f ) );
        
        vec4 Vent_Color = COLOR_GRAY( 0.15f );
        
        DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( Vent_PosA, Vent_Dim, Radians + PI + PI * 0.125f ), Vent_Color * Alpha );
        DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( Vent_PosB, Vent_Dim, Radians + PI - PI * 0.125f ), Vent_Color * Alpha );
    }
    
    if( Loader.Health > 0 ) {
        vec2 Plate_Pos = GetT( BoundA, Vec2( -0.025f, 0.5f ) );
        DrawORect( Pass, TextureID_Worm_Tag, ORectCD( Plate_Pos, WORM_TAG_DIM, Radians + PI * 0.5f ), COLOR_GRAY( 0.1f ) );
        
        DrawEntityHealth( Pass, AppState, Camera, Plate_Pos, Loader.Health, COLOR_WHITE );
    }
}

internal void
DrawSpitter( RENDER_PASS * Pass, SPITTER Spitter, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 t    = Clamp01( Spitter.Timer / SPITTER_ACTIVE_TARGET_TIME );
    flo32 Alph = t * t;
    flo32 Sca  = lerp( 0.7f, t, 1.0f );
    
    flo32 bRadius = 0.0f;
    vec2  bDim    = {};
    vec2  bPos    = Spitter.Position;
    
    SPIT_BASE_COLORS;
    vec4 bColor = SpitBaseColors[ Spitter.DoSpit_Type ];
    
    vec4 Bound_Color         = COLOR_GRAY( 0.15f );
    vec4 Base_Color          = COLOR_GRAY( 0.1f );
    vec4 Barrel_Color        = COLOR_GRAY( 0.15f );
    vec4 InsideBarrel_Color  = COLOR_GRAY( 0.025f );
    vec4 FrontOfBarrel_Color = TowardBlack( Barrel_Color, 0.2f );
    
    flo32 tSpit = 0.0f;
    
    switch( Spitter.Type ) {
        case SpitterType_Single:
        case SpitterType_Double: {
            bRadius = 2.0f * Sca;
            bDim    = bRadius * Vec2( 0.65f, 1.0f );
            
            if( Spitter.DoSpit_Timer < 0.0f ) {
                tSpit   = Clamp01( ( Spitter.DoSpit_Timer + SPITTER_WARMUP_TARGET_TIME_SINGLE - ( 20.0f / 60.0f ) ) / SPITTER_WARMUP_TARGET_TIME_SINGLE );
            }
        } break;
        
        case SpitterType_Spiral: {
            bRadius = 3.5f * Sca;
            bDim    = bRadius * Vec2( 0.75f, 0.75f );
            
            if( Spitter.DoSpit_Timer < 0.0f ) {
                tSpit   = Clamp01( ( Spitter.DoSpit_Timer + SPITTER_WARMUP_TARGET_TIME_SPIRAL - ( 80.0f / 60.0f ) ) / SPITTER_WARMUP_TARGET_TIME_SPIRAL );
            }
        } break;
        
        case SpitterType_Short: {
            bRadius = 1.5f * Sca;
            bDim    = bRadius * Vec2( 0.5f, 1.2f );
            
            if( Spitter.DoSpit_Timer < 0.0f ) {
                tSpit   = Clamp01( ( Spitter.DoSpit_Timer + SPITTER_WARMUP_TARGET_TIME_SHORT - ( 8.0f / 60.0f ) ) / SPITTER_WARMUP_TARGET_TIME_SHORT );
            }
        } break;
    }
    
    {
        InsideBarrel_Color  = lerp( InsideBarrel_Color,  tSpit,         bColor ) * Alph;
        FrontOfBarrel_Color = lerp( FrontOfBarrel_Color, tSpit * 0.25f, bColor ) * Alph;
        Bound_Color         = lerp( Bound_Color,         tSpit * 0.65f, bColor ) * Alph;
        Barrel_Color        = lerp( Barrel_Color,        tSpit * 0.65f, bColor ) * Alph;
        Base_Color *= Alph;
    }
    
    if( ( Spitter.IsActive ) && ( Spitter.DoSpit ) && ( Spitter.DoSpit_Timer >= 0.0f ) ) {
        InsideBarrel_Color  = lerp( InsideBarrel_Color,  0.75f, bColor );
        FrontOfBarrel_Color = lerp( FrontOfBarrel_Color, 0.15f, bColor );
        Barrel_Color        = lerp( Barrel_Color,        0.35f, bColor );
        Bound_Color         = lerp( Bound_Color,         0.5f,  bColor );
    }
    
    { // Bound
        DrawRect( Pass, TextureID_RoundedDome, RectCR( Spitter.Position, bRadius ), Bound_Color );
        
        if( Spitter.Type == SpitterType_Spiral ) {
            DrawRect( Pass, TextureID_Dome, RectCR( Spitter.Position, bRadius * 0.8f ), TowardBlack( Bound_Color, 0.8f ) );
            DrawRect( Pass, TextureID_Dome, RectCR( Spitter.Position, bRadius * 0.7f ), Bound_Color );
        }
    }
    
    //DrawCircle( Pass, Spitter.Position, bRadius,         ColorA * Alph );
    //DrawCircle( Pass, Spitter.Position, bRadius * 0.85f, ColorB * Alph );
    
    flo32 Radians = ToRadians( -Spitter.Direction );
    
    { // Base
        vec2  Base_Dim   = bDim;
        orect Base_Bound = ORectTD( bPos, Vec2( 0.0f, 0.5f ), Base_Dim, Radians );
        Base_Bound = ORectAddDim( Base_Bound, Base_Dim.y * 0.4f, 0.0f, 0.0f, 0.0f );
        Base_Bound.HalfDim += Vec2( 0.2f, 0.2f );
        
        DrawORect( Pass, TextureID_Spitter_Base, Base_Bound, Base_Color );
    }
    
    { // Barrel
        vec2  Barrel_Dim    = bDim;
        orect Barrel_Bound  = ORectTD( bPos, Vec2( 0.0f, 0.5f ), Barrel_Dim, Radians );
        orect Barrel_BoundA = ORectCD( bPos, Vec2( Barrel_Dim.y * 0.8f, Barrel_Dim.y ), Radians );
        Barrel_BoundA = ORectAddDim( Barrel_BoundA, 0.0f, 0.0f, -Barrel_Dim.y * 0.4f, 0.0f );
        
        DrawORect( Pass, TextureID_Barrel_Rot90, Barrel_Bound, Barrel_Color );
        DrawORect( Pass, TextureID_Barrel_End, ORectTD( GetT( Barrel_Bound, Vec2( 1.0f, 0.5f ) ), Vec2( 0.0f, 0.5f ), Vec2( Barrel_Dim.y * 0.5f, Barrel_Dim.y ), Radians ), Barrel_Color );
        DrawEllipse( Pass, GetT( Barrel_BoundA, Vec2( 1.0f, 0.5f ) ), Vec2( Barrel_Dim.y * 0.8f, Barrel_Dim.y ) * 0.5f, Radians, FrontOfBarrel_Color );
        DrawORect( Pass, Barrel_BoundA, FrontOfBarrel_Color );
        DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( bPos, Vec2( Barrel_Dim.y * 0.6f, Barrel_Dim.y * 0.8f ), Radians ), InsideBarrel_Color );
    }
}

internal void
DrawSpitter( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    SPITTER_STATE * SpitterS = Draw->SpitterS;
    
    if( SpitterS->SpitBonus_IsActive ) {
        SPIT_ALT_COLORS;
        
        flo32 t = 1.0f - Clamp01( SpitterS->SpitBonus_Timer / SPITTER_SPIT_BONUS_CYCLE_TARGET_TIME );
        
        vec4 ColorA  = SpitAltColors[ SpitterS->SpitBonus_Type ];
        vec4 ColorAa = ColorA;
        ColorAa.rgb *= 0.25f;
        
        vec4 ColorB = SpitAltColors[ SpitterS->SpitBonus_NextType ];
        ColorB.rgb *= 0.5f;
        
        flo32 RadiusA = 1.0f;
        flo32 RadiusB = 0.5f;
        
        rect Bound = SpitterS->SpitBonus_Bound;
        vec2 PosA  = GetT( Bound, Vec2( 0.8f, 0.5f ) );
        vec2 PosB  = GetT( Bound, Vec2( 0.98f, 0.5f ) );
        
        vec4 Bound_ColorA = COLOR_GRAY( 0.15f );
        vec4 Bound_ColorB = COLOR_GRAY( 0.025f );
        
        DrawRect( Pass,            Bound,           COLOR_GRAY( 0.15f  ) );
        DrawRect( Pass, AddRadius( Bound, -0.25f ), COLOR_GRAY( 0.025f ) );
        
        DrawCircle( Pass, PosA, RadiusA + 0.5f,  Bound_ColorA );
        DrawCircle( Pass, PosA, RadiusA + 0.25f, Bound_ColorB );
        
        DrawCircle( Pass, PosB, RadiusB + 0.45f, Bound_ColorA );
        DrawCircle( Pass, PosB, RadiusB + 0.20f, Bound_ColorB );
        
        DrawRect( Pass, AddRadius( Bound, -0.25f ), COLOR_GRAY( 0.025f ) );
        
        DrawCircle( Pass, PosA, RadiusA, ColorAa );
        DrawCircleSegment( Pass, PosA, RadiusA, PI * 0.5f, TAU * t, true, ColorA );
        
        DrawCircle( Pass, PosB, RadiusB, ColorB );
        
        vec2 Pos = WorldToUIPos( AppState, Camera, GetT( Bound, Vec2( 0.1f, 0.5f ) ) );
        
        DrawString( Pass, AppState->FontList + FontID_1_50, "3x Bonus:", Pos, TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), TowardBlack( COLOR_YELLOW, 0.2f ) );
    }
    
    for( int32 iDamageZone = 0; iDamageZone < SpitterS->nDamageZone; iDamageZone++ ) {
        DAMAGE_ZONE DamageZone = SpitterS->DamageZone[ iDamageZone ];
        
        DrawDamageZone( Pass, DamageZone, 1.0f, 1.0f );
    }
    
    for( int32 iSpitter = 0; iSpitter < SpitterS->nSpitter; iSpitter++ ) {
        SPITTER Spitter = SpitterS->Spitter[ iSpitter ];
        
        DrawSpitter( Pass, Spitter, 1.0f, 1.0f );
    }
    
    for( int32 iLoader = 0; iLoader < SpitterS->nLoader; iLoader++ ) {
        LOADER Loader = SpitterS->Loader[ iLoader ];
        
        DrawLoader( Pass, AppState, Camera, Loader, 1.0f, 1.0f );
    }
    
    for( int32 iBarrel = 0; iBarrel < SpitterS->nBarrel; iBarrel++ ) {
        BARREL Barrel = SpitterS->Barrel[ iBarrel ];
        
        DrawBarrel( Pass, AppState, Camera, Barrel, 1.0f, 1.0f );
    }
}

internal void
DrawSpit( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    SPITTER_STATE * SpitterS = Draw->SpitterS;
    
    for( int32 iSpit = 0; iSpit < SpitterS->nSpit; iSpit++ ) {
        SPIT Spit = SpitterS->Spit[ iSpit ];
        
        DrawSpit( Pass, Spit, 1.0f, 1.0f );
    }
}

internal void
FinalizeSpitter( APP_STATE * AppState ) {
    SCORE_STATE   * ScoreS   = &AppState->ScoreS;
    SPITTER_STATE * SpitterS = &AppState->SpitterS;
    
    { // SPIT
        rect Remove_Bound = AddRadius( AppState->World_Bound, 2.0f );
        
        uint32 NewCount = 0;
        for( int32 iSpit = 0; iSpit < SpitterS->nSpit; iSpit++ ) {
            SPIT Spit = SpitterS->Spit[ iSpit ];
            
            if( Spit.Health <= 0 ) {
                int32 bPoints = 2;
                
                if( ( !Spit.IsSterilized ) && ( SpitterS->SpitBonus_IsActive ) && ( SpitterS->SpitBonus_Type == Spit.Type ) ) {
                    bPoints *= 3;
                    
                    SpitterS->nSpitBonusTotal += 1;
                }
                
                AddKillToScore( AppState, EntityType_Spit, Spit.Position, bPoints );
            }
            
            if( Spit.Health > 0 ) {
                if( IsInRect( Spit.Position, Remove_Bound ) ) {
                    SpitterS->Spit[ NewCount++ ] = Spit;
                } else {
                    AddEntityToMissedCountForLevel( AppState, EntityType_Spit, 1 );
                }
            }
        }
        SpitterS->nSpit = NewCount;
    }
    
    { // BARREL
        uint32 NewCount = 0;
        for( int32 iBarrel = 0; iBarrel < SpitterS->nBarrel; iBarrel++ ) {
            BARREL Barrel = SpitterS->Barrel[ iBarrel ];
            
            if( Barrel.Health > 0 ) {
                SpitterS->Barrel[ NewCount++ ] = Barrel;
            } else {
                //AddKillToScore( AppState, EntityType_Barrel, Barrel.Position );
                
                AddEntityToKilledCountForLevel( AppState, EntityType_Barrel, 1 );
                
                int32 nScoreMultiplier = 3;
                
                vec2 Pos = ClampToRect( Barrel.Position, AppState->World_Bound );
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                    AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                }
                
                AddEntityDeathParticles( AppState, Barrel.Position, BARREL_BASE_COLOR );
            }
        }
        SpitterS->nBarrel = NewCount;
    }
    
    { // LOADER
        uint32 NewCount = 0;
        for( int32 iLoader = 0; iLoader < SpitterS->nLoader; iLoader++ ) {
            LOADER Loader = SpitterS->Loader[ iLoader ];
            
            if( Loader.Health > 0 ) {
                SpitterS->Loader[ NewCount++ ] = Loader;
            } else {
                AddEntityDeathParticles( AppState, Loader.Position, LOADER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Loader.Position, LOADER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Loader.Position, LOADER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Loader.Position, COLOR_WHITE );
                
                SpitterS->BonusObjective_nLoaderKills += 1;
                
                AddEntityToKilledCountForLevel( AppState, EntityType_Loader, 1 );
            }
        }
        SpitterS->nLoader = NewCount;
    }
}