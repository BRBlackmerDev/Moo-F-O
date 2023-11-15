
internal ROGUE
RogueC( int32 WaveGuid, vec2 Position, flo32 Radians ) {
    ROGUE Result = {};
    Result.WaveGuid     = WaveGuid;
    Result.Position     = Position;
    Result.Radius       = ROGUE_RADIUS;
    Result.Health       = ROGUE_HEALTH;
    Result.Radians      = Radians;
    
    return Result;
}

internal void
AddRogue( APP_STATE * AppState, ROGUE Rogue ) {
    ROGUE_STATE * RogueS = &AppState->RogueS;
    if( RogueS->nRogue < ROGUE_MAX_COUNT ) {
        RogueS->Rogue[ RogueS->nRogue++ ] = Rogue;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add ROGUE to ROGUE_STATE: %u", ROGUE_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateRogue( APP_STATE * AppState, flo32 dT ) {
    ROGUE_STATE * RogueS = &AppState->RogueS;
    
    for( int32 iRogue = 0; iRogue < RogueS->nRogue; iRogue++ ) {
        ROGUE * Rogue = RogueS->Rogue + iRogue;
        
        if( Rogue->DoTeleport ) {
            Rogue->DoTeleport_Timer += dT;
            if( Rogue->DoTeleport_Timer >= ROGUE_TELEPORT_ANIM_TARGET_TIME ) {
                int32 nPoint = 8;
                vec2   Point[ 8 ] = {};
                Point[ 0 ] = Vec2( 0.5f, 1.0f );
                Point[ 1 ] = Vec2( 0.0f, 1.0f );
                Point[ 2 ] = Vec2( 0.0f, 0.5f );
                Point[ 3 ] = Vec2( 0.0f, 0.0f );
                Point[ 4 ] = Vec2( 0.5f, 0.0f );
                Point[ 5 ] = Vec2( 1.0f, 0.0f );
                Point[ 6 ] = Vec2( 1.0f, 0.5f );
                Point[ 7 ] = Vec2( 1.0f, 1.0f );
                
                rect Teleport_Bound = AddRadius( AppState->World_Bound, -6.0f );
                
                int32 nAvailable      = 0;
                int32  Available[ 8 ] = {};
                for( int32 iPoint = 0; iPoint < nPoint; iPoint++ ) {
                    vec2 P = GetT( Teleport_Bound, Point[ iPoint ] );
                    
                    boo32 IsValid = true;
                    
                    {
                        ESCORT_STATE * EscortS = &AppState->EscortS;
                        if( EscortS->nEscort > 0 ) {
                            ESCORT Escort = EscortS->Escort[ 0 ];
                            
                            vec2 Dim = ESCORT_DIM + Vec2( ROGUE_RADIUS + 0.5f, ROGUE_RADIUS + 0.5f );
                            if( IsInEllipse( P, Escort.Position, Dim, PI * 0.5f ) ) {
                                IsValid = false;
                            }
                        }
                    }
                    
                    {
                        flo32 Radius   = ROGUE_RADIUS + ROGUE_RADIUS;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( P - Rogue->Position );
                        if( DistSq <= RadiusSq ) {
                            IsValid = false;
                        }
                    }
                    
                    if( IsValid ) {
                        Available[ nAvailable++ ] = iPoint;
                    }
                }
                
                int32 Select = Available[ RandomS32InRange( 0, nAvailable - 1 ) ];
                
                Rogue->DoTeleport = false;
                Rogue->Position   = GetT( Teleport_Bound, Point[ Select ] );
                Rogue->Timer      = 0.0f;
                Rogue->Radians    = ( flo32 )Select * PI * 0.25f + PI * 0.25f;
            }
        } else {
            Rogue->DoTeleport_Timer = MaxValue( Rogue->DoTeleport_Timer - dT, 0.0f );
        }
        
        flo32 Teleport_TargetTime = 8.0f;
        
        Rogue->Timer += dT;
        if( Rogue->Timer >= ( 60.0f / 60.0f ) ) {
            Rogue->Radians = WrapRadians( Rogue->Radians - ROGUE_ROTATE_SPEED * dT );
        }
        if( Rogue->Timer >= Teleport_TargetTime ) {
            Rogue->DoTeleport     = true;
        }
        
        if( !Rogue->DoTeleport ) {
            if( ( Rogue->Timer >= 2.0f ) && ( Rogue->Timer < 5.25f ) ) {
                Rogue->Attack_Timer = MaxValue( Rogue->Attack_Timer - dT, 0.0f );
                if( Rogue->Attack_Timer <= 0.0f ) {
                    Rogue->Attack_Timer = ( 30.0f / 60.0f );
                    
                    flo32 RadiansA = Rogue->Radians + PI * 5.0f / 6.0f;
                    flo32 RadiansB = Rogue->Radians - PI * 5.0f / 6.0f;
                    
                    vec2 DirA = ToDirection2D( RadiansA );
                    vec2 DirB = ToDirection2D( RadiansB );
                    
                    vec2 PosA = Rogue->Position + DirA * ( ROGUE_RADIUS * 0.8f );
                    vec2 PosB = Rogue->Position + DirB * ( ROGUE_RADIUS * 0.8f );
                    
                    flo32 Speed = MISSILE_SPEED;
                    
                    boo32 DoTargetEscort = false;
                    if( AppState->EscortS.nEscort > 0 ) {
                        DoTargetEscort = true;
                    }
                    
                    AddMissile( AppState, MissileC( Rogue->WaveGuid, PosA, DirA * Speed, DoTargetEscort ) );
                    AddMissile( AppState, MissileC( Rogue->WaveGuid, PosB, DirB * Speed, DoTargetEscort ) );
                }
            }
        }
    }
}

internal void
DrawRogue( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, ROGUE Rogue, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    
    flo32 Anim_TargetTime = ROGUE_TELEPORT_ANIM_TARGET_TIME;
    flo32 AnimT = Clamp01( Rogue.DoTeleport_Timer / Anim_TargetTime );
    flo32 Alph = 1.0f - AnimT;
    flo32 Scal  = lerp( 1.0f, AnimT, 1.2f );
    flo32 ScalB = lerp( 1.0f, AnimT, 0.9f );
    
    {
        vec4 ColorA = COLOR_GRAY( 0.08f  );
        vec4 ColorB = COLOR_GRAY( 0.055f );
        vec4 ColorC = COLOR_GRAY( 0.03f  );
        
        DrawCircle( Pass, Rogue.Position, Rogue.Radius * ( ScalB * Scale ),        ColorA * ( Alph * Alpha ) );
        DrawCircle( Pass, Rogue.Position, Rogue.Radius * ( ScalB * Scale * 0.5f ), ColorC * ( Alph * Alpha ) );
        
        {
            vec2 Dir = ToDirection2D( Rogue.Radians );
            vec2 Pos = Rogue.Position + Dir * 0.2f;
            vec4 ColorD = COLOR_GRAY( 0.035f );
            
            flo32 Radius = ROGUE_RADIUS * 0.95f * Scal;
            
            vec2 DirA = ToDirection2D( Rogue.Radians + PI * 2.0f / 3.0f );
            vec2 DirB = ToDirection2D( Rogue.Radians - PI * 2.0f / 3.0f );
            
            vec2 PosA = Pos + DirA * ( 0.8f * AnimT );
            vec2 PosB = Pos - Dir  * ( 1.2f * AnimT );
            vec2 PosC = Pos + DirB * ( 0.8f * AnimT );
            
            DrawCircleSegment( Pass, PosA, Radius,        Rogue.Radians + PI * 0.5f, PI * 0.3f, false, ColorD * ( Alph * Alpha ) );
            DrawCircleSegment( Pass, PosB, Radius * 0.8f, Rogue.Radians + PI * 0.8f, PI * 0.4f, false, ColorD * ( Alph * Alpha ) );
            DrawCircleSegment( Pass, PosC, Radius,        Rogue.Radians - PI * 0.5f, PI * 0.3f, true, ColorD * ( Alph * Alpha ) );
            
            vec2 PosD = Pos + Dir * ( 0.8f * AnimT );
            vec2 Sca  = Vec2( Radius, 0.5f ) * Scal;
            DrawTriangleBC( Pass, PosD, Sca, Rogue.Radians - PI * 0.5f, ColorD * ( Alph * Alpha ) );
        }
        
        {
            
            flo32 Radians = Rogue.Radians;
            vec2  Dir     = ToDirection2D( Radians );
            
            vec2  bPos    = Rogue.Position + Dir * ( 0.8f * AnimT );
            
            vec2  DimA = Vec2( 1.2f,  0.6f   ) * ( ROGUE_RADIUS * Scal );
            vec2  DimB = Vec2( 0.325, 0.925f ) * ( ROGUE_RADIUS * Scal );
            
            orect BoundB = ORectCD ( bPos + Dir * 0.4f, DimA * Scale, Radians );
            orect BoundC = ORectBCD( bPos,              DimB * Scale, Radians );
            
            DrawORect( Pass, BoundC, ColorC * ( Alph * Alpha ) );
            DrawORect( Pass, BoundB, ColorB * ( Alph * Alpha ) );
            
            {
                vec2 Pos = bPos + Dir * ( ROGUE_RADIUS * 0.6f * Scale );
                vec2 Sca = Vec2( 0.15f, 0.25f ) * ( Scal * Scale );
                DrawTriangle( Pass, Pos, Sca, Radians - PI * 0.5f, COLOR_RED * ( Alph * Alpha ) );
            }
        }
        
        if( Rogue.Health > 0 ) {
            Assert( Font );
            Assert( Camera );
            
            char Str[ 4 ] = {};
            sprintf( Str, "%u", Rogue.Health );
            DrawString( Pass, Font, Str, Rogue.Position + Vec2( 0.8f * AnimT, 0.0f ), TextAlign_Center, WORLD_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE * Alph );
        }
    }
}

internal void
DrawRogue( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    ROGUE_STATE * RogueS = Draw->RogueS;
    
    for( int32 iRogue = 0; iRogue < RogueS->nRogue; iRogue++ ) {
        ROGUE Rogue = RogueS->Rogue[ iRogue ];
        
        DrawRogue( Pass, Font, Camera, Rogue );
    }
}

internal void
FinalizeRogue( APP_STATE * AppState ) {
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    ROGUE_STATE * RogueS = &AppState->RogueS;
    
    { // ROGUE
        uint32 NewCount = 0;
        for( int32 iRogue = 0; iRogue < RogueS->nRogue; iRogue++ ) {
            ROGUE Rogue = RogueS->Rogue[ iRogue ];
            
            boo32 DoKeep = ( Rogue.Health > 0 );
            
            if( DoKeep ) {
                RogueS->Rogue[ NewCount++ ] = Rogue;
            } else {
                AddEntityDeathParticles( AppState, Rogue.Position, COLOR_GRAY( 0.2f ) );
                AddEntityDeathParticles( AppState, Rogue.Position, COLOR_RED );
                AddEntityDeathParticles( AppState, Rogue.Position, COLOR_WHITE );
                AddKillToWave( AppState, Rogue.WaveGuid );
                
                RogueS->BonusObjective_nRogueKills += 1;
            }
        }
        RogueS->nRogue = NewCount;
    }
}