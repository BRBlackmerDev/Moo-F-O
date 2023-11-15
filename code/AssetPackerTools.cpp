

internal uint32
ToU32Color( uint8 r, uint8 g, uint8 b, uint8 a = 255 ) {
    uint32 Result = ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );
    return Result;
}

internal uint32
ToU32Color( vec4 V4Color ) {
    uint8 r = ( uint8 )( V4Color.r * 255 );
    uint8 g = ( uint8 )( V4Color.g * 255 );
    uint8 b = ( uint8 )( V4Color.b * 255 );
    uint8 a = ( uint8 )( V4Color.a * 255 );
    uint32 Result = ToU32Color( r, g, b, a );
    return Result;
}

internal uint32
ToU32Color( vec3 V3Color, flo32 Alpha ) {
    uint32 Result = ToU32Color( Vec4( V3Color, Alpha ) );
    return Result;
}

internal vec4
ToColor( uint8 r, uint8 g, uint8 b ) {
    vec4 Result = {};
    Result.r = ( flo32 )r / 255.0f;
    Result.g = ( flo32 )g / 255.0f;
    Result.b = ( flo32 )b / 255.0f;
    Result.a = 1.0f;
    return Result;
}

internal vec4
ToV4Color( uint32 Color ) {
    vec4 Result = {};
    Result.r = ( ( flo32 )( ( Color       ) & 0xFF ) ) / 255.0f;
    Result.g = ( ( flo32 )( ( Color >>  8 ) & 0xFF ) ) / 255.0f;
    Result.b = ( ( flo32 )( ( Color >> 16 ) & 0xFF ) ) / 255.0f;
    Result.a = ( ( flo32 )( ( Color >> 24 ) & 0xFF ) ) / 255.0f;
    return Result;
}

//----------
//----------

internal vec2
ClosestPointToEdge( vec2 C, vec2 A, vec2 B ) {
    vec2 AB = B - A;
    vec2 AC = C - A;
    
    flo32 t      = Clamp01( dot( AC, AB ) / dot( AB, AB ) );
    vec2  Result = A + AB * t;
    return Result;
}

internal flo32
DistPointToEdge( vec2 C, vec2 A, vec2 B ) {
    vec2  P      = ClosestPointToEdge( C, A, B );
    flo32 Result = GetLength( C - P );
    return Result;
}

internal flo32
DistSqPointToEdge( vec2 C, vec2 A, vec2 B ) {
    vec2  P      = ClosestPointToEdge( C, A, B );
    flo32 Result = GetLengthSq( C - P );
    return Result;
}

internal flo32
DistSqPointToTri( vec2 P, vec2 A, vec2 B, vec2 C ) {
    flo32 Result = FLT_MAX;
    Result = MinValue( Result, DistSqPointToEdge( P, A, B ) );
    Result = MinValue( Result, DistSqPointToEdge( P, B, C ) );
    Result = MinValue( Result, DistSqPointToEdge( P, C, A ) );
    
    return Result;
}

internal flo32
DistPointToTri( vec2 P, vec2 A, vec2 B, vec2 C ) {
    flo32 Result = sqrtf( DistSqPointToTri( P, A, B, C ) );
    return Result;
}

internal flo32
DistPointToRect( vec2 P, rect R ) {
    vec2 ClosestPoint = P;
    ClosestPoint.x = MaxValue( ClosestPoint.x, R.Left   );
    ClosestPoint.x = MinValue( ClosestPoint.x, R.Right  );
    ClosestPoint.y = MaxValue( ClosestPoint.y, R.Bottom );
    ClosestPoint.y = MinValue( ClosestPoint.y, R.Top    );
    
    flo32 Result = GetLength( P - ClosestPoint );
    return Result;
}

internal flo32
DistPointToRectPerimeter( vec2 P, rect R ) {
    flo32 Result = FLT_MAX;
    if( IsInRect( P, R ) ) {
        Result = MinValue( fabsf( P.x - R.Left   ), Result );
        Result = MinValue( fabsf( P.y - R.Bottom ), Result );
        Result = MinValue( fabsf( P.x - R.Right  ), Result );
        Result = MinValue( fabsf( P.y - R.Top    ), Result );
    } else {
        Result = DistPointToRect( P, R );
    }
    
    return Result;
}

