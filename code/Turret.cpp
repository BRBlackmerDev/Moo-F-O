
internal MODEL_DATA
GenModel_Turret_Shield_EndCap( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[] = {
        0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
    };
    uint32 nVertex = _ArrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
    };
    uint32 nIndex = _ArrayCount( index );
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal TURRET
TurretC( int32 WaveGuid, vec2 Position, flo32 Radius, uint32 Health, ENTITY_SHIELD Shield = {} ) {
    TURRET Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Radius   = MaxValue( Radius, 1.25f );
    Result.Health   = Health;
    Result.Shield   = Shield;
    Result.Fire_TargetTime = RandomF32InRange( 3.0f, 4.0f );
    
    if( ( Result.Shield.HasShield ) && ( Result.Shield.Radians == TAU ) ) {
        Result.Base_Radians = RandomF32() * TAU;
    }
    
    return Result;
}

internal void
AddTurret( APP_STATE * AppState, TURRET Turret ) {
    TURRET_STATE * TurretS = &AppState->TurretS;
    if( TurretS->nTurret < TURRET_MAX_COUNT ) {
        TurretS->Turret[ TurretS->nTurret++ ] = Turret;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add TURRET to TURRET_STATE: %u", TURRET_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
AddMegaTurret( APP_STATE * AppState, TURRET MegaTurret ) {
    TURRET_STATE * TurretS = &AppState->TurretS;
    if( TurretS->nMegaTurret < MEGA_TURRET_MAX_COUNT ) {
        TurretS->MegaTurret[ TurretS->nMegaTurret++ ] = MegaTurret;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add MEGA_TURRET to TURRET_STATE: %u", TURRET_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal vec2
FindPosForTurretSpawn( APP_STATE * AppState ) {
    TURRET_STATE       * TurretS      = &AppState->TurretS;
    ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
    
    int32 nRow = 4;
    int32 nCol = 6;
    
    flo32 xSpacing = TurretS->Spawn_xSpacing;
    flo32 ySpacing = 12.0f;
    
    flo32 Width  = xSpacing * ( int32 )( nCol - 1 );
    flo32 Height = ySpacing * ( int32 )( nRow - 1 );
    vec2  bPos   = Vec2( Width, Height ) * -0.5f;
    
    int32 nPos = nRow * nCol;
    vec2  Pos  = Vec2( FLT_MAX, FLT_MAX );
    
    boo32 FoundPos = false;
    for( int32 Iter = 0; ( !FoundPos ) && ( Iter < 10 ); Iter++ ) {
        FoundPos = true;
        
        int32 Select = RandomS32InRange( 0, nPos - 1 );
        int32 iRow   = Select / nCol;
        int32 iCol   = Select % nCol;
        
        Pos = bPos + Vec2( ( flo32 )iCol, ( flo32 )iRow ) * Vec2( xSpacing, ySpacing );
        
        for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
            TURRET Turret = TurretS->Turret[ iTurret ];
            
            flo32 Radius   = ySpacing + 1.0f;
            flo32 RadiusSq = Radius * Radius;
            flo32 DistSq   = GetLengthSq( Turret.Position - Pos );
            if( DistSq <= RadiusSq ) {
                FoundPos = false;
            }
        }
        
        for( int32 iMegaTurret = 0; iMegaTurret < TurretS->nMegaTurret; iMegaTurret++ ) {
            TURRET MegaTurret = TurretS->MegaTurret[ iMegaTurret ];
            
            flo32 Radius   = ySpacing + 1.0f;
            flo32 RadiusSq = Radius * Radius;
            flo32 DistSq   = GetLengthSq( MegaTurret.Position - Pos );
            if( DistSq <= RadiusSq ) {
                FoundPos = false;
            }
        }
        
        for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
            ENTITY EntitySpawn = EntitySpawnS->EntitySpawn[ iEntitySpawn ];
            
            if( ( EntitySpawn.Type == EntityType_Turret ) || ( EntitySpawn.Type == EntityType_MegaTurret ) ) {
                flo32 Radius   = ySpacing + 1.0f;
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( EntitySpawn.Position - Pos );
                if( DistSq <= RadiusSq ) {
                    FoundPos = false;
                }
            }
        }
    }
    
    return Pos;
}

internal void
UpdateTurret( APP_STATE * AppState, flo32 dT ) {
    TURRET_STATE * TurretS = &AppState->TurretS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    {
        if( TurretS->SpawnMegaTurret_IsActive ) {
            TurretS->SpawnMegaTurret_Timer += dT;
            if( TurretS->SpawnMegaTurret_Timer >= MEGA_TURRET_SPAWN_TARGET_TIME ) {
                TurretS->SpawnMegaTurret_Timer = 0.0f;
                
                vec2 Pos = FindPosForTurretSpawn( AppState );
                if( IsInRect( Pos, AppState->World_Bound ) ) {
                    
                    ENTITY_SHIELD Shield = {};
                    
                    int32 Select_Radians  = RandomS32InRange( 2, 2 );
                    int32 Select_Speed    = RandomS32InRange( 0, 1 );
                    int32 Select_bRadians = RandomS32InRange( 0, 3 );
                    
                    if( Select_Radians > 0 ) {
                        flo32 bRadians = ( flo32 )Select_bRadians * ( PI * 0.5f ) + ( PI * 0.25f );
                        flo32  Radians = ( flo32 )Select_Radians  * ( PI * 0.5f );
                        
                        Shield.HasShield   = true;
                        Shield.bRadians    = bRadians;
                        Shield.Radians     = Radians;
                        Shield.Dist        = 1.0f;
                        Shield.Width       = 2.0f;
                        Shield.RotateSpeed = ( Select_Speed == 0 ) ? -PLAYER_ROTATE_SPEED_SLOW : -PLAYER_ROTATE_SPEED_FAST;
                    }
                    
                    flo32 Radius = MEGA_TURRET_RADIUS;
                    int32 Health = TurretS->SpawnMegaTurret_Health;
                    
                    AddEntitySpawn( AppState, GenValIdentity( -1, EntityType_MegaTurret, Pos, Health, Radius, Shield ) );
                } else {
                    TurretS->SpawnMegaTurret_Timer = MEGA_TURRET_SPAWN_TARGET_TIME - 1.0f;
                }
            }
        }
        
        if( TurretS->Spawn_IsActive ) {
            ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
            
            int32 nSpawn = TurretS->nTurret;
            for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
                ENTITY Entity = EntitySpawnS->EntitySpawn[ iEntitySpawn ];
                if( Entity.Type == EntityType_Turret ) {
                    nSpawn += 1;
                }
            }
            
            if( nSpawn < TurretS->Spawn_MaxTurret ) {
                TurretS->Spawn_Cooldown_Timer = MaxValue( TurretS->Spawn_Cooldown_Timer - dT, 0.0f );
                
                if( TurretS->Spawn_Cooldown_Timer <= 0.0f ) {
                    TurretS->Spawn_Cooldown_Timer = TurretS->Spawn_Cooldown_TargetTime;
                    
                    vec2 Pos = FindPosForTurretSpawn( AppState );
                    if( IsInRect( Pos, AppState->World_Bound ) ) {
                        int32 nNumer = TurretSettings_Speed_Count * TurretSettings_Radians_Count;
                        int32 Select = RandomSelectByOdds( ( int32 * )( &TurretS->Numer_SpeedRadians ), nNumer );
                        Assert( Select > -1 );
                        
                        ENTITY_SHIELD Shield = {};
                        
                        int32 Select_Radians  = Select % TurretSettings_Radians_Count;
                        int32 Select_Speed    = Select / TurretSettings_Radians_Count;
                        int32 Select_bRadians = RandomS32InRange( 0, 3 );
                        int32 Select_Radius   = RandomS32InRange( 0, 4 );
                        
                        if( Select_Radians > 0 ) {
                            flo32 bRadians = ( flo32 )Select_bRadians * ( PI * 0.5f ) + ( PI * 0.25f );
                            flo32  Radians = ( flo32 )Select_Radians  * ( PI * 0.5f );
                            
                            Shield.HasShield   = true;
                            Shield.bRadians    = bRadians;
                            Shield.Radians     = Radians;
                            Shield.Dist        = 1.0f;
                            Shield.Width       = 1.0f;
                            Shield.RotateSpeed = ( Select_Speed == 0 ) ? -PLAYER_ROTATE_SPEED_SLOW : -PLAYER_ROTATE_SPEED_FAST;
                        }
                        
                        flo32 Radius = 1.0f;
                        if( Select_Radius >= 3 ) {
                            Radius = 2.0f;
                        }
                        int32 Health = ( int32 )Radius * 10;
                        
                        AddEntitySpawn( AppState, GenValIdentity( -1, EntityType_Turret, Pos, Health, Radius, Shield ) );
                    } else {
                        TurretS->Spawn_Cooldown_Timer = 1.0f;
                    }
                }
            }
        }
    }
    
    { // Update Shield_SegmentT
        for( int32 iMegaTurret = 0; iMegaTurret < TurretS->nMegaTurret; iMegaTurret++ ) {
            TURRET * MegaTurret = TurretS->MegaTurret + iMegaTurret;
            
            flo32 Temp[ TURRET_SHIELD_SEGMENT_MAX_COUNT ] = {};
            memcpy( Temp, MegaTurret->Shield_SegmentT, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
            
            for( int32 iSegment = 0; iSegment < TURRET_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
                int32 iSegmentA =   iSegment;
                int32 iSegmentB = ( iSegment + 1 ) % TURRET_SHIELD_SEGMENT_MAX_COUNT;
                
                flo32 SegmentTA = MegaTurret->Shield_SegmentT[ iSegmentA ];
                flo32 SegmentTB = MegaTurret->Shield_SegmentT[ iSegmentB ];
                
                flo32 tA = MaxValue( ( SegmentTB - SegmentTA ) * 0.65f, 0.0f );
                flo32 tB = MaxValue( ( SegmentTA - SegmentTB ) * 0.65f, 0.0f );
                
                Temp[ iSegmentA ] = MinValue( Temp[ iSegmentA ] + tA, TURRET_SHIELD_MAX_T );
                Temp[ iSegmentB ] = MinValue( Temp[ iSegmentB ] + tB, TURRET_SHIELD_MAX_T );
            }
            
            memcpy( MegaTurret->Shield_SegmentT, Temp, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
        }
        
        for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
            TURRET * Turret = TurretS->Turret + iTurret;
            
            flo32 Temp[ TURRET_SHIELD_SEGMENT_MAX_COUNT ] = {};
            memcpy( Temp, Turret->Shield_SegmentT, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
            
            for( int32 iSegment = 0; iSegment < TURRET_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
                int32 iSegmentA =   iSegment;
                int32 iSegmentB = ( iSegment + 1 ) % TURRET_SHIELD_SEGMENT_MAX_COUNT;
                
                flo32 SegmentTA = Turret->Shield_SegmentT[ iSegmentA ];
                flo32 SegmentTB = Turret->Shield_SegmentT[ iSegmentB ];
                
                flo32 tA = MaxValue( ( SegmentTB - SegmentTA ) * 0.65f, 0.0f );
                flo32 tB = MaxValue( ( SegmentTA - SegmentTB ) * 0.65f, 0.0f );
                
                Temp[ iSegmentA ] = MinValue( Temp[ iSegmentA ] + tA, TURRET_SHIELD_MAX_T );
                Temp[ iSegmentB ] = MinValue( Temp[ iSegmentB ] + tB, TURRET_SHIELD_MAX_T );
            }
            
            memcpy( Turret->Shield_SegmentT, Temp, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
        }
    }
    
    for( int32 iMegaTurret = 0; iMegaTurret < TurretS->nMegaTurret; iMegaTurret++ ) {
        TURRET * MegaTurret = TurretS->MegaTurret + iMegaTurret;
        
        MegaTurret->Damage_Timer = MaxValue( MegaTurret->Damage_Timer - dT, 0.0f );
        
        MegaTurret->Exit_Timer += dT;
        
        { // Movement
            MegaTurret->Move_Timer += dT;
            if( MegaTurret->Move_Timer >= MegaTurret->Move_TargetTime ) {
                MegaTurret->Move_Timer      = 0.0f;
                MegaTurret->Move_TargetTime = RandomF32InRange( 1.5f, 3.0f );
                
                flo32 t = RandomF32();
                MegaTurret->Move_Speed = lerp( 2.0f, t * t, 4.0f );
                
                rect  Bound   = AddRadius( AppState->World_Bound, -8.0f );
                flo32 Radians = WrapRadians( MegaTurret->Move_Radians + RandomF32InRange( -PI / 3.0f, PI / 3.0f ) );
                vec2  Dir     = ToDirection2D( Radians );
                vec2  Dest    = MegaTurret->Position + Dir * ( MegaTurret->Move_Speed * MegaTurret->Move_TargetTime );
                if( ( Dest.x < Bound.Left ) || ( Dest.x > Bound.Right ) ) {
                    Dir.x = -Dir.x;
                }
                if( ( Dest.y < Bound.Bottom ) || ( Dest.y > Bound.Top ) ) {
                    Dir.y = -Dir.y;
                }
                
                Dest = MegaTurret->Position + Dir * ( MegaTurret->Move_Speed * MegaTurret->Move_TargetTime );
                if( !IsInRect( Dest, Bound ) ) {
                    Dest = ClampToRect( Dest, Bound );
                    Dir  = GetNormal( Dest - MegaTurret->Position );
                }
                
                MegaTurret->Move_Radians = ToRadians( Dir );
            }
            
            vec2  Dir   = GetNormal( Player->Position - MegaTurret->Position );
            flo32 Speed = MEGA_TURRET_SPEED;
            
            Dir   = ToDirection2D( MegaTurret->Move_Radians );
            Speed = MegaTurret->Move_Speed;
            
            vec2 Accel = ( Dir * Speed - MegaTurret->Velocity ) * MEGA_TURRET_FRICTION;
            
            vec2 dPos  = Accel * ( 0.5f * dT * dT ) + MegaTurret->Velocity * dT;
            // TODO: Collision!
            MegaTurret->Position += dPos;
            MegaTurret->Velocity += Accel * dT;
        }
        
        ENTITY_SHIELD * Shield = &MegaTurret->Shield;
        Shield->bRadians = WrapRadians( Shield->bRadians + Shield->RotateSpeed * dT );
        
        vec2 DirectionToPlayer = GetNormal( Player->Position - MegaTurret->Position );
        MegaTurret->Fire_Radians = ToRadians( DirectionToPlayer );
        
        flo32 t = dT * 4.0f;
        
        for( int32 iSegment = 0; iSegment < TURRET_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
            MegaTurret->Shield_SegmentT[ iSegment ] = MaxValue( MegaTurret->Shield_SegmentT[ iSegment ] - t, 0.0f );
        }
        
        if( MegaTurret->Exit_Timer < MEGA_TURRET_EXIT_TARGET_TIME ) {
            MegaTurret->Fire_Timer += dT;
            if( MegaTurret->Fire_Timer >= MEGA_TURRET_FIRE_TARGET_TIME ) {
                MegaTurret->Fire_Timer = 0.0f;
                
                for( int32 iEnemyBullet = 0; iEnemyBullet < 3; iEnemyBullet++ ) {
                    flo32 Radians = MegaTurret->Fire_Radians + ( TAU / 3.0f ) * ( flo32 )iEnemyBullet;
                    AddEnemyBullet( AppState, EnemyBulletC( MegaTurret->Position, ToDirection2D( Radians ) ) );
                }
            }
        }
    }
    
    for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
        TURRET * Turret = TurretS->Turret + iTurret;
        
        Turret->Damage_Timer = MaxValue( Turret->Damage_Timer - dT, 0.0f );
        
        ENTITY_SHIELD * Shield = &Turret->Shield;
        Shield->bRadians = WrapRadians( Shield->bRadians + Shield->RotateSpeed * dT );
        
        vec2 DirectionToPlayer = GetNormal( Player->Position - Turret->Position );
        Turret->Fire_Radians = ToRadians( DirectionToPlayer );
        
        {
            flo32 t = dT * 4.0f;
            
            for( int32 iSegment = 0; iSegment < TURRET_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
                Turret->Shield_SegmentT[ iSegment ] = MaxValue( Turret->Shield_SegmentT[ iSegment ] - t, 0.0f );
            }
        }
        
        Turret->Fire_Timer += dT;
        if( Turret->Fire_Timer >= TURRET_FIRE_TARGET_TIME ) {
            Turret->Fire_Timer = 0.0f;
            
            AddEnemyBullet( AppState, EnemyBulletC( Turret->Position, DirectionToPlayer ) );
        }
    }
    
    //DISPLAY_VALUE( uint32, TurretS->nTurret );
}

internal void
Editor_DrawTurret( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, TURRET Turret ) {
    {
        DrawCircle( Pass, Turret.Position, Turret.Radius, COLOR_BLUE );
        
        vec4 ColorA = Vec4( 0.0f, 0.0f, 0.7f, 1.0f );
        vec4 ColorB = Vec4( 0.0f, 0.0f, 0.6f, 1.0f );
        
        orect BoundA = ORectBCD( Turret.Position, Vec2( 1.25f, 0.5f ), Turret.Fire_Radians );
        orect BoundB = ORectTCD( Turret.Position, Vec2( 1.40f, 0.5f ), Turret.Fire_Radians );
        
        DrawORect( Pass, BoundA, ColorA );
        DrawORect( Pass, BoundB, ColorB );
    }
    
    ENTITY_SHIELD Shield = Turret.Shield;
    if( Shield.HasShield ) {
        flo32 Radius_Inner   = Turret.Radius + Shield.Dist;
        flo32 Radius_Outer   = Radius_Inner + Shield.Width;
        
        vec4 ColorA = Vec4( 0.0f, 0.35f, 0.35f, 1.0f );
        vec4 ColorB = Vec4( 0.0f, 0.35f, 0.35f, 1.0f );
        
        if( Shield.RotateSpeed < 0.0f ) {
            ColorA = Vec4( 0.0f, 0.1f, 0.1f, 1.0f );
            ColorB = Vec4( 0.5f, 1.0f, 1.0f, 1.0f );
        }
        if( Shield.RotateSpeed > 0.0f ) {
            ColorA = Vec4( 0.5f, 1.0f, 1.0f, 1.0f );
            ColorB = Vec4( 0.0f, 0.1f, 0.1f, 1.0f );
        }
        if( fabsf( Shield.RotateSpeed ) == PLAYER_ROTATE_SPEED_FAST ) {
            ColorA.r = 0.0f;
            ColorA.b = 0.0f;
            ColorB.r = 0.0f;
            ColorB.b = 0.0f;
        }
        if( Shield.RotateSpeed == TURRET_SHIELD_TEMP_SPEED_TO_INDICATE_TARGET_SHIELD ) {
            ColorA = Vec4( 0.5f, 0.0f, 0.0f, 1.0f );
            ColorB = Vec4( 0.5f, 0.0f, 0.0f, 1.0f );
        }
        
        int32 nSegment = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * Clamp01( Shield.Radians / TAU ) );
        
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            flo32 Segment_Radians = Shield.bRadians + ( ( flo32 )iSegment / ( flo32 )nSegment ) * Shield.Radians;
            vec2  Segment_Offset  = ToDirection2D( Segment_Radians ) * ( ( Radius_Outer + Radius_Inner ) * 0.5f );
            vec2  Segment_Scale   = {};
            if( ( iSegment % 2 ) == 0 ) {
                Segment_Scale.x = ( TAU / ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Outer * 1.01f;
            } else {
                Segment_Scale.x = ( TAU / ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Inner * 1.01f;
            }
            Segment_Scale.y = ( Radius_Outer - Radius_Inner ) * 0.5f;
            
            flo32 t     = Clamp01( ( flo32 )iSegment / ( flo32 )nSegment );
            vec4  Color = lerp( ColorA, t, ColorB );
            
            if( ( iSegment % 2 ) == 0 ) {
                DrawTriangle( Pass, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color );
            } else {
                DrawTriangle( Pass, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians - PI * 0.5f, Color );
            }
        }
    }
    
    if( Turret.Health > 0 ) {
        Assert( Font );
        Assert( Camera );
        
        char Str[ 4 ] = {};
        sprintf( Str, "%u", Turret.Health );
        DrawString( Pass, Font, Str, Turret.Position, TextAlign_Center, WORLD_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE );
    }
}

internal void
DrawTurretShield( RENDER_PASS * Pass, TURRET Turret, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    ENTITY_SHIELD Shield = Turret.Shield;
    if( Shield.HasShield ) {
        flo32 Radius_Inner   = ( Turret.Radius + Shield.Dist                       ) * Scale;
        flo32 Radius_Outer   = ( Turret.Radius + Shield.Dist + Shield.Width * 1.5f ) * Scale;
        
        vec4 ColorA = TowardBlack( TURRET_BASE_COLOR, 0.25f );
        vec4 ColorB = TowardBlack( TURRET_BASE_COLOR, 0.0f );
        vec4 ColorC = TowardWhite( TURRET_BASE_COLOR, 0.0f );
        vec4 ColorD = TowardWhite( TURRET_BASE_COLOR, 0.25f );
        
        if( fabs( Shield.RotateSpeed ) == PLAYER_ROTATE_SPEED_FAST ) {
            ColorA = TowardWhite( MEGA_TURRET_BASE_COLOR, 0.0f );
            ColorB = TowardWhite( MEGA_TURRET_BASE_COLOR, 0.25f  );
            ColorC = TowardWhite( MEGA_TURRET_BASE_COLOR, 0.125f );
            ColorD = TowardWhite( MEGA_TURRET_BASE_COLOR, 0.5f  );
        }
        
        int32 nSegment = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * Clamp01( Shield.Radians / TAU ) );
        int32 nDrawSegment = nSegment;
        if( Shield.Radians < TAU ) {
            nDrawSegment += 1;
        }
        
        for( int32 iSegment = 0; iSegment < nDrawSegment; iSegment++ ) {
            flo32 Segment_Radians = Shield.bRadians + ( ( flo32 )iSegment / ( flo32 )nSegment ) * Shield.Radians;
            vec2  Segment_Offset  = ToDirection2D( Segment_Radians ) * ( ( Radius_Outer + Radius_Inner ) * 0.5f );
            vec2  Segment_Scale   = {};
            if( ( iSegment % 2 ) == 0 ) {
                Segment_Scale.x = ( TAU / ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Outer * 1.01f;
            } else {
                Segment_Scale.x = ( TAU / ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT ) * Radius_Inner * 1.01f;
            }
            Segment_Scale.y = ( Radius_Outer - Radius_Inner ) * 0.5f;
            
            flo32 t = Turret.Shield_SegmentT[ iSegment ];
            vec4 Color_Glow = {};
            vec4 Color_Ring = {};
            if( t > 1.0f ) {
                flo32 tA = Clamp01( ( t - 1.0f ) / ( TURRET_SHIELD_MAX_T - 1.0f ) );
                
                Color_Glow = lerp( ColorB, tA, COLOR_WHITE );
                Color_Ring = lerp( ColorD, tA, COLOR_WHITE );
            } else {
                Color_Glow = lerp( ColorA, Turret.Shield_SegmentT[ iSegment ], ColorB );
                Color_Ring = lerp( ColorC, Turret.Shield_SegmentT[ iSegment ], ColorD );
            }
            
            flo32 tOffset = lerp( 1.0f, t / TURRET_SHIELD_MAX_T, 0.985f );
            flo32 tScaleY = lerp( 1.0f, t / TURRET_SHIELD_MAX_T, 0.9f );
            
            Segment_Offset  *= tOffset;
            Segment_Scale.y *= tScaleY;
            
            if( iSegment == 0 ) {
                MODEL_ID ModelID = MeshID_Triangle;
                if( Shield.Radians < TAU ) {
                    ModelID = MeshID_Turret_Shield_EndCap;
                }
                DrawModel( Pass, ModelID, TextureID_Turret_Shield_Glow, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Glow * Alpha );
                DrawModel( Pass, ModelID, TextureID_Turret_Shield_Ring, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Ring * Alpha );
            } else if( iSegment == nSegment ) {
                Segment_Scale.x = -Segment_Scale.x;
                DrawModel( Pass, MeshID_Turret_Shield_EndCap, TextureID_Turret_Shield_Glow, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Glow * Alpha );
                DrawModel( Pass, MeshID_Turret_Shield_EndCap, TextureID_Turret_Shield_Ring, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Ring * Alpha );
            } else if( ( iSegment % 2 ) == 0 ) {
                DrawTriangle      ( Pass, TextureID_Turret_Shield_Glow, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Glow * Alpha );
                DrawTriangle      ( Pass, TextureID_Turret_Shield_Ring, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians + PI * 0.5f, Color_Ring * Alpha );
            } else {
                DrawTriangleFlipUV( Pass, TextureID_Turret_Shield_Glow, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians - PI * 0.5f, Color_Glow * Alpha );
                DrawTriangleFlipUV( Pass, TextureID_Turret_Shield_Ring, Turret.Position + Segment_Offset, Segment_Scale, Segment_Radians - PI * 0.5f, Color_Ring * Alpha );
            }
            
            //DrawCircleOutline( Pass, Turret.Position, Radius_Inner + Shield.Width, COLOR_RED );
            //DrawCircleOutline( Pass, Turret.Position, Radius_Inner, COLOR_RED );
            
            //DrawCircleSegmentOutline( Pass, Turret.Position, Radius_Inner, Shield.bRadians, Shield.Radians, false, COLOR_RED );
            //DrawCircleSegmentOutline( Pass, Turret.Position, Radius_Outer, Shield.bRadians, Shield.Radians, false, COLOR_RED );
        }
    }
}

internal void
DrawTurret( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, TURRET Turret, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    {
        vec4 Color  = TURRET_BASE_COLOR;
        vec4 ColorA = TowardBlack( Color, 0.75f  );
        vec4 ColorB = TowardBlack( Color, 0.25f );
        vec4 ColorC = TowardBlack( Color, 0.35f  );
        vec4 ColorD = ColorB;
        vec4 ColorE = COLOR_BLACK;
        vec4 ColorF = ColorC;
        vec4 ColorG = TowardBlack( ColorB, 0.35f );
        vec4 ColorH = ColorC;
        
        flo32 Anim_TargetTime = ( 60.0f / 60.0f );
        flo32      TargetTime = TURRET_FIRE_TARGET_TIME - Anim_TargetTime;
        
        {
            flo32 tDamage = Clamp01( Turret.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
            ColorH = lerp( ColorH, tDamage, TowardWhite( ColorH, 0.75f ) );
        }
        
        if( Turret.Fire_Timer >= TargetTime ) {
            flo32 t  = Clamp01( ( Turret.Fire_Timer - TargetTime ) / ( Anim_TargetTime ) );
            flo32 tA = Clamp01( ( Turret.Fire_Timer - TargetTime ) / ( Anim_TargetTime - ( 20.0f / 60.0f ) ) );
            //ColorA = lerp( ColorA, t, Vec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
            ColorB = lerp( ColorB, t, Vec4( 0.1f, 0.1f, 0.8f, 1.0f ) );
            
            ColorA = lerp( ColorA, t,  TowardWhite( ColorA, 0.025f ) );
            ColorB = lerp( ColorB, tA, TowardWhite( ColorB, 0.5f ) );
            ColorE = lerp( ColorE, t,  TowardWhite( ENEMY_BULLET_BASE_COLOR, 0.35f ) );
            ColorF = lerp( ColorF, t,  TowardWhite( ColorF, 0.1f ) );
            ColorG = lerp( ColorG, t,  TowardWhite( ColorB, 0.5f ) );
        }
        
        vec2  Dir        = ToDirection2D( Turret.Fire_Radians );
        vec2  Pos_Base   = Turret.Position - Dir * ( Turret.Radius * 0.65f * Scale );
        vec2  Pos_Barrel = Turret.Position - Dir * ( Turret.Radius * 0.75f * Scale );
        flo32 Distort_Scale = 0.85f;
        
        if( ( Turret.Shield.HasShield ) && ( Turret.Shield.Radians == TAU ) ) {
            vec2  xAxis = ToDirection2D( Turret.Base_Radians );
            vec2  yAxis = GetPerp( xAxis );
            {
                flo32 dX = dot( Pos_Base - Turret.Position, xAxis );
                flo32 dY = dot( Pos_Base - Turret.Position, yAxis ) * Distort_Scale;
                Pos_Base = Turret.Position + xAxis * dX + yAxis * dY;
            }
            {
                flo32 dX = dot( Pos_Barrel - Turret.Position, xAxis );
                flo32 dY = dot( Pos_Barrel - Turret.Position, yAxis ) * Distort_Scale;
                Pos_Barrel = Turret.Position + xAxis * dX + yAxis * dY;
            }
        }
        
        { // Bound
            orect BoundA = ORectCR( Turret.Position, Turret.Radius        * Scale, Turret.Base_Radians );
            orect BoundB = ORectCR( Turret.Position, Turret.Radius * 0.8f * Scale, Turret.Base_Radians );
            orect BoundC = ORectCR( Turret.Position, Turret.Radius * 0.7f * Scale, Turret.Base_Radians );
            
            if( ( Turret.Shield.HasShield ) && ( Turret.Shield.Radians == TAU ) ) {
                BoundA.HalfDim.y *= Distort_Scale;
                BoundB.HalfDim.y *= Distort_Scale;
                BoundC.HalfDim.y *= Distort_Scale;
            }
            
            DrawORect( Pass, TextureID_RoundedDome, BoundA, ColorH                      * Alpha );
            DrawORect( Pass, TextureID_Dome,        BoundB, TowardBlack( ColorC, 0.8f ) * Alpha );
            DrawORect( Pass, TextureID_Dome,        BoundC, ColorF                      * Alpha );
        }
        
        //DrawCircle( Pass, Turret.Position, Turret.Radius * Scale, ColorC * Alpha );
        //DrawORect( Pass, BoundA, ColorA * Alpha );
        //DrawORect( Pass, BoundB, ColorB * Alpha );
        
        { // Base
            vec2 Pos = Pos_Base;
            vec2 Dim = Vec2( 1.3f, 1.0f ) * ( Turret.Radius * Scale );
            
            DrawORect( Pass, TextureID_Turret_Base, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Dim, Turret.Fire_Radians ), ColorA * Alpha );
            DrawEllipse( Pass, Pos, Vec2( 0.5f * Turret.Radius * Scale, Dim.y ) * 0.5f, Turret.Fire_Radians, ColorA * Alpha );
        }
        
        { // Barrel
            vec2 Pos = Pos_Barrel;
            vec2 Dim = Vec2( 1.1f, 0.8f ) * ( Turret.Radius * Scale );
            
            if( Turret.Radius <= 1.25f ) {
                Dim.y = 1.5f;
            }
            
            DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Dim, Turret.Fire_Radians ), ColorB * Alpha );
            DrawEllipse( Pass, Pos + Dir * ( Dim.x * 0.75f ), Vec2( Dim.x * 0.3f, Dim.y ) * 0.5f, Turret.Fire_Radians, TowardBlack( ColorB, 0.2f ) * Alpha );
            DrawORect( Pass, ORectTD( Pos + Dir * Dim.x, Vec2( 1.0f, 0.5f ), Dim * Vec2( 0.25f, 1.0f ), Turret.Fire_Radians ), TowardBlack( ColorB, 0.2f ) * Alpha );
            
            //DrawEllipse( Pass, Pos, Vec2( 0.6f * Turret.Radius * Scale, Dim.y * 1.2f ) * 0.5f, Turret.Fire_Radians, ColorB );
            DrawORect( Pass, TextureID_SatelliteDish, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Vec2( 0.3f * Turret.Radius * Scale, Dim.y * 1.2f ), Turret.Fire_Radians + PI ), ColorG * Alpha );
            
            //DrawEllipse( Pass, Pos + Dir * ( Dim.x * 0.8f ), Dim * Vec2( 0.3f, 0.9f ) * 0.5f, Turret.Fire_Radians, COLOR_BLACK );
            DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( Pos + Dir * ( Dim.x * 0.8f ), Dim * Vec2( 0.3f, 0.9f ), Turret.Fire_Radians + PI ), ColorE * Alpha );
            DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Dim * Vec2( 0.3f, 1.4f ), Turret.Fire_Radians ), TowardBlack( ColorD, 0.4f ) * Alpha );
        }
        
        //DrawCircle( Pass, PosB, 0.7f * Scale, ColorC * Alpha );
        
        if( Turret.Health > 0 ) {
            vec2 Pos = Pos_Base;
            DrawRect( Pass, TextureID_Dome, RectCR( Pos, 0.55f ), ColorA );
            
            DrawEntityHealth( Pass, AppState, Camera, Pos, Turret.Health, COLOR_WHITE );
        }
    }
    
    DrawTurretShield( Pass, Turret, Alpha, Scale );
}

