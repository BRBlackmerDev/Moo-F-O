
internal MISSILE
MissileC( int32 WaveGuid, vec2 Position, vec2 Velocity, boo32 DoTargetEscort = false ) {
    MISSILE Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Velocity = Velocity;
    Result.Health   = 1;
    Result.Friction = 0.5f;
    
    Result.Exhaust_Timer  = -MISSILE_EXHAUST_WARMUP_TARGET_TIME;
    Result.DoTargetEscort = DoTargetEscort;
    
    return Result;
}

internal void
AddMissile( APP_STATE * AppState, MISSILE Missile ) {
    MISSILE_STATE * MissileS = &AppState->MissileS;
    if( MissileS->nMissile < MISSILE_MAX_COUNT ) {
        MissileS->Missile[ MissileS->nMissile++ ] = Missile;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add MISSILE to MISSILE_STATE: %u", MISSILE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateMissile( APP_STATE * AppState, flo32 dT ) {
    MISSILE_STATE  * MissileS = &AppState->MissileS;
    PLAYER_STATE * Player = &AppState->Player;
    
    for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
        MISSILE * Missile = MissileS->Missile + iMissile;
        
        { // EXHAUST
            Missile->Exhaust_Timer += dT;
            if( Missile->Exhaust_Timer >= Missile->Exhaust_TargetTime ) {
                Missile->Exhaust_Timer     -= Missile->Exhaust_TargetTime;
                Missile->Exhaust_TargetTime = ( flo32 )RandomS32InRange( 4, 8 ) / 60.0f;
                
                PARTICLE Particle = {};
                Particle.TextureID = TextureID_Dome;
                
                vec2  Dir        = -GetNormal( Missile->Velocity );
                flo32 Radians    = ToRadians( Dir ) + ( RandomF32InRange( -1.0f, 1.0f ) * ( PI / 6.0f ) );
                flo32 Speed      = RandomF32InRange( 3.0f, 5.0f );
                flo32 TargetTime = ( flo32 )RandomS32InRange( 30, 50 ) / 60.0f;
                
                Particle.Position  = Missile->Position + Dir * MISSILE_LENGTH;
                Particle.Velocity  = Dir * Speed;
                Particle.Friction  = 2.0f;
                Particle.Dim       = Vec2( 0.25f, 0.25f ) * RandomF32InRange( 0.8f, 1.2f );
                Particle.dDim      = Vec2( 1.5f,  1.5f  ) * ( RandomF32InRange( 0.9f, 1.1f ) / TargetTime );
                
                Particle.TargetTime = TargetTime;
                
                flo32 tGray = RandomF32InRange( 0.25f, 0.75f );
                Particle.Color      = COLOR_GRAY( tGray ) * RandomF32InRange( 0.6f, 0.8f );
                Particle.dColor     = Vec4( 0.0f, 0.0f, 0.0f, -1.0f / TargetTime );
                
                AddParticle( AppState, Particle );
            }
        }
        
        for( int32 iRepel = iMissile + 1; iRepel < MissileS->nMissile; iRepel++ ) {
            MISSILE * Repel = MissileS->Missile + iRepel;
            
            vec2 V = Repel->Position - Missile->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = MISSILE_REPEL_RADIUS + MISSILE_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * MISSILE_REPEL_SPEED;
                
                Missile->Repel -= Repel_Vector;
                Repel->Repel += Repel_Vector;
            }
        }
        
        flo32 dFriction = 1.0f / 3.0f;
        Missile->Friction = MinValue( Missile->Friction + dFriction * dT, MISSILE_FRICTION );
        
        vec2 Dir   = GetNormal( Player->Position - Missile->Position );
#if 0
        if( Missile->DoTargetEscort ) {
            ESCORT_STATE * EscortS = &AppState->EscortS;
            if( EscortS->nEscort > 0 ) {
                Assert( EscortS->nEscort == 1 );
                
                ESCORT Escort = EscortS->Escort[ 0 ];
                Dir = GetNormal( Escort.Position - Missile->Position );
            }
        }
#endif
        flo32 Speed = MISSILE_SPEED;
        if( Missile->Exhaust_Timer < 0.0f ) {
            Speed    = 0.0f;
        }
        vec2 Accel = ( Dir * Speed - Missile->Velocity + Missile->Repel ) * Missile->Friction;
        if( Player->GravityWell_IsActive ) {
            Accel += GetPerp( Dir ) * ( Player->Rotate_Speed * MISSILE_FRICTION * 12.0f );
        }
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Missile->Velocity * dT;
        // TODO: Collision!
        Missile->Position += dPos;
        Missile->Velocity += Accel * dT;
        
        Missile->Repel = {};
    }
    
    if( Player->Shield_IsActive ) {
        for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
            MISSILE * Missile = MissileS->Missile + iMissile;
            
            flo32 Radius   = ( PLAYER_SHIELD_RADIUS + MISSILE_RADIUS - 0.1f );
            
            vec2 A = Missile->Position;
            vec2 B = A - GetNormal( Missile->Velocity ) * MISSILE_LENGTH;
            CAPSULE2 Cap = Capsule2( A, B, Radius );
            
            if( IsInCapsule( Player->Position, Cap ) ) {
                Missile->Health = 0;
            }
        }
    }
}

