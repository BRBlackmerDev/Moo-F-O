
//----------
// EDITOR
//----------

internal void
InitCameraForWaveEditor( APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Editor_Panel;
    
    rect View_Bound = AppState->App_Bound;
    if( Panel->Show ) {
        View_Bound.Right = Panel->Bound.Left;
    }
    
    ZoomCameraToWorldBound( &Editor->Camera_WaveEditor, AppState->App_Bound, View_Bound, Editor->World_Bound );
}

internal void
InitCameraForLayoutEditor( APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Layout_Panel;
    
    rect View_Bound = AppState->App_Bound;
    if( Panel->Show ) {
        View_Bound.Right = Panel->Bound.Left;
    }
    
    flo32 App_AspectRatio = AppState->App_Dim.x / AppState->App_Dim.y;
    vec2  Wave_Dim        = Vec2( App_AspectRatio * 3.0f, 3.0f );
    
    rect  World_Bound     = {};
    World_Bound.Left    =  1000000.0f;
    World_Bound.Bottom  =  1000000.0f;
    World_Bound.Right   = -1000000.0f;
    World_Bound.Top     = -1000000.0f;
    
    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
    for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
        WAVE Wave  = WaveS->Wave[ iWave ];
        rect Bound = RectCD( Wave.Layout_Position, Wave_Dim );
        
        World_Bound = RectExtrema( Bound, World_Bound );
    }
    World_Bound = AddRadius( World_Bound, 1.0f );
    
    ZoomCameraToWorldBound( &Editor->Camera_LayoutEditor, AppState->App_Bound, View_Bound, World_Bound );
}

internal void
ResetEditor( EDITOR_STATE * Editor, APP_STATE * AppState ) {
    Editor->Mode = EditorMode_Add;
    
    ResetFileSaveOpen( &Editor->SaveOpen );
    
    uint8 * AddrA = ( uint8 * )Editor;
    uint8 * AddrB = ( uint8 * )&Editor->Reset_Reset;
    uint32 Reset_Size = sizeof( EDITOR_STATE ) - ( uint32 )( AddrB - AddrA );
    memset( AddrB, 0, Reset_Size );
    
    Editor->DrawGrid = true;
}

//----------
// KEYBOARD INPUT
//----------

internal void
UpdateKeyboardInputForFileSaveOpen( INPUT_STRING * i, KEYBOARD_STATE * Keyboard, FILE_SAVE_OPEN * State ) {
    for( uint32 iKeyEvent = 0; iKeyEvent < Keyboard->nEvents; iKeyEvent++ ) {
        KEY_STATE Event = Keyboard->Event[ iKeyEvent ];
        if( Event.IsDown ) {
            if( ( Event.KeyCode >= KeyCode_a ) && ( Event.KeyCode <= KeyCode_z ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_a );
                if( Event.flags == KEYBOARD_FLAGS__SHIFT ) {
                    addChar( i, 'A' + code );
                } else if( Event.flags == 0 ) {
                    addChar( i, 'a' + code );
                }
                State->DoOverwrite = false;
            } else if( ( Event.KeyCode >= KeyCode_0 ) && ( Event.KeyCode <= KeyCode_9 ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_0 );
                addChar( i, '0' + code );
                State->DoOverwrite = false;
            } else if( ( Event.KeyCode == KeyCode_dash ) && ( Event.flags == KEYBOARD_FLAGS__SHIFT ) ) {
                addChar( i, '_' );
                State->DoOverwrite = false;
            } else if( Event.KeyCode == KeyCode_Backspace ) {
                deleteChar_Backspace( i );
                State->DoOverwrite = false;
            }
        }
    }
}

//----------
// LEVEL BITMAP
//----------

internal UINT32_PAIR
UploadBMPFromFile( PLATFORM * Platform, MEMORY * DestMemory, TEXTURE_ID TextureID, char * SaveDir, char * FileName ) {
    TEXTURE_DATA Texture = BMP_ReadFile( Platform, DestMemory, SaveDir, FileName );
    UploadTexture( &Platform->Renderer, TextureID, Texture.Width, Texture.Height, Texture.TexelData );
    
    UINT32_PAIR Result = UInt32Pair( Texture.Width, Texture.Height );
    return Result;
}

