
internal void
GenCubeCD( vec3 * Output, vec3 xAxis, vec3 yAxis, vec3 Origin, vec3 Dim ) {
    vec3 zAxis = Cross( xAxis, yAxis );
    
    xAxis *= ( Dim.x * 0.5f );
    yAxis *= ( Dim.y * 0.5f );
    zAxis *= ( Dim.z * 0.5f );
    
    Output[ 0 ] = Origin + xAxis + yAxis + zAxis;
    Output[ 1 ] = Origin + xAxis + yAxis - zAxis;
    Output[ 2 ] = Origin + xAxis - yAxis + zAxis;
    Output[ 3 ] = Origin + xAxis - yAxis - zAxis;
    Output[ 4 ] = Origin - xAxis + yAxis + zAxis;
    Output[ 5 ] = Origin - xAxis + yAxis - zAxis;
    Output[ 6 ] = Origin - xAxis - yAxis + zAxis;
    Output[ 7 ] = Origin - xAxis - yAxis - zAxis;
}

struct MODEL_GEN {
    VERTEX1 * Vertex;
    int32   * Index;
    int32     AtVertex;
    int32     AtIndex;
};

internal void
AddCube( MODEL_GEN * ModelGen, vec3 * Pos ) {
    int32 bVertex = ModelGen->AtVertex;
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 5 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 4 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 5 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 6 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 4 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 6 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 5 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 7 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 4 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 6 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 4 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 7 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 7 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 6 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 6 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 7 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 5 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 5 ], Vec2( 1.0f, 0.0f ) );
    
    int32 nVertex = ModelGen->AtVertex - bVertex;
    for( int32 iIndex = 0; iIndex < nVertex; iIndex++ ) {
        ModelGen->Index[ ModelGen->AtIndex ] = ModelGen->AtIndex;
        ModelGen->AtIndex++;
    }
}

internal void
AddTriangularBipyramid( MODEL_GEN * ModelGen, vec3 * Pos ) {
    int32 bVertex = ModelGen->AtVertex;
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 0.0f, 1.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 0 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 4 ], Vec2( 0.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 1.0f, 0.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 0.0f, 1.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 4 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 2 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 1.0f, 0.0f ) );
    
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 4 ], Vec2( 1.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 3 ], Vec2( 0.0f, 1.0f ) );
    ModelGen->Vertex[ ModelGen->AtVertex++ ] = Vertex1( Pos[ 1 ], Vec2( 1.0f, 0.0f ) );
    
    int32 nVertex = ModelGen->AtVertex - bVertex;
    for( int32 iIndex = 0; iIndex < nVertex; iIndex++ ) {
        ModelGen->Index[ ModelGen->AtIndex ] = ModelGen->AtIndex;
        ModelGen->AtIndex++;
    }
}

internal void
ScalePoint( vec3 * Point, vec3 Origin, vec3 Scale ) {
    Point[ 0 ] = Origin + ( Point[ 0 ] - Origin ) * Scale;
}

internal void
ScaleSideOfCube( vec3 * Pos, int32 iSide, vec3 Origin, vec3 Scale ) {
    // NOTE : This function assumes you're using the standard coordinate axes.
    // NOTE : This code is probably temporary. I probably don't want to think about cubes in this format/orientation, but it's fine for this project. Probably, in the future, I'll just be using Blender or something for modelling, instead of generating the models.
    
    switch( iSide ) {
        case 0: { // Right  || +X
            ScalePoint( Pos + 0, Origin, Scale );
            ScalePoint( Pos + 1, Origin, Scale );
            ScalePoint( Pos + 2, Origin, Scale );
            ScalePoint( Pos + 3, Origin, Scale );
        } break;
        
        case 1: { // Back   || +Y
            ScalePoint( Pos + 0, Origin, Scale );
            ScalePoint( Pos + 1, Origin, Scale );
            ScalePoint( Pos + 4, Origin, Scale );
            ScalePoint( Pos + 5, Origin, Scale );
        } break;
        
        case 2: { // Top    || +Z
            ScalePoint( Pos + 0, Origin, Scale );
            ScalePoint( Pos + 2, Origin, Scale );
            ScalePoint( Pos + 4, Origin, Scale );
            ScalePoint( Pos + 6, Origin, Scale );
        } break;
        
        case 3: { // Left   || -X
            ScalePoint( Pos + 4, Origin, Scale );
            ScalePoint( Pos + 5, Origin, Scale );
            ScalePoint( Pos + 6, Origin, Scale );
            ScalePoint( Pos + 7, Origin, Scale );
        } break;
        
        case 4: { // Front  || -Y
            ScalePoint( Pos + 2, Origin, Scale );
            ScalePoint( Pos + 3, Origin, Scale );
            ScalePoint( Pos + 6, Origin, Scale );
            ScalePoint( Pos + 7, Origin, Scale );
        } break;
        
        case 5: { // Bottom || -Z
            ScalePoint( Pos + 1, Origin, Scale );
            ScalePoint( Pos + 3, Origin, Scale );
            ScalePoint( Pos + 5, Origin, Scale );
            ScalePoint( Pos + 7, Origin, Scale );
        } break;
    }
}

internal void
TranslateSideOfCube( vec3 * Pos, int32 iSide, vec3 Translate ) {
    switch( iSide ) {
        case 0: { // Right  || +X
            Pos[ 0 ] += Translate;
            Pos[ 1 ] += Translate;
            Pos[ 2 ] += Translate;
            Pos[ 3 ] += Translate;
        } break;
        
        case 1: { // Back   || +Y
            Pos[ 0 ] += Translate;
            Pos[ 1 ] += Translate;
            Pos[ 4 ] += Translate;
            Pos[ 5 ] += Translate;
        } break;
        
        case 2: { // Top    || +Z
            Pos[ 0 ] += Translate;
            Pos[ 2 ] += Translate;
            Pos[ 4 ] += Translate;
            Pos[ 6 ] += Translate;
        } break;
        
        case 3: { // Left   || -X
            Pos[ 4 ] += Translate;
            Pos[ 5 ] += Translate;
            Pos[ 6 ] += Translate;
            Pos[ 7 ] += Translate;
        } break;
        
        case 4: { // Front  || -Y
            Pos[ 2 ] += Translate;
            Pos[ 3 ] += Translate;
            Pos[ 6 ] += Translate;
            Pos[ 7 ] += Translate;
        } break;
        
        case 5: { // Bottom || -Z
            Pos[ 1 ] += Translate;
            Pos[ 3 ] += Translate;
            Pos[ 5 ] += Translate;
            Pos[ 7 ] += Translate;
        } break;
    }
}

internal MODEL_DATA
GenModel_Cow( DIRECTX_12_RENDERER * Renderer, MEMORY * TempMemory ) {
    VERTEX1_BUFFER * Buffer = &Renderer->ModelBuffer;
    
    int32 nVertex = 4096;
    int32 nIndex  = 4096;
    
    MODEL_GEN  _ModelGen = {};
    MODEL_GEN * ModelGen = &_ModelGen;
    ModelGen->Vertex = _PushArray_Clear( TempMemory, VERTEX1, nVertex );
    ModelGen->Index  = _PushArray_Clear( TempMemory, int32,   nIndex  );
    
    vec3 Body_Dim  = Vec3( 2.0f, 1.0f, 1.0f );
    vec3 Leg_Dim   = Vec3( 0.25f, 0.25f, 1.0f );
    vec3 Head_Dim  = Vec3( 1.0f, 1.0f, 1.0f );
    vec3 Ear_Dim   = Vec3( 0.1f, 0.1f, 0.25f );
    vec3 Leg_Offset = Vec3( Body_Dim.x - Leg_Dim.x, Body_Dim.y - Leg_Dim.y, -Body_Dim.z - Leg_Dim.z ) * 0.5f;
    
    vec3 Origin   = Vec3( 0.0f, 0.0f, 0.0f );
    vec3 Pos[ 8 ] = {};
    
    { // BODY
        GenCubeCD( Pos, Vec3( 1.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Origin, Body_Dim );
        AddCube( ModelGen, Pos );
        
        GenCubeCD( Pos, Vec3( 1.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Origin + Leg_Offset * Vec3(  1.0f,  1.0f, 1.0f ), Leg_Dim );
        AddCube( ModelGen, Pos );
        GenCubeCD( Pos, Vec3( 1.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Origin + Leg_Offset * Vec3(  1.0f, -1.0f, 1.0f ), Leg_Dim );
        AddCube( ModelGen, Pos );
        GenCubeCD( Pos, Vec3( 1.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Origin + Leg_Offset * Vec3( -1.0f,  1.0f, 1.0f ), Leg_Dim );
        AddCube( ModelGen, Pos );
        GenCubeCD( Pos, Vec3( 1.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Origin + Leg_Offset * Vec3( -1.0f, -1.0f, 1.0f ), Leg_Dim );
        AddCube( ModelGen, Pos );
        
        GenCubeCD( Pos, Vec3( 1.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Origin + Body_Dim * Vec3( 0.5f, 0.0f, 0.0f ) + Head_Dim * Vec3( 0.5f, 0.0f, 0.0f ), Head_Dim );
        ScaleSideOfCube    ( Pos, 0, Origin, Vec3(  1.0f,  0.7f, 0.65f ) );
        TranslateSideOfCube( Pos, 0,         Vec3( -0.4f, 0.0f, 0.35f ) );
        Pos[ 1 ].x -= 0.1f;
        Pos[ 3 ].x -= 0.1f;
        AddCube( ModelGen, Pos );
        
        Pos[ 4 ] = Pos[ 0 ];
        Pos[ 5 ] = Pos[ 1 ];
        Pos[ 6 ] = Pos[ 2 ];
        Pos[ 7 ] = Pos[ 3 ];
        TranslateSideOfCube( Pos, 0,         Vec3( 0.65f, 0.0f, -0.35f ) );
        Pos[ 0 ].z -= 0.35f;
        Pos[ 2 ].z -= 0.35f;
        ScaleSideOfCube    ( Pos, 0, Origin, Vec3( 1.0f,  0.5f,  1.0f  ) );
        AddCube( ModelGen, Pos );
        
        Pos[ 0 ] = Pos[ 6 ] + Vec3( 0.0f, 0.0f, -0.05f );
        Pos[ 4 ] = Pos[ 0 ] + Vec3( 0.0f, 0.0f, Ear_Dim.z );
        Pos[ 1 ] = Pos[ 0 ] + Vec3( 0.0f, -Ear_Dim.y * 0.5f, Ear_Dim.z * 0.5f );
        Pos[ 2 ] = Pos[ 0 ] + Vec3( 0.0f,  Ear_Dim.y * 0.5f, Ear_Dim.z * 0.5f );
        Pos[ 3 ] = Pos[ 0 ] + Vec3( -Ear_Dim.x,  0.0f, Ear_Dim.z * 0.5f );
        AddTriangularBipyramid( ModelGen, Pos );
        
        flo32 OffsetY = fabsf( Pos[ 0 ].y ) * 2.0f;
        Pos[ 0 ].y += OffsetY;
        Pos[ 1 ].y += OffsetY;
        Pos[ 2 ].y += OffsetY;
        Pos[ 3 ].y += OffsetY;
        Pos[ 4 ].y += OffsetY;
        AddTriangularBipyramid( ModelGen, Pos );
    }
    
    nVertex = ModelGen->AtVertex;
    nIndex  = ModelGen->AtIndex;
    
    MODEL_DATA Result = UploadModel( Buffer, ModelGen->Vertex, nVertex, ( uint32 * )ModelGen->Index, nIndex );
    
    _PopArray( TempMemory, int32,   nIndex  );
    _PopArray( TempMemory, VERTEX1, nVertex );
    
    return Result;
}

internal MODEL_DATA
GenModel_ObservationDeckBarrier( DIRECTX_12_RENDERER * Renderer, MEMORY * TempMemory ) {
    flo32 Radius_Inner = OBSERVATION_DECK_RADIUS + OBSERVATION_DECK_BARRIER_DIST;
    flo32 Radius_Outer = Radius_Inner + OBSERVATION_DECK_BARRIER_WIDTH;
    flo32 Radians      = OBSERVATION_DECK_BARRIER_RADIANS;
    vec2  Center       = Vec2( -Radius_Inner, 0.0f );
    
    int32 nSegment          = OBSERVATION_DECK_BARRIER_SEGMENT_COUNT;
    flo32 RadiansPerSegment = Radians / ( flo32 )( nSegment );
    flo32 bRadians          = -Radians * 0.5f;
    
    VERTEX1_BUFFER * Buffer = &Renderer->ModelBuffer;
    
    int32 nVertex = 1 + ( nSegment * 2 );
    int32 nTri    = ( ( nSegment ) + ( nSegment - 1 ) );
    int32 nIndex  = nTri * 3;
    
    nTri    = nSegment + nSegment - 1;
    nVertex = nTri * 3;
    nIndex  = nTri * 3;
    
    VERTEX1 * Vertex = _PushArray_Clear( TempMemory, VERTEX1, nVertex );
    int32   * Index  = _PushArray_Clear( TempMemory, int32,   nIndex  );
    
    int32 AtVertex = 0;
    int32 AtIndex  = 0;
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment ++ ) {
        vec2 PosA = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment - 0.5f ) ) * Radius_Outer;
        vec2 PosB = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment + 0.0f ) ) * Radius_Inner;
        vec2 PosC = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment + 0.5f ) ) * Radius_Outer;
        vec2 PosD = Center + ToDirection2D( bRadians + RadiansPerSegment * ( ( flo32 )iSegment + 1.0f ) ) * Radius_Inner;
        
        if( iSegment > 0 ) {
            Vertex[ AtVertex++ ] = Vertex1( Vec3( PosA, 0.0f ), Vec2( 1.0f, 1.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( PosB, 0.0f ), Vec2( 1.0f, 0.0f ) );
            Vertex[ AtVertex++ ] = Vertex1( Vec3( PosC, 0.0f ), Vec2( 0.0f, 1.0f ) );
            
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
            Index[ AtIndex ] = AtIndex;  AtIndex++;
        }
        
        Vertex[ AtVertex++ ] = Vertex1( Vec3( PosD, 0.0f ), Vec2( 0.0f, 0.0f ) );
        Vertex[ AtVertex++ ] = Vertex1( Vec3( PosC, 0.0f ), Vec2( 0.0f, 1.0f ) );
        Vertex[ AtVertex++ ] = Vertex1( Vec3( PosB, 0.0f ), Vec2( 1.0f, 0.0f ) );
        
        Index[ AtIndex ] = AtIndex;  AtIndex++;
        Index[ AtIndex ] = AtIndex;  AtIndex++;
        Index[ AtIndex ] = AtIndex;  AtIndex++;
    }
    
    MODEL_DATA Result = UploadModel( Buffer, Vertex, nVertex, ( uint32 * )Index, nIndex );
    
    _PopArray( TempMemory, int32,   nIndex  );
    _PopArray( TempMemory, VERTEX1, nVertex );
    
    return Result;
}

internal SECURITY_DRONE
SecurityDroneC( vec2 Position, vec2 Destination ) {
    SECURITY_DRONE Result = {};
    Result.Position    = Position;
    Result.Destination = Destination;
    Result.Radius      = SECURITY_DRONE_RADIUS;
    Result.Health      = SECURITY_DRONE_HIT_OBSERVATION_DECK_RECOVER_HEALTH;
    Result.Friction    = SECURITY_DRONE_FRICTION;
    
    Result.Stage       = SecurityDroneStage_Spawn;
    
    Result.nLaser          = SECURITY_DRONE_LASER_MAX_COUNT;
    Result.Laser_MaxLength = 100.0f;
    
    return Result;
}

internal BOARD_MEMBER
BoardMemberC( vec2 Position ) {
    BOARD_MEMBER Result = {};
    Result.Position = Position;
    Result.Color[ 0 ] = Vec4( RandomF32(), RandomF32(), RandomF32(), 1.0f );
    Result.Color[ 1 ] = Vec4( RandomF32(), RandomF32(), RandomF32(), 1.0f );
    Result.Color[ 2 ] = Vec4( RandomF32(), RandomF32(), RandomF32(), 1.0f );
    
    Result.Direction = ToDirection2D( ( -PI * 0.5f ) + RandomF32InRange( -1.0f, 1.0f ) * ( PI * 0.5f ) );
    Result.Speed     = RandomF32InRange( 2.5f, 4.5f );
    Result.Timer     = -( ( flo32 )RandomS32InRange( 2, 60 ) / 60.0f );
    Result.dRadians  = RandomF32InRange( -1.0f, 1.0f ) * ( TAU / 3.0f );
    
    return Result;
}

internal vec2
GetMoveDestinationForSecurityDrone( APP_STATE * AppState ) {
    PLAYER_STATE     * Player     = &AppState->Player;
    BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
    
    OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
    SECURITY_DRONE   * SecurityDrone   = &BoardRoomS->SecurityDrone;
    
    // NOTE: The destination can place the security drone partially inside the observation deck shield, but the security drone shouldn't be fully within the shield.
    // NOTE: So, this bound needs to account for the missile launcher shield, also.
    // NOTE: This should also try to select a destination near the player.
    
    flo32 Top = ObservationDeck->Position.y - ObservationDeck->Radius;
    if( ObservationDeck->Shield_IsActive ) {
        Top -= ObservationDeck->Shield_MaxDist;
    } else if( ObservationDeck->Barrier_IsActive ) {
        Top -= ( ObservationDeck->Barrier_Dist + OBSERVATION_DECK_BARRIER_WIDTH );
    }
    
    rect Bound = AppState->World_Bound;
    
    if( ObservationDeck->IsPresent ) {
        Bound.Top  = Top;
    }
    
    flo32 tMargin          = 1.0f - Clamp01( ( flo32 )ObservationDeck->Health / ( flo32 )OBSERVATION_DECK_HEALTH );
    flo32 LowHealth_Margin = lerp( 0.0f, tMargin, 2.0f );
    
    Bound      = AddRadius( Bound, -( SECURITY_DRONE_MOVE_MARGIN + SecurityDrone->Radius + LowHealth_Margin ) );
    
    // NOTE: Currently, the Player can move to the top of the screen where Player_Bound and Bound do not overlap. I want to fix the level later, so that Player_Bound and Bound will always overlap. I can probably just make Player_Bound bigger.
    vec2 Result = {};
    if( IsInRect( SecurityDrone->Position, AppState->World_Bound ) ) {
        if( ( ObservationDeck->IsPresent ) && ( ObservationDeck->Health >= 0 ) ) {
            rect BoundA = AddRadius( Bound, SECURITY_DRONE_MOVE_PLAYER_DIST );
            if( IsInRect( Player->Position, BoundA ) ) {
                rect Player_Bound = RectCR( Player->Position, SECURITY_DRONE_MOVE_PLAYER_DIST );
                Bound = GetRectRectIntersect( Bound, Player_Bound );
            }
        }
        Result = GetRandomPoint( Bound );
    } else {
        Result = ClampToRect( SecurityDrone->Position, Bound );
    }
    
    return Result;
}

