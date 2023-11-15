
//----------
// SAVE LEVEL
//----------

internal void
GetFileList( STRING_BUFFER * List, char * FileTag ) {
    char WorkingDir[ 512 + 1 ] = {};
    GetCurrentDirectory( 512, WorkingDir );
    SetCurrentDirectory( LEVEL_SAVE_DIRECTORY );
    
    uint32 Length = ( uint32 )strlen( FileTag );
    Reset( List );
    
    WIN32_FIND_DATA File_Data   = {};
    HANDLE          File_Handle = FindFirstFile( "*", &File_Data ); // .
    FindNextFile( File_Handle, &File_Data ); // ..
    while( FindNextFile( File_Handle, &File_Data ) ) {
        char * ptr = File_Data.cFileName;
        
        uint32 len = ( uint32 )strlen( ptr );
        if( len > Length ) {
            while( *ptr ) { ptr++; }   ptr -= Length;
            
            if( MatchSegment( FileTag, ptr, Length ) ) {
                ptr--;
                ptr[ 0 ] = 0;
                
                char * Status = AddString( List, File_Data.cFileName );
                if( !Status ) {
                    GlobalVar_DebugSystem.ErrorOccurred = true;
                    char str[ 512 ] = {};
                    sprintf( str, "ERROR! FILE LIST ran out of space in STRING_BUFFER for storing FileNames! Max Count = %u", List->nStrMax );
                    CONSOLE_STRING( str );
                }
            }
        }
    }
    
    SetCurrentDirectory( WorkingDir );
}

internal void
OutputEntitiesToWaveBitmap( WAVE Wave, rect World_Bound, uint32 nPixelPerUnit, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    for( uint32 iEntity = 0; iEntity < Wave.nEntity; iEntity++ ) {
        ENTITY Entity = Wave.Entity[ iEntity ];
        
        switch( Entity.Type ) {
            case EntityType_Turret: {
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius ), Pixel, xPixel, yPixel, COLOR_BLUE );
                OutputShieldToWaveBitmap( World_Bound, Entity.Shield, Entity.Position, Entity.Radius, Pixel, xPixel, yPixel );
            } break;
            
            case EntityType_Chaser: {
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius ), Pixel, xPixel, yPixel, Vec4( 0.0f, 0.6f, 0.6f, 1.0f ) );
            } break;
            
            case EntityType_Swarm: {
                vec4 Color = COLOR_YELLOW;
                rect Bound = RectCR( Entity.Position, Entity.Radius );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForWaveBitmap( Bound, World_Bound, xPixel, yPixel );
                OutputTriangleToWaveBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
            } break;
            
            case EntityType_Drifter: {
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius ), Pixel, xPixel, yPixel, Vec4( 0.65f, 0.275f, 0.0f, 1.0f ) );
            } break;
            
            case EntityType_Looper: {
                vec4 Color = Vec4( 0.75f, 0.15f, 0.35f, 1.0f );
                rect Bound = RectCR( Entity.Position, Entity.Radius );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForWaveBitmap( Bound, World_Bound, xPixel, yPixel );
                OutputRectToWaveBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
            } break;
            
            case EntityType_Starburst: {
                vec4  Color = ToColor( 255, 110, 145 );
                orect Bound = ORectCR( Entity.Position, Entity.Radius, PI * 0.25f );
                OutputORectToWaveBitmap( World_Bound, Bound, Pixel, xPixel, yPixel, Color );
            } break;
            
            case EntityType_Miner: {
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius ), Pixel, xPixel, yPixel, COLOR_GRAY( 0.08f ) );
                
                flo32 dRadians = TAU / ( flo32 )Entity.nEntity;
                for( int32 iLaser = 0; iLaser < Entity.nEntity; iLaser++ ) {
                    flo32 Radians = dRadians * ( flo32 )iLaser;
                    vec2  Dim     = Vec2( Entity.Radius * 0.25f, Entity.Radius );
                    orect Bound   = ORectBCD( Entity.Position, Dim, Radians );
                    
                    OutputORectToWaveBitmap( World_Bound, Bound, Pixel, xPixel, yPixel, COLOR_RED );
                }
            } break;
            
            case EntityType_Bomber: {
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius ), Pixel, xPixel, yPixel, COLOR_GRAY( 0.08f ) );
                
                flo32 dRadians = TAU / ( flo32 )Entity.nEntity;
                for( int32 iBomb = 0; iBomb < Entity.nEntity; iBomb++ ) {
                    flo32 Radians = dRadians * ( flo32 )iBomb;
                    vec2  Pos     = Entity.Position + ToDirection2D( Radians ) * ( Entity.Radius * 0.8f );
                    
                    OutputCircleToWaveBitmap( World_Bound, Circ( Pos, Entity.Radius * 0.25f ), Pixel, xPixel, yPixel, COLOR_RED );
                }
            } break;
            
            case EntityType_Bumper: {
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius ), Pixel, xPixel, yPixel, ToColor( 100, 0, 100 ) );
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius * 0.8f ), Pixel, xPixel, yPixel, COLOR_BLACK );
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius * 0.3f ), Pixel, xPixel, yPixel, ToColor( 100, 0, 100 ) );
            } break;
            
            case EntityType_Bank: {
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius ), Pixel, xPixel, yPixel, BANK_BASE_COLOR );
                OutputCircleToWaveBitmap( World_Bound, Circ( Entity.Position, Entity.Radius - 0.2f ), Pixel, xPixel, yPixel, COLOR_BLACK );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
    }
}

