

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
	uint16 BitsPerPixel;         // BITS per pixel, not BYTES!! Multiply bytes by 8.
	uint32 CompressionType;      // UNUSED
	uint32 BitmapSize_InBytes;   // UNUSED
	uint32 xPixelsPerMeter;      // UNUSED
	uint32 yPixelsPerMeter;      // UNUSED
	uint32 ColorCount;           // UNUSED
	uint32 ImportantColorCount;  // UNUSED
};
#pragma pack( pop )

struct TEXTURE_DATA {
	uint32  Width;
	uint32  Height;
	uint32  BytesPerPixel;
    uint8 * TexelData;
};

internal TEXTURE_DATA
BMP_ReadFile( PLATFORM * Platform, MEMORY * DestMemory, char * SaveDir, char * FileName ) {
	TEXTURE_DATA Result = {};
	
	// TODO: verify requested File extension
	FILE_DATA File = Platform->ReadFile( &Platform->TempMemory, SaveDir, FileName, "bmp" );
	
	if( File.Contents ) {
		BMP_HEADER * info = ( BMP_HEADER * )File.Contents;
		Result.Width         = info->Width;
		Result.Height        = info->Height;
		Result.BytesPerPixel = 4;
		
		uint32 BMP_bytesPerPixel = info->BitsPerPixel / 8;
		
		uint8 * BMP_Data = ( uint8 * )File.Contents + info->OffsetToData;
		uint8 * ptr = BMP_Data;
		
		uint32 pixelCount  = Result.Width * Result.Height;
		uint32 TextureSize = pixelCount * Result.BytesPerPixel;
		Result.TexelData = ( uint8 * )_PushSize( DestMemory, TextureSize );
		
		flo32 inv255 = 1.0f / 255.0f;
		uint32 * ResultData  = ( uint32 * )Result.TexelData;
        
        if( BMP_bytesPerPixel == 4 ) {
            uint32 * TextureData = ( uint32 * )BMP_Data;
            for( uint32 counter = 0; counter < pixelCount; counter++ ) {
                // TODO: RGBA vs BGRA
                uint32 inColor = TextureData[ counter ];
                
                uint8 r = ( uint8 )( ( inColor >>  0 ) & 0xFF );
                uint8 g = ( uint8 )( ( inColor >>  8 ) & 0xFF );
                uint8 b = ( uint8 )( ( inColor >> 16 ) & 0xFF );
                uint8 a = ( uint8 )( ( inColor >> 24 ) & 0xFF );
                flo32 alpha = ( flo32 )a * inv255;
                
                uint32 outColor =
                    ( ( uint8 )( ( flo32 )b * alpha ) <<  0 ) |
                    ( ( uint8 )( ( flo32 )g * alpha ) <<  8 ) |
                    ( ( uint8 )( ( flo32 )r * alpha ) << 16 ) |
                    ( a << 24 );
                
                ResultData[ counter ] = outColor;
            }
        } else if( BMP_bytesPerPixel == 3 ) {
            uint8 * TextureData = BMP_Data;
            for( uint32 counter = 0; counter < pixelCount; counter++ ) {
                // TODO: RGBA vs BGRA
                uint8 r = *TextureData++;
                uint8 g = *TextureData++;
                uint8 b = *TextureData++;
                uint8 a = 255;
                
                uint32 outColor = ( b <<  0 ) | ( g <<  8 ) | ( r << 16 ) | ( a << 24 );
                
                ResultData[ counter ] = outColor;
            }
        }
		
		_PopSize( &Platform->TempMemory, File.Size );
	}
	
	return Result;
}

internal void
BMP_WriteFile( PLATFORM * Platform, MEMORY * TempMemory, char * SaveDir, char * FileName, int32 Width, int32 Height, uint32 * Texture ) {
    MEMORY  _Output = SubArena( TempMemory );
    MEMORY * Output = &_Output;
    
    BMP_HEADER * Header = _PushType( Output, BMP_HEADER );
    Header->FileType             = 0x4d42;
    Header->FileSize             = sizeof( BMP_HEADER ) + Width * Height * sizeof( uint32 );
    Header->OffsetToData         = sizeof( BMP_HEADER );
    Header->BitmapInfoHeaderSize = 40;
    Header->Width                = Width;
    Header->Height               = Height;
    Header->Planes               = 1;
    Header->BitsPerPixel         = 32;
    
    int32    Texture_Size = Width * Height * sizeof( uint32 );
    uint32 * Texture_Data = ( uint32 * )_PushSize( Output, Texture_Size );
    memcpy( Texture_Data, Texture, Texture_Size );
    
    boo32 Result = Platform->WriteFile( SaveDir, FileName, "bmp", Output->Base, ( uint32 )Output->Used );
    Assert( Result );
    
    _PopSize( TempMemory, Output->Size );
}