internal void
SelectStageForSecurityDrone( APP_STATE * AppState, int32 * Numer, int32 nNumer ) {
    BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
    OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
    SECURITY_DRONE   * SecurityDrone   = &BoardRoomS->SecurityDrone;
    PLAYER_STATE     * Player          = &AppState->Player;
    
    flo32 Top = ObservationDeck->Position.y - ObservationDeck->Radius;
    if( ObservationDeck->Shield_IsActive ) {
        Top -= ObservationDeck->Shield_MaxDist;
    } else if( ObservationDeck->Barrier_IsActive ) {
        Top -= ( ObservationDeck->Barrier_Dist + OBSERVATION_DECK_BARRIER_WIDTH );
    }
    
    rect Bound_Center = AppState->World_Bound;
    Bound_Center.Top  = Top;
    Bound_Center = AddRadius( Bound_Center, -( SecurityDrone->Radius + SECURITY_DRONE_MOVE_MARGIN ) );
    
    rect Bound_Left  = Bound_Center;
    rect Bound_Right = Bound_Center;
    
    flo32 Mid          = ( Bound_Center.Left + Bound_Center.Right ) * 0.5f;
    flo32 Width        = GetWidth( AppState->World_Bound ) * 0.35f;
    flo32 HalfWidth    = Width * 0.5f;
    Bound_Center.Left  = Mid - HalfWidth;
    Bound_Center.Right = Mid + HalfWidth;
    Bound_Left.Right   = Mid - HalfWidth;
    Bound_Right.Left   = Mid + HalfWidth;
    
    if( ( SecurityDrone->Position.y > Bound_Center.Top ) || ( SecurityDrone->Position.y < Bound_Center.Bottom ) ) {
        Numer[ SecurityDroneStage_Spin_Lasers  ] = 0;
        Numer[ SecurityDroneStage_Sweep_Lasers ] = 0;
    }
    if( ( SecurityDrone->Position.x > Bound_Center.Left ) && ( SecurityDrone->Position.x < Bound_Center.Right ) ) {
        Numer[ SecurityDroneStage_Sweep_Bullets ] = 0;
        Numer[ SecurityDroneStage_Sweep_Lasers  ] = 0;
    } else {
        Numer[ SecurityDroneStage_Spin_Lasers  ] = 0;
    }
    if( ObservationDeck->Health <= 0 ) {
        Numer[ SecurityDroneStage_Spin_Lasers  ] = 0;
        Numer[ SecurityDroneStage_Sweep_Lasers ] = 0;
    }
    
    int32 Select = RandomSelectByOdds( Numer, SecurityDroneStage_Count );
    Assert( Select != ( int32 )SecurityDroneStage_Spawn );
    
    if( ( !ObservationDeck->IsPresent ) || ( Player->Health <= 0 ) ) {
        Select = SecurityDroneStage_Move;
    }
    if( Select == -1 ) {
        Select = SecurityDroneStage_Move;
    }
    
    switch( Select ) {
        case SecurityDroneStage_Move: {
            SecurityDrone->Stage = SecurityDroneStage_Move;
            SecurityDrone->Timer = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME );
            SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
            
            SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
        } break;
        
        case SecurityDroneStage_Spin_Bullets: {
            SecurityDrone->Timer      = -( SECURITY_DRONE_BULLETS_WARMUP_TARGET_TIME );
            SecurityDrone->Fire_Timer = -( SECURITY_DRONE_BULLETS_WARMUP_TARGET_TIME );
            SecurityDrone->Fire_TargetTime = ( 9.0f / 60.0f );
            SecurityDrone->Stage           = SecurityDroneStage_Spin_Bullets;
            SecurityDrone->Counter         = 60;
            
            if( ObservationDeck->Health >= OBSERVATION_DECK_HEALTH ) {
                SecurityDrone->Counter = 45;
            }
        } break;
        
        case SecurityDroneStage_Spin_Lasers: {
            SecurityDrone->Timer   = -( SECURITY_DRONE_LASERS_WARMUP_TARGET_TIME );
            SecurityDrone->Stage   = SecurityDroneStage_Spin_Lasers;
        } break;
        
        case SecurityDroneStage_Sweep_Bullets: {
            SecurityDrone->Timer      = -( SECURITY_DRONE_BULLETS_WARMUP_TARGET_TIME );
            SecurityDrone->Fire_Timer = -( SECURITY_DRONE_BULLETS_WARMUP_TARGET_TIME );
            SecurityDrone->Fire_TargetTime = ( 18.0f / 60.0f );
            SecurityDrone->Stage           = SecurityDroneStage_Sweep_Bullets;
            
            if( SecurityDrone->Position.x < Mid ) {
                SecurityDrone->Destination = GetRandomPoint( Bound_Right );
            } else {
                SecurityDrone->Destination = GetRandomPoint( Bound_Left );
            }
        } break;
        
        case SecurityDroneStage_Sweep_Lasers: {
            SecurityDrone->Timer   = -( SECURITY_DRONE_LASERS_WARMUP_TARGET_TIME );
            SecurityDrone->Stage   = SecurityDroneStage_Sweep_Lasers;
            
            if( SecurityDrone->Position.x < Mid ) {
                SecurityDrone->Destination = GetRandomPoint( Bound_Right );
            } else {
                SecurityDrone->Destination = GetRandomPoint( Bound_Left );
            }
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
}

internal void
UpdateBoardRoom( APP_STATE * AppState, flo32 dT ) {
    BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
    PLAYER_STATE     * Player     = &AppState->Player;
    
    if( BoardRoomS->State_IsActive ) {
        OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
        SECURITY_DRONE   * SecurityDrone   = &BoardRoomS->SecurityDrone;
        
        DISPLAY_VALUE( flo32, ObservationDeck->Barrier_Dist );
        
        { // Update Observation Deck Exit
            SCORE_STATE * ScoreS = &AppState->ScoreS;
            if( ( ObservationDeck->IsPresent ) && ( ObservationDeck->Health >= 0 ) && ( ScoreS->Level_DisplayTimer <= 6.0f ) && ( Player->Health > 0 ) ) {
                ObservationDeck->Teleport_Timer      += dT;
                ObservationDeck->Teleport_ScaleTimer += dT;
            } else {
                ObservationDeck->Teleport_Timer      = MaxValue( ObservationDeck->Teleport_Timer - ( dT * 4.0f ), 0.0f );
                ObservationDeck->Teleport_ScaleTimer = MaxValue( ObservationDeck->Teleport_Timer - ( dT * 4.0f ), 0.0f );
            }
            
            vec2 Teleport_bPos  = {};
            ObservationDeck->Teleport_tFlash = MaxValue( ObservationDeck->Teleport_tFlash - ( 8.0f * dT ) * 2.0f, 0.0f );
            if( ObservationDeck->Teleport_tFlash > 0.0f ) {
                AddYachtExitParticles( AppState, Teleport_bPos, 4, 1.0f, 8.0f );
            }
            
            boo32 DoExit = true;
            if( ( ObservationDeck->Teleport_Timer <= 0.0f ) || ( !ObservationDeck->IsPresent ) || ( ObservationDeck->Health < 0 ) || ( Player->Health <= 0 ) ) {
                DoExit = false;
            }
            
            vec2 InnerRing_Speed = Vec2( TAU / 6.0f, TAU / 2.0f );
            vec2 MidRing_Speed   = Vec2( TAU / 8.0f, TAU / 2.0f );
            UpdateRingForYachtExit( &ObservationDeck->Teleport_InnerRing, dT, DoExit, ObservationDeck->Teleport_Timer, InnerRing_Speed, Vec3( 1.25f, 2.0f, 0.25f ) );
            UpdateRingForYachtExit( &ObservationDeck->Teleport_MidRing,   dT, DoExit, ObservationDeck->Teleport_Timer, MidRing_Speed, Vec3( 2.0f,  2.25f, 0.35f ) );
            
            { // Update particles
                YACHT_EXIT_RING_DATA Ring = ObservationDeck->Teleport_MidRing;
                if( Ring.Alpha >= 0.7f ) {
                    flo32 tSpeed = Clamp01( ( Ring.RotateSpeed - MidRing_Speed.x ) / ( MidRing_Speed.y - MidRing_Speed.x ) );
                    flo32 Numer = lerp( 6.0f, tSpeed, 2.0f );
                    
                    flo32 TargetTime = ( Numer / 60.0f );
                    
                    ObservationDeck->Teleport_ParticleTimer += dT;
                    if( ObservationDeck->Teleport_ParticleTimer >= TargetTime ) {
                        ObservationDeck->Teleport_ParticleTimer = 0.0f;
                        
                        AddYachtExitParticles( AppState, Teleport_bPos, 1, 0.1f, 8.0f );
                    }
                }
            }
        }
        
        { // Update Observation Deck Art 
            if( ObservationDeck->Health < OBSERVATION_DECK_HEALTH ) {
                ObservationDeck->Light_Timer += dT;
                if( ObservationDeck->Light_Timer >= OBSERVATION_DECK_CONFERENCE_ROOM_LIGHT_TARGET_TIME ) {
                    ObservationDeck->Light_Timer = -OBSERVATION_DECK_CONFERENCE_ROOM_LIGHT_TARGET_TIME;
                }
            }
        }
        
        { // Update Observation Deck Barrier
            if( ( !ObservationDeck->Barrier_IsActive ) && ( ObservationDeck->Health < OBSERVATION_DECK_HEALTH ) ) {
                ObservationDeck->Barrier_IsActive = true;
            }
            
            if( ObservationDeck->Barrier_IsActive ) {
                {
                    flo32 dBarrier = ( OBSERVATION_DECK_BARRIER_DIST / 2.5f ) * dT;
                    ObservationDeck->Barrier_Dist = MinValue( ObservationDeck->Barrier_Dist + dBarrier, OBSERVATION_DECK_BARRIER_DIST );
                }
                
                if( ObservationDeck->Barrier_Dist >= OBSERVATION_DECK_BARRIER_DIST ) {
                    flo32 dRotateSpeed = ( OBSERVATION_DECK_BARRIER_ROTATE_SPEED / 4.0f ) * dT;
                    
                    if( ObservationDeck->Health <= -1 ) {
                        ObservationDeck->Barrier_RotateSpeed = MaxValue( ObservationDeck->Barrier_RotateSpeed - dRotateSpeed * 0.5f, 0.0f );
                    } else {
                        ObservationDeck->Barrier_RotateSpeed = MinValue( ObservationDeck->Barrier_RotateSpeed + dRotateSpeed, OBSERVATION_DECK_BARRIER_ROTATE_SPEED );
                    }
                    
                    if( ObservationDeck->Barrier_RotateSpeed > 0.0f ) {
                        ObservationDeck->Barrier_Radians = WrapRadians( ObservationDeck->Barrier_Radians - ObservationDeck->Barrier_RotateSpeed * dT );
                    }
                }
            }
        }
        
        { // Update Observation Deck Attack
            if( ObservationDeck->IsPresent ) {
                flo32 bRadians          = -( PI / 4.0f );
                flo32 Range             = PI - ( -bRadians * 2.0f );
                int32 nSegments         = 12;
                flo32 RadiansPerSegment = Range / ( flo32 )nSegments;
                flo32 Max_dRadians = ( RadiansPerSegment / 3.0f ) / OBSERVATION_DECK_ATTACK_CYCLE_TARGET_TIME;
                
                if( ObservationDeck->Health == 0 ) {
                    ObservationDeck->TurretAnim_Timer = MinValue( ObservationDeck->TurretAnim_Timer + dT, OBSERVATION_DECK_TURRET_ANIM_TARGET_TIME );
                    
                    if( ObservationDeck->TurretAnim_Timer >= OBSERVATION_DECK_TURRET_ANIM_TARGET_TIME ) {
                        ObservationDeck->Attack_Timer += dT;
                    }
                    if( Player->Health <= 0 ) {
                        ObservationDeck->Attack_Timer = 0.0f;
                    }
                    
                    flo32 TargetTime = OBSERVATION_DECK_ATTACK_TARGET_TIME;
                    if( ObservationDeck->Attack_Counter > 0 ) {
                        TargetTime = OBSERVATION_DECK_ATTACK_CYCLE_TARGET_TIME;
                    }
                    
                    if( ObservationDeck->TurretAnim_Timer >= ( OBSERVATION_DECK_TURRET_ANIM_TARGET_TIME - ( 30.0f / 60.0f ) ) ) {
                        ObservationDeck->Attack_dRadians = MinValue( ObservationDeck->Attack_dRadians + Max_dRadians * dT / 2.0f, Max_dRadians );
                    }
                    ObservationDeck->Attack_Radians += ( ObservationDeck->Attack_dRadians * dT );
                    if( ObservationDeck->Attack_Radians >= RadiansPerSegment ) {
                        ObservationDeck->Attack_Radians = 0.0f;
                    }
                    
                    if( ObservationDeck->Attack_Timer >= TargetTime ) {
                        ObservationDeck->Attack_Timer -= TargetTime;
                        
                        if( ObservationDeck->Attack_Counter > 0 ) {
                            ObservationDeck->Attack_Counter -= 1;
                        } else {
                            ObservationDeck->Attack_Counter  = 11;
                        }
                        
                        for( int32 iSegment = 0; iSegment < nSegments; iSegment++ ) {
                            flo32 Radians = bRadians + ObservationDeck->Attack_Radians - RadiansPerSegment * ( flo32 )iSegment;
                            
                            vec2  Dir     = ToDirection2D( Radians );
                            flo32 Radius  = ObservationDeck->Radius + 0.5f;
                            vec2  Pos     = ObservationDeck->Position + Dir * Radius;
                            
                            AddEnemyBullet( AppState, EnemyBulletC( Pos, Dir ) );
                        }
                    }
                } else if( ObservationDeck->Health <= -1 ) {
                    ObservationDeck->Attack_Timer = MaxValue( ObservationDeck->Attack_Timer - dT / 3.0f, 0.0f );
                    
                    ObservationDeck->Attack_dRadians = MaxValue( ObservationDeck->Attack_dRadians - Max_dRadians * dT / 3.0f, 0.0f );
                    ObservationDeck->Attack_Radians += ( ObservationDeck->Attack_dRadians * dT );
                    if( ObservationDeck->Attack_Radians >= RadiansPerSegment ) {
                        ObservationDeck->Attack_Radians = 0.0f;
                    }
                }
            }
        }
        
        { // Collide Enemy Bullets with Observation Deck
            ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
            
            flo32 Radius   = ObservationDeck->Radius;
            if( ObservationDeck->Shield_IsActive ) {
                Radius += ObservationDeck->Shield_Dist;
            }
            flo32 RadiusSq = Radius * Radius;
            
            for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
                ENEMY_BULLET * EnemyBullet = EnemyBulletS->EnemyBullet + iEnemyBullet;
                
                flo32 DistSq = GetLengthSq( EnemyBullet->Position - ObservationDeck->Position );
                if( DistSq <= RadiusSq ) {
                    EnemyBullet->Health = 0;
                    
                    if( ObservationDeck->Shield_IsActive ) {
                        EnemyBullet->DoShieldAbsorb = true;
                    } else {
                        EnemyBullet->DoHitPointDrop = true;
                    }
                }
            }
        }
        
        // Collide Entities with Observation Deck Barrier
        if( ObservationDeck->Barrier_IsActive ) {
            int32 nBarrier = OBSERVATION_DECK_BARRIER_COUNT;
            
            flo32 Radius_Inner = ObservationDeck->Radius + ObservationDeck->Barrier_Dist;
            flo32 Radius_Outer = Radius_Inner + OBSERVATION_DECK_BARRIER_WIDTH;
            
            flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
            flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
            
            vec2 Edge_Origin[ OBSERVATION_DECK_BARRIER_COUNT ][ 2 ] = {};
            vec2 Edge_Dir   [ OBSERVATION_DECK_BARRIER_COUNT ][ 2 ] = {};
            
            for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                flo32 Radians    = ObservationDeck->Barrier_Radians + ( TAU * 0.25f * ( flo32 )iBarrier ) - OBSERVATION_DECK_BARRIER_RADIANS * 0.5f;
                flo32 MinRadians_Inner = Radians;
                flo32 MaxRadians_Inner = MinRadians_Inner + OBSERVATION_DECK_BARRIER_RADIANS;
                
                flo32 BarrierRadiansPerSegment = ( OBSERVATION_DECK_BARRIER_RADIANS / ( flo32 )OBSERVATION_DECK_BARRIER_SEGMENT_COUNT );
                flo32 MinRadians_Outer = MinRadians_Inner + ( BarrierRadiansPerSegment * 0.5f );
                flo32 MaxRadians_Outer = MaxRadians_Inner - ( BarrierRadiansPerSegment * 0.5f );
                
                vec2 A = ObservationDeck->Position + ToDirection2D( MinRadians_Inner ) * Radius_Inner;
                vec2 B = ObservationDeck->Position + ToDirection2D( MinRadians_Outer ) * Radius_Outer;
                vec2 C = ObservationDeck->Position + ToDirection2D( MaxRadians_Outer ) * Radius_Outer;
                vec2 D = ObservationDeck->Position + ToDirection2D( MaxRadians_Inner ) * Radius_Inner;
                
                Edge_Origin[ iBarrier ][ 0 ] = A;
                Edge_Origin[ iBarrier ][ 1 ] = C;
                Edge_Dir   [ iBarrier ][ 0 ] = GetPerp( B - A );
                Edge_Dir   [ iBarrier ][ 1 ] = GetPerp( D - C );
            }
            
            { // Enemy Bullets
                ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
                for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
                    ENEMY_BULLET * EnemyBullet = EnemyBulletS->EnemyBullet + iEnemyBullet;
                    
                    flo32 DistSq = GetLengthSq( EnemyBullet->Position - ObservationDeck->Position );
                    if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                        
                        for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                            vec2  VA   = EnemyBullet->Position - Edge_Origin[ iBarrier ][ 0 ];
                            vec2  VB   = EnemyBullet->Position - Edge_Origin[ iBarrier ][ 1 ];
                            
                            flo32 DotA = dot( VA, Edge_Dir[ iBarrier ][ 0 ] );
                            flo32 DotB = dot( VB, Edge_Dir[ iBarrier ][ 1 ] );
                            
                            if( ( DotA >= 0.0f ) && ( DotB >= 0.0f ) ) {
                                EnemyBullet->Health         = 0;
                                EnemyBullet->DoHitPointDrop = true;
                            }
                        }
                    }
                }
            }
            
            { // Missiles
                MISSILE_STATE * MissileS = &AppState->MissileS;
                for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
                    MISSILE * Missile = MissileS->Missile + iMissile;
                    
                    flo32 DistSq = GetLengthSq( Missile->Position - ObservationDeck->Position );
                    if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                        
                        for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                            vec2  VA   = Missile->Position - Edge_Origin[ iBarrier ][ 0 ];
                            vec2  VB   = Missile->Position - Edge_Origin[ iBarrier ][ 1 ];
                            
                            flo32 DotA = dot( VA, Edge_Dir[ iBarrier ][ 0 ] );
                            flo32 DotB = dot( VB, Edge_Dir[ iBarrier ][ 1 ] );
                            
                            if( ( DotA >= 0.0f ) && ( DotB >= 0.0f ) ) {
                                Missile->Health = 0;
                            }
                        }
                    }
                }
            }
        }
        
        { // Collide Lasers with Observation Deck
            if( SecurityDrone->Laser_IsActive[ 0 ] ) {
                int32 nLaser = SecurityDrone->nLaser;
                for( int32 iLaser = 0; iLaser < nLaser; iLaser++ ) {
                    flo32 Radians0 = SecurityDrone->Radians + ( ( TAU / ( flo32 )nLaser ) * ( flo32 )iLaser ) + ( TAU / ( ( flo32 )nLaser * 2.0f ) );
                    
                    vec2 Dir = ToDirection2D( Radians0 );
                    vec2 V   = Dir * SecurityDrone->Laser_MaxLength;
                    RAY2 Ray = Ray2( SecurityDrone->Position, V );
                    
                    SecurityDrone->Laser_Dest[ iLaser ] = SecurityDrone->Position + V;
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    BestIntersect.t = 1.0f;
                    
                    { // Observation Deck
                        flo32 Radius = ObservationDeck->Radius - MINER_LASER_WIDTH * 0.5f;
                        if( ObservationDeck->Shield_IsActive ) {
                            Radius += ObservationDeck->Shield_Dist;
                        }
                        
                        circ Bound = Circ( ObservationDeck->Position, Radius );
                        DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                    }
                    
                    { // Observation Deck Barrier
                        int32 nBarrier = OBSERVATION_DECK_BARRIER_COUNT;
                        
                        flo32 Radius_Inner = ObservationDeck->Radius + ObservationDeck->Barrier_Dist;
                        flo32 Radius_Outer = Radius_Inner + OBSERVATION_DECK_BARRIER_WIDTH;
                        
                        for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                            flo32 Radians    = ObservationDeck->Barrier_Radians + ( TAU * 0.25f * ( flo32 )iBarrier ) - OBSERVATION_DECK_BARRIER_RADIANS * 0.5f;
                            flo32 MinRadians_Inner = Radians;
                            flo32 MaxRadians_Inner = MinRadians_Inner + OBSERVATION_DECK_BARRIER_RADIANS;
                            
                            flo32 BarrierRadiansPerSegment = ( OBSERVATION_DECK_BARRIER_RADIANS / ( flo32 )OBSERVATION_DECK_BARRIER_SEGMENT_COUNT );
                            flo32 MinRadians_Outer = MinRadians_Inner + ( BarrierRadiansPerSegment * 0.5f );
                            flo32 MaxRadians_Outer = MaxRadians_Inner - ( BarrierRadiansPerSegment * 0.5f );
                            
                            DoesRayIntersectCircleSegmentExt( &BestIntersect, Ray, ObservationDeck->Position, Radius_Outer, MinRadians_Outer, MaxRadians_Outer );
                            
                            vec2 A = ObservationDeck->Position + ToDirection2D( MinRadians_Inner ) * Radius_Inner;
                            vec2 B = ObservationDeck->Position + ToDirection2D( MinRadians_Outer ) * Radius_Outer;
                            vec2 C = ObservationDeck->Position + ToDirection2D( MaxRadians_Outer ) * Radius_Outer;
                            vec2 D = ObservationDeck->Position + ToDirection2D( MaxRadians_Inner ) * Radius_Inner;
                            
                            DoesRayIntersectEdge( &BestIntersect, Ray, B, A );
                            DoesRayIntersectEdge( &BestIntersect, Ray, D, C );
                        }
                    }
                    
                    { // Missile Launchers
                        for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                            MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                            
                            if( MissileLauncher->Health > 0 ) {
                                flo32 Radius = MissileLauncher->Radius - MINER_LASER_WIDTH * 0.5f;
                                circ  Bound  = Circ( MissileLauncher->Position, Radius );
                                DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                            }
                        }
                    }
                    
                    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                        SecurityDrone->Laser_Dest[ iLaser ] = BestIntersect.P;
                    }
                }
            }
        }
        
        { // Collide Security Drone with Observation Deck
            if( ( ObservationDeck->IsPresent ) && ( SecurityDrone->Stage == SecurityDroneStage_Stunned ) ) {
                flo32 Radius   = ObservationDeck->Radius + SecurityDrone->Radius - 0.5f;
                flo32 RadiusSq = Radius * Radius;
                flo32 DistSq   = GetLengthSq( SecurityDrone->Position - ObservationDeck->Position );
                if( DistSq <= RadiusSq ) {
                    int32 Damage = 1;
                    if( ObservationDeck->Health > 0 ) {
                        flo32 Speed  = GetLength( SecurityDrone->Velocity );
                        if( Speed >= SECURITY_DRONE_HI_DAMAGE_SPEED_THRESHOLD ) {
                            Damage = 2;
                        }
                        ObservationDeck->Health = MaxValue( ObservationDeck->Health - Damage, 0 );
                    } else {
                        ObservationDeck->Health = -1;
                        
                        AddEntityToKilledCountForLevel( AppState, EntityType_ObservationDeck, 1 );
                        AddObservationDeckWindowSmashParticles( AppState, ObservationDeck->Position );
                    }
                    
                    vec2 N = GetNormal( SecurityDrone->Position - ObservationDeck->Position );
                    
                    SecurityDrone->Position = ObservationDeck->Position + N * Radius;
                    SecurityDrone->Velocity -= dot( SecurityDrone->Velocity, N ) * N;
                    SecurityDrone->Velocity *= 0.25f;
                    SecurityDrone->Velocity += N * 4.0f;
                    
                    SecurityDrone->ZeroPoint_IsActive = false;
                    Player->ZeroPoint_IsActive        = false;
                    Player->ZeroPoint_IsAsteroid      = false;
                    Player->ZeroPoint_IsSecurityDrone = false;
                    
                    flo32 tCameraShake = Clamp01( GetLength( SecurityDrone->Velocity ) / SECURITY_DRONE_HI_DAMAGE_SPEED_THRESHOLD );
                    
                    flo32 Offset = 4.0f * tCameraShake;
                    InitCameraShake( &AppState->CameraShake, 0.0f, 0.0f, 1.0f, 60.0f, Vec2( Offset, Offset ) );
                    
                    if( ObservationDeck->Health >= 0 ) {
                        SecurityDrone->Stage  = SecurityDroneStage_Move;
                        SecurityDrone->Timer  = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME + SECURITY_DRONE_COLLIDE_RECOVER_TARGET_TIME );
                        SecurityDrone->Fire_Timer      = -( SECURITY_DRONE_COLLIDE_RECOVER_TARGET_TIME );
                        SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                        SecurityDrone->Health          = SECURITY_DRONE_HIT_OBSERVATION_DECK_RECOVER_HEALTH;
                        SecurityDrone->Health += ( OBSERVATION_DECK_HEALTH - ObservationDeck->Health ) * 10;
                        SecurityDrone->Recover_Timer = SECURITY_DRONE_RECOVER_LIGHT_TARGET_TIME;
                        
                        SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                        ObservationDeck->Shield_MaxDist += ( flo32 )Damage;
                        
                        BoardRoomS->Spawn_HitObservationDeck = true;
                        
                        BoardRoomS->Spawn_Timer     = 0.0f;
                        BoardRoomS->Spawn_nEntities = 0;
                        BoardRoomS->nSpawnLayout    = 0;
                        KillAllEnemies( AppState );
                    }
                    
                    int32 nSegments = ObservationDeck->WindowCrack_MaxSegments / 10;
                    if( Damage >= 2 ) {
                        nSegments += ObservationDeck->WindowCrack_MaxSegments / 10;
                    }
                    if( ObservationDeck->WindowCrack_nSegments == 0 ) {
                        ObservationDeck->WindowCrack_nSegments = RandomS32InRange( 12, 16 );
                    } else {
                        ObservationDeck->WindowCrack_nSegments = MinValue( ObservationDeck->WindowCrack_nSegments + nSegments, ObservationDeck->WindowCrack_MaxSegments );
                    }
                    
                    if( ObservationDeck->Health == 0 ) {
                        ObservationDeck->WindowCrack_nSegments = ObservationDeck->WindowCrack_MaxSegments;
                    }
                }
            }
        }
        
        { // Collide Security Drone with Observation Deck Barrier
            if( ( ObservationDeck->IsPresent ) && ( ObservationDeck->Barrier_IsActive ) ) {
                // NOTE: Security Drone should never collide with the inner barrier
                
                flo32 Radius_Inner = ObservationDeck->Radius + ObservationDeck->Barrier_Dist;
                flo32 Radius_Outer = Radius_Inner + OBSERVATION_DECK_BARRIER_WIDTH;
                
                flo32 Radius   = Radius_Outer + SecurityDrone->Radius;
                flo32 RadiusSq = Radius * Radius;
                if( IsInCircle( SecurityDrone->Position, Circ( ObservationDeck->Position, Radius ) ) ) {
                    int32 nBarrier = OBSERVATION_DECK_BARRIER_COUNT;
                    
                    boo32 DoCollision   = false;
                    vec2  DoCollision_P = {};
                    vec2  DoCollision_N = {};
                    
                    for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                        flo32 Radians    = ObservationDeck->Barrier_Radians + ( TAU * 0.25f * ( flo32 )iBarrier ) - OBSERVATION_DECK_BARRIER_RADIANS * 0.5f;
                        flo32 MinRadians_Inner = Radians;
                        flo32 MaxRadians_Inner = MinRadians_Inner + OBSERVATION_DECK_BARRIER_RADIANS;
                        
                        flo32 BarrierRadiansPerSegment = ( OBSERVATION_DECK_BARRIER_RADIANS / ( flo32 )OBSERVATION_DECK_BARRIER_SEGMENT_COUNT );
                        flo32 MinRadians_Outer = MinRadians_Inner + ( BarrierRadiansPerSegment * 0.5f );
                        flo32 MaxRadians_Outer = MaxRadians_Inner - ( BarrierRadiansPerSegment * 0.5f );
                        
                        {
                            flo32 InRadians = ToRadians( SecurityDrone->Position - ObservationDeck->Position );
                            if( InRadians < MinRadians_Outer ) {
                                InRadians += TAU;
                            }
                            
                            if( ( InRadians >= MinRadians_Outer ) && ( InRadians <= MaxRadians_Outer ) ) {
                                DoCollision   = true;
                                DoCollision_N = GetNormal( SecurityDrone->Position - ObservationDeck->Position );
                                DoCollision_P = ObservationDeck->Position + DoCollision_N * Radius;
                            }
                        }
                        
                        if( !DoCollision ) {
                            vec2 P   = ObservationDeck->Position + ToDirection2D( MinRadians_Outer ) * Radius_Outer;
                            vec2 Q   = ObservationDeck->Position + ToDirection2D( MinRadians_Inner ) * Radius_Inner;
                            
                            if( IsInCapsule( SecurityDrone->Position, Capsule2( P, Q, SecurityDrone->Radius ) ) ) {
                                DoCollision   = true;
                                vec2 ClosestP = ClosestPointToEdge( SecurityDrone->Position, P, Q );
                                DoCollision_N = GetNormal( SecurityDrone->Position - ClosestP );
                                DoCollision_P = ClosestP + DoCollision_N * SecurityDrone->Radius;
                            }
                        }
                        
                        if( !DoCollision ) {
                            vec2 P   = ObservationDeck->Position + ToDirection2D( MaxRadians_Inner ) * Radius_Inner;
                            vec2 Q   = ObservationDeck->Position + ToDirection2D( MaxRadians_Outer ) * Radius_Outer;
                            
                            if( IsInCapsule( SecurityDrone->Position, Capsule2( P, Q, SecurityDrone->Radius ) ) ) {
                                DoCollision   = true;
                                vec2 ClosestP = ClosestPointToEdge( SecurityDrone->Position, P, Q );
                                DoCollision_N = GetNormal( SecurityDrone->Position - ClosestP );
                                DoCollision_P = ClosestP + DoCollision_N * SecurityDrone->Radius;
                            }
                        }
                    }
                    
                    if( DoCollision ) {
                        vec2 N = DoCollision_N;
                        
                        SecurityDrone->Position = DoCollision_P;
                        SecurityDrone->Velocity -= dot( SecurityDrone->Velocity, N ) * N;
                        SecurityDrone->Velocity *= 0.25f;
                        SecurityDrone->Velocity += N * 4.0f;
                        
                        SecurityDrone->ZeroPoint_IsActive = false;
                        Player->ZeroPoint_IsActive        = false;
                        Player->ZeroPoint_IsAsteroid      = false;
                        Player->ZeroPoint_IsSecurityDrone = false;
                    }
                }
            }
        }
        
        { // Collide Security Drone with Missile Launchers
            if( ( ObservationDeck->IsPresent ) && ( SecurityDrone->Stage == SecurityDroneStage_Stunned ) ) {
                for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                    MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                    
                    if( MissileLauncher->Health > 0 ) {
                        flo32 Radius   = MissileLauncher->Radius + SecurityDrone->Radius - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( MissileLauncher->Position - SecurityDrone->Position );
                        if( DistSq <= RadiusSq ) {
                            int32 Damage = 25;
                            flo32 Speed  = GetLength( SecurityDrone->Velocity );
                            if( Speed >= SECURITY_DRONE_HI_DAMAGE_SPEED_THRESHOLD ) {
                                Damage = 50;
                            }
                            MissileLauncher->Health = MaxValue( MissileLauncher->Health - Damage, 0 );
                            
                            vec2 N = GetNormal( SecurityDrone->Position - MissileLauncher->Position );
                            
                            SecurityDrone->Position = MissileLauncher->Position + N * Radius;
                            SecurityDrone->Velocity -= dot( SecurityDrone->Velocity, N ) * N;
                            SecurityDrone->Velocity *= 0.25f;
                            SecurityDrone->Velocity += N * 4.0f;
                            
                            SecurityDrone->ZeroPoint_IsActive = false;
                            Player->ZeroPoint_IsActive        = false;
                            Player->ZeroPoint_IsAsteroid      = false;
                            Player->ZeroPoint_IsSecurityDrone = false;
                            
                            SecurityDrone->Stage  = SecurityDroneStage_Move;
                            SecurityDrone->Timer  = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME + SECURITY_DRONE_COLLIDE_RECOVER_TARGET_TIME );
                            SecurityDrone->Fire_Timer      = -( SECURITY_DRONE_COLLIDE_RECOVER_TARGET_TIME );
                            SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                            SecurityDrone->Health          = SECURITY_DRONE_HIT_MISSILE_LAUNCHER_RECOVER_HEALTH;
                            SecurityDrone->Recover_Timer   = SECURITY_DRONE_RECOVER_LIGHT_TARGET_TIME;
                            
                            SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                            
                            BoardRoomS->Spawn_HitMissileLauncher = true;
                        }
                    }
                }
            }
        }
        
        { // Update Observation Deck Shield
            if( ObservationDeck->Health > 0 ) {
                flo32 dShield = ( 1.0f / 2.0f ) * dT;
                ObservationDeck->Shield_Dist = MinValue( ObservationDeck->Shield_Dist + dShield, ObservationDeck->Shield_MaxDist );
            } else {
                ObservationDeck->Shield_IsActive = false;
            }
            
            for( int32 iShieldHit = 0; iShieldHit < SATELLITE_SHIELD_HIT_MAX_COUNT; iShieldHit++ ) {
                OBSERVATION_DECK_SHIELD_HIT * ShieldHit = ObservationDeck->ShieldHit + iShieldHit;
                ShieldHit->Timer = MaxValue( ShieldHit->Timer - dT, 0.0f );
            }
            
            if( !ObservationDeck->Shield_IsActive ) {
                ObservationDeck->ShieldDown_Timer = MinValue( ObservationDeck->ShieldDown_Timer + dT, SATELLITE_SHIELD_DOWN_TARGET_TIME );
            }
        }
        
        { // Update Missile Launcher
            if( ( ObservationDeck->IsPresent ) && ( ObservationDeck->Barrier_Dist >= ( OBSERVATION_DECK_BARRIER_DIST - 0.5f ) ) ) {
                for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                    MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                    
                    MissileLauncher->Damage_Timer  = MaxValue( MissileLauncher->Damage_Timer  - dT, 0.0f );
                    
                    if( MissileLauncher->Health > 0 ) {
                        { // Rotate Toward the Player
                            flo32 dRadians = TAU / 16.0f;
                            
                            flo32 Dest = ToRadians( Player->Position - MissileLauncher->Position );
                            flo32 At   = MissileLauncher->Radians;
                            if( Dest != At ) {
                                flo32 Diff = GetSmallestDiffInRadians( At, Dest );
                                
                                flo32 dRadiansHi = ( dRadians * dT );
                                flo32 dRadiansLo = -dRadiansHi;
                                if( ( Diff < dRadiansLo ) || ( Diff > dRadiansHi ) ) {
                                    At += Clamp( dRadiansLo, Diff, dRadiansHi );
                                } else {
                                    At  = Dest;
                                }
                                
                                MissileLauncher->Radians = WrapRadians( At );
                            }
                        }
                        
                        {
                            MissileLauncher->Launch_Timer += dT;
                            if( ObservationDeck->nMissileLaunchersDestroyed > 0 ) {
                                MissileLauncher->Launch_Timer += dT;
                            }
                            
                            flo32 TargetTime = OBSERVATION_DECK_MISSILE_LAUNCHER_TARGET_TIME;
                            if( MissileLauncher->Launch_Timer >= TargetTime ) {
                                MissileLauncher->Launch_Timer -= TargetTime;
                                
                                vec2  Dir     = ToDirection2D( MissileLauncher->Radians );
                                vec2  Pos     = MissileLauncher->Position + Dir * ( MISSILE_LAUNCHER_RADIUS * 1.0f );
                                flo32 Speed   = MISSILE_SPEED * 0.5f;
                                
                                AddMissile( AppState, MissileC( -1, Pos, Dir * Speed ) );
                            }
                        }
                    }
                }
            }
        }
        
        if( ObservationDeck->IsPresent ) { // Collide Missiles with Observation Deck
            MISSILE_STATE * MissileS = &AppState->MissileS;
            
            for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
                MISSILE * Missile = MissileS->Missile + iMissile;
                
                if( Missile->Health > 0 ) {
                    if( IsInCircle( Missile->Position, Circ( ObservationDeck->Position, ObservationDeck->Radius ) ) ) {
                        Missile->Health = 0;
                    }
                }
            }
        }
        
        { // Update Gunner Spawn
            if( BoardRoomS->Spawn_nGunners > 0 ) {
                BoardRoomS->Spawn_Timer += dT;
                if( BoardRoomS->Spawn_Timer >= BoardRoomS->Spawn_TargetTime ) {
                    int32 nGunners             = BoardRoomS->Spawn_nGunners;
                    int32 nGunnersPerSide[ 2 ] = {};
                    flo32 X[ 2 ]               = {};
                    
                    if( ( nGunners % 2 ) == 0 ) {
                        nGunnersPerSide[ 0 ] = nGunners / 2;
                        nGunnersPerSide[ 1 ] = nGunners / 2;
                    } else {
                        nGunnersPerSide[ 0 ] = RandomS32InRange( ( nGunners / 2 ), ( nGunners / 2 ) + 1 );
                        nGunnersPerSide[ 1 ] = nGunners - nGunnersPerSide[ 0 ];
                    }
                    
                    flo32 Top = ObservationDeck->Position.y - ObservationDeck->Radius;
                    if( ObservationDeck->Shield_IsActive ) {
                        Top -= ObservationDeck->Shield_MaxDist;
                    }
                    
                    rect Bound   = AppState->World_Bound;
                    Bound.Top    = Top;
                    Bound        = AddRadius( Bound, -2.0f );
                    Bound.Left  += 4.0f;
                    Bound.Right -= 4.0f;
                    X[ 0 ]       = Bound.Left;
                    X[ 1 ]       = Bound.Right;
                    
                    int32 nSide = 2;
                    for( int32 iSide = 0; iSide < nSide; iSide++ ) {
                        flo32 Offset = GetHeight( Bound ) / ( ( flo32 )( nGunnersPerSide[ iSide ] + 1 ) );
                        flo32 Y      = Bound.Bottom;
                        
                        for( int32 iGunner = 0; iGunner < nGunnersPerSide[ iSide ]; iGunner++ ) {
                            Y += Offset;
                            
                            vec2 P = Vec2( X[ iSide ], Y );
                            
                            AddEntitySpawn( AppState, GenValIdentity( -1, EntityType_Gunner, P ) );
                        }
                    }
                    
                    BoardRoomS->Spawn_nGunners = 0;
                }
            }
        }
        
        { // Update Board Members
            if( ( ObservationDeck->IsPresent ) && ( ObservationDeck->Health < 0 ) ) {
                vec2 bPos = ObservationDeck->Position + Vec2( 0.0f, -ObservationDeck->Radius );
                
                for( int32 iBoardMember = 0; iBoardMember < BoardRoomS->nBoardMember; iBoardMember++ ) {
                    BOARD_MEMBER * BoardMember = BoardRoomS->BoardMember + iBoardMember;
                    
                    BoardMember->Timer += dT;
                    
                    vec2  Dir      = GetNormal( bPos - BoardMember->Position );
                    flo32 Speed    = 4.0f;
                    flo32 Friction = 2.0f;
                    
                    if( BoardMember->Timer >= 0.0f ) {
                        Dir      = BoardMember->Direction;
                        Speed    = BoardMember->Speed;
                        Friction = 2.0f;
                        
                        if( BoardMember->DoFloat ) {
                            BoardMember->Radians = WrapRadians( BoardMember->Radians + BoardMember->dRadians * dT );
                        } else {
                            if( IsInCircle( BoardMember->Position, Circ( ObservationDeck->Position, ObservationDeck->Radius - 0.25f ) ) ) {
                                BoardMember->LimbRadians[ 0 ] = RandomF32() * PI * 0.75f;
                                BoardMember->LimbRadians[ 1 ] = RandomF32() * PI * 0.75f;
                                BoardMember->LimbRadians[ 2 ] = RandomF32() * PI * 0.25f;
                                BoardMember->LimbRadians[ 3 ] = RandomF32() * PI * 0.25f;
                                
                                Dir   = GetNormal( bPos - BoardMember->Position );
                                Speed = 20.0f;
                            } else {
                                BoardMember->DoFloat = true;
                            }
                        }
                    }
                    
                    vec2 Accel = ( Dir * Speed - BoardMember->Velocity ) * Friction;
                    vec2 dPos = Accel * ( 0.5f * dT * dT ) + BoardMember->Velocity * dT;
                    
                    BoardMember->Position += dPos;
                    BoardMember->Velocity += Accel * dT;
                }
            }
        }
        
        { // Collide Entities with Security Drone
            {
                CHASER_STATE * ChaserS = &AppState->ChaserS;
                for( int32 iChaser = 0; iChaser < ChaserS->nChaser; iChaser++ ) {
                    CHASER * Chaser = ChaserS->Chaser + iChaser;
                    
                    flo32 Radius   = Chaser->Radius + SecurityDrone->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Chaser->Position - SecurityDrone->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        Chaser->Health = 0;
                    }
                }
            }
            
            {
                DRIFTER_STATE * DrifterS = &AppState->DrifterS;
                for( int32 iDrifter = 0; iDrifter < DrifterS->nDrifter; iDrifter++ ) {
                    DRIFTER * Drifter = DrifterS->Drifter + iDrifter;
                    
                    flo32 Radius   = Drifter->Radius + SecurityDrone->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Drifter->Position - SecurityDrone->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        Drifter->Health = 0;
                    }
                }
            }
            
            {
                LOOPER_STATE * LooperS = &AppState->LooperS;
                for( int32 iLooper = 0; iLooper < LooperS->nLooper; iLooper++ ) {
                    LOOPER * Looper = LooperS->Looper + iLooper;
                    
                    flo32 bRadians = ( 1.0f - Clamp01( Looper->Loop_Timer / LOOPER_LOOP_CYCLE_TARGET_TIME ) ) * TAU - PI;
                    vec2  bPos     = Looper->Position + ToDirection2D( bRadians ) * Looper->Loop_Radius;
                    rect  Looper_Bound = RectCR( bPos, LOOPER_COLLISION_RADIUS );
                    
                    flo32 Dist     = DistPointToRect( SecurityDrone->Position, Looper_Bound );
                    flo32 Radius   = SecurityDrone->Radius;
                    
                    if( Dist <= Radius ) {
                        Looper->Health = 0;
                    }
                }
            }
            
            {
                GUNNER_STATE * GunnerS = &AppState->GunnerS;
                for( int32 iGunner = 0; iGunner < GunnerS->nGunner; iGunner++ ) {
                    GUNNER * Gunner = GunnerS->Gunner + iGunner;
                    
                    flo32 Radius   = GUNNER_RADIUS + SecurityDrone->Radius - 0.25f;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Gunner->Position - SecurityDrone->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        flo32 Speed  = GetLength( SecurityDrone->Velocity );
                        flo32 t      = Clamp01( Speed / SECURITY_DRONE_HI_DAMAGE_SPEED_THRESHOLD );
                        int32 Damage = ( int32 )lerp( 1.0f, t, 5.0f );
                        Gunner->Health = MaxValue( Gunner->Health - Damage, 0 );
                        
                        vec2 Dir = GetNormal( Gunner->Position - SecurityDrone->Position );
                        
                        if( !SecurityDrone->ZeroPoint_IsActive ) {
                            SecurityDrone->Velocity -= Dir * lerp( 0.25f, t, 0.5f );
                        }
                        
                        Gunner->Velocity = Dir * lerp( 4.0f, t, 12.0f );
                        
                        Gunner->Move_DoMove = false;
                        Gunner->Move_Timer  = -( 30.0f / 60.0f );
                        Gunner->Fire_Timer  = -( 30.0f / 60.0f );
                        Gunner->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        
                        Gunner->Position = SecurityDrone->Position + Dir * ( Radius + 0.01f );
                    }
                }
            }
            
            {
                ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
                for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
                    ENEMY_BULLET * EnemyBullet = EnemyBulletS->EnemyBullet + iEnemyBullet;
                    
                    if( EnemyBullet->Health > 0 ) {
                        flo32 Radius   = ENEMY_BULLET_RADIUS + SecurityDrone->Radius - 0.35f;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( EnemyBullet->Position - SecurityDrone->Position );
                        
                        if( DistSq <= RadiusSq ) {
                            EnemyBullet->Health         = 0;
                            EnemyBullet->DoHitPointDrop = true;
                        }
                    }
                }
            }
            
