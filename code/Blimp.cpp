
internal MODEL_DATA
GenModel_BlimpA( DIRECTX_12_RENDERER * Renderer, MEMORY * TempMemory ) {
    flo32 Radius   = 1.0f;
    int32 nLayer   = 5;
    int32 nSegment = BLIMP_SEGMENT_MAX_COUNT;
    
    int32 nVertex = nSegment * 3 * nLayer;
    int32 nIndex  = nVertex;
    
    VERTEX1_BUFFER * Buffer = &Renderer->ModelBuffer;
    
    VERTEX1 * Vertex = _PushArray_Clear( TempMemory, VERTEX1, nVertex );
    int32   * Index  = _PushArray_Clear( TempMemory, int32,   nIndex  );
    
    int32 AtVertex = 0;
    int32 AtIndex  = 0;
    
    for( int32 iLayer = 0; iLayer < nLayer; iLayer++ ) {
        flo32 DistA = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer     ) / ( flo32 )nLayer ), PI * 0.5f ) );
        flo32 DistB = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer + 1 ) / ( flo32 )nLayer ), PI * 0.5f ) );
        
        flo32 Adjust = 0.0f;
        if( ( iLayer % 2 ) == 1 ) {
            Adjust = ( TAU / ( flo32 )nSegment ) * 0.5f;
        }
        
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            flo32 tA = ( ( flo32 )iSegment            / ( flo32 )nSegment );
            flo32 tB = ( ( flo32 )( iSegment + 1 )    / ( flo32 )nSegment );
            flo32 tC = ( ( ( flo32 )iSegment + 0.5f ) / ( flo32 )nSegment );
            
            flo32 RadiansA = TAU * tA + Adjust;
            flo32 RadiansB = TAU * tB + Adjust;
            flo32 RadiansC = TAU * tC + Adjust;
            
            vec2 A = ToDirection2D( RadiansA ) * DistA;
            vec2 B = ToDirection2D( RadiansB ) * DistA;
            vec2 C = ToDirection2D( RadiansC ) * DistB;
            
            Vertex[ AtVertex++ ] = Vertex1( Vec3( A, 0.0f ), Vec2( 0.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( B, 0.0f ), Vec2( 1.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( C, 0.0f ), Vec2( 0.5f, 1.0f ) );
            
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
    }
    
    MODEL_DATA Result = UploadModel( Buffer, Vertex, nVertex, ( uint32 * )Index, nIndex );
    
    _PopArray( TempMemory, int32,   nIndex  );
    _PopArray( TempMemory, VERTEX1, nVertex );
    
    return Result;
}

internal MODEL_DATA
GenModel_BlimpB( DIRECTX_12_RENDERER * Renderer, MEMORY * TempMemory ) {
    flo32 Radius   = 1.0f;
    int32 nLayer   = 5;
    int32 nSegment = 24;
    
    int32 nVertex = nSegment * 3 * nLayer;
    int32 nIndex  = nVertex;
    
    VERTEX1_BUFFER * Buffer = &Renderer->ModelBuffer;
    
    VERTEX1 * Vertex = _PushArray_Clear( TempMemory, VERTEX1, nVertex );
    int32   * Index  = _PushArray_Clear( TempMemory, int32,   nIndex  );
    
    int32 AtVertex = 0;
    int32 AtIndex  = 0;
    
    for( int32 iLayer = 0; iLayer < ( nLayer - 1 ); iLayer++ ) {
        flo32 DistA = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer     ) / ( flo32 )nLayer ), PI * 0.5f ) );
        flo32 DistB = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer + 1 ) / ( flo32 )nLayer ), PI * 0.5f ) );
        
        flo32 Adjust = 0.0f;
        if( ( iLayer % 2 ) == 1 ) {
            Adjust = ( TAU / ( flo32 )nSegment ) * 0.5f;
        }
        
        for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
            flo32 tA = ( ( ( flo32 )iSegment + 0.5f ) / ( flo32 )nSegment );
            flo32 tB = ( ( ( flo32 )iSegment + 1.0f ) / ( flo32 )nSegment );
            flo32 tC = ( ( ( flo32 )iSegment + 1.5f ) / ( flo32 )nSegment );
            
            flo32 RadiansA = TAU * tA + Adjust;
            flo32 RadiansB = TAU * tB + Adjust;
            flo32 RadiansC = TAU * tC + Adjust;
            
            vec2 A = ToDirection2D( RadiansA ) * DistB;
            vec2 B = ToDirection2D( RadiansB ) * DistA;
            vec2 C = ToDirection2D( RadiansC ) * DistB;
            
            Vertex[ AtVertex++ ] = Vertex1( Vec3( A, 0.0f ), Vec2( 0.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( B, 0.0f ), Vec2( 1.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( C, 0.0f ), Vec2( 0.5f, 1.0f ) );
            
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
    }
    
    MODEL_DATA Result = UploadModel( Buffer, Vertex, nVertex, ( uint32 * )Index, nIndex );
    
    _PopArray( TempMemory, int32,   nIndex  );
    _PopArray( TempMemory, VERTEX1, nVertex );
    
    return Result;
}

