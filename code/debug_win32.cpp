
//----------
// Debug declarations
//----------

#if DEBUG_BUILD

internal void CONSOLE_STRING( const char * String );

struct DEBUG_SYSTEM {
    boo32 ShowDebug;
    boo32 ErrorOccurred;
    RENDER_PASS * Pass_Game;
    RENDER_PASS * Pass_UI;
    FONT        * Font;
    vec2       AtPos;
    flo32      AdvanceHeight;
};
DEBUG_SYSTEM GlobalVar_DebugSystem = {};

#define DISPLAY_VALUE( Type, name ) {\
char __Debug_String[ 2048 ] = {};\
serialize_##Type( __Debug_String, #name, name );\
DISPLAY_STRING( __Debug_String );\
}
#define CONSOLE_VALUE( Type, name ) {\
char __Debug_String[ 2048 ] = {};\
serialize_##Type( __Debug_String, #name, name );\
strcat( __Debug_String, "\n" );\
OutputDebugString( __Debug_String );\
}
#define CONSOLE_VALUE_HEX( Type, name ) {\
char __Debug_String[ 2048 ] = {};\
serialize_hex_##Type( __Debug_String, #name, name );\
strcat( __Debug_String, "\n" );\
OutputDebugString( __Debug_String );\
}
#define CONSOLE_ERROR( String ) {\
OutputDebugString( String );\
GlobalVar_DebugSystem.ErrorOccurred = true;\
}

#else
#define DISPLAY_STRING( Str )
#define DISPLAY_STRING( Str, Color )
#define DISPLAY_VALUE( Type, Name )
#define CONSOLE_STRING( Str )
#define CONSOLE_VALUE( Type, Name )
#define CONSOLE_ERROR( Str )

internal void PROFILE_String( char * String ) {}
internal void PROFILE_tooltip( char * String, vec2 Position ) {}

#endif

//----------
// Debug functions
//----------

#if DEBUG_BUILD
#define DEBUG_SYSTEM__MAX_VERTEX_COUNT		( 2097152 )
#define DEBUG_SYSTEM__MAX_MEMORY_SIZE    _MB( 2 )
#define DEBUG_SYSTEM__SIZE_IN_BYTES  		( sizeof( RENDER_SYSTEM ) + DEBUG_SYSTEM__MAX_MEMORY_SIZE )

internal void
CONSOLE_STRING( const char * String ) {
	OutputDebugString( String );
	OutputDebugString( "\n" );
}

#if 0
internal void
DEBUG_addTask( ASSET_ID TextureID, rect Bound, vec4 Color ) {
    RENDERER * Renderer = GlobalVar_DebugSystem.Renderer;
    
    int32 vertex_index    = Renderer->vertex_Count[ 0 ];
    int32 vertex_Count    = 4;
	DEBUG_VERTEX * vertex = ( ( DEBUG_VERTEX * )Renderer->vertex_ptr[ 0 ] ) + vertex_index;
    
    vec2 pos[ 4 ] = {
        Vec2( Bound.Left,  Bound.Bottom ),
        Vec2( Bound.Right, Bound.Bottom ),
        Vec2( Bound.Left,  Bound.Top ),
        Vec2( Bound.Right, Bound.Top ),
    };
    
    vec2 tex[ 4 ] = {
        Vec2( 0.0f, 0.0f ),
        Vec2( 1.0f, 0.0f ),
        Vec2( 0.0f, 1.0f ),
        Vec2( 1.0f, 1.0f ),
    };
    
    *vertex++ = DebugVertex( pos[ 0 ], tex[ 0 ] );
    *vertex++ = DebugVertex( pos[ 1 ], tex[ 1 ] );
    *vertex++ = DebugVertex( pos[ 2 ], tex[ 2 ] );
    *vertex++ = DebugVertex( pos[ 3 ], tex[ 3 ] );
    
    addVertex( Renderer, 0, vertex_Count );
    addObject( Renderer, RenderPassID_Debug, RenderObject( meshDrawType_triangleStrip, vertex_index, vertex_Count, TextureID, Color ) );
}
#endif
#endif

