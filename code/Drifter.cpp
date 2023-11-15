
internal flo32
Drifter_GetChangeTargetTime() {
    flo32 Result = RandomF32InRange( 1.5f, 3.0f );
    return Result;
}

internal void
Drifter_SetRandomDirAndSpeed( DRIFTER * Drifter, rect World_Bound ) {
    flo32 t = RandomF32();
    Drifter->Move_Speed = lerp( 1.0f, t * t, 2.5f );
    
    rect  Bound   = AddRadius( World_Bound, -Drifter->Radius * 0.5f );
    flo32 Radians = WrapRadians( Drifter->Move_Radians + RandomF32InRange( -PI / 3.0f, PI / 3.0f ) );
    vec2  Dir     = ToDirection2D( Radians );
    vec2  Dest    = Drifter->Position + Dir * ( Drifter->Move_Speed * Drifter->Move_TargetTime );
    if( ( Dest.x < Bound.Left ) || ( Dest.x > Bound.Right ) ) {
        Dir.x = -Dir.x;
    }
    if( ( Dest.y < Bound.Bottom ) || ( Dest.y > Bound.Top ) ) {
        Dir.y = -Dir.y;
    }
    Drifter->Move_Radians = ToRadians( Dir );
}

internal DRIFTER
DrifterC( int32 WaveGuid, vec2 Position ) {
    DRIFTER Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Radius   = DRIFTER_RADIUS;
    Result.Health   = 1;
    Result.Move_TargetTime = RandomF32InRange( 0.15f, 2.0f );
    Result.Anim_Timer        = RandomF32InRange( 0.0f, DRIFTER_ANIM_CYCLE_TARGET_TIME );
    
    return Result;
}

