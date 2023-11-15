
struct MESH {
    vec3  * Vertex;
    vec2  * UV;
    int32 * Index;
    
    int32  nVertex;
    int32  nUV;
    int32  nIndex;
};

typedef MESH ( GEN_MESH )( MEMORY * Memory );

internal MESH
InitMesh( MEMORY * Memory, int32 nVertex, int32 nIndex ) {
    MESH Result = {};
    Result.Vertex  = _PushArray_Clear( Memory, vec3,  nVertex );
    Result.UV      = _PushArray_Clear( Memory, vec2,  nVertex );
    Result.Index   = _PushArray_Clear( Memory, int32, nIndex  );
    Result.nVertex = nVertex;
    Result.nUV     = nVertex;
    Result.nIndex  = nIndex;
    return Result;
}

internal MESH
GenMesh_Line( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 2, 2 );
    
    Result.Vertex[ 0 ] = Vec3( 0.0f, 0.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3( 1.0f, 0.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 0.0f );
    Result.UV[ 1 ] = Vec2( 0.0f, 0.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    
    return Result;
}

internal MESH
GenMesh_Rect( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 4, 6 );
    
    Result.Vertex[ 0 ] = Vec3( -1.0f, -1.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3(  1.0f, -1.0f, 0.0f );
    Result.Vertex[ 2 ] = Vec3( -1.0f,  1.0f, 0.0f );
    Result.Vertex[ 3 ] = Vec3(  1.0f,  1.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 0.0f );
    Result.UV[ 1 ] = Vec2( 1.0f, 0.0f );
    Result.UV[ 2 ] = Vec2( 0.0f, 1.0f );
    Result.UV[ 3 ] = Vec2( 1.0f, 1.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    Result.Index[ 2 ] = 2;
    Result.Index[ 3 ] = 3;
    Result.Index[ 4 ] = 2;
    Result.Index[ 5 ] = 1;
    
    return Result;
}

internal MESH
GenMesh_RectOutline( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 4, 5 );
    
    Result.Vertex[ 0 ] = Vec3( -1.0f, -1.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3(  1.0f, -1.0f, 0.0f );
    Result.Vertex[ 2 ] = Vec3( -1.0f,  1.0f, 0.0f );
    Result.Vertex[ 3 ] = Vec3(  1.0f,  1.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 0.0f );
    Result.UV[ 1 ] = Vec2( 1.0f, 0.0f );
    Result.UV[ 2 ] = Vec2( 0.0f, 1.0f );
    Result.UV[ 3 ] = Vec2( 1.0f, 1.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    Result.Index[ 2 ] = 3;
    Result.Index[ 3 ] = 2;
    Result.Index[ 4 ] = 0;
    
    return Result;
}

internal MESH
GenMesh_Circle( MEMORY * Memory ) {
    int32 nSegment = 64;
    
    MESH Result = InitMesh( Memory, nSegment + 1, nSegment * 3 );
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
        flo32 Radians = ( ( flo32 )iSegment / ( flo32 )nSegment ) * TAU;
        flo32 X = cosf( Radians );
        flo32 Y = sinf( Radians );
        
        Result.Vertex[ iSegment ] = Vec3( X, Y, 0.0f );
        Result.UV    [ iSegment ] = Vec2( 0.0f, 0.0f );
    }
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
        int32 bIndex = iSegment * 3;
        Result.Index[ bIndex + 0 ] = Result.nVertex;
        Result.Index[ bIndex + 1 ] = iSegment;                    // [0..63]
        Result.Index[ bIndex + 2 ] = ( iSegment + 1 ) % nSegment; // [1..64]
    }
    
    return Result;
}

internal MESH
GenMesh_CircleOutline( MEMORY * Memory ) {
    int32 nSegment = 64;
    
    MESH Result = InitMesh( Memory, nSegment, nSegment + 1 );
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
        flo32 Radians = ( ( flo32 )iSegment / ( flo32 )nSegment ) * TAU;
        flo32 X = cosf( Radians );
        flo32 Y = sinf( Radians );
        
        Result.Vertex[ iSegment ] = Vec3( X, Y, 0.0f );
        Result.UV    [ iSegment ] = Vec2( 0.0f, 0.0f );
    }
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
        Result.Index[ iSegment ] = iSegment;
    }
    
    return Result;
}