internal UINT32_QUAD
GetPixelBoundOfEntityForWaveBitmap( rect EntityBound, rect LevelBound, uint32 xPixel, uint32 yPixel ) {
    vec2 LevelDim = GetDim( LevelBound );
    
    UINT32_QUAD Result = {};
    if( DoesRectIntersectRectInc( EntityBound, LevelBound ) ) {
        flo32 tMinX = Clamp01( ( ( EntityBound.Left   - LevelBound.Left   ) / LevelDim.x ) );
        flo32 tMinY = Clamp01( ( ( EntityBound.Bottom - LevelBound.Bottom ) / LevelDim.y ) );
        flo32 tMaxX = Clamp01( ( ( EntityBound.Right  - LevelBound.Left   ) / LevelDim.x ) );
        flo32 tMaxY = Clamp01( ( ( EntityBound.Top    - LevelBound.Bottom ) / LevelDim.y ) );
        
        Result.min.x = ( uint32 )( tMinX * ( flo32 )xPixel );
        Result.min.y = ( uint32 )( tMinY * ( flo32 )yPixel );
        Result.max.x = ( uint32 )( tMaxX * ( flo32 )xPixel );
        Result.max.y = ( uint32 )( tMaxY * ( flo32 )yPixel );
    }
    
    return Result;
}

internal void
OutputRectToWaveBitmap( UINT32_QUAD Bound, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    uint32 Color = ToU32Color_FlipRBForWin32( V4Color );
    for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
        for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
            uint32 iPixel = Y * xPixel + X;
            Pixel[ iPixel ] = Color;
        }
    }
}

internal void
OutputRectOutlineToWaveBitmap( UINT32_QUAD Bound, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    uint32 Color = ToU32Color_FlipRBForWin32( V4Color );
    for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
        for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
            if( !( ( X > Bound.min.x ) && ( Y > Bound.min.y ) && ( X < ( Bound.max.x - 1 ) ) && ( Y < ( Bound.max.y - 1 ) ) ) ) {
                uint32 iPixel = Y * xPixel + X;
                Pixel[ iPixel ] = Color;
            }
        }
    }
}

internal void
OutputORectToWaveBitmap( rect World_Bound, orect Bound, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    vec2   Pixel_Dim    = Vec2( ( flo32 )xPixel, ( flo32 )yPixel );
    vec2   Scale_Ratio  = Pixel_Dim / GetDim( World_Bound );
    
    orect  R            = Bound;
    R.Center            = ( Bound.Center - GetBL( World_Bound ) ) * Scale_Ratio;
    R.HalfDim           = Bound.HalfDim * Scale_Ratio;
    
    uint32 Color        = ToU32Color_FlipRBForWin32( V4Color );
    
    flo32  Radius       = sqrtf( R.HalfDim.x * R.HalfDim.x + R.HalfDim.y * R.HalfDim.y );
    rect   Entity_Bound = RectCR( Bound.Center, Radius );
    
    UINT32_QUAD Pixel_Bound = GetPixelBoundOfEntityForWaveBitmap( Entity_Bound, World_Bound, xPixel, yPixel );
    for( uint32 Y = Pixel_Bound.min.y; Y < Pixel_Bound.max.y; Y++ ) {
        for( uint32 X = Pixel_Bound.min.x; X < Pixel_Bound.max.x; X++ ) {
            vec2  P      = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
            if( IsInORect( P, R ) ) {
                uint32 iPixel = Y * xPixel + X;
                Pixel[ iPixel ] = Color;
            }
        }
    }
}

internal void
OutputCircleToWaveBitmap( rect World_Bound, circ Circle, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    vec2  Pixel_Dim   = Vec2( ( flo32 )xPixel, ( flo32 )yPixel );
    vec2  Scale_Ratio = Pixel_Dim / GetDim( World_Bound );
    vec2  Center      = ( Circle.Center - GetBL( World_Bound ) ) * Scale_Ratio;
    
    flo32 Radius   = Circle.Radius * Scale_Ratio.x;
    flo32 RadiusSq = Radius * Radius;
    
    uint32 Color = ToU32Color_FlipRBForWin32( V4Color );
    
    rect        Entity_Bound = RectCR( Circle.Center, Circle.Radius );
    UINT32_QUAD Bound        = GetPixelBoundOfEntityForWaveBitmap( Entity_Bound, World_Bound, xPixel, yPixel );
    for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
        for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
            vec2  P      = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
            flo32 DistSq = GetLengthSq( P - Center );
            if( DistSq <= RadiusSq ) {
                uint32 iPixel = Y * xPixel + X;
                Pixel[ iPixel ] = Color;
            }
        }
    }
}

