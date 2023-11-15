
#ifdef	STD_INCLUDE_DECL

#define FONT__START_CHAR      ( '!' )
#define FONT__END_CHAR        ( '~' )
#define FONT__CHAR_COUNT      ( '~' - '!' + 1 )

struct FONT_CHAR {
    vec2  Dim;
    vec2  Offset;
    vec2  TexCoord_Min;
    vec2  TexCoord_Max;
    flo32 AdvanceWidth;
};

enum FONT_ID {
    FontID_0_75,
    FontID_0_90,
    FontID_1_00,
    FontID_1_25,
    FontID_1_35,
    FontID_1_50,
    FontID_1_75,
    FontID_2_00,
    FontID_2_25,
    FontID_2_50,
    FontID_2_75,
    FontID_3_00,
    FontID_7_00,
    
    FontID_Title_1_00,
    FontID_Title_2_00,
    FontID_Title_2_50,
    FontID_Title_3_00,
    FontID_Title_3_50,
    FontID_Title_4_00,
    FontID_Title_14_00,
    
    FontID_Count,
};

struct FONT {
    int32 TextureID;
    
    flo32 AdvanceWidth_Space;
    flo32 Ascent;
    flo32 Descent;
    flo32 LineGap;
    flo32 AdvanceHeight;
    
    FONT_CHAR Alphabet[ FONT__CHAR_COUNT ];
    
    uint32 * Texture_Data;
    uint32   Texture_Width;
    uint32   Texture_Height;
};

enum TEXT_ALIGNMENT {
    TextAlign_default,
    
    TextAlign_BottomLeft,
    TextAlign_BottomCenter,
    TextAlign_BottomRight,
    
    TextAlign_CenterLeft,
    TextAlign_Center,
    TextAlign_CenterRight,
    
    TextAlign_TopLeft,
    TextAlign_TopCenter,
    TextAlign_TopRight,
    
    TextAlign_BaseLeft,
    TextAlign_BaseCenter,
    TextAlign_BaseRight,
    
