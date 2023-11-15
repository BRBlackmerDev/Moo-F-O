

internal void
WrapPanel( PANEL * Panel, flo32 addY = 0.0f ) {
    flo32 Inner_margin = 4.0f;
    
    Panel->AtPos.x  = Panel->Inner.Left;
    Panel->AtPos.y -= ( Panel->AdvanceY + Inner_margin + addY );
    
    Panel->AdvanceY = 0.0f;
}

internal rect
GetBound( PANEL * Panel, vec2 Dim ) {
    vec2 Inner_margin = Vec2( 4.0f, 4.0f );
    vec2 advance      = Dim + Inner_margin;
    
    if( ( Panel->AtPos.x + advance.x ) >= Panel->Inner.Right ) {
        WrapPanel( Panel );
    }
    Panel->AdvanceY = MaxValue( Panel->AdvanceY, Dim.y );
    
    rect Result = RectTLD( Panel->AtPos, Dim );
    Panel->AtPos.x += advance.x;
    
    return Result;
}

internal void
AddX( PANEL * Panel, flo32 X ) {
    if( ( Panel->AtPos.x + X ) >= Panel->Inner.Right ) {
        WrapPanel( Panel );
    }
    Panel->AtPos.x += X;
}

internal void
AddLabel( PANEL * Panel, RENDER_PASS * Pass, FONT * Font, char * Label, flo32 AdvanceY ) {
    vec2 Dim   = Vec2( GetWidth( Font, Label ), AdvanceY );
    rect Bound = GetBound( Panel, Dim );
    
    flo32 Margin = 8.0f;
    
    DrawString( Pass, Font, Label, GetTL( Bound ) + Vec2( Margin, 0.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    Panel->AtPos.x += ( Margin * 2.0f );
}

typedef void ( RENDER_FUNC )( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw );

internal void
AddDebugButton( PANEL * Panel, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, APP_STATE * AppState, DRAW_STATE * Draw, char * label, RENDER_FUNC * func, boo32 * value ) {
    vec2 button_Dim   = Vec2( 72.0f, 28.0f );
    rect Bound = GetBound( Panel, button_Dim );
    
    if( ( WasPressed( Mouse, MouseButton_Left ) ) && ( IsInRect( Mouse->Position, Bound ) ) ) {
        *value = !( *value );
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( *value ) {
        func( Pass_Game, AppState, Draw );
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->Font, label, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal void
addS32Counter( PANEL * Panel, MOUSE_STATE * Mouse, RENDER_PASS * Pass, FONT * Font, char * label, vec2 Dim, int32 * value, int32 lo, int32 hi ) {
    rect Bound = GetBound( Panel, Dim );
    
    *value = Clamp( *value, lo, hi );
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( IsInRect( Mouse->Position, Bound ) ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
        if( Mouse->WheelClicks != 0 ) {
            int32 at    = *value - lo;
            int32 range = hi - lo;
            *value      = lo + ( at + ( range * 2 ) + Mouse->WheelClicks ) % range;
        }
    }
    
    DrawRect( Pass, Bound, Color );
    DrawRectOutline( Pass, Bound, COLOR_BLACK );
    
    char str[ 32 ] = {};
    sprintf( str, "%s: %d", label, *value );
    DrawString( Pass, Font, str, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}