internal MODEL_DATA
GenModel_BlimpC( DIRECTX_12_RENDERER * Renderer, MEMORY * TempMemory ) {
    flo32 Radius   = 1.0f;
    int32 nLayer   = 5;
    int32 nSegment = BLIMP_SEGMENT_MAX_COUNT;
    
    int32 nVertex = nSegment * 3 * nLayer;
    int32 nIndex  = nVertex;
    
    VERTEX1_BUFFER * Buffer = &Renderer->ModelBuffer;
    
    VERTEX1 * Vertex = _PushArray_Clear( TempMemory, VERTEX1, nVertex );
    int32   * Index  = _PushArray_Clear( TempMemory, int32,   nIndex  );
    
    int32 AtVertex = 0;
    int32 AtIndex  = 0;
    
    int32 iLayer = 2;
    
    flo32 DistA = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer     ) / ( flo32 )nLayer ), PI * 0.5f ) );
    flo32 DistB = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer + 1 ) / ( flo32 )nLayer ), PI * 0.5f ) );
    
    flo32 Adjust = 0.0f;
    if( ( iLayer % 2 ) == 1 ) {
        Adjust = ( TAU / ( flo32 )nSegment ) * 0.5f;
    }
    
    boo32 FillSegment[ BLIMP_SEGMENT_MAX_COUNT * 2 ] = {
        0, 1, 1, 1,  0, 1, 1, 0,  0, 1, 0, 0,  1, 0, 0, 1,  1, 1, 1, 0,  1, 1, 0, 0,
        1, 1, 0, 1,  0, 1, 1, 0,  1, 1, 1, 0,  0, 1, 1, 0,  0, 1, 1, 1,  0, 0, 1, 1,
    };
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
        flo32 tA = ( ( ( flo32 )iSegment + 0.0f ) / ( flo32 )nSegment );
        flo32 tB = ( ( ( flo32 )iSegment + 0.5f ) / ( flo32 )nSegment );
        flo32 tC = ( ( ( flo32 )iSegment + 1.0f ) / ( flo32 )nSegment );
        flo32 tD = ( ( ( flo32 )iSegment + 1.5f ) / ( flo32 )nSegment );
        
        flo32 RadiansA = TAU * tA + Adjust;
        flo32 RadiansB = TAU * tB + Adjust;
        flo32 RadiansC = TAU * tC + Adjust;
        flo32 RadiansD = TAU * tD + Adjust;
        
        vec2 A = ToDirection2D( RadiansA ) * DistA;
        vec2 B = ToDirection2D( RadiansB ) * DistB;
        vec2 C = ToDirection2D( RadiansC ) * DistA;
        vec2 D = ToDirection2D( RadiansD ) * DistB;
        
        int32 IndexA = iSegment * 2;
        int32 IndexB = iSegment * 2 + 1;
        
        if( FillSegment[ IndexA ] ) {
            Vertex[ AtVertex++ ] = Vertex1( Vec3( A, 0.0f ), Vec2( 0.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( C, 0.0f ), Vec2( 1.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( B, 0.0f ), Vec2( 0.5f, 1.0f ) );
            
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
        
        if( FillSegment[ IndexB ] ) {
            Vertex[ AtVertex++ ] = Vertex1( Vec3( D, 0.0f ), Vec2( 0.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( B, 0.0f ), Vec2( 1.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( C, 0.0f ), Vec2( 0.5f, 1.0f ) );
            
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
    }
    
    MODEL_DATA Result = UploadModel( Buffer, Vertex, AtVertex, ( uint32 * )Index, AtIndex );
    
    _PopArray( TempMemory, int32,   nIndex  );
    _PopArray( TempMemory, VERTEX1, nVertex );
    
    return Result;
}

internal BLIMP
BlimpC( int32 WaveGuid, int32 Move_iDest, vec2 Position ) {
    BLIMP Result = {};
    Result.WaveGuid   = WaveGuid;
    Result.Position   = Position;
    Result.Radius     = BLIMP_RADIUS;
    Result.Health     = BLIMP_HEALTH;
    Result.Move_iDest = Move_iDest;
    
    return Result;
}

internal void
AddBlimp( APP_STATE * AppState, BLIMP Blimp ) {
    BLIMP_STATE * BlimpS = &AppState->BlimpS;
    if( BlimpS->nBlimp < BLIMP_MAX_COUNT ) {
        BlimpS->Blimp[ BlimpS->nBlimp++ ] = Blimp;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BLIMP to BLIMP_STATE: %u", BLIMP_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateBlimp( APP_STATE * AppState, flo32 dT ) {
    BLIMP_STATE * BlimpS = &AppState->BlimpS;
    PLAYER_STATE  * Player   = &AppState->Player;
    
    for( int32 iBlimp = 0; iBlimp < BlimpS->nBlimp; iBlimp++ ) {
        BLIMP * Blimp = BlimpS->Blimp + iBlimp;
        
        Blimp->Damage_Timer = MaxValue( Blimp->Damage_Timer - dT, 0.0f );
        
        Blimp->Display_Timer += dT;
        if( Blimp->Display_Timer >= BLIMP_DISPLAY_SEGMENT_TARGET_TIME ) {
            Blimp->Display_Timer   -= BLIMP_DISPLAY_SEGMENT_TARGET_TIME;
            Blimp->Display_iSegment = ( Blimp->Display_iSegment + 1 ) % BLIMP_SEGMENT_MAX_COUNT;
        }
        
        { // Movement
            if( IsInRect( Blimp->Position, BlimpS->MoveDest[ Blimp->Move_iDest ] ) ) {
                Blimp->Move_iDest = ( Blimp->Move_iDest + 1 ) % BlimpS->nMoveDest;
            }
            
            rect Dest_Bound = RectCD( Blimp->Move_Dest, Vec2( 1.0f, 1.0f ) );
            if( ( IsInRect( Blimp->Position, Dest_Bound ) ) || ( Blimp->Move_Timer >= Blimp->Move_TargetTime ) ) {
                Blimp->Move_Timer      = 0.0f;
                Blimp->Move_TargetTime = RandomF32InRange( 2.5f, 4.0f );
                
                rect Bound = RectCD( Blimp->Position, Vec2( 5.0f, 5.0f ) );
                Blimp->Move_Dest  = GetRandomPoint( BlimpS->MoveDest[ Blimp->Move_iDest ] );
                Blimp->Move_Dest  = ClampToRect( Blimp->Move_Dest, Bound );
                
                flo32 t       = RandomF32();
                flo32 tHealth = 1.0f - Clamp01( ( flo32 )Blimp->Health / ( flo32 )BLIMP_HEALTH );
                Blimp->Move_Speed = lerp( 1.5f, t * t, 2.5f ) + tHealth;
            }
            
            vec2 Dir   = GetNormal( Blimp->Move_Dest - Blimp->Position );
            vec2 Accel = ( Dir * Blimp->Move_Speed - Blimp->Velocity ) * BLIMP_FRICTION;
            
            vec2 dPos = Accel * ( 0.5f * dT * dT ) + Blimp->Velocity * dT;
            // TODO: Collision!
            Blimp->Position += dPos;
            Blimp->Velocity += Accel * dT;
        }
        
        // i want a channel going around the world area
        // i want the blimp to travel ccw or cw around the world area
        // i want the blimp randomly drifting side to side within the channel
        // i want the blimp to randomly adjust its speed, like the drifter
        
        // pick a random degrees between 0-180, adjust dir so its headed toward a corner of the channel
        // we could just pick a random spot in the channel some distance in front of the blimp
        //  also can change its speed and dest based on a timer
        // it could cut corners. it doesn't have to stay strictly in the channel.
        // if the dest is outside the channel bound, then clip the dest and change direction
    }
}

internal void
DrawBlimp( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, BLIMP Blimp, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4  ColorA = BLIMP_BASE_COLORA;
    vec4  ColorB = BLIMP_BASE_COLORB;
    vec4  ColorC = TowardBlack( BLIMP_BASE_COLORA, 0.6f );
    flo32 Radius = Blimp.Radius * Scale;
    
    {
        flo32 tDamage = Clamp01( Blimp.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
        ColorA = lerp( ColorA, tDamage, TowardWhite( ColorA, 0.5f ) );
        ColorB = lerp( ColorB, tDamage, TowardWhite( ColorB, 0.5f ) );
    }
    
    
    DrawModel( Pass, MeshID_BlimpA, TextureID_Blimp, Blimp.Position, Vec2( Radius, Radius ), ColorA * Alpha );
    DrawModel( Pass, MeshID_BlimpB, TextureID_Blimp, Blimp.Position, Vec2( Radius, Radius ), ColorB * Alpha );
    
    if( Alpha >= 1.0f ) {
        flo32 Radians = Clamp01( ( flo32 )Blimp.Display_iSegment / ( flo32 )BLIMP_SEGMENT_MAX_COUNT ) * TAU;
        DrawModel( Pass, MeshID_BlimpC, TextureID_Blimp, Blimp.Position, Vec2( Radius, Radius ), Radians, ColorC );
        DrawCircle( Pass, Blimp.Position, Radius * 0.2f, ColorC );
    }
    
    if( Blimp.Health > 0 ) {
        DrawEntityHealth( Pass, AppState, Camera, Blimp.Position, Blimp.Health, COLOR_WHITE );
    }
}

internal void
DrawBlimp( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    BLIMP_STATE * BlimpS = Draw->BlimpS;
    
    for( int32 iBlimp = 0; iBlimp < BlimpS->nBlimp; iBlimp++ ) {
        BLIMP Blimp = BlimpS->Blimp[ iBlimp ];
        
        DrawBlimp( Pass, AppState, Camera, Blimp );
    }
}

internal void
FinalizeBlimp( APP_STATE * AppState ) {
    BLIMP_STATE * BlimpS = &AppState->BlimpS;
    
    uint32 NewCount = 0;
    for( int32 iBlimp = 0; iBlimp < BlimpS->nBlimp; iBlimp++ ) {
        BLIMP Blimp = BlimpS->Blimp[ iBlimp ];
        
        if( Blimp.Health > 0 ) {
            BlimpS->Blimp[ NewCount++ ] = Blimp;
        } else {
            AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORA );
            AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORA );
            AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORB );
            AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORB );
            AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, COLOR_WHITE, false );
            
            AddEntityDeathParticles( AppState, Blimp.Position, BLIMP_BASE_COLORA );
            AddEntityDeathParticles( AppState, Blimp.Position, BLIMP_BASE_COLORB );
            
            AddKillToWave( AppState, Blimp.WaveGuid );
            AddEntityToKilledCountForLevel( AppState, EntityType_Blimp, 1 );
            
            BlimpS->BonusObjective_nBlimpKills += 1;
            
            PlayEntityDeathAudio( AppState, AudioID_Entity_Death_Large );
        }
    }
    BlimpS->nBlimp = NewCount;
}