    TextAlign_Pixel_Center,
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

struct STRING_DIM {
    union {
        struct {
            vec2  Dim;
        };
        struct {
            flo32 Width;
            flo32 Height;
        };
    };
    flo32 yOffsetToBaseline;
};

internal STRING_DIM
GetDimOfString( FONT * Font, const char * String, int32 nChar = -1, vec2 Scale = Vec2( 1.0f, 1.0f ) ) {
    // NOTE : This function returns the dimensions of the string in pixels. It only uses the pixels that are actually drawn and any internal white space. External white space is excluded.
    // NOTE : This function assumes that character bitmaps are being drawn to the nearest pixel. They are not being drawn subpixel.
    
    if( nChar == -1 ) {
        nChar = ( int32 )strlen( String );
    }
    
    STRING_DIM Result = {};
    if( nChar > 0 ) {
        vec2 AtPos = {};
        rect Bound = {};
        
        {
            char C = String[ 0 ];
            if( C == ' ' ) {
                Bound.Left   = AtPos.x;
                Bound.Bottom = AtPos.y;
                Bound.Right  = AtPos.x + Font->AdvanceWidth_Space;
                Bound.Top    = AtPos.y;
                
                AtPos.x += Font->AdvanceWidth_Space;
            } else {
                int32     Char_Index = C - FONT__START_CHAR;
                FONT_CHAR Char       = Font->Alphabet[ Char_Index ];
                
                Bound.Left   = AtPos.x;
                Bound.Bottom = AtPos.y + Font->Descent;
                Bound.Right  = AtPos.x + Char.AdvanceWidth;
                Bound.Top    = AtPos.y + Font->Ascent;
                
                AtPos.x += Char.AdvanceWidth;
            }
        }
        
        for( int32 iChar = 1; iChar < nChar; iChar++ ) {
            AtPos.x = ( flo32 )RoundToInt32( AtPos.x );
            AtPos.y = ( flo32 )RoundToInt32( AtPos.y );
            
            char C = String[ iChar ];
            if( C == ' ' ) {
                flo32 xMin = AtPos.x;
                flo32 xMax = AtPos.x + Font->AdvanceWidth_Space;
                
                Bound.Left  = MinValue( xMin, Bound.Left  );
                Bound.Right = MaxValue( xMax, Bound.Right );
                
                AtPos.x += Font->AdvanceWidth_Space;
            } else {
                int32     Char_Index = C - FONT__START_CHAR;
                FONT_CHAR Char       = Font->Alphabet[ Char_Index ];
                
                Bound.Left   = MinValue( AtPos.x, Bound.Left   );
                Bound.Right  = MaxValue( AtPos.x + Char.AdvanceWidth, Bound.Right );
                
                AtPos.x += Char.AdvanceWidth;
            }
        }
        
        Bound.Left  = ( flo32 )RoundToInt32( Bound.Left  );
        Bound.Right = ( flo32 )RoundToInt32( Bound.Right );
        
        Result.Width  = ( Bound.Right - Bound.Left   ) * Scale.x;
        Result.Height = ( Bound.Top   - Bound.Bottom ) * Scale.y;
        Result.yOffsetToBaseline = -Bound.Bottom * Scale.y;
    }
    
    return Result;
}

internal STRING_DIM
GetDimOfStringBitmap( FONT * Font, const char * String, int32 nChar = -1, vec2 Scale = Vec2( 1.0f, 1.0f ) ) {
    // NOTE : This function returns the dimensions of the string in pixels. It only uses the pixels that are actually drawn and any internal white space. External white space is excluded.
    // NOTE : This function assumes that character bitmaps are being drawn to the nearest pixel. They are not being drawn subpixel.
    
    if( nChar == -1 ) {
        nChar = ( int32 )strlen( String );
    }
    
    STRING_DIM Result = {};
    if( nChar > 0 ) {
        vec2 AtPos = {};
        rect Bound = {};
        
        {
            char C = String[ 0 ];
            if( C == ' ' ) {
                Bound.Left   = AtPos.x;
                Bound.Bottom = AtPos.y;
                Bound.Right  = AtPos.x + Font->AdvanceWidth_Space;
                Bound.Top    = AtPos.y;
                
                AtPos.x += Font->AdvanceWidth_Space;
            } else {
                int32     Char_Index = C - FONT__START_CHAR;
                FONT_CHAR Char       = Font->Alphabet[ Char_Index ];
                
                Bound.Left   = AtPos.x + Char.Offset.x;
                Bound.Bottom = AtPos.y + Char.Offset.y - Char.Dim.y;
                Bound.Right  = AtPos.x + Char.Offset.x + Char.Dim.x;
                Bound.Top    = AtPos.y + Char.Offset.y;
                
                AtPos.x += Char.AdvanceWidth;
            }
        }
        
        for( int32 iChar = 1; iChar < nChar; iChar++ ) {
            AtPos.x = ( flo32 )RoundToInt32( AtPos.x );
            AtPos.y = ( flo32 )RoundToInt32( AtPos.y );
            
            char C = String[ iChar ];
            if( C == ' ' ) {
                flo32 xMin = AtPos.x;
                flo32 xMax = AtPos.x + Font->AdvanceWidth_Space;
                
                Bound.Left  = MinValue( xMin, Bound.Left  );
                Bound.Right = MaxValue( xMax, Bound.Right );
                
                AtPos.x += Font->AdvanceWidth_Space;
            } else {
                int32     Char_Index = C - FONT__START_CHAR;
                FONT_CHAR Char       = Font->Alphabet[ Char_Index ];
                
                Bound.Left   = MinValue( AtPos.x + Char.Offset.x,              Bound.Left   );
                Bound.Bottom = MinValue( AtPos.y + Char.Offset.y - Char.Dim.y, Bound.Bottom );
                Bound.Right  = MaxValue( AtPos.x + Char.Offset.x + Char.Dim.x, Bound.Right  );
                Bound.Top    = MaxValue( AtPos.y + Char.Offset.y,              Bound.Top    );
                
                AtPos.x += Char.AdvanceWidth;
            }
        }
        
        Result.Width  = ( Bound.Right - Bound.Left   ) * Scale.x;
        Result.Height = ( Bound.Top   - Bound.Bottom ) * Scale.y;
        Result.yOffsetToBaseline = -Bound.Bottom * Scale.y;
    }
    
    return Result;
}

internal flo32
GetWidth( FONT * Font, const char * String ) {
    STRING_DIM Dim = GetDimOfString( Font, String );
    return Dim.Width;
}

internal flo32
GetHeight( FONT * Font, const char * String ) {
    STRING_DIM Dim = GetDimOfString( Font, String );
    return Dim.Height;
}

internal vec2
newLine( FONT * Font, uint32 nLines = 1 ) {
    // NOTE: This is just a hacky utility function so I don't have to do this everytime: Vec2( 0.0f, Font->AdvanceHeight * ( flo32 )nLines )
    vec2 Result = {};
    Result.y = -Font->AdvanceHeight * ( flo32 )nLines;
    return Result;
}

internal vec2
GetOffsetFromAlignment( FONT * Font, const char * String, vec2 Scale, TEXT_ALIGNMENT align ) {
    vec2 Result = {};
    
    STRING_DIM Dim           = GetDimOfString( Font, String );
    flo32      String_Width = Dim.Width;
	
    flo32 Width     = String_Width * Scale.x;
    flo32 HalfWidth = Width        * 0.5f;
    
    flo32 Ascent  = Font->Ascent  * Scale.y;
    flo32 Descent = Font->Descent * Scale.y;
    
    flo32 Height     = Ascent + Descent;
    flo32 HalfHeight = Height * 0.5f;
    
    switch( align ) {
        case TextAlign_BottomLeft:   { Result = Vec2(       0.0f, Descent ); } break;
        case TextAlign_BottomCenter: { Result = Vec2( -HalfWidth, Descent ); } break;
        case TextAlign_BottomRight:  { Result = Vec2(     -Width, Descent ); } break;
        
        case TextAlign_CenterLeft:   { Result = Vec2(       0.0f, -HalfHeight + Descent ); } break;
        case TextAlign_Center:       { Result = Vec2( -HalfWidth, -HalfHeight + Descent ); } break;
        case TextAlign_CenterRight:  { Result = Vec2(     -Width, -HalfHeight + Descent ); } break;
        
        case TextAlign_TopLeft:      { Result = Vec2(       0.0f, -Ascent ); } break;
        case TextAlign_TopCenter:    { Result = Vec2( -HalfWidth, -Ascent ); } break;
        case TextAlign_TopRight:     { Result = Vec2(     -Width, -Ascent ); } break;
        
        case TextAlign_default:
        case TextAlign_BaseLeft:     {} break;
        case TextAlign_BaseCenter:   { Result = Vec2( -HalfWidth, 0.0f ); } break;
        case TextAlign_BaseRight:    { Result = Vec2(     -Width, 0.0f ); } break;
        
        case TextAlign_Pixel_Center : {
            Dim    = GetDimOfStringBitmap( Font, String );
            Result = Vec2( -Dim.Width * 0.5f * Scale.x, -Dim.Height * 0.5f * Scale.y + Dim.yOffsetToBaseline * Scale.y ); } break;
        
        default: { InvalidCodePath; } break;
    };
    
    return Result;
}

internal void
GenFilePath( char * FilePath, char * SaveDir, char * FileName, char * extension ) {
	if( SaveDir ) {
        strcpy( FilePath, SaveDir );
        strcat( FilePath, "/" );
	}
    strcat( FilePath, FileName );
    if( extension ) {
        strcat( FilePath, "." );
        strcat( FilePath, extension );
    }
}

internal FONT
LoadFont( char * SaveDir, char * Font_Name, flo32 Font_Height ) {
    FONT Result = {};
    
    char File_Path[ 128 ] = {};
    GenFilePath( File_Path, SaveDir, Font_Name, "ttf" );
    
    FILE * FontFile = fopen( File_Path, "rb" );
    if( FontFile ) {
        fseek( FontFile, 0, SEEK_END );
        uint32 FontFile_bytes = ftell( FontFile );
        fseek( FontFile, 0, SEEK_SET );
        
        uint8 * Memory = ( uint8 * )calloc( FontFile_bytes, 1 );
        uint32 bytesRead = ( uint32 )fread( Memory, 1, FontFile_bytes, FontFile );
        Assert( FontFile_bytes == bytesRead );
        
        stbtt_Fontinfo FontInfo = {};
        boo32 FontFileIsValid = stbtt_InitFont( &FontInfo, ( uint8 * )Memory, 0 );
        Assert( FontFileIsValid );
        
        flo32 Scale = stbtt_ScaleForPixelHeight( &FontInfo, Font_Height );
        
        int32 Ascent = 0;
        int32 Descent = 0;
        int32 LineGap = 0;
        stbtt_GetFontVMetrics( &FontInfo, &Ascent, &Descent, &LineGap );
        
        Result.Ascent  = fabsf( Scale * ( flo32 )Ascent  );
        Result.Descent = fabsf( Scale * ( flo32 )Descent );
        Result.LineGap = fabsf( Scale * ( flo32 )LineGap );
        Result.AdvanceHeight = Result.Ascent + Result.Descent + Result.LineGap;
        
        int32 Cell_maxWidth = 0;
        int32 Cell_maxHeight = 0;
        
        int32 maxIndex = FONT__CHAR_COUNT;
        for( int32 index = 0; index < maxIndex; index++ ) {
            int32 x0 = 0;
            int32 x1 = 0;
            int32 y0 = 0;
            int32 y1 = 0;
            
            int32 char_index = index + '!';
            
            stbtt_GetCodepointBitmapBox( &FontInfo, char_index, Scale, Scale, &x0, &y0, &x1, &y1 );
            
            int32 Width  = x1 - x0;
            int32 Height = y1 - y0;
            
            Cell_maxWidth  = MaxValue( Cell_maxWidth, Width );
            Cell_maxHeight = MaxValue( Cell_maxHeight, Height );
        }
        
        // 1-pixel apron around char
        Cell_maxWidth  += 1;
        Cell_maxHeight += 1;
        
        int32 Cell_xCount = 10;
        int32 Cell_yCount = 10;
        int32 Cell_Count  = Cell_xCount * Cell_yCount;
        int32 pixel_Count = Cell_Count * Cell_maxWidth * Cell_maxHeight;
        uint32 * Texture_Data = ( uint32 * )calloc( pixel_Count, sizeof( uint32 ) );
        
        uint32 Texture_Width  = Cell_maxWidth  * Cell_xCount;
        uint32 Texture_Height = Cell_maxHeight * Cell_yCount;
        uint32 xPixel = Texture_Width;
        uint32 yPixel = Texture_Height;
        
        int32 AdvanceWidth    = 0;
        int32 LeftSideBearing = 0;
        
        int32 char_Count = FONT__CHAR_COUNT;
        for( int32 index = 0; index < char_Count; index++ ) {
            int32 Width   = 0;
            int32 Height  = 0;
            int32 xOffset = 0;
            int32 yOffset = 0;
            
            int32 char_index = index + FONT__START_CHAR;
            
            uint8 * bitmap = stbtt_GetCodepointBitmap( &FontInfo, 0, Scale, char_index, &Width, &Height, &xOffset, &yOffset );
            stbtt_GetCodepointHMetrics( &FontInfo, char_index, &AdvanceWidth, &LeftSideBearing );
            
            int32 xCell = index % Cell_yCount;
            int32 yCell = index / Cell_yCount;
            int32 xMin = xCell * Cell_maxWidth;
            int32 yMin = yCell * Cell_maxHeight;
            
            FONT_CHAR _char = {};
            _char.Dim				= Vec2( ( flo32 )Width, 	( flo32 )Height );
            _char.Offset         = Vec2( ( flo32 )xOffset, 	( flo32 )-yOffset );
            _char.TexCoord_Min.x = ( flo32 )xMin / ( flo32 )xPixel;
            _char.TexCoord_Min.y = ( flo32 )yMin / ( flo32 )yPixel;
            _char.TexCoord_Max.x = ( flo32 )( xMin + Width  ) / ( flo32 )xPixel;
            _char.TexCoord_Max.y = ( flo32 )( yMin + Height ) / ( flo32 )yPixel;
            _char.AdvanceWidth   = ( ( flo32 )AdvanceWidth * Scale );
            Result.Alphabet[ index ] = _char;
            
            uint32 * destCell = Texture_Data + ( ( index / Cell_yCount ) * xPixel * Cell_maxHeight ) + ( ( index % Cell_yCount ) * Cell_maxWidth );
            
            uint8  * srcRow  = bitmap + ( ( Height - 1 ) * Width );
            uint32 * destRow = destCell;
            for( int32 y = 0; y < Height; y++ ) {
                uint8 * srcPixel  = srcRow;
                uint8 * destPixel = ( uint8 * )destRow;
                for( int32 x = 0; x < Width; x++ ) {
                    uint8 alpha = *srcPixel++;
                    
                    flo32 _alpha   = ( flo32 )alpha / 255.0f;
                    flo32 _alphaSq = _alpha * _alpha;
                    uint8 rgb = ( uint8 )( _alphaSq * 255.0f );
                    
                    *destPixel++ = rgb;
                    *destPixel++ = rgb;
                    *destPixel++ = rgb;
                    *destPixel++ = alpha;
                }
                srcRow  -= Width;
                destRow += xPixel;
            }
            
            stbtt_FreeBitmap( bitmap, 0 );
        }
        
        stbtt_GetCodepointHMetrics( &FontInfo, ' ', &AdvanceWidth, &LeftSideBearing );
        Result.AdvanceWidth_Space = ( ( flo32 )AdvanceWidth * Scale );
        
        free( Memory );
        
        Result.Texture_Data   = Texture_Data;
        Result.Texture_Width  = Texture_Width;
        Result.Texture_Height = Texture_Height;
    } else {
        InvalidCodePath;
    }
    
    return Result;
}

#endif	// STD_INCLUDE_FUNC