#if 0
            {
                BOLT_STATE * BoltS = &AppState->BoltS;
                for( int32 iBolt = 0; iBolt < BoltS->nBolt; iBolt++ ) {
                    BOLT * Bolt = BoltS->Bolt + iBolt;
                    
                    flo32 Radius   = Bolt->Radius + SecurityDrone->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Bolt->Position - SecurityDrone->Position );
                    
                    if( DistSq <= RadiusSq ) {
                        SecurityDrone->Health = MaxValue( SecurityDrone->Health - 1, 0 );
                        Bolt->Health          = 0;
                    }
                }
            }
#endif
            
            {
                MISSILE_STATE * MissileS = &AppState->MissileS;
                for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
                    MISSILE * Missile = MissileS->Missile + iMissile;
                    
                    if( IsInCircle( Missile->Position, Circ( SecurityDrone->Position, SecurityDrone->Radius ) ) ) {
                        Missile->Health       = 0;
                        SecurityDrone->Health = MaxValue( SecurityDrone->Health - 4, 0 );
                        
                        if( SecurityDrone->Stage != SecurityDroneStage_Stunned ) {
                            SecurityDrone->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        }
                        
                        vec2 Dir  = GetNormal( Missile->Velocity );
                        vec2 Norm = GetNormal( Missile->Position - SecurityDrone->Position );
                        vec2 Pos  = SecurityDrone->Position + Norm * SecurityDrone->Radius;
                        
                        AddBulletHitParticles( AppState, Pos, Dir, Norm, SECURITY_DRONE_BASE_COLOR );
                        AddBulletHitParticles( AppState, Pos, Dir, Norm, SECURITY_DRONE_BASE_COLOR );
                        AddBulletHitParticles( AppState, Pos, Dir, Norm, COLOR_WHITE );
                    }
                }
            }
        }
        
        if( ObservationDeck->IsPresent ) { // Update Missile Launcher Position
            for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                
                if( MissileLauncher->Health > 0 ) {
                    flo32 Radians = ObservationDeck->MissileLauncher_Radians[ iMissileLauncher ];
                    vec2  P       = ObservationDeck->MissileLauncher_Pos    [ iMissileLauncher ][ 0 ];
                    vec2  Q       = ObservationDeck->MissileLauncher_Pos    [ iMissileLauncher ][ 1 ];
                    vec2  R       = P + ( Q - P ) * 2.25f;
                    
                    flo32 tDist    = Clamp01( ObservationDeck->Barrier_Dist / OBSERVATION_DECK_BARRIER_DIST );
                    flo32 tRadians = 1.0f - ( cosf( ( ObservationDeck->Barrier_Radians - Radians ) * 4.0f ) * 0.5f + 0.5f );
                    
                    vec2 Dest = lerp( P, tRadians  * tRadians, Q );
                    MissileLauncher->Position = lerp( R, tDist, Dest );
                }
            }
        }
        
        { // Update Entity Spawn if Security Drone is Stunned
            if( ( BoardRoomS->Spawn_nGunners <= 0 ) && ( BoardRoomS->Spawn_nEntities > 0 ) ) {
                BoardRoomS->Spawn_Timer += dT;
                
                flo32 TargetTime = 3.0f;
                if( BoardRoomS->Spawn_Timer >= TargetTime ) {
                    BoardRoomS->Spawn_Timer     -= TargetTime;
                    BoardRoomS->Spawn_nEntities -= 1;
                    
                    flo32 Top = ObservationDeck->Position.y - ObservationDeck->Radius;
                    if( ObservationDeck->Shield_IsActive ) {
                        Top -= ObservationDeck->Shield_MaxDist;
                    }
                    
                    rect Bound = AppState->World_Bound;
                    Bound.Top = Top;
                    Bound     = AddDim( Bound, Vec2( -8.0f, -6.0f ) );
                    
                    ENTITY_TYPE Spawn_Type = {};
                    int32       Select     = RandomS32InRange( 0, 2 );
                    if( Select == 0 ) {
                        Spawn_Type = EntityType_Chaser;
                    } else if( Select == 1 ) {
                        Spawn_Type = EntityType_Drifter;
                    } else if( Select == 2 ) {
                        Spawn_Type = EntityType_Looper;
                    }
                    
                    if( BoardRoomS->iSpawnLayout_Index >= SPAWN_LAYOUT_PATTERN_MAX_COUNT ) {
                        BoardRoomS->iSpawnLayout_Index = 0;
                        
                        int32 Temp[ SPAWN_LAYOUT_PATTERN_MAX_COUNT ] = {};
                        memcpy( Temp, BoardRoomS->SpawnLayout_Index, sizeof( int32 ) * SPAWN_LAYOUT_PATTERN_MAX_COUNT );
                        
                        int32 SrcCount = SPAWN_LAYOUT_PATTERN_MAX_COUNT - 1;
                        
                        for( int32 DestIndex = 0; DestIndex < SPAWN_LAYOUT_PATTERN_MAX_COUNT; DestIndex++ ) {
                            int32 SrcIndex = RandomS32InRange( 0, SrcCount );
                            
                            BoardRoomS->SpawnLayout_Index[ DestIndex ] = Temp[ SrcIndex ];
                            _swap( int32, Temp[ SrcIndex ], Temp[ SrcCount ] );
                            
                            SrcCount -= 1;
                        }
                    }
                    
                    Select = BoardRoomS->SpawnLayout_Index[ BoardRoomS->iSpawnLayout_Index ];
                    BoardRoomS->iSpawnLayout_Index += 1;
                    
                    //BoardRoomS->Spawn_nEntities = 10;
                    //Select = ( BoardRoomS->Spawn_Counter % nLayout );
                    //BoardRoomS->Spawn_Counter += 1;
                    
                    SPAWN_LAYOUT SpawnLayout[ 2 ] = {};
                    int32       nSpawn            = 0;
                    
                    // Type 0 : Line
                    // Type 1 : Circle  ( EndPos = { xRadius, yRadius )
                    // Type 2 : Random  ( RectMM( StartPos, EndPos ) = Spawn Bound )
                    
                    vec2 BL = GetT( Bound, Vec2( 0.0f, 0.0f ) );
                    vec2 TL = GetT( Bound, Vec2( 0.0f, 1.0f ) );
                    vec2 BR = GetT( Bound, Vec2( 1.0f, 0.0f ) );
                    vec2 TR = GetT( Bound, Vec2( 1.0f, 1.0f ) );
                    
                    flo32 xPos[ 5 ] = { 0.05f, 0.275f, 0.5f, 0.725f, 0.95f };
                    flo32 yPos[ 3 ] = { 0.1f,  0.5f,   0.9f };
                    
                    if( ( Select >= 0 ) && ( Select <= 4 ) ) { // Vertical Line
                        vec2 StartPos = GetT( Bound, Vec2( xPos[ Select ], 0.0f ) );
                        vec2 EndPos   = GetT( Bound, Vec2( xPos[ Select ], 1.0f ) );
                        if( ( RandomS32InRange( 0, 1 ) == 0 ) ) {
                            _swap( vec2, StartPos, EndPos );
                        }
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 5, StartPos, EndPos );
                    } else if( ( Select >= 5 ) && ( Select <= 7 ) ) { // Horizontal Line
                        int32 Index = Select - 5;
                        
                        vec2 StartPos = GetT( Bound, Vec2( 0.0f, yPos[ Index ] ) );
                        vec2 EndPos   = GetT( Bound, Vec2( 1.0f, yPos[ Index ] ) );
                        if( ( RandomS32InRange( 0, 1 ) == 0 ) ) {
                            _swap( vec2, StartPos, EndPos );
                        }
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 10, StartPos, EndPos );
                    } else if( ( Select >= 8 ) && ( Select <= 10 ) ) { // Circle
                        int32 Index  = Select - 8 + 1;
                        vec2  Center = GetT( Bound, Vec2( xPos[ Index ], 0.5f ) );
                        vec2  Scale  = Vec2( 8.0f, 8.0f );
                        
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Circle, 8, Center, Scale );
                    } else if( ( Select >= 11 ) && ( Select <= 13 ) ) { // Backslash
                        int32 Index    = Select - 11 + 1;
                        vec2  bPos     = GetT( Bound, Vec2( xPos[ Index ], 0.5f ) );
                        vec2  StartPos = bPos + GetNormal( Vec2( -1.0f, 1.0f ) ) * 12.0f;
                        vec2  EndPos   = bPos - GetNormal( Vec2( -1.0f, 1.0f ) ) * 12.0f;
                        if( ( RandomS32InRange( 0, 1 ) == 0 ) ) {
                            _swap( vec2, StartPos, EndPos );
                        }
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 5, StartPos, EndPos );
                    } else if( ( Select >= 14 ) && ( Select <= 16 ) ) { // Forward Slash
                        int32 Index    = Select - 14 + 1;
                        vec2  bPos     = GetT( Bound, Vec2( xPos[ Index ], 0.5f ) );
                        vec2  StartPos = bPos + GetNormal( Vec2( -1.0f, -1.0f ) ) * 12.0f;
                        vec2  EndPos   = bPos - GetNormal( Vec2( -1.0f, -1.0f ) ) * 12.0f;
                        if( ( RandomS32InRange( 0, 1 ) == 0 ) ) {
                            _swap( vec2, StartPos, EndPos );
                        }
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 5, StartPos, EndPos );
                    }
                    
                    