internal void
DrawString_noErrorCheck( RENDER_PASS * Pass, FONT * Font, const char * String, vec2 Position, vec2 Scale, vec4 Color ) {
    VERTEX1_BUFFER * buffer = Pass->textBuffer;
    VERTEX1 * vertex = buffer->vertex_Start + buffer->nVertex;
    
    uint32 bVertex = buffer->nVertex;
    
    int32 nChar = ( int32 )strlen( String );
    
    vec2 AtPos = Position;
    for( int32 iChar = 0; iChar < nChar; iChar++ ) {
        char c = String[ iChar ];
        if( c != ' ' ) {
            int32 char_index = c - FONT__START_CHAR;
            FONT_CHAR _char = Font->Alphabet[ char_index ];
            
            vec2 Offset = _char.Offset * Scale;
            vec2 Dim    = _char.Dim    * Scale;
            
            flo32 Left   = AtPos.x + Offset.x;
            flo32 Right  = Left    + Dim.x;
            flo32 Top    = AtPos.y + Offset.y;
            flo32 Bottom = Top     - Dim.y;
            rect Bound = Rect( Left, Bottom, Right, Top );
            
            vec3 pos[ 4 ] = {
                Vec3( Bound.Left,  Bound.Bottom, 0.0f ),
                Vec3( Bound.Right, Bound.Bottom, 0.0f ),
                Vec3( Bound.Left,  Bound.Top,    0.0f ),
                Vec3( Bound.Right, Bound.Top,    0.0f ),
            };
            
            vec2 tex[ 4 ] = {
                Vec2( _char.TexCoord_Min.x, _char.TexCoord_Min.y ),
                Vec2( _char.TexCoord_Max.x, _char.TexCoord_Min.y ),
                Vec2( _char.TexCoord_Min.x, _char.TexCoord_Max.y ),
                Vec2( _char.TexCoord_Max.x, _char.TexCoord_Max.y ),
            };
            
            *vertex++ = Vertex1( pos[ 0 ], tex[ 0 ] );
            *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
            *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
            *vertex++ = Vertex1( pos[ 3 ], tex[ 3 ] );
            *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
            *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
            
            buffer->nVertex += 6;
            
            AtPos.x += ( _char.AdvanceWidth * Scale.x );
        } else {
            AtPos.x += ( Font->AdvanceWidth_Space * Scale.x );
        }
    }
    
    TEXT_RENDER_OBJECT object = {};
    object.TextureID = TextureID_Font;
    object.bVertex   = bVertex;
    object.nVertex   = buffer->nVertex - bVertex;
    object.modColor  = Color;
    
    Assert( object.TextureID < 1000 );
    
    TEXT_RENDER_OBJECT_LIST * TextList = &Pass->TextList;
    TextList->object[ TextList->nObjects++ ] = object;
}

