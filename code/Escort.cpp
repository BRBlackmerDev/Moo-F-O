
internal ESCORT
EscortC( int32 WaveGuid, vec2 Position ) {
    ESCORT Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Health   = ESCORT_HEALTH;
    
    return Result;
}

internal void
AddEscort( APP_STATE * AppState, ESCORT Escort ) {
    ESCORT_STATE * EscortS = &AppState->EscortS;
    if( EscortS->nEscort < ESCORT_MAX_COUNT ) {
        EscortS->Escort[ EscortS->nEscort++ ] = Escort;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add ESCORT to ESCORT_STATE: %u", ESCORT_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateEscort( APP_STATE * AppState, flo32 dT ) {
    ESCORT_STATE * EscortS = &AppState->EscortS;
    PLAYER_STATE  * Player   = &AppState->Player;
    
    for( int32 iEscort = 0; iEscort < EscortS->nEscort; iEscort++ ) {
        ESCORT * Escort = EscortS->Escort + iEscort;
        
        { // Missile Collision
            MISSILE_STATE * MissileS = &AppState->MissileS;
            for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
                MISSILE * Missile = MissileS->Missile + iMissile;
                
                if( IsInEllipse( Missile->Position, Escort->Position, ESCORT_DIM * 0.9f, PI * 0.5f ) ) {
                    Missile->Health = 0;
                }
            }
        }
        
        { // Update Engine Scale T
            flo32 dScale = ( 1.0f / ( 12.0f / 60.0f ) ) * dT;
            for( int32 iEngine = 0; iEngine < 3; iEngine++ ) {
                flo32 t = Escort->Engine_DestT[ iEngine ] - Escort->Engine_AtT[ iEngine ];
                if( fabsf( t ) > dScale ) {
                    dScale = Clamp( Escort->Engine_DestT[ iEngine ] - Escort->Engine_AtT[ iEngine ], -dScale, dScale );
                    Escort->Engine_AtT[ iEngine ] += dScale;
                } else {
                    Escort->Engine_AtT[ iEngine ]  = Escort->Engine_DestT[ iEngine ];
                }
                
                if( Escort->Engine_AtT[ iEngine ] == Escort->Engine_DestT[ iEngine ] ) {
                    Escort->Engine_DestT[ iEngine ] = RandomF32();
                }
            }
        }
        
        { // Update Indicator
            ESCORT_BONUS_POINTS_INDICATOR_TARGET_TIME;
            
            for( int32 iTimer = 0; iTimer < 2; iTimer++ ) {
                Escort->BonusPoints_IndicatorT[ iTimer ] += dT;
                
                int32 AtIndicator = ESCORT_BONUS_POINTS_MAX_INDICATOR;
                for( int32 iIndicator = 0; iIndicator < ESCORT_BONUS_POINTS_MAX_INDICATOR; iIndicator++ ) {
                    flo32 t = Escort->BonusPoints_IndicatorT[ iTimer ];
                    if( ( t >= EscortBonusPointsIndicator_TargetTime[ iIndicator ] ) && ( t < EscortBonusPointsIndicator_TargetTime[ iIndicator + 1 ] ) ) {
                        AtIndicator = iIndicator;
                    }
                }
                
                Escort->BonusPoints_AtIndicator[ iTimer ] = AtIndicator;
            }
        }
        
        { // Update Bonus Points T
            // slowest should be every 2.0  seconds
            // fastest should be every 0.25 seconds
            // do i want the times to be discrete or continuous?
            // probably discrete
            
            // 0.25
            // 0.35
            // 0.5
            // 0.75
            // 1.0
            // 1.5
            // 2.0
            
            // how much time should elapse between the highest and second highest rating?
            
            // 0.25
            // 0.25
            // 0.35
            // 0.5
            // 0.75
            // 1.15
            // 2.0
            // 5.25 total
            //  that seems kinda short
            
            // so timert should increase at a constant rate with different times between each rating
            // timer increases at a rate dependent on timert
            
            vec2 P[ 2 ] = {};
            P[ 0 ] = Escort->Position + Vec2( 1.5f,  3.0f );
            P[ 1 ] = Escort->Position + Vec2( 1.5f, -3.0f );
            
            flo32 dTimer[ ESCORT_BONUS_POINTS_MAX_INDICATOR + 1 ] = {};
            dTimer[ 0 ] = 2.0f;
            dTimer[ 1 ] = 2.0f;
            dTimer[ 2 ] = 1.5f;
            dTimer[ 3 ] = 1.0f;
            dTimer[ 4 ] = 0.75f;
            dTimer[ 5 ] = 0.5f;
            dTimer[ 6 ] = 0.35f;
            dTimer[ 7 ] = 0.25f;
            
            for( int32 iTimer = 0; iTimer < 2; iTimer++ ) {
                int32 AtIndicator = Escort->BonusPoints_AtIndicator[ iTimer ];
                flo32 dTimerA = ( 1.0f / dTimer[ AtIndicator ] ) * dT;
                
                Escort->BonusPoints_Timer[ iTimer ] += dTimerA;
                if( Escort->BonusPoints_Timer[ iTimer ] >= 1.0f ) {
                    Escort->BonusPoints_Timer[ iTimer ] -= 1.0f;
                    AddKillToScore( AppState, EntityType_BonusPoints, P[ iTimer ], -1, Vec2( -10.0f, 0.0f ), false );
                }
            }
        }
        
        //Player->Position = Vec2( -18.0f, 2.0f );
        //Player->DoCollision_Radians = PI * 0.2f;
        //vec2 Origin = Player->Position;
        //vec2 Vector = ToDirection2D( Player->DoCollision_Radians ) * 100.0f;
        //RAY2 Ray    = Ray2( Origin, Vector );
        
        //RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
        //Player->DoCollision      = DoesRayIntersectEllipse( &BestIntersect, Ray, Escort->Position, ESCORT_DIM, PI * 0.5f );
        //Player->DoCollision_Pos  = BestIntersect.P;
        //Player->DoCollision_Norm = BestIntersect.N;
        
        vec2 Accel = ( -Escort->Velocity ) * ESCORT_FRICTION;
        rect Bound = RectCD( Vec2( 0.0f, 0.0f ), ESCORT_DIM * 1.5f );
        if( !IsInRect( Escort->Position, Bound ) ) {
            vec2 Dir = GetNormal( GetCenter( Bound ) - Escort->Position );
            Accel += Dir * ESCORT_SPEED * ESCORT_FRICTION;
        }
        vec2 dPos  = Accel * ( 0.5f * dT * dT ) + Escort->Velocity * dT;
        // TODO: Collision!
        Escort->Position += dPos;
        Escort->Velocity += Accel * dT;
        
        //DISPLAY_VALUE( flo32, Escort->BonusPoints_Timer );
        //DISPLAY_VALUE( int32, Escort->BonusPoints_AtIndicator );
    }
}

