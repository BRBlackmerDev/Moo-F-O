
internal void
InitEditorPanel( APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Editor_Panel;
    
    { // set Bound
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        Panel->Bound = R;
    }
    
    ENTITY * LastEntity = &Editor->Entity_LastEntity;
    LastEntity->nEntity            = 1;
    LastEntity->Radius             = 3.0f;
    Editor->EntityTemplate_nEntity = 1;
    Editor->EntityTemplate_Spacing = 1.0f;
    
    ENTITY_SHIELD * Shield = &LastEntity->Shield;
    Shield->HasShield   = false;
    Shield->bRadians    = 0.0f;
    Shield->Radians     = PI;
    Shield->Dist        = 1.0f;
    Shield->Width       = 1.0f;
    Shield->RotateSpeed = -PLAYER_ROTATE_SPEED_SLOW;
}

internal void
InitLayoutPanel( APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Layout_Panel;
    
    { // set Bound
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        Panel->Bound = R;
    }
    
    Panel->Show = true;
}

#if 0
internal void
DrawGrid( RENDER_PASS * Pass, CAMERA_STATE * Camera ) {
    rect Bound_Camera = RectCD( Camera->Pos, Camera->Dim );
    rect Bound_Grid   = RectBLD( Vec2( 0.0f, 0.0f ), Vec2( ( flo32 )EDITOR__GRID_MAX_WIDTH, ( flo32 )EDITOR__GRID_MAX_HEIGHT ) * TILE_DIM );
    
    Bound_Grid.Left   = MaxValue( Bound_Grid.Left,   Bound_Camera.Left   );
    Bound_Grid.Bottom = MaxValue( Bound_Grid.Bottom, Bound_Camera.Bottom );
    Bound_Grid.Right  = MinValue( Bound_Grid.Right,  Bound_Camera.Right  );
    Bound_Grid.Top    = MinValue( Bound_Grid.Top,    Bound_Camera.Top    );
    
    UINT32_PAIR CellMin = GetCellIndexFromPos( Bound_Grid.min );
    UINT32_PAIR CellMax = GetCellIndexFromPos( Bound_Grid.max );
    
    uint32 Scale = 4;
    CellMin.x -= ( CellMin.x % Scale );
    CellMin.y -= ( CellMin.y % Scale );
    CellMax.x -= ( CellMax.x % Scale );
    CellMax.y -= ( CellMax.y % Scale );
    
    uint32 nX = ( ( CellMax.x - CellMin.x ) / Scale ) + 1;
    uint32 nY = ( ( CellMax.y - CellMin.y ) / Scale ) + 1;
    
    vec2 Offset = Vec2( ( flo32 )Scale, ( flo32 )Scale ) * TILE_DIM;
    
    vec2 bPos = GetWorldPos( CellMin );
    
    vec2 AtPos = bPos;
    for( uint32 iX = 0; iX <= nX; iX++ ) {
        vec2 A = AtPos;
        vec2 B = AtPos + Vec2( 0.0f, ( flo32 )nY * Offset.y );
        
        DrawLine( Pass, A, B, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
        
        AtPos.x += Offset.x;
    }
    
    AtPos = bPos;
    for( uint32 iY = 0; iY <= nY; iY++ ) {
        vec2 A = AtPos;
        vec2 B = AtPos + Vec2( ( flo32 )nX * Offset.x, 0.0f );
        
        DrawLine( Pass, A, B, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
        
        AtPos.y += Offset.y;
    }
}
#endif

internal boo32
AddPressButton( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID ButtonID, RENDER_PASS * Pass, FONT * Font, char * Text, rect Bound ) {
    boo32 Result = false;
    vec4  Color  = COLOR_GRAY( 0.8f );
    
    if( IsInRect( Mouse->Position, Bound ) ) {
        Color = COLOR_GRAY( 0.5f );
        if( WasPressed( Mouse, ButtonID ) ) {
            Result = true;
            Color  = Vec4( 1.0f, 0.2f, 0.2f, 1.0f );
        }
    }
    
    DrawRect( Pass, Bound, Color );
    DrawRectOutline( Pass, Bound, COLOR_BLACK );
    DrawString( Pass, Font, Text, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    return Result;
}

internal boo32
AddToggleButton( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID ButtonID, RENDER_PASS * Pass, FONT * Font, char * Text, rect Bound, boo32 IsDown ) {
    boo32 Result = false;
    vec4  Color  = COLOR_GRAY( 0.8f );
    
    if( IsDown ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    
    if( IsInRect( Mouse->Position, Bound ) ) {
        if( !IsDown ) {
            Color = COLOR_GRAY( 0.5f );
        }
        if( WasPressed( Mouse, ButtonID ) ) {
            Result = true;
            Color  = Vec4( 1.0f, 0.2f, 0.2f, 1.0f );
        }
    }
    
    DrawRect( Pass, Bound, Color );
    DrawRectOutline( Pass, Bound, COLOR_BLACK );
    DrawString( Pass, Font, Text, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    return Result;
}

internal boo32
AddScrollButton( MOUSE_STATE * Mouse, RENDER_PASS * Pass, FONT * Font, char * Text, rect Bound ) {
    boo32 Result = false;
    vec4  Color  = COLOR_GRAY( 0.8f );
    
    if( IsInRect( Mouse->Position, Bound ) ) {
        Color = COLOR_GRAY( 0.5f );
        if( Mouse->WheelClicks != 0 ) {
            Result = true;
            Color  = Vec4( 1.0f, 0.2f, 0.2f, 1.0f );
        }
    }
    
    DrawRect( Pass, Bound, Color );
    DrawRectOutline( Pass, Bound, COLOR_BLACK );
    DrawString( Pass, Font, Text, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    return Result;
}

internal void
UpdateAndDrawEditorPanel( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, boo32 ShowLayout ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Editor_Panel;
    FONT         * Font   = AppState->Font;
    
    if( Panel->Show ) {
        vec2  Button_Dim = Vec2( 40.0f, 16.0f );
        flo32 LabelY     = 14.0f;
        flo32 LabelPreY  = 8.0f;
        
        Panel->Inner = AddRadius( Panel->Bound, -4.0f );
        Panel->AtPos = GetTL( Panel->Inner );
        Panel->AdvanceY = 0.0f;
        
        flo32 Label_AdvanceY = 16.0f;
        
        DrawRect( Pass_UI, Panel->Bound, COLOR_WHITE );
        
        EDITOR__WAVE_STATE * WaveS       = &Editor->WaveS;
        WAVE               * CurrentWave = WaveS->Wave + WaveS->CurrentWave;
        
        WrapPanel( Panel );
        
        { // iWave and WaveGuid
            char Str[ 128 ] = {};
            sprintf( Str, "iWave : %u, Guid : %d", WaveS->CurrentWave, CurrentWave->Guid );
            AddLabel( Panel, Pass_UI, Font, Str, Label_AdvanceY );
            
            sprintf( Str, "nNextWave : %u, nKillWave : %u", CurrentWave->nNextWave, CurrentWave->nKillWave );
            AddLabel( Panel, Pass_UI, Font, Str, Label_AdvanceY );
        }
        
        if( !ShowLayout ) {
            { // WORLD Y UNITS
                WrapPanel( Panel, 12.0f );
                
                char Str[ 128 ] = {};
                sprintf( Str, "World Y Units: %u", ( uint32 )Editor->World_YUnits );
                
                rect Bound = GetBound( Panel, Vec2( GetWidth( Panel->Inner ), 24.0f ) );
                if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                    Editor->World_YUnits += ( flo32 )Mouse->WheelClicks;
                    Editor->World_YUnits  = Clamp( Editor->World_YUnits, 16.0f, 96.0f );
                    
                    Editor->World_Bound   = GetWorldBound( AppState->App_Dim, Editor->World_YUnits );
                }
            }
            
            { // WAVE TYPE
                WAVE_TYPE_LABELS;
                
                WrapPanel( Panel, 12.0f );
                AddLabel( Panel, Pass_UI, Font, " Wave Settings", Label_AdvanceY );
                
                char Str[ 128 ] = {};
                sprintf( Str, "Wave Type: %s", WaveTypeLabels[ CurrentWave->Type ] );
                
                rect Bound = GetBound( Panel, Vec2( GetWidth( Panel->Inner ), 24.0f ) );
                if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                    CurrentWave->Type = ( WAVE_TYPE )UInt32Wrap( CurrentWave->Type, Mouse->WheelClicks, WaveType_Count );
                    
                    if( CurrentWave->Type == WaveType_EndLevel ) {
                        if( Mouse->WheelClicks > 0 ) {
                            CurrentWave->Type = ( WAVE_TYPE )UInt32Wrap( CurrentWave->Type,  1, WaveType_Count );
                        } else {
                            CurrentWave->Type = ( WAVE_TYPE )UInt32Wrap( CurrentWave->Type, -1, WaveType_Count );
                        }
                    }
                    
                    { // Clear Wave Data for New Type
                        int32     Guid = CurrentWave->Guid;
                        WAVE_TYPE Type = CurrentWave->Type;
                        
                        memset( CurrentWave, 0, sizeof( WAVE ) );
                        
                        CurrentWave->Guid = Guid;
                        CurrentWave->Type = Type;
                    }
                }
            }
            
            switch( CurrentWave->Type ) {
                case WaveType_EndLevel: {
                    
                } break;
                
                case WaveType_SpawnEntities: {
                    WrapPanel( Panel );
                    WrapPanel( Panel, 8.0f );
                    ENTITY_TYPE_LABELS;
                    
                    { // Add Entity
                        rect Bound_Add    = GetBound( Panel, Button_Dim );
                        rect Bound_Move   = GetBound( Panel, Button_Dim );
                        rect Bound_Copy   = GetBound( Panel, Button_Dim );
                        rect Bound_Shield = GetBound( Panel, Vec2( 60.0f, 16.0f ) );
                        if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "ADD", Bound_Add, ( Editor->Mode == EditorMode_Add ) ) ) {
                            Editor->Mode = EditorMode_Add;
                        }
                        if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "MOVE", Bound_Move, ( Editor->Mode == EditorMode_Move ) ) ) {
                            if( Editor->Mode == EditorMode_Add ) {
                                memset( WaveS->Entity_IsSelected, 0, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
                            }
                            Editor->Mode = EditorMode_Move;
                        }
                        if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "COPY", Bound_Copy, ( Editor->Mode == EditorMode_Copy ) ) ) {
                            if( Editor->Mode == EditorMode_Add ) {
                                memset( WaveS->Entity_IsSelected, 0, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
                            }
                            Editor->Mode = EditorMode_Copy;
                        }
                        if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "SHIELD", Bound_Shield, ( Editor->Mode == EditorMode_ModifyShield ) ) ) {
                            Editor->Mode = EditorMode_ModifyShield;
                        }
                    }
                    
                    if( Editor->Mode == EditorMode_Add ) {
                        WrapPanel( Panel );
                        WrapPanel( Panel, 4.0f );
                        
                        ENTITY * LastEntity = &Editor->Entity_LastEntity;
                        { // Entity Type
                            char Str[ 64 ] = {};
                            sprintf( Str, "Type: %s", EntityTypeLabels[ LastEntity->Type ] );
                            rect Bound = GetBound( Panel, Vec2( 200.0f, 28.0f ) );
                            if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                LastEntity->Type = ( ENTITY_TYPE )UInt32Wrap( LastEntity->Type, Mouse->WheelClicks, EntityType_Count );
                                
                                while( ( LastEntity->Type != EntityType_Turret    ) && (  LastEntity->Type != EntityType_Chaser    ) &&
                                      (  LastEntity->Type != EntityType_Swarm     ) &&
                                      (  LastEntity->Type != EntityType_Drifter   ) &&
                                      (  LastEntity->Type != EntityType_Looper    ) &&
                                      (  LastEntity->Type != EntityType_Starburst ) &&
                                      (  LastEntity->Type != EntityType_Miner     ) &&
                                      (  LastEntity->Type != EntityType_Bomber    ) &&
                                      (  LastEntity->Type != EntityType_Bumper    ) &&
                                      (  LastEntity->Type != EntityType_Bank      ) ) {
                                    if( Mouse->WheelClicks > 0 ) {
                                        LastEntity->Type = ( ENTITY_TYPE )UInt32Wrap( LastEntity->Type,  1, EntityType_Count );
                                    } else {
                                        LastEntity->Type = ( ENTITY_TYPE )UInt32Wrap( LastEntity->Type, -1, EntityType_Count );
                                    }
                                }
                            }
                            
                            if( LastEntity->Type != EntityType_Turret ) {
                                Editor->EntityMouseWheel_Mode = EntityMouseWheelMode_nEntity;
                            }
                            
#if 0                    
                            if( ( IsInRect( Mouse->Position, Bound ) ) && ( WasPressed( Mouse, MouseButton_Right ) ) ) {
                                Editor->EntityMouseWheel_Mode = EntityMouseWheelMode_Type;
                            }
                            if( Editor->EntityMouseWheel_Mode == EntityMouseWheelMode_Type ) {
                                DrawRectOutline( Pass_UI, AddRadius( Bound, 1.0f ), COLOR_RED );
                            }
#endif
                        }
                        { // Entity Count
                            char Str[ 64 ] = {};
                            sprintf( Str, "nEntity: %u", LastEntity->nEntity );
                            rect Bound = GetBound( Panel, Vec2( 100.0f, 28.0f ) );
                            if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                LastEntity->nEntity += Mouse->WheelClicks;
                                LastEntity->nEntity  = MaxValue( LastEntity->nEntity, 1 );
                            }
                            
                            if( ( IsInRect( Mouse->Position, Bound ) ) && ( WasPressed( Mouse, MouseButton_Right ) ) ) {
                                Editor->EntityMouseWheel_Mode = EntityMouseWheelMode_nEntity;
                            }
                            if( Editor->EntityMouseWheel_Mode == EntityMouseWheelMode_nEntity ) {
                                DrawRectOutline( Pass_UI, AddRadius( Bound, 1.0f ), COLOR_RED );
                            }
                        }
                        if( LastEntity->Type == EntityType_Turret ) {
                            char Str[ 64 ] = {};
                            sprintf( Str, "Health: %u", ( int32 )LastEntity->Radius * 10 );
                            rect Bound = GetBound( Panel, Vec2( 92.0f, 28.0f ) );
                            if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                LastEntity->Radius += ( flo32 )Mouse->WheelClicks;
                                LastEntity->Radius  = Clamp( LastEntity->Radius, 1.0f, 5.0f );
                            }
                            
                            if( ( IsInRect( Mouse->Position, Bound ) ) && ( WasPressed( Mouse, MouseButton_Right ) ) ) {
                                Editor->EntityMouseWheel_Mode = EntityMouseWheelMode_TargetRadiusOrHealth;
                            }
                            if( Editor->EntityMouseWheel_Mode == EntityMouseWheelMode_TargetRadiusOrHealth ) {
                                DrawRectOutline( Pass_UI, AddRadius( Bound, 1.0f ), COLOR_RED );
                            }
                        }
                        
                        WrapPanel( Panel );
                        { // ENTITY TEMPLATES
                            flo32 Button_Height = 20.0f;
                            {
                                WrapPanel( Panel );
                                rect Bound = GetBound( Panel, Vec2( 120.0f, Button_Height ) );
                                if( AddPressButton( Mouse, MouseButton_Left, Pass_UI, Font, "Clear Entities", Bound ) ) {
                                    CurrentWave->nEntity = 0;
                                }
                                WrapPanel( Panel );
                            }
                            { // Template Type
                                rect Bound_Line   = GetBound( Panel, Vec2( 44.0f, Button_Height ) );
                                rect Bound_Circle = GetBound( Panel, Vec2( 60.0f, Button_Height ) );
                                if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "Line", Bound_Line, ( Editor->EntityTemplate_Mode == EntityTemplateMode_Line ) ) ) {
                                    Editor->EntityTemplate_Mode = EntityTemplateMode_Line;
                                }
                                if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "Circle", Bound_Circle, ( Editor->EntityTemplate_Mode == EntityTemplateMode_Circle ) ) ) {
                                    Editor->EntityTemplate_Mode = EntityTemplateMode_Circle;
                                }
                            }
                            { // Entity Count
                                char Str[ 32 ] = {};
                                sprintf( Str, "nEntity: %u", ( uint32 )Editor->EntityTemplate_nEntity );
                                
                                rect Bound = GetBound( Panel, Vec2( 92.0f, Button_Height ) );
                                if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                    Editor->EntityTemplate_nEntity += Mouse->WheelClicks;
                                    Editor->EntityTemplate_nEntity  = MaxValue( Editor->EntityTemplate_nEntity, 1 );
                                }
                            }
                            { // Entity Spacing
                                char Str[ 32 ] = {};
                                if( Editor->EntityTemplate_Mode == EntityTemplateMode_Line ) {
                                    sprintf( Str, "Spacing: %0.1f", Editor->EntityTemplate_Spacing );
                                } else if( Editor->EntityTemplate_Mode == EntityTemplateMode_Circle ) {
                                    sprintf( Str, "Radius: %0.1f", Editor->EntityTemplate_Spacing );
                                }
                                
                                rect Bound = GetBound( Panel, Vec2( 108.0f, Button_Height ) );
                                if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                    Editor->EntityTemplate_Spacing += ( ( flo32 )Mouse->WheelClicks * 0.5f );
                                    Editor->EntityTemplate_Spacing  = MaxValue( Editor->EntityTemplate_Spacing, 1.0f );
                                }
                            }
                            { // Entity Radians
                                int32 Degrees = ( uint32 )( Editor->EntityTemplate_Radians / TAU * 360.0f );
                                
                                char Str[ 32 ] = {};
                                sprintf( Str, "Degrees: %u", ( uint32 )Degrees );
                                
                                rect   Bound  = GetBound( Panel, Vec2( 108.0f, Button_Height ) );
                                uint32 Offset = 45;
                                
                                if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                    Degrees += ( Mouse->WheelClicks * Offset );
                                    
                                    while( Degrees < 0.0f ) {
                                        Degrees += 180;
                                    }
                                    while( Degrees >= 180 ) {
                                        Degrees -= 180;
                                    }
                                    
                                    Editor->EntityTemplate_Radians = ( flo32 )Degrees / 360.0f * TAU;
                                }
                            }
                        }
                    }
                    
                    if( Editor->Mode == EditorMode_ModifyShield ) {
                        ENTITY * Entity = 0;
                        if( ( Editor->Entity_iEntity > -1 ) && ( Editor->Entity_iEntity < ( int32 )CurrentWave->nEntity ) ) {
                            Entity = CurrentWave->Entity + Editor->Entity_iEntity;
                        }
                        
                        WrapPanel( Panel );
                        if( ( Entity ) && ( Entity->Type == EntityType_Turret ) ) {
                            ENTITY_SHIELD * Shield = &Entity->Shield;
                            
                            flo32 Button_Height = 20.0f;
                            
                            { // Has Shield
                                rect Bound = GetBound( Panel, Vec2( 108.0f, Button_Height ) );
                                if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "Has Shield", Bound, Shield->HasShield ) ) {
                                    Shield->HasShield = !Shield->HasShield;
                                    if( Shield->HasShield ) {
                                        ENTITY * LastEntity = &Editor->Entity_LastEntity;
                                        LastEntity->Shield.HasShield = true;
                                        
                                        *Shield = LastEntity->Shield;
                                    }
                                }
                            }
                            
                            if( Shield->HasShield ) {
                                { // bRadians
                                    int32  Degrees = ( int32 )( Shield->bRadians / TAU * 360.0f );
                                    uint32 Offset  = 45;
                                    
                                    char Str[ 32 ] = {};
                                    sprintf( Str, "bDegrees: %u", Degrees );
                                    
                                    rect Bound = GetBound( Panel, Vec2( 120.0f, Button_Height ) );
                                    if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                        Degrees += ( Mouse->WheelClicks * Offset );
                                        
                                        while( Degrees < 0.0f ) {
                                            Degrees += 360;
                                        }
                                        while( Degrees >= 360 ) {
                                            Degrees -= 360;
                                        }
                                        
                                        Shield->bRadians = ( flo32 )Degrees / 360.0f * TAU;
                                        Editor->Entity_LastEntity.Shield = *Shield;
                                    }
                                }
                                
                                { // Dist
                                    flo32 Dist = Shield->Dist;
                                    
                                    char Str[ 32 ] = {};
                                    sprintf( Str, "Dist: %.01f", Dist );
                                    
                                    rect Bound = GetBound( Panel, Vec2( 120.0f, Button_Height ) );
                                    if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                        Dist += ( flo32 )Mouse->WheelClicks;
                                        Dist  = MaxValue( Dist, 1.0f );
                                        
                                        Shield->Dist = Dist;
                                        Editor->Entity_LastEntity.Shield = *Shield;
                                    }
                                }
                                
                                { // Radians
                                    int32  Degrees = ( int32 )( Shield->Radians / TAU * 360.0f );
                                    uint32 Offset  = 90;
                                    
                                    char Str[ 32 ] = {};
                                    sprintf( Str, "Degrees: %u", Degrees );
                                    
                                    rect Bound = GetBound( Panel, Vec2( 120.0f, Button_Height ) );
                                    if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                        Degrees += ( Mouse->WheelClicks * Offset );
                                        Degrees  = Clamp( Degrees, 180, 360 );
                                        
                                        Shield->Radians = ( flo32 )Degrees / 360.0f * TAU;
                                        Editor->Entity_LastEntity.Shield = *Shield;
                                    }
                                }
                                
                                { // Width
                                    flo32 Width = Shield->Width;
                                    
                                    char Str[ 32 ] = {};
                                    sprintf( Str, "Width: %.01f", Width );
                                    
                                    rect Bound = GetBound( Panel, Vec2( 120.0f, Button_Height ) );
                                    if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                        Width += ( flo32 )Mouse->WheelClicks;
                                        Width  = MaxValue( Width, 1.0f );
                                        
                                        Shield->Width = Width;
                                        Editor->Entity_LastEntity.Shield = *Shield;
                                    }
                                }
                                
                                { // RotateSpeed
                                    flo32 Speed[] = {
                                        TURRET_SHIELD_TEMP_SPEED_TO_INDICATE_TARGET_SHIELD,
                                        -PLAYER_ROTATE_SPEED_FAST,
                                        -PLAYER_ROTATE_SPEED_SLOW,
                                        0.0f,
                                        PLAYER_ROTATE_SPEED_SLOW,
                                        PLAYER_ROTATE_SPEED_FAST,
                                    };
                                    uint32 nSpeed = _ArrayCount( Speed );
                                    
                                    char * SpeedStr[] = {
                                        "TARGET", "-FAST", "-SLOW", "STOP", "SLOW", "FAST",
                                    };
                                    
                                    int32 Index = -1;
                                    for( uint32 iSpeed = 0; iSpeed < nSpeed; iSpeed++ ) {
                                        if( Shield->RotateSpeed == Speed[ iSpeed ] ) {
                                            Index = ( int32 )iSpeed;
                                        }
                                    }
                                    Assert( Index > -1 );
                                    
                                    rect Bound = GetBound( Panel, Vec2( 120.0f, Button_Height ) );
                                    if( AddScrollButton( Mouse, Pass_UI, Font, SpeedStr[ Index ], Bound ) ) {
                                        Index += Mouse->WheelClicks;
                                        Index  = Clamp( Index, 0, ( int32 )nSpeed - 1 );
                                        
                                        Shield->RotateSpeed = Speed[ Index ];
                                        Editor->Entity_LastEntity.Shield = *Shield;
                                    }
                                }
                            }
                        } else {
                            AddLabel( Panel, Pass_UI, Font, "No Target is Selected", Label_AdvanceY );
                        }
                    }
                } break;
                
                case WaveType_OpenBayDoors: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    rect Bound_Type  = GetBound( Panel, Dim );
                    rect Bound_iSrc  = GetBound( Panel, Dim );
                    rect Bound_iDest = GetBound( Panel, Dim );
                    
                    BAY_DOOR_EVENT * Event = &CurrentWave->BayDoorEvent;
                    
                    BAY_DOOR_EVENT_TYPE_LABELS;
                    
                    char * Label_Type = BayDoorEventTypeLabels[ Event->Type ];
                    
                    char Label_iSrc [ 8 ] = {};
                    char Label_iDest[ 8 ] = {};
                    
                    sprintf( Label_iSrc,  "%u", ( uint32 )Event->iSrc  );
                    sprintf( Label_iDest, "%u", ( uint32 )Event->iDest );
                    
                    if( AddScrollButton( Mouse, Pass_UI, Font, Label_Type, Bound_Type ) ) {
                        Event->Type = ( BAY_DOOR_EVENT_TYPE )( ( ( uint32 )Event->Type + 1 ) % BayDoorEventType_Count );
                    }
                    if( AddScrollButton( Mouse, Pass_UI, Font, Label_iSrc, Bound_iSrc ) ) {
                        Event->iSrc += Mouse->WheelClicks;
                        Event->iSrc  = Clamp( Event->iSrc, 0, BAY_DOOR_SPAWN_MAX_COUNT - 1 );
                    }
                    if( AddScrollButton( Mouse, Pass_UI, Font, Label_iDest, Bound_iDest ) ) {
                        Event->iDest += Mouse->WheelClicks;
                        Event->iDest  = Clamp( Event->iDest, 0, BAY_DOOR_SPAWN_DEST_MAX_COUNT - 1 );
                    }
                } break;
                
                case WaveType_SetAsteroidSettings: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    ASTEROID_SETTINGS_EVENT * Event = &CurrentWave->AsteroidSettingsEvent;
                    
                    { // Spawn Target Time
                        flo32 TargetTime = ( flo32 )Event->Spawn_nFrames / 60.0f; 
                        char Label_TargetTime[ 64 ] = {};
                        sprintf( Label_TargetTime, "Spawn Target Time: %.04f sec", TargetTime );
                        AddLabel( Panel, Pass_UI, Font, Label_TargetTime, Label_AdvanceY );
                        WrapPanel( Panel );
                        
                        char Label_nFrames[ 32 ] = {};
                        sprintf( Label_nFrames, " nFrames: %u/60 fps", ( uint32 )Event->Spawn_nFrames );
                        AddLabel( Panel, Pass_UI, Font, Label_nFrames, Label_AdvanceY );
                        
                        rect Bound_20 = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_5  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_1  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "20", Bound_20 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 20 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "5", Bound_5 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 5 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "1", Bound_1 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 1 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                    }
                    
                    { // Max Asteroids
                        WrapPanel( Panel, 8.0f );
                        
                        char Label_MaxAsteroid[ 32 ] = {};
                        if( Event->Spawn_MaxAsteroid < 0 ) {
                            strcpy( Label_MaxAsteroid, "Max Asteroids: INF" );
                        } else if( Event->Spawn_MaxAsteroid == 0 ) {
                            strcpy( Label_MaxAsteroid, "Max Asteroids: ---" );
                        } else {
                            sprintf( Label_MaxAsteroid, "Max Asteroids: %u", ( uint32 )Event->Spawn_MaxAsteroid );
                        }
                        
                        rect Bound = GetBound( Panel, Vec2( 200.0f, 20.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label_MaxAsteroid, Bound ) ) {
                            Event->Spawn_MaxAsteroid += Mouse->WheelClicks;
                            Event->Spawn_MaxAsteroid  = MaxValue( Event->Spawn_MaxAsteroid, -1 );
                        }
                    }
                    
                    { // Asteroid Type and Class
                        WrapPanel( Panel );
                        
                        AddLabel( Panel, Pass_UI, Font, "          TI   SM    MD   LG", Label_AdvanceY );
                        WrapPanel( Panel );
                        
                        char * Label[ AsteroidType_Count ] = {
                            "Default", "Bomb   ", "Laser  ", "Swarm  ",
                        };
                        
                        int32 Denom = 0;
                        for( uint32 iType = 0; iType < AsteroidType_Count; iType++ ) {
                            for( uint32 iClass = 0; iClass < AsteroidClass_Count; iClass++ ) {
                                Denom += Event->Numer_TypeClass[ iType ][ iClass ];
                            }
                        }
                        
                        for( uint32 iType = 0; iType < AsteroidType_Count; iType++ ) {
                            AddLabel( Panel, Pass_UI, Font, Label[ iType ], Label_AdvanceY );
                            
                            int32 Numer_Type = 0;
                            
                            for( uint32 iClass = 0; iClass < AsteroidClass_Count; iClass++ ) {
                                int32 * Numer_Class = Event->Numer_TypeClass[ iType ];
                                
                                Numer_Type += Numer_Class[ iClass ];
                                
                                char Label_Numer[ 8 ] = {};
                                if( Numer_Class[ iClass ] > 0 ) {
                                    sprintf( Label_Numer, "%u", ( uint32 )Numer_Class[ iClass ] );
                                } else {
                                    sprintf( Label_Numer, "-" );
                                }
                                
                                rect Bound = GetBound( Panel, Dim );
                                if( AddScrollButton( Mouse, Pass_UI, Font, Label_Numer, Bound ) ) {
                                    Numer_Class[ iClass ] += Mouse->WheelClicks;
                                    Numer_Class[ iClass ]  = MaxValue( Numer_Class[ iClass ], 0 );
                                    
                                    Numer_Type += Mouse->WheelClicks;
                                    Numer_Type  = MaxValue( Numer_Type, 0 );
                                    
                                    Denom += Mouse->WheelClicks;
                                    Denom  = MaxValue( Denom, 0 );
                                }
                            }
                            
                            if( Denom > 0 ) {
                                flo32 Percent = ( flo32 )Numer_Type / ( flo32 )Denom;
                                char Label_Percent[ 12 ] = {};
                                sprintf( Label_Percent, "%.1f%%", Percent * 100.0f );
                                AddLabel( Panel, Pass_UI, Font, Label_Percent, Label_AdvanceY );
                            }
                            
                            WrapPanel( Panel );
                        }
                    }
                    
                    { // Bound
                        WrapPanel( Panel );
                        
                        rect Bound = GetBound( Panel, Vec2( GetWidth( Panel->Inner ), Dim.y ) );
                        if( AddPressButton( Mouse, MouseButton_Left, Pass_UI, Font, "Init Spawn Bound to World Bound", Bound ) ) {
                            Event->Spawn_Bound = Editor->World_Bound;
                            Event->Spawn_SideIsEnabled[ 0 ] = true;
                            Event->Spawn_SideIsEnabled[ 1 ] = true;
                            Event->Spawn_SideIsEnabled[ 2 ] = true;
                            Event->Spawn_SideIsEnabled[ 3 ] = true;
                        }
                        
                        WrapPanel( Panel );
                        
                        vec2 DimA = Vec2( 40.0f, Dim.y );
                        AddLabel( Panel, Pass_UI, Font, "Enable Side: ", DimA.y );
                        
                        int32 nButton = 4;
                        char * Label[ 4 ] = { "LFT", "BTM", "RGT", "TOP" };
                        
                        for( int32 iButton = 0; iButton < nButton; iButton++ ) {
                            if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, Label[ iButton ], GetBound( Panel, DimA ), Event->Spawn_SideIsEnabled[ iButton ] ) ) {
                                Event->Spawn_SideIsEnabled[ iButton ] = !Event->Spawn_SideIsEnabled[ iButton ];
                            }
                        }
                        
                        WrapPanel( Panel );
                        
                        AddLabel( Panel, Pass_UI, Font, "Bound      : ", DimA.y );
                        for( int32 iButton = 0; iButton < nButton; iButton++ ) {
                            flo32 * Elem = Event->Spawn_Bound.elem + iButton;
                            
                            char Str[ 16 ] = {};
                            sprintf( Str, "%d", ( int32 )( *Elem ) );
                            if( AddScrollButton( Mouse, Pass_UI, Font, Str, GetBound( Panel, DimA ) ) ) {
                                switch( iButton ) {
                                    case 0:
                                    case 1: {
                                        ( *Elem ) -= ( flo32 )Mouse->WheelClicks;
                                    } break;
                                    
                                    case 2:
                                    case 3: {
                                        ( *Elem ) += ( flo32 )Mouse->WheelClicks;
                                    } break;
                                }
                            }
                        }
                        
                        DrawRectOutline( Pass_Game, AddRadius( Event->Spawn_Bound, 0.1f ), COLOR_MAGENTA );
                    }
                } break;
                
                case WaveType_SetExplosiveSettings: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    EXPLOSIVE_SETTINGS_EVENT * Event = &CurrentWave->ExplosiveSettingsEvent;
                    
                    { // Spawn Target Time
                        flo32 TargetTime = ( flo32 )Event->Spawn_nFrames / 60.0f; 
                        char Label_TargetTime[ 64 ] = {};
                        if( Event->Spawn_nFrames > 0 ) {
                            sprintf( Label_TargetTime, "Spawn Target Time: %.04f sec", TargetTime );
                        } else {
                            sprintf( Label_TargetTime, "Spawn Target Time: ------" );
                        }
                        AddLabel( Panel, Pass_UI, Font, Label_TargetTime, Label_AdvanceY );
                        WrapPanel( Panel );
                        
                        char Label_nFrames[ 32 ] = {};
                        sprintf( Label_nFrames, " nFrames: %u/60 fps", ( uint32 )Event->Spawn_nFrames );
                        AddLabel( Panel, Pass_UI, Font, Label_nFrames, Label_AdvanceY );
                        
                        rect Bound_20 = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_5  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_1  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "20", Bound_20 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 20 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "5", Bound_5 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 5 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "1", Bound_1 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 1 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                    }
                } break;
                
                case WaveType_SetSwarmSettings: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 20.0f );
                    
                    SWARM_SETTINGS_EVENT * Event = &CurrentWave->SwarmSettingsEvent;
                    
                    { // Is Active
                        rect Bound = GetBound( Panel, Vec2( 80.0f, 20.0f ) );
                        char Label[ 16 ] = {};
                        if( Event->IsActive ) {
                            strcpy( Label, "Active" );
                        } else {
                            strcpy( Label, "Inactive" );
                        }
                        
                        if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, Label, Bound, Event->IsActive ) ) {
                            Event->IsActive = !Event->IsActive;
                        }
                    }
                    
                    { // Style
                        WrapPanel( Panel );
                        AddLabel( Panel, Pass_UI, Font, "        Style:", Label_AdvanceY );
                        
                        SWARM_SPAWN_STYLE_LABELS;
                        char * Label = SwarmSpawnStyleLabels[ Event->Style ];
                        
                        rect Bound = GetBound( Panel, Vec2( 80.0f, 20.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label, Bound ) ) {
                            Event->Style = ( SWARM_SPAWN_STYLE )( ( ( uint32 )Event->Style + 1 ) % SwarmSpawnStyle_Count );
                        }
                    }
                    
                    { // nSpawn
                        WrapPanel( Panel );
                        AddLabel( Panel, Pass_UI, Font, "       nSpawn:", Label_AdvanceY );
                        
                        char Label[ 64 ] = {};
                        sprintf( Label, "%u", ( uint32 )Event->nMaxSpawn );
                        
                        rect Bound = GetBound( Panel, Dim );
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label, Bound ) ) {
                            Event->nMaxSpawn += Mouse->WheelClicks;
                            Event->nMaxSpawn  = MaxValue( Event->nMaxSpawn, 0 );
                        }
                    }
                    
                    { // nRespawnAt
                        WrapPanel( Panel );
                        AddLabel( Panel, Pass_UI, Font, "   nRespawnAt:", Label_AdvanceY );
                        
                        char Label[ 64 ] = {};
                        sprintf( Label, "%u", ( uint32 )Event->nRespawnAt );
                        
                        rect Bound = GetBound( Panel, Dim );
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label, Bound ) ) {
                            Event->nRespawnAt += Mouse->WheelClicks;
                            Event->nRespawnAt  = MaxValue( Event->nRespawnAt, 0 );
                        }
                    }
                    
                    { // InnerRadius
                        WrapPanel( Panel );
                        AddLabel( Panel, Pass_UI, Font, "  InnerRadius:", Label_AdvanceY );
                        
                        char Label[ 64 ] = {};
                        sprintf( Label, "%.1f", Event->InnerRadius );
                        
                        
                        rect Bound = GetBound( Panel, Dim );
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label, Bound ) ) {
                            Event->InnerRadius += ( flo32 )Mouse->WheelClicks;
                        }
                        
                        Event->InnerRadius = MaxValue( Event->InnerRadius, 2.0f );
                    }
                    
                    { // OuterRadius
                        WrapPanel( Panel );
                        AddLabel( Panel, Pass_UI, Font, "  OuterRadius:", Label_AdvanceY );
                        
                        char Label[ 64 ] = {};
                        sprintf( Label, "%.1f", Event->OuterRadius );
                        
                        rect Bound = GetBound( Panel, Dim );
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label, Bound ) ) {
                            Event->OuterRadius += ( flo32 )Mouse->WheelClicks;
                        }
                        
                        Event->OuterRadius  = MaxValue( Event->OuterRadius, 4.0f );
                    }
                    
                    { // Delay_nFrames
                        WrapPanel( Panel );
                        AddLabel( Panel, Pass_UI, Font, "Delay_nFrames:", Label_AdvanceY );
                        
                        char Label[ 64 ] = {};
                        sprintf( Label, "%u", ( uint32 )Event->Delay_TargetTime_nFrames );
                        
                        rect Bound = GetBound( Panel, Dim );
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label, Bound ) ) {
                            Event->Delay_TargetTime_nFrames += Mouse->WheelClicks;
                            Event->Delay_TargetTime_nFrames  = MaxValue( Event->Delay_TargetTime_nFrames, 0 );
                        }
                    }
                } break;
                
                case WaveType_SetTurretSettings: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    TURRET_SETTINGS_EVENT * Event = &CurrentWave->TurretSettingsEvent;
                    
                    { // Spawn Target Time
                        flo32 TargetTime = ( flo32 )Event->Spawn_nFrames / 60.0f; 
                        char Label_TargetTime[ 64 ] = {};
                        sprintf( Label_TargetTime, "Spawn Target Time: %.04f sec", TargetTime );
                        AddLabel( Panel, Pass_UI, Font, Label_TargetTime, Label_AdvanceY );
                        WrapPanel( Panel );
                        
                        char Label_nFrames[ 32 ] = {};
                        sprintf( Label_nFrames, " nFrames: %u/60 fps", ( uint32 )Event->Spawn_nFrames );
                        AddLabel( Panel, Pass_UI, Font, Label_nFrames, Label_AdvanceY );
                        
                        rect Bound_20 = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_5  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_1  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "20", Bound_20 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 20 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "5", Bound_5 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 5 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "1", Bound_1 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 1 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                    }
                    
                    { // Max Turrets
                        WrapPanel( Panel, 8.0f );
                        
                        char Label_MaxTurret[ 32 ] = {};
                        if( Event->Spawn_MaxTurret < 0 ) {
                            strcpy( Label_MaxTurret, "Max Turrets: INF" );
                        } else if( Event->Spawn_MaxTurret == 0 ) {
                            strcpy( Label_MaxTurret, "Max Turrets: ---" );
                        } else {
                            sprintf( Label_MaxTurret, "Max Turrets: %u", ( uint32 )Event->Spawn_MaxTurret );
                        }
                        
                        rect Bound = GetBound( Panel, Vec2( 200.0f, 20.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label_MaxTurret, Bound ) ) {
                            Event->Spawn_MaxTurret += Mouse->WheelClicks;
                            Event->Spawn_MaxTurret  = MaxValue( Event->Spawn_MaxTurret, -1 );
                        }
                    }
                    
                    { // Turret Type and Class
                        WrapPanel( Panel );
                        
                        AddLabel( Panel, Pass_UI, Font, "        0   90   180  270  360", Label_AdvanceY );
                        WrapPanel( Panel );
                        
                        char * Label[ TurretSettings_Speed_Count ] = {
                            "Slow", "Fast",
                        };
                        
                        int32 Denom = 0;
                        for( uint32 iType = 0; iType < TurretSettings_Speed_Count; iType++ ) {
                            for( uint32 iClass = 0; iClass < TurretSettings_Radians_Count; iClass++ ) {
                                Denom += Event->Numer_SpeedRadians[ iType ][ iClass ];
                            }
                        }
                        
                        for( uint32 iType = 0; iType < TurretSettings_Speed_Count; iType++ ) {
                            AddLabel( Panel, Pass_UI, Font, Label[ iType ], Label_AdvanceY );
                            
                            int32 Numer_Type = 0;
                            
                            for( uint32 iClass = 0; iClass < TurretSettings_Radians_Count; iClass++ ) {
                                int32 * Numer_Class = Event->Numer_SpeedRadians[ iType ];
                                
                                Numer_Type += Numer_Class[ iClass ];
                                
                                char Label_Numer[ 8 ] = {};
                                if( Numer_Class[ iClass ] > 0 ) {
                                    sprintf( Label_Numer, "%u", ( uint32 )Numer_Class[ iClass ] );
                                } else {
                                    sprintf( Label_Numer, "-" );
                                }
                                
                                rect Bound = GetBound( Panel, Vec2( 36.0f, 16.0f ) );
                                if( AddScrollButton( Mouse, Pass_UI, Font, Label_Numer, Bound ) ) {
                                    Numer_Class[ iClass ] += Mouse->WheelClicks;
                                    Numer_Class[ iClass ]  = MaxValue( Numer_Class[ iClass ], 0 );
                                    
                                    Numer_Type += Mouse->WheelClicks;
                                    Numer_Type  = MaxValue( Numer_Type, 0 );
                                    
                                    Denom += Mouse->WheelClicks;
                                    Denom  = MaxValue( Denom, 0 );
                                }
                            }
                            
                            if( Denom > 0 ) {
                                flo32 Percent = ( flo32 )Numer_Type / ( flo32 )Denom;
                                char Label_Percent[ 12 ] = {};
                                sprintf( Label_Percent, "%.1f%%", Percent * 100.0f );
                                AddLabel( Panel, Pass_UI, Font, Label_Percent, Label_AdvanceY );
                            }
                            
                            WrapPanel( Panel );
                        }
                    }
                } break;
                
                case WaveType_EnableBombLauncher: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    rect Bound_iSrc  = GetBound( Panel, Dim );
                    
                    BOMB_LAUNCHER_EVENT * Event = &CurrentWave->BombLauncherEvent;
                    
                    char Label_iSrc [ 8 ] = {};
                    sprintf( Label_iSrc,  "%u", ( uint32 )Event->iSrc  );
                    
                    if( AddScrollButton( Mouse, Pass_UI, Font, Label_iSrc, Bound_iSrc ) ) {
                        Event->iSrc += Mouse->WheelClicks;
                        Event->iSrc  = Clamp( Event->iSrc, 0, BOMB_LAUNCHER_MAX_COUNT - 1 );
                    }
                } break;
                
                case WaveType_EnableMegaTurret: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    rect Bound_iSrc  = GetBound( Panel, Dim );
                    
                    MEGA_TURRET_EVENT * Event = &CurrentWave->MegaTurretEvent;
                    // NOTE : For now, we're only enabling the TurretS->SpawnMegaTurret_IsActive when this wave type is received. We're not disabling it. We're not changing any parameters.
                } break;
                
                case WaveType_EnableBlimp: {
                    WrapPanel( Panel, 12.0f );
                    
                    //vec2 Dim = Vec2( 40.0f, 16.0f );
                    //rect Bound_iSrc  = GetBound( Panel, Dim );
                    
                    BLIMP_EVENT * Event = &CurrentWave->BlimpEvent;
                    // NOTE : For now, we're only adding a Blimp to BLIMP_STATE with constants set in UpdateWave() this wave type is received.
                } break;
                
                case WaveType_EnableMegaMiner: {
                    WrapPanel( Panel, 12.0f );
                    
                    //vec2 Dim = Vec2( 40.0f, 16.0f );
                    //rect Bound_iSrc  = GetBound( Panel, Dim );
                    
                    MEGA_MINER_EVENT * Event = &CurrentWave->MegaMinerEvent;
                    // NOTE : For now, we're only adding a MegaMiner to MINER_STATE with constants set in UpdateWave() this wave type is received.
                } break;
                
                case WaveType_SetWormSettings: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    WORM_SETTINGS_EVENT * Event = &CurrentWave->WormSettingsEvent;
                    
                    { // Spawn Target Time
                        flo32 TargetTime = ( flo32 )Event->Spawn_nFrames / 60.0f; 
                        char Label_TargetTime[ 64 ] = {};
                        sprintf( Label_TargetTime, "Spawn Target Time: %.04f sec", TargetTime );
                        AddLabel( Panel, Pass_UI, Font, Label_TargetTime, Label_AdvanceY );
                        WrapPanel( Panel );
                        
                        char Label_nFrames[ 32 ] = {};
                        sprintf( Label_nFrames, " nFrames: %u/60 fps", ( uint32 )Event->Spawn_nFrames );
                        AddLabel( Panel, Pass_UI, Font, Label_nFrames, Label_AdvanceY );
                        
                        rect Bound_20 = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_5  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        rect Bound_1  = GetBound( Panel, Dim * Vec2( 0.8f, 1.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "20", Bound_20 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 20 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "5", Bound_5 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 5 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, "1", Bound_1 ) ) {
                            Event->Spawn_nFrames += ( Mouse->WheelClicks * 1 );
                            Event->Spawn_nFrames  = MaxValue( Event->Spawn_nFrames, 0 );
                        }
                    }
                    
                    { // Max Worms
                        WrapPanel( Panel, 8.0f );
                        
                        char Label_MaxWorm[ 32 ] = {};
                        if( Event->Spawn_MaxWorm < 0 ) {
                            strcpy( Label_MaxWorm, "Max Worms: INF" );
                        } else if( Event->Spawn_MaxWorm == 0 ) {
                            strcpy( Label_MaxWorm, "Max Worms: ---" );
                        } else {
                            sprintf( Label_MaxWorm, "Max Worms: %u", ( uint32 )Event->Spawn_MaxWorm );
                        }
                        
                        rect Bound = GetBound( Panel, Vec2( 200.0f, 20.0f ) );
                        
                        if( AddScrollButton( Mouse, Pass_UI, Font, Label_MaxWorm, Bound ) ) {
                            Event->Spawn_MaxWorm += Mouse->WheelClicks;
                            Event->Spawn_MaxWorm  = MaxValue( Event->Spawn_MaxWorm, -1 );
                        }
                    }
                } break;
                
                case WaveType_EnableLoader: {
                    WrapPanel( Panel, 12.0f );
                    
                    //vec2 Dim = Vec2( 40.0f, 16.0f );
                    //rect Bound_iSrc  = GetBound( Panel, Dim );
                    
                    LOADER_EVENT * Event = &CurrentWave->LoaderEvent;
                    // NOTE : For now, we're only adding a Loader to SPITTER_STATE with constants set in UpdateWave() when this wave type is received.
                } break;
                
                case WaveType_SetSpitterSettings: {
                    WrapPanel( Panel, 12.0f );
                    
                    vec2 Dim = Vec2( 40.0f, 16.0f );
                    
                    SPITTER_SETTINGS_EVENT * Event = &CurrentWave->SpitterSettingsEvent;
                    
                    SPITTER_GROUP_ID_LABELS;
                    
                    char Str[ 128 ] = {};
                    for( int32 iGroupID = 0; iGroupID < SpitterGroupID_Count; iGroupID++ ) {
                        sprintf( Str, "%12s", SpitterGroupIDLabels[ iGroupID ] );
                        AddLabel( Panel, Pass_UI, Font, Str, Label_AdvanceY );
                        
                        if( AddToggleButton( Mouse, MouseButton_Left, Pass_UI, Font, "", GetBound( Panel, Vec2( 20.0f, 20.0f ) ), Event->Group_IsActive[ iGroupID ] ) ) {
                            Event->Group_IsActive[ iGroupID ] = !Event->Group_IsActive[ iGroupID ];
                        }
                        
                        if( ( iGroupID == SpitterGroupID_Spiral ) && ( Event->Group_IsActive[ iGroupID ] ) ) {
                            rect Bound = GetBound( Panel, Vec2( 32.0f, 20.0f ) );
                            sprintf( Str, "%u", Event->nSpiralSpittersToFire );
                            if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                                Event->nSpiralSpittersToFire = Clamp( Event->nSpiralSpittersToFire + Mouse->WheelClicks, 0, 2 );
                            }
                            
                            AddLabel( Panel, Pass_UI, Font, ": n to Fire", Label_AdvanceY );
                        }
                        
                        WrapPanel( Panel );
                    }
                } break;
                
                case WaveType_EnableLevelTimer: {
                    WrapPanel( Panel, 12.0f );
                } break;
                
                default: {
                    InvalidCodePath;
                } break;
            }
        }
        
        {
            WrapPanel( Panel, 12.0f );
            AddLabel( Panel, Pass_UI, Font, " Exit Settings", Label_AdvanceY );
            
            WrapPanel( Panel );
            { // Exit Type
                EXIT_TYPE_LABELS;
                
                char Str[ 64 ] = {};
                sprintf( Str, "Type: %s", ExitTypeLabels[ CurrentWave->Exit ] );
                rect Bound = GetBound( Panel, Vec2( GetWidth( Panel->Inner ) - 4.0f, 28.0f ) );
                if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                    CurrentWave->Exit = ( EXIT_TYPE )UInt32Wrap( CurrentWave->Exit, Mouse->WheelClicks, ExitType_Count );
                }
                
                if( CurrentWave->Exit == ExitType_KillAllEntitiesInWave ) {
                    uint32 nTotalEntitiesInWave = CountTotalEntitiesInWave( CurrentWave );
                    
                    sprintf( Str, "  nEntities in Wave: %u", nTotalEntitiesInWave );
                    AddLabel( Panel, Pass_UI, Font, Str, Label_AdvanceY );
                }
            }
        }
        
        switch( CurrentWave->Exit ) {
            case ExitType_Instant: {
                
            } break;
            
            case ExitType_BayDoorEventIsComplete:
            case ExitType_TargetTime: {
                WrapPanel( Panel );
                AddX    ( Panel, 8.0f );
                AddLabel( Panel, Pass_UI, Font, "Time:     ", Label_AdvanceY );
                
                vec2 TimeCounter_Dim = Vec2( 48.0f, 20.0f );
                
                boo32 DoUpdateTargetTime = false;
                
                int32 Minutes = ( int32 )CurrentWave->Exit_TargetTime / 60;
                int32 Seconds = ( int32 )CurrentWave->Exit_TargetTime % 60;
                
                char Str_Minutes[ 16 ] = {};
                char Str_Seconds[ 16 ] = {};
                sprintf( Str_Minutes, "%d", Minutes );
                sprintf( Str_Seconds, "%d", Seconds );
                
                rect Bound_Minutes = GetBound( Panel, TimeCounter_Dim );
                if( AddScrollButton( Mouse, Pass_UI, Font, Str_Minutes, Bound_Minutes ) ) {
                    Minutes += Mouse->WheelClicks;
                    Minutes  = MaxValue( Minutes, 0 );
                    DoUpdateTargetTime = true;
                }
                AddLabel( Panel, Pass_UI, Font, ":", Label_AdvanceY );
                
                rect Bound_Seconds = GetBound( Panel, TimeCounter_Dim );
                if( AddScrollButton( Mouse, Pass_UI, Font, Str_Seconds, Bound_Seconds ) ) {
                    Seconds += Mouse->WheelClicks;
                    Seconds  = MaxValue( Seconds, 0 );
                    DoUpdateTargetTime = true;
                    
                    while( Seconds >= 60 ) {
                        Seconds -= 60;
                        Minutes += 1;
                    }
                }
                
                if( DoUpdateTargetTime ) {
                    CurrentWave->Exit_TargetTime = ( flo32 )( Minutes * 60 + Seconds );
                    
                    if( CurrentWave->Type == WaveType_OpenBayDoors ) {
                        CurrentWave->BayDoorEvent.TargetTime = CurrentWave->Exit_TargetTime;
                    }
                }
            } break;
            
            case ExitType_NoExit: {
                
            } break;
        }
        
        { // In Delay
            WrapPanel( Panel );
            AddX    ( Panel, 8.0f );
            AddLabel( Panel, Pass_UI, Font, "In Delay: ", Label_AdvanceY );
            
            vec2 TimeCounter_Dim = Vec2( 48.0f, 20.0f );
            
            boo32 DoUpdateTargetTime = false;
            
            int32 Minutes = ( int32 )CurrentWave->InDelay_TargetTime / 60;
            int32 Seconds = ( int32 )CurrentWave->InDelay_TargetTime % 60;
            
            char Str_Minutes[ 16 ] = {};
            char Str_Seconds[ 16 ] = {};
            sprintf( Str_Minutes, "%d", Minutes );
            sprintf( Str_Seconds, "%d", Seconds );
            
            rect Bound_Minutes = GetBound( Panel, TimeCounter_Dim );
            if( AddScrollButton( Mouse, Pass_UI, Font, Str_Minutes, Bound_Minutes ) ) {
                Minutes += Mouse->WheelClicks;
                Minutes  = MaxValue( Minutes, 0 );
                DoUpdateTargetTime = true;
            }
            AddLabel( Panel, Pass_UI, Font, ":", Label_AdvanceY );
            
            rect Bound_Seconds = GetBound( Panel, TimeCounter_Dim );
            if( AddScrollButton( Mouse, Pass_UI, Font, Str_Seconds, Bound_Seconds ) ) {
                Seconds += Mouse->WheelClicks;
                Seconds  = MaxValue( Seconds, 0 );
                DoUpdateTargetTime = true;
                
                while( Seconds >= 60 ) {
                    Seconds -= 60;
                    Minutes += 1;
                }
            }
            
            if( DoUpdateTargetTime ) {
                CurrentWave->InDelay_TargetTime = ( flo32 )( Minutes * 60 + Seconds );
            }
        }
        
        { // Out Delay
            WrapPanel( Panel );
            AddX    ( Panel, 8.0f );
            AddLabel( Panel, Pass_UI, Font, "Out Delay:", Label_AdvanceY );
            
            vec2 TimeCounter_Dim = Vec2( 48.0f, 20.0f );
            
            boo32 DoUpdateTargetTime = false;
            
            int32 Minutes = ( int32 )CurrentWave->OutDelay_TargetTime / 60;
            int32 Seconds = ( int32 )CurrentWave->OutDelay_TargetTime % 60;
            
            char Str_Minutes[ 16 ] = {};
            char Str_Seconds[ 16 ] = {};
            sprintf( Str_Minutes, "%d", Minutes );
            sprintf( Str_Seconds, "%d", Seconds );
            
            rect Bound_Minutes = GetBound( Panel, TimeCounter_Dim );
            if( AddScrollButton( Mouse, Pass_UI, Font, Str_Minutes, Bound_Minutes ) ) {
                Minutes += Mouse->WheelClicks;
                Minutes  = MaxValue( Minutes, 0 );
                DoUpdateTargetTime = true;
            }
            AddLabel( Panel, Pass_UI, Font, ":", Label_AdvanceY );
            
            rect Bound_Seconds = GetBound( Panel, TimeCounter_Dim );
            if( AddScrollButton( Mouse, Pass_UI, Font, Str_Seconds, Bound_Seconds ) ) {
                Seconds += Mouse->WheelClicks;
                Seconds  = MaxValue( Seconds, 0 );
                DoUpdateTargetTime = true;
                
                while( Seconds >= 60 ) {
                    Seconds -= 60;
                    Minutes += 1;
                }
            }
            
            if( DoUpdateTargetTime ) {
                CurrentWave->OutDelay_TargetTime = ( flo32 )( Minutes * 60 + Seconds );
            }
        }
        
        { // nRepeatWave
            WrapPanel( Panel );
            rect Bound = GetBound( Panel, Vec2( 180.0f, 28.0f ) );
            
            char Str[ 128 ] = {};
            if( CurrentWave->nRepeatWave < 0 ) {
                strcpy( Str, "nRepeat: INF" );
            } else if( CurrentWave->nRepeatWave == 0 ) {
                strcpy( Str, "nRepeat: NONE" );
            } else {
                sprintf( Str, "nRepeat: %d (%d Total)", CurrentWave->nRepeatWave, CurrentWave->nRepeatWave + 1 );
            }
            
            if( AddScrollButton( Mouse, Pass_UI, Font, Str, Bound ) ) {
                CurrentWave->nRepeatWave += Mouse->WheelClicks;
                CurrentWave->nRepeatWave  = MaxValue( CurrentWave->nRepeatWave, -1 );
            }
        }
    }
}