internal void
DrawString( RENDER_PASS * Pass, FONT * Font, const char * String, vec2 Position, vec2 Scale, vec4 Color ) {
    VERTEX1_BUFFER * buffer = Pass->textBuffer;
    VERTEX1 * vertex = buffer->vertex_Start + buffer->nVertex;
    
    TEXT_RENDER_OBJECT_LIST * TextList = &Pass->TextList;
    
    int32 nChar = ( int32 )strlen( String );
    int32 charLeft = ( ( buffer->maxVertex - buffer->nVertex ) / 6 );
    
    if( ( TextList->nObjects < TextList->maxObjects ) && ( nChar < charLeft ) ) {
        uint32 bVertex = buffer->nVertex;
        
        vec2 AtPos = Position;
        for( int32 iChar = 0; iChar < nChar; iChar++ ) {
            // NOTE : If we are scaling a font by anything other than 1.0, then we should assume that it will look blurry-ish. Scale should always be 1.0, except for prototype/testing purposes. This means that we only need to round AtPos to a whole pixel boundary. We do not need to round  Offset  or  Dim.
            AtPos.x = ( flo32 )RoundToInt32( AtPos.x );
            AtPos.y = ( flo32 )RoundToInt32( AtPos.y );
            
            char c = String[ iChar ];
            if( c != ' ' ) {
                int32 char_index = c - FONT__START_CHAR;
                FONT_CHAR _char = Font->Alphabet[ char_index ];
                
                vec2 Offset = _char.Offset * Scale;
                vec2 Dim    = _char.Dim    * Scale;
                
                flo32 Left   = AtPos.x + Offset.x;
                flo32 Right  = Left    + Dim.x;
                flo32 Top    = AtPos.y + Offset.y;
                flo32 Bottom = Top     - Dim.y;
                rect Bound = Rect( Left, Bottom, Right, Top );
                
                vec3 pos[ 4 ] = {
                    Vec3( Bound.Left,  Bound.Bottom, 0.0f ),
                    Vec3( Bound.Right, Bound.Bottom, 0.0f ),
                    Vec3( Bound.Left,  Bound.Top,    0.0f ),
                    Vec3( Bound.Right, Bound.Top,    0.0f ),
                };
                
                vec2 tex[ 4 ] = {
                    Vec2( _char.TexCoord_Min.x, _char.TexCoord_Min.y ),
                    Vec2( _char.TexCoord_Max.x, _char.TexCoord_Min.y ),
                    Vec2( _char.TexCoord_Min.x, _char.TexCoord_Max.y ),
                    Vec2( _char.TexCoord_Max.x, _char.TexCoord_Max.y ),
                };
                
                *vertex++ = Vertex1( pos[ 0 ], tex[ 0 ] );
                *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
                *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
                *vertex++ = Vertex1( pos[ 3 ], tex[ 3 ] );
                *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
                *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
                
                buffer->nVertex += 6;
                
                AtPos.x += ( _char.AdvanceWidth * Scale.x );
            } else {
                AtPos.x += ( Font->AdvanceWidth_Space * Scale.x );
            }
        }
        
        TEXT_RENDER_OBJECT object = {};
        object.TextureID = ( TEXTURE_ID )Font->TextureID;
        object.bVertex   = bVertex;
        object.nVertex   = buffer->nVertex - bVertex;
        object.modColor  = Color;
        
        Assert( object.TextureID < 1000 );
        
        TextList->object[ TextList->nObjects++ ] = object;
    } else {
        char str[ 512 ] = {};
        if( TextList->nObjects >= TextList->maxObjects ) {
            sprintf( str, "ERROR! Text Object List ran out of objects for rendering text! Max Count = %u", TextList->maxObjects );
            CONSOLE_ERROR( str );
        }
        if( nChar >= charLeft ) {
            sprintf( str, "ERROR! Text Vertex Buffer ran out of space for vertices for rendering text! Max Count = %u", buffer->maxVertex );
            CONSOLE_ERROR( str );
        }
    }
}

internal void
DrawString( RENDER_PASS * Pass, FONT * Font, const char * String, vec2 Position, TEXT_ALIGNMENT align, vec4 Color ) {
    vec2 Offset = GetOffsetFromAlignment( Font, String, Vec2( 1.0f, 1.0f ), align );
    DrawString( Pass, Font, String, Position + Offset, Vec2( 1.0f, 1.0f ), Color );
}

internal void
DrawString( RENDER_PASS * Pass, FONT * Font, const char * String, vec2 Position, TEXT_ALIGNMENT align, vec2 Scale, vec4 Color ) {
    vec2 Offset = GetOffsetFromAlignment( Font, String, Scale, align );
    DrawString( Pass, Font, String, Position + Offset, Scale, Color );
}

