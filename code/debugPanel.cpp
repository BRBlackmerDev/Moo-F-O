
internal void
InitDebugPanel( APP_STATE * AppState ) {
    PANEL * Panel = &AppState->Debug;
    
    {
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        Panel->Bound = R;
    }
}

internal void
UpdateAndDrawDebugPanel( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw ) {
    PANEL * Panel = &AppState->Debug;
    if( Panel->Show ) {
        Panel->Inner = AddRadius( Panel->Bound, -4.0f );
        Panel->AtPos = GetTL( Panel->Inner );
        Panel->AdvanceY = 0.0f;
        
        DrawRect( Pass_UI, Panel->Bound, COLOR_WHITE );
        
        boo32 * IsActive = AppState->Debug_IsActive;
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "MULT", DrawScoreMultiplierDebug, IsActive++ );
        
        Assert( ( IsActive - AppState->Debug_IsActive ) <= DEBUG_MAX_COUNT );
    }
}