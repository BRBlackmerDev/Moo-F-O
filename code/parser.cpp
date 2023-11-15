
struct FILE_PARSER {
    char * Start;
    char * At;
    int32  Size;
};

internal FILE_PARSER
FileParser( void * Memory, int32 Size ) {
    FILE_PARSER Result = {};
    Result.Start = ( char * )Memory;
    Result.At    = Result.Start;
    Result.Size  = Size;
    return Result;
}

internal FILE_PARSER
FileParser( STRING Str ) {
    FILE_PARSER Result = {};
    Result.Start = Str.String;
    Result.At    = Str.String;
    Result.Size  = Str.nChar;
    return Result;
}

internal boo32
HasTextRemaining( FILE_PARSER * Parser ) {
    boo32 Result = ( ( Parser->At - Parser->Start ) < Parser->Size );
    return Result;
}

internal boo32
IsWhiteSpace( char c ) {
    boo32 Result = ( c == 0 ) || ( c == ' ' ) || ( c == '\n' ) || ( c == '\t' ) || ( c == '\r' );
    return Result;
}

internal void
EatWhiteSpace( FILE_PARSER * Parser ) {
    while( HasTextRemaining( Parser ) && ( IsWhiteSpace( *Parser->At ) ) ) { Parser->At++; }
}

internal void
EatNonWhiteSpace( FILE_PARSER * Parser ) {
    while( HasTextRemaining( Parser ) && ( !IsWhiteSpace( *Parser->At ) ) ) { Parser->At++; }
}

internal char *
GetEnd( char * str ) {
    Assert( ( str ) && ( str[ 0 ] ) );
    
    char * Result = str;
    while( Result[ 0 ] ) { Result++; }
    return Result;
}

internal STRING
ParseToken( FILE_PARSER * Parser ) {
    Assert( Parser->Size > 0 );
    
    STRING Result = {};
    
    EatWhiteSpace( Parser );
    
    if( HasTextRemaining( Parser ) ) {
        Result.String = Parser->At;
    }
    
    EatNonWhiteSpace( Parser );
    Result.nChar = ( uint32 )( Parser->At - Result.String );
    
    EatWhiteSpace( Parser );
    
    return Result;
}

internal STRING
ParseLine( FILE_PARSER * Parser ) {
    STRING Result = {};
    
    EatWhiteSpace( Parser );
    
    if( HasTextRemaining( Parser ) ) {
        Result.String = Parser->At;
    }
    
    while( ( *Parser->At != 0 ) && ( *Parser->At != '\n' ) && ( *Parser->At != '\r' ) ) { Parser->At++; }
    Result.nChar = ( uint32 )( Parser->At - Result.String );
    
    EatWhiteSpace( Parser );
    return Result;
}

internal uint8
ParseU8( STRING Token ) {
    uint8 Result = ( uint8 )strtoul( Token.String, 0, 0 );
    return Result;
}

internal uint8
ParseU8( FILE_PARSER * Parser ) {
    STRING Token  = ParseToken( Parser );
    uint8  Result = ParseU8( Token );
    return Result;
}

internal uint16
ParseU16( STRING Token ) {
    uint16 Result = ( uint16 )strtoul( Token.String, 0, 0 );
    return Result;
}

internal uint32
ParseU32( STRING Token ) {
    uint32 Result = strtoul( Token.String, 0, 0 );
    return Result;
}

internal uint32
ParseU32( FILE_PARSER * Parser ) {
    STRING Token  = ParseToken( Parser );
    uint32 Result = ParseU32( Token );
    return Result;
}

internal int32
ParseS32( STRING Token ) {
    int32  Result = strtol( Token.String, 0, 0 );
    return Result;
}

internal int32
ParseS32( FILE_PARSER * Parser ) {
    STRING Token  = ParseToken( Parser );
    int32  Result = ParseS32( Token );
    return Result;
}

internal flo32
ParseF32( STRING Token ) {
    flo32  Result = strtof( Token.String, 0 );
    return Result;
}

internal flo32
ParseF32( FILE_PARSER * Parser ) {
    STRING Token  = ParseToken( Parser );
    flo32  Result = ParseF32( Token );
    return Result;
}

internal vec2
ParseV2( STRING Token ) {
    FILE_PARSER Parser = FileParser( Token.String, Token.nChar );
    
    vec2 Result = {};
    for( uint32 iElem = 0; iElem < 2; iElem++ ) {
        Result.elem[ iElem ] = ParseF32( &Parser );
    }
    return Result;
}

internal vec3
ParseV3( STRING Token ) {
    FILE_PARSER Parser = FileParser( Token.String, Token.nChar );
    
    vec3 Result = {};
    for( uint32 iElem = 0; iElem < 3; iElem++ ) {
        Result.elem[ iElem ] = ParseF32( &Parser );
    }
    return Result;
}

internal vec4
ParseV4( STRING Token ) {
    FILE_PARSER Parser = FileParser( Token.String, Token.nChar );
    
    vec4 Result = {};
    for( uint32 iElem = 0; iElem < 4; iElem++ ) {
        Result.elem[ iElem ] = ParseF32( &Parser );
    }
    return Result;
}

internal quat
ParseQuat( STRING Token ) {
    FILE_PARSER Parser = FileParser( Token.String, Token.nChar );
    
    quat Result = {};
    for( uint32 iElem = 0; iElem < 4; iElem++ ) {
        Result.elem[ iElem ] = ParseF32( &Parser );
    }
    return Result;
}