#if 0                    
                    if( Select == 0 ) { // Left Edge
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 4, BL, TL );
                    } else if( Select == 1 ) { // Right Edge
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 4, BR, TR );
                    } else if( Select == 2 ) { // Left and Right Edge
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 4, BL, TL );
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 4, BR, TR );
                    } else if( Select == 3 ) { // Bottom Edge
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 8, BL, BR );
                    } else if( Select == 4 ) { // Top Edge
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 8, TL, TR );
                    } else if( Select == 5 ) { // Bottom and Top Edge
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Line, 4, BR, TR );
                    } else if( Select == 6 ) { // Center Circle
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Circle, 8, GetT( Bound, Vec2( 0.5f, 0.5f ) ), Vec2( 8.0f, 8.0f ) );
                    } else if( Select == 7 ) { // Left Circle
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Circle, 8, GetT( Bound, Vec2( 0.3333f, 0.5f ) ), Vec2( 8.0f, 8.0f ) );
                    } else if( Select == 8 ) { // Right Circle
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Circle, 8, GetT( Bound, Vec2( 0.6667f, 0.5f ) ), Vec2( 8.0f, 8.0f ) );
                    } else if( Select == 9 ) { // Random
                        SpawnLayout[ nSpawn++ ] = SpawnLayoutC( Spawn_Type, SpawnLayoutPattern_Random, 8, BL, TR );
                    }