internal void
DrawEscort( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, ESCORT Escort, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    
    {
        flo32 Offset = ESCORT_ENGINE_OFFSET * Scale;
        vec2  bPos   = Escort.Position + Vec2( -1.0f, 0.0f ) * Offset;
        
        vec2 PosA = bPos;
        vec4 ColorA = COLOR_RED;
        
        vec2 PosB = bPos + Vec2( 0.0f,  0.75f );
        vec2 PosC = bPos + Vec2( 0.0f, -0.75f );
        
        vec2 DimA = Vec2( 4.0f,  lerp( ESCORT_ENGINE_MIN_SCALE_Y, Escort.Engine_AtT[ 0 ], 5.0f ) ) * 0.5f;
        vec2 DimB = Vec2( 2.75f, lerp( 3.5f, Escort.Engine_AtT[ 1 ], 4.0f ) ) * 0.5f;
        vec2 DimC = Vec2( 2.75f, lerp( 3.5f, Escort.Engine_AtT[ 2 ], 4.0f ) ) * 0.5f;
        
        vec4 ColorB = Vec4( 0.9f, 0.0f, 0.0f, 1.0f );
        
        DrawTriangleBC( Pass, PosB, DimB, PI * 0.5f, ColorB * 0.85f );
        DrawTriangleBC( Pass, PosC, DimC, PI * 0.5f, ColorB * 0.85f );
        DrawTriangleBC( Pass, PosA, DimA, PI * 0.5f, ColorA * 0.85f );
    }
    
#if 0
    {
        vec2 yAxis = Vec2( -1.0f, 0.0f );
        vec2 xAxis = -GetPerp( yAxis );
        
        vec2 bPos  = Escort.Position + yAxis * ( ESCORT_ENGINE_OFFSET + ESCORT_ENGINE_MIN_SCALE_Y * 0.5f );
        vec2 Sca   = Vec2( 4.0f, ESCORT_ENGINE_MIN_SCALE_Y ) * 0.5f * 0.8f;
        vec2 A     = bPos + xAxis * -Sca.x * 0.6f + yAxis * -Sca.y * 0.4f;
        vec2 B     = bPos + yAxis *  Sca.y;
        vec2 C     = bPos + xAxis *  Sca.x * 0.6f + yAxis * -Sca.y * 0.4f;
        
        DrawLine( Pass, A, B, COLOR_BLUE );
        DrawLine( Pass, B, C, COLOR_BLUE );
        DrawLine( Pass, C, A, COLOR_BLUE );
    }
#endif
    
#if 0
    {
        vec2 yAxis = Vec2( -1.0f, 0.0f );
        vec2 xAxis = -GetPerp( yAxis );
        
        vec2 bPos  = Escort.Position + yAxis * ESCORT_ENGINE_OFFSET;
        vec2 Sca   = Vec2( 4.0f, ESCORT_ENGINE_MIN_SCALE_Y ) * 0.5f * 0.95f;
        vec2 A     = bPos + xAxis * -Sca.x;
        vec2 B     = bPos + yAxis *  Sca.y * 2.0f;
        vec2 C     = bPos + xAxis *  Sca.x;
        
        DrawLine( Pass, A, B, COLOR_BLUE );
        DrawLine( Pass, B, C, COLOR_BLUE );
        DrawLine( Pass, C, A, COLOR_BLUE );
    }
#endif
    
    DrawEllipse( Pass, Escort.Position, ESCORT_DIM, PI * 0.5f, ESCORT_BASE_COLOR );
    
    {
        vec4 Color = COLOR_GRAY( 0.05f );
        vec2 Dim   = ESCORT_DIM * Vec2( 0.6f, 0.15f );
        flo32 Offset = ESCORT_RADIUS_LONG * 0.7f * Scale;
        vec2  Pos    = Escort.Position + Vec2( 1.0f, 0.0f ) * Offset;
        DrawEllipse( Pass, Pos, Dim, PI * 0.5f, Color );
    }
    
#if 0
    {
        int32 AtIndicator = Escort.BonusPoints_AtIndicator[ 0 ];
        
        vec2  Dim     = Vec2( 0.25f, 0.25f );
        flo32 OffsetX = 0.4f;
        vec2  AtPos   = Escort.Position;
        
        for( int32 iIndicator = ESCORT_BONUS_POINTS_MAX_INDICATOR - 1; iIndicator >= 0; iIndicator-- ) {
            vec4 Color = COLOR_BLACK;
            if( iIndicator < AtIndicator ) {
                Color = Vec4( 0.5f, 0.5f, 0.0f, 1.0f );
            }
            
            DrawTriangleBC( Pass, AtPos, Dim, 0.0f, Color );
            DrawTriangleBC( Pass, AtPos, Dim, PI,   Color );
            
            AtPos.x -= ( Dim.x + OffsetX );
        }
    }
#endif
    
#if 0
    {
        flo32 OffsetX = 2.0f;
        vec2 A = Escort.Position + Vec2( -OffsetX,  3.0f );
        vec2 B = Escort.Position + Vec2(  OffsetX,  3.0f );
        vec2 C = Escort.Position + Vec2( -OffsetX, -3.0f );
        vec2 D = Escort.Position + Vec2(  OffsetX, -3.0f );
        
        flo32 tA = Clamp01( Escort.BonusPoints_IndicatorT[ 0 ] / 8.0f );
        flo32 tB = Clamp01( Escort.BonusPoints_IndicatorT[ 1 ] / 8.0f );
        
        B = lerp( A, tA, B );
        D = lerp( C, tB, D );
        
        DrawLine( Pass, A, B, COLOR_YELLOW );
        DrawLine( Pass, C, D, COLOR_YELLOW );
    }
#endif
    
    {
        // xOffset
        // yOffset
        // Radians
        // DimX
        // DimY
        flo32 x[ ESCORT_BONUS_POINTS_MAX_INDICATOR ][ 5 ] = {
            { -3.0f,  3.5f,   PI * 0.5f + PI * (  54.0f / 360.0f ), 1.5f, 0.125f },
            { -1.45f, 4.05f,  PI * 0.5f + PI * (  24.0f / 360.0f ), 1.45f, 0.130f },
            {  0.0f,  4.25f,  PI * 0.5f + PI * (   0.0f / 360.0f ), 1.15f, 0.140f },
            {  1.1f,  4.20f,  PI * 0.5f + PI * ( -12.0f / 360.0f ), 0.75f, 0.20f },
            {  1.8f,  4.10f,  PI * 0.5f + PI * ( -24.0f / 360.0f ), 0.4f,  0.35f },
            {  2.3f,  3.975f, PI * 0.5f + PI * ( -40.0f / 360.0f ), 0.3f,  0.55f },
            {  2.7f,  3.85f,  PI * 0.5f + PI * ( -48.0f / 360.0f ), 0.2f, 0.85f },
        };
        
        for( int32 iTimer = 0; iTimer < 2; iTimer++ ) {
            int32 AtIndicator = Escort.BonusPoints_AtIndicator[ iTimer ];
            
            for( int32 iIndicator = 0; iIndicator < ESCORT_BONUS_POINTS_MAX_INDICATOR; iIndicator++ ) {
                flo32 * xA = ( flo32 * )x + ( iIndicator * 5 );
                flo32 OffsetY = ( iTimer == 0 ) ? xA[ 1 ] : -xA[ 1 ];
                vec2  Pos     = Escort.Position + Vec2( xA[ 0 ], OffsetY );
                flo32 Radians = ( iTimer == 0 ) ? xA[ 2 ] : -xA[ 2 ];
                vec2  Dim     = Vec2( xA[ 3 ], xA[ 4 ] );
                
                vec4  Color = COLOR_BLACK;
                flo32 RG    = lerp( 0.2f, Clamp01( ( flo32 )iIndicator / ( flo32 )ESCORT_BONUS_POINTS_MAX_INDICATOR ), 0.35f );
                if( iIndicator < AtIndicator ) {
                    Color = Vec4( RG, RG, 0.0f, 1.0f );
                }
                
                orect Bound = ( iTimer == 0 ) ? ORectTCD( Pos, Dim, Radians ) : ORectBCD( Pos, Dim, Radians + PI );
                DrawORect( Pass, Bound, Color );
            }
        }
    }
    
#if 0
    {
        flo32 Radians = PI * 5.0f / 6.0f;
        
        vec2 DirA = ToDirection2D(  Radians ) * 10.0f;
        vec2 DirB = ToDirection2D( -Radians ) * 10.0f;
        
        DrawLine( Pass, Escort.Position, Escort.Position + DirA, COLOR_CYAN );
        DrawLine( Pass, Escort.Position, Escort.Position + DirB, COLOR_CYAN );
    }
#endif
}

internal void
DrawEscort( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    ESCORT_STATE * EscortS = Draw->EscortS;
    
    for( int32 iEscort = 0; iEscort < EscortS->nEscort; iEscort++ ) {
        ESCORT Escort = EscortS->Escort[ iEscort ];
        
        DrawEscort( Pass, Font, Camera, Escort );
    }
}

internal void
FinalizeEscort( APP_STATE * AppState ) {
    ESCORT_STATE * EscortS = &AppState->EscortS;
    
    uint32 NewCount = 0;
    for( int32 iEscort = 0; iEscort < EscortS->nEscort; iEscort++ ) {
        ESCORT Escort = EscortS->Escort[ iEscort ];
        
        if( Escort.Health > 0 ) {
            EscortS->Escort[ NewCount++ ] = Escort;
        } else {
            AddKillToWave( AppState, Escort.WaveGuid );
        }
    }
    EscortS->nEscort = NewCount;
}