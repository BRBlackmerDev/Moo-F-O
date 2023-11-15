
internal SCORE_MULTIPLIER
NewScoreMultiplier( vec2 Position, rect World_Bound, boo32 DoExplode = false, boo32 IsBonusPoints = false ) {
    vec2  Direction = ToDirection2D( RandomF32() * TAU );
    
    vec2  Dest = Position + Direction * 2.0f;
    if( DoExplode ) {
        Dest = Position + Direction * 8.0f;
    }
    if( ( Dest.x <= World_Bound.Left ) || ( Dest.x >= World_Bound.Right ) ) {
        Direction.x = -Direction.x;
    }
    if( ( Dest.y <= World_Bound.Bottom ) || ( Dest.y >= World_Bound.Top ) ) {
        Direction.y = -Direction.y;
    }
    
    flo32 Speed     = RandomF32InRange( 1.0f, 2.0f );
    flo32 Dist      = RandomF32InRange( 0.0f, 1.0f );
    flo32 Friction  = RandomF32InRange( 2.0f, 3.0f );
    vec2  Velocity  = Direction * Speed;
    if( DoExplode ) {
        Velocity = Direction * ( Speed * 10.0f );
    }
    
    SCORE_MULTIPLIER Result = {};
    Result.IsBonusPoints = IsBonusPoints;
    Result.Position   = Position + Direction * Dist;
    Result.Velocity   = Velocity;
    Result.Direction  = Direction;
    Result.Friction   = Friction;
    Result.Speed      = Speed;
    return Result;
}

internal SCORE_MULTIPLIER
Star_NewScoreMultiplier( vec2 Position, vec2 Direction ) {
    flo32 Speed     = RandomF32InRange( 1.0f, 2.0f );
    flo32 Friction  = RandomF32InRange( 2.0f, 3.0f );
    vec2  Velocity  = Direction * Speed * 4.0f;
    
    SCORE_MULTIPLIER Result = {};
    Result.Position  = Position;
    Result.Velocity  = Velocity;
    Result.Direction = Direction;
    Result.Friction  = Friction;
    Result.Speed     = Speed;
    return Result;
}

