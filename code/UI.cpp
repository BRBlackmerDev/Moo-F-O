
internal void
DrawUI( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    PLAYER_STATE * Player = Draw->Player;
    SCORE_STATE  * ScoreS = Draw->ScoreS;
    
    vec2 Panel_Dim   = Vec2( AppState->UI_Dim.x, UI_PANEL_HEIGHT );
    rect Panel_Bound = RectTLD( GetTL( AppState->UI_Bound ), Panel_Dim );
    DrawRect( Pass, AddDim( Panel_Bound, 0.0f, 0.0f, 0.0f, 10.0f ), COLOR_BLACK );
    
    { // PLAYER HEALTH
        vec2  bPos = GetLC( Panel_Bound ) + Vec2( 60.0f, 0.0f );
        
        DrawString( Pass, AppState->FontList + FontID_1_25, "HP:", bPos + Vec2( -8.0f, 0.0f ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
        
        flo32 HealthUnit_Width  = 18.0f;
        flo32 HealthUnit_Margin = 8.0f;
        vec2  HealthUnit_Dim    = Vec2( HealthUnit_Width, HealthUnit_Width );
        
        vec2  At = bPos;
        
        rect  LastFull_Bound = {};
        
        int32 nHealthUnit = 3;
        for( int32 iHealthUnit = 0; iHealthUnit < nHealthUnit; iHealthUnit++ ) {
            rect R = RectLCD( At, HealthUnit_Dim );
            
            vec4 Color = COLOR_RED;
            if( iHealthUnit >= ( int32 )Player->Health ) {
                Color = ToColor( 50, 20, 20 );
            }
            
            DrawRect( Pass, R, Color );
            
            if( iHealthUnit == ( ( int32 )Player->Health - 1 ) ) {
                LastFull_Bound = R;
            }
            
            At.x += ( HealthUnit_Width + HealthUnit_Margin );
        }
        
        if( Player->SecondaryDamage_Timer > 0.0f ) {
            flo32 t = Clamp01( Player->SecondaryDamage_Timer / PLAYER_SECONDARY_DAMAGE_TARGET_TIME );
            rect  Bound = LastFull_Bound;
            Bound.Bottom = lerp( Bound.Top, t, Bound.Bottom );
            
            vec4 ColorA = ToColor( 100, 20, 0 );
            vec4 ColorB = ToColor( 80, 0, 0 );
            
            flo32 Radians = Player->SecondaryDamage_Timer * 4.0f * PI * 2.0f;
            flo32 tColor  = cosf( Radians ) * 0.5f + 0.5f;
            vec4  Color   = lerp( ColorA, tColor, ColorB );
            
            DrawRect( Pass, Bound, Color );
        }
    }
    
    {
        vec2  Colon_Pos       = GetCenter( Panel_Bound );
        flo32 OffsetFromColon = 8.0f;
        FONT * Font = AppState->FontList + FontID_2_00;
        
        if( ScoreS->Level_DisplayTimer >= 0.0f ) {
            int32 Minutes = ( int32 )ScoreS->Level_DisplayTimer / 60;
            int32 Seconds = ( int32 )ScoreS->Level_DisplayTimer % 60;
            
            char Str[ 16 ] = {};
            
            DrawString( Pass, Font, ":", Colon_Pos, TextAlign_Center, COLOR_WHITE );
            
            sprintf( Str, "%2d", Minutes );
            DrawString( Pass, Font, Str, Colon_Pos + Vec2( -OffsetFromColon, 0.0f ), TextAlign_CenterRight, COLOR_WHITE );
            
            sprintf( Str, "%02d", Seconds );
            DrawString( Pass, Font, Str, Colon_Pos + Vec2(  OffsetFromColon, 0.0f ), TextAlign_CenterLeft, COLOR_WHITE );
        } else {
            DrawString( Pass, Font, "--:--", Colon_Pos, TextAlign_Center, COLOR_WHITE );
        }
    }
    
    { // SCORE
        vec2  bPos = GetRC( Panel_Bound ) + Vec2( -20.0f, -1.0f );
        vec2  At   = bPos;
        
        flo32 Score = ScoreS->Score;
        if( ScoreS->DoBonusObjective ) {
            Score *= 2.0f;
        }
        
        FONT * Font = AppState->FontList + FontID_2_00;
        
        char Str[ 64 ] = {};
        
        sprintf( Str, "%ux", ( uint32 )ScoreS->Multiplier );
        DrawString( Pass, Font, Str, At, TextAlign_CenterRight, Vec4( 0.5f, 1.0f, 0.5f, 1.0f ) );
        
        if( ScoreS->Multiplier >= 100.0f ) {
            At.x -= 108.0f;
        } else {
            At.x -=  88.0f;
        }
        
        sprintf( Str, "%u", ( uint32 )Score );
        DrawString( Pass, Font, Str, At, TextAlign_CenterRight, Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
        
        vec2 Bar_Dim   = Vec2( 212.0f, 5.0f );
        rect Bar_Bound = RectTRD( GetBR( Panel_Bound ) + Vec2( -20.0f, -4.0f ), Bar_Dim );
        
        flo32 t = 1.0f - ScoreS->BonusMultiplier;
        DrawBarForBonusMultiplier( Pass, Bar_Bound, 1.0f, ScoreS->BonusMultiplier, true, ScoreS->BonusMultiplier_Radians );
    }
    
    { // PROGRESS METER
        //DrawProgressMeter( Pass, AppState, Draw );
    }
}