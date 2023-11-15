

#include <math.h> // sqrtf, cosf, sinf
#include <stdint.h> // Type definitions
#include <stdlib.h> // rand
#include <time.h> // time
#include <float.h> // FLT_MAX
#include <cstdio> // sprintf
#include <shlwapi.h>
#include <windows.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_trueType.h"

#include "TypeDef.h"
#include "math.cpp"

#define 	STD_INCLUDE_DECL
#include "Vector.cpp"
#include "Memory.cpp"
#include "Font.cpp"
#undef	STD_INCLUDE_DECL

#define	STD_INCLUDE_FUNC
#include "Vector.cpp"
#include "Memory.cpp"
#include "Font.cpp"
#undef	STD_INCLUDE_FUNC

#include "Shmup_Config.h"
#include "AssetID.h"

#define AUDIO__BYTES_PER_SAMPLE    ( 2 )
#define AUDIO__CHANNEL_COUNT       ( 2 )

struct AUDIO_DATA {
    uint8 * Audio;
    int32  nSamples;
};

#include "win32_platform.h"
#include "Entity.h"
#include "AssetPackerTools.cpp"
#include "GenMesh.cpp"
#include "Parser.cpp"
#include "X.cpp"
#include "wav.cpp"

struct ASSET_OUTPUT {
    MEMORY * Data_Output;
    MEMORY * AssetID_Output;
    
    int32 nTexture;
    int32 nMesh;
    int32 nLevel;
    int32 nAudio;
};

struct ASSET_TEXTURE {
#define ASSET_TEXTURE_NAME_CHAR_MAX_COUNT  ( 64 )
    char     Name[ ASSET_TEXTURE_NAME_CHAR_MAX_COUNT ];
    int32    Width;
    int32    Height;
    uint32 * Texture_Data;
};

internal void
AddTextureID( MEMORY * AssetID_Output, char * TextureID ) {
    WriteSegment( AssetID_Output, "   TextureID_" );
    WriteSegment( AssetID_Output, TextureID );
    WriteSegment( AssetID_Output, ",\n" );
}

internal void
AddTexture( ASSET_OUTPUT * AssetOutput, char * TextureID, int32 Width, int32 Height, uint32 * Texture_Data ) {
    _writem( AssetOutput->Data_Output, int32, Width  );
    _writem( AssetOutput->Data_Output, int32, Height );
    
    if( Texture_Data ) {
        _writeb( AssetOutput->Data_Output, Texture_Data, Width * Height * sizeof( uint32 ) );
    }
    
    AddTextureID( AssetOutput->AssetID_Output, TextureID );
    
    AssetOutput->nTexture += 1;
}

internal FILE_DATA
ReadFile( MEMORY * Memory, char * File_Path ) {
    FILE_DATA Result = {};
    
    FILE * File = fopen( File_Path, "rb" );
    if( File ) {
        fseek( File, 0, SEEK_END );
        int32 File_Bytes = ftell( File );
        fseek( File, 0, SEEK_SET );
        
        uint8 * Dest = ( uint8 * )_PushSize( Memory, File_Bytes );
        int32 BytesRead = ( int32 )fread( Dest, 1, File_Bytes, File );
        if( File_Bytes != BytesRead ) {
            printf( "ERROR! Error reading file!  ( File_Bytes != BytesRead )" );
        } else {
            Result.Contents = Dest;
            Result.Size     = File_Bytes;
        }
        
        fclose( File );
    }
    
    return Result;
}

internal FILE_DATA
ReadFile( MEMORY * Memory, char * SaveDir, char * FileName, char * Extension ) {
	char FilePath[ 2048 ] = {};
    GenFilePath( FilePath, SaveDir, FileName, Extension );
    
    FILE_DATA Result = ReadFile( Memory, FilePath );
    return Result;
}

internal void
WriteFile( char * FilePath, void * Data, uint64 Size ) {
	FILE * File = fopen( FilePath, "wb" );
	if( File ) {
        char Str[ 1024 ] = {};
        
		int32 BytesWritten = ( int32 )fwrite( Data, 1, Size, File );
		if( BytesWritten != Size ) {
			sprintf( Str, "ERROR. Unable to write File: %s", FilePath );
		} else {
            sprintf( Str, "SUCCESS. Wrote File: %s", FilePath );
        }
        printf( Str );
        printf( "\n" );
        
        fclose( File );
    }
}

#pragma pack( push, 1 )
struct BMP_HEADER {
	uint16 FileType;             // MUST be 0x4d42
	uint32 FileSize;
	uint16 Reserved01;           // UNUSED
	uint16 Reserved02;           // UNUSED
	uint32 OffsetToData;         // sizeof( BMP_HEADER )
	uint32 BitmapInfoHeaderSize; // MUST be 40
	uint32 Width;
	uint32 Height;
	uint16 Planes;               // MUST be 1
	uint16 BitsPerPixel;
	uint32 CompressionType;      // UNUSED
	uint32 BitmapSize_InBytes;   // UNUSED
	uint32 xPixelsPerMeter;      // UNUSED
	uint32 yPixelsPerMeter;      // UNUSED
	uint32 ColorCount;           // UNUSED
	uint32 ImportantColorCount;  // UNUSED
};
#pragma pack( pop )

internal void
AddBMP( ASSET_OUTPUT * AssetOutput, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char FilePath[ 2048 ] = {};
    GenFilePath( FilePath, SaveDir, FileName, "bmp" );
    
    FILE_DATA File = ReadFile( TempMemory, FilePath );
    if( File.Contents ) {
		BMP_HEADER * Info = ( BMP_HEADER * )File.Contents;
		int32 Width         = Info->Width;
		int32 Height        = Info->Height;
		int32 BytesPerPixel = 4;
		
		uint32 BMP_BytesPerPixel = Info->BitsPerPixel / 8;
		
		uint8 * BMP_Data = ( uint8 * )File.Contents + Info->OffsetToData;
		uint8 * Ptr = BMP_Data;
		
		uint32 nPixel      = Width * Height;
		uint32 TextureSize = nPixel * BytesPerPixel;
		uint8 * TexelData = ( uint8 * )_PushSize( TempMemory, TextureSize );
		
		flo32 Inv255 = 1.0f / 255.0f;
		uint32 * ResultData  = ( uint32 * )TexelData;
        
        if( BMP_BytesPerPixel == 4 ) {
            uint32 * TextureData = ( uint32 * )BMP_Data;
            for( uint32 Iter = 0; Iter < nPixel; Iter++ ) {
                // TODO: RGBA vs BGRA
                uint32 InColor = TextureData[ Iter ];
                
                uint8 r = ( uint8 )( ( InColor >>  0 ) & 0xFF );
                uint8 g = ( uint8 )( ( InColor >>  8 ) & 0xFF );
                uint8 b = ( uint8 )( ( InColor >> 16 ) & 0xFF );
                uint8 a = ( uint8 )( ( InColor >> 24 ) & 0xFF );
                flo32 Alpha = ( flo32 )a * Inv255;
                
                uint32 OutColor =
                    ( ( uint8 )( ( flo32 )b * Alpha ) <<  0 ) |
                    ( ( uint8 )( ( flo32 )g * Alpha ) <<  8 ) |
                    ( ( uint8 )( ( flo32 )r * Alpha ) << 16 ) |
                    ( a << 24 );
                
                ResultData[ Iter ] = OutColor;
            }
        } else if( BMP_BytesPerPixel == 3 ) {
            uint8 * TextureData = BMP_Data;
            for( uint32 Iter = 0; Iter < nPixel; Iter++ ) {
                // TODO: RGBA vs BGRA
                uint8 r = *TextureData++;
                uint8 g = *TextureData++;
                uint8 b = *TextureData++;
                uint8 a = 255;
                
                uint32 OutColor = ( b <<  0 ) | ( g <<  8 ) | ( r << 16 ) | ( a << 24 );
                
                ResultData[ Iter ] = OutColor;
            }
        }
        
        AddTexture( AssetOutput, FileName, Width, Height, ResultData );
		
        _PopSize( TempMemory, TextureSize );
		_PopSize( TempMemory, File.Size );
	}
}

