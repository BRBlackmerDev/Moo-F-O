
internal MESH
LoadXFile( READ_FILE * ReadFile, MEMORY * PermMemory, MEMORY * TempMemory, char * SaveDir, char * FileName, MODEL_ID MeshID ) {
    MESH Result = {};
    
	FILE_DATA File = ReadFile( TempMemory, SaveDir, FileName, "x" );
	if( File.Contents ) {
        FILE_PARSER  _Parser = FileParser( File.Contents, File.Size );
        FILE_PARSER * Parser = &_Parser;
        
        { // Verify Header
            STRING Str = ParseToken( Parser );
            Assert( MatchString( Str, "xof" ) );
            
            Str = ParseToken( Parser );
            Assert( MatchString( Str, "0303txt" ) );
            
            Str = ParseToken( Parser );
            Assert( MatchString( Str, "0032" ) );
        }
        
        { // Remove Comments, Commas, Semicolons
            boo32 DoComment = false;
            
            char * At = Parser->At;
            while( ( At - Parser->Start ) < Parser->Size ) {
                if( DoComment ) {
                    if( At[ 0 ] == '\n' ) {
                        DoComment = false;
                    } else {
                        At[ 0 ] = ' ';
                    }
                } else {
                    if( ( At[ 0 ] == ',' ) || ( At[ 0 ] == ';' ) ){
                        At[ 0 ] = ' ';
                    } else if( At[ 0 ] == '/' ) {
                        if( At[ 1 ] == '/' ) {
                            DoComment = true;
                            At[ 0 ] = ' ';
                        }
                    }
                }
                
                At++;
            }
        }
        
        vec3  * Pos   = 0;
        vec2  * UV    = 0;
        int32 * Index = 0;
        
        int32 nPos = 0;
        int32 nUV  = 0;
        int32 nFaceIndex = 0;
        
        int32 NestLevel = 0;
        
        while( ( Parser->At - Parser->Start ) < Parser->Size ) {
            STRING Token = ParseToken( Parser );
            STRING Str   = {};
            
            if( MatchString( Token, "{" ) ) {
                NestLevel += 1;
            } else if( MatchString( Token, "}" ) ) {
                NestLevel -= 1;
                Assert( NestLevel >= 0 );
            } else if( MatchString( Token, "FrameTransformMatrix" ) ) {
                Str = ParseToken( Parser );
                Assert( MatchString( Str, "{" ) );
                
                for( int32 iElem = 0; iElem < 16; iElem++ ) {
                    flo32 Value = ParseF32( Parser );
                }
                
                Str = ParseToken( Parser );
                Assert( MatchString( Str, "}" ) );
            } else if( MatchString( Token, "Mesh" ) ) {
                Str = ParseToken( Parser );
                Assert( MatchString( Str, "{" ) );
                
                nPos = ParseS32( Parser );
                Pos = _PushArray( TempMemory, vec3, nPos );
                
                for( int32 iPos = 0; iPos < nPos; iPos++ ) {
                    flo32 X = ParseF32( Parser );
                    flo32 Y = ParseF32( Parser );
                    flo32 Z = ParseF32( Parser );
                    
                    Pos[ iPos ] = Vec3( X, Y, Z );
                }
                
                Result.nVertex = nPos;
                
                int32 nFace = ParseS32( Parser );
                nFaceIndex = nFace * 2 * 3;
                // NOTE : We are assuming that every Face we read has 4 indices, just for the memory size.
                
                Index = _PushArray( TempMemory, int32, nFaceIndex );
                
                int32 AtIndex = 0;
                
                for( int32 iFace = 0; iFace < nFace; iFace++ ) {
                    int32 nIndex = ParseS32( Parser );
                    
                    if( nIndex == 4 ) {
                        int32 I0 = ParseS32( Parser );
                        int32 I1 = ParseS32( Parser );
                        int32 I2 = ParseS32( Parser );
                        int32 I3 = ParseS32( Parser );
                        
                        Index[ AtIndex++ ] = I0;
                        Index[ AtIndex++ ] = I1;
                        Index[ AtIndex++ ] = I3;
                        
                        Index[ AtIndex++ ] = I2;
                        Index[ AtIndex++ ] = I3;
                        Index[ AtIndex++ ] = I1;
                    } else if( nIndex == 3 ) {
                        Index[ AtIndex++ ] = ParseS32( Parser );
                        Index[ AtIndex++ ] = ParseS32( Parser );
                        Index[ AtIndex++ ] = ParseS32( Parser );
                    } else {
                        InvalidCodePath;
                    }
                }
                
                Result.nIndex = AtIndex;
                
                {
                    Str = ParseToken( Parser );
                    Assert( MatchString( Str, "MeshTextureCoords" ) );
                    
                    Str = ParseToken( Parser );
                    Assert( MatchString( Str, "{" ) );
                    
                    nUV = ParseS32( Parser );
                    UV = _PushArray( TempMemory, vec2, nUV );
                    
                    for( int32 iUV = 0; iUV < nUV; iUV++ ) {
                        flo32 U = ParseF32( Parser );
                        flo32 V = ParseF32( Parser );
                        
                        UV[ iUV ] = Vec2( U, 1.0f - V );
                    }
                    
                    Str = ParseToken( Parser );
                    Assert( MatchString( Str, "}" ) );
                    
                    Result.nUV = nUV;
                }
                
                
                Str = ParseToken( Parser );
                Assert( MatchString( Str, "}" ) );
            } else if( MatchString( Token, "Frame" ) ) {
                STRING Name = ParseToken( Parser );
            } else {
                InvalidCodePath;
            }
        }
        
        Assert( NestLevel == 0 );
        
        Result.Vertex  = _PushArray( PermMemory, vec3,  Result.nVertex );
        Result.UV      = _PushArray( PermMemory, vec2,  Result.nUV     );
        Result.Index   = _PushArray( PermMemory, int32, Result.nIndex  );
        
        for( int32 iVertex = 0; iVertex < Result.nVertex; iVertex++ ) {
            Result.Vertex[ iVertex ] = Pos[ iVertex ];
        }
        for( int32 iUV = 0; iUV < Result.nUV; iUV++ ) {
            Result.UV[ iUV ] = UV[ iUV ];
        }
        for( int32 iIndex = 0; iIndex < Result.nIndex; iIndex++ ) {
            Result.Index[ iIndex ] = Index[ iIndex ];
        }
        
        _PopArray( TempMemory, vec3,  nPos       );
        _PopArray( TempMemory, vec2,  nUV        );
        _PopArray( TempMemory, int32, nFaceIndex );
        _PopSize ( TempMemory, File.Size );
    }
    
    return Result;
}