internal void
DrawMegaTurret( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, TURRET MegaTurret, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    if( MegaTurret.Exit_Timer >= MEGA_TURRET_EXIT_TARGET_TIME ) {
        flo32 t = Clamp01( ( MegaTurret.Exit_Timer - MEGA_TURRET_EXIT_TARGET_TIME ) / MEGA_TURRET_EXIT_ANIM_TARGET_TIME );
        Alpha = 1.0f - t;
    }
    
    {
        vec4 Color  = MEGA_TURRET_BASE_COLOR;
        vec4 ColorA = TowardBlack( Color, 0.75f  );
        vec4 ColorB = TowardBlack( Color, 0.25f );
        vec4 ColorC = TowardBlack( Color, 0.35f  );
        vec4 ColorD = ColorB;
        vec4 ColorE = COLOR_BLACK;
        vec4 ColorF = ColorC;
        vec4 ColorG = TowardBlack( ColorB, 0.35f );
        vec4 ColorH = ColorC;
        
        flo32 Anim_TargetTime = ( 60.0f / 60.0f );
        flo32      TargetTime = MEGA_TURRET_FIRE_TARGET_TIME - Anim_TargetTime;
        
        {
            flo32 tDamage = Clamp01( MegaTurret.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
            ColorH = lerp( ColorH, tDamage, TowardWhite( ColorH, 0.75f ) );
        }
        
        DISPLAY_VALUE( flo32, MegaTurret.Fire_Timer );
        if( MegaTurret.Fire_Timer >= TargetTime ) {
            flo32 t  = Clamp01( ( MegaTurret.Fire_Timer - TargetTime ) / ( Anim_TargetTime ) );
            flo32 tA = Clamp01( ( MegaTurret.Fire_Timer - TargetTime ) / ( Anim_TargetTime - ( 20.0f / 60.0f ) ) );
            //ColorA = lerp( ColorA, t, Vec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
            ColorB = lerp( ColorB, t, Vec4( 0.1f, 0.1f, 0.8f, 1.0f ) );
            
            ColorA = lerp( ColorA, t,  TowardWhite( ColorA, 0.025f ) );
            ColorB = lerp( ColorB, tA, TowardWhite( ColorB, 0.5f ) );
            ColorE = lerp( ColorE, t,  TowardWhite( ENEMY_BULLET_BASE_COLOR, 0.35f ) );
            ColorF = lerp( ColorF, t,  TowardWhite( ColorF, 0.1f ) );
            ColorG = lerp( ColorG, t,  TowardWhite( ColorB, 0.5f ) );
        }
        
        { // Bound
            orect BoundA = ORectCR( MegaTurret.Position, MegaTurret.Radius        * Scale, MegaTurret.Base_Radians );
            orect BoundB = ORectCR( MegaTurret.Position, MegaTurret.Radius * 0.8f * Scale, MegaTurret.Base_Radians );
            orect BoundC = ORectCR( MegaTurret.Position, MegaTurret.Radius * 0.7f * Scale, MegaTurret.Base_Radians );
            
            DrawORect( Pass, TextureID_RoundedDome, BoundA, ColorH * Alpha );
            DrawORect( Pass, TextureID_Dome,        BoundB, TowardBlack( ColorC, 0.8f ) * Alpha );
            DrawORect( Pass, TextureID_Dome,        BoundC, ColorF * Alpha );
        }
        
        for( int32 iTurret = 0; iTurret < 3; iTurret++ ) {
            flo32 Radians    = MegaTurret.Fire_Radians + ( TAU / 3.0f ) * ( flo32 )iTurret;
            vec2  Dir        = ToDirection2D( Radians );
            vec2  Pos_Base   = MegaTurret.Position - Dir * ( MegaTurret.Radius * 0.65f * Scale );
            vec2  Pos_Barrel = MegaTurret.Position - Dir * ( MegaTurret.Radius * 0.75f * Scale );
            
            Pos_Base   = MegaTurret.Position + Dir * ( MegaTurret.Radius * 0.2f   * Scale );
            Pos_Barrel = MegaTurret.Position + Dir * ( MegaTurret.Radius * 0.15f  * Scale );
            
            { // Base
                vec2 Pos = Pos_Base;
                vec2 Dim = Vec2( 0.55f, 1.0f ) * ( MegaTurret.Radius * Scale );
                
                DrawORect( Pass, TextureID_Turret_Base, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Dim, Radians ), ColorA * Alpha );
                //DrawEllipse( Pass, Pos, Vec2( 0.5f * MegaTurret.Radius * Scale, Dim.y ) * 0.5f, Radians, ColorA );
            }
            
            { // Barrel
                vec2 Pos = Pos_Barrel;
                vec2 Dim = Vec2( 0.7f, 0.8f ) * ( MegaTurret.Radius * Scale );
                
                if( MegaTurret.Radius <= 1.25f ) {
                    Dim.y = 1.5f;
                }
                
                DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Dim * Vec2( 0.8f, 1.0f ), Radians ), ColorB * Alpha );
                DrawEllipse( Pass, Pos + Dir * ( Dim.x * 0.75f ), Vec2( Dim.x * 0.3f, Dim.y ) * 0.5f, Radians, TowardBlack( ColorB, 0.2f ) * Alpha );
                DrawEllipse( Pass, Pos + Dir * ( Dim.x * 0.85f ), Vec2( Dim.x * 0.3f, Dim.y ) * 0.5f, Radians, TowardBlack( ColorB, 0.2f ) * Alpha );
                DrawORect( Pass, ORectCD( Pos + Dir * ( Dim.x * 0.8f  ), Dim * Vec2( 0.1f, 1.0f ), Radians ), TowardBlack( ColorB, 0.2f ) * Alpha );
                
                //DrawEllipse( Pass, Pos, Vec2( 0.6f * MegaTurret.Radius * Scale, Dim.y * 1.2f ) * 0.5f, Radians, ColorB );
                //DrawORect( Pass, TextureID_SatelliteDish, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Vec2( 0.3f * MegaTurret.Radius * Scale, Dim.y * 1.2f ), Radians + PI ), ColorG );
                
                //DrawEllipse( Pass, Pos + Dir * ( Dim.x * 0.8f ), Dim * Vec2( 0.3f, 0.9f ) * 0.5f, Radians, COLOR_BLACK );
                DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( Pos + Dir * ( Dim.x * 0.8f ), Dim * Vec2( 0.3f, 0.9f ), Radians + PI ), ColorE * Alpha );
                //DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Pos, Vec2( 0.0f, 0.5f ), Dim * Vec2( 0.3f, 1.4f ), Radians ), TowardBlack( ColorD, 0.4f ) );
            }
        }
        
        for( int32 iTurret = 0; iTurret < 3; iTurret++ ) {
            flo32 Radians    = MegaTurret.Fire_Radians + ( TAU / 3.0f ) * ( flo32 )iTurret + ( TAU / 6.0f );
            vec2  Dim        = Vec2( 0.6f, 0.25f ) * ( MegaTurret.Radius * Scale );
            DrawORect( Pass, TextureID_Barrel, ORectTD( MegaTurret.Position, Vec2( 0.0f, 0.5f ), Dim, Radians ), ColorG * Alpha );
        }
        
        DrawORect( Pass, TextureID_Dome, ORectCR( MegaTurret.Position, MegaTurret.Radius * 0.4f * Scale ), ColorA * Alpha );
    }
    
    DrawTurretShield( Pass, MegaTurret, Alpha, Scale );
    
    if( MegaTurret.Health > 0 ) {
        DrawEntityHealth( Pass, AppState, Camera, MegaTurret.Position, MegaTurret.Health, COLOR_WHITE * Alpha );
    }
}