internal WAVE
ReadWave( uint32 Version, uint8 ** Ptr ) {
    WAVE Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            WAVE Wave = {};
            Wave.Guid             = _read( ptr, int32 );
            Wave.Layout_TextureID = _read( ptr, TEXTURE_ID );
            Wave.Layout_Position  = _read( ptr, vec2 );
            
            Wave.nRepeatWave = _read( ptr, int32 );
            
            Wave.Type = _read( ptr, WAVE_TYPE );
            Wave.Exit = _read( ptr, EXIT_TYPE );
            
            Wave.nKills               = _read( ptr, int32 );
            Wave.nTotalEntitiesInWave = _read( ptr, int32 );
            
            Wave.nEntity = _read( ptr, uint32 );
            
            for( uint32 iEntity = 0; iEntity < WAVE_ENTITY_MAX_COUNT; iEntity++ ) {
                ENTITY Entity = {};
                Entity.WaveGuid = _read( ptr, int32 );
                Entity.Type     = _read( ptr, ENTITY_TYPE );
                Entity.nEntity  = _read( ptr, int32 );
                Entity.Position = _read( ptr, vec2 );
                Entity.Radius   = _read( ptr, flo32 );
                
                Entity.Shield = _read( ptr, ENTITY_SHIELD );
                
                Wave.Entity[ iEntity ] = Entity;
            }
            
            Wave.Exit_Timer          = _read( ptr, flo32 );
            Wave.Exit_TargetTime     = _read( ptr, flo32 );
            int32 Exit_Minutes       = _read( ptr, int32 );
            int32 Exit_Seconds       = _read( ptr, int32 );
            
            Wave.InDelay_Timer       = _read( ptr, flo32 );
            Wave.InDelay_TargetTime  = _read( ptr, flo32 );
            int32 InDelay_Minutes    = _read( ptr, int32 );
            int32 InDelay_Seconds    = _read( ptr, int32 );
            
            Wave.DoOutDelay          = _read( ptr, boo32 );
            Wave.OutDelay_Timer      = _read( ptr, flo32 );
            Wave.OutDelay_TargetTime = _read( ptr, flo32 );
            int32 OutDelay_Minutes   = _read( ptr, int32 );
            int32 OutDelay_Seconds   = _read( ptr, int32 );
            
            Wave.nNextWave = _read( ptr, uint32 );
            for( uint32 iNextWave = 0; iNextWave < NEXT_WAVE_MAX_COUNT; iNextWave++ ) {
                Wave.NextWave     [ iNextWave ] = _read( ptr, int32 );
            }
            for( uint32 iNextWave = 0; iNextWave < NEXT_WAVE_MAX_COUNT; iNextWave++ ) {
                Wave.NextWave_Guid[ iNextWave ] = _read( ptr, int32 );
            }
            
            Wave.nKillWave = _read( ptr, uint32 );
            for( uint32 iKillWave = 0; iKillWave < KILL_WAVE_MAX_COUNT; iKillWave++ ) {
                Wave.KillWave_Guid[ iKillWave ] = _read( ptr, int32 );
            }
            
            Result = Wave;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

struct ENTITY_FILE_HEADER {
    char EntityTag[ 8 ];
    char Version  [ 4 ];
};

internal uint32
VerifyEntityHeaderAndGetVersion( uint8 ** ptr, char * FileTag ) {
    ENTITY_FILE_HEADER * header = _addr( *ptr, ENTITY_FILE_HEADER );
    Assert( MatchSegment( header->EntityTag, FileTag, 8 ) );
    
    char str[ 4 + 1 ] = {};
    memcpy( str, header->Version, 4 );
    
    uint32 Result = strtoul( str, 0, 0 );
    return Result;
}

internal void
PackMesh( ASSET_OUTPUT * AssetOutput, MODEL_ID MeshID, MESH Mesh ) {
    _writem( AssetOutput->Data_Output, MODEL_ID, MeshID );
    
    _writem( AssetOutput->Data_Output, int32, Mesh.nVertex );
    _writem( AssetOutput->Data_Output, int32, Mesh.nIndex  );
    
    for( int32 iVertex = 0; iVertex < Mesh.nVertex; iVertex++ ) {
        _writem( AssetOutput->Data_Output, vec3, Mesh.Vertex[ iVertex ] );
        _writem( AssetOutput->Data_Output, vec2, Mesh.UV    [ iVertex ] );
    }
    
    _writea( AssetOutput->Data_Output, Mesh.Index, int32, Mesh.nIndex );
    
    AssetOutput->nMesh += 1;
}

internal void
PackGeneratedMesh( ASSET_OUTPUT * AssetOutput, MEMORY * TempMemory, MODEL_ID MeshID, GEN_MESH * GenMesh ) {
    MESH Mesh = GenMesh( TempMemory );
    
    PackMesh( AssetOutput, MeshID, Mesh );
    
    _PopArray( TempMemory, vec3,  Mesh.nVertex );
    _PopArray( TempMemory, vec2,  Mesh.nUV     );
    _PopArray( TempMemory, int32, Mesh.nIndex  );
}

internal void
PackXFile( ASSET_OUTPUT * AssetOutput, READ_FILE * ReadFile, MEMORY * TempMemory, char * SaveDir, char * FileName, MODEL_ID MeshID ) {
    int32  Memory_Size = _MB( 1 );
    MEMORY Memory      = SubArena( TempMemory, Memory_Size );
    MESH   Mesh   = LoadXFile( ReadFile, &Memory, TempMemory, SaveDir, FileName, MeshID );
    
    PackMesh( AssetOutput, MeshID, Mesh );
    
    _PopSize ( TempMemory, Memory_Size );
}

internal void
PackAudio( ASSET_OUTPUT * AssetOutput, AUDIO_ID AudioID, AUDIO_DATA Audio ) {
    _writem( AssetOutput->Data_Output, AUDIO_ID, AudioID );
    _writem( AssetOutput->Data_Output, int32, Audio.nSamples );
    
    int32 SampleSize = AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT;
    _writeb( AssetOutput->Data_Output, Audio.Audio, Audio.nSamples * SampleSize );
    
    AssetOutput->nAudio += 1;
}

internal void
PackWavFile( ASSET_OUTPUT * AssetOutput, READ_FILE * ReadFile, MEMORY * TempMemory, char * SaveDir, char * FileName, AUDIO_ID AudioID ) {
    int32  Memory_Size = _MB( 50 );
    MEMORY Memory      = SubArena( TempMemory, Memory_Size );
    
    AUDIO_DATA Audio = WAV_ReadFile( ReadFile, &Memory, TempMemory, SaveDir, FileName );
    
    PackAudio( AssetOutput, AudioID, Audio );
    
    _PopSize( TempMemory, Memory_Size );
}

internal void
PackLevel( ASSET_OUTPUT * AssetOutput, MEMORY * TempMemory, char * SaveDir, char * FileName, LEVEL_ID LevelID ) {
    if( FileName ) {
        char * FileTag = FILETAG__LEVEL;
        char FilePath[ 2048 ] = {};
        GenFilePath( FilePath, SaveDir, FileName, FileTag );
        
        FILE_DATA File = ReadFile( TempMemory, FilePath );
        if( File.Contents ) {
            uint8 * ptr = ( uint8 * )File.Contents;
            
            uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
            
            _writem( AssetOutput->Data_Output, LEVEL_ID, LevelID );
            
            int32  CurrentGuid  = _read( ptr, int32 );
            uint32 StartingWave = _read( ptr, uint32 );
            
            _writem( AssetOutput->Data_Output, uint32, StartingWave );
            
            // NOTE : Notice that LEVEL_SETTINGS is being read from the level file, but only World_YUnits is being packed.
            LEVEL_SETTINGS Level_Settings = _read( ptr, LEVEL_SETTINGS );
            _writem( AssetOutput->Data_Output, flo32, Level_Settings.World_YUnits );
            
            uint32 nWave = _read( ptr, uint32 );
            _writem( AssetOutput->Data_Output, uint32, nWave );
            
            for( uint32 iWave = 0; iWave < nWave; iWave++ ) {
                WAVE Src = ReadWave( 1, &ptr );
                _writem( AssetOutput->Data_Output, WAVE, Src );
            }
            
            _PopSize( TempMemory, File.Size );
            
            AssetOutput->nLevel += 1;
        }
    }
}

int32 main() {
    int32    Data_Output_Size = _MB( 512 );
    MEMORY  _Data_Output      = Memory( calloc( Data_Output_Size, 1 ), Data_Output_Size );
    MEMORY * Data_Output      = &_Data_Output;
    
    int32    AssetID_Output_Size = _MB( 64 );
    MEMORY  _AssetID_Output      = Memory( calloc( AssetID_Output_Size, 1 ), AssetID_Output_Size );
    MEMORY * AssetID_Output      = &_AssetID_Output;
    
    int32    TempMemory_Size = _MB( 256 );
    MEMORY  _TempMemory      = Memory( calloc( TempMemory_Size, 1 ), TempMemory_Size );
    MEMORY * TempMemory      = &_TempMemory;
    
    ASSET_OUTPUT  _AssetOutput  = {};
    ASSET_OUTPUT * AssetOutput  = &_AssetOutput;
    AssetOutput->Data_Output    = Data_Output;
    AssetOutput->AssetID_Output = AssetID_Output;
    
    int32 * nTexture = _PushType( Data_Output, int32 );
    
    WriteSegment( AssetID_Output, "\n" );
    WriteSegment( AssetID_Output, "enum TEXTURE_ID {\n" );
    
    { // create and Upload default Debug Texture
        uint32 Width  = 256;
        uint32 Height = 256;
        uint32 nTexel = Width * Height;
        
        uint32 * DefaultTexture = _PushArray_Clear( TempMemory, uint32, nTexel );
        for( uint32 iter = 0; iter < nTexel; iter++ ) {
            uint32 iRow = ( iter / 8192 );
            uint32 iCol = ( iter / 32 ) % 8;
            
            if( ( ( ( iRow + iCol ) % 2 ) == 0 ) ) {
                DefaultTexture[ iter ] = 0xFFFF00FF;
            } else {
                DefaultTexture[ iter ] = 0xFF444444;
            }
        }
        AddTexture( AssetOutput, "DefaultTexture", Width, Height, DefaultTexture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // create and Upload white Texture
        uint32 WhiteTexture = 0xFFFFFFFF;
        AddTexture( AssetOutput, "WhiteTexture", 1, 1, &WhiteTexture );
    }
    { // Create and Upload Texture for Entity Spawn
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center    = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 RadiusA   = ( flo32 )Width * 0.5f;
        flo32 RadiusB   = RadiusA * 0.75f;
        flo32 RadiusASq = RadiusA * RadiusA;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos    = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V      = Pos - Center;
            flo32 DistSq = GetLengthSq( V );
            if( DistSq <= RadiusASq ) {
                flo32 Dist  = sqrtf( DistSq );
                flo32 t     = Clamp01( ( Dist - RadiusB ) / ( RadiusA - RadiusB ) );
                flo32 Alpha = lerp( 1.0f, t, 0.0f );
                Texture[ iTexel ] = ToU32Color( Vec3( 1.0f, 1.0f, 1.0f ) * Alpha, Alpha );
            }
            
        }
        AddTexture( AssetOutput, "EntitySpawn_Indicator", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Worm Body
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.5f;
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        flo32 RadiusSq = Radius * Radius;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos    = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V      = Pos - Center;
            flo32 DistSq = GetLengthSq( V );
            if( DistSq <= RadiusSq ) {
                flo32 Dist    = sqrtf( DistSq );
                flo32 t     = Clamp01( Dist / Radius );
                flo32 tGray = lerp( GrayB, cosf( t * ( PI * 0.5f ) ), GrayA );
                
                flo32 tDist   = Clamp01( Dist / Radius );
                flo32 Radians = acosf( tDist );
                flo32 Sin     = sinf( Radians );
                tGray         = lerp( GrayB, Sin, GrayA );
                
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "Worm_Body", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Barrel
        int32 Width     = 256;
        int32 HalfWidth = Width / 2;
        int32 Height    = 256;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            flo32 tWidth  = Clamp01( ( flo32 )iCol / ( flo32 )Width ) * 2.0f - 1.0f;
            flo32 Radians = acosf( tWidth );
            flo32 Sin     = sinf( Radians );
            flo32 tGray   = lerp( GrayB, Sin, GrayA );
            Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
        }
        AddTexture( AssetOutput, "Barrel", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Barrel Rotated 90 Degrees
        int32 Width     = 256;
        int32 Height    = 256;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            flo32 tHeight = Clamp01( ( flo32 )iRow / ( flo32 )Height ) * 2.0f - 1.0f;
            flo32 Radians = acosf( tHeight );
            flo32 Sin     = sinf( Radians );
            flo32 tGray   = lerp( GrayB, Sin, GrayA );
            Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
        }
        AddTexture( AssetOutput, "Barrel_Rot90", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Barrel End
        int32 Width  = 128;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center = Vec2( 0.0f, ( flo32 )Height * 0.5f );
        flo32 Radius = ( flo32 )Width;
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            if( Dist <= Radius ) {
                flo32 tDist   = Clamp01( Dist / Radius );
                flo32 Radians = acosf( tDist );
                flo32 Sin     = sinf( Radians );
                flo32 tGray   = lerp( GrayB, Sin, GrayA );
                //flo32 t     = cosf( Clamp01( Dist / Radius ) * ( PI * 0.5f ) );
                //flo32 tGray = lerp( GrayB, t, GrayA );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
            
        }
        AddTexture( AssetOutput, "Barrel_End", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Collector
        int32 Width     = 256;
        int32 Height    = 256;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 Radius_Outer = ( flo32 )Width * 0.5f;
        flo32 Radius_Inner = 20.0f;
        
        flo32 Offset = 12.0f;
        vec2  Center = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f + Vec2( Offset, 0.0f );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  P       = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            flo32 Dist    = GetLength( P - Center );
            flo32 Radians = ToRadians( P - Center );
            flo32 ModRadius_Outer = Radius_Outer - cosf( Radians ) * Offset;
            
            if( Dist <= ModRadius_Outer ) {
                
                flo32 t = Clamp01( ( Dist - Radius_Inner ) / ( ModRadius_Outer - Radius_Inner ) );
                //t = t;
                
                flo32 tGray   = lerp( 1.0f, 1.0f - t, 0.05f );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "Collector", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Collector Flash
        int32 Width     = 256;
        int32 Height    = 256;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 Radius_Outer = ( flo32 )Width * 0.5f;
        flo32 Radius_Inner = 20.0f;
        
        flo32 Offset = 12.0f;
        vec2  Center = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f + Vec2( Offset, 0.0f );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  P       = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            flo32 Dist    = GetLength( P - Center );
            flo32 Radians = ToRadians( P - Center );
            flo32 ModRadius_Outer = Radius_Outer - cosf( Radians ) * Offset;
            
            if( Dist <= ModRadius_Outer ) {
                
                flo32 t = Clamp01( ( Dist - Radius_Inner ) / ( ModRadius_Outer - Radius_Inner ) );
                //t = t;
                
                flo32 tGray   = lerp( 1.0f, 1.0f - t, 0.2f );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "Collector_Flash", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Player Bullet Head
        int32 Margin    = 32;
        int32 Width     = 8  + Margin * 2;
        int32 Height    = 96 + Margin;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect  Bound  = Rect( ( flo32 )Margin, 0.0f, ( flo32 )( Width - Margin ), ( flo32 )( Height - Margin ) );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            if( !IsInRect( P, Bound ) ) {
                flo32 Dist  = DistPointToRect( P, Bound );
                flo32 t     = 1.0f - Clamp01( Dist / Margin );
                Color       = Vec4( 0.0f, t, 0.0f, t );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "PlayerBullet_Head", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Player Bullet Tail
        int32 Margin    = 32;
        int32 Width     = 8  + Margin * 2;
        int32 Height    = 96 + Margin * 2;
        int32 nTexel    = Width * Height;
        
        flo32 HalfHeight = ( flo32 )Height * 0.5f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect  Bound  = Rect( ( flo32 )Margin, ( flo32 )Margin, ( flo32 )( Width - Margin ), ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            if( !IsInRect( P, Bound ) ) {
                flo32 Dist  = DistPointToRect( P, Bound );
                flo32 t     = 1.0f - Clamp01( Dist / Margin );
                Color       = Vec4( 0.0f, t, 0.0f, t );
            }
            
            flo32 tY = Clamp01( P.y / ( flo32 )Height );
            flo32 tX = Clamp01( P.x / ( flo32 )Width );
            
            if( tX >= 0.5f ) {
                tX = 1.0f - tX;
            }
            tX *= 2.0f;
            tX  = lerp( tY, tX, 1.0f );
            
            Color *= ( tY * tX );
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "PlayerBullet_Tail", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Miner Laser Body
        int32 Margin    = 32;
        int32 Width     = 8  + Margin * 2;
        int32 Height    = 96 + Margin * 2;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect  Bound  = Rect( ( flo32 )Margin, 0.0f, ( flo32 )( Width - Margin ), ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 1.0f, 0.5f, 0.5f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            if( !IsInRect( P, Bound ) ) {
                flo32 Dist  = DistPointToRect( P, Bound );
                flo32 t     = 1.0f - Clamp01( Dist / Margin );
                Color       = Vec4( t, 0.0f, 0.0f, t );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "MinerLaser_Body", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Miner Laser End
        int32 Margin    = 32;
        int32 Width     = 8  + Margin * 2;
        int32 Height    = Margin;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2 A = Vec2( ( flo32 )Margin, 0.0f );
        vec2 B = Vec2( ( flo32 )( Width - Margin ), 0.0f );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 1.0f, 0.5f, 0.5f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            
            flo32 Dist  = DistPointToEdge( P, A, B );
            flo32 t     = 1.0f - Clamp01( Dist / Margin );
            Color       = Vec4( t, 0.0f, 0.0f, t );
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "MinerLaser_End", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Circle
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            if( Dist <= Radius ) {
                Texture[ iTexel ] = ToU32Color( COLOR_WHITE );
            }
            
        }
        AddTexture( AssetOutput, "Circle", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Half Circle
        int32 Width  = 128;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height * 0.5f );
        flo32 Radius   = ( flo32 )Height * 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            if( Dist <= Radius ) {
                Texture[ iTexel ] = ToU32Color( COLOR_WHITE );
            }
            
        }
        AddTexture( AssetOutput, "HalfCircle", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Rounded Dome
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        flo32 RadiusSq = Radius * Radius;
        
        flo32 Radius_Inner = Radius * 0.8f;
        flo32 RadiansA     = PI * 0.45f;
        flo32 RadiansB     = RadiansA * 0.15f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos    = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V      = Pos - Center;
            flo32 DistSq = GetLengthSq( V );
            if( DistSq <= RadiusSq ) {
                flo32 Radians = 0.0f;
                flo32 Dist    = sqrtf( DistSq );
                
                if( Dist >= Radius_Inner ) {
                    flo32 t = Clamp01( ( Dist - Radius_Inner ) / ( Radius - Radius_Inner ) );
                    Radians = lerp( RadiansB, t, RadiansA );
                } else {
                    flo32 t = Clamp01( Dist / Radius_Inner );
                    Radians = lerp( 0.0f, t, RadiansB );
                }
                
                flo32 t     = cosf( Radians );
                flo32 tGray = lerp( GrayB, t, GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "RoundedDome", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Dome
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        flo32 RadiansA = PI * 0.2f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V    = Pos - Center;
            flo32 Dist = GetLength( V );
            if( Dist <= Radius ) {
                flo32 t       = Clamp01( Dist / Radius );
                flo32 Radians = lerp( 0.0f, t, RadiansA );
                
                flo32 tGray = lerp( GrayB, cosf( Radians ), GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "Dome", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Half Dome
        int32 Width  = 128;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height * 0.5f );
        flo32 Radius   = ( flo32 )Height * 0.5f;
        flo32 RadiansA = PI * 0.2f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V    = Pos - Center;
            flo32 Dist = GetLength( V );
            if( Dist <= Radius ) {
                flo32 t       = Clamp01( Dist / Radius );
                flo32 Radians = lerp( 0.0f, t, RadiansA );
                
                flo32 tGray = lerp( GrayB, cosf( Radians ), GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "HalfDome", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Glow
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V    = Pos - Center;
            flo32 Dist = GetLength( V );
            
            flo32 t       = Clamp01( Dist / Radius );
            vec4  Color   = COLOR_WHITE * ( 1.0f - t );
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "Glow", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Powerup Body
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V    = Pos - Center;
            flo32 Dist = GetLength( V );
            
            if( Dist <= Radius ) {
                flo32 t       = Clamp01( Dist / Radius );
                vec4  Color   = COLOR_WHITE * ( t * t );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "Powerup_Body", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Powerup Health
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        rect R = Rect( 0.0f, 0.0f, ( flo32 )Width, ( flo32 )Height );
        
        flo32 Margin = 16.0f;
        vec2  Dim    = Vec2( ( flo32 )Width, ( flo32 )Width ) * 0.55f * 0.5f;
        rect  Bound[ 4 ] = {
            RectBLD( Vec2( 0.0f,           0.0f ), Dim ),
            RectBRD( Vec2( ( flo32 )Width, 0.0f ), Dim ),
            RectTRD( Vec2( ( flo32 )Width, ( flo32 )Height ), Dim ),
            RectTLD( Vec2( 0.0f,           ( flo32 )Height ), Dim ),
        };
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.35f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            
            boo32 IsValid = true;
            flo32 Dist    = DistPointToRectPerimeter( Pos, R );
            for( int32 Iter = 0; Iter < 4; Iter++ ) {
                if( IsInRect( Pos, Bound[ Iter ] ) ) {
                    IsValid = false;
                } else {
                    Dist = MinValue( DistPointToRect( Pos, Bound[ Iter ] ), Dist );
                }
            }
            
            if( IsValid ) {
                flo32 tDist = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayB, tDist, GrayA );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "Powerup_Health", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Powerup Shield
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        vec2  Dim = Vec2( ( flo32 )Width, ( flo32 )Height * 0.75f );
        rect  R   = RectCD( Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f, Dim );
        
        flo32 Margin = 16.0f;
        flo32 Left  = ( flo32 )Width * 0.3333f;
        flo32 Right = ( flo32 )Width * 0.6667f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.35f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            
            if( IsInRect( Pos, R ) ) {
                vec4 Color = COLOR_BLUE;
                if( Pos.x < Left ) {
                    Color = COLOR_RED;
                } else if( Pos.x > Right ) {
                    Color = COLOR_YELLOW;
                }
                
                flo32 Dist  = DistPointToRectPerimeter( Pos, R );
                flo32 tDist = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayB, tDist, GrayA );
                Color.rgb *= tGray;
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "Powerup_Health", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Score Multiplier
        flo32 Margin = 16.0f;
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.35f;
        
        vec2 A = Vec2( 0.0f, 0.0f );
        vec2 B = Vec2( ( flo32 )Width, 0.0f );
        vec2 C = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            if( IsInTri( Pos, A, B, C ) ) {
                flo32 Dist  = DistPointToTri( Pos, A, B, C );
                flo32 t     = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayB, t, GrayA );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
            
        }
        AddTexture( AssetOutput, "ScoreMultiplier", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Auto-Turret
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.1f;
        
        vec2  Center  = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius  = ( flo32 )Width * 0.5f;
        flo32 RadiusA = Radius * 0.5f;
        flo32 RadiusB = Radius * 0.95f;
        
        flo32 RadiansA = PI * 0.35f;
        flo32 RadiansB = RadiansA * 0.1f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V    = Pos - Center;
            flo32 Dist = GetLength( V );
            if( Dist <= Radius ) {
                flo32 Radians = 0.0f;
                if( Dist <= RadiusA ) {
                    Radians = PI * 0.4f;
                } else if( Dist <= RadiusB ) {
                    Radians = RadiansA;
                } else {
                    flo32 t = Clamp01( ( Dist - RadiusB ) / ( Radius - RadiusB ) );
                    Radians = lerp( RadiansA, t, RadiansB );
                }
                
                flo32 t     = cosf( Radians );
                flo32 tGray = lerp( GrayA, t, GrayB );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "AutoTurret", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for AutoTurret Laser Body
        int32 Margin    = 32;
        int32 Width     = 8  + Margin * 2;
        int32 Height    = 96 + Margin * 2;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect  Bound  = Rect( ( flo32 )Margin, 0.0f, ( flo32 )( Width - Margin ), ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 0.5f, 1.0f, 0.5f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            if( !IsInRect( P, Bound ) ) {
                flo32 Dist  = DistPointToRect( P, Bound );
                flo32 t     = 1.0f - Clamp01( Dist / Margin );
                Color       = Vec4( 0.0f, t, 0.0f, t );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "AutoTurretLaser_Body", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for AutoTurret Laser End
        int32 Margin    = 32;
        int32 Width     = 8  + Margin * 2;
        int32 Height    = Margin;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2 A = Vec2( ( flo32 )Margin, 0.0f );
        vec2 B = Vec2( ( flo32 )( Width - Margin ), 0.0f );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 1.0f, 0.5f, 0.5f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            
            flo32 Dist  = DistPointToEdge( P, A, B );
            flo32 t     = 1.0f - Clamp01( Dist / Margin );
            Color       = Vec4( 0.0f, t, 0.0f, t );
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "AutoTurretLaser_End", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Bay Doors Interior
        int32 xMargin   = 120;
        flo32 yMargin   = 156;
        int32 Width     = 256;
        int32 Height    = 256;
        int32 nTexel    = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  bPos  = Vec2( ( flo32 )Width * 0.5f, 0.0f );
        rect  Bound = Rect( ( flo32 )xMargin, 0.0f, ( flo32 )( Width - xMargin ), ( flo32 )( Height - yMargin ) );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            if( !IsInRect( P, Bound ) ) {
                flo32 Dist  = DistPointToRect( P, Bound );
                flo32 t     = 1.0f - Clamp01( Dist / xMargin );
                Color       = Vec4( t, t, t, 1.0f );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "BayDoors_Interior", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Bay Doors Spawn Dest
        int32 Width     = 256;
        int32 Height    = 256;
        int32 nTexel    = Width * Height;
        
        flo32 HalfWidth  = ( flo32 )Width  * 0.5f;
        flo32 HalfHeight = ( flo32 )Height * 0.5f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec4 Color = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            vec2 P     = Vec2( ( flo32 )iCol, ( flo32 )iRow );
            
            flo32 tX = 0.0f;
            if( P.x < HalfWidth ) {
                tX = Clamp01( P.x / HalfWidth );
            } else {
                tX = 1.0f - Clamp01( ( P.x - HalfWidth ) / HalfWidth );
            }
            
            flo32 MinY = lerp( 0.0f, tX, HalfHeight );
            flo32 MaxY = MinY + HalfHeight;
            
            if( ( P.y >= MinY ) && ( P.y <= MaxY ) ) {
                Texture[ iTexel ] = ToU32Color( COLOR_WHITE );
            }
            
        }
        AddTexture( AssetOutput, "BayDoors_SpawnDest", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Zero-Point Base Inactive
        flo32 Margin = 8.0f;
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        vec2  Center  = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius  = ( flo32 )Width * 0.5f;
        flo32 RadiusA = Radius - Margin;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V    = Pos - Center;
            flo32 Dist = GetLength( V );
            if( Dist <= Radius ) {
                flo32 t     = Clamp01( ( Dist - RadiusA ) / ( Radius - RadiusA ) ); 
                flo32 tGray = lerp( GrayA, t, GrayB );
                Texture[ iTexel ] = ToU32Color( Vec4( tGray, tGray, tGray, tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "ZeroPoint_Base_Inactive", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Zero-Point Base Active
        flo32 Margin = 16.0f;
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        vec2  Center  = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius  = ( flo32 )Width * 0.5f;
        flo32 RadiusA = Radius - Margin;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V    = Pos - Center;
            flo32 Dist = GetLength( V );
            if( Dist <= Radius ) {
                flo32 t     = Clamp01( ( Dist - RadiusA ) / ( Radius - RadiusA ) ); 
                flo32 tGray = lerp( GrayA, t, GrayB );
                Texture[ iTexel ] = ToU32Color( Vec4( tGray, tGray, tGray, tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "ZeroPoint_Base_Active", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Looper
        flo32 Margin = 8.0f;
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        rect  Bound = Rect( Margin, Margin, ( flo32 )Width - Margin, ( flo32 )Height - Margin );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4 Color = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            if( !IsInRect( Pos, Bound ) ) {
                flo32 Dist  = DistPointToRect( Pos, Bound );
                flo32 t     = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayA, t, GrayB );
                Color = COLOR_GRAY( tGray );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "Looper", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Blimp
        flo32 Margin = 8.0f;
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.4f;
        
        vec2 A = Vec2( 0.0f, 0.0f );
        vec2 B = Vec2( ( flo32 )Width, 0.0f );
        vec2 C = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            if( IsInTri( Pos, A, B, C ) ) {
                flo32 Dist  = DistPointToTri( Pos, A, B, C );
                flo32 t     = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayB, t, GrayA );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
            
        }
        AddTexture( AssetOutput, "Blimp", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Satellite Dish
        int32 Width  = 128;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center = Vec2( 0.0f, ( flo32 )Height * 0.5f );
        flo32 Radius = ( flo32 )Width;
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.65f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            if( Dist <= Radius ) {
                flo32 t     = cosf( Clamp01( Dist / Radius ) * ( PI * 0.5f ) );
                flo32 tGray = lerp( GrayB, t, GrayA );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
            
        }
        AddTexture( AssetOutput, "SatelliteDish", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Satellite Shield
        flo32 MarginA = 24.0f;
        flo32 MarginB = 6.0f;
        
        int32 Width  = 512;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 MarginC = ( flo32 )Width - ( MarginA + MarginB );
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect Bound = Rect( MarginA, 0.0f, MarginA + MarginB, ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4  Color = {};
            
            if( Pos.x < Bound.Left ) {
                flo32 t = 1.0f - Clamp01( ( Bound.Left - Pos.x ) / MarginA );
                Color   = Vec4( 0, t, t, t );
            } else if( Pos.x < Bound.Right ) {
                Color = Vec4( 0.85f, 1.0f, 1.0f, 1.0f );
            } else {
                flo32 t = 1.0f - Clamp01( ( Pos.x - Bound.Right ) / MarginC );
                t       = t * t * t;
                Color   = Vec4( 0, t, t, t );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "SatelliteDish_Shield", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Satellite Shield Hit
        flo32 Margin = 96.0f;
        
        int32 Width  = 128 + ( int32 )Margin;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center = Vec2( Margin, ( flo32 )Height * 0.5f );
        flo32 Radius = ( flo32 )Height * 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4  Color = {};
            
            if( Pos.x < Center.x ) {
                flo32 t  = 1.0f - Clamp01( fabsf( Pos.x - Center.x ) / Margin );
                flo32 tY = 1.0f - Clamp01( fabsf( Pos.y - Center.y ) / Radius );
                t *= tY;
                
                Color   = Vec4( t, t, t, t );
            } else {
                flo32 Dist = GetLength( Pos - Center );
                if( Dist <= Radius ) {
                    flo32 t = 1.0f - Clamp01( Dist / Radius );
                    Color   = Vec4( t, t, t, t );
                }
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        
        AddTexture( AssetOutput, "SatelliteDish_ShieldHit", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Asteroid
#define ASTEROID_TEXTURE_ID_MAX_COUNT  ( 8 )
        for( int32 iTexture = 0; iTexture < ASTEROID_TEXTURE_ID_MAX_COUNT; iTexture++ ) {
            int32 Width  = 256;
            int32 Height = 256;
            int32 nTexel = Width * Height;
            
            uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
            
            vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
            flo32 Radius   = ( flo32 )Width * 0.5f;
            int32 nLayer   = 2;
            int32 nSegment = 8;
            int32 nVertex  = nLayer * nSegment + 1;
            
            vec2 * Offset = _PushArray_Clear( TempMemory, vec2, nVertex );
            
            for( int32 iLayer = 0; iLayer < nLayer; iLayer++ ) {
                flo32 DistA = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer     ) / ( flo32 )nLayer ), PI * 0.5f ) );
                flo32 DistB = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer + 1 ) / ( flo32 )nLayer ), PI * 0.5f ) );
                
                flo32 Adjust = 0.0f;
                if( ( iLayer % 2 ) == 1 ) {
                    Adjust = ( TAU / ( flo32 )nSegment ) * 0.5f;
                }
                
                flo32 RadAdjust = 0.0f;
                if( iLayer == 0 ) {
                    RadAdjust = 8.0f;
                }
                
                for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                    int32 iOffset = iLayer * nSegment + iSegment;
                    
                    flo32 Radians = ( ( flo32 )iSegment / ( flo32 )nSegment ) * TAU + Adjust;
                    flo32 Dist    = ( DistA * Radius + RadAdjust );
                    
                    if( iLayer > 0 ) {
                        Radians      += RandomF32InRange( -1.0f, 1.0f ) * ( PI / 32.0f );
                        Dist         += RandomF32InRange( -1.0f, 1.0f ) * ( Radius * 0.1f );
                    }
                    
                    Offset[ iOffset ] = Vec2( Radians, Dist );
                }
            }
            
            flo32 GrayA = 1.0f;
            flo32 GrayB = 0.5f;
            
            for( int32 iLayer = 0; iLayer < nLayer; iLayer++ ) {
                for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                    int32 iSegmentA = iSegment;
                    int32 iSegmentB = ( iSegment + 1 ) % nSegment;
                    
                    iSegmentA += iLayer * nSegment;
                    iSegmentB += iLayer * nSegment;
                    
                    int32 iSegmentC = iSegmentA + nSegment;
                    int32 iSegmentD = iSegmentB + nSegment;
                    
                    iSegmentA = MinValue( iSegmentA, nVertex - 1 );
                    iSegmentB = MinValue( iSegmentB, nVertex - 1 );
                    iSegmentC = MinValue( iSegmentC, nVertex - 1 );
                    iSegmentD = MinValue( iSegmentD, nVertex - 1 );
                    
                    vec2 OffsetA = Offset[ iSegmentA ];
                    vec2 OffsetB = Offset[ iSegmentB ];
                    vec2 OffsetC = Offset[ iSegmentC ];
                    vec2 OffsetD = Offset[ iSegmentD ];
                    
                    vec2 A = Center + ToDirection2D( OffsetA.x ) * ( OffsetA.y );
                    vec2 B = Center + ToDirection2D( OffsetB.x ) * ( OffsetB.y );
                    vec2 C = Center + ToDirection2D( OffsetC.x ) * ( OffsetC.y );
                    vec2 D = Center + ToDirection2D( OffsetD.x ) * ( OffsetD.y );
                    
                    flo32 tGrayA = RandomF32InRange( 0.55f, 0.95f ) + ( flo32 )iLayer * 0.05f;
                    flo32 tGrayB = RandomF32InRange( 0.55f, 0.95f ) + ( flo32 )iLayer * 0.05f;
                    vec4  ColorA = COLOR_GRAY( tGrayA );
                    vec4  ColorB = COLOR_GRAY( tGrayB );
                    
                    {
                        int32 xMin = ( int32 )( MinValue( MinValue( A.x, B.x ), C.x ) + 0.5f );
                        int32 yMin = ( int32 )( MinValue( MinValue( A.y, B.y ), C.y ) + 0.5f );
                        int32 xMax = ( int32 )( MaxValue( MaxValue( A.x, B.x ), C.x ) + 0.5f );
                        int32 yMax = ( int32 )( MaxValue( MaxValue( A.y, B.y ), C.y ) + 0.5f );
                        
                        xMin = MaxValue( xMin,     0 );
                        yMin = MaxValue( yMin,     0 );
                        xMax = MinValue( xMax + 1, Width );
                        yMax = MinValue( yMax + 1, Height );
                        
                        for( int32 Y = yMin; Y < yMax; Y++ ) {
                            for( int32 X = xMin; X < xMax; X++ ) {
                                int32 iTexel = Y * Width + X;
                                
                                vec2  Pos    = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
                                if( IsInTri( Pos, A, B, C ) ) {
                                    flo32 Dist = GetLength( Pos - Center );
                                    if( Dist <= Radius ) {
                                        flo32 tDist  = Clamp01( Dist / Radius );
                                        flo32 tGray0 = tGrayA * lerp( GrayA, tDist, GrayB );
                                        Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray0 ) );
                                    }
                                }
                            }
                        }
                    }
                    
                    {
                        int32 xMin = ( int32 )MinValue( MinValue( B.x, C.x ), D.x );
                        int32 yMin = ( int32 )MinValue( MinValue( B.y, C.y ), D.y );
                        int32 xMax = ( int32 )MaxValue( MaxValue( B.x, C.x ), D.x );
                        int32 yMax = ( int32 )MaxValue( MaxValue( B.y, C.y ), D.y );
                        
                        xMin = MaxValue( xMin, 0 );
                        yMin = MaxValue( yMin, 0 );
                        xMax = MinValue( xMax, Width );
                        yMax = MinValue( yMax, Height );
                        
                        for( int32 Y = yMin; Y < yMax; Y++ ) {
                            for( int32 X = xMin; X < xMax; X++ ) {
                                int32 iTexel = Y * Width + X;
                                
                                vec2  Pos    = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
                                if( IsInTri( Pos, B, C, D ) ) {
                                    flo32 Dist = GetLength( Pos - Center );
                                    if( Dist <= Radius ) {
                                        flo32 tDist  = Clamp01( Dist / Radius );
                                        flo32 tGray0 = tGrayB * lerp( GrayA, tDist, GrayB );
                                        Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray0 ) );
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            { // Manually patch texture holes
                for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
                    if( Texture[ iTexel ] == 0 ) {
                        int32 iRow = ( iTexel / Width );
                        int32 iCol = ( iTexel % Width );
                        
                        vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
                        flo32 Dist = GetLength( Pos - Center );
                        if( Dist <= ( Radius * 0.95f ) ) {
                            int32 IndexA = iTexel - 1;
                            int32 IndexB = iTexel - Width;
                            int32 IndexC = iTexel + 1;
                            int32 IndexD = iTexel + Width;
                            
                            vec4   Color = {};
                            int32 nColor = 0;
                            
                            if( ( iCol > 0      ) && ( Texture[ IndexA ] != 0 ) ) {
                                Color  += ToV4Color( Texture[ IndexA ] );
                                nColor += 1;
                            }
                            if( ( iRow > 0      ) && ( Texture[ IndexB ] != 0 ) ) {
                                Color  += ToV4Color( Texture[ IndexB ] );
                                nColor += 1;
                            }
                            if( ( iCol < Width  ) && ( Texture[ IndexC ] != 0 ) ) {
                                Color  += ToV4Color( Texture[ IndexC ] );
                                nColor += 1;
                            }
                            if( ( iRow < Height ) && ( Texture[ IndexD ] != 0 ) ) {
                                Color  += ToV4Color( Texture[ IndexD ] );
                                nColor += 1;
                            }
                            
                            Assert( nColor > 0 );
                            Color /= ( flo32 )nColor;
                            
                            Texture[ iTexel ] = ToU32Color( Color );
                        }
                    }
                }
            }
            
            char Texture_Name[ 128 ] = {};
            sprintf( Texture_Name, "Asteroid%02u", iTexture );
            AddTexture( AssetOutput, Texture_Name, Width, Height, Texture );
            
            _PopArray( TempMemory, vec2,   nVertex );
            _PopArray( TempMemory, uint32, nTexel );
        }
    }
    
    { // Create and Upload Texture for Inside Turret Barrel
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center  = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius  = ( flo32 )Width * 0.5f;
        vec2  CenterA = Vec2( ( flo32 )Width, ( flo32 )Height * 0.5f );
        flo32 RadiusA = Radius * 1.9f;
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            if( Dist <= Radius ) {
                flo32 DistA = GetLength( Pos - CenterA );
                flo32 t     = Clamp01( DistA / RadiusA );
                flo32 tGray = lerp( GrayA, t, GrayB );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "Turret_InsideBarrel", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Turret Base
        flo32 Margin  = 32.0f;
        flo32 MarginA = Margin * 0.25f;
        int32 Width   = 256;
        int32 Height  = 256;
        int32 nTexel  = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect  Bound = Rect( MarginA, Margin, ( flo32 )Width - MarginA, ( flo32 )Height - Margin );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        flo32 GrayC = 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            if( IsInRect( Pos, Bound ) ) {
                flo32 tX    = Clamp01( ( Pos.x - Bound.Left ) / GetWidth( Bound ) );
                flo32 tGray = lerp( GrayA, sinf( tX * PI ), GrayC );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            } else {
                flo32 Dist  = DistPointToRect( Pos, Bound );
                flo32 t     = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayA, t, GrayC );
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "Turret_Base", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Turret Shield
        flo32 MarginA = 72.0f;
        flo32 MarginB = 24.0f;
        
        int32 Width   = 256;
        int32 Height  = 256;
        int32 nTexel  = Width * Height;
        
        flo32 MarginC = ( flo32 )Height - ( MarginA + MarginB );
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect  Bound = Rect( 0.0f, MarginA, ( flo32 )Width, MarginA + MarginB );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        flo32 GrayC = 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4 Color = {};
            
            if( Pos.y < Bound.Bottom ) {
                flo32 t = 1.0f - Clamp01( ( Bound.Bottom - Pos.y ) / MarginA );
                Color   = Vec4( t, t, t, t );
            } else if( Pos.y < Bound.Top ) {
                Color = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            } else {
                flo32 t = 1.0f - Clamp01( ( Pos.y - Bound.Top ) / MarginC );
                t = lerp( 0.5f, t, 0.75f );
                
                Color   = Vec4( t, t, t, t );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "Turret_Shield_Glow", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Turret Shield Ring
        flo32 MarginA = 72.0f;
        flo32 MarginB = 24.0f;
        
        int32 Width   = 256;
        int32 Height  = 256;
        int32 nTexel  = Width * Height;
        
        flo32 MarginC = ( flo32 )Height - ( MarginA + MarginB );
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        rect  Bound = Rect( 0.0f, MarginA, ( flo32 )Width, MarginA + MarginB );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        flo32 GrayC = 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4 Color = {};
            
            if( ( Pos.y >= Bound.Bottom ) && ( Pos.y < Bound.Top ) ) {
                Color = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "Turret_Shield_Ring", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Recon
        flo32 Margin = 24.0f;
        
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2 A = Vec2( 0.0f, 0.0f );
        vec2 B = Vec2( ( flo32 )Width, 0.0f );
        vec2 C = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.8f;
        flo32 GrayC = 0.4f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            
            if( IsInTri( Pos, A, B, C ) ) {
                flo32 Dist  = DistPointToTri( Pos, A, B, C );
                flo32 t     = Dist / Margin;
                flo32 tGray = GrayA;
                if( t <= 1.0f ) {
                    t     = t * 0.8f + 0.2f;
                    tGray = lerp( GrayB, t, GrayC );
                }
                
                vec4  Color = COLOR_GRAY( tGray );
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "Recon", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Enemy Bullet Core
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        flo32 RadiusSq = Radius * Radius;
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos    = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 DistSq = GetLengthSq( Pos - Center );
            
            if( DistSq <= RadiusSq ) {
                flo32 Dist  = sqrtf( DistSq );
                flo32 t     = 1.0f - Clamp01( Dist / Radius );
                flo32 tGray = lerp( GrayB, sinf( t * PI * 0.5f ), GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "EnemyBullet_Core", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Enemy Bullet Surface
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        flo32 RadiusSq = Radius * Radius;
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            
            if( Dist <= Radius ) {
                flo32 t     = 1.0f - Clamp01( Dist / Radius );
                flo32 Alpha = ( 1.0f - sinf( t * PI * 0.5f ) );
                flo32 tGray = lerp( GrayA, Alpha, GrayB );
                vec4  Color = COLOR_GRAY( tGray ) * Alpha;
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "EnemyBullet_Surface", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Tractor Beam Glow
        flo32 MarginA = 12.0f;
        flo32 MarginB =  3.0f;
        flo32 MarginC =  4.0f;
        
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center  = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius  = ( flo32 )Width * 0.5f;
        flo32 RadiusA = Radius  - MarginA;
        flo32 RadiusB = RadiusA - MarginB;
        flo32 RadiusC = RadiusB - MarginC;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            
            if( Dist <= Radius ) {
                if( Dist >= RadiusA ) {
                    flo32 t     = 1.0f - Clamp01( ( Dist - RadiusA ) / MarginA );
                    vec4  Color = Vec4( t, t, t, t );
                    Texture[ iTexel ] = ToU32Color( Color );
                } else if( Dist >= RadiusB ) {
                    vec4 Color = COLOR_WHITE;
                    Texture[ iTexel ] = ToU32Color( Color );
                } else if( Dist >= RadiusC ) {
                    flo32  t = Clamp01( ( Dist - RadiusC ) / MarginC );
                    vec4 Color = Vec4( t, t, t, t );
                    Texture[ iTexel ] = ToU32Color( Color );
                }
            }
        }
        AddTexture( AssetOutput, "TractorBeam_Glow", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Tractor Beam Ring
        flo32 MarginA = 10.0f;
        flo32 MarginB =  3.0f;
        flo32 MarginC =  3.0f;
        
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center  = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius  = ( flo32 )Width * 0.5f;
        flo32 RadiusA = Radius  - MarginA;
        flo32 RadiusB = RadiusA - MarginB;
        flo32 RadiusC = RadiusB - MarginC;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist = GetLength( Pos - Center );
            
            if( ( Dist >= RadiusB ) && ( Dist <= RadiusA ) ) {
                vec4 Color = COLOR_WHITE;
                Texture[ iTexel ] = ToU32Color( Color );
            }
        }
        AddTexture( AssetOutput, "TractorBeam_Ring", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Mega Miner Body
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 Margin  = 32.0f;
        flo32 Margin0 = 8.0f;
        flo32 xMax    = ( flo32 )Width - Margin;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        flo32 RadiusSq = Radius * Radius;
        
        flo32 Radius_Inner = Radius * 0.8f;
        flo32 RadiansA     = PI * 0.45f;
        flo32 RadiansB     = RadiansA * 0.15f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos    = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  V      = Pos - Center;
            flo32 DistSq = GetLengthSq( V );
            if( ( Pos.x <= xMax ) && ( DistSq <= RadiusSq ) ) {
                flo32 Radians = 0.0f;
                flo32 Dist    = sqrtf( DistSq );
                
                if( Dist >= Radius_Inner ) {
                    flo32 t = Clamp01( ( Dist - Radius_Inner ) / ( Radius - Radius_Inner ) );
                    Radians = lerp( RadiansB, t, RadiansA );
                } else {
                    flo32 t = Clamp01( Dist / Radius_Inner );
                    Radians = lerp( 0.0f, t, RadiansB );
                }
                
                flo32 Dist0 = fabsf( Pos.x - xMax );
                flo32 t0    = Clamp01( Dist0 / Margin0 );
                
                flo32 t     = cosf( Radians );
                flo32 tGray = lerp( GrayB, t * t0, GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
            
        }
        AddTexture( AssetOutput, "MegaMiner_Body", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Worm Tag
        flo32 MarginA = 16.0f;
        flo32 MarginB = 36.0f;
        flo32 Margin  = MarginA + MarginB;
        
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        rect Bound = Rect( Margin, Margin, ( flo32 )Width - Margin, ( flo32 )Height - Margin );
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.2f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos  = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            
            if( IsInRect( Pos, Bound ) ) {
                Texture[ iTexel ] = ToU32Color( COLOR_WHITE );
            } else {
                flo32 Dist = DistPointToRect( Pos, Bound );
                if( Dist <= Margin ) {
                    flo32 t     = Clamp01( ( Dist - MarginB ) / MarginA );
                    flo32 tGray = lerp( GrayA, t, GrayB );
                    Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
                }
            }
        }
        
        AddTexture( AssetOutput, "Worm_Tag", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Worm Tag Chain
        int32 nSegment = 16;
        
        int32 Height = 16;
        int32 Width  = Height * nSegment;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.75f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            int32 iSegment = iCol / nSegment;
            
            vec2  Pos    = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec2  Center = Vec2( ( flo32 )Height * ( ( flo32 )iSegment + 0.5f ), ( flo32 )Height * 0.5f );
            flo32 Radius = ( flo32 )Height * 0.5f;
            flo32 Dist   = GetLength( Pos - Center );
            if( Dist <= Radius ) {
                flo32 t     = cosf( Clamp01( Dist / Radius ) * ( PI * 0.5f ) );
                flo32 tGray = lerp( GrayB, t, GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        
        AddTexture( AssetOutput, "Worm_TagChain", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Loader Body
        flo32 Edge_Margin = 6.0f;
        
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        vec2  Radius = Vec2( ( flo32 )Width, ( flo32 )Height * 0.2f ) * 0.5f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.75f;
        
        vec2 CenterA = Vec2( ( flo32 )Width * 0.5f, 0.0f );
        vec2 CenterB = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height );
        
        rect Bound = Rect( 0.0f, 0.0f, ( flo32 )Width, ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            
            flo32 DistToEdge = DistPointToRectPerimeter( Pos, Bound );
            flo32 tEdge      = lerp( 0.5f, Clamp01( DistToEdge / Edge_Margin ), 1.0f );
            
            if( ( IsInEllipse( Pos, CenterA, Radius, 0.0f ) ) || (  IsInEllipse( Pos, CenterB, Radius, 0.0f ) ) ) {
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( 0.5f * tEdge ) );
            } else {
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( 0.6f * tEdge ) );
            }
        }
        
        AddTexture( AssetOutput, "Loader_Body", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Spitter Base
        flo32 Edge_Margin = 8.0f;
        
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.5f;
        
        rect Bound = Rect( 0.0f, 0.0f, ( flo32 )Width, ( flo32 )Height );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            
            flo32 DistToEdge = DistPointToRectPerimeter( Pos, Bound );
            flo32 tEdge      = Clamp01( DistToEdge / Edge_Margin );
            flo32 tGray      = lerp( GrayB, tEdge, GrayA );
            
            Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
        }
        
        AddTexture( AssetOutput, "Spitter_Base", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Spit
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        vec2  Center   = Vec2( ( flo32 )Width, ( flo32 )Height ) * 0.5f;
        flo32 Radius   = ( flo32 )Width * 0.5f;
        int32 nLayer   = 2;
        int32 nSegment = 8;
        int32 nVertex  = nLayer * nSegment + 1;
        
        vec2 * Offset = _PushArray_Clear( TempMemory, vec2, nVertex );
        
        for( int32 iLayer = 0; iLayer < nLayer; iLayer++ ) {
            flo32 DistA = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer     ) / ( flo32 )nLayer ), PI * 0.5f ) );
            flo32 DistB = cosf( lerp( 0.0f, Clamp01( ( flo32 )( iLayer + 1 ) / ( flo32 )nLayer ), PI * 0.5f ) );
            
            flo32 Adjust = 0.0f;
            if( ( iLayer % 2 ) == 1 ) {
                Adjust = ( TAU / ( flo32 )nSegment ) * 0.5f;
            }
            
            flo32 RadAdjust = 0.0f;
            if( iLayer == 0 ) {
                RadAdjust = 8.0f;
            }
            
            for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                int32 iOffset = iLayer * nSegment + iSegment;
                
                flo32 Radians = ( ( flo32 )iSegment / ( flo32 )nSegment ) * TAU + Adjust;
                flo32 Dist    = ( DistA * Radius + RadAdjust );
                
                if( iLayer > 0 ) {
                    Radians      += RandomF32InRange( -1.0f, 1.0f ) * ( PI / 32.0f );
                    Dist         += RandomF32InRange( -1.0f, 1.0f ) * ( Radius * 0.1f );
                }
                
                Offset[ iOffset ] = Vec2( Radians, Dist );
            }
        }
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.5f;
        
        for( int32 iLayer = 0; iLayer < nLayer; iLayer++ ) {
            for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                int32 iSegmentA = iSegment;
                int32 iSegmentB = ( iSegment + 1 ) % nSegment;
                
                iSegmentA += iLayer * nSegment;
                iSegmentB += iLayer * nSegment;
                
                int32 iSegmentC = iSegmentA + nSegment;
                int32 iSegmentD = iSegmentB + nSegment;
                
                iSegmentA = MinValue( iSegmentA, nVertex - 1 );
                iSegmentB = MinValue( iSegmentB, nVertex - 1 );
                iSegmentC = MinValue( iSegmentC, nVertex - 1 );
                iSegmentD = MinValue( iSegmentD, nVertex - 1 );
                
                vec2 OffsetA = Offset[ iSegmentA ];
                vec2 OffsetB = Offset[ iSegmentB ];
                vec2 OffsetC = Offset[ iSegmentC ];
                vec2 OffsetD = Offset[ iSegmentD ];
                
                vec2 A = Center + ToDirection2D( OffsetA.x ) * ( OffsetA.y );
                vec2 B = Center + ToDirection2D( OffsetB.x ) * ( OffsetB.y );
                vec2 C = Center + ToDirection2D( OffsetC.x ) * ( OffsetC.y );
                vec2 D = Center + ToDirection2D( OffsetD.x ) * ( OffsetD.y );
                
                flo32 tGrayA = RandomF32InRange( 0.55f, 0.95f ) + ( flo32 )iLayer * 0.05f;
                flo32 tGrayB = RandomF32InRange( 0.55f, 0.95f ) + ( flo32 )iLayer * 0.05f;
                vec4  ColorA = COLOR_GRAY( tGrayA );
                vec4  ColorB = COLOR_GRAY( tGrayB );
                
                {
                    int32 xMin = ( int32 )( MinValue( MinValue( A.x, B.x ), C.x ) + 0.5f );
                    int32 yMin = ( int32 )( MinValue( MinValue( A.y, B.y ), C.y ) + 0.5f );
                    int32 xMax = ( int32 )( MaxValue( MaxValue( A.x, B.x ), C.x ) + 0.5f );
                    int32 yMax = ( int32 )( MaxValue( MaxValue( A.y, B.y ), C.y ) + 0.5f );
                    
                    xMin = MaxValue( xMin,     0 );
                    yMin = MaxValue( yMin,     0 );
                    xMax = MinValue( xMax + 1, Width );
                    yMax = MinValue( yMax + 1, Height );
                    
                    for( int32 Y = yMin; Y < yMax; Y++ ) {
                        for( int32 X = xMin; X < xMax; X++ ) {
                            int32 iTexel = Y * Width + X;
                            
                            vec2  Pos    = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
                            if( IsInTri( Pos, A, B, C ) ) {
                                flo32 Dist = GetLength( Pos - Center );
                                if( Dist <= Radius ) {
                                    Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGrayA ) );
                                }
                            }
                        }
                    }
                }
                
                {
                    int32 xMin = ( int32 )MinValue( MinValue( B.x, C.x ), D.x );
                    int32 yMin = ( int32 )MinValue( MinValue( B.y, C.y ), D.y );
                    int32 xMax = ( int32 )MaxValue( MaxValue( B.x, C.x ), D.x );
                    int32 yMax = ( int32 )MaxValue( MaxValue( B.y, C.y ), D.y );
                    
                    xMin = MaxValue( xMin, 0 );
                    yMin = MaxValue( yMin, 0 );
                    xMax = MinValue( xMax, Width );
                    yMax = MinValue( yMax, Height );
                    
                    for( int32 Y = yMin; Y < yMax; Y++ ) {
                        for( int32 X = xMin; X < xMax; X++ ) {
                            int32 iTexel = Y * Width + X;
                            
                            vec2  Pos    = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
                            if( IsInTri( Pos, B, C, D ) ) {
                                flo32 Dist = GetLength( Pos - Center );
                                if( Dist <= Radius ) {
                                    Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGrayB ) );
                                }
                            }
                        }
                    }
                }
            }
        }
        
        { // Manually patch texture holes
            for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
                if( Texture[ iTexel ] == 0 ) {
                    int32 iRow = ( iTexel / Width );
                    int32 iCol = ( iTexel % Width );
                    
                    vec2 Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
                    flo32 Dist = GetLength( Pos - Center );
                    if( Dist <= ( Radius * 0.95f ) ) {
                        int32 IndexA = iTexel - 1;
                        int32 IndexB = iTexel - Width;
                        int32 IndexC = iTexel + 1;
                        int32 IndexD = iTexel + Width;
                        
                        vec4   Color = {};
                        int32 nColor = 0;
                        
                        if( ( iCol > 0      ) && ( Texture[ IndexA ] != 0 ) ) {
                            Color  += ToV4Color( Texture[ IndexA ] );
                            nColor += 1;
                        }
                        if( ( iRow > 0      ) && ( Texture[ IndexB ] != 0 ) ) {
                            Color  += ToV4Color( Texture[ IndexB ] );
                            nColor += 1;
                        }
                        if( ( iCol < Width  ) && ( Texture[ IndexC ] != 0 ) ) {
                            Color  += ToV4Color( Texture[ IndexC ] );
                            nColor += 1;
                        }
                        if( ( iRow < Height ) && ( Texture[ IndexD ] != 0 ) ) {
                            Color  += ToV4Color( Texture[ IndexD ] );
                            nColor += 1;
                        }
                        
                        Assert( nColor > 0 );
                        Color /= ( flo32 )nColor;
                        
                        Texture[ iTexel ] = ToU32Color( Color );
                    }
                }
            }
        }
        
        AddTexture( AssetOutput, "Spit", Width, Height, Texture );
        
        _PopArray( TempMemory, vec2,   nVertex );
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Observation Deck
        int32 Width  = 2048;
        int32 Height = 1024;
        int32 nTexel = Width * Height;
        
        vec2  Center = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height );
        flo32 Radius = ( flo32 )Height;
        
        flo32 Radius_InnerA = Radius        - 2.0f;
        flo32 Radius_InnerB = Radius_InnerA - 12.0f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.5f;
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist  = GetLength( Pos - Center );
            
            if( Dist <= Radius ) {
                flo32 tDist   = Clamp01( Dist / Radius );
                flo32 Radians = acosf( tDist );
                flo32 Sin     = sinf( Radians );
                flo32 tGray   = lerp( GrayB, Sin, GrayA );
                
                if( ( Dist >= Radius_InnerB ) && ( Dist <= Radius_InnerA ) ) {
                    //tGray *= 0.8f;
                }
                
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "ObservationDeck", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Observation Deck Shield Inner
        flo32 MarginA = 48.0f;
        flo32 MarginB = 4.0f;
        
        int32 Width  = 2048;
        int32 Height = 1024;
        int32 nTexel = Width * Height;
        
        vec2  Center       = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height );
        flo32 Radius_Outer = ( flo32 )Height - MarginA;
        flo32 Radius_Inner = Radius_Outer - MarginB;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4  Color = {};
            flo32 Dist  = GetLength( Pos - Center );
            
            if( Dist > Radius_Outer ) {
                flo32 t = 1.0f - Clamp01( ( Dist - Radius_Outer ) / MarginA );
                Color   = Vec4( 0, t, t, t );
                Color   = {};
            } else if( Dist >= Radius_Inner ) {
                Color = Vec4( 0.85f, 1.0f, 1.0f, 1.0f );
                
                flo32 t = 1.0f - Clamp01( ( Dist - Radius_Inner ) / MarginB );
                Color *= t;
            } else {
                flo32 SubRadius = Radius_Inner * 0.625f;
                flo32 t = Clamp01( ( Dist - SubRadius ) / ( Radius_Inner - SubRadius ) );
                t       = t * t * t * 0.75f;
                Color   = Vec4( 0, t, t, t );
            }
            
            Texture[ iTexel ] = ToU32Color( Color );
        }
        AddTexture( AssetOutput, "ObservationDeck_Shield_Inner", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Observation Deck Shield Ring and Glow
        flo32 MarginA = 48.0f;
        flo32 MarginB = 4.0f;
        
        int32 Width  = 2048;
        int32 Height = 1024;
        int32 nTexel = Width * Height;
        
        vec2  Center       = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height );
        flo32 Radius_Outer = ( flo32 )Height - MarginA;
        flo32 Radius_Inner = Radius_Outer - MarginB;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4  Color = {};
            flo32 Dist  = GetLength( Pos - Center );
            
            if( Dist > Radius_Outer ) {
                flo32 t = 1.0f - Clamp01( ( Dist - Radius_Outer ) / MarginA );
                Color   = Vec4( 0, t, t, t );
            } else if( Dist >= Radius_Inner ) {
                Color = Vec4( 0.85f, 1.0f, 1.0f, 1.0f );
            } else {
                flo32 SubRadius = Radius_Inner * 0.5f;
                flo32 t = Clamp01( ( Dist - SubRadius ) / ( Radius_Inner - SubRadius ) );
                t       = t * t * t * 0.75f;
                Color   = Vec4( 0, t, t, t );
                Color   = {};
            }
            
            Texture[ iTexel ] = ToU32Color( Color * 0.8f );
        }
        AddTexture( AssetOutput, "ObservationDeck_Shield_RingAndGlow", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Observation Deck Barrier
        flo32 Margin_Top    = 8.0f;
        
        flo32 Stripe_Height = 48.0f;
        flo32 Stripe_Width  = 32.0f;
        
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        rect Bound = Rect( 0.0f, ( flo32 )Height - ( Margin_Top + Stripe_Height ), ( flo32 )Width, ( flo32 )Height - Margin_Top );
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            vec4  Color = {};
            
            if( IsInRect( Pos, Bound ) ) {
                flo32 tY = Clamp01( ( Pos.y - Bound.Bottom ) / Stripe_Height );
                flo32  X = Pos.x - ( tY * Stripe_Height );
                
                while( X < 0.0f ) {
                    X += ( Stripe_Width * 2.0f );
                }
                
                int32 iStripe = ( int32 )( X / Stripe_Width );
                
                if( ( iStripe % 2 ) == 0 ) {
                    Color = COLOR_YELLOW;
                } else {
                    Color = COLOR_BLACK;
                }
                
                Texture[ iTexel ] = ToU32Color( Color );
            }
            
        }
        AddTexture( AssetOutput, "ObservationDeck_Barrier", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
#define OBSERVATION_DECK_WINDOW_TEXTURE_SCALE  ( 0.25f )
#define OBSERVATION_DECK_WINDOW_RADIANS  ( TAU / 16.0f )
    { // Create and Upload Texture for Observation Deck Window
        int32 Width  = 1024;
        int32 Height = 512;
        int32 nTexel = Width * Height;
        flo32 Scale  = OBSERVATION_DECK_WINDOW_TEXTURE_SCALE;
        vec2  Center = Vec2( ( flo32 )Width * 0.5f, ( flo32 )Height / Scale );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.65f;
        
        flo32 Radius_Outer = ( flo32 )Height / Scale;;
        flo32 Radius_Inner = Radius_Outer * 0.885f;
        Radius_Outer -= 20.0f;
        
        flo32 bRadians = PI * 1.5f;
        flo32 MinRadians = bRadians - OBSERVATION_DECK_WINDOW_RADIANS * 0.5f;
        flo32 MaxRadians = bRadians + OBSERVATION_DECK_WINDOW_RADIANS * 0.5f;
        
        flo32 Bottom_Radius     = Radius_Outer - 12.0f;
        vec2  Window_BottomLeft = Center + ToDirection2D( MinRadians ) * Radius_Outer;
        
        vec4  Window_Color = COLOR_GRAY( 0.05f );
        vec4  Side_Color   = COLOR_GRAY( 0.2f );
        vec4  Bottom_Color = COLOR_GRAY( 0.1f );
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist  = GetLength( Pos - Center );
            if( ( Dist >= Radius_Inner ) && ( Dist <= Radius_Outer ) ) {
                flo32 Radians = ToRadians( Pos - Center );
                if( ( Radians >= MinRadians ) && ( Radians <= MaxRadians ) ) {
                    vec4 Color = Window_Color;
                    flo32 Side_Dist = DistPointToEdge( Pos, Center, Window_BottomLeft );
                    if( Dist >= Bottom_Radius ) {
                        Color = Bottom_Color;
                    } else if( Side_Dist <= 2.0f ) {
                        Color = Side_Color;
                    }
                    
                    Texture[ iTexel ] = ToU32Color( Color );
                }
            }
        }
        AddTexture( AssetOutput, "ObservationDeck_Window", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Security Drone Turret End
        int32 Width  = 128;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        vec2  Center = Vec2( ( flo32 )Width, ( flo32 )Height * 0.5f );
        flo32 Radius = ( flo32 )Width;
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist  = GetLength( Pos - Center );
            
            if( Dist <= Radius ) {
                flo32 tHeight = Clamp01( ( flo32 )iRow / ( flo32 )Height ) * 2.0f - 1.0f;
                flo32 Radians = acosf( tHeight );
                flo32 Sin     = sinf( Radians );
                flo32 tGray   = lerp( GrayB, Sin, GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "SecurityDrone_Turret_End", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Gunner Barrel
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 Margin = 32.0f;
        
        vec2  Center = Vec2( ( flo32 )Width, ( flo32 )Height * 0.5f );
        flo32 Radius = ( flo32 )Width;
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.0f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos        = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 tX         = Clamp01( Pos.x / ( flo32 )Width );
            flo32 yMin       = Margin * tX;
            flo32 yMax       = ( flo32 )Height - yMin;
            
            if( ( Pos.y >= yMin ) && ( Pos.y <= yMax ) ) {
                flo32 Dist0   = Pos.y - yMin;
                flo32 Height0 = ( flo32 )Height - ( Margin * tX * 2.0f );
                flo32 tHeight = Clamp01( Dist0 / Height0 ) * 2.0f - 1.0f;
                flo32 Radians = acosf( tHeight );
                flo32 Sin     = sinf( Radians );
                flo32 tGray   = lerp( GrayB, Sin, GrayA );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "Gunner_Barrel", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Missile Launcher Barrel
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 Margin = 32.0f;
        
        rect  Bound  = Rect( 0.0f, Margin, ( flo32 )Width, ( flo32 )Height - Margin );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.65f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist  = DistPointToRect( Pos, Bound );
            flo32 tDist = Clamp01( Dist / Margin );
            flo32 tGray = lerp( GrayA, tDist, GrayB );
            Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
        }
        AddTexture( AssetOutput, "MissileLauncher_Barrel", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Missile Launcher Barrel Back End
        int32 Width  = 128;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 Margin = 32.0f;
        
        rect  Bound  = Rect( 0.0f, Margin, ( flo32 )Width - Margin, ( flo32 )Height - Margin );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.65f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist  = DistPointToRect( Pos, Bound );
            if( Dist <= Margin ) {
                flo32 tDist = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayA, tDist, GrayB );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "MissileLauncher_Barrel_BackEnd", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Missile Launcher Front End
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 Margin = 32.0f;
        
        rect  Bound  = Rect( Margin, Margin, ( flo32 )Width - Margin, ( flo32 )Height - Margin );
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist  = DistPointToRect( Pos, Bound );
            if( Dist <= Margin ) {
                Texture[ iTexel ] = ToU32Color( COLOR_WHITE );
            }
        }
        AddTexture( AssetOutput, "MissileLauncher_Barrel_FrontEnd", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Missile Launcher Health
        int32 Width  = 256;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 Margin = 32.0f;
        
        rect  Bound  = Rect( Margin, Margin, ( flo32 )Width - Margin, ( flo32 )Height - Margin );
        
        flo32 GrayA = 1.0f;
        flo32 GrayB = 0.65f;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos   = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            flo32 Dist  = DistPointToRect( Pos, Bound );
            if( Dist <= Margin ) {
                flo32 tDist = Clamp01( Dist / Margin );
                flo32 tGray = lerp( GrayA, tDist, GrayB );
                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
            }
        }
        AddTexture( AssetOutput, "MissileLauncher_Health", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Mouse
        int32 Width  = 192;
        int32 Height = 256;
        int32 nTexel = Width * Height;
        
        flo32 Radius  = ( flo32 )Width * 0.5f;
        vec2  Center0 = Vec2( Radius, ( flo32 )Height - Radius );
        vec2  Center1 = Vec2( Radius, Radius );
        rect  Bound   = Rect( 0.0f, Radius, ( flo32 )Width, ( flo32 )Height - Radius );
        
        flo32 Line_Width0 = 12.0f;
        flo32 Line_Width1 = Line_Width0 - 2.0f;
        flo32 Line_Gap    =  6.0f;
        
        CAPSULE2 Body_Outer = Capsule2( Center0, Center1, Radius );
        CAPSULE2 Body_Inner = Capsule2( Center0, Center1, Radius - Line_Width0 );
        
        rect Line0 = Rect( Line_Width0 + Line_Gap, ( flo32 )Height - Radius - Line_Width0 - Line_Width1, ( flo32 )Width - Line_Width0 - Line_Gap, ( flo32 )Height - Radius - Line_Width0 );
        rect Line1 = RectBCD( Vec2( Radius, 0.0f ), Vec2( Line_Width1, ( flo32 )Height ) );
        Line1.Top    -= ( Line_Width0 + Line_Gap );
        Line1.Bottom  = Line0.Top + Line_Gap;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            int32 iRow = ( iTexel / Width );
            int32 iCol = ( iTexel % Width );
            
            vec2  Pos = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
            
            boo32 IsInOutline = ( IsInCapsule( Pos, Body_Outer ) ) && ( !IsInCapsule( Pos, Body_Inner ) );
            boo32 IsInLine0   = IsInRect( Pos, Line0 );
            boo32 IsInLine1   = IsInRect( Pos, Line1 );
            
            if( ( IsInOutline ) || ( IsInLine0 ) || ( IsInLine1 ) ) {
                Texture[ iTexel ] = ToU32Color( COLOR_WHITE );
            }
        }
        AddTexture( AssetOutput, "HowToPlay_Mouse", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    { // Create and Upload Texture for Background
        int32 Width  = 1920;
        int32 Height = 1080;
        int32 nTexel = Width * Height;
        
        uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
        
        for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
            Texture[ iTexel ] = ToU32Color( COLOR_BLACK );
        }
        
        int32 nStar = 256;
        for( int32 iStar = 0; iStar < nStar; iStar++ ) {
            int32 iRow = RandomS32InRange( 0, Height - 1 );
            int32 iCol = RandomS32InRange( 0, Width  - 1 );
            
            flo32 tGray = RandomF32InRange( 0.1f, 0.75f );
            vec4  Color = COLOR_GRAY( tGray );
            
            int32 iTexel = iRow * Width + iCol;
            Texture[ iTexel ] = ToU32Color( Color );
        }
        
        AddTexture( AssetOutput, "Background_01", Width, Height, Texture );
        
        _PopArray( TempMemory, uint32, nTexel );
    }
    
    AddTexture( AssetOutput, "Item_None", 0, 0, 0 );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Item_Blaster"     );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Item_GrappleBeam" );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Item_TractorBeam" );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Item_GravityWell" );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Item_Shield"      );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Item_Collector"   );
    
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "ObservationDeck_Icon" );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Cow" );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Spacesuit" );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Player_Avatar" );
    AddBMP( AssetOutput, TempMemory, ART_SAVE_DIRECTORY, "Icon_LevelComplete" );
    
    WriteSegment( AssetID_Output, "\n" );
    WriteSegment( AssetID_Output, "   TextureID_Count,\n" );
    WriteSegment( AssetID_Output, "};\n\n" );
    
    *nTexture = AssetOutput->nTexture;
    
    { // MESHES
        int32 * nMesh = _PushType( Data_Output, int32 );
        
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_Line,            GenMesh_Line            );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_Rect,            GenMesh_Rect            );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_RectOutline,     GenMesh_RectOutline     );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_Circle,          GenMesh_Circle          );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_CircleOutline,   GenMesh_CircleOutline   );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_Triangle,        GenMesh_Triangle        );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_Triangle_FlipUV, GenMesh_Triangle_FlipUV );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_TriangleOutline, GenMesh_TriangleOutline );
        
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_ObservationDeck_Barrier, GenMesh_ObservationDeck_Barrier );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_ObservationDeck_Icon, GenMesh_ObservationDeck_Icon );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_BlimpA, GenMesh_BlimpA );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_BlimpB, GenMesh_BlimpB );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_BlimpC, GenMesh_BlimpC );
        PackGeneratedMesh( AssetOutput, TempMemory, MeshID_Turret_Shield_EndCap, GenMesh_Turret_Shield_EndCap );
        
        PackXFile( AssetOutput, ReadFile, TempMemory, ART_SAVE_DIRECTORY, "Cow",       MeshID_Cow );
        PackXFile( AssetOutput, ReadFile, TempMemory, ART_SAVE_DIRECTORY, "Spacesuit", MeshID_Spacesuit );
        
        *nMesh = AssetOutput->nMesh;
    }
    
    { // LEVELS
        int32 * nLevel = _PushType( Data_Output, int32 );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, 0,          LevelID_Market          );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level01a", LevelID_Blimp           );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level01b", LevelID_Turret          );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level02a", LevelID_TurretShield    );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level02b", LevelID_BombLauncher    );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level03a", LevelID_AutoTurret      );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level04a", LevelID_PlayerShield    );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level04b", LevelID_MegaMiner       );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level05" , LevelID_GravityWell     );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level07",  LevelID_Worm            );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level08",  LevelID_Collector       );
        PackLevel( AssetOutput, TempMemory, LEVEL_SAVE_DIRECTORY, "Level09",  LevelID_ObservationDeck );
        *nLevel = AssetOutput->nLevel;
    }
    
    { // MUSIC
        int32 * nAudio = _PushType( Data_Output, int32 );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_TitleScreen_Intro", AudioID_Music_TitleScreen_Intro );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_TitleScreen_Loop", AudioID_Music_TitleScreen_Loop );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track01_Intro", AudioID_Music_Level_Track01_Intro );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track01_Loop", AudioID_Music_Level_Track01_Loop );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track03_Intro", AudioID_Music_Level_Track03_Intro );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track03_Loop", AudioID_Music_Level_Track03_Loop );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track04_Intro", AudioID_Music_Level_Track04_Intro );
        PackWavFile( AssetOutput, ReadFile, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track04_Loop", AudioID_Music_Level_Track04_Loop );
        *nAudio = AssetOutput->nAudio;
    }
    
    WriteFile( "Asset.pak",                    Data_Output->Base,    Data_Output->Used );
    WriteFile( "../../code/AssetID_Temp.h", AssetID_Output->Base, AssetID_Output->Used );
    
    return 0;
}