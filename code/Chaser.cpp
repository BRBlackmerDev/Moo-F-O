
internal CHASER
ChaserC( int32 WaveGuid, vec2 Position ) {
    CHASER Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Radius   = CHASER_RADIUS;
    Result.Health   = 1;
    
    return Result;
}

internal void
AddChaser( APP_STATE * AppState, CHASER Chaser ) {
    CHASER_STATE * ChaserS = &AppState->ChaserS;
    if( ChaserS->nChaser < CHASER_MAX_COUNT ) {
        ChaserS->Chaser[ ChaserS->nChaser++ ] = Chaser;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add CHASER to CHASER_STATE: %u", CHASER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateChaser( APP_STATE * AppState, flo32 dT ) {
    CHASER_STATE * ChaserS = &AppState->ChaserS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    for( int32 iChaser = 0; iChaser < ChaserS->nChaser; iChaser++ ) {
        CHASER * Chaser = ChaserS->Chaser + iChaser;
        
        Chaser->Blink_Timer += dT;
        if( Chaser->Blink_Timer >= Chaser->Blink_TargetTime ) {
            Chaser->Blink_TargetTime =  ( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
            Chaser->Blink_Timer      = -( ( flo32 )RandomS32InRange( 6, 60 ) / 60.0f );
        }
        
        for( int32 iRepel = iChaser + 1; iRepel < ChaserS->nChaser; iRepel++ ) {
            CHASER * Repel = ChaserS->Chaser + iRepel;
            
            vec2 V = Repel->Position - Chaser->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = CHASER_REPEL_RADIUS + CHASER_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * CHASER_REPEL_SPEED;
                
                Chaser->Repel -= Repel_Vector;
                Repel->Repel  += Repel_Vector;
            }
        }
        
        vec2 Dir   = GetNormal( Player->Position - Chaser->Position );
        vec2 Accel = ( Dir * CHASER_SPEED - Chaser->Velocity + Chaser->Repel ) * CHASER_FRICTION;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Chaser->Velocity * dT;
        // TODO: Collision!
        Chaser->Position += dPos;
        Chaser->Velocity += Accel * dT;
        
        Chaser->Repel = {};
        
        Chaser->DirToPlayer = Dir;
    }
    
    //DISPLAY_VALUE( uint32, ChaserS->nChaser );
}

internal void
DrawChaser( RENDER_PASS * Pass, CHASER Chaser, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4 ColorA = CHASER_BASE_COLOR;
    vec4 ColorB = TowardBlack( ColorA, 0.3f );
    
    rect BoundA = RectCR( Chaser.Position, Chaser.Radius * Scale );
    DrawRect( Pass, TextureID_RoundedDome, BoundA, ColorB * Alpha );
    
    flo32 Radians = ToRadians( Chaser.DirToPlayer );
    vec2  RadiusA = Vec2( 0.55f, 0.35f ) * Chaser.Radius;
    vec2  Pos     = Chaser.Position + Chaser.DirToPlayer * ( ( Chaser.Radius - RadiusA.x + 0.025f ) * Scale );
    
    orect BoundB  = ORectCD( Pos, RadiusA * 2.0f * Scale, Radians );
    DrawORect( Pass, TextureID_RoundedDome, BoundB, ColorA * Alpha );
    
    if( Chaser.Health > 0 ) {
        vec2  P     = Chaser.Position + ToDirection2D( Radians + PI * 1.3f ) * ( Chaser.Radius * 0.7f );
        vec4  Color = COLOR_BLACK;
        if( Chaser.Blink_Timer >= 0.0f ) {
            Color = Vec4( 0.8f, 0.0f, 0.0f, 1.0f );
        }
        DrawPoint( Pass, P, Vec2( 0.15f, 0.15f ), Color * 0.75f );
    }
}

internal void
DrawChaser( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player  = Draw->Player;
    CHASER_STATE * ChaserS = Draw->ChaserS;
    
    for( int32 iChaser = 0; iChaser < ChaserS->nChaser; iChaser++ ) {
        CHASER Chaser = ChaserS->Chaser[ iChaser ];
        DrawChaser( Pass, Chaser );
    }
}

internal void
FinalizeChaser( APP_STATE * AppState ) {
    SCORE_STATE  * ScoreS  = &AppState->ScoreS;
    CHASER_STATE * ChaserS = &AppState->ChaserS;
    
    uint32 NewCount = 0;
    for( int32 iChaser = 0; iChaser < ChaserS->nChaser; iChaser++ ) {
        CHASER Chaser = ChaserS->Chaser[ iChaser ];
        
        if( Chaser.Health <= 0 ) {
            AddKillToScore( AppState, EntityType_Chaser, Chaser.Position );
            
            POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Chaser );
            int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
            
            for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                AddScoreMultiplier( AppState, NewScoreMultiplier( Chaser.Position, AppState->World_Bound ) );
            }
            
            AddEntityDeathParticles( AppState, Chaser.Position, CHASER_BASE_COLOR );
            AddEntityDeathParticles( AppState, Chaser.Position, CHASER_BASE_COLOR );
            
            AddKillToWave( AppState, Chaser.WaveGuid );
            
            PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Small );
        } else {
            ChaserS->Chaser[ NewCount++ ] = Chaser;
        }
    }
    ChaserS->nChaser = NewCount;
}