#endif
                    
                    BoardRoomS->SpawnLayout[ BoardRoomS->nSpawnLayout++ ] = SpawnLayout[ 0 ];
                }
            }
            
            if( BoardRoomS->nSpawnLayout > 0 ) {
                SPAWN_LAYOUT * SpawnLayout = BoardRoomS->SpawnLayout + 0;
                
                if( SpawnLayout->TargetTime > 0.0f ) {
                    SpawnLayout->Timer += dT;
                    if( SpawnLayout->Timer >= SpawnLayout->TargetTime ) {
                        SpawnLayout->Timer -= SpawnLayout->TargetTime;
                        
                        if( SpawnLayout->Pattern == SpawnLayoutPattern_Line ) {
                            flo32 t = Clamp01( ( flo32 )SpawnLayout->AtEntity / ( flo32 )( SpawnLayout->nEntity - 1 ) );
                            vec2  P = lerp( SpawnLayout->StartPos, t, SpawnLayout->EndPos );
                            
                            AddEntitySpawn( AppState, GenValIdentity( -1, SpawnLayout->EntityType, P ) );
                        } else if( SpawnLayout->Pattern == SpawnLayoutPattern_Circle ) {
                            flo32 t = Clamp01( ( flo32 )SpawnLayout->AtEntity / ( flo32 )SpawnLayout->nEntity );
                            flo32 Radians = t * -TAU;
                            vec2  P       = SpawnLayout->StartPos + ToDirection2D( Radians ) * SpawnLayout->Scale;
                            
                            AddEntitySpawn( AppState, GenValIdentity( -1, SpawnLayout->EntityType, P ) );
                        } else if( SpawnLayout->Pattern == SpawnLayoutPattern_Random ) {
                            rect R = RectMM( SpawnLayout->StartPos, SpawnLayout->EndPos );
                            vec2 P = GetRandomPoint( R );
                            
                            AddEntitySpawn( AppState, GenValIdentity( -1, SpawnLayout->EntityType, P ) );
                        } else {
                            InvalidCodePath;
                        }
                        
                        SpawnLayout->AtEntity += 1;
                    }
                } else {
                    if( SpawnLayout->Pattern == SpawnLayoutPattern_Line ) {
                        for( int32 iEntity = 0; iEntity < SpawnLayout->nEntity; iEntity++ ) {
                            flo32 t = Clamp01( ( flo32 )iEntity / ( flo32 )( SpawnLayout->nEntity - 1 ) );
                            vec2  P = lerp( SpawnLayout->StartPos, t, SpawnLayout->EndPos );
                            
                            AddEntitySpawn( AppState, GenValIdentity( -1, SpawnLayout->EntityType, P ) );
                        }
                    } else if( SpawnLayout->Pattern == SpawnLayoutPattern_Circle ) {
                        for( int32 iEntity = 0; iEntity < SpawnLayout->nEntity; iEntity++ ) {
                            flo32 t = Clamp01( ( flo32 )iEntity / ( flo32 )SpawnLayout->nEntity );
                            flo32 Radians = t * TAU;
                            vec2  P       = SpawnLayout->StartPos + ToDirection2D( Radians ) * SpawnLayout->Scale;
                            
                            AddEntitySpawn( AppState, GenValIdentity( -1, SpawnLayout->EntityType, P ) );
                        }
                    } else if( SpawnLayout->Pattern == SpawnLayoutPattern_Random ) {
                        for( int32 iEntity = 0; iEntity < SpawnLayout->nEntity; iEntity++ ) {
                            rect R = RectMM( SpawnLayout->StartPos, SpawnLayout->EndPos );
                            vec2 P = GetRandomPoint( R );
                            
                            AddEntitySpawn( AppState, GenValIdentity( -1, SpawnLayout->EntityType, P ) );
                        }
                    } else {
                        InvalidCodePath;
                    }
                    
                    SpawnLayout->AtEntity = SpawnLayout->nEntity;
                }
                
                if( SpawnLayout->AtEntity >= SpawnLayout->nEntity ) {
                    BoardRoomS->nSpawnLayout--;
                    for( int32 iSpawnLayout = 0; iSpawnLayout < BoardRoomS->nSpawnLayout; iSpawnLayout++ ) {
                        BoardRoomS->SpawnLayout[ iSpawnLayout ] = BoardRoomS->SpawnLayout[ iSpawnLayout + 1 ];
                    }
                }
            }
        }
        
        if( !Player->ZeroPoint_IsActive ) {
            SecurityDrone->ZeroPoint_IsActive = false;
        }
        if( Player->IsDead ) {
            SecurityDrone->ZeroPoint_ShowUI   = false;
        }
        
        { // Update Security Drone
            SecurityDrone->Recover_Timer = MaxValue( SecurityDrone->Recover_Timer - dT, 0.0f );
            SecurityDrone->Damage_Timer  = MaxValue( SecurityDrone->Damage_Timer  - dT, 0.0f );
            
            if(   ( SecurityDrone->Stage != SecurityDroneStage_Stunned )
               && ( SecurityDrone->Stage != SecurityDroneStage_Move    )
               && ( SecurityDrone->Stage != SecurityDroneStage_Spawn   ) ) {
                SecurityDrone->Light_Timer += ( dT * 2.0f );
            } else if(   ( SecurityDrone->Stage == SecurityDroneStage_Stunned )
                      && ( SecurityDrone->Velocity != Vec2( 0.0f, 0.0f ) )
                      && ( GetLength( SecurityDrone->Velocity ) >= SECURITY_DRONE_HI_DAMAGE_SPEED_THRESHOLD  ) ) {
                SecurityDrone->Light_Timer += ( dT * 2.0f );
            } else {
                SecurityDrone->Light_Timer += dT;
            }
            
            if( SecurityDrone->Light_Timer >= SECURITY_DRONE_LIGHT_TARGET_TIME ) {
                SecurityDrone->Light_Timer -= SECURITY_DRONE_LIGHT_TARGET_TIME;
            }
            
            if( ( SecurityDrone->Friction == 0.0f ) && ( SecurityDrone->Stage != SecurityDroneStage_Stunned ) ) {
                SecurityDrone->Friction = SECURITY_DRONE_FRICTION;
            }
            
            if( SecurityDrone->ZeroPoint_IsActive ) {
                SecurityDrone->ZeroPoint_Radians = WrapRadians( SecurityDrone->ZeroPoint_Radians - Player->Rotate_Speed * dT );
                
                vec2 Dir = ToDirection2D( SecurityDrone->ZeroPoint_Radians );
                
                SecurityDrone->Friction = 0.0f;
                SecurityDrone->Position = Player->Position + Dir * SecurityDrone->ZeroPoint_Dist;
                
                flo32 Speed = Player->Rotate_Speed * SecurityDrone->ZeroPoint_Dist;
                SecurityDrone->Velocity = -GetPerp( Dir ) * Speed;
                
                SecurityDrone->Timer      = 0.0f;
                SecurityDrone->Fire_Timer = 0.0f;
            } else {
                vec2  Dir   = {};
                flo32 Speed = 0.0f;
                
                flo32 Radians_Dir      = 0.0f;
                flo32 Radians_Speed    = 0.0f;
                flo32 Radians_Friction = 0.0f;
                
                if( ObservationDeck->Health >= 0 ) {
                    SecurityDrone->Timer += dT;
                } else {
                    SecurityDrone->Timer      = 0.0f;
                    SecurityDrone->Fire_Timer = 0.0f;
                    SecurityDrone->Stage      = SecurityDroneStage_Stunned;
                }
                
                flo32 tMulForLowHealth =  1.0f - Clamp01( ( flo32 )ObservationDeck->Health / ( flo32 )OBSERVATION_DECK_HEALTH );
                flo32 SpeedMulForLowHealth          = lerp( 1.0f, tMulForLowHealth, 2.0f );
                flo32 FireTargetTimeMulForLowHealth = lerp( 1.0f, tMulForLowHealth, 1.5f );
                
                DISPLAY_VALUE( flo32, SecurityDrone->MaxTimeSinceLastAttack );
                
                switch( SecurityDrone->Stage ) {
                    case SecurityDroneStage_Stunned: {
                        Radians_Friction = 0.5f;
                        
                        if( ObservationDeck->Health < 0 ) {
                            SecurityDrone->Friction = SECURITY_DRONE_FRICTION;
                        }
                    } break;
                    
                    case SecurityDroneStage_Spawn: {
                        if( !SecurityDrone->Dest_Trigger ) {
                            Dir   = GetNormal( SecurityDrone->Destination - SecurityDrone->Position );
                            Speed = SECURITY_DRONE_SPAWN_SPEED;
                        }
                        
                        Radians_Dir      = 1.0f;
                        Radians_Speed    = ( TAU / 12.0f );
                        Radians_Friction = 2.0f;
                    } break;
                    
                    case SecurityDroneStage_Move: {
                        SecurityDrone->TimeSinceLastAttack += dT;
                        SecurityDrone->MaxTimeSinceLastAttack = MaxValue( SecurityDrone->MaxTimeSinceLastAttack, SecurityDrone->TimeSinceLastAttack );
                        
                        if( ( ObservationDeck->IsPresent ) && ( ObservationDeck->Health >= 0 ) && ( Player->Health > 0 ) ) {
                            SecurityDrone->Fire_Timer += ( dT * FireTargetTimeMulForLowHealth );
                        }
                        
                        if( SecurityDrone->Timer >= 0.0f ) {
                            Dir   = GetNormal( SecurityDrone->Destination - SecurityDrone->Position );
                            Speed = SECURITY_DRONE_MOVE_SPEED * SpeedMulForLowHealth;
                            
                            if( ( ObservationDeck->IsPresent ) && ( Player->Health > 0 ) && ( SecurityDrone->Fire_Timer >= SecurityDrone->Fire_TargetTime ) ) {
                                SecurityDrone->Fire_Timer -= SecurityDrone->Fire_TargetTime;
                                
                                if( IsInRect( SecurityDrone->Position, AppState->World_Bound ) ) {
                                    int32 nEnemyBullet = 3;
                                    for( int32 iEnemyBullet = 0; iEnemyBullet < nEnemyBullet; iEnemyBullet++ ) {
                                        flo32 Radians = SecurityDrone->Radians + ( TAU / 3.0f ) * ( flo32 )iEnemyBullet;
                                        flo32 Dist    = SecurityDrone->Radius + ENEMY_BULLET_RADIUS - 0.25f;
                                        vec2  DirA    = ToDirection2D( Radians );
                                        vec2  Pos     = SecurityDrone->Position + DirA * Dist;
                                        
                                        AddEnemyBullet( AppState, EnemyBulletC( Pos, DirA ) );
                                    }
                                }
                            }
                        }
                        
                        Radians_Dir      = 1.0f;
                        Radians_Speed    = ( TAU / 12.0f );
                        Radians_Friction = 2.0f;
                    } break;
                    
                    case SecurityDroneStage_Spin_Bullets: {
                        SecurityDrone->Fire_Timer     += dT;
                        if( SecurityDrone->Fire_Timer >= SecurityDrone->Fire_TargetTime ) {
                            SecurityDrone->Fire_Timer  -= SecurityDrone->Fire_TargetTime;
                            SecurityDrone->Counter = MaxValue( SecurityDrone->Counter - 1, 0 );
                            
                            int32 nEnemyBullet = 3;
                            for( int32 iEnemyBullet = 0; iEnemyBullet < nEnemyBullet; iEnemyBullet++ ) {
                                flo32 Radians = SecurityDrone->Radians + ( TAU / 3.0f ) * ( flo32 )iEnemyBullet;
                                flo32 Dist    = SecurityDrone->Radius + ENEMY_BULLET_RADIUS - 0.25f;
                                vec2  DirA    = ToDirection2D( Radians );
                                vec2  Pos     = SecurityDrone->Position + DirA * Dist;
                                
                                AddEnemyBullet( AppState, EnemyBulletC( Pos, DirA ) );
                            }
                        }
                        
                        Radians_Dir      = -1.0f;
                        Radians_Speed    = ( TAU / 4.0f );
                        Radians_Friction = 2.0f;
                    } break;
                    
                    case SecurityDroneStage_Spin_Lasers: {
                        for( int32 iLaser = 0; iLaser < SecurityDrone->nLaser; iLaser++ ) {
                            SecurityDrone->Laser_IsActive[ iLaser ] = true;
                        }
                        
                        Radians_Dir      = -1.0f;
                        Radians_Speed    = ( TAU / 5.0f );
                        Radians_Friction = 2.0f;
                    } break;
                    
                    case SecurityDroneStage_Sweep_Bullets: {
                        SecurityDrone->Fire_Timer     += dT;
                        if( SecurityDrone->Fire_Timer >= SecurityDrone->Fire_TargetTime ) {
                            SecurityDrone->Fire_Timer -= SecurityDrone->Fire_TargetTime;
                            
                            int32 nEnemyBullet = 3;
                            for( int32 iEnemyBullet = 0; iEnemyBullet < nEnemyBullet; iEnemyBullet++ ) {
                                flo32 Radians = SecurityDrone->Radians + ( TAU / 3.0f ) * ( flo32 )iEnemyBullet;
                                flo32 Dist    = SecurityDrone->Radius + ENEMY_BULLET_RADIUS - 0.25f;
                                vec2  DirA    = ToDirection2D( Radians );
                                vec2  Pos     = SecurityDrone->Position + DirA * Dist;
                                
                                AddEnemyBullet( AppState, EnemyBulletC( Pos, DirA ) );
                            }
                        }
                        
                        if( SecurityDrone->Timer >= 0.0f ) {
                            Dir   = GetNormal( SecurityDrone->Destination - SecurityDrone->Position );
                            Speed = SECURITY_DRONE_SWEEP_SPEED * SpeedMulForLowHealth;
                        }
                        
                        Radians_Dir      = -1.0f;
                        Radians_Speed    = ( TAU / 4.0f );
                        Radians_Friction = 2.0f;
                    } break;
                    
                    case SecurityDroneStage_Sweep_Lasers: {
                        for( int32 iLaser = 0; iLaser < SecurityDrone->nLaser; iLaser++ ) {
                            SecurityDrone->Laser_IsActive[ iLaser ] = true;
                        }
                        
                        if( SecurityDrone->Timer >= 0.0f ) {
                            Dir   = GetNormal( SecurityDrone->Destination - SecurityDrone->Position );
                            Speed = SECURITY_DRONE_SWEEP_SPEED * SpeedMulForLowHealth;
                        }
                        
                        Radians_Dir      = -1.0f;
                        Radians_Speed    = ( TAU / 5.0f );
                        Radians_Friction = 2.0f;
                    } break;
                }
                
                vec2  Boost_Dir   = {};
                flo32 Boost_Speed = 0.0f;
                
                { // Update Boost
                    boo32 CanBoost = ( ( ObservationDeck->Health <= 3 )
                                      || ( ObservationDeck->nMissileLaunchersDestroyed >= 2 ) )
                        && ( SecurityDrone->Stage != SecurityDroneStage_Stunned )
                        && ( SecurityDrone->Stage != SecurityDroneStage_Spawn );
                    
                    if( ObservationDeck->Health < 0 ) {
                        CanBoost = true;
                    }
                    
                    flo32 Top = ObservationDeck->Position.y - ObservationDeck->Radius;
                    if( ObservationDeck->Shield_IsActive ) {
                        Top -= ObservationDeck->Shield_MaxDist;
                    } else if( ObservationDeck->Barrier_IsActive ) {
                        Top -= ( ObservationDeck->Barrier_Dist + OBSERVATION_DECK_BARRIER_WIDTH );
                    }
                    
                    rect Bound = AppState->World_Bound;
                    Bound.Top  = Top;
                    Bound      = AddRadius( Bound, -( SECURITY_DRONE_MOVE_MARGIN + SecurityDrone->Radius ) );
                    
                    if( CanBoost ) {
                        flo32 Warmup_TargetTime = SecurityDrone->Boost_TargetTime - SECURITY_DRONE_BOOST_WARMUP_TARGET_TIME;
                        flo32 PrevTimer         = SecurityDrone->Boost_Timer;
                        
                        SecurityDrone->Boost_Timer += dT;
                        
                        if( ( PrevTimer < Warmup_TargetTime ) && ( SecurityDrone->Boost_Timer >= Warmup_TargetTime ) ) {
                            vec2  Dir0    = RandomDirection2D();
                            flo32 Dist0   = 9.0f;
                            vec2  Dest0   = SecurityDrone->Position + Dir0 * Dist0;
                            
                            if( ( ( Dir0.x < 0.0f ) && ( Dest0.x < Bound.Left  ) ) ||
                               (  ( Dir0.x > 0.0f ) && ( Dest0.x > Bound.Right ) ) ) {
                                Dir0.x = -Dir0.x;
                            }
                            if( ( ( Dir0.y < 0.0f ) && ( Dest0.y < Bound.Bottom ) ) ||
                               (  ( Dir0.y > 0.0f ) && ( Dest0.y > Bound.Top    ) ) ) {
                                Dir0.y = -Dir0.y;
                            }
                            
                            SecurityDrone->Boost_Direction = Dir0;
                        }
                        
                        if( SecurityDrone->Boost_Timer >= SecurityDrone->Boost_TargetTime ) {
                            SecurityDrone->Boost_Timer -= SecurityDrone->Boost_TargetTime;
                            
                            if( ObservationDeck->Health <= 0 ) {
                                SecurityDrone->Boost_TargetTime = lerp( ( 40.0f / 60.0f ), RandomF32_X2(), ( 120.0f / 60.0f ) );
                            } else if( ObservationDeck->Health <= 2 ) {
                                SecurityDrone->Boost_TargetTime = lerp( ( 100.0f / 60.0f ), RandomF32_X2(), ( 180.0f / 60.0f ) );
                            } else if( ObservationDeck->Health <= 3 ) {
                                SecurityDrone->Boost_TargetTime = RandomF32InRange( 3.0f, 5.0f );
                            } else {
                                SecurityDrone->Boost_TargetTime = RandomF32InRange( 8.0f, 10.0f );
                            }
                            
                            vec2  Dir0    = SecurityDrone->Boost_Direction;
                            flo32 tSpeed0 = RandomF32();
                            flo32 Speed0  = lerp( 16.0f, tSpeed0 * tSpeed0, 24.0f );
                            
                            flo32 BoostMulForLowHealth = lerp( 1.0f, tMulForLowHealth, 0.75f );
                            Speed0 *= BoostMulForLowHealth;
                            
                            SecurityDrone->Velocity += Dir0 * Speed0;
                            
                            if( GetLength( SecurityDrone->Velocity ) >= 24.0f ) {
                                SecurityDrone->Velocity = GetNormal( SecurityDrone->Velocity ) * 24.0f;
                            }
                        }
                    } else {
                        SecurityDrone->Boost_Timer = 0.0f;
                    }
                }
                
                {
                    vec2 Accel = ( Dir * Speed + Boost_Dir * Boost_Speed - SecurityDrone->Velocity ) * SecurityDrone->Friction;
                    vec2 dPos  = Accel * ( 0.5f * dT * dT ) + SecurityDrone->Velocity * dT;
                    // TODO: Collision!
                    SecurityDrone->Position += dPos;
                    SecurityDrone->Velocity += Accel * dT;
                }
                
                {
                    flo32 Accel    = ( Radians_Dir * Radians_Speed - SecurityDrone->Radians_Velocity ) * Radians_Friction;
                    flo32 dRadians = Accel * ( 0.5f * dT * dT ) + SecurityDrone->Radians_Velocity * dT;
                    
                    DISPLAY_VALUE( flo32, SecurityDrone->Radians_Velocity );
                    
                    SecurityDrone->Radians           = WrapRadians( SecurityDrone->Radians + dRadians );
                    SecurityDrone->Radians_Velocity += Accel * dT;
                }
            }
        }
    }
}

internal void
DrawBoardMember( RENDER_PASS * Pass, BOARD_MEMBER BoardMember, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4 ColorA = BoardMember.Color[ 0 ];
    vec4 ColorB = BoardMember.Color[ 1 ];
    vec4 ColorC = BoardMember.Color[ 2 ];
    
    orect bBound = {};
    
    {
        vec2  Dim   = Vec2( 0.25f, 0.4f );
        orect Bound = ORectCD( BoardMember.Position, Dim, BoardMember.Radians );
        bBound = Bound;
    }
    
    {
        vec2  Dim      = Vec2( 0.1f, 0.4f );
        vec2  PosA     = GetT( bBound, Vec2( 0.0f, 0.0f ) ) + Vec2(  Dim.x, Dim.x ) * 0.5f;
        vec2  PosB     = GetT( bBound, Vec2( 1.0f, 0.0f ) ) + Vec2( -Dim.x, Dim.x ) * 0.5f;
        flo32 RadiansA = BoardMember.Radians - BoardMember.LimbRadians[ 2 ];
        flo32 RadiansB = BoardMember.Radians + BoardMember.LimbRadians[ 3 ];
        
        orect BoundA = ORectTD( PosA, Vec2( 0.5f, 1.0f ), Dim, RadiansA );
        orect BoundB = ORectTD( PosB, Vec2( 0.5f, 1.0f ), Dim, RadiansB );
        
        DrawORect( Pass, BoundA, ColorC );
        DrawORect( Pass, BoundB, ColorC );
    }
    
    {
        vec2  Dim      = Vec2( 0.1f, 0.3f );
        vec2  PosA     = GetT( bBound, Vec2( 0.0f, 1.0f ) ) + Vec2(  Dim.x, -Dim.x ) * 0.5f;
        vec2  PosB     = GetT( bBound, Vec2( 1.0f, 1.0f ) ) + Vec2( -Dim.x, -Dim.x ) * 0.5f;
        flo32 RadiansA = BoardMember.Radians + ( -PI / 6.0f ) - BoardMember.LimbRadians[ 0 ];
        flo32 RadiansB = BoardMember.Radians + (  PI / 6.0f ) + BoardMember.LimbRadians[ 1 ];
        
        orect BoundA = ORectTD( PosA, Vec2( 0.5f, 1.0f ), Dim, RadiansA );
        orect BoundB = ORectTD( PosB, Vec2( 0.5f, 1.0f ), Dim, RadiansB );
        
        DrawORect( Pass, BoundA, ColorB );
        DrawORect( Pass, BoundB, ColorB );
    }
    
    {
        vec2  Dim   = Vec2( 0.2f, 0.2f );
        vec2  Pos   = GetT( bBound, Vec2( 0.5f, 1.0f ) );
        orect Bound = ORectTD( Pos, Vec2( 0.5f, 0.0f ), Dim, BoardMember.Radians );
        
        DrawORect( Pass, Bound, ColorA );
    }
    
    DrawORect( Pass, bBound, ColorB );
}