internal void
AddDrifter( APP_STATE * AppState, DRIFTER Drifter ) {
    DRIFTER_STATE * DrifterS = &AppState->DrifterS;
    if( DrifterS->nDrifter < DRIFTER_MAX_COUNT ) {
        DrifterS->Drifter[ DrifterS->nDrifter++ ] = Drifter;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add DRIFTER to DRIFTER_STATE: %u", DRIFTER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateDrifter( APP_STATE * AppState, flo32 dT ) {
    DRIFTER_STATE * DrifterS = &AppState->DrifterS;
    PLAYER_STATE  * Player   = &AppState->Player;
    
    for( int32 iDrifter = 0; iDrifter < DrifterS->nDrifter; iDrifter++ ) {
        DRIFTER * Drifter = DrifterS->Drifter + iDrifter;
        
        Drifter->Blink_Timer += dT;
        if( Drifter->Blink_Timer >= Drifter->Blink_TargetTime ) {
            Drifter->Blink_TargetTime =  ( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
            Drifter->Blink_Timer      = -( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
        }
        
        Drifter->Anim_Timer   += dT;
        if( Drifter->Anim_Timer >= DRIFTER_ANIM_CYCLE_TARGET_TIME ) {
            Drifter->Anim_Timer -= DRIFTER_ANIM_CYCLE_TARGET_TIME;
        }
        
        Drifter->Move_Timer += dT;
        if( Drifter->Move_Timer >= Drifter->Move_TargetTime ) {
            Drifter->Move_Timer      = 0.0f;
            Drifter->Move_TargetTime = Drifter_GetChangeTargetTime();
            
            Drifter_SetRandomDirAndSpeed( Drifter, AppState->World_Bound );
        }
        
        for( int32 iRepel = iDrifter + 1; iRepel < DrifterS->nDrifter; iRepel++ ) {
            DRIFTER * Repel = DrifterS->Drifter + iRepel;
            
            vec2 V = Repel->Position - Drifter->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = DRIFTER_REPEL_RADIUS + DRIFTER_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * DRIFTER_REPEL_SPEED;
                
                Drifter->Repel -= Repel_Vector;
                Repel->Repel   += Repel_Vector;
            }
        }
        
        vec2 Dir   = ToDirection2D( Drifter->Move_Radians );
        vec2 Accel = ( Dir * Drifter->Move_Speed - Drifter->Velocity + Drifter->Repel ) * DRIFTER_FRICTION;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Drifter->Velocity * dT;
        // TODO: Collision!
        Drifter->Position += dPos;
        Drifter->Velocity += Accel * dT;
        
        Drifter->Repel = {};
    }
}

internal void
DrawDrifter( RENDER_PASS * Pass, DRIFTER Drifter, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 Radians    = ToRadians( Drifter.Velocity );
    flo32 Orb_Radius = ( Drifter.Radius + 0.1f ) * Scale;
    flo32 Len        = Drifter.Radius * 0.4f * Scale;
    vec2 A = ToDirection2D( Radians )                    * Len;
    vec2 B = ToDirection2D( Radians + PI * 2.0f / 3.0f ) * Len;
    vec2 C = ToDirection2D( Radians + PI * 4.0f / 3.0f ) * Len;
    
    flo32 TimeA = Drifter.Anim_Timer;
    flo32 TimeB = Drifter.Anim_Timer + DRIFTER_ANIM_CYCLE_TARGET_TIME * 1.0f / 3.0f;
    flo32 TimeC = Drifter.Anim_Timer + DRIFTER_ANIM_CYCLE_TARGET_TIME * 2.0f / 3.0f;
    flo32 tA = cosf( ( TimeA / DRIFTER_ANIM_CYCLE_TARGET_TIME ) * TAU ) * 0.5f + 0.5f;
    flo32 tB = cosf( ( TimeB / DRIFTER_ANIM_CYCLE_TARGET_TIME ) * TAU ) * 0.5f + 0.5f;
    flo32 tC = cosf( ( TimeC / DRIFTER_ANIM_CYCLE_TARGET_TIME ) * TAU ) * 0.5f + 0.5f;
    
    vec4 ColorA = DRIFTER_BASE_COLOR;
    vec4 ColorB = TowardBlack( ColorA, 0.2f );
    vec4 ColorC = TowardBlack( ColorA, 0.35f  );
    
    rect BoundA = RectCR( Drifter.Position + A * tA, Orb_Radius );
    rect BoundB = RectCR( Drifter.Position + B * tB, Orb_Radius );
    rect BoundC = RectCR( Drifter.Position + C * tC, Orb_Radius );
    
    DrawRect( Pass, TextureID_Dome, BoundC, ColorC * Alpha );
    DrawRect( Pass, TextureID_Dome, BoundB, ColorB * Alpha );
    DrawRect( Pass, TextureID_Dome, BoundA, ColorA * Alpha );
    
    if( Drifter.Health > 0 ) {
        vec2  P     = Drifter.Position + ToDirection2D( Radians + PI * 1.3f ) * ( Drifter.Radius * 0.7f );
        vec4  Color = COLOR_BLACK;
        if( Drifter.Blink_Timer >= 0.0f ) {
            Color = Vec4( 0.8f, 0.0f, 0.0f, 1.0f );
        }
        DrawPoint( Pass, P, Vec2( 0.15f, 0.15f ), Color * 0.75f );
    }
}

internal void
DrawDrifter( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    DRIFTER_STATE * DrifterS = Draw->DrifterS;
    
    for( int32 iDrifter = 0; iDrifter < DrifterS->nDrifter; iDrifter++ ) {
        DRIFTER Drifter = DrifterS->Drifter[ iDrifter ];
        
        DrawDrifter( Pass, Drifter );
    }
}

internal void
FinalizeDrifter( APP_STATE * AppState ) {
    SCORE_STATE   * ScoreS   = &AppState->ScoreS;
    DRIFTER_STATE * DrifterS = &AppState->DrifterS;
    
    uint32 NewCount = 0;
    for( int32 iDrifter = 0; iDrifter < DrifterS->nDrifter; iDrifter++ ) {
        DRIFTER Drifter = DrifterS->Drifter[ iDrifter ];
        
        if( Drifter.Health <= 0 ) {
            AddKillToScore( AppState, EntityType_Drifter, Drifter.Position );
            
            POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Drifter );
            int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
            
            for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                AddScoreMultiplier( AppState, NewScoreMultiplier( Drifter.Position, AppState->World_Bound ) );
            }
            
            AddEntityDeathParticles( AppState, Drifter.Position, DRIFTER_BASE_COLOR );
            AddEntityDeathParticles( AppState, Drifter.Position, DRIFTER_BASE_COLOR );
            
            AddKillToWave( AppState, Drifter.WaveGuid );
            
            PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Small );
        } else {
            DrifterS->Drifter[ NewCount++ ] = Drifter;
        }
    }
    DrifterS->nDrifter = NewCount;
}