internal MESH
GenMesh_Triangle( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 3, 3 );
    
    Result.Vertex[ 0 ] = Vec3( -1.0f, -1.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3(  1.0f, -1.0f, 0.0f );
    Result.Vertex[ 2 ] = Vec3(  0.0f,  1.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 0.0f );
    Result.UV[ 1 ] = Vec2( 1.0f, 0.0f );
    Result.UV[ 2 ] = Vec2( 0.5f, 1.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    Result.Index[ 2 ] = 2;
    
    return Result;
}

internal MESH
GenMesh_Triangle_FlipUV( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 3, 3 );
    
    Result.Vertex[ 0 ] = Vec3( -1.0f, -1.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3(  1.0f, -1.0f, 0.0f );
    Result.Vertex[ 2 ] = Vec3(  0.0f,  1.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 1.0f );
    Result.UV[ 1 ] = Vec2( 1.0f, 1.0f );
    Result.UV[ 2 ] = Vec2( 0.5f, 0.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    Result.Index[ 2 ] = 2;
    
    return Result;
}

internal MESH
GenMesh_TriangleOutline( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 3, 4 );
    
    Result.Vertex[ 0 ] = Vec3( -1.0f, -1.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3(  1.0f, -1.0f, 0.0f );
    Result.Vertex[ 2 ] = Vec3(  0.0f,  1.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 0.0f );
    Result.UV[ 1 ] = Vec2( 0.0f, 0.0f );
    Result.UV[ 2 ] = Vec2( 0.0f, 0.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    Result.Index[ 2 ] = 2;
    Result.Index[ 3 ] = 0;
    
    return Result;
}

internal MESH
GenMesh_ObservationDeck_Barrier( MEMORY * Memory ) {
    flo32 Radius_Inner = OBSERVATION_DECK_RADIUS + OBSERVATION_DECK_BARRIER_DIST;
    flo32 Radius_Outer = Radius_Inner + OBSERVATION_DECK_BARRIER_WIDTH;
    flo32 Radians      = OBSERVATION_DECK_BARRIER_RADIANS;
    vec2  Center       = Vec2( -Radius_Inner, 0.0f );
    
    int32 nSegment          = OBSERVATION_DECK_BARRIER_SEGMENT_COUNT;
    flo32 RadiansPerSegment = Radians / ( flo32 )( nSegment );
    flo32 bRadians          = -Radians * 0.5f;
    
    int32 nTri    = nSegment + nSegment - 1;
    int32 nVertex = nTri * 3;
    int32 nIndex  = nTri * 3;
    
    MESH Result = InitMesh( Memory, nVertex, nIndex );
    
    int32 AtVertex = 0;
    int32 AtIndex  = 0;
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment ++ ) {
        vec2 PosA = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment - 0.5f ) ) * Radius_Outer;
        vec2 PosB = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment + 0.0f ) ) * Radius_Inner;
        vec2 PosC = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment + 0.5f ) ) * Radius_Outer;
        vec2 PosD = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment + 1.0f ) ) * Radius_Inner;
        
        if( iSegment > 0 ) {
            Result.Vertex[ AtVertex ] = Vec3( PosA, 0.0f );
            Result.UV    [ AtVertex ] = Vec2( 1.0f, 1.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( PosB, 0.0f );
            Result.UV    [ AtVertex ] = Vec2( 1.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( PosC, 0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.0f, 1.0f );  AtVertex++;
            
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
        
        Result.Vertex[ AtVertex ] = Vec3( PosD, 0.0f );
        Result.UV    [ AtVertex ] = Vec2( 0.0f, 0.0f );  AtVertex++;
        
        Result.Vertex[ AtVertex ] = Vec3( PosC, 0.0f );
        Result.UV    [ AtVertex ] = Vec2( 0.0f, 1.0f );  AtVertex++;
        
        Result.Vertex[ AtVertex ] = Vec3( PosB, 0.0f );
        Result.UV    [ AtVertex ] = Vec2( 1.0f, 0.0f );  AtVertex++;
        
        Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
        Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
        Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
    }
    
    return Result;
}

internal MESH
GenMesh_ObservationDeck_Icon( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 4, 6 );
    
    Result.Vertex[ 0 ] = Vec3( -1.0f, -1.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3(  1.0f, -1.0f, 0.0f );
    Result.Vertex[ 2 ] = Vec3( -1.0f,  1.0f, 0.0f );
    Result.Vertex[ 3 ] = Vec3(  1.0f,  1.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 0.35f );
    Result.UV[ 1 ] = Vec2( 1.0f, 0.35f );
    Result.UV[ 2 ] = Vec2( 0.0f, 1.0f );
    Result.UV[ 3 ] = Vec2( 1.0f, 1.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    Result.Index[ 2 ] = 2;
    Result.Index[ 3 ] = 3;
    Result.Index[ 4 ] = 2;
    Result.Index[ 5 ] = 1;
    
    return Result;
}

internal MESH
GenMesh_BlimpA( MEMORY * Memory ) {
    flo32 Radius   = 1.0f;
    int32 nLayer   = 5;
    int32 nSegment = BLIMP_SEGMENT_MAX_COUNT;
    
    int32 nVertex = nSegment * 3 * nLayer;
    int32 nIndex  = nVertex;
    
    MESH Result = InitMesh( Memory, nVertex, nIndex );
    
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
            
            Result.Vertex[ AtVertex ] = Vec3( A,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( B,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 1.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( C,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.5f, 1.0f );  AtVertex++;
            
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
    }
    
    return Result;
}

internal MESH
GenMesh_BlimpB( MEMORY * Memory ) {
    flo32 Radius   = 1.0f;
    int32 nLayer   = 5;
    int32 nSegment = BLIMP_SEGMENT_MAX_COUNT;
    
    int32 nVertex = nSegment * 3 * nLayer;
    int32 nIndex  = nVertex;
    
    MESH Result = InitMesh( Memory, nVertex, nIndex );
    
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
            flo32 tA = ( ( ( flo32 )iSegment + 0.5f ) / ( flo32 )nSegment );
            flo32 tB = ( ( ( flo32 )iSegment + 1.0f ) / ( flo32 )nSegment );
            flo32 tC = ( ( ( flo32 )iSegment + 1.5f ) / ( flo32 )nSegment );
            
            flo32 RadiansA = TAU * tA + Adjust;
            flo32 RadiansB = TAU * tB + Adjust;
            flo32 RadiansC = TAU * tC + Adjust;
            
            vec2 A = ToDirection2D( RadiansA ) * DistB;
            vec2 B = ToDirection2D( RadiansB ) * DistA;
            vec2 C = ToDirection2D( RadiansC ) * DistB;
            
            Result.Vertex[ AtVertex ] = Vec3( A,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( B,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 1.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( C,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.5f, 1.0f );  AtVertex++;
            
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
    }
    
    return Result;
}

internal MESH
GenMesh_BlimpC( MEMORY * Memory ) {
    flo32 Radius   = 1.0f;
    int32 nLayer   = 5;
    int32 nSegment = BLIMP_SEGMENT_MAX_COUNT;
    
    int32 nVertex = nSegment * 3 * nLayer;
    int32 nIndex  = nVertex;
    
    MESH Result = InitMesh( Memory, nVertex, nIndex );
    
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
            Result.Vertex[ AtVertex ] = Vec3( A,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( C,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 1.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( B,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.5f, 1.0f );  AtVertex++;
            
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
        
        if( FillSegment[ IndexB ] ) {
            Result.Vertex[ AtVertex ] = Vec3( D,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( B,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 1.0f, 0.0f );  AtVertex++;
            
            Result.Vertex[ AtVertex ] = Vec3( C,    0.0f );
            Result.UV    [ AtVertex ] = Vec2( 0.5f, 1.0f );  AtVertex++;
            
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
            Result.Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
    }
    
    return Result;
}

internal MESH
GenMesh_Turret_Shield_EndCap( MEMORY * Memory ) {
    MESH Result = InitMesh( Memory, 3, 3 );
    
    Result.Vertex[ 0 ] = Vec3( 0.0f, -1.0f, 0.0f );
    Result.Vertex[ 1 ] = Vec3( 1.0f, -1.0f, 0.0f );
    Result.Vertex[ 2 ] = Vec3( 0.0f,  1.0f, 0.0f );
    
    Result.UV[ 0 ] = Vec2( 0.0f, 0.0f );
    Result.UV[ 1 ] = Vec2( 1.0f, 0.0f );
    Result.UV[ 2 ] = Vec2( 0.0f, 1.0f );
    
    Result.Index[ 0 ] = 0;
    Result.Index[ 1 ] = 1;
    Result.Index[ 2 ] = 2;
    
    return Result;
}