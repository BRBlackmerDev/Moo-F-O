
internal SATELLITE
SatelliteC( int32 WaveGuid, vec2 Position ) {
    SATELLITE Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Velocity = Vec2( 2.0f, 4.0f );
    Result.Radius   = SATELLITE_RADIUS;
    Result.Health   = SATELLITE_HEALTH;
    
    return Result;
}

internal void
AddSatellite( APP_STATE * AppState, SATELLITE Satellite ) {
    SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
    if( SatelliteS->nSatellite < SATELLITE_MAX_COUNT ) {
        SatelliteS->Satellite[ SatelliteS->nSatellite++ ] = Satellite;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SATELLITE to SATELLITE_STATE: %u", SATELLITE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateSatellite( APP_STATE * AppState, flo32 dT ) {
    SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
    
    if( SatelliteS->nSatellite > 0 ) {
        for( int32 iShieldHit = 0; iShieldHit < SATELLITE_SHIELD_HIT_MAX_COUNT; iShieldHit++ ) {
            SATELLITE_SHIELD_HIT * ShieldHit = SatelliteS->ShieldHit + iShieldHit;
            ShieldHit->Timer = MaxValue( ShieldHit->Timer - dT, 0.0f );
        }
    }
    
    for( int32 iSatellite = 0; iSatellite < SatelliteS->nSatellite; iSatellite++ ) {
        SATELLITE * Satellite = SatelliteS->Satellite + iSatellite;
        
        Satellite->Damage_Timer = MaxValue( Satellite->Damage_Timer - dT, 0.0f );
        
        if( Satellite->IsDead ) {
            Satellite->IsDead_Timer += dT;
            
            Satellite->Spark_Timer += dT;
            if( Satellite->Spark_Timer >= Satellite->Spark_TargetTime ) {
                Satellite->Spark_Timer     -= Satellite->Spark_TargetTime;
                Satellite->Spark_TargetTime = RandomF32InRange( 12.0f, 60.0f ) / 60.0f;
                
                AddBombLauncherSparks( AppState, Satellite->Position );
            }
            
            SatelliteS->ShieldDown_Timer = MinValue( SatelliteS->ShieldDown_Timer + dT, SATELLITE_SHIELD_DOWN_TARGET_TIME );
        }
        
        Satellite->Anim_Timer += dT;
        if( Satellite->Anim_Timer >= SATELLITE_ANIM_TARGET_TIME ) {
            Satellite->Anim_Timer = -SATELLITE_ANIM_BLIP_TARGET_TIME;
        }
        
        Satellite->Anim_Radians = WrapRadians( Satellite->Anim_Radians - SATELLITE_ANIM_ROTATION_SPEED * dT );
        
        { // MOVEMENT
            vec2 Speed = {};
            Speed.x = 2.0f;
            Speed.y = 4.0f;
            if( Satellite->Health <= ( SATELLITE_HEALTH -  20 ) ) {
                Speed.y =  6.0f;
            }
            if( Satellite->Health <= ( SATELLITE_HEALTH -  50 ) ) {
                Speed.y =  8.0f;
            }
            if( Satellite->Health <= ( SATELLITE_HEALTH -  80 ) ) {
                Speed.y = 10.0f;
            }
            if( Satellite->Health <= ( SATELLITE_HEALTH - 120 ) ) {
                Speed.x =  4.0f;
                Speed.y = 12.0f;
            }
            if( Satellite->Health <= ( SATELLITE_HEALTH - 150 ) ) {
                Speed.x =  6.0f;
                Speed.y = 14.0f;
            }
            
            
            vec2 dPos = Satellite->Velocity * dT;
            
            flo32 CooldownTargetTime = ( 60.0f / 60.0f );
            if( Satellite->Health <= ( SATELLITE_HEALTH - 20 ) ) {
                CooldownTargetTime = ( 30.0f / 60.0f );
            }
            if( Satellite->Health <= ( SATELLITE_HEALTH - 50 ) ) {
                CooldownTargetTime = 0.0f;
            }
            Satellite->Move_CooldownTimer = Clamp( Satellite->Move_CooldownTimer - dT, 0.0f, CooldownTargetTime );
            if( Satellite->Move_CooldownTimer > 0.0f ) {
                dPos.y = 0.0f;
            }
            
            flo32 tSpeed = 1.0f - Clamp01( Satellite->IsDead_Timer / ( 120.0f / 60.0f ) );
            dPos *= tSpeed;
            
            Satellite->Position += dPos;
            
            vec2 Pos   = Satellite->Position;
            vec2 Vel   = Satellite->Velocity;
            rect Bound = SatelliteS->Move_Bound;
            
            flo32 tX = 1.0f - GetT01( Satellite->Health, SATELLITE_HEALTH - 150, SATELLITE_HEALTH - 80 );
            flo32 tY = 1.0f - GetT01( Satellite->Health, SATELLITE_HEALTH - 150, SATELLITE_HEALTH - 80 );
            Bound.Bottom -= tY * 4.0f;
            Bound.Top    += tY * 4.0f;
            
            if( Satellite->Health <= ( SATELLITE_HEALTH - 80 ) ) {
                Bound.Left  -= lerp( 0.5f, tX, 1.0f );
                Bound.Right += lerp( 0.5f, tX, 2.0f );
            }
            
            if( ( Pos.x <= Bound.Left   ) && ( Vel.x < 0.0f ) ) {
                Satellite->Velocity.x         =  Speed.x;
            }
            if( ( Pos.x >= Bound.Right  ) && ( Vel.x > 0.0f ) ) {
                Satellite->Velocity.x         = -Speed.x;
            }
            if( ( Pos.y <= Bound.Bottom ) && ( Vel.y < 0.0f ) ) {
                Satellite->Move_CooldownTimer =  CooldownTargetTime;
                Satellite->Velocity.y         =  Speed.y;
            }
            if( ( Pos.y >= Bound.Top    ) && ( Vel.y > 0.0f ) ) {
                Satellite->Move_CooldownTimer =  CooldownTargetTime;
                Satellite->Velocity.y         = -Speed.y;
            }
            
            Satellite->Position = ClampToRect( Satellite->Position, Bound );
            
            flo32 tShield = 1.0f - GetT01( Satellite->Health, 20, SATELLITE_HEALTH - 20 );
            flo32 Left    = SatelliteS->Shield_bBound.Left - tShield * 6.0f;
            flo32 Rate    = 0.25f;
            SatelliteS->Shield_Bound.Left = MaxValue( SatelliteS->Shield_Bound.Left - Rate * dT, Left );
        }
        
        { // SWARM
            SWARM_STATE * SwarmS = &AppState->SwarmS;
            for( int32 iSwarm = 0; ( Satellite->Health > 0 ) && ( iSwarm < SwarmS->nSwarm ); iSwarm++ ) {
                SWARM * Swarm = SwarmS->Swarm + iSwarm;
                
                flo32 Radius   = ( Satellite->Radius * 0.9f ) + Swarm->Radius - 0.1f;
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( Swarm->Position - Satellite->Position );
                if( DistSq <= RadiusSq ) {
                    Swarm->Health = 0;
                    
                    Satellite->Health       = MaxValue( Satellite->Health - 1, 0 );
                    Satellite->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                    
                    vec2 Pos   = Swarm->Position;
                    vec2 Dir   = GetNormal( Swarm->Velocity );
                    vec2 Norm  = GetNormal( Swarm->Position - Satellite->Position );
                    vec4 Color = SWARM_BASE_COLOR;
                    
                    AddBulletHitParticles( AppState, Pos, Dir, Norm, Color );
                    AddBulletHitParticles( AppState, Pos, Dir, Norm, Color );
                    AddBulletHitParticles( AppState, Pos, Dir, Norm, Color );
                    AddBulletHitParticles( AppState, Pos, Dir, Norm, COLOR_WHITE );
                }
            }
        }
    }
}

internal void
DrawSatellite( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, SATELLITE Satellite, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    
    vec2  Pos    = Satellite.Position;
    flo32 Radius = Satellite.Radius * Scale;
    
    {
        vec4  ColorE = COLOR_GRAY( 0.05f );
        DrawORect( Pass, ORectBCD( Pos, Vec2( 0.5f, 10.0f ), 0.0f ), ColorE );
        DrawORect( Pass, ORectBCD( Pos + Vec2( 7.0f, 0.0f ), Vec2( 0.35f, 10.0f ),  PI * 0.15f ), ColorE );
        DrawORect( Pass, ORectBCD( Pos + Vec2( 7.0f, 0.0f ), Vec2( 0.35f, 10.0f ), -PI * 0.15f ), ColorE );
    }
    
    if( !Satellite.IsDead ) {
        vec4 ColorC = TowardBlack( SATELLITE_BASE_COLOR, 0.55f );
        vec4 ColorB = TowardBlack( ColorC, 0.3333f );
        vec4 ColorA = TowardBlack( ColorC, 0.5f );
        
        {
            flo32 tDamage = Clamp01( Satellite.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
            ColorA = lerp( ColorA, tDamage, TowardWhite( ColorA, 0.35f ) );
        }
        
        DrawCircle( Pass, Pos, Radius,        ColorA );
        
        DrawRect( Pass, TextureID_Dome, RectCR( Pos, Radius * 0.5f ), ColorB );
        //DrawCircle( Pass, Pos, Radius * 0.5f, ColorB );
        
        flo32 Radians = Satellite.Anim_Radians;
        vec2  Dir     = ToDirection2D( Radians );
        
        flo32 Offset  = Radius * 0.05f;
        vec2  PosA    = Pos + Dir * Offset;
        flo32 RadiusA = Radius * 0.75f;
        vec2  DimA    = Vec2( Radius * 0.0575f, RadiusA );
        orect BoundA  = ORectCD( PosA, Vec2( DimA.x, RadiusA ), Radians + PI * 0.5f );
        DrawORect( Pass, BoundA, ColorC );
        
        vec4 ColorD = COLOR_BLACK;
        if( Satellite.Anim_Timer <= 0.0f ) {
            ColorD = Vec4( 0.5f, 0.0f, 0.0f, 1.0f );
        }
        
        vec2  PosB    = GetT( BoundA, Vec2( 0.5f, 1.0f ) );
        orect BoundB  = ORectCD( PosB, Vec2( DimA.x, DimA.x ) * 1.5f, Radians );
        DrawORect( Pass, BoundB, ColorD );
        
        //DrawCircleSegment( Pass, PosA, RadiusA, Radians - PI * 0.5f, PI, false, ColorC );
        orect BoundC = ORectTD( PosA, Vec2( 0.0f, 0.5f ), Vec2( RadiusA, RadiusA * 2.0f ), Radians );
        DrawORect( Pass, TextureID_SatelliteDish, BoundC, ColorC );
        
        {
            rect Bound = RectTCD( Pos + Vec2( 0.0f, -( Radius - 0.2f ) ), Vec2( 2.0f, 0.85f ) );
            DrawRect( Pass, Bound, COLOR_GRAY( 0.05f ) );
            
            if( Satellite.Health > 0 ) {
                DrawEntityHealth( Pass, AppState, Camera, GetCenter( Bound ), Satellite.Health, COLOR_WHITE );
            }
        }
    }
}

internal void
DrawSatellite( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    SATELLITE_STATE * SatelliteS = Draw->SatelliteS;
    
    for( int32 iSatellite = 0; iSatellite < SatelliteS->nSatellite; iSatellite++ ) {
        SATELLITE Satellite = SatelliteS->Satellite[ iSatellite ];
        
        DrawSatellite( Pass, AppState, Camera, Satellite );
    }
    
    { // Shield
        if( SatelliteS->nSatellite > 0 ) {
            {
                rect Bound  = SatelliteS->Shield_Bound;
                Bound.Left -= 1.0f;
                Bound.Right = Bound.Left + 20.0f;
                
                flo32 tAlpha = 1.0f - Clamp01( SatelliteS->ShieldDown_Timer / SATELLITE_SHIELD_DOWN_TARGET_TIME );
                
                flo32 TargetTimeA = (  4.0f / 60.0f );
                flo32 TargetTimeB = (  8.0f / 60.0f );
                flo32 TargetTimeC = ( 12.0f / 60.0f );
                flo32 TargetTimeD = ( 16.0f / 60.0f );
                flo32 TargetTimeE = ( 22.0f / 60.0f );
                flo32 TargetTimeF = ( 28.0f / 60.0f );
                
                if( ( SatelliteS->ShieldDown_Timer >= TargetTimeA ) && ( SatelliteS->ShieldDown_Timer < TargetTimeB ) ) {
                    flo32 t = GetT01( SatelliteS->ShieldDown_Timer, TargetTimeA, TargetTimeB );
                    if( t < 0.5f ) {
                        t = 1.0f - t;
                    }
                    tAlpha = 0.35f * t;
                }
                if( ( SatelliteS->ShieldDown_Timer >= TargetTimeC ) && ( SatelliteS->ShieldDown_Timer < TargetTimeD ) ) {
                    flo32 t = GetT01( SatelliteS->ShieldDown_Timer, TargetTimeC, TargetTimeD );
                    if( t < 0.5f ) {
                        t = 1.0f - t;
                    }
                    tAlpha = 0.35f;
                }
                if( ( SatelliteS->ShieldDown_Timer >= TargetTimeE ) && ( SatelliteS->ShieldDown_Timer < TargetTimeF ) ) {
                    flo32 t = GetT01( SatelliteS->ShieldDown_Timer, TargetTimeE, TargetTimeF );
                    if( t < 0.5f ) {
                        t = 1.0f - t;
                    }
                    tAlpha = 0.35f;
                }
                
                DrawRect( Pass, TextureID_SatelliteDish_Shield, Bound, COLOR_WHITE * ( 0.8f * tAlpha ) );
            }
            
            for( int32 iShieldHit = 0; iShieldHit < SATELLITE_SHIELD_HIT_MAX_COUNT; iShieldHit++ ) {
                SATELLITE_SHIELD_HIT ShieldHit = SatelliteS->ShieldHit[ iShieldHit ];
                
                flo32 Margin = 0.75f;
                vec2  Dim    = Vec2( 2.0f, 8.0f ) * ShieldHit.Scale;
                Dim.x += Margin;
                
                vec2  P     = ShieldHit.Pos + Vec2( -Margin, 0.0f );
                rect  R     = RectTD( P, Vec2( 0.0f, 0.5f ), Dim );
                flo32 Alpha = Clamp01( ShieldHit.Timer / SATELLITE_SHIELD_HIT_TARGET_TIME );
                DrawRect( Pass, TextureID_SatelliteDish_ShieldHit, R, COLOR_WHITE * ( Alpha * Alpha ) );
            }
        }
    }
}

internal void
FinalizeSatellite( APP_STATE * AppState ) {
    SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
    
    for( int32 iSatellite = 0; iSatellite < SatelliteS->nSatellite; iSatellite++ ) {
        SATELLITE * Satellite = SatelliteS->Satellite + iSatellite;
        
        if( ( Satellite->Health <= 0 ) && ( !Satellite->IsDead ) ) {
            Satellite->IsDead = true;
            
            AddBigEntityDeathParticles( AppState, Satellite->Position, Satellite->Radius, SATELLITE_BASE_COLOR );
            AddBigEntityDeathParticles( AppState, Satellite->Position, Satellite->Radius, SATELLITE_BASE_COLOR );
            AddBigEntityDeathParticles( AppState, Satellite->Position, Satellite->Radius, SATELLITE_BASE_COLOR );
            AddBigEntityDeathParticles( AppState, Satellite->Position, Satellite->Radius, SATELLITE_BASE_COLOR );
            AddBigEntityDeathParticles( AppState, Satellite->Position, Satellite->Radius, COLOR_WHITE, false );
            
            AddEntityDeathParticles( AppState, Satellite->Position, SATELLITE_BASE_COLOR );
            AddEntityDeathParticles( AppState, Satellite->Position, SATELLITE_BASE_COLOR );
            AddKillToWave( AppState, Satellite->WaveGuid );
            
            SatelliteS->BonusObjective_nSatelliteKills += 1;
        }
    }
}