internal void
DrawTurret( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    TURRET_STATE * TurretS = Draw->TurretS;
    
    for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
        TURRET Turret = TurretS->Turret[ iTurret ];
        
        DrawTurret( Pass, AppState, Camera, Turret, 1.0f, 1.0f );
    }
    
    for( int32 iMegaTurret = 0; iMegaTurret < TurretS->nMegaTurret; iMegaTurret++ ) {
        TURRET MegaTurret = TurretS->MegaTurret[ iMegaTurret ];
        
        DrawMegaTurret( Pass, AppState, Camera, MegaTurret, 1.0f, 1.0f );
    }
}

internal void
FinalizeTurret( APP_STATE * AppState ) {
    SCORE_STATE  * ScoreS  = &AppState->ScoreS;
    TURRET_STATE * TurretS = &AppState->TurretS;
    
    { // MEGA TURRET
        uint32 NewCount = 0;
        for( int32 iMegaTurret = 0; iMegaTurret < TurretS->nMegaTurret; iMegaTurret++ ) {
            TURRET MegaTurret = TurretS->MegaTurret[ iMegaTurret ];
            
            if( MegaTurret.Health == 0 ) {
                AddKillToScore( AppState, EntityType_MegaTurret, MegaTurret.Position );
                
                POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_MegaTurret );
                int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                    AddScoreMultiplier( AppState, NewScoreMultiplier( MegaTurret.Position, AppState->World_Bound, true ) );
                }
                
                AddEntityDeathParticles( AppState, MegaTurret.Position, MEGA_TURRET_BASE_COLOR );
                AddEntityDeathParticles( AppState, MegaTurret.Position, MEGA_TURRET_BASE_COLOR );
                AddEntityDeathParticles( AppState, MegaTurret.Position, COLOR_WHITE );
                AddKillToWave( AppState, MegaTurret.WaveGuid );
                
                TurretS->BonusObjective_nMegaTurretKills += 1;
                
                PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Large );
            } else if( MegaTurret.Exit_Timer >= ( MEGA_TURRET_EXIT_TARGET_TIME + MEGA_TURRET_EXIT_ANIM_TARGET_TIME ) ) {
                TurretS->SpawnMegaTurret_Health = MegaTurret.Health;
            } else {
                TurretS->MegaTurret[ NewCount++ ] = MegaTurret;
            }
        }
        TurretS->nMegaTurret = NewCount;
    }
    
    { // TURRET
        uint32 NewCount = 0;
        for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
            TURRET Turret = TurretS->Turret[ iTurret ];
            
            if( Turret.Health > 0 ) {
                TurretS->Turret[ NewCount++ ] = Turret;
            } else {
                AddKillToScore( AppState, EntityType_Turret, Turret.Position );
                
                POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Turret );
                int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                    AddScoreMultiplier( AppState, NewScoreMultiplier( Turret.Position, AppState->World_Bound, true ) );
                }
                
                AddBigEntityDeathParticles( AppState, Turret.Position, Turret.Radius, TURRET_BASE_COLOR );
                AddKillToWave( AppState, Turret.WaveGuid );
                
                PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Medium );
            }
        }
        TurretS->nTurret = NewCount;
    }
}