#if DEBUG_BUILD
internal void
DEBUG_DrawString( char * String, vec2 Position, vec4 Color ) {
    RENDER_PASS * Pass = GlobalVar_DebugSystem.Pass_UI;
	FONT        * Font = GlobalVar_DebugSystem.Font;
    
    if( GlobalVar_DebugSystem.ShowDebug ) {
        DrawString( Pass, Font, String, Position, Vec2( 1.0f, 1.0f ), Color );
    }
}

#if 0
internal void
DEBUG_DrawRect( rect Bound, vec4 Color ) {
	DEBUG_addTask( assetID_Texture_whiteTexture, Bound, Color );
}
#endif

internal void
DISPLAY_STRING( char * String, vec4 Color = COLOR_WHITE ) {
	DEBUG_DrawString( String, GlobalVar_DebugSystem.AtPos, Color );
	GlobalVar_DebugSystem.AtPos.y += GlobalVar_DebugSystem.AdvanceHeight;
}

#if 0
internal void
DISPLAY_STRING( char * String, vec2 Position ) {
	DEBUG_DrawString( String, Position, COLOR_BLACK );
}
#endif

internal void
PROFILE_String( char * String ) {
	DEBUG_DrawString( String, GlobalVar_DebugSystem.AtPos, COLOR_WHITE );
	GlobalVar_DebugSystem.AtPos.y += GlobalVar_DebugSystem.AdvanceHeight;
}

internal void
PROFILE_tooltip( char * String, vec2 Position ) {
	DEBUG_DrawString( String, Position, COLOR_WHITE );
}

internal void serialize_int8  ( char * buffer, const char * name, int8   value ) {
    sprintf( buffer, "%s: %hhd",    name, value ); }
internal void serialize_uint8 ( char * buffer, const char * name, uint8  value ) {
    sprintf( buffer, "%s: %hhu",    name, value ); }
internal void serialize_int16 ( char * buffer, const char * name, int16  value ) { sprintf( buffer, "%s: %hd",    name, value ); }
internal void serialize_int32 ( char * buffer, const char * name, int32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_int64 ( char * buffer, const char * name, int64  value ) { sprintf( buffer, "%s: %lld",   name, value ); }
internal void serialize_uint16( char * buffer, const char * name, uint16 value ) { sprintf( buffer, "%s: %hu",    name, value ); }
internal void serialize_uint32( char * buffer, const char * name, uint32 value ) { sprintf( buffer, "%s: %u",     name, value ); }
internal void serialize_uint64( char * buffer, const char * name, uint64 value ) { sprintf( buffer, "%s: %llu",   name, value ); }
internal void serialize_boo32 ( char * buffer, const char * name, boo32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_flo32 ( char * buffer, const char * name, flo32  value ) { sprintf( buffer, "%s: %f",     name, value ); }
internal void serialize_vec2  ( char * buffer, const char * name, vec2   value ) { sprintf( buffer, "%s: %f, %f", name, value.x, value.y ); }
internal void serialize_vec3  ( char * buffer, const char * name, vec3   value ) { sprintf( buffer, "%s: %f, %f, %f", name, value.x, value.y, value.z ); }
internal void serialize_vec4  ( char * buffer, const char * name, vec4   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.x, value.y, value.z, value.w ); }
internal void serialize_quat  ( char * buffer, const char * name, quat   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.x, value.y, value.z, value.w ); }
internal void serialize_rect  ( char * buffer, const char * name, rect   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.Left, value.Bottom, value.Right, value.Top ); }
internal void serialize_MEMORY ( char * buffer, const char * name, MEMORY Memory ) { sprintf( buffer, "%s: base: %llx, size: %llu, used: %llu", name, ( uint64 )Memory.Base, Memory.Size, Memory.Used ); }

internal void serialize_hex_uint32( char * buffer, const char * name, uint32 value ) { sprintf( buffer, "%s: %X",     name, value ); }

internal void serialize_UINT32_PAIR( char * buffer, const char * name, UINT32_PAIR value ) {
    sprintf( buffer, "%s: %u, %u", name, value.x, value.y );
}

#endif // DEBUG_BUILD