internal int32
UpdateAndDrawLayoutPanel( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, int32 iHover_iWave ) {
    int32 iHover = -1;
    
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Layout_Panel;
    FONT         * Font   = AppState->Font;
    
    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
    
    flo32 Margin = 4.0f;
    Panel->Inner = AddRadius( Panel->Bound, -Margin );
    Panel->AtPos = GetTL( Panel->Inner );
    
    DrawRect( Pass_UI, Panel->Bound, COLOR_WHITE );
    
    // SCROLLING
    vec2  Dim     = Vec2( GetWidth( Panel->Inner ) - 4.0f, 24.0f );
    flo32 MarginY = 2.0f;
    flo32 List_Height  = ( flo32 )WaveS->nWave * Dim.y + ( flo32 )( WaveS->nWave - 1 ) * MarginY;
    flo32 Panel_Height = GetHeight( Panel->Inner );
    
    if( List_Height > Panel_Height ) {
        flo32 Scroll_Height = List_Height - Panel_Height;
        Panel->AtPos.y += Scroll_Height * Panel->ScrollT;
        
        uint32 nEntriesPerScroll = 1;
        flo32 ScrollT = ( flo32 )( ( Dim.y + MarginY ) * ( flo32 )nEntriesPerScroll ) / Scroll_Height;
        if( Mouse->WheelClicks != 0 ) {
            Panel->ScrollT = Clamp01( Panel->ScrollT - ( ( flo32 )Mouse->WheelClicks * ScrollT ) );
        }
        
        flo32 Bar_Height = Clamp01( Panel_Height / List_Height ) * Panel_Height;
        flo32 Bar_Y      = Panel->ScrollT * ( Panel_Height - Bar_Height );
        rect  Bar_Bound  = RectTRD( GetTR( Panel->Inner ) + Vec2( 0.0f, -Bar_Y ), Vec2( 2.0f, Bar_Height ) );
        DrawRect( Pass_UI, Bar_Bound, COLOR_BLACK );
    }
    
    { // Wave List
        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WAVE * Wave = WaveS->Wave + iWave;
            //if( FindSubStringNoCase( Wave->name, Filter->String ) ) {
            rect R = RectTLD( Panel->AtPos, Dim );
            DrawRect( Pass_UI, R, COLOR_GRAY( 0.8f ) );
            
            char Str[ 64 ] = {};
            sprintf( Str, "%u) #%d", iWave, Wave->Guid );
            DrawString( Pass_UI, Font, Str, Panel->AtPos + Vec2( 6.0f, -4.0f ), TextAlign_TopLeft, COLOR_BLACK );
            
            if( ( IsInRect( Mouse->Position, R ) ) || ( ( int32 )iWave == iHover_iWave ) ) {
                iHover = iWave;
                
                DrawRectOutline( Pass_UI, R, COLOR_BLACK );
            }
            if( ( IsInRect( Mouse->Position, R ) ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                CAMERA_STATE * Camera = &Editor->Camera_LayoutEditor;
                Camera->Pos = Wave->Layout_Position;
            }
            
            Panel->AtPos.y -= ( Dim.y + 2.0f );
            //}
        }
    }
    
    return iHover;
}