internal void
DrawMissile( RENDER_PASS * Pass, MISSILE Missile, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 Radians = ToRadians( Missile.Velocity );
    
    vec4 Body_Color = TowardBlack( MISSILE_BASE_COLOR, 0.5f );
    vec4 Tip_Color  = TowardBlack( MISSILE_BASE_COLOR, 0.4f );
    vec4 Fire_Color = COLOR_RED;
    
    vec2 Dim = Vec2( MISSILE_LENGTH, MISSILE_RADIUS * 2.0f ) * Scale;
    
    vec2 A = Missile.Position;
    vec2 B = A - GetNormal( Missile.Velocity ) * MISSILE_LENGTH;
    
    orect Bound = ORectCD( ( A + B ) * 0.5f, Dim, Radians );
    
    DrawORect( Pass, TextureID_Barrel_End, ORectTD( GetT( Bound, Vec2( 1.0f, 0.5f ) ), Vec2( 0.0f, 0.5f ), Vec2( Dim.y * 0.5f, Dim.y ), Radians ), Tip_Color * Alpha );
    
    DrawORect( Pass, TextureID_Barrel_Rot90, ORectCD( ( A + B ) * 0.5f, Dim, Radians ), Body_Color * Alpha );
    
    if( Missile.Exhaust_Timer >= 0.0f ) {
        vec2 Sca = Vec2( Dim.y * 0.8f, Dim.y ) * 0.5f;
        Sca.y *= RandomF32InRange( 1.0f, 2.0f );
        DrawTriangleBC( Pass, GetT( Bound, Vec2( 0.0f, 0.5f ) ), Sca, Radians + PI * 0.5f, Fire_Color * Alpha );
    }
}

internal void
DrawMissile( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    MISSILE_STATE * MissileS = Draw->MissileS;
    
    for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
        MISSILE Missile = MissileS->Missile[ iMissile ];
        
        DrawMissile( Pass, Missile );
    }
}

internal void
FinalizeMissile( APP_STATE * AppState ) {
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    MISSILE_STATE * MissileS = &AppState->MissileS;
    
    uint32 NewCount = 0;
    for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
        MISSILE Missile = MissileS->Missile[ iMissile ];
        
        if( Missile.Health > 0 ) {
            MissileS->Missile[ NewCount++ ] = Missile;
        } else {
            AddKillToScore( AppState, EntityType_Missile, Missile.Position );
            
            //uint32 nScoreMultiplier = 1;
            //if( RandomF32() >= 0.8f ) {
            //nScoreMultiplier += 1;
            //}
            
            //for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
            //AddScoreMultiplier( AppState, NewScoreMultiplier( Missile.Position, AppState->World_Bound, true ) );
            //}
            
            AddMissileDeathParticles( AppState, Missile.Position );
            AddKillToWave( AppState, Missile.WaveGuid );
        }
    }
    MissileS->nMissile = NewCount;
}