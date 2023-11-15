
internal flo32
Looper_GetChangeTargetTime() {
    flo32 Result = RandomF32InRange( 1.0f, 2.0f );
    return Result;
}

internal void
Looper_SetRandomDirAndSpeed( LOOPER * Looper, rect World_Bound ) {
    flo32 t = RandomF32();
    Looper->Speed = lerp( 1.0f, t * t, 2.5f );
    
    rect  Bound   = AddRadius( World_Bound, -LOOPER_RADIUS * 0.5f );
    flo32 Radians = WrapRadians( Looper->Radians + RandomF32InRange( -PI / 3.0f, PI / 3.0f ) );
    vec2  Dir     = ToDirection2D( Radians );
    vec2  Dest    = Looper->Position + Dir * ( Looper->Speed * Looper->Change_TargetTime );
    if( ( Dest.x < Bound.Left ) || ( Dest.x > Bound.Right ) ) {
        Dir.x = -Dir.x;
    }
    if( ( Dest.y < Bound.Bottom ) || ( Dest.y > Bound.Top ) ) {
        Dir.y = -Dir.y;
    }
    Looper->Radians = ToRadians( Dir );
}

internal LOOPER
LooperC( int32 WaveGuid, vec2 Position ) {
    LOOPER Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position + Vec2( LOOPER_LOOP_RADIUS, 0.0f );
    Result.Health   = 1;
    Result.Change_TargetTime =  RandomF32InRange( 0.15f, 2.0f );
    Result.Loop_Timer        = -RandomF32InRange( 0.0f, 1.0f );
    Result.Loop_Radius       = LOOPER_LOOP_RADIUS;
    
    return Result;
}