internal void
Editor_DrawEntity( RENDER_PASS * Pass, FONT * Font, CAMERA_STATE * Camera, ENTITY Entity, boo32 IsHover, boo32 IsSelected, boo32 DoesIntersectEntity = false ) {
    flo32 Radius = Entity.Radius + 0.1f;
    rect  Bound  = RectCR( Entity.Position, Radius );
    
    vec4 BackgroundColor = COLOR_BLACK;
    vec4 OutlineColor    = Vec4( 0.1f, 0.1f, 0.1f, 1.0f );
    if( IsSelected ) {
        OutlineColor = COLOR_YELLOW;
    } else if( IsHover ) {
        OutlineColor = Vec4( 0.6f, 0.6f, 0.6f, 1.0f );
    }
    
    DrawRect       ( Pass, Bound, BackgroundColor );
    DrawRectOutline( Pass, Bound, OutlineColor );
    
    switch( Entity.Type ) {
        case EntityType_Turret: {
            TURRET Turret = TurretC( Entity.WaveGuid, Entity.Position, Entity.Radius, ( uint32 )Entity.Radius * 10, Entity.Shield );
            Editor_DrawTurret( Pass, Font, Camera, Turret );
        } break;
        
        case EntityType_Chaser: {
            CHASER Chaser = ChaserC( Entity.WaveGuid, Entity.Position );
            DrawChaser( Pass, Chaser );
        } break;
        
        case EntityType_Swarm: {
            SWARM Swarm = SwarmC( Entity.WaveGuid, Entity.Position );
            Swarm.Velocity = Vec2( 0.0f, 1.0f );
            DrawSwarm( Pass, Swarm );
        } break;
        
        case EntityType_Drifter: {
            DRIFTER Drifter = {};
            Drifter.WaveGuid = Entity.WaveGuid;
            Drifter.Position = Entity.Position;
            Drifter.Velocity = Vec2( 1.0f, 0.0f );
            DrawDrifter( Pass, Drifter );
        } break;
        
        case EntityType_Looper: {
            LOOPER Looper = LooperC( Entity.WaveGuid, Entity.Position );
            DrawLooper( Pass, Looper );
        } break;
        
#if 0        
        case EntityType_Starburst: {
            STARBURST Starburst = StarburstC( Entity.WaveGuid, Entity.Position );
            DrawStarburst( Pass, Starburst );
        } break;
#endif
        
        case EntityType_Miner: {
            uint32 nLaser = Entity.nEntity;
            MINER   Miner = MinerC_Stationary( Entity.WaveGuid, Entity.Position, 0.0f, nLaser );
            DrawMiner( Pass, 0, 0, Miner );
        } break;
        
#if 0        
        case EntityType_Bomber: {
            uint32 nBomb = Entity.nEntity;
            BOMBER Bomber = BomberC( Entity.WaveGuid, Entity.Position, nBomb );
            DrawBomber( Pass, 0, 0, Bomber );
        } break;
        
        case EntityType_Bumper: {
            BUMPER Bumper = BumperC( Entity.WaveGuid, Entity.Position );
            DrawBumper( Pass, Bumper );
        } break;
        
        case EntityType_Bank: {
            BANK Bank = BankC( Entity.WaveGuid, Entity.Position );
            DrawBank( Pass, Bank );
        } break;
#endif
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    char NumString[ 4 ] = {};
    sprintf( NumString, "%u", Entity.nEntity );
    DrawString( Pass, Font, NumString, GetBR( Bound ), TextAlign_BottomRight, WORLD_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE );
    
    if( DoesIntersectEntity ) {
        DrawRect( Pass, Bound, Vec4( 0.5f, 0.0f, 0.0f, 0.5f ) );
    }
}