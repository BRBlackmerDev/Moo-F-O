
internal SMOG
SmogC( vec2 Position ) {
    SMOG Result = {};
    Result.Position = Position;
    Result.Radius   = SMOG_RADIUS;
    Result.Health   = 1;
    
    flo32  tColor   = RandomF32InRange( 0.05f, 0.10f );
    Result.Color    = COLOR_GRAY( tColor );
    
    return Result;
}

internal void
AddSmog( APP_STATE * AppState, SMOG Smog ) {
    SMOG_STATE * SmogS = &AppState->SmogS;
    if( SmogS->nSmog < SMOG_MAX_COUNT ) {
        SmogS->Smog[ SmogS->nSmog++ ] = Smog;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SMOG to SMOG_STATE: %u", SMOG_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateSmog( MEMORY * TempMemory, APP_STATE * AppState, flo32 dT ) {
    SMOG_STATE   * SmogS  = &AppState->SmogS;
    PLAYER_STATE * Player = &AppState->Player;
    
    uint64 Clock_Start = __rdtsc();
    
    flo32  Cell_Dim = SMOG_REPEL_RADIUS * 2.0f;
    int32 nCellX    = ( int32 )( GetWidth ( AppState->World_Bound ) / Cell_Dim ) * 2;
    int32 nCellY    = ( int32 )( GetHeight( AppState->World_Bound ) / Cell_Dim ) * 2;
    int32 nCell     = nCellX * nCellY;
    vec2  Grid_bPos     = Vec2( ( flo32 )nCellX, ( flo32 )nCellY ) * ( -Cell_Dim * 0.5f );
    rect  Grid_Bound    = RectMM( Grid_bPos, -Grid_bPos );
    rect  Grid_MinBound = AddRadius( Grid_Bound, -Cell_Dim );
    
    INT32_PAIR * Smog_iCell_iSmog        = _PushArray_Clear( TempMemory, INT32_PAIR, SmogS->nSmog );
    INT32_PAIR * Smog_bIndex_nSmogInCell = _PushArray_Clear( TempMemory, INT32_PAIR, nCell );
    
#define ENABLE_SMOG_GRID  ( 1 )
#if ENABLE_SMOG_GRID
    for( int32 iSmog = 0; iSmog < SmogS->nSmog; iSmog++ ) {
        SMOG * Smog = SmogS->Smog + iSmog;
        Assert( IsInRect( Smog->Position, Grid_MinBound ) );
        
        vec2 V = Smog->Position - Grid_bPos;
        int32 iCellX = ( int32 )( V.x / Cell_Dim );
        int32 iCellY = ( int32 )( V.y / Cell_Dim );
        int32 iCell  = iCellY * nCellX + iCellX;
        
        Smog_iCell_iSmog       [ iSmog ]    = { iCell, iSmog };
        Smog_bIndex_nSmogInCell[ iCell ].n += 1;
    }
    
    { // Sort
        int32       nValue = SmogS->nSmog;
        INT32_PAIR * Value = Smog_iCell_iSmog;
        
        boo32 DoSort = true;
        while( DoSort ) {
            DoSort = false;
            for( int32 iValue = 0; iValue < ( nValue - 1 ); iValue++ ) {
                if( Value[ iValue ].m > Value[ iValue + 1 ].m ) {
                    _swap( INT32_PAIR, Value[ iValue ], Value[ iValue + 1 ] );
                    DoSort = true;
                }
            }
        }
        
        int32 Sum = 0;
        for( int32 iCell = 0; iCell < nCell; iCell++ ) {
            Smog_bIndex_nSmogInCell[ iCell ].m = Sum;
            Sum += Smog_bIndex_nSmogInCell[ iCell ].n;
        }
    }
    
    uint32 BreakHere = 10;
    
    for( int32 iCell = 0; iCell < nCell; iCell++ ) {
        INT32_PAIR bSmogA = Smog_bIndex_nSmogInCell[ iCell ];
        if( bSmogA.n > 0 ) {
            
            INT32_PAIR Adj[ 4 ] = {};
            
            // NOTE : I'm not checking that the adjacent cells exist. I'm assuming that there's an empty cell apron surrounding the grid.
            // TODO : Resize the grid based on max Smog bound, then add apron.
            
            Adj[ 0 ] = Smog_bIndex_nSmogInCell[ iCell + nCellX - 1 ];
            Adj[ 1 ] = Smog_bIndex_nSmogInCell[ iCell + nCellX     ];
            Adj[ 2 ] = Smog_bIndex_nSmogInCell[ iCell + nCellX + 1 ];
            Adj[ 3 ] = Smog_bIndex_nSmogInCell[ iCell          + 1 ];
            
#if 0            
            Adj[ 0 ] = Smog_bIndex_nSmogInCell[ iCell          - 1 ];
            Adj[ 1 ] = Smog_bIndex_nSmogInCell[ iCell + nCellX - 1 ];
            Adj[ 2 ] = Smog_bIndex_nSmogInCell[ iCell + nCellX     ];
            Adj[ 3 ] = Smog_bIndex_nSmogInCell[ iCell + nCellX + 1 ];
            Adj[ 4 ] = Smog_bIndex_nSmogInCell[ iCell          + 1 ];
            Adj[ 5 ] = Smog_bIndex_nSmogInCell[ iCell - nCellX + 1 ];
            Adj[ 6 ] = Smog_bIndex_nSmogInCell[ iCell - nCellX     ];
            Adj[ 7 ] = Smog_bIndex_nSmogInCell[ iCell - nCellX - 1 ];
#endif
            
            { // Current Cell
                for( int32 iSmog = 0; iSmog < bSmogA.n; iSmog++ ) {
                    SMOG * Smog = SmogS->Smog + Smog_iCell_iSmog[ bSmogA.m + iSmog ].n;
                    
                    for( int32 iRepel = iSmog + 1; iRepel < bSmogA.n; iRepel++ ) {
                        SMOG * Repel = SmogS->Smog + Smog_iCell_iSmog[ bSmogA.m + iRepel ].n;
                        
                        vec2 V = Repel->Position - Smog->Position;
                        
                        flo32 DistSq   = GetLengthSq( V );
                        flo32 Radius   = SMOG_REPEL_RADIUS + SMOG_REPEL_RADIUS;
                        flo32 RadiusSq = Radius * Radius;
                        if( DistSq <= RadiusSq ) {
                            flo32 tSpeed = 1.0f - Clamp01( DistSq / RadiusSq );
                            flo32 Speed  = ( tSpeed * SMOG_REPEL_SPEED );
                            Speed *= Speed;
                            
                            vec2 Repel_Vector = GetNormal( V ) * Speed;
                            
                            Smog->Repel  -= Repel_Vector;
                            Repel->Repel += Repel_Vector;
                        }
                    }
                }
            }
            
            { // Adjacent Cells
                for( int32 iAdj = 0; iAdj < 4; iAdj++ ) {
                    INT32_PAIR bSmogB = Adj[ iAdj ];
                    if( bSmogB.n > 0 ) {
                        for( int32 iSmog = 0; iSmog < bSmogA.n; iSmog++ ) {
                            SMOG * Smog = SmogS->Smog + Smog_iCell_iSmog[ bSmogA.m + iSmog ].n;
                            
                            for( int32 iRepel = 0; iRepel < bSmogB.n; iRepel++ ) {
                                SMOG * Repel = SmogS->Smog + Smog_iCell_iSmog[ bSmogB.m + iRepel ].n;
                                
                                vec2 V = Repel->Position - Smog->Position;
                                
                                flo32 DistSq   = GetLengthSq( V );
                                flo32 Radius   = SMOG_REPEL_RADIUS + SMOG_REPEL_RADIUS;
                                flo32 RadiusSq = Radius * Radius;
                                if( DistSq <= RadiusSq ) {
                                    flo32 tSpeed = 1.0f - Clamp01( DistSq / RadiusSq );
                                    flo32 Speed  = ( tSpeed * SMOG_REPEL_SPEED );
                                    Speed *= Speed;
                                    
                                    vec2 Repel_Vector = GetNormal( V ) * Speed;
                                    
                                    Smog->Repel  -= Repel_Vector;
                                    Repel->Repel += Repel_Vector;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#endif
    
    for( int32 iSmog = 0; iSmog < SmogS->nSmog; iSmog++ ) {
        SMOG * Smog = SmogS->Smog + iSmog;
        
        {
            Smog->ModColor = 0.0f;
            
            BULLET_STATE * BulletS = &AppState->BulletS;
            for( int32 iBullet = 0; iBullet < BulletS->nBullet; iBullet++ ) {
                BULLET * Bullet = BulletS->Bullet + iBullet;
                
                vec2 A = Bullet->PrevPos;
                vec2 B = Bullet->Position + Bullet->Direction * BULLET_HEIGHT;
                
                flo32 Dist   = DistPointToEdge( Smog->Position, A, B );
                flo32 tColor = 1.0f - Clamp01( Dist / 2.0f );
                Smog->ModColor = MaxValue( tColor * 0.5f, Smog->ModColor );
            }
        }
        
        {
            rect Bound = AppState->World_Bound;
            if( Smog->Position.x < Bound.Left ) {
                Smog->Repel.x += SMOG_REPEL_SPEED * 12.0f;
            }
            if( Smog->Position.x > Bound.Right ) {
                Smog->Repel.x -= SMOG_REPEL_SPEED * 12.0f;
            }
            if( Smog->Position.y < Bound.Bottom ) {
                Smog->Repel.y += SMOG_REPEL_SPEED * 12.0f;
            }
            if( Smog->Position.y > Bound.Top ) {
                Smog->Repel.y -= SMOG_REPEL_SPEED * 12.0f;
            }
        }
        
        {
            vec2 V = Player->Position - Smog->Position;
            
            vec2 A = Player->Position - Player->dPos;
            vec2 B = Player->Position;
            
            flo32 tPlayer = 1.0f + MaxValue( GetLength( B - A ), dT );
            tPlayer = MinValue( tPlayer, 2.0f );
            
            flo32 DistSq   = DistSqPointToEdge( Smog->Position, A, B );
            flo32 Radius   = SMOG_REPEL_RADIUS + SMOG_REPEL_RADIUS * tPlayer;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                flo32 tSpeed = 1.0f - Clamp01( DistSq / RadiusSq );
                flo32 Speed  = ( tSpeed * SMOG_REPEL_SPEED * tPlayer * 2.0f );
                Speed *= Speed;
                
                vec2 Repel_Vector = GetNormal( V ) * Speed;
                
                Smog->Repel  -= Repel_Vector;
            }
        }
        
#if 0
        for( int32 iRepel = iSmog + 1; iRepel < SmogS->nSmog; iRepel++ ) {
            SMOG * Repel = SmogS->Smog + iRepel;
            
            vec2 V = Repel->Position - Smog->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = SMOG_REPEL_RADIUS + SMOG_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                flo32 tSpeed = 1.0f - Clamp01( DistSq / RadiusSq );
                flo32 Speed  = ( tSpeed * SMOG_REPEL_SPEED );
                Speed *= Speed;
                
                vec2 Repel_Vector = GetNormal( V ) * Speed;
                
                Smog->Repel  -= Repel_Vector;
                Repel->Repel += Repel_Vector;
            }
        }
#endif
    }
    
    for( int32 iSmog = 0; iSmog < SmogS->nSmog; iSmog++ ) {
        SMOG * Smog = SmogS->Smog + iSmog;
        
        vec2 Accel = ( -Smog->Velocity + Smog->Repel ) * SMOG_FRICTION;
        
        if( Player->GravityWell_IsActive ) {
            vec2  Dir    = GetNormal( Player->Position - Smog->Position );
            flo32 Dist   = GetLength( Player->Position - Smog->Position );
            flo32 tSpeed = Clamp01( ( Dist - 2.0f ) / 18.0f );
            flo32 Speed  = lerp( 8.0f, tSpeed, 0.0f ) * 3.0f;
            
            Accel += GetPerp( Dir ) * ( Player->Rotate_Speed * SMOG_FRICTION * Speed );
        }
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Smog->Velocity * dT;
        
        Smog->Position += dPos;
        Smog->Velocity += Accel * dT;
        
        Smog->Repel = {};
    }
    
    _PopArray( TempMemory, INT32_PAIR, nCell );
    _PopArray( TempMemory, INT32_PAIR, SmogS->nSmog );
    
    uint64 Clock_End = __rdtsc();
    uint64 Clock_Total = Clock_End - Clock_Start;
    //DISPLAY_VALUE( uint64, Clock_Total );
}

internal void
DrawSmog( RENDER_PASS * Pass, SMOG Smog, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4  ColorA  = Smog.Color;
    ColorA.r += Smog.ModColor;
    ColorA   *= Alpha;
    flo32 RadiusA = Smog.Radius * 1.25f * Scale;
    rect  BoundA  = RectCR( Smog.Position, RadiusA );
    
    //DrawRect( Pass, TextureID_Smog, BoundA, ColorA );
}

internal void
DrawSmog( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    SMOG_STATE * SmogS = Draw->SmogS;
    
    for( int32 iSmog = 0; iSmog < SmogS->nSmog; iSmog++ ) {
        SMOG Smog = SmogS->Smog[ iSmog ];
        
        DrawSmog( Pass, Smog );
    }
}

internal void
FinalizeSmog( APP_STATE * AppState ) {
    SMOG_STATE * SmogS = &AppState->SmogS;
    
    uint32 NewCount = 0;
    for( int32 iSmog = 0; iSmog < SmogS->nSmog; iSmog++ ) {
        SMOG Smog = SmogS->Smog[ iSmog ];
        
        if( Smog.Health > 0 ) {
            SmogS->Smog[ NewCount++ ] = Smog;
        } else {
            AddEntityDeathParticles( AppState, Smog.Position, SMOG_BASE_COLOR );
        }
    }
    SmogS->nSmog = NewCount;
}