internal void
DrawObservationDeckBarrier( RENDER_PASS * Pass, OBSERVATION_DECK_BARRIER ObservationDeckBarrier, flo32 Alpha = 1.0f, flo32 Scale = 1.0f, vec4 Flash_Color = {} ) {
    vec4  Barrier_ColorA = TowardBlack( OBSERVATION_DECK_BASE_COLOR, 0.2f );
    vec4  Barrier_ColorB = COLOR_WHITE;
    
    if( Alpha < 1.0f ) {
        Barrier_ColorA = TowardBlack( OBSERVATION_DECK_BASE_COLOR, 0.4f );
        Barrier_ColorB = COLOR_GRAY( 0.8f );
    }
    
    vec2  Sca           = Vec2( 1.0f, 1.0f ) * Vec2( 1.0f + ( Scale - 1.0f ) * 2.5f, Scale );
    
    //ObservationDeckBarrier.Position -= ToDirection2D( ObservationDeckBarrier.Radians ) * ( OBSERVATION_DECK_BARRIER_WIDTH * 0.5f * ( Sca.x - 1.0f ) );
    //ObservationDeckBarrier.Position += ToDirection2D( ObservationDeckBarrier.Radians ) * ( OBSERVATION_DECK_BARRIER_WIDTH * 0.25f * ( Sca.x - 1.0f ) );
    
    DrawModel( Pass, MeshID_ObservationDeck_Barrier, TextureID_WhiteTexture, ObservationDeckBarrier.Position, Sca, ObservationDeckBarrier.Radians, Barrier_ColorA * Alpha );
    DrawModel( Pass, MeshID_ObservationDeck_Barrier, TextureID_ObservationDeck_Barrier, ObservationDeckBarrier.Position, Sca, ObservationDeckBarrier.Radians, Barrier_ColorB * Alpha );
    DrawModel( Pass, MeshID_ObservationDeck_Barrier, TextureID_WhiteTexture, ObservationDeckBarrier.Position, Sca, ObservationDeckBarrier.Radians, Flash_Color * Alpha );
}

internal void
DrawObservationDeck( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, BOARD_ROOM_STATE * BoardRoomS, flo32 Alpha = 1.0f, flo32 Scale = 1.0f, vec4 Flash_Color = {} ) {
    OBSERVATION_DECK ObservationDeck = BoardRoomS->ObservationDeck;
    
    vec4 Body_Color       = TowardBlack(       OBSERVATION_DECK_BASE_COLOR, 0.35f );
    vec4 TurretBody_Color = TowardGray ( 0.1f, OBSERVATION_DECK_BASE_COLOR, 0.9f );
    vec4 Barrier_Color    = TowardBlack(       OBSERVATION_DECK_BASE_COLOR, 0.2f );
    
    flo32 RadiusA = ObservationDeck.Radius;
    flo32 RadiusB = RadiusA - 0.35f;
    
    { // SHIELD GLOW
        flo32 tAlpha = 1.0f - Clamp01( ObservationDeck.ShieldDown_Timer / SATELLITE_SHIELD_DOWN_TARGET_TIME );
        
        flo32 TargetTimeA = (  4.0f / 60.0f );
        flo32 TargetTimeB = (  8.0f / 60.0f );
        flo32 TargetTimeC = ( 12.0f / 60.0f );
        flo32 TargetTimeD = ( 16.0f / 60.0f );
        flo32 TargetTimeE = ( 22.0f / 60.0f );
        flo32 TargetTimeF = ( 28.0f / 60.0f );
        
        if( ( ObservationDeck.ShieldDown_Timer >= TargetTimeA ) && ( ObservationDeck.ShieldDown_Timer < TargetTimeB ) ) {
            flo32 t = GetT01( ObservationDeck.ShieldDown_Timer, TargetTimeA, TargetTimeB );
            if( t < 0.5f ) {
                t = 1.0f - t;
            }
            tAlpha = 0.35f * t;
        }
        if( ( ObservationDeck.ShieldDown_Timer >= TargetTimeC ) && ( ObservationDeck.ShieldDown_Timer < TargetTimeD ) ) {
            flo32 t = GetT01( ObservationDeck.ShieldDown_Timer, TargetTimeC, TargetTimeD );
            if( t < 0.5f ) {
                t = 1.0f - t;
            }
            tAlpha = 0.35f;
        }
        if( ( ObservationDeck.ShieldDown_Timer >= TargetTimeE ) && ( ObservationDeck.ShieldDown_Timer < TargetTimeF ) ) {
            flo32 t = GetT01( ObservationDeck.ShieldDown_Timer, TargetTimeE, TargetTimeF );
            if( t < 0.5f ) {
                t = 1.0f - t;
            }
            tAlpha = 0.35f;
        }
        
        if( tAlpha > 0.0f ) {
            //vec4 Shield_Color = Vec4( 0.0f, 0.05f, 0.05f, 0.05f );
            //DrawCircle( Pass, ObservationDeck.Position, ObservationDeck.Radius + ObservationDeck.Shield_Dist, Shield_Color );
            
            //DrawCircleOutline( Pass, ObservationDeck.Position, ObservationDeck.Radius + ObservationDeck.Shield_Dist, COLOR_RED );
            
            flo32 Radius = ObservationDeck.Radius + ObservationDeck.Shield_Dist + 3.5f;
            DrawRect( Pass, TextureID_ObservationDeck_Shield_RingAndGlow, RectTD( ObservationDeck.Position, Vec2( 0.5f, 1.0f ), Vec2( Radius * 2.0f, Radius ) ), COLOR_WHITE * tAlpha );
        }
    }
    
    { // MISSILE LAUNCHER MOUNT
        for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
            vec2 P   = ObservationDeck.MissileLauncher_Pos[ iMissileLauncher ][ 0 ];
            vec2 Q   = ObservationDeck.MissileLauncher_Pos[ iMissileLauncher ][ 1 ];
            vec2 R   = P + ( Q - P ) * 2.25f;
            vec2 Dim = Vec2( GetLength( R - P ), 0.75f );
            
            flo32 tDist = Clamp01( ( ObservationDeck.Barrier_Dist + 6.0f ) / OBSERVATION_DECK_BARRIER_DIST );
            vec2  bPos  = lerp( R, tDist, P );
            orect Bound = ORectTD( bPos, Vec2( 0.0f, 0.5f ), Dim, ToRadians( Q - P ) );
            DrawORect( Pass, Bound, COLOR_GRAY( 0.05f ) );
        }
    }
    
    if( ObservationDeck.Barrier_IsActive ) {
        int32 nBarrier = OBSERVATION_DECK_BARRIER_COUNT;
        
        for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
            flo32 t       = ( flo32 )iBarrier / ( flo32 )nBarrier;
            flo32 Radians = ObservationDeck.Barrier_Radians + TAU * t;
            flo32 Dist    = ObservationDeck.Radius + ObservationDeck.Barrier_Dist;
            vec2  Pos     = ObservationDeck.Position + ToDirection2D( Radians ) * Dist;
            
            OBSERVATION_DECK_BARRIER ObservationDeckBarrier = {};
            ObservationDeckBarrier.Position = Pos;
            ObservationDeckBarrier.Radians  = Radians;
            DrawObservationDeckBarrier( Pass, ObservationDeckBarrier, 1.0f, 1.0f, Flash_Color );
        }
    }
    
    { // MISSILE LAUNCHER
        for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
            
            MISSILE_LAUNCHER MissileLauncher = ObservationDeck.MissileLauncher[ iMissileLauncher ];
            if( MissileLauncher.Health > 0 ) {
                DrawMissileLauncher( Pass, AppState, Camera, MissileLauncher );
            }
        }
    }
    
    { // BODY
        { // TURRETS
            if( ObservationDeck.TurretAnim_Timer > 0.0f ) {
                flo32 Margin  = 1.0f;
                flo32 tTurret = Clamp01( ObservationDeck.TurretAnim_Timer / OBSERVATION_DECK_TURRET_ANIM_TARGET_TIME );
                flo32 Radius        = ObservationDeck.Radius + Margin * tTurret;
                flo32 Turret_Radius = Radius - ( Margin * 0.5f );
                DrawRect( Pass, TextureID_ObservationDeck, RectTD( ObservationDeck.Position, Vec2( 0.5f, 1.0f ), Vec2( Radius * 2.0f, Radius ) * Scale ), TurretBody_Color );
                
                {
                    flo32 bRadians          = -( PI / 4.0f );
                    flo32 Range             = PI - ( -bRadians * 2.0f );
                    int32 nSegments         = 12;
                    flo32 RadiansPerSegment = Range / ( flo32 )nSegments;
                    
                    flo32 TargetTime = OBSERVATION_DECK_ATTACK_TARGET_TIME;
                    if( ObservationDeck.Attack_Counter > 0 ) {
                        TargetTime = OBSERVATION_DECK_ATTACK_CYCLE_TARGET_TIME;
                    }
                    
                    vec4 Turret_Color = COLOR_BLACK;
                    
                    flo32 Warmup_TargetTime = TargetTime - OBSERVATION_DECK_ATTACK_CYCLE_TARGET_TIME;
                    if( ObservationDeck.Attack_Timer >= Warmup_TargetTime ) {
                        flo32 tWarmup = Clamp01( ( ObservationDeck.Attack_Timer - Warmup_TargetTime ) / OBSERVATION_DECK_ATTACK_CYCLE_TARGET_TIME );
                        Turret_Color = lerp( Turret_Color, tWarmup, TowardWhite( ENEMY_BULLET_BASE_COLOR, 0.35f ) );
                    }
                    
                    for( int32 iSegment = 0; iSegment < nSegments; iSegment++ ) {
                        flo32 Radians = bRadians + ObservationDeck.Attack_Radians - RadiansPerSegment * ( flo32 )iSegment;
                        
                        vec2  Dir     = ToDirection2D( Radians );
                        vec2  Pos     = ObservationDeck.Position + Dir * Turret_Radius;
                        
                        DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( Pos, Vec2( Margin * 0.6f, 1.5f ), Radians + PI ), Turret_Color );
                    }
                }
            }
        }
        
        //DrawCircle( Pass, ObservationDeck.Position, RadiusA * Scale, ColorA );
        DrawRect( Pass, TextureID_ObservationDeck, RectTD( ObservationDeck.Position, Vec2( 0.5f, 1.0f ), Vec2( RadiusA * 2.0f, RadiusA ) * Scale ), Body_Color );
    }
    
    { // WINDOW
        flo32 Radians     = OBSERVATION_DECK_WINDOW_RADIANS;
        flo32 HalfRadians = Radians * 0.5f;
        
#if 0        
        if( ObservationDeck.Health > -1 ) {
            DrawCircleSegment( Pass, ObservationDeck.Position, RadiusB * Scale, ( TAU * 0.75f ) - HalfRadians, Radians, false, ColorB );
        } else {
            DrawCircleSegment( Pass, ObservationDeck.Position, RadiusB * Scale, ( TAU * 0.75f ) - HalfRadians, Radians, false, COLOR_GRAY( 0.025f ) );
            
            flo32 RadiansA = ( TAU * 0.75f ) - HalfRadians;
            flo32 RadiansB = ( TAU * 0.75f ) + HalfRadians;
            flo32 RadiansC = ( TAU * 0.75f ) + HalfRadians * 0.2f;
            
            vec2 P = ObservationDeck.Position + ToDirection2D( RadiansA ) * RadiusB;
            vec2 Q = ObservationDeck.Position + ToDirection2D( RadiansB ) * RadiusB;
            
            vec2 AA = lerp( ObservationDeck.Position, 0.95f,  P );
            vec2 AB = lerp( ObservationDeck.Position, 0.965f, P );
            vec2 AC = lerp( ObservationDeck.Position, 0.91f, P );
            vec2 BA = lerp( ObservationDeck.Position, 0.93f, Q );
            vec2 BB = lerp( ObservationDeck.Position, 0.975f, Q );
            vec2 C  = ObservationDeck.Position + ToDirection2D( RadiansC ) * RadiusB;
            
            DrawTriangleBC( Pass, AA, Vec2( 1.0f,  1.5f  ), RadiansA,      ColorB );
            DrawTriangleBC( Pass, AB, Vec2( 0.75f, 1.0f  ), RadiansA,      ColorB );
            DrawTriangleBC( Pass, AC, Vec2( 0.35f, 0.45f ), RadiansA,      ColorB );
            DrawTriangleBC( Pass, BA, Vec2( 1.0f,  1.1f  ), RadiansB - PI, ColorB );
            DrawTriangleBC( Pass, BB, Vec2( 0.35f, 0.45f ), RadiansB - PI, ColorB );
            DrawTriangleBC( Pass, C,  Vec2( 0.45f, 0.55f ), RadiansC + PI * 0.5f, ColorB );
        }
#endif
        
#define OBSERVATION_DECK_WINDOW_TEXTURE_SCALE  ( 0.25f )
        DrawRect( Pass, TextureID_ObservationDeck_Window, RectTD( ObservationDeck.Position + Vec2( 0.0f, -OBSERVATION_DECK_RADIUS ), Vec2( 0.5f, 0.0f ), Vec2( OBSERVATION_DECK_WINDOW_TEXTURE_SCALE * 2.0f, OBSERVATION_DECK_WINDOW_TEXTURE_SCALE ) * OBSERVATION_DECK_RADIUS ), COLOR_WHITE );
        
        {
            vec2 Room_Dim = Vec2( OBSERVATION_DECK_RADIUS * 2.0f, OBSERVATION_DECK_RADIUS * 0.5f ) * OBSERVATION_DECK_WINDOW_TEXTURE_SCALE;
            rect Room_Bound = RectTD( ObservationDeck.Position + Vec2( 0.0f, -OBSERVATION_DECK_RADIUS ), Vec2( 0.5f, 0.0f ), Room_Dim );
            
            vec2 Table_Pos = GetT( Room_Bound, Vec2( 0.5f, 0.475f ) );
            
            { // Draw Board Members Behind Table
                for( int32 iBoardMember = 0; iBoardMember < BoardRoomS->nBoardMember; iBoardMember++ ) {
                    BOARD_MEMBER BoardMember = BoardRoomS->BoardMember[ iBoardMember ];
                    flo32 yPos = BoardMember.Position.y - ( 0.2f + 0.4f );
                    
                    if( yPos > Table_Pos.y ) {
                        DrawBoardMember( Pass, BoardRoomS->BoardMember[ iBoardMember ] );
                    }
                }
            }
            
            {// TABLE
                vec4 TableTop_Color = Vec4( 0.25f, 0.25f, 0.1f, 1.0f );
                
                if( ObservationDeck.Health >= 3 ) {
                    DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.375f, 0.4575f ) ), Vec2( Room_Dim.y * 0.1f, Room_Dim.x * 0.125f ), -PI * 0.5f, TowardBlack( TableTop_Color, 0.75f ) );
                    DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.625f, 0.4575f ) ), Vec2( Room_Dim.y * 0.1f, Room_Dim.x * 0.125f ),  PI * 0.5f, TowardBlack( TableTop_Color, 0.75f ) );
                    DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.45f, 0.4525f ) ), Vec2( Room_Dim.y * 0.125f, Room_Dim.x * 0.07f ),  -PI * 0.5f, TowardBlack( TableTop_Color, 0.725f ) );
                    DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.55f, 0.4525f ) ), Vec2( Room_Dim.y * 0.125f, Room_Dim.x * 0.07f ),  PI * 0.5f, TowardBlack( TableTop_Color, 0.725f ) );
                    
                    DrawRect( Pass, TextureID_Circle, RectCD( GetT( Room_Bound, Vec2( 0.5f, 0.475f ) ), Vec2( 0.3f, 0.2f ) * Room_Dim ), TowardBlack( TableTop_Color, 0.6f ) );
                    DrawRect( Pass, TextureID_Circle, RectCD( GetT( Room_Bound, Vec2( 0.5f, 0.5f ) ), Vec2( 0.3f, 0.2f ) * Room_Dim ), TowardBlack( TableTop_Color, 0.5f ) );
                    
                    DrawRect( Pass, TextureID_Circle, RectCD( GetT( Room_Bound, Vec2( 0.5f, 0.5375f ) ), Vec2( 0.04f, 0.025f ) * Room_Dim ), COLOR_GRAY( 0.05f ) );
                    DrawRect( Pass, TextureID_Circle, RectCD( GetT( Room_Bound, Vec2( 0.5f, 0.55f ) ), Vec2( 0.035f, 0.025f ) * Room_Dim ), COLOR_GRAY( 0.1f ) );
                    
                    DrawRect( Pass, RectTD( GetT( Room_Bound, Vec2( 0.51f, 0.55f ) ), Vec2( 0.5f, 0.0f ), Vec2( 0.1f, 0.35f ) ), COLOR_GRAY( 0.05f ) );
                    
                    vec4 Light_Color = COLOR_YELLOW;
                    if( ObservationDeck.Health < OBSERVATION_DECK_HEALTH ) {
                        if( ObservationDeck.Light_Timer >= 0.0f ) {
                            Light_Color = COLOR_RED;
                        } else {
                            Light_Color = COLOR_BLACK;
                        }
                    }
                    DrawPoint( Pass, GetT( Room_Bound, Vec2( 0.4925f, 0.545f ) ), Vec2( 0.05f, 0.035f ), Light_Color );
                } else if( ObservationDeck.Health >= 0 ) {
                    DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.375f, 0.4575f ) ), Vec2( Room_Dim.y * 0.05f, Room_Dim.x * 0.0625f ), -PI * 0.5f + PI / 256.0f, TowardBlack( TableTop_Color, 0.75f ) );
                    //DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.625f, 0.4575f ) ), Vec2( Room_Dim.y * 0.1f, Room_Dim.x * 0.125f ),  PI * 0.5f, TowardBlack( TableTop_Color, 0.75f ) );
                    //DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.45f, 0.4525f ) ), Vec2( Room_Dim.y * 0.125f, Room_Dim.x * 0.07f ),  -PI * 0.5f, TowardBlack( TableTop_Color, 0.725f ) );
                    //DrawTriangleBC( Pass, GetT( Room_Bound, Vec2( 0.55f, 0.4525f ) ), Vec2( Room_Dim.y * 0.125f, Room_Dim.x * 0.07f ),  PI * 0.5f, TowardBlack( TableTop_Color, 0.725f ) );
                    
                    DrawORect( Pass, TextureID_HalfCircle, ORectTD( GetT( Room_Bound, Vec2( 0.5f, 0.46f ) ), Vec2( 1.0f, 0.5f ), Vec2( 0.145f, 0.1125f ) * Room_Dim, -PI / 24.0f ), TowardBlack( TableTop_Color, 0.6f ) );
                    DrawORect( Pass, TextureID_HalfCircle, ORectTD( GetT( Room_Bound, Vec2( 0.4995f, 0.485f ) ), Vec2( 1.0f, 0.5f ), Vec2( 0.145f, 0.1125f ) * Room_Dim, -PI / 24.0f ), TowardBlack( TableTop_Color, 0.5f ) );
                    
                    DrawORect( Pass, TextureID_HalfCircle, ORectTD( GetT( Room_Bound, Vec2( 0.51f, 0.455f ) ), Vec2( 1.0f, 0.5f ), Vec2( 0.125f, 0.105f ) * Room_Dim, PI / 256.0f + PI ), TowardBlack( TableTop_Color, 0.6f ) );
                    DrawORect( Pass, TextureID_HalfCircle, ORectTD( GetT( Room_Bound, Vec2( 0.5105f, 0.48f ) ), Vec2( 1.0f, 0.5f ), Vec2( 0.125f, 0.105f ) * Room_Dim, PI / 256.0f + PI ), TowardBlack( TableTop_Color, 0.5f ) );
                }
            }
            
            { // Draw Board Members In Front Of Table
                for( int32 iBoardMember = 0; iBoardMember < BoardRoomS->nBoardMember; iBoardMember++ ) {
                    BOARD_MEMBER BoardMember = BoardRoomS->BoardMember[ iBoardMember ];
                    flo32 yPos = BoardMember.Position.y - ( 0.2f + 0.4f );
                    
                    if( yPos <= Table_Pos.y ) {
                        DrawBoardMember( Pass, BoardRoomS->BoardMember[ iBoardMember ] );
                    }
                }
            }
        }
        
        vec2 P = ObservationDeck.Position + ToDirection2D( PI * 1.5f - HalfRadians ) * OBSERVATION_DECK_RADIUS;
        vec2 Q = ObservationDeck.Position + ToDirection2D( PI * 1.5f + HalfRadians ) * OBSERVATION_DECK_RADIUS;
        
        //DrawLine( Pass, ObservationDeck.Position, P, COLOR_RED );
        //DrawLine( Pass, ObservationDeck.Position, Q, COLOR_RED );
        //DrawCircleOutline( Pass, ObservationDeck.Position, ObservationDeck.Radius * 0.885f, COLOR_RED );
        
        { // Crack
            if( ObservationDeck.Health >= 0 ) {
                flo32 tHealth = 1.0f - Clamp01( ( flo32 )( ObservationDeck.Health - 1 ) / ( flo32 )OBSERVATION_DECK_HEALTH );
                
                int32 nSegments = ObservationDeck.WindowCrack_nSegments;
                for( int32 iSegment = 0; iSegment < nSegments; iSegment++ ) {
                    WINDOW_CRACK_SEGMENT Segment = ObservationDeck.WindowCrack[ iSegment ];
                    
                    vec2  V     = Segment.Q - Segment.P;
                    vec2  Dim   = Vec2( GetLength( V ), lerp( 0.05f, Segment.tLevel, 0.1f ) );
                    orect Bound = ORectTD( Segment.P, Vec2( 0.0f, 0.5f ), Dim, ToRadians( V ) );
                    vec4  Color = COLOR_WHITE * 0.75f;
                    
                    DrawORect( Pass, Bound, Color );
                }
            }
        }
    }
    
