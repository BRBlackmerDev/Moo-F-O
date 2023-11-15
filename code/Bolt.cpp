
internal BOLT
BoltC( vec2 Position, vec2 Direction ) {
    BOLT Result = {};
    Result.Position  = Position;
    Result.Velocity  = Direction * BOLT_SPEED;
    Result.Radius    = BOLT_RADIUS;
    Result.Health    = 1;
    
    return Result;
}

internal void
AddBolt( APP_STATE * AppState, BOLT Bolt ) {
    BOLT_STATE * BoltS = &AppState->BoltS;
    if( BoltS->nBolt < BOLT_MAX_COUNT ) {
        BoltS->Bolt[ BoltS->nBolt++ ] = Bolt;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BOLT to BOLT_STATE: %u", BOLT_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateBolt( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE       * Player       = &AppState->Player;
    BOLT_STATE * BoltS = &AppState->BoltS;
    
    for( int32 iBolt = 0; iBolt < BoltS->nBolt; iBolt++ ) {
        BOLT * Bolt = BoltS->Bolt + iBolt;
        
        vec2 dPos = Bolt->Velocity * dT;
        Bolt->Position += dPos;
    }
    
    //DISPLAY_VALUE( uint32, BoltS->nBolt );
}

internal void
DrawBolt( RENDER_PASS * Pass, BOLT Bolt, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4 Color = BOLT_BASE_COLOR;
    
    DrawCircle( Pass, Bolt.Position, Bolt.Radius * Scale, Color * Alpha );
}

internal void
DrawBolt( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BOLT_STATE * BoltS = Draw->BoltS;
    
    for( int32 iBolt = 0; iBolt < BoltS->nBolt; iBolt++ ) {
        BOLT Bolt = BoltS->Bolt[ iBolt ];
        
        DrawBolt( Pass, Bolt );
    }
}

internal void
FinalizeBolt( APP_STATE * AppState ) {
    BOLT_STATE * BoltS = &AppState->BoltS;
    
    rect Remove_Bound = AddRadius( AppState->World_Bound, 2.0f );
    
    uint32 NewCount = 0;
    for( int32 iBolt = 0; iBolt < BoltS->nBolt; iBolt++ ) {
        BOLT Bolt = BoltS->Bolt[ iBolt ];
        
        if( ( Bolt.Health <= 0 ) && ( !Bolt.DoPointDrop ) ) {
            AddEntityDeathParticles( AppState, Bolt.Position, ToColor( 165, 75, 25 ) );
        }
        
        if( ( Bolt.Health <= 0 ) && ( Bolt.DoPointDrop ) ) {
            AddKillToScore( AppState, EntityType_Bolt, Bolt.Position );
            
            POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Bolt );
            int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
            
            vec2 Pos = ClampToRect( Bolt.Position, AppState->World_Bound );
            for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
            }
            
            AddEntityDeathParticles( AppState, Bolt.Position, COLOR_WHITE );
            AddEntityDeathParticles( AppState, Bolt.Position, ToColor( 255, 165, 115 ) );
        }
        
        if( ( IsInRect( Bolt.Position, Remove_Bound ) ) && ( Bolt.Health > 0 ) ) {
            BoltS->Bolt[ NewCount++ ] = Bolt;
        }
    }
    BoltS->nBolt = NewCount;
}