internal void
AddLooper( APP_STATE * AppState, LOOPER Looper ) {
    LOOPER_STATE * LooperS = &AppState->LooperS;
    if( LooperS->nLooper < LOOPER_MAX_COUNT ) {
        LooperS->Looper[ LooperS->nLooper++ ] = Looper;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add LOOPER to LOOPER_STATE: %u", LOOPER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateLooper( APP_STATE * AppState, flo32 dT ) {
    LOOPER_STATE * LooperS = &AppState->LooperS;
    PLAYER_STATE  * Player = &AppState->Player;
    
    for( int32 iLooper = 0; iLooper < LooperS->nLooper; iLooper++ ) {
        LOOPER * Looper = LooperS->Looper + iLooper;
        
        Looper->Blink_Timer += dT;
        if( Looper->Blink_Timer >= Looper->Blink_TargetTime ) {
            Looper->Blink_TargetTime =  ( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
            Looper->Blink_Timer      = -( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
        }
        
        Looper->Loop_Timer += dT;
        if( Looper->Loop_Timer >= LOOPER_LOOP_CYCLE_TARGET_TIME ) {
            Looper->Loop_Timer -= LOOPER_LOOP_CYCLE_TARGET_TIME;
        }
        
        Looper->Change_Timer += dT;
        if( Looper->Change_Timer >= Looper->Change_TargetTime ) {
            Looper->Change_Timer      = 0.0f;
            Looper->Change_TargetTime = Looper_GetChangeTargetTime();
            
            Looper_SetRandomDirAndSpeed( Looper, AppState->World_Bound );
        }
        
        for( int32 iRepel = iLooper + 1; iRepel < LooperS->nLooper; iRepel++ ) {
            LOOPER * Repel = LooperS->Looper + iRepel;
            
            vec2 V = Repel->Position - Looper->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = LOOPER_REPEL_RADIUS + LOOPER_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * LOOPER_REPEL_SPEED;
                
                Looper->Repel -= Repel_Vector;
                Repel->Repel   += Repel_Vector;
            }
        }
        
        vec2 Dir   = ToDirection2D( Looper->Radians );
        vec2 Accel = ( Dir * Looper->Speed - Looper->Velocity + Looper->Repel ) * LOOPER_FRICTION;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Looper->Velocity * dT;
        // TODO: Collision!
        Looper->Position += dPos;
        Looper->Velocity += Accel * dT;
        
        Looper->Repel = {};
    }
}

internal void
DrawLooper( RENDER_PASS * Pass, LOOPER Looper, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 bRadians = ( 1.0f - Clamp01( Looper.Loop_Timer / LOOPER_LOOP_CYCLE_TARGET_TIME ) ) * TAU - PI;
    vec2  bPos     = Looper.Position + ToDirection2D( bRadians ) * Looper.Loop_Radius;
    
    flo32 Radians    = ToRadians( Looper.Velocity );
    flo32 Len        = LOOPER_RADIUS * 0.3f * Scale;
    vec2 A = ToDirection2D( Radians )                    * Len;
    vec2 B = ToDirection2D( Radians + PI * 2.0f / 3.0f ) * Len;
    vec2 C = ToDirection2D( Radians + PI * 4.0f / 3.0f ) * Len;
    
    vec2 VA = ToDirection2D( bRadians )      * 0.1f;
    vec2 VB = ToDirection2D( bRadians + PI ) * 0.1f;
    
    vec4 ColorA = LOOPER_BASE_COLOR;
    vec4 ColorB = TowardWhite( ColorA, 0.1f );
    vec4 ColorC = TowardWhite( ColorA, 0.2f );
    vec4 ColorD = COLOR_YELLOW * 0.5f;
    
    vec2 Cube_Dim  = Vec2( 1.2f, 1.2f ) * Scale;
    vec2 Light_Dim = Vec2( 1.5f, 1.5f ) * Scale;
    DrawRect( Pass, TextureID_Looper, RectCD( bPos + C, Cube_Dim        ), ColorC * Alpha );
    DrawRect( Pass, TextureID_Glow,   RectCD( bPos + B + VA, Light_Dim  ), ColorD * Alpha );
    DrawRect( Pass, TextureID_Looper, RectCD( bPos + B, Cube_Dim * 0.9f ), ColorB * Alpha );
    DrawRect( Pass, TextureID_Glow,   RectCD( bPos + A + VB, Light_Dim  ), ColorD * Alpha );
    DrawRect( Pass, TextureID_Looper, RectCD( bPos + A, Cube_Dim        ), ColorA * Alpha );
    
    if( Looper.Health > 0 ) {
        vec2  P     = bPos + A + Vec2( 0.35f, -0.4f );
        vec4  Color = COLOR_BLACK;
        if( Looper.Blink_Timer >= 0.0f ) {
            Color = Vec4( 0.8f, 0.0f, 0.0f, 1.0f );
        }
        DrawPoint( Pass, P, Vec2( 0.15f, 0.15f ), Color * 0.75f );
    }
}

internal void
DrawLooper( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    LOOPER_STATE * LooperS = Draw->LooperS;
    
    for( int32 iLooper = 0; iLooper < LooperS->nLooper; iLooper++ ) {
        LOOPER Looper = LooperS->Looper[ iLooper ];
        
        DrawLooper( Pass, Looper );
    }
}

internal void
FinalizeLooper( APP_STATE * AppState ) {
    SCORE_STATE  * ScoreS  = &AppState->ScoreS;
    LOOPER_STATE * LooperS = &AppState->LooperS;
    
    uint32 NewCount = 0;
    for( int32 iLooper = 0; iLooper < LooperS->nLooper; iLooper++ ) {
        LOOPER Looper = LooperS->Looper[ iLooper ];
        
        flo32 bRadians = ( 1.0f - Clamp01( Looper.Loop_Timer / LOOPER_LOOP_CYCLE_TARGET_TIME ) ) * TAU - PI;
        vec2  bPos     = Looper.Position + ToDirection2D( bRadians ) * Looper.Loop_Radius;
        
        if( Looper.Health <= 0 ) {
            AddKillToScore( AppState, EntityType_Looper, bPos );
            
            POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Looper );
            int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
            
            for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                AddScoreMultiplier( AppState, NewScoreMultiplier( bPos, AppState->World_Bound, true ) );
            }
            
            AddEntityDeathParticles( AppState, bPos, LOOPER_BASE_COLOR );
            AddEntityDeathParticles( AppState, bPos, LOOPER_BASE_COLOR );
            
            AddKillToWave( AppState, Looper.WaveGuid );
            
            PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Small );
        } else {
            LooperS->Looper[ NewCount++ ] = Looper;
        }
    }
    LooperS->nLooper = NewCount;
}