internal void
SaveBitmapForWave( PLATFORM * Platform, MEMORY * TempMemory, char * SaveDir, char * FileName, rect World_Bound, int32 Guid, WAVE Wave ) {
    vec2 World_Dim = GetDim( World_Bound );
    
    uint32 nPixelPerUnit = ( uint32 )LAYOUT_BITMAPS_PIXELS_PER_UNIT;
    
    uint32 xUnit = ( uint32 )( World_Dim.x );
    uint32 yUnit = ( uint32 )( World_Dim.y );
    uint32 nUnit = xUnit * yUnit;
    
    if( ( xUnit > 0 ) && ( yUnit > 0 ) ) {
        uint32 xPixel = xUnit * nPixelPerUnit;
        uint32 yPixel = yUnit * nPixelPerUnit;
        uint32 nPixel = xPixel * yPixel;
        
        uint32   Output_Size = sizeof( BMP_HEADER ) + sizeof( uint32 ) * nPixel;
        MEMORY  _Output      = SubArena( TempMemory, Output_Size );
        MEMORY * Output      = &_Output;
        
        BMP_HEADER * Header = _PushType( Output, BMP_HEADER );
        Header->FileType     = 0x4d42;
        Header->FileSize     = Output_Size;
        Header->OffsetToData = sizeof( BMP_HEADER );
        Header->BitmapInfoHeaderSize = 40;
        Header->Width        = xPixel;
        Header->Height       = yPixel;
        Header->Planes       = 1;
        Header->BitsPerPixel = 32;
        
        uint32 * Pixel = _PushArray( Output, uint32, nPixel );
        for( uint32 iPixel = 0; iPixel < nPixel; iPixel++ ) {
            Pixel[ iPixel ] = 0xFF000000;
        }
        
        OutputEntitiesToWaveBitmap( Wave, World_Bound, nPixelPerUnit, Pixel, xPixel, yPixel );
        
        Assert( Guid > -1 );
        
        char Name[ 128 + 1 ] = {};
        sprintf( Name, "%s-%04u", FileName, Guid );
        
        OutputFile( Platform, Output, SaveDir, Name, "bmp" );
        _PopSize( TempMemory, Output_Size );
    }
}

internal void
SaveBitmaps( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    EDITOR_STATE   * Editor = &AppState->Editor;
    FILE_SAVE_OPEN * Save   = &Editor->SaveOpen;
    
    char * SaveDir  = LEVEL_SAVE_DIRECTORY;
    char * FileName = Save->CurrentFile;
    
    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
    
    if( WaveS->nWave > 0 ) {
        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WAVE Wave = WaveS->Wave[ iWave ];
            SaveBitmapForWave( Platform, TempMemory, SaveDir, FileName, Editor->World_Bound, Wave.Guid, Wave );
        }
    }
}