#if 0    
    if( ObservationDeck.Health > 0 ) {
        Assert( Font );
        Assert( Camera );
        
        char Str[ 4 ] = {};
        sprintf( Str, "%u", ObservationDeck.Health );
        DrawString( Pass, Font, Str, ObservationDeck.Position + Vec2( 0.0f, -( ObservationDeck.Radius - 6.0f ) ), TextAlign_Center, WORLD_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE );
    }
#endif
    
    DISPLAY_VALUE( int32, ObservationDeck.Health );
}

internal void
DrawSecurityDrone( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, SECURITY_DRONE SecurityDrone, boo32 ObservationDeckIsActive, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4 Ring_Color = TowardBlack( SECURITY_DRONE_BASE_COLOR, 0.5f );
    vec4 Dome_Color = TowardBlack( SECURITY_DRONE_BASE_COLOR, 0.35f );
    
    flo32 Radius_Outer = SecurityDrone.Radius * Scale;
    flo32 Radius_Inner = Radius_Outer - 0.75f * Scale;
    
    {
        flo32 tDamage = Clamp01( SecurityDrone.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
        Dome_Color = lerp( Dome_Color, tDamage, TowardWhite( SECURITY_DRONE_BASE_COLOR, 0.35f ) );
        Ring_Color = lerp( Ring_Color, tDamage, TowardWhite( SECURITY_DRONE_BASE_COLOR, 0.2f ) );
    }
    
    if( SecurityDrone.ZeroPoint_IsActive ) {
        vec4  Color  = PLAYER_GADGET_BASE_COLOR;
        flo32 Radius = ( SecurityDrone.Radius + 0.1f ) * 1.125f;
        
        rect Bound = RectCR( SecurityDrone.Position, Radius );
        DrawRect( Pass, TextureID_ZeroPoint_Base_Active, Bound, Color );
    } else if( SecurityDrone.ZeroPoint_ShowUI ) {
        vec4  Color  = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.5f );
        flo32 Radius = ( SecurityDrone.Radius + 0.1f ) * 1.0625f;
        
        rect Bound = RectCR( SecurityDrone.Position, Radius );
        DrawRect( Pass, TextureID_ZeroPoint_Base_Inactive, Bound, Color );
    }
    
    {
        if( ( ObservationDeckIsActive) && ( SecurityDrone.Stage == SecurityDroneStage_Stunned ) && ( SecurityDrone.Velocity != Vec2( 0.0f, 0.0f ) ) ) {
            flo32 Speed = GetLength( SecurityDrone.Velocity );
            if( Speed >= SECURITY_DRONE_HI_DAMAGE_SPEED_THRESHOLD ) {
                Ring_Color = TowardWhite( SECURITY_DRONE_BASE_COLOR, 0.1f );
                Dome_Color = TowardWhite( SECURITY_DRONE_BASE_COLOR, 0.25f );
            }
        }
    }
    
    { // Draw Boost
        flo32 Warmup_TargetTimeA = SecurityDrone.Boost_TargetTime - SECURITY_DRONE_BOOST_WARMUP_TARGET_TIME;
        flo32 Warmup_TargetTimeB = SecurityDrone.Boost_TargetTime - (  6.0f / 60.0f );
        if( SecurityDrone.Boost_Timer >= Warmup_TargetTimeA ) {
            vec2 bPos = SecurityDrone.Position - SecurityDrone.Boost_Direction * ( SecurityDrone.Radius + 0.05f );
            
            flo32 tScale = Clamp01( ( SecurityDrone.Boost_Timer - Warmup_TargetTimeA ) / ( Warmup_TargetTimeB - Warmup_TargetTimeA ) );
            tScale = lerp( 0.0f, tScale, 0.1f );
            
            if( SecurityDrone.Boost_Timer >= Warmup_TargetTimeB ) {
                tScale = Clamp01( ( SecurityDrone.Boost_Timer - Warmup_TargetTimeB ) / ( SecurityDrone.Boost_TargetTime - Warmup_TargetTimeB ) );
                tScale = lerp( 0.1f, tScale, 1.0f );
            }
            
            flo32 xDimA  = lerp( 0.5f, tScale, 2.5f );
            flo32 yDimA  = lerp( 0.25f, tScale, 0.3f );
            
            flo32 xDimB  = lerp( 0.15f, tScale, 0.1f );
            flo32 yDimB  = lerp( 0.45f, tScale, 0.25f );
            
            flo32 tAlpha = Clamp01( ( SecurityDrone.Boost_Timer - Warmup_TargetTimeA ) / SecurityDrone.Boost_TargetTime - Warmup_TargetTimeA );
            tAlpha     = lerp( 0.20f, tAlpha, 0.35f );
            vec4 Color = Vec4( tAlpha, tAlpha, tAlpha, tAlpha );
            
            DrawEllipse( Pass, bPos, Vec2( xDimA, yDimA ), ToRadians( SecurityDrone.Boost_Direction ), Color );
            DrawEllipse( Pass, bPos, Vec2( xDimB, yDimB ), ToRadians( SecurityDrone.Boost_Direction ), Color );
        }
    }
    
    DrawCircle( Pass, SecurityDrone.Position, Radius_Outer, Ring_Color * Alpha );
    
    {
        vec4   Light_Color = COLOR_BLACK;
        int32 bLight       = ( int32 )( SecurityDrone.Light_Timer / SECURITY_DRONE_LIGHT_CYCLE_TARGET_TIME );
        
        flo32 Radius = ( Radius_Outer + Radius_Inner ) * 0.5f;
        
        int32 nLight = 12;
        for( int32 iLight = 0; iLight < nLight; iLight++ ) {
            flo32 t       = Clamp01( ( ( flo32 )iLight + 0.5f ) / ( flo32 )nLight );
            flo32 Radians = t * -TAU + SecurityDrone.Radians;
            vec2  Dir     = ToDirection2D( Radians );
            vec2  Pos     = SecurityDrone.Position + Dir * Radius;
            
            if( SecurityDrone.Recover_Timer > 0.0f ) {
                int32 Select = ( int32 )( SecurityDrone.Recover_Timer / SECURITY_DRONE_LIGHT_CYCLE_TARGET_TIME );
                if( ( Select % 2 ) == 0 ) {
                    Light_Color = COLOR_RED;
                }
            } else if( SecurityDrone.Stage == SecurityDroneStage_Stunned ) {
                int32 Select = ( int32 )( SecurityDrone.Light_Timer / ( SECURITY_DRONE_LIGHT_CYCLE_TARGET_TIME * 3.0f ) );
                if( ( Select % 2 ) == 0 ) {
                    Light_Color = COLOR_RED;
                }
            } else if(   ( SecurityDrone.Stage == SecurityDroneStage_Move  )
                      || ( SecurityDrone.Stage == SecurityDroneStage_Spawn ) ) {
                flo32 tColor = ( ( flo32 )( ( bLight + iLight ) % 4 ) ) / 3.0f;
                Light_Color = lerp( COLOR_BLACK, tColor, COLOR_YELLOW );
            } else {
                flo32 tColor = 1.0f - ( ( flo32 )( ( bLight + iLight ) % nLight ) / ( flo32 )( nLight - 1 ) );
                tColor = Clamp01( tColor * 2.0f - 1.0f );
                Light_Color = lerp( COLOR_BLACK, tColor, COLOR_YELLOW );
            }
            
            DrawRect( Pass, TextureID_Dome, RectCR( Pos, Radius_Outer * 0.04f ), Light_Color * Alpha );
        }
    }
    
    DrawRect( Pass, TextureID_Worm_Body, RectCR( SecurityDrone.Position, Radius_Inner ), Dome_Color * Alpha );
    
    {
        vec4 Turret_Barrel_Color       = TowardBlack( SECURITY_DRONE_BASE_COLOR, 0.25f );
        vec4 Turret_InsideBarrel_Color = COLOR_BLACK;
        
        int32 nEnemyBullet = 3;
        if( SecurityDrone.Stage == SecurityDroneStage_Move ) {
            flo32 TargetTime = SecurityDrone.Fire_TargetTime - ( 60.0f / 60.0f );
            if( SecurityDrone.Fire_Timer >= TargetTime ) {
                flo32 t = Clamp01( ( SecurityDrone.Fire_Timer - TargetTime ) / ( SecurityDrone.Fire_TargetTime - TargetTime - ( 4.0f / 60.0f ) ) );
                Turret_InsideBarrel_Color  = lerp( Turret_InsideBarrel_Color, t, TowardWhite( ENEMY_BULLET_BASE_COLOR, 0.35f ) );
                Turret_Barrel_Color = lerp( Turret_Barrel_Color, t, TowardWhite( Turret_Barrel_Color, 0.5f ) );
            }
        }
        if( ( SecurityDrone.Stage == SecurityDroneStage_Spin_Bullets ) || ( SecurityDrone.Stage == SecurityDroneStage_Sweep_Bullets ) ) {
            flo32 TargetTime = SecurityDrone.Fire_TargetTime - ( 80.0f / 60.0f );
            Assert( TargetTime < 0.0f );
            
            if( SecurityDrone.Fire_Timer >= TargetTime ) {
                flo32 t = Clamp01( ( SecurityDrone.Fire_Timer - TargetTime ) / ( SecurityDrone.Fire_TargetTime - TargetTime ) );
                Turret_InsideBarrel_Color  = lerp( Turret_InsideBarrel_Color, t, TowardWhite( ENEMY_BULLET_BASE_COLOR, 0.35f ) );
                Turret_Barrel_Color = lerp( Turret_Barrel_Color, t, TowardWhite( Turret_Barrel_Color, 0.5f ) );
            }
        }
        
        for( int32 iEnemyBullet = 0; iEnemyBullet < nEnemyBullet; iEnemyBullet++ ) {
            flo32 Radians = SecurityDrone.Radians + ( TAU / 3.0f ) * ( flo32 )iEnemyBullet;
            flo32 Dist    = Radius_Outer * 0.8f;
            vec2  Dir     = ToDirection2D( Radians );
            
            { // Barrel
                vec2 Dim  = Vec2( 0.15f, 0.5f ) * Radius_Outer;
                vec2 Pos  = SecurityDrone.Position + Dir * ( Radius_Outer * 0.95f );
                vec2 PosA = Pos - Dir * Dim.x;
                
                DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Pos, Vec2( 1.0f, 0.5f ), Dim, Radians ), Turret_Barrel_Color * Alpha );
                DrawORect( Pass, TextureID_SecurityDrone_Turret_End, ORectTD( PosA, Vec2( 1.0f, 0.5f ), Dim * Vec2( 1.0f, 1.0f ), Radians ), Turret_Barrel_Color * Alpha );
                
                DrawEllipse( Pass, Pos, Vec2( 0.2f * Radius_Outer, Dim.y ) * 0.5f, Radians, TowardBlack( Turret_Barrel_Color, 0.4f ) * Alpha );
                
                DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( Pos, Vec2( 0.15f * Radius_Outer, Dim.y * 0.9f ), Radians + PI ), Turret_InsideBarrel_Color * Alpha );
            }
        }
    }
    
    {
        int32 nLaser = SecurityDrone.nLaser;
        for( int32 iLaser = 0; iLaser < nLaser; iLaser++ ) {
            flo32 Radians = SecurityDrone.Radians + ( ( TAU / ( flo32 )nLaser ) * ( flo32 )iLaser ) + ( TAU / ( ( flo32 )nLaser * 2.0f ) );
            vec2  Dir    = ToDirection2D( Radians );
            flo32 Dist   = Radius_Outer * 0.8f;
            
            {
                vec4 Laser_Barrel_Color = TowardBlack( SECURITY_DRONE_BASE_COLOR, 0.4f  );
                vec4 Laser_Tip_Color    = TowardBlack( SECURITY_DRONE_BASE_COLOR, 0.75f );
                
                if( SecurityDrone.Laser_IsActive[ 0 ] ) {
                    flo32 tWarmUp = Clamp01( ( SecurityDrone.Timer + SECURITY_DRONE_LASERS_WARMUP_TARGET_TIME - ( 20.0f / 60.0f ) ) / SECURITY_DRONE_LASERS_WARMUP_TARGET_TIME );
                    tWarmUp *= tWarmUp;
                    
                    Laser_Barrel_Color = lerp( Laser_Barrel_Color, tWarmUp, TowardWhite( Laser_Barrel_Color, 0.5f ) );
                    Laser_Tip_Color = lerp( Laser_Tip_Color, tWarmUp, TowardWhite( Laser_Tip_Color, 0.2f ) );
                }
                
                vec2 Barrel_Dim = Vec2( Dist * 0.25f, 0.3f   * Radius_Outer );
                vec2 Tip_Dim    = Vec2( Dist * 0.25f, 0.165f * Radius_Outer );
                
                vec2 Tip_Pos     = SecurityDrone.Position + Dir * Dist;
                vec2 Barrel_Pos  = SecurityDrone.Position + Dir * ( Dist - 0.35f * Scale );
                vec2 Barrel_PosA = Barrel_Pos - Dir * Barrel_Dim.x;
                
                DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Tip_Pos, Vec2( 1.0f, 0.5f ), Tip_Dim, Radians ), Laser_Tip_Color * Alpha );
                DrawORect( Pass, TextureID_Barrel_Rot90, ORectTD( Barrel_Pos, Vec2( 1.0f, 0.5f ), Barrel_Dim, Radians ), Laser_Barrel_Color * Alpha );
                DrawORect( Pass, TextureID_SecurityDrone_Turret_End, ORectTD( Barrel_PosA, Vec2( 1.0f, 0.5f ), Barrel_Dim, Radians ), Laser_Barrel_Color * Alpha );
            }
            
            if( SecurityDrone.Laser_IsActive[ iLaser ] ) {
                if( Alpha >= 1.0f ) {
                    flo32 tWarmUp = Clamp01( ( SecurityDrone.Timer + SECURITY_DRONE_LASERS_WARMUP_TARGET_TIME - ( 20.0f / 60.0f ) ) / SECURITY_DRONE_LASERS_WARMUP_TARGET_TIME );
                    tWarmUp *= tWarmUp;
                    
                    vec4 Color = Vec4( tWarmUp, 0.0f, 0.0f, tWarmUp );
                    
                    vec2 A = SecurityDrone.Position + Dir * ( Dist );
                    vec2 B = SecurityDrone.Position + Dir * ( SecurityDrone.Laser_MaxLength );
                    DrawLine( Pass, A, B, Color );
                }
                if( SecurityDrone.Timer >= 0.0f ) {
                    vec2  Pos    = SecurityDrone.Position + Dir * Dist;
                    vec2  Dest   = SecurityDrone.Laser_Dest[ iLaser ];
                    flo32 Length = GetLength( Dest - Pos );
                    flo32 Margin = MINER_LASER_WIDTH * 0.75f;
                    flo32 Width  = MINER_LASER_WIDTH + Margin * 2.0f;
                    
                    if( Scale > 1.0f ) {
                        Length += lerp( 0.0f, Scale - 1.0f, 4.0f );
                        
                        Margin *= Scale * 1.5f;
                        Width  *= Scale * 1.5f;
                    }
                    
                    orect Bound  = ORectBCD( Pos, Vec2( Width, Length ), Radians );
                    DrawORect( Pass, TextureID_MinerLaser_Body, Bound, COLOR_WHITE * Alpha );
                    
                    DrawORect( Pass, TextureID_MinerLaser_End, ORectBCD( GetT( Bound, Vec2( 0.5f, 0.0f ) ), Vec2( Width, Margin ), Radians + PI ), COLOR_WHITE * Alpha );
                    DrawORect( Pass, TextureID_MinerLaser_End, ORectBCD( GetT( Bound, Vec2( 0.5f, 1.0f ) ), Vec2( Width, Margin ), Radians ), COLOR_WHITE * Alpha );
                }
            }
        }
    }
    
    {
        flo32 Radius = Radius_Outer * 0.25f;
        DrawRect( Pass, TextureID_Dome, RectCR( SecurityDrone.Position, Radius ), TowardBlack( Dome_Color, 0.5f ) * Alpha );
        
        if( SecurityDrone.Stage == SecurityDroneStage_Stunned ) {
            flo32 tStunned = 1.0f - Clamp01( SecurityDrone.Timer / SECURITY_DRONE_STUNNED_TARGET_TIME );
            
            vec4 Stunned_Color = lerp( Vec4( 0.25f, 0.0f, 0.0f, 1.0f ), tStunned, Vec4( 0.75f, 0.0f, 0.0f, 1.0f ) );
            if( SecurityDrone.ZeroPoint_IsActive ) {
                Stunned_Color = TowardBlack( PLAYER_GADGET_BASE_COLOR, 0.25f );
            }
            
            DrawRect( Pass, TextureID_Dome, RectCR( SecurityDrone.Position, Radius * tStunned ), Stunned_Color * Alpha );
            DrawRect( Pass, TextureID_Glow, RectCR( SecurityDrone.Position, Radius * tStunned + 0.5f * Scale ), Stunned_Color * Alpha );
        }
    }
    
    if( SecurityDrone.Health > 0 ) {
        DrawEntityHealth( Pass, AppState, Camera, SecurityDrone.Position, SecurityDrone.Health, COLOR_WHITE );
    }
}

internal void
DrawBoardRoom( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    PLAYER_STATE     * Player     = Draw->Player;
    BOARD_ROOM_STATE * BoardRoomS = Draw->BoardRoomS;
    
    if( BoardRoomS->State_IsActive ) {
        OBSERVATION_DECK ObservationDeck = BoardRoomS->ObservationDeck;
        
        vec2 Portal_Position = {};
        { // Draw Observation Deck Exit
            {
                flo32 tRotate = Clamp01( ObservationDeck.Teleport_ScaleTimer / 2.5f );
                flo32 Sca  = lerp( 2.0f, tRotate, 10.0f );
                flo32 Alph = 1.0f;
                vec2 bPos  = GetCenter( AppState->World_Bound ) + Vec2( 0.0f, 2.0f );
                flo32 Color_Scale = 0.5f;
                
                Portal_Position = bPos;
                
                { // Inner Rect
                    YACHT_EXIT_RING_DATA Ring = ObservationDeck.Teleport_InnerRing;
                    
                    Ring.Radius *= Sca;
                    
                    flo32 Radius = Ring.Radius;
                    vec4  Color  = lerp( COLOR_GRAY( 0.1f ), Ring.tColor, Vec4( 0.6f, 0.6f, 0.4f, 1.0f ) );
                    
                    if( ObservationDeck.Teleport_tFlash > 0.0f ) {
                        Color = lerp( Color, Clamp01( ObservationDeck.Teleport_tFlash ), TowardWhite( Color, 0.85f ) );
                    }
                    
                    Color.rgb *= Color_Scale;
                    
                    Color *= Ring.Alpha;
                    orect Bound  = ORectCR( bPos, Radius * 0.8f, Ring.Radians );
                    DrawORect( Pass, Bound, Color * Alph );
                }
                
                { // Mid Rects
                    YACHT_EXIT_RING_DATA Ring = ObservationDeck.Teleport_MidRing;
                    
                    Ring.Radius *= Sca;
                    
                    int32 nRect  = 10;
                    flo32 Radius = Ring.Radius;
                    
                    // NOTE : tWidth  is the  t  of  MidRing.Radius  from  MidRing.InitRadius  to  MidRing.MinRadius
                    flo32 tWidth = Clamp01( ( Ring.Radius - 0.35f ) / ( 1.65f ) );
                    
                    flo32 Width  = lerp( 0.65f, tWidth, 1.65f ) * Sca;
                    vec2  Dim    = Vec2( Width, Width );
                    
                    for( int32 iRect = 0; iRect < nRect; iRect++ ) {
                        flo32 RadiansA = Ring.Radians + ( TAU / ( flo32 )nRect ) * ( flo32 )iRect;
                        flo32 RadiansB = ( ( PI ) / ( flo32 )nRect ) * ( flo32 )iRect;
                        vec2  Dir     = ToDirection2D( RadiansA );
                        vec2  Pos     = bPos + Dir * Radius;
                        
                        vec4  ColorA   = COLOR_GRAY( 0.075f );
                        vec4  ColorB   = Vec4( 0.2f, 0.2f, 0.1f, 1.0f );
                        if( ( iRect % 2 ) == 0 ) {
                            ColorA = COLOR_GRAY( 0.08f );
                            ColorB = Vec4( 0.25f, 0.25f, 0.15f, 1.0f );
                        }
                        
                        vec4 Color = lerp( ColorA, Ring.tColor, ColorB );
                        
                        if( ObservationDeck.Teleport_tFlash > 0.0f ) {
                            Color = lerp( Color, Clamp01( ObservationDeck.Teleport_tFlash ), TowardWhite( Color, 0.75f ) );
                        }
                        
                        Color.rgb *= Color_Scale;
                        
                        Color *= Ring.Alpha;
                        orect Bound   = ORectCD( Pos, Dim, RadiansB );
                        DrawORect( Pass, Bound, Color * Alph );
                    }
                }
            }
        }
        
        if( ObservationDeck.IsPresent ) {
            vec4 Flash_Color = {};
            if( ( ObservationDeck.Health >= 0 ) && ( ObservationDeck.Teleport_Timer >= ( 2.0f / 60.0f ) ) ) {
                flo32 TargetTime = ( 240.0f / 60.0f );
                flo32 tColor = ObservationDeck.Teleport_Timer / TargetTime;
                Flash_Color = TowardWhite( OBSERVATION_DECK_BASE_COLOR, 0.4f ) * Clamp01( tColor );
                Flash_Color = lerp( Flash_Color, ObservationDeck.Teleport_tFlash, COLOR_WHITE );
            }
            DrawObservationDeck( Pass, AppState, Camera, BoardRoomS, 1.0f, 1.0f, Flash_Color );
        }
        
        if( ( ObservationDeck.Health >= 0 ) && ( ObservationDeck.Teleport_Timer >= ( 2.0f / 60.0f ) ) ) {
            flo32 TargetTime = ( 240.0f / 60.0f );
            flo32 tColor = ObservationDeck.Teleport_Timer / TargetTime;
            vec4   Color = TowardWhite( OBSERVATION_DECK_BASE_COLOR, 0.5f ) * ( Clamp01( tColor ) * 0.75f );
            Color = lerp( Color, ObservationDeck.Teleport_tFlash, COLOR_WHITE );
            
            flo32 Radius   = ObservationDeck.Radius;
            vec2  Position = ObservationDeck.Position;
            if( !ObservationDeck.IsPresent ) {
                flo32 t = Clamp01( ObservationDeck.Teleport_tFlash / 4.0f );
                Position = lerp( Portal_Position, t * t, ObservationDeck.Position );
                Radius  *= ( t * t );
            }
            DrawORect( Pass, TextureID_Dome, ORectCR( Position, Radius, tColor * TAU * 32.0f ), Color );
        }
        
        DrawSecurityDrone( Pass, AppState, Camera, BoardRoomS->SecurityDrone, ( ObservationDeck.IsPresent ) && ( ObservationDeck.Health >= 0 ) );
    }
}

internal void
DrawObservationDeckInnerShield( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BOARD_ROOM_STATE * BoardRoomS      = Draw->BoardRoomS;
    OBSERVATION_DECK   ObservationDeck = BoardRoomS->ObservationDeck;
    
    if( ( BoardRoomS->State_IsActive ) && ( ObservationDeck.IsPresent ) ) {
        flo32 tAlpha = 1.0f - Clamp01( ObservationDeck.ShieldDown_Timer / SATELLITE_SHIELD_DOWN_TARGET_TIME );
        
        flo32 TargetTimeA = (  4.0f / 60.0f );
        flo32 TargetTimeB = (  8.0f / 60.0f );
        flo32 TargetTimeC = ( 12.0f / 60.0f );
        flo32 TargetTimeD = ( 16.0f / 60.0f );
        flo32 TargetTimeE = ( 22.0f / 60.0f );
        flo32 TargetTimeF = ( 28.0f / 60.0f );
        
        if( ( ObservationDeck.ShieldDown_Timer >= TargetTimeA ) && ( ObservationDeck.ShieldDown_Timer < TargetTimeB ) ) {
            flo32 t = GetT01( ObservationDeck.ShieldDown_Timer, TargetTimeA, TargetTimeB );
            if( t < 0.5f ) {
                t = 1.0f - t;
            }
            tAlpha = 0.35f * t;
        }
        if( ( ObservationDeck.ShieldDown_Timer >= TargetTimeC ) && ( ObservationDeck.ShieldDown_Timer < TargetTimeD ) ) {
            flo32 t = GetT01( ObservationDeck.ShieldDown_Timer, TargetTimeC, TargetTimeD );
            if( t < 0.5f ) {
                t = 1.0f - t;
            }
            tAlpha = 0.35f;
        }
        if( ( ObservationDeck.ShieldDown_Timer >= TargetTimeE ) && ( ObservationDeck.ShieldDown_Timer < TargetTimeF ) ) {
            flo32 t = GetT01( ObservationDeck.ShieldDown_Timer, TargetTimeE, TargetTimeF );
            if( t < 0.5f ) {
                t = 1.0f - t;
            }
            tAlpha = 0.35f;
        }
        
        if( tAlpha > 0.0f ) {
            //vec4 Shield_Color = Vec4( 0.0f, 0.05f, 0.05f, 0.05f );
            //DrawCircle( Pass, ObservationDeck.Position, ObservationDeck.Radius + ObservationDeck.Shield_Dist, Shield_Color );
            
            //DrawCircleOutline( Pass, ObservationDeck.Position, ObservationDeck.Radius + ObservationDeck.Shield_Dist, COLOR_RED );
            
            flo32 Radius = ObservationDeck.Radius + ObservationDeck.Shield_Dist + 3.5f;
            DrawRect( Pass, TextureID_ObservationDeck_Shield_Inner, RectTD( ObservationDeck.Position, Vec2( 0.5f, 1.0f ), Vec2( Radius * 2.0f, Radius ) ), COLOR_WHITE * tAlpha * 0.65f );
            
            for( int32 iShieldHit = 0; iShieldHit < OBSERVATION_DECK_SHIELD_HIT_MAX_COUNT; iShieldHit++ ) {
                OBSERVATION_DECK_SHIELD_HIT ShieldHit = ObservationDeck.ShieldHit[ iShieldHit ];
                
                flo32 Margin = 0.75f;
                vec2  Dim    = Vec2( 2.0f, 8.0f ) * ShieldHit.Scale;
                Dim.x += Margin;
                
                vec2  Dir    = ToDirection2D( ShieldHit.Radians );
                vec2  P      = ShieldHit.Pos + Dir * Margin;
                orect R      = ORectTD( P, Vec2( 0.0f, 0.5f ), Dim, ShieldHit.Radians + PI );
                flo32 Alph   = Clamp01( ShieldHit.Timer / OBSERVATION_DECK_SHIELD_HIT_TARGET_TIME );
                DrawORect( Pass, TextureID_SatelliteDish_ShieldHit, R, COLOR_WHITE * ( Alph * Alph * tAlpha ) );
            }
        }
    }
}

internal void
FinalizeBoardRoom( APP_STATE * AppState ) {
    SCORE_STATE      * ScoreS     = &AppState->ScoreS;
    BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
    PLAYER_STATE     * Player     = &AppState->Player;
    
    if( BoardRoomS->State_IsActive ) {
        OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
        SECURITY_DRONE   * SecurityDrone   = &BoardRoomS->SecurityDrone;
        
        if( ( ( !ObservationDeck->IsPresent ) || ( Player->Health <= 0 ) ) && ( SecurityDrone->Stage != SecurityDroneStage_Stunned ) && ( SecurityDrone->Stage != SecurityDroneStage_Move ) ) {
            SecurityDrone->Stage           = SecurityDroneStage_Move;
            SecurityDrone->Timer           = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME );
            SecurityDrone->Fire_Timer      = 0.0f;
            SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
            SecurityDrone->Health          = SECURITY_DRONE_OFFSCREEN_RECOVER_HEALTH;
            SecurityDrone->Destination     = GetMoveDestinationForSecurityDrone( AppState );
            SecurityDrone->Recover_Timer   = 0.0f;
            
            for( int32 iLaser = 0; iLaser < SecurityDrone->nLaser; iLaser++ ) {
                SecurityDrone->Laser_IsActive[ iLaser ]  = false;
            }
        }
        
        if( ( SecurityDrone->Stage != SecurityDroneStage_Stunned ) && ( SecurityDrone->Health <= 0 ) ) {
            SecurityDrone->Stage          = SecurityDroneStage_Stunned;
            SecurityDrone->Timer          = 0.0f;
            SecurityDrone->Fire_Timer     = 0.0f;
            SecurityDrone->Dest_Trigger   = false;
            
            for( int32 iLaser = 0; iLaser < SecurityDrone->nLaser; iLaser++ ) {
                SecurityDrone->Laser_IsActive[ iLaser ] = false;
            }
            
            if( ( BoardRoomS->Spawn_HitObservationDeck ) || ( BoardRoomS->Spawn_HitMissileLauncher ) ) {
                if( ObservationDeck->Health >= 5 ) {
                    BoardRoomS->Spawn_nGunners = 2;
                } else if( ObservationDeck->Health >= 3 ) {
                    BoardRoomS->Spawn_nGunners = 4;
                } else if( ObservationDeck->Health >= 1 ) {
                    BoardRoomS->Spawn_nGunners = 6;
                }
                
                if( ( BoardRoomS->Spawn_HitMissileLauncher ) && ( !BoardRoomS->Spawn_HitObservationDeck ) ) {
                    BoardRoomS->Spawn_nGunners /= 2;
                }
                
                BoardRoomS->Spawn_HitObservationDeck = false;
                BoardRoomS->Spawn_HitMissileLauncher = false;
                
                BoardRoomS->Spawn_Timer      = 0.0f;
                BoardRoomS->Spawn_TargetTime = ( 20.0f / 60.0f );
                
                BoardRoomS->Spawn_nEntities = ( int32 )( ( flo32 )BoardRoomS->Spawn_nGunners * 1.5f );
            }
        }
        
        if( !SecurityDrone->ZeroPoint_IsActive ) {
            switch( SecurityDrone->Stage ) {
                case SecurityDroneStage_Stunned: {
                    flo32 TargetTime = SECURITY_DRONE_STUNNED_TARGET_TIME;
                    
                    DISPLAY_VALUE( flo32, SecurityDrone->Timer );
                    
                    rect Bound = AddRadius( AppState->World_Bound, SecurityDrone->Radius );
                    if( ( !SecurityDrone->ZeroPoint_IsActive ) && ( !IsInRect( SecurityDrone->Position, Bound ) ) ) {
                        SecurityDrone->Timer = TargetTime;
                    }
                    
                    if( SecurityDrone->Timer >= TargetTime ) {
                        if( IsInRect( SecurityDrone->Position, AppState->World_Bound ) ) {
                            int32 Numer[ SecurityDroneStage_Count ] = {};
                            Numer[ SecurityDroneStage_Spin_Bullets  ] = 1;
                            Numer[ SecurityDroneStage_Spin_Lasers   ] = 1;
                            Numer[ SecurityDroneStage_Sweep_Bullets ] = 1;
                            Numer[ SecurityDroneStage_Sweep_Lasers  ] = 1;
                            
                            SelectStageForSecurityDrone( AppState, Numer, SecurityDroneStage_Count );
                        } else {
                            SecurityDrone->Stage       = SecurityDroneStage_Spawn;
                            SecurityDrone->Timer       = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME );
                            SecurityDrone->Fire_Timer  = 0.0f;
                            SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                            SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                        }
                        
                        SecurityDrone->Health = SECURITY_DRONE_OFFSCREEN_RECOVER_HEALTH;
                        SecurityDrone->Recover_Timer = SECURITY_DRONE_RECOVER_LIGHT_TARGET_TIME;
                    }
                } break;
                
                case SecurityDroneStage_Spawn: {
                    flo32 Radius   = 2.0f;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( SecurityDrone->Destination - SecurityDrone->Position );
                    if( DistSq <= RadiusSq ) {
                        SecurityDrone->Dest_Trigger = true;
                    }
                    
                    flo32 Speed = GetLength( SecurityDrone->Velocity );
                    if( ( SecurityDrone->Dest_Trigger ) && ( Speed <= 1.0f ) ) {
                        if( ObservationDeck->IsPresent ) {
                            int32 Numer[ SecurityDroneStage_Count ] = {};
                            Numer[ SecurityDroneStage_Spin_Bullets  ] = 1;
                            Numer[ SecurityDroneStage_Spin_Lasers   ] = 1;
                            Numer[ SecurityDroneStage_Sweep_Bullets ] = 1;
                            Numer[ SecurityDroneStage_Sweep_Lasers  ] = 1;
                            
                            SelectStageForSecurityDrone( AppState, Numer, SecurityDroneStage_Count );
                        } else {
                            SecurityDrone->Stage           = SecurityDroneStage_Move;
                            SecurityDrone->Timer           = -( SECURITY_DRONE_SPAWN_COOLDOWN_TARGET_TIME );
                            SecurityDrone->Fire_Timer      = SECURITY_DRONE_FIRE_TARGET_TIME - ( 60.0f / 60.0f );
                            SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                            SecurityDrone->Dest_Trigger    = false;
                            
                            SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                        }
                    }
                } break;
                
                case SecurityDroneStage_Move: {
                    if( SecurityDrone->Timer >= 0.0f ) {
                        flo32 Radius   = 2.0f;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( SecurityDrone->Destination - SecurityDrone->Position );
                        if( DistSq <= RadiusSq ) {
                            int32 Numer[ SecurityDroneStage_Count ] = {};
                            Numer[ SecurityDroneStage_Move          ] = SecurityDrone->Numer_Move;
                            Numer[ SecurityDroneStage_Spin_Bullets  ] = 1;
                            Numer[ SecurityDroneStage_Spin_Lasers   ] = 1;
                            Numer[ SecurityDroneStage_Sweep_Bullets ] = 1;
                            Numer[ SecurityDroneStage_Sweep_Lasers  ] = 1;
                            
                            SecurityDrone->Numer_Move = MaxValue( SecurityDrone->Numer_Move - 1, 0 );
                            
                            SelectStageForSecurityDrone( AppState, Numer, SecurityDroneStage_Count );
                        }
                    }
                } break;
                
                case SecurityDroneStage_Spin_Bullets: {
                    if( SecurityDrone->Counter <= 0 ) {
                        SecurityDrone->Stage      = SecurityDroneStage_Move;
                        SecurityDrone->Timer      = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME );
                        SecurityDrone->Fire_Timer = SECURITY_DRONE_FIRE_TARGET_TIME - ( 120.0f / 60.0f );
                        SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                        
                        SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                        SecurityDrone->Numer_Move  = 3;
                    }
                } break;
                
                case SecurityDroneStage_Spin_Lasers: {
                    flo32 TargetTime = ( 480.0f / 60.0f );
                    if( ObservationDeck->Health >= OBSERVATION_DECK_HEALTH ) {
                        TargetTime = ( 360.0f / 60.0f );
                    }
                    if( SecurityDrone->Timer >= TargetTime ) {
                        SecurityDrone->Stage      = SecurityDroneStage_Move;
                        SecurityDrone->Timer      = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME );
                        SecurityDrone->Fire_Timer = SECURITY_DRONE_FIRE_TARGET_TIME - ( 120.0f / 60.0f );
                        SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                        
                        for( int32 iLaser = 0; iLaser < SecurityDrone->nLaser; iLaser++ ) {
                            SecurityDrone->Laser_IsActive[ iLaser ] = false;
                        }
                        
                        SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                        SecurityDrone->Numer_Move  = 3;
                    }
                } break;
                
                case SecurityDroneStage_Sweep_Bullets: {
                    flo32 Radius   = 2.0f;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( SecurityDrone->Destination - SecurityDrone->Position );
                    if( DistSq <= RadiusSq ) {
                        SecurityDrone->Stage      = SecurityDroneStage_Move;
                        SecurityDrone->Timer      = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME );
                        SecurityDrone->Fire_Timer = SECURITY_DRONE_FIRE_TARGET_TIME - ( 120.0f / 60.0f );
                        SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                        
                        SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                        SecurityDrone->Numer_Move  = 3;
                    }
                } break;
                
                case SecurityDroneStage_Sweep_Lasers: {
                    flo32 Radius   = 2.0f;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( SecurityDrone->Destination - SecurityDrone->Position );
                    if( DistSq <= RadiusSq ) {
                        SecurityDrone->Stage      = SecurityDroneStage_Move;
                        SecurityDrone->Timer      = -( SECURITY_DRONE_MOVE_WARMUP_TARGET_TIME );
                        SecurityDrone->Fire_Timer = SECURITY_DRONE_FIRE_TARGET_TIME - ( 120.0f / 60.0f );
                        SecurityDrone->Fire_TargetTime = SECURITY_DRONE_FIRE_TARGET_TIME;
                        
                        for( int32 iLaser = 0; iLaser < SecurityDrone->nLaser; iLaser++ ) {
                            SecurityDrone->Laser_IsActive[ iLaser ] = false;
                        }
                        
                        SecurityDrone->Destination = GetMoveDestinationForSecurityDrone( AppState );
                        SecurityDrone->Numer_Move  = 3;
                    }
                } break;
            }
        }
        
        {
            for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                
                if( MissileLauncher->Health == 0 ) {
                    MissileLauncher->Health = -1;
                    
                    AddBigEntityDeathParticles( AppState, MissileLauncher->Position, MissileLauncher->Radius, MISSILE_LAUNCHER_BASE_COLOR );
                    AddBigEntityDeathParticles( AppState, MissileLauncher->Position, MissileLauncher->Radius, MISSILE_LAUNCHER_BASE_COLOR );
                    AddBigEntityDeathParticles( AppState, MissileLauncher->Position, MissileLauncher->Radius, COLOR_WHITE, false );
                    
                    AddKillToScore( AppState, EntityType_MissileLauncher, MissileLauncher->Position );
                    
                    POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_MissileLauncher );
                    int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                    
                    AddScoreMultiplierSpawner( AppState, ScoreMultiplierSpawnerC( MissileLauncher->Position, nScoreMultiplier ) );
                    
                    ObservationDeck->nMissileLaunchersDestroyed += 1;
                }
            }
        }
    }
}