internal void
OutputShieldToWaveBitmap( rect World_Bound, ENTITY_SHIELD Shield, vec2 Shield_Pos, flo32 Shield_bRadius, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    if( Shield.HasShield ) {
        vec2  Pixel_Dim   = Vec2( ( flo32 )xPixel, ( flo32 )yPixel );
        vec2  Scale_Ratio = Pixel_Dim / GetDim( World_Bound );
        vec2  Center      = ( Shield_Pos - GetBL( World_Bound ) ) * Scale_Ratio;
        
        flo32 Radius_Inner   = ( Shield_bRadius + Shield.Dist                ) * Scale_Ratio.x;
        flo32 Radius_Outer   = ( Shield_bRadius + Shield.Dist + Shield.Width ) * Scale_Ratio.x;
        flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
        flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
        
        flo32 MinRadians = Shield.bRadians;
        flo32 MaxRadians = Shield.bRadians + Shield.Radians;
        
        vec4 ColorA = Vec4( 0.0f, 0.35f, 0.35f, 1.0f );
        vec4 ColorB = Vec4( 0.0f, 0.35f, 0.35f, 1.0f );
        
        if( Shield.RotateSpeed < 0.0f ) {
            ColorA = Vec4( 0.0f, 0.1f, 0.1f, 1.0f );
            ColorB = Vec4( 0.5f, 1.0f, 1.0f, 1.0f );
        }
        if( Shield.RotateSpeed > 0.0f ) {
            ColorA = Vec4( 0.5f, 1.0f, 1.0f, 1.0f );
            ColorB = Vec4( 0.0f, 0.1f, 0.1f, 1.0f );
        }
        if( fabsf( Shield.RotateSpeed ) == PLAYER_ROTATE_SPEED_FAST ) {
            ColorA.r = 0.0f;
            ColorA.b = 0.0f;
            ColorB.r = 0.0f;
            ColorB.b = 0.0f;
        }
        if( Shield.RotateSpeed == TURRET_SHIELD_TEMP_SPEED_TO_INDICATE_TARGET_SHIELD ) {
            ColorA = Vec4( 0.5f, 0.0f, 0.0f, 1.0f );
            ColorB = Vec4( 0.5f, 0.0f, 0.0f, 1.0f );
        }
        
        rect        Entity_Bound = RectCR( Shield_Pos, Shield_bRadius + Shield.Dist + Shield.Width );
        UINT32_QUAD Bound        = GetPixelBoundOfEntityForWaveBitmap( Entity_Bound, World_Bound, xPixel, yPixel );
        for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
            for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
                vec2  P      = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
                flo32 DistSq = GetLengthSq( P - Center );
                if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
                    flo32 Radians = ToRadians( P - Center );
                    if( Radians < 0.0f ) {
                        Radians += TAU;
                    }
                    if( Radians < MinRadians ) {
                        Radians += TAU;
                    }
                    
                    if( ( Radians >= MinRadians ) && ( Radians <= MaxRadians ) ) {
                        flo32 tRadians = Clamp01( ( Radians - Shield.bRadians ) / ( Shield.Radians ) );
                        vec4   V4Color = lerp( ColorA, tRadians, ColorB );
                        uint32   Color = ToU32Color_FlipRBForWin32( V4Color );
                        
                        uint32 iPixel = Y * xPixel + X;
                        Pixel[ iPixel ] = Color;
                    }
                }
            }
        }
    }
}

internal void
OutputTriangleToWaveBitmap( UINT32_QUAD Bound, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    vec2 A = Vec2( ( flo32 )Bound.min.x, ( flo32 )Bound.min.y );
    vec2 B = Vec2( ( flo32 )Bound.max.x, ( flo32 )Bound.min.y );
    vec2 C = Vec2( ( A.x + B.x ) * 0.5f, ( flo32 )Bound.max.y );
    
    vec2 PerpAC = GetPerp( C - A );
    vec2 PerpBC = GetPerp( C - B );
    
    uint32 Color = ToU32Color_FlipRBForWin32( V4Color );
    for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
        for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
            vec2  P = Vec2( ( flo32 )X, ( flo32 )Y );
            vec2 AP = P - A;
            vec2 BP = P - B;
            
            flo32 DotAC = dot( AP, PerpAC );
            flo32 DotBC = dot( BP, PerpBC );
            if( ( DotAC <= 0.0f ) && ( DotBC >= 0.0f ) ) {
                uint32 iPixel = Y * xPixel + X;
                Pixel[ iPixel ] = Color;
            }
        }
    }
}

//----------
//----------

internal WAVE *
GetWaveByGuid( EDITOR__WAVE_STATE * WaveS, int32 Guid ) {
    WAVE * Result = 0;
    
    for( uint32 iWave = 0; ( Result == 0 ) && ( iWave < WaveS->nWave ); iWave++ ) {
        WAVE * Wave = WaveS->Wave + iWave;
        if( Wave->Guid == Guid ) {
            Result = Wave;
        }
    }
    return Result;
}

internal flo32
Target_ConvertHealthToRadius( int32 Health ) {
    // TODO: NOTE: This function only exists because ENTITY struct has a radius field and doesn't have a health field and I don't want to change the format at this time. The format will be changed later when we add shields, arbitrary bullet patterns, etc.
    
    // NOTE: This function expects health values: 10, 20, 30, 40, 50
    // NOTE: This function returns radius values: 1.0f, 2.0f, 3.0f, 4.0f, 5.0f
    flo32 t = ( flo32 )( Health / 10 ) / 5.0f;
    flo32 Radius = t * 5.0f;
    return Radius;
}