internal void
AddScoreMultiplier( APP_STATE * AppState, SCORE_MULTIPLIER ScoreMultiplier ) {
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
    if( ScoreMultiplierS->nScoreMultiplier < SCORE_MULTIPLIER_MAX_COUNT ) {
        //if( ScoreMultiplierS->Powerup_Timer > 0.0f ) {
        //ScoreMultiplier.Life_Timer -= ScoreMultiplierS->Powerup_TimeToAdd;
        //}
        
        ScoreMultiplierS->ScoreMultiplier[ ScoreMultiplierS->nScoreMultiplier++ ] = ScoreMultiplier;
        
        ScoreMultiplierS->nScoreMultiplierSpawned += 1;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SCORE_MULTIPLIER to SCORE_MULTIPLIER_STATE: %u", SCORE_MULTIPLIER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal SCORE_MULTIPLIER_SPAWNER
ScoreMultiplierSpawnerC( vec2 Position, int32 nScoreMultiplier ) {
    SCORE_MULTIPLIER_SPAWNER Spawner = {};
    Spawner.Position         = Position;
    Spawner.nScoreMultiplier = nScoreMultiplier;
    
    return Spawner;
}

internal void
AddScoreMultiplierSpawner( APP_STATE * AppState, SCORE_MULTIPLIER_SPAWNER Spawner ) {
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
    if( ScoreMultiplierS->nSpawner < SCORE_MULTIPLIER_SPAWNER_MAX_COUNT ) {
        ScoreMultiplierS->Spawner[ ScoreMultiplierS->nSpawner++ ] = Spawner;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SCORE_MULTIPLIER_SPAWNER to SCORE_MULTIPLIER_STATE: %u", SCORE_MULTIPLIER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateScoreMultiplier( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE           * Player           = &AppState->Player;
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
    
    { // Update Score Multiplier Powerup
        ScoreMultiplierS->Powerup_Timer = MaxValue( ScoreMultiplierS->Powerup_Timer - dT, 0.0f );
    }
    
    for( int32 iSpawner = 0; iSpawner < ScoreMultiplierS->nSpawner; iSpawner++ ) {
        SCORE_MULTIPLIER_SPAWNER * Spawner = ScoreMultiplierS->Spawner + iSpawner;
        AddScoreMultiplier( AppState, NewScoreMultiplier( Spawner->Position, AppState->World_Bound, true ) );
        
        Spawner->nScoreMultiplier--;
    }
    
    for( int32 iScoreMultiplier = 0; iScoreMultiplier < ScoreMultiplierS->nScoreMultiplier; iScoreMultiplier++ ) {
        SCORE_MULTIPLIER * ScoreMultiplier = ScoreMultiplierS->ScoreMultiplier + iScoreMultiplier;
        ScoreMultiplier->Life_Timer += dT;
        
        // TODO: Accel! Gravity Well!
        
        flo32 dRadians = SCORE_MULTIPLIER_ROTATE_SPEED;
        if( ScoreMultiplierS->Powerup_Timer > 0.0f ) {
            dRadians *= 3.0f;
        }
        ScoreMultiplier->Radians = WrapRadians( ScoreMultiplier->Radians + dRadians * dT );
        
        if( ( Player->TractorBeam_IsActive ) || ( ScoreMultiplierS->Powerup_Timer > 0.0f ) ) {
            flo32 Speed = 24.0f;
            if( ( Player->TractorBeam_IsActive ) && ( ScoreMultiplierS->Powerup_Timer > 0.0f ) ) {
                Speed = 48.0f;
            }
            
            ScoreMultiplier->Direction = GetNormal( Player->Position - ScoreMultiplier->Position );
            ScoreMultiplier->Speed     = Speed;
        } else {
            ScoreMultiplier->Speed     = MinValue( ScoreMultiplier->Speed, 2.0f );
        }
        
        if( !ScoreMultiplier->DoSeek ) {
            SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
            if( SatelliteS->nSatellite > 0 ) {
                rect Bound = SatelliteS->Shield_Bound;
                if( IsInRect( ScoreMultiplier->Position, Bound ) ) {
                    ScoreMultiplier->Position = ClosestPointToRectPerimeter( ScoreMultiplier->Position, Bound );
                    ScoreMultiplier->Velocity -= ( 2.0f * dot( ScoreMultiplier->Velocity, SatelliteS->Shield_BounceDir ) ) * SatelliteS->Shield_BounceDir;
                    ScoreMultiplier->Direction = GetNormal( ScoreMultiplier->Velocity );
                    
                    SATELLITE_SHIELD_HIT ShieldHit = {};
                    ShieldHit.Pos   = ScoreMultiplier->Position;
                    ShieldHit.Scale = RandomF32InRange( 0.25f, 0.35f );
                    ShieldHit.Timer = SATELLITE_SHIELD_HIT_TARGET_TIME * 0.75f;
                    
                    SatelliteS->ShieldHit[ SatelliteS->iShieldHit ] = ShieldHit;
                    SatelliteS->iShieldHit = ( SatelliteS->iShieldHit + 1 ) % SATELLITE_SHIELD_HIT_MAX_COUNT;
                }
            }
        }
        
        vec2  ToPlayer   = Player->Position - ScoreMultiplier->Position;
        flo32 DistSq     = GetLengthSq( ToPlayer );
        {
            flo32 Radius     = SCORE_MULTIPLIER_PULL_RADIUS;
            //if( ScoreMultiplierS->Powerup_Timer > 0.0f ) {
            //Radius += ScoreMultiplierS->Powerup_RadiusToAdd;
            //}
            flo32 RadiusSq   = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                ScoreMultiplier->DoSeek = true;
            }
        }
        
        if( Player->Shield_IsActive ) {
            ScoreMultiplier->DoSeek = false;
        }
        //flo32 Pull_Speed = ( 1.0f - Clamp01( DistSq / RadiusSq ) ) * 14.0f;
        
        vec2 Accel = ( ScoreMultiplier->Direction * ScoreMultiplier->Speed - ScoreMultiplier->Velocity ) * ScoreMultiplier->Friction;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + ScoreMultiplier->Velocity * dT;
        
        if( ScoreMultiplier->DoSeek ) {
            flo32 Pull_Speed = 10.0f;
            if( GetLengthSq( ToPlayer ) <= 1.0f ) {
                ToPlayer = GetNormal( ToPlayer );
            }
            dPos += ToPlayer * Pull_Speed * dT;
        }
        
        if( ( Player->Shield_IsActive ) && ( DistSq <= ( PLAYER_SHIELD_RADIUS * PLAYER_SHIELD_RADIUS ) ) ) {
            dPos += Player->dPos;
        }
        
        ScoreMultiplier->Position += dPos;
        ScoreMultiplier->Velocity += Accel * dT;
    }
    //DISPLAY_VALUE( uint32, ScoreMultiplierS->nScoreMultiplier );
    DISPLAY_VALUE( int32, ScoreMultiplierS->nScoreMultiplierSpawned );
    
}

internal void
DrawBonusPoints( RENDER_PASS * Pass, SCORE_MULTIPLIER ScoreMultiplier ) {
    flo32 Alpha = 1.0f;
    
    flo32 CycleTargetTimeA = ( 24.0f / 60.0f );
    flo32 CycleTargetTimeB = (  6.0f / 60.0f );
    
    flo32 TargetTimeB = SCORE_MULTIPLIER_LIFE_TARGET_TIME - ( CycleTargetTimeB * 10.0f );
    flo32 TargetTimeA = TargetTimeB                       - ( CycleTargetTimeA * 4.0f );
    
    if( ScoreMultiplier.Life_Timer >= TargetTimeA ) {
        flo32 CycleTargetTime = CycleTargetTimeA;
        if( ScoreMultiplier.Life_Timer >= TargetTimeB ) {
            CycleTargetTime = CycleTargetTimeB;
        }
        
        flo32 Timer = ScoreMultiplier.Life_Timer - TargetTimeA;
        
        uint32 Select = ( uint32 )( Timer / CycleTargetTime );
        if( ( Select % 2 ) == 0 ) {
            Alpha = 0.25f;
        }
    }
    vec4  Color = Vec4( 1.0f, 0.5f, 0.0f, 1.0f );
    Color *= Alpha;
    
    DrawTriangleBC( Pass, ScoreMultiplier.Position, Vec2( 0.25f, 0.2f ), ScoreMultiplier.Radians, Color );
    DrawTriangleBC( Pass, ScoreMultiplier.Position, Vec2( 0.25f, 0.2f ), ScoreMultiplier.Radians + PI, Color );
}

internal void
DrawScoreMultiplier( RENDER_PASS * Pass, SCORE_MULTIPLIER ScoreMultiplier, flo32 Scale = 1.0f, flo32 Alpha = 1.0f ) {
    flo32 Alph = 1.0f;
    
    flo32 CycleTargetTimeA = ( 24.0f / 60.0f );
    flo32 CycleTargetTimeB = (  6.0f / 60.0f );
    
    flo32 TargetTimeB = SCORE_MULTIPLIER_LIFE_TARGET_TIME - ( CycleTargetTimeB * 10.0f );
    flo32 TargetTimeA = TargetTimeB                       - ( CycleTargetTimeA * 4.0f );
    
    if( ScoreMultiplier.Life_Timer >= TargetTimeA ) {
        flo32 CycleTargetTime = CycleTargetTimeA;
        if( ScoreMultiplier.Life_Timer >= TargetTimeB ) {
            CycleTargetTime = CycleTargetTimeB;
        }
        
        flo32 Timer = ScoreMultiplier.Life_Timer - TargetTimeA;
        
        uint32 Select = ( uint32 )( Timer / CycleTargetTime );
        if( ( Select % 2 ) == 0 ) {
            Alph = 0.25f;
        }
    }
    vec4  Color = SCORE_MULTIPLIER_BASE_COLOR;
    DrawORect( Pass, TextureID_ScoreMultiplier, ORectCR( ScoreMultiplier.Position, 0.25f * Scale, ScoreMultiplier.Radians ), Color * Alpha * Alph );
    //DrawTriangle( Pass, ScoreMultiplier.Position, Vec2( 0.25f, 0.25f ) * Scale, ScoreMultiplier.Radians, Color );
}

internal void
DrawScoreMultiplier( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = Draw->ScoreMultiplierS;
    
    for( int32 iScoreMultiplier = 0; iScoreMultiplier < ScoreMultiplierS->nScoreMultiplier; iScoreMultiplier++ ) {
        SCORE_MULTIPLIER ScoreMultiplier = ScoreMultiplierS->ScoreMultiplier[ iScoreMultiplier ];
        DrawScoreMultiplier( Pass, ScoreMultiplier );
    }
}

internal void
DrawScoreMultiplierDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = Draw->ScoreMultiplierS;
    
    for( int32 iScoreMultiplier = 0; iScoreMultiplier < ScoreMultiplierS->nScoreMultiplier; iScoreMultiplier++ ) {
        SCORE_MULTIPLIER ScoreMultiplier = ScoreMultiplierS->ScoreMultiplier[ iScoreMultiplier ];
        DrawCircleOutline( Pass, ScoreMultiplier.Position, SCORE_MULTIPLIER_PULL_RADIUS, COLOR_RED );
    }
}

internal void
FinalizeScoreMultiplier( APP_STATE * AppState ) {
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
    
    { // SPAWNER
        uint32 NewCount = 0;
        for( int32 iSpawner = 0; iSpawner < ScoreMultiplierS->nSpawner; iSpawner++ ) {
            SCORE_MULTIPLIER_SPAWNER Spawner = ScoreMultiplierS->Spawner[ iSpawner ];
            
            if( Spawner.nScoreMultiplier > 0 ) {
                ScoreMultiplierS->Spawner[ NewCount++ ] = Spawner;
            }
        }
        ScoreMultiplierS->nSpawner = NewCount;
    }
    
    { // SCORE MULTIPLIER
        uint32 NewCount = 0;
        for( int32 iScoreMultiplier = 0; iScoreMultiplier < ScoreMultiplierS->nScoreMultiplier; iScoreMultiplier++ ) {
            SCORE_MULTIPLIER ScoreMultiplier = ScoreMultiplierS->ScoreMultiplier[ iScoreMultiplier ];
            
            boo32 DoRemove = ( ScoreMultiplier.Life_Timer >= SCORE_MULTIPLIER_LIFE_TARGET_TIME )
                && ( !ScoreMultiplier.DoSeek );
            
            if( !DoRemove ) {
                ScoreMultiplierS->ScoreMultiplier[ NewCount++ ] = ScoreMultiplier;
            }
        }
        ScoreMultiplierS->nScoreMultiplier = NewCount;
    }
}