internal flo32
DistPointToORect( vec2 P, orect R ) {
    flo32 Result = 0.0f;
    if( !IsInORect( P, R ) ) {
        vec2  V     = P - R.Center;
        flo32 DistX = fabsf( dot( V, R.xAxis ) ) - R.HalfDim.x;
        flo32 DistY = fabsf( dot( V, R.yAxis ) ) - R.HalfDim.y;
        Result = sqrtf( DistX * DistX + DistY * DistY );
    }
    
    return Result;
}

internal boo32
IsInEllipse( vec2 P, vec2 Ellipse_Center, vec2 Ellipse_Radius, flo32 Ellipse_Radians ) {
    boo32 Result = false;
    
    vec2  xAxis       = ToDirection2D( Ellipse_Radians );
    flo32 AspectRatio = Ellipse_Radius.y / Ellipse_Radius.x;
    flo32 Radius      = Ellipse_Radius.y;
    flo32 RadiusSq    = Radius * Radius;
    
    P += xAxis * ( dot( P - Ellipse_Center, xAxis ) * ( AspectRatio - 1.0f ) );
    flo32 DistSq = GetLengthSq( P - Ellipse_Center );
    if( DistSq <= RadiusSq ) {
        Result = true;
    }
    
    return Result;
}

internal boo32
IsInCapsule( vec2 Pos, CAPSULE2 Cap ) {
    boo32 Result = false;
    
    vec2  P = ClosestPointToEdge( Pos, Cap.P, Cap.Q );
    flo32 DistSq   = GetLengthSq( Pos - P );
    flo32 RadiusSq = Cap.Radius * Cap.Radius;
    if( DistSq <= RadiusSq ) {
        Result = true;
    }
    return Result;
}

internal vec2
ClosestPointToTriPerimeter( vec2 Pos, vec2 A, vec2 B, vec2 C ) {
    flo32 MaxDist = FLT_MAX;
    vec2  Result  = {};
    
    vec2 Point[  ] = {
        A,
        B,
        C,
        A,
    };
    
    for( uint32 iEdge = 0; iEdge < 3; iEdge++ ) {
        vec2  P    = ClosestPointToEdge( Pos, Point[ iEdge ], Point[ iEdge + 1 ] );
        flo32 Dist = GetLength( P - Pos );
        if( Dist < MaxDist ) {
            MaxDist = Dist;
            Result  = P;
        }
    }
    return Result;
}

internal vec2
ClosestPointToRectPerimeter( vec2 Pos, rect Bound ) {
    flo32 MaxDist = FLT_MAX;
    vec2  Result  = {};
    
    vec2 Point[  ] = {
        GetBL( Bound ),
        GetTL( Bound ),
        GetTR( Bound ),
        GetBR( Bound ),
        GetBL( Bound ),
    };
    
    for( uint32 iEdge = 0; iEdge < 4; iEdge++ ) {
        vec2  P    = ClosestPointToEdge( Pos, Point[ iEdge ], Point[ iEdge + 1 ] );
        flo32 Dist = GetLength( P - Pos );
        if( Dist < MaxDist ) {
            MaxDist = Dist;
            Result  = P;
        }
    }
    return Result;
}

internal boo32
DoIntersectEdgeEdge( vec2 A, vec2 B, vec2 C, vec2 D ) {
    boo32 Result = false;
    
    vec2 AB = B - A;
    vec2 CD = D - C;
    vec2 AC = C - A;
    
    vec2 N = GetNormal( GetPerp( CD ) );
    
    flo32 Denom = dot( N, AB );
    if( Denom != 0.0f ) {
        flo32 t = dot( N, C - A ) / Denom;
        
        if( ( t >= 0.0f ) && ( t <= 1.0f ) ) {
            vec2  P = A + AB * t;
            flo32 s = dot( P - C, CD ) / dot( CD, CD );
            
            if( ( s >= 0.0f ) && ( s <= 1.0f ) ) {
                Result = true;
            }
        }
    }
    return Result;
}

internal boo32
DoIntersectEdgeRect( vec2 A, vec2 B, rect R ) {
    vec2 BL = GetBL( R );
    vec2 TL = GetTL( R );
    vec2 TR = GetTR( R );
    vec2 BR = GetBR( R );
    
    boo32 Result = ( DoIntersectEdgeEdge( A, B, BL, TL ) )
        || ( DoIntersectEdgeEdge( A, B, TL, TR ) )
        || ( DoIntersectEdgeEdge( A, B, TR, BR ) )
        || ( DoIntersectEdgeEdge( A, B, BR, BL ) );
    
    return Result;
}