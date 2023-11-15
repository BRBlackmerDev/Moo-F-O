
internal boo32
DrawAndUpdateTitleButton( RENDER_PASS * Pass, MOUSE_STATE * Mouse, FONT * Font, char * Label, rect Button_Bound, boo32 IsActive = true, vec2 TextOffset = Vec2( 0.0f, 0.0f ) ) {
    DrawString( Pass, Font, Label, GetCenter( Button_Bound ) + TextOffset, TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
    
    boo32 ButtonWasPressed = false;
    
    if( ( IsActive ) && ( IsInRect( Mouse->Position, Button_Bound ) ) ) {
        DrawRectOutline( Pass, Button_Bound, COLOR_YELLOW );
        if( WasPressed( Mouse, MouseButton_Left ) ) {
            ButtonWasPressed = true;
        }
    }
    
    return ButtonWasPressed;
}

internal void
APP_UpdateAndRender( APP_STATE * AppState, PLATFORM * Platform ) {
    RENDERER * Renderer = &Platform->Renderer;
    RENDER_PASS * Pass_Game = &Renderer->Pass_Game;
    RENDER_PASS * Pass_UI   = &Renderer->Pass_UI;
    
	MOUSE_STATE      * Mouse    = &Platform->Mouse;
	KEYBOARD_STATE   * Keyboard = &Platform->Keyboard;
	
	MEMORY * PermMemory = &Platform->PermMemory;
	MEMORY * TempMemory = &Platform->TempMemory;
	
    AUDIO_SYSTEM * AudioSystem = &Platform->AudioSystem;
	//ASSET_SYSTEM * assetSystem = &Platform->assetSystem;
    
    flo32 dT = Platform->TargetSec;
    AppState->dT = dT;
    
    DrawString( Pass_UI, AppState->FontList + FontID_1_00, "Why does the first drawn character flicker?", Vec2( 960.0f, -540.0f ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
    
    if( !AppState->IsInitialized ) {
        AppState->IsInitialized    = true;
        
#if ENABLE_DEBUG_REPLAY
        InitDebugPanel ( AppState );
#endif
        
        AppState->UI_Bound   = AppState->App_Bound;
        AppState->UI_Dim     = GetDim( AppState->UI_Bound );
        AppState->UI_HalfDim = AppState->UI_Dim * 0.5f;
        
        ZoomCameraToWorldBound( &AppState->Camera, AppState->UI_Bound, AppState->UI_Bound, AppState->UI_Bound );
        
        GenOverworld   ( AppState );
        GenPowerupStats( AppState );
        
        StartGame( Platform, AppState, TempMemory );
        
        AppState->AudioSystem = AudioSystem;
        AudioSystem->MainVolume = 0.5f;
        
        AppState->Mode = AppMode_TitleScreen;
        
        int32 nScript = 0;
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_TitleScreen_Intro, MUSIC_TITLE_SCREEN_VOLUME, 1.0f, false, AudioID_Music_TitleScreen_Loop );
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_TitleScreen_Loop,  MUSIC_TITLE_SCREEN_VOLUME, 1.0f, true );
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_Level_Track01_Intro, MUSIC_LEVEL_VOLUME, 1.0f, false, AudioID_Music_Level_Track01_Loop );
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_Level_Track01_Loop,  MUSIC_LEVEL_VOLUME, 1.0f, true );
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_Level_Track03_Intro, MUSIC_LEVEL_VOLUME, 1.0f, false, AudioID_Music_Level_Track03_Loop );
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_Level_Track03_Loop,  MUSIC_LEVEL_VOLUME, 1.0f, true );
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_Level_Track04_Intro, MUSIC_LEVEL_VOLUME, 1.0f, false, AudioID_Music_Level_Track04_Loop );
        AppState->AudioScript[ nScript++ ] = AudioEntry( AppState, AudioID_Music_Level_Track04_Loop,  MUSIC_LEVEL_VOLUME, 1.0f, true );
        
        Assert( nScript <= AUDIO_SCRIPT_MAX_COUNT  );
        
        PlayAudioScript( AppState, AudioID_Music_TitleScreen_Intro );
    }
    
    if( !MusicIsPlaying( AudioSystem ) ) {
        PlayAudioScript( AppState, AudioID_Music_TitleScreen_Intro );
    }
    
    switch( AppState->Mode ) {
        case AppMode_TitleScreen: {
            {
                flo32 dAlpha = 1.0f / 0.5f;
                AppState->FadeOut_Alpha = Clamp01( AppState->FadeOut_Alpha + dAlpha * dT );
            }
            
            if( AppState->TitleMenu_ShowConfirm ) {
                vec2 Button_Dim = Vec2( 200.0f, 80.0f );
                vec2 bPos       = GetCenter( AppState->UI_Bound );
                
                FONT * Font_Big   = AppState->FontList + FontID_2_25;
                FONT * Font_Small = AppState->FontList + FontID_2_00;
                
                DrawString( Pass_UI, Font_Big, "Are you sure?", bPos + Vec2( 0.0f, 40.0f ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, Font_Small, "This will delete your save file", bPos + Vec2( 0.0f, 0.0f ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, Font_Small, "and restart the game!", bPos + Vec2( 0.0f, -40.0f ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                
                vec2 Continue_Pos = bPos + Vec2( 0.0f, -120.0f );
                DrawString( Pass_UI, Font_Big, "Continue?", Continue_Pos, TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                
                if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "Yes", RectTRD( Continue_Pos + Vec2( -10.0, -40.0f ), Button_Dim ) ) ) {
                    // PlayAudio( AppState, AudioID_Button_Yes, BUTTON_VOLUME );
                    
                    AppState->TitleMenu_ShowConfirm = false;
                    
                    if( Platform->DoesFileExist( 0, "MyGame", FILETAG__SAVEGAME_FILE ) ) {
                        Platform->DeleteFile( 0, "MyGame", FILETAG__SAVEGAME_FILE );
                    }
                    if( Platform->DoesFileExist( 0, "MyGame", "POWERUPS" ) ) {
                        Platform->DeleteFile( 0, "MyGame", "POWERUPS" );
                    }
                    
                    ResetGameData  ( AppState );
                    GenOverworld   ( AppState );
                    GenPowerupStats( AppState );
                    
                    StartGame( Platform, AppState, TempMemory );
                }
                if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "No", RectTLD( Continue_Pos + Vec2( 10.0f, -40.0f ), Button_Dim ) ) ) {
                    AppState->TitleMenu_ShowConfirm = false;
                    // PlayAudio( AppState, AudioID_Button_No, BUTTON_VOLUME );
                }
            } else {
                TITLE_MENU_COW * Cow = &AppState->TitleMenu_Cow;
                { // Update Cow
                    Cow->Radians = WrapRadians( Cow->Radians - PLAYER_ROTATE_SPEED_SLOW * dT * 0.5f );
                    
                    vec2 Range_Min = Vec2( 12.0f,  6.0f );
                    vec2 Range_Max = Vec2( 50.0f, 30.0f );
                    vec2 ZRange    = Vec2( -10.0f, -50.0f );
                    
                    if( Cow->Position == Vec3( 0.0f, 0.0f, 0.0f ) ) {
                        Cow->AtTheta     = PI * 0.5f;
                        Cow->Direction   = Vec3( cosf( Cow->AtPhi ) * cosf( Cow->AtTheta ), sinf( Cow->AtPhi ), -sinf( Cow->AtTheta ) );
                        Cow->Velocity    = Cow->Direction * COW_SPEED_FOR_TITLE_SCREEN * 0.75f;
                        
                        rect Bound = RectCD( Vec2( 0.0f, 0.0f ), Range_Min * 2.0f );
                        vec2 Pos   = GetRandomPointOnPerimeter( Bound );
                        
                        Cow->Position    = Vec3( Pos.x, Pos.y, 0.0f );
                        Cow->Destination = Vec3( 0.0f, 0.0f, lerp( ZRange.x, 0.5f, ZRange.y ) );
                    }
                    
                    flo32 TargetTime = ( 180.0f / 60.0f );
                    Cow->Timer += dT;
                    if( Cow->Timer >= TargetTime ) {
                        Cow->Timer -= TargetTime;
                        flo32 tZ = RandomF32();
                        
                        flo32 XRange = lerp( Range_Min.x, tZ, Range_Max.x );
                        flo32 YRange = lerp( Range_Min.y, tZ, Range_Max.y );
                        flo32 X = RandomF32InRange( -XRange, XRange );
                        flo32 Y = RandomF32InRange( -YRange, YRange );
                        flo32 Z = lerp( ZRange.x, tZ, ZRange.y );
                        Cow->Destination = Vec3( X, Y, Z );
                    }
                    
                    flo32 Speed    = COW_SPEED_FOR_TITLE_SCREEN;
                    flo32 Friction =  0.5f;
                    
                    vec3 V = Cow->Destination - Cow->Position;
                    Cow->DestPhi = ToRadians( Vec2( fabsf( V.x ),  V.y ) );
                    
                    flo32 dTheta  = -( PI / 4.0f ) * dT;
                    flo32 dPhi    =  ( PI / 4.0f ) * dT;
                    if( dot( Cow->Direction, V ) < 0.0f ) {
                        dTheta *= 2.0f;
                        dPhi   *= 2.0f;
                        Speed  *= 0.5f;
                    }
                    Cow->AtTheta  = WrapRadians( Cow->AtTheta + dTheta );
                    Cow->AtPhi   += Clamp( -dPhi, GetSmallestDiffInRadians( Cow->AtPhi, Cow->DestPhi ), dPhi );
                    
                    Cow->Direction = Vec3( cosf( Cow->AtPhi ) * cosf( Cow->AtTheta ), sinf( Cow->AtPhi ), -sinf( Cow->AtTheta ) );
                    
                    vec3 Dir   = GetNormal( Cow->Destination - Cow->Position );
                    Dir = Cow->Direction;
                    vec3 Accel = ( Dir * Speed - Cow->Velocity ) * Friction;
                    
                    vec3 dPos = Accel * ( 0.5f * dT * dT ) + Cow->Velocity * dT;
                    
                    Cow->Position += dPos;
                    Cow->Velocity += Accel * dT;
                    
                    Cow->Orientation = Quat( Vec3( 1.0f, 0.0f, 0.0f ), -PI * 0.5f );
                    Cow->Orientation = Quat( Vec3( 0.0f, 1.0f, 0.0f ), Cow->Radians ) * Cow->Orientation;
                    Cow->Orientation = Quat( Vec3( 0.0f, 0.0f, 1.0f ), Cow->AtPhi   ) * Cow->Orientation;
                    Cow->Orientation = Quat( Vec3( 0.0f, 1.0f, 0.0f ), Cow->AtTheta ) * Cow->Orientation;
                    
                    //vec2  MouseP = Mouse->Position;
                    //flo32 tX     = Clamp01( MouseP.x / AppState->App_Dim.x );
                    //Cow->Orientation = Quat( Vec3( 1.0f, 0.0f, 0.0f ), TAU * tX );
                    
                    // NOTE : TEMP!
                    //Cow->Position = Vec3( 0.0f, 0.0f, -3.0f );
                    //Cow->Orientation = Quat( Vec3( 1.0f, 0.0f, 0.0f ), PI * 0.1f ) * Quat( Vec3( 0.0f, 1.0f, 0.0f ), -PI * 0.5f ) * Quat( Vec3( 1.0f, 0.0f, 0.0f ), -PI * 0.5f );
                }
                
                {
                    AppState->World_Bound = AppState->UI_Bound;
                    ZoomCameraToWorldBound( &AppState->Camera, AppState->UI_Bound, AppState->UI_Bound, AppState->World_Bound );
                    
                    vec2 Dim  = GetDim( AppState->World_Bound ) + Vec2( 480.0f, 480.0f );
                    flo32 X   = -Dim.x;
                    vec2 dPos = Vec2( -1.0f, 0.0f ) * ( Dim.x / 180.0f );
                    AppState->TitleMenu_Background_Pos += ( dPos * dT );
                    
                    if( AppState->TitleMenu_Background_Pos.x <= X ) {
                        AppState->TitleMenu_Background_Pos.x += Dim.x;
                    }
                    
                    vec2 Pos = AppState->TitleMenu_Background_Pos + GetDim( AppState->World_Bound ) * 0.5f;
                    rect Bound1 = RectCD( Pos,                       Dim );
                    rect Bound2 = RectCD( Pos + Vec2( Dim.x, 0.0f ), Dim );
                    
                    DrawRect( Pass_Game, TextureID_Background_01, Bound1, COLOR_WHITE );
                    DrawRect( Pass_Game, TextureID_Background_01, Bound2, COLOR_WHITE );
                }
                
                DrawString( Pass_UI, AppState->FontList + FontID_Title_14_00, "Moo.F.O.", GetT( AppState->UI_Bound, Vec2( 0.5f, 0.75f ) ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                
                
                vec2  AtPos    = GetT( AppState->UI_Bound, Vec2( 0.5f, 0.5f ) );
                flo32 AdvanceY = 100.0f;
                
                vec2 Button_Dim   = Vec2( 400.0f, 92.0f );
                
                boo32 GameFileExists = Platform->DoesFileExist( 0, "MyGame", FILETAG__SAVEGAME_FILE );
                
                FONT * Font_Sm = AppState->FontList + FontID_Title_3_50;
                
                if( GameFileExists ) {
                    if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Sm, "Continue", RectCD( AtPos, Button_Dim ) ) ) {
                        AppState->Mode = AppMode_Overworld;
                        AppState->Overworld_Cow.IsInitialized = false;
                        // PlayAudio( AppState, AudioID_Button_Yes, BUTTON_VOLUME );
                    }
                    
                    AtPos.y -= AdvanceY;
                }
                
                if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Sm, "New Game", RectCD( AtPos, Button_Dim ) ) ) {
                    // PlayAudio( AppState, AudioID_Button_Yes, BUTTON_VOLUME );
                    if( GameFileExists ) {
                        AppState->TitleMenu_ShowConfirm = true;
                    } else {
                        StartGame( Platform, AppState, TempMemory );
                    }
                }
                AtPos.y -= AdvanceY;
                
                if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Sm, "How To Play", RectCD( AtPos, Button_Dim ), true, Vec2( 0.0f, -4.0f ) ) ) {
                    AppState->Mode = AppMode_HowToPlay;
                    // PlayAudio( AppState, AudioID_Button_Yes, BUTTON_VOLUME );
                }
                AtPos.y -= AdvanceY;
                
                if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Sm, "Settings", RectCD( AtPos, Button_Dim ), true, Vec2( 0.0f, -4.0f ) ) ) {
                    // PlayAudio( AppState, AudioID_Button_Yes, BUTTON_VOLUME );
                    
                }
                AtPos.y -= AdvanceY;
                
                
                rect ExitButton_Bound = RectBRD( GetBR( AppState->UI_Bound ) + Vec2( -40.0f, 40.0f ), Button_Dim );
                if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Sm, "Exit Game", ExitButton_Bound ) ) {
                    Platform->IsRunning = false;
                }
                
                DrawModel( &Renderer->Pass_Cow_TitleScreen, MeshID_Cow, TextureID_Cow, Cow->Position, Vec3( 1.0f, 1.0f, 1.0f ), Cow->Orientation, COLOR_WHITE );
                DrawModel( &Renderer->Pass_Cow_TitleScreen, MeshID_Spacesuit, TextureID_Spacesuit, Cow->Position, Vec3( 1.0f, 1.0f, 1.0f ), Cow->Orientation, COLOR_WHITE );
            }
        } break;
        
        case AppMode_HowToPlay: {
            {
                flo32 dAlpha = 1.0f / 0.5f;
                AppState->FadeOut_Alpha = Clamp01( AppState->FadeOut_Alpha + dAlpha * dT );
            }
            
            vec4 Outline_Color    = COLOR_GRAY( 0.1f );
            vec4 Background_Color = COLOR_GRAY( 0.05f );
            
            vec4 Text_Color = COLOR_WHITE;
            vec4 Heading_Color = COLOR_GRAY( 0.9f );
            
            vec2  Pic_Dim    = Vec2( 160.0f, 160.0f );
            flo32 Pic_Margin = 10.0f;
            
            vec2  Mouse_xOffset = Vec2( 320.0f, 0.0f );
            
            { // Mouse Controls
                DrawString( Pass_UI, AppState->FontList + FontID_Title_4_00, "CONTROLS", GetT( AppState->UI_Bound, Vec2( 0.5f, 0.9f ) ), TextAlign_Center, Vec2( 1.0f, 1.0f ), Heading_Color );
                
                DrawRect( Pass_UI, TextureID_HowToPlay_Mouse, RectCD( Vec2( 960.0f, 780.0f ) + Mouse_xOffset, Vec2( 155.0f, 220.0f ) ), Heading_Color );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "Fire WEAPON", GetT( AppState->UI_Bound, Vec2( 0.425f, 0.8f ) ) + Mouse_xOffset, TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "Use GADGET", GetT( AppState->UI_Bound, Vec2( 0.575f, 0.8f ) ) + Mouse_xOffset, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                //DrawString( Pass_UI, AppState->FontList + FontID_2_00, "(if available)", GetT( AppState->UI_Bound, Vec2( 0.575f, 0.8f ) ) + Vec2( 80.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_GRAY( 0.8f ) );
                
                {
                    vec2 P = GetT( AppState->UI_Bound, Vec2( 0.425f, 0.8f ) ) + Vec2( 20.0f, -28.0f ) + Mouse_xOffset;
                    vec2 Q = Vec2( 920.0f, 828.0f ) + Mouse_xOffset;
                    
                    orect Bound0 = ORectPP( P, Q, 16.0f );
                    orect Bound1 = ORectPP( P, Q, 8.0f  );
                    DrawORect( Pass_UI, Bound0, COLOR_BLACK );
                    DrawORect( Pass_UI, Bound1, COLOR_WHITE );
                }
                
                {
                    vec2 P = GetT( AppState->UI_Bound, Vec2( 0.575f, 0.8f ) ) + Vec2( -20.0f, -28.0f ) + Mouse_xOffset;
                    vec2 Q = Vec2( 1000.0f, 828.0f ) + Mouse_xOffset;
                    
                    orect Bound0 = ORectPP( P, Q, 16.0f );
                    orect Bound1 = ORectPP( P, Q, 8.0f  );
                    DrawORect( Pass_UI, Bound0, COLOR_BLACK );
                    DrawORect( Pass_UI, Bound1, COLOR_WHITE );
                }
            }
            
            { // Escape Controls
                rect Bound = RectCD( Vec2( 620.0f, 760.0f ), Vec2( 160.0f, 96.0f ) );
                DrawRect( Pass_UI, TextureID_Worm_Tag, Bound, COLOR_WHITE );
                
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "Open MENU", GetT( Bound, Vec2( 0.5f, 1.0f ) ) + Vec2( 0.0f, 8.0f ), TextAlign_BottomCenter, COLOR_WHITE );
                
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "Esc", GetCenter( Bound ), TextAlign_Center, COLOR_BLACK );
            }
            
            flo32 AdvanceY = 108.0f;
            vec2  AtPos    = Vec2( 960, 440.0f );
            
            DrawString( Pass_UI, AppState->FontList + FontID_Title_4_00, "HIGH SCORE TIPS", AtPos, TextAlign_BottomCenter, Vec2( 1.0f, 1.0f ), Heading_Color );
            AtPos.y -= 104.0f;
            
            vec2 Tip1_Pos = AtPos + Vec2( -20.0f, 0.0f );  AtPos.y -= AdvanceY;
            vec2 Tip2_Pos = AtPos + Vec2(  20.0f, 0.0f );  AtPos.y -= AdvanceY;
            vec2 Tip3_Pos = AtPos;                         AtPos.y -= AdvanceY;
            
            rect Tip1_Bound = RectTRD( Tip1_Pos + Vec2( -460.0f, 60.0f ), Pic_Dim );
            rect Tip2_Bound = RectTLD( Tip2_Pos + Vec2(  440.0f, 84.0f ), Pic_Dim * Vec2( 1.5f, 0.75f ) );
            
            DrawRect( Pass_UI,            Tip1_Bound,                Outline_Color    );
            DrawRect( Pass_UI, AddRadius( Tip1_Bound, -Pic_Margin ), Background_Color );
            
            {
                flo32 Width = GetWidth( AppState->FontList + FontID_3_00, "1. Collect MULTIPLIERS for more points." );
                
                flo32 Width0 = GetWidth( AppState->FontList + FontID_3_00, "1. Collect " );
                flo32 Width1 = GetWidth( AppState->FontList + FontID_3_00, "MULTIPLIERS" );
                
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "1. Collect ", Tip1_Pos + Vec2( -Width * 0.5f, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "MULTIPLIERS", Tip1_Pos + Vec2( -Width * 0.5f + Width0, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), SCORE_MULTIPLIER_BASE_COLOR );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, " for more points.", Tip1_Pos + Vec2( -Width * 0.5f + Width0 + Width1, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
            }
            
            DrawRect( Pass_UI,            Tip2_Bound,                Outline_Color    );
            DrawRect( Pass_UI, AddRadius( Tip2_Bound, -Pic_Margin ), Background_Color );
            
            {
                flo32 Width = GetWidth( AppState->FontList + FontID_3_00, "2. Keep your BONUS METER full." );
                
                flo32 Width0 = GetWidth( AppState->FontList + FontID_3_00, "2. Keep your " );
                flo32 Width1 = GetWidth( AppState->FontList + FontID_3_00, "BONUS METER" );
                
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "2. Keep your ", Tip2_Pos + Vec2( -Width * 0.5f, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "BONUS METER", Tip2_Pos + Vec2( -Width * 0.5f + Width0, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_YELLOW );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, " full.", Tip2_Pos + Vec2( -Width * 0.5f + Width0 + Width1, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
            }
            
            {
                flo32 Width = GetWidth( AppState->FontList + FontID_3_00, "3. Destroy the 2X TARGET to double your score." );
                
                flo32 Width0 = GetWidth( AppState->FontList + FontID_3_00, "3. Destroy the " );
                flo32 Width1 = GetWidth( AppState->FontList + FontID_3_00, "2X TARGET" );
                
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "3. Destroy the ", Tip3_Pos + Vec2( -Width * 0.5f, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, "2X TARGET", Tip3_Pos + Vec2( -Width * 0.5f + Width0, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), TowardBlack( MINER_STATIONARY_BASE_COLOR, 0.1f ) );
                DrawString( Pass_UI, AppState->FontList + FontID_3_00, " to double your score.", Tip3_Pos + Vec2( -Width * 0.5f + Width0 + Width1, 0.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
            }
            
            { // SCORE MULTIPLIER
                AppState->HowToPlay_Radians = WrapRadians( AppState->HowToPlay_Radians + SCORE_MULTIPLIER_ROTATE_SPEED * dT );
                SCORE_MULTIPLIER ScoreMultiplier = {};
                ScoreMultiplier.Position = GetCenter( Tip1_Bound );
                ScoreMultiplier.Radians  = AppState->HowToPlay_Radians;
                DrawScoreMultiplier( Pass_UI, ScoreMultiplier, 120.0f );
            }
            
            { // BONUS METER
                rect Bound = Tip2_Bound;
                Bound.Left  += Pic_Margin;
                Bound.Right -= Pic_Margin;
                Bound.Bottom = lerp( Bound.Bottom, 0.35f, Bound.Top );
                Bound.Top   -= Pic_Margin;
                DrawRect( Pass_UI, Bound, COLOR_BLACK );
                
                rect Bar_Bound = RectBRD( GetBR( Bound ) + Vec2( -8.0f, 4.0f ), Vec2( 180.0f, 12.0f ) );
                DrawBarForBonusMultiplier( Pass_UI, Bar_Bound, 2.0f, 0.8f );
                
                DrawString( Pass_UI, AppState->FontList + FontID_2_00, "12x", GetTR( Bar_Bound ) + Vec2( -12.0f, 4.0f ), TextAlign_BottomRight, Vec2( 1.0f, 1.0f ), Vec4( 0.0f, 0.125f, 0.0f, 1.0f ) );
                DrawString( Pass_UI, AppState->FontList + FontID_2_00, "26968", GetTR( Bar_Bound ) + Vec2( -108.0f, 4.0f ), TextAlign_BottomRight, Vec2( 1.0f, 1.0f ), COLOR_GRAY( 0.125f ) );
            }
            
            if( DrawAndUpdateTitleButton( Pass_UI, Mouse, AppState->FontList + FontID_Title_2_00, "Back", RectBRD( GetBR( AppState->UI_Bound ) + Vec2( -20.0f, 20.0f ), Vec2( 200.0f, 80.0f ) ) ) ) {
                AppState->Mode = AppMode_TitleScreen;
                // PlayAudio( AppState, AudioID_Button_No, BUTTON_VOLUME );
            }
        } break;
        
        case AppMode_LevelInfo: {
            { // Draw Background
                vec2  Dim   = GetDim( AppState->UI_Bound ) * 4.0f;
                flo32 X     = -Dim.x;
                vec2  dPos  = Vec2( -1.0f, 0.0f ) * ( Dim.x / ( 180.0f * 4.0f ) );
                AppState->LevelInfo_Background_Pos += ( dPos * dT );
                
                if( AppState->LevelInfo_Background_Pos.x <= X ) {
                    AppState->LevelInfo_Background_Pos.x += Dim.x;
                }
                
                vec2 Pos    = AppState->LevelInfo_Background_Pos + GetDim( AppState->UI_Bound ) * 0.5f;
                rect Bound1 = RectCD( Pos,                       Dim );
                rect Bound2 = RectCD( Pos + Vec2( Dim.x, 0.0f ), Dim );
                
                DrawRect( Pass_UI, TextureID_Background_01, Bound1, COLOR_WHITE );
                DrawRect( Pass_UI, TextureID_Background_01, Bound2, COLOR_WHITE );
            }
            
            char Str[ 128 ] = {};
            
            vec2 MouseP = Mouse->Position;
            
            OVERWORLD_LEVEL Level = AppState->CurrentLevel.Level;
            LEVEL_INFO      Info  = AppState->CurrentLevel.Info;
            
            vec2  Icon_Dim    = Vec2( 200.0f, 200.0f );
            flo32 Icon_Margin = 8.0f;
            
            flo32 OffsetBetween = 40.0f;
            vec2  Label_Offset  = Vec2( 32.0f, 8.0f );
            
            flo32 TotalWidth = Icon_Dim.x * 5.7f + OffsetBetween * 3.0f;
            
            vec2 AtPos = {};
            vec2 Weapon_Pos = Vec2( 960.0f - TotalWidth * 0.5f, 880.0f );
            
            vec4 Outline_Color    = COLOR_GRAY( 0.1f );
            vec4 Background_Color = COLOR_GRAY( 0.05f );
            vec4 Hover_Color      = COLOR_GRAY( 0.3f );
            boo32 DoHover = false;
            
            rect Weapon_Bound = RectTLD( Weapon_Pos, Icon_Dim );
            rect Gadget_Bound = RectTLD( GetTR( Weapon_Bound ) + Vec2( OffsetBetween, 0.0f ), Icon_Dim );
            rect Target_Bound = RectTLD( GetTR( Gadget_Bound ) + Vec2( OffsetBetween, 0.0f ), Icon_Dim * Vec2( 1.5f, 1.2f ) );
            rect Time_Bound   = RectTLD( GetBL( Weapon_Bound ) + Vec2( OffsetBetween, -OffsetBetween ) * 0.5f, Icon_Dim * Vec2( 2.0f, 0.5f ) );
            rect Score_Bound  = RectTLD( GetTR( Target_Bound ) + Vec2( OffsetBetween, 0.0f ), Icon_Dim * Vec2( 2.2f, 1.2f ) );
            rect Info_Bound   = RectBLD( Vec2( 60.0f, 60.0f ), Vec2( 1200.0f, 360.0f ) );
            
            FONT * Info_Font     = AppState->FontList + FontID_2_00;
            flo32  Info_AdvanceY = 40.0f;
            vec2   Info_bPos     = GetTL( Info_Bound ) + Vec2( Icon_Margin, -Icon_Margin ) + Vec2( 20.0f, -20.0f );
            
            DrawRect( Pass_UI,            Info_Bound,          Outline_Color );
            DrawRect( Pass_UI, AddRadius( Info_Bound, -2.0f ), Background_Color );
            
            { // WEAPON
                DrawString( Pass_UI, AppState->FontList + FontID_2_00, "WEAPON", Weapon_Pos + Label_Offset, TextAlign_BottomLeft, COLOR_WHITE );
                
                if( IsInRect( MouseP, Weapon_Bound ) ) {
                    Assert( Info.WeaponID == ItemID_Blaster );
                    
                    DoHover = true;
                    DrawRect( Pass_UI, Weapon_Bound, Hover_Color );
                    
                    AtPos = Info_bPos;
                    
                    DrawString( Pass_UI, Info_Font, "  Blaster", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                    AtPos.y -= Info_AdvanceY;
                    
                    DrawString( Pass_UI, Info_Font, "Hold the LEFT MOUSE BUTTON to rotate faster.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                } else {
                    DrawRect( Pass_UI, Weapon_Bound, Outline_Color );
                }
                
                DrawRect( Pass_UI, AddRadius( Weapon_Bound, -Icon_Margin ), Background_Color );
                if( Info.WeaponID != ItemID_None ) {
                    TEXTURE_ID TextureID = ( TEXTURE_ID )( ( int32 )TextureID_Item_None + ( int32 )Info.WeaponID );
                    DrawRect( Pass_UI, TextureID, AddRadius( Weapon_Bound, -Icon_Margin * 2.0f ), COLOR_WHITE );
                } else {
                    DrawString( Pass_UI, AppState->FontList + FontID_2_00, "NONE", GetCenter( Weapon_Bound ), TextAlign_Center, COLOR_WHITE );
                }
            }
            
            { // GADGET
                DrawString( Pass_UI, AppState->FontList + FontID_2_00, "GADGET", GetTL( Gadget_Bound ) + Label_Offset, TextAlign_BottomLeft, COLOR_WHITE );
                
                if( IsInRect( MouseP, Gadget_Bound ) ) {
                    DoHover = true;
                    DrawRect( Pass_UI, Gadget_Bound, Hover_Color );
                    
                    AtPos = Info_bPos;
                    switch( Info.GadgetID ) {
                        case ItemID_None: {
                            DrawString( Pass_UI, Info_Font, "No gadget for this level.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        } break;
                        
                        case ItemID_GrappleBeam: {
                            DrawString( Pass_UI, Info_Font, "  Grapple Beam", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                            AtPos.y -= Info_AdvanceY;
                            
                            DrawString( Pass_UI, Info_Font, "Hold the RIGHT MOUSE BUTTON to grab a target.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        } break;
                        
                        case ItemID_TractorBeam: {
                            DrawString( Pass_UI, Info_Font, "  Tractor Beam", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                            AtPos.y -= Info_AdvanceY;
                            
                            DrawString( Pass_UI, Info_Font, "Hold the RIGHT MOUSE BUTTON to activate the Tractor Beam.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        } break;
                        
                        case ItemID_GravityWell: {
                            DrawString( Pass_UI, Info_Font, "  Gravity Well", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                            AtPos.y -= Info_AdvanceY;
                            
                            DrawString( Pass_UI, Info_Font, "Hold the RIGHT MOUSE BUTTON to activate the Gravity Well.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        } break;
                        
                        case ItemID_Shield: {
                            DrawString( Pass_UI, Info_Font, "  Shield", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                            AtPos.y -= Info_AdvanceY;
                            
                            DrawString( Pass_UI, Info_Font, "Hold the RIGHT MOUSE BUTTON to activate your Shield.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        } break;
                        
                        case ItemID_Collector: {
                            DrawString( Pass_UI, Info_Font, "  Collector", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                            AtPos.y -= Info_AdvanceY;
                            
                            DrawString( Pass_UI, Info_Font, "Hold the RIGHT MOUSE BUTTON to deactivate your Collector.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                            AtPos.y -= Info_AdvanceY;
                            
                            DrawString( Pass_UI, AppState->FontList + FontID_1_50, "  ...not sure why you'd wanna do that...", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                        } break;
                    }
                } else {
                    DrawRect( Pass_UI, Gadget_Bound, Outline_Color );
                }
                
                DrawRect( Pass_UI, AddRadius( Gadget_Bound, -Icon_Margin ), Background_Color );
                if( Info.GadgetID != ItemID_None ) {
                    TEXTURE_ID TextureID = ( TEXTURE_ID )( ( int32 )TextureID_Item_None + ( int32 )Info.GadgetID );
                    DrawRect( Pass_UI, TextureID, AddRadius( Gadget_Bound, -Icon_Margin * 2.0f ), COLOR_WHITE );
                } else {
                    DrawString( Pass_UI, AppState->FontList + FontID_2_00, "NONE", GetCenter( Gadget_Bound ), TextAlign_Center, COLOR_WHITE );
                }
            }
            
            
            { // 2X TARGET
                DrawString( Pass_UI, AppState->FontList + FontID_2_00, "2X TARGET", GetTL( Target_Bound ) + Label_Offset, TextAlign_BottomLeft, COLOR_WHITE );
                
                if( IsInRect( MouseP, Target_Bound ) ) {
                    DoHover = true;
                    DrawRect( Pass_UI, Target_Bound, Hover_Color );
                    
                    AtPos = Info_bPos;
                    
                    DrawString( Pass_UI, Info_Font, "Destroy the 2X TARGET to double your score and end the level.", AtPos, TextAlign_TopLeft, COLOR_WHITE );
                } else {
                    DrawRect( Pass_UI, Target_Bound, Outline_Color );
                }
                DrawRect( Pass_UI, AddRadius( Target_Bound, -Icon_Margin ), Background_Color );
                
                rect Icon_Bound = AddRadius( Target_Bound, -Icon_Margin );
                DrawEntityIcon( Pass_UI, Info.TargetID, Icon_Bound );
            }
            
            //sprintf( Str, "Target Info: %d", Info.TargetInfo );
            //DrawString( Pass_UI, AppState->FontList + FontID_2_00, Str, GetT( Target_Bound, Vec2( 0.5f, 0.1f ) ), TextAlign_BottomCenter, COLOR_WHITE );
            
            
            { // TIME
                flo32 Time = fabsf( Info.Time );
                int32 Minutes = ( int32 )Time / 60;
                int32 Seconds = ( int32 )Time % 60;
                
                if( IsInRect( MouseP, Time_Bound ) ) {
                    DoHover = true;
                    DrawRect( Pass_UI, Time_Bound, Hover_Color );
                    
                    AtPos = Info_bPos;
                    
                    if( Info.Time == 0.0f ) {
                        strcpy( Str, "The level has no time limit." );
                    } else if( Info.Time < 0.0f ) {
                        sprintf( Str, "The level will end after appr. %d:%02d.", Minutes, Seconds );
                    } else {
                        sprintf( Str, "The level will end after %d:%02d.", Minutes, Seconds );
                    }
                    DrawString( Pass_UI, Info_Font, Str, AtPos, TextAlign_TopLeft, COLOR_WHITE );
                } else {
                    DrawRect( Pass_UI, Time_Bound, Outline_Color );
                }
                
                DrawRect( Pass_UI, AddRadius( Time_Bound, -Icon_Margin ), Background_Color );
                if( Info.Time == 0.0f ) {
                    strcpy( Str, "Time: No Limit" );
                } else if( Info.Time < 0.0f ) {
                    sprintf( Str, "Time: ~%d:%02d", Minutes, Seconds );
                } else {
                    sprintf( Str, "Time: %d:%02d", Minutes, Seconds );
                }
                DrawString( Pass_UI, AppState->FontList + FontID_2_00, Str, GetCenter( Time_Bound ), TextAlign_Center, COLOR_WHITE );
            }
            
            
            { // SCORE RATING
                DrawString( Pass_UI, AppState->FontList + FontID_2_00, "HIGH SCORE", GetTL( Score_Bound ) + Label_Offset, TextAlign_BottomLeft, COLOR_WHITE );
                
                if( IsInRect( MouseP, Score_Bound ) ) {
                    DoHover = true;
                    DrawRect( Pass_UI, Score_Bound, Hover_Color );
                    
                    flo32 AdvanceY = 52.0f;
                    AtPos = GetCenter( Info_Bound ) + Vec2( 0.0f, AdvanceY * 2.0f );
                    
                    flo32 OffsetFromColon = 32.0f;
                    vec2  Rating_Dim    = Vec2( 40.0f, 32.0f );
                    flo32 Rating_Offset = 2.0f;
                    flo32 RatingX = AtPos.x - ( OffsetFromColon + Rating_Dim.x * 2.5f + Rating_Offset * 2.0f );
                    
                    for( int32 iScoreRating = 0; iScoreRating < SCORE_RATING_MAX_RATING; iScoreRating++ ) {
                        DrawScoreThresholdColonScoreRating( Pass_UI, AppState->FontList + FontID_2_25, iScoreRating + 1, Info.ScoreRating_Threshold[ iScoreRating ], AtPos, RatingX, AtPos.x + 200.0f, Rating_Dim, Rating_Offset, COLOR_WHITE );
                        AtPos.y -= AdvanceY;
                    }
                } else {
                    DrawRect( Pass_UI, Score_Bound, Outline_Color );
                }
                DrawRect( Pass_UI, AddRadius( Score_Bound, -Icon_Margin ), Background_Color );
                
                vec2 Rating_Dim = Vec2( 64.0f, 54.0f );
                
                
                DrawScoreRating( Pass_UI, Level.ScoreRating, GetTC( Score_Bound ) + Vec2( 0.0f, -( Icon_Margin + 28.0f + Rating_Dim.y * 0.5f ) ), Rating_Dim, 2.0f );
                
                if( Level.ScoreRating < SCORE_RATING_MAX_RATING ) {
                    flo32 ColonOffset    = 192.0f;
                    flo32 OffsetFromEdge = 20.0f;
                    
                    AtPos = GetLC( Score_Bound ) + Vec2( Icon_Margin + OffsetFromEdge, -12.0f );
                    
                    DrawLabelColonS32( Pass_UI, AppState->FontList + FontID_2_00, "High Score", ( int32 )Level.HighScore, AtPos, AtPos.x + ColonOffset, Score_Bound.Right - Icon_Margin - OffsetFromEdge, COLOR_WHITE );
                    AtPos.y -= 40.0f;
                    
                    DrawLabelColonS32( Pass_UI, AppState->FontList + FontID_2_00, "Next Target", Info.ScoreRating_Threshold[ Level.ScoreRating ], AtPos, AtPos.x + ColonOffset, Score_Bound.Right - Icon_Margin - OffsetFromEdge, COLOR_WHITE );
                } else {
                    AtPos = GetT( Score_Bound, Vec2( 0.5f, 0.25f ) ) + Vec2( 0.0f, 12.0f );
                    //DrawString( Pass_UI, AppState->FontList + FontID_2_00, "High Score", AtPos, TextAlign_TopCenter, COLOR_WHITE );
                    //AtPos.y -= 40.0f;
                    
                    sprintf( Str, "%u", Level.HighScore );
                    DrawString( Pass_UI, AppState->FontList + FontID_3_00, Str, AtPos, TextAlign_Center, COLOR_WHITE );
                }
            }
            
            if( !DoHover ) {
                DrawString( Pass_UI, AppState->FontList + FontID_1_50, "Hover the cursor over any information to see more details.", GetCenter( Info_Bound ), TextAlign_Center, COLOR_WHITE );
            }
            
            
            FONT * Button_Font = AppState->FontList + FontID_Title_3_00;
            vec2   Button_Dim  = Vec2( 320.0f, 100.0f );
            if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Button_Font, "Go Back", RectBRD( GetBR( AppState->UI_Bound ) + Vec2( -160.0f, 128.0f ), Button_Dim ), !AppState->DoTransition ) ) {
                AppState->Mode = AppMode_Overworld;
                AppState->Overworld_Cow.IsInitialized = false;
                // PlayAudio( AppState, AudioID_Button_No, BUTTON_VOLUME );
            }
            if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Button_Font, "Start Level", RectBRD( GetBR( AppState->UI_Bound ) + Vec2( -160.0f, 240.0f ), Button_Dim ), !AppState->DoTransition ) ) {
                AppState->DoTransition       = true;
                AppState->DoTransition_Timer = 0.0f;
            }
            
            if( AppState->DoTransition ) {
                flo32 Transition_TargetTime = ( 60.0f / 60.0f );
                AppState->DoTransition_Timer += dT;
                
                flo32 tVolume = 1.0f - Clamp01( AppState->DoTransition_Timer / Transition_TargetTime );
                SetTVolumeOfCurrentMusicTrack( AppState, tVolume );
                
                flo32 tAlpha  = 1.0f - Clamp01( AppState->DoTransition_Timer / Transition_TargetTime );
                AppState->FadeOut_Alpha = tAlpha;
                
                //flo32 FadeOut_TargetTime = ( 20.0f / 60.0f );
                //if( AppState->DoTransition_Timer >= ( Transition_TargetTime - FadeOut_TargetTime ) ) {
                //AppState->FadeOut_RGB    = Vec3( 0.0f, 0.0f, 0.0f );
                //AppState->FadeOut_dAlpha = ( 1.0f / FadeOut_TargetTime );
                //}
                
                if( AppState->DoTransition_Timer >= Transition_TargetTime ) {
                    StartLevel( Platform, AppState, TempMemory, AppState->CurrentLevel.LevelID );
                    Mouse->ResetPos = true;
                    
                    AppState->DoTransition = false;
                    
                    RemoveCurrentMusicTrack( AppState );
                    
                    int32 Select = ( ( RandomS32InRange( 0, 2 ) * 2 ) + 3 );
                    PlayAudioScript( AppState, ( AUDIO_ID )Select );
                }
            }
        } break;
        
        case AppMode_Game: {
            CAMERA_STATE * Camera = &AppState->Camera;
            
            boo32 DoUpdate   = true;
            boo32 DoFinalize = true;
            
#if ENABLE_DEBUG_REPLAY
            REPLAY_STATE * Replay = &AppState->Replay;
            PANEL        * Panel  = &AppState->Debug;
            if( Replay->Show ) {
                DoUpdate   = false;
                DoFinalize = false;
            }
#endif
            
            
            if( AppState->InGameMenu_ShowMenu ) {
                boo32 DoReplayLevel     = false;
                boo32 DoExitToOverworld = false;
                
                vec2 A = GetCenter( AppState->UI_Bound );
                vec2 B = A + Vec2( 0.0f, -40.0f );
                vec2 C = B + Vec2( 0.0f, -80.0f );
                vec2 D = A + Vec2( 0.0f,  40.0f );
                vec2 E = A + Vec2( 0.0f,  80.0f );
                
                vec2  bPos       = GetCenter( AppState->UI_Bound );
                
                FONT * Font_Big   = AppState->FontList + FontID_2_25;
                FONT * Font_Small = AppState->FontList + FontID_2_00;
                
                if( AppState->InGameMenu_ShowConfirm ) {
                    vec2  Button_Dim = Vec2( 200.0f, 80.0f );
                    flo32 AdvanceY   = Button_Dim.y + 12.0f;
                    
                    DrawString( Pass_UI, Font_Small, "Don't worry! Your progress will", A, TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    DrawString( Pass_UI, Font_Small, "be saved, even if you exit early!", B, TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    DrawString( Pass_UI, Font_Small, "Continue?", C, TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    
                    if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "Yes", RectTRD( C + Vec2( -10.0f, -40.0f ), Button_Dim ) ) ) {
                        switch( AppState->InGameMenu_Mode ) {
                            case InGameMenuMode_RestartLevel: {
                                AppState->EndLevelAction = EndLevelAction_ReplayLevel;
                            } break;
                            
                            case InGameMenuMode_ExitToOverworld: {
                                AppState->EndLevelAction = EndLevelAction_ExitToOverworld;
                            } break;
                            
                            case InGameMenuMode_ExitToDesktop: {
                                AppState->EndLevelAction = EndLevelAction_ExitToDesktop;
                            } break;
                        }
                    }
                    if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "No", RectTLD( C + Vec2(  10.0f, -40.0f ), Button_Dim ) ) ) {
                        AppState->InGameMenu_ShowConfirm = false;
                        AppState->InGameMenu_ShowMenu    = true;
                    }
                } else {
                    vec2 AtPos = GetCenter( AppState->UI_Bound ) + Vec2( 0.0f, 100.0f );
                    
                    vec2  Button_Dim = Vec2( 320.0f, 64.0f );
                    flo32 AdvanceY   = Button_Dim.y + 16.0f;
                    
                    if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "Resume", RectCD( AtPos, Button_Dim ) ) ) {
                        AppState->InGameMenu_ShowMenu    = false;
                        AppState->InGameMenu_ShowConfirm = false;
                        
                        Mouse->ResetPos     = true;
                        Mouse->DoHideCursor = true;
                    }
                    AtPos.y -= AdvanceY;
                    
                    
                    if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "Restart Level", RectCD( AtPos, Button_Dim ) ) ) {
                        AppState->InGameMenu_ShowConfirm = true;
                        AppState->InGameMenu_Mode        = InGameMenuMode_RestartLevel;
                    }
                    AtPos.y -= AdvanceY;
                    
                    
                    if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "Exit To Overworld", RectCD( AtPos, Button_Dim ) ) ) {
                        AppState->InGameMenu_ShowConfirm = true;
                        AppState->InGameMenu_Mode        = InGameMenuMode_ExitToOverworld;
                    }
                    AtPos.y -= ( AdvanceY + 2.0f ); 
                    
                    
                    if( DrawAndUpdateTitleButton( Pass_UI, Mouse, Font_Big, "Exit To Desktop", RectCD( AtPos, Button_Dim ), true, Vec2( 0.0f, -4.0f ) ) ) {
                        AppState->InGameMenu_ShowConfirm = true;
                        AppState->InGameMenu_Mode        = InGameMenuMode_ExitToDesktop;
                    }
                    AtPos.y -= AdvanceY;
                }
                
                if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                    AppState->InGameMenu_ShowMenu    = false;
                    AppState->InGameMenu_ShowConfirm = false;
                    
                    Mouse->ResetPos     = true;
                    Mouse->DoHideCursor = true;
                }
            } else {
                if( DoUpdate ) {
                    UpdateWave( AppState, dT );
                    
                    UpdatePlayerInput    ( AppState, Mouse, dT );
                    
                    UpdatePlayer         ( AppState, dT );
                    UpdateBullet         ( AppState, dT );
                    UpdateBulletCollision   ( AppState );
                    UpdateBomb           ( AppState, dT );
                    UpdateEnemyBullet    ( AppState, dT );
                    UpdateTurret         ( AppState, dT );
                    UpdateSpitter         ( AppState, dT );
                    UpdateChaser         ( AppState, dT );
                    UpdateSwarm          ( AppState, dT );
                    UpdateDrifter        ( AppState, dT );
                    UpdatePowerup        ( AppState, dT );
                    UpdateLooper         ( AppState, dT );
                    UpdateAutoTurret     ( AppState, dT );
                    UpdateBoardRoom     ( AppState, dT );
                    UpdateBayDoors       ( AppState, dT );
                    UpdateMinerAndBomberRepel( AppState );
                    UpdateMiner          ( AppState, dT );
                    UpdateMinerLaserCollision( AppState, dT );
                    //UpdateBomber         ( AppState, dT );
                    UpdateBombLauncher   ( AppState, dT );
                    //UpdateMissileLauncher   ( AppState, dT );
                    UpdateBlimp          ( AppState, dT );
                    UpdateRecon          ( AppState, dT );
                    UpdateSatellite      ( AppState, dT );
                    UpdateGunner         ( AppState, dT );
                    UpdateMissile        ( AppState, dT );
                    UpdateWorm           ( AppState, dT );
                    UpdateFireball       ( AppState, dT );
                    UpdateAsteroid       ( AppState, dT );
                    UpdateScoreMultiplier( AppState, dT );
                    UpdateEntitySpawn    ( AppState, dT );
                    
                    UpdateBonusObjective( AppState, dT );
                    //UpdateProgressMeter ( AppState, dT );
                    
                    {
                        BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
                        SECURITY_DRONE SecurityDrone = BoardRoomS->SecurityDrone;
                        
                        rect Bound_Center = AppState->World_Bound;
                        Bound_Center = AddRadius( Bound_Center, -( SecurityDrone.Radius + SECURITY_DRONE_MOVE_MARGIN ) );
                        
                        rect Bound_Left  = Bound_Center;
                        rect Bound_Right = Bound_Center;
                        
                        flo32 Mid          = ( Bound_Center.Left + Bound_Center.Right ) * 0.5f;
                        flo32 Width        = GetWidth( AppState->World_Bound ) * 0.35f;
                        flo32 HalfWidth    = Width * 0.5f;
                        Bound_Center.Left  = Mid - HalfWidth;
                        Bound_Center.Right = Mid + HalfWidth;
                        Bound_Left.Right   = Mid - HalfWidth;
                        Bound_Right.Left   = Mid + HalfWidth;
                        
                        //DrawRectOutline( Pass_Game, AddRadius( Bound_Center, -0.1f ), COLOR_RED );
                        //DrawRectOutline( Pass_Game, AddRadius( Bound_Left,   -0.1f ), COLOR_BLUE );
                        //DrawRectOutline( Pass_Game, AddRadius( Bound_Right,  -0.1f ), COLOR_GREEN );
                    }
                    
                    CheckIfDamagePlayer( AppState );
                    
                    UpdateScoreParticle( AppState, dT );
                    UpdateParticle     ( AppState, dT );
                    
                    UpdateCameraShake( &AppState->CameraShake, dT );
                    InitCameraForLevel( AppState );
                    
#if ENABLE_DEBUG_REPLAY
                    CaptureReplay( AppState );
#endif
                    
                    UpdateEndLevel( Keyboard, Mouse, AppState, dT );
                }
                
                DRAW_STATE  _Draw = {};
                DRAW_STATE * Draw = &_Draw;
                
                {
                    Draw->Camera           = &AppState->Camera;
                    Draw->ScoreS           = &AppState->ScoreS;
                    Draw->Player           = &AppState->Player;
                    Draw->BulletS          = &AppState->BulletS;
                    Draw->EnemyBulletS     = &AppState->EnemyBulletS;
                    //Draw->ExplosiveS       = &AppState->ExplosiveS;
                    Draw->TurretS          = &AppState->TurretS;
                    Draw->SpitterS         = &AppState->SpitterS;
                    Draw->ChaserS          = &AppState->ChaserS;
                    Draw->SwarmS           = &AppState->SwarmS;
                    //Draw->BumperS          = &AppState->BumperS;
                    Draw->DrifterS         = &AppState->DrifterS;
                    Draw->PowerupS         = &AppState->PowerupS;
                    Draw->LooperS          = &AppState->LooperS;
                    //Draw->StarburstS       = &AppState->StarburstS;
                    Draw->AutoTurretS      = &AppState->AutoTurretS;
                    Draw->BoardRoomS       = &AppState->BoardRoomS;
                    Draw->BayDoorS         = &AppState->BayDoorS;
                    Draw->AsteroidS        = &AppState->AsteroidS;
                    Draw->MinerS           = &AppState->MinerS;
                    Draw->BombS          = &AppState->BombS;
                    Draw->BombLauncherS    = &AppState->BombLauncherS;
                    //Draw->MissileLauncherS = &AppState->MissileLauncherS;
                    //Draw->BankS            = &AppState->BankS;
                    Draw->BlimpS           = &AppState->BlimpS;
                    Draw->ReconS           = &AppState->ReconS;
                    Draw->SatelliteS       = &AppState->SatelliteS;
                    //Draw->SmogS            = &AppState->SmogS;
                    //Draw->EscortS          = &AppState->EscortS;
                    //Draw->BoltS            = &AppState->BoltS;
                    Draw->GunnerS          = &AppState->GunnerS;
                    //Draw->RogueS           = &AppState->RogueS;
                    Draw->MissileS         = &AppState->MissileS;
                    Draw->WormS            = &AppState->WormS;
                    Draw->FireballS        = &AppState->FireballS;
                    Draw->EntitySpawnS     = &AppState->EntitySpawnS;
                    Draw->ScoreMultiplierS = &AppState->ScoreMultiplierS;
                    Draw->ScoreParticleS   = &AppState->ScoreParticleS;
                    Draw->ParticleS        = &AppState->ParticleS;
                    //Draw->ProgressS        = &AppState->ProgressS;
                    Draw->EndLevel         = &AppState->EndLevel;
                }
                
#if ENABLE_DEBUG_REPLAY
                if( Replay->Show ) {
                    UpdateCamera( Camera, Keyboard, dT );
                    
                    if( !Replay->Show_HoldFrame ) {
                        flo32 t = Clamp01( Mouse->Position.x / AppState->App_Dim.x );
                        Replay->Show_AtFrame = ( ( uint32 )( t * ( flo32 )REPLAY_FRAME_MAX_COUNT ) + Replay->AtFrame ) % REPLAY_FRAME_MAX_COUNT;
                        DISPLAY_VALUE( uint32, Replay->Show_AtFrame );
                    }
                    
                    if( WasPressed( Mouse, MouseButton_Right ) ) {
                        Replay->Show_HoldFrame = !Replay->Show_HoldFrame;
                    }
                    
                    REPLAY_FRAME * Frame = Replay->Frame + Replay->Show_AtFrame;
                    Draw->Camera           = &Frame->Camera;
                    Draw->ScoreS           = &Frame->ScoreS;
                    Draw->Player           = &Frame->Player;
                    Draw->BulletS          = &Frame->BulletS;
                    Draw->EnemyBulletS     = &Frame->EnemyBulletS;
                    //Draw->ExplosiveS       = &Frame->ExplosiveS;
                    Draw->TurretS          = &Frame->TurretS;
                    Draw->SpitterS          = &Frame->SpitterS;
                    Draw->ChaserS          = &Frame->ChaserS;
                    Draw->SwarmS           = &Frame->SwarmS;
                    //Draw->BumperS          = &Frame->BumperS;
                    Draw->DrifterS         = &Frame->DrifterS;
                    Draw->PowerupS         = &Frame->PowerupS;
                    Draw->LooperS          = &Frame->LooperS;
                    //Draw->StarburstS       = &Frame->StarburstS;
                    Draw->AutoTurretS      = &Frame->AutoTurretS;
                    Draw->BoardRoomS      = &Frame->BoardRoomS;
                    Draw->BayDoorS         = &Frame->BayDoorS;
                    Draw->AsteroidS        = &Frame->AsteroidS;
                    Draw->MinerS           = &Frame->MinerS;
                    Draw->BombS          = &Frame->BombS;
                    Draw->BombLauncherS    = &Frame->BombLauncherS;
                    //Draw->MissileLauncherS    = &Frame->MissileLauncherS;
                    //Draw->BankS            = &Frame->BankS;
                    Draw->BlimpS           = &Frame->BlimpS;
                    Draw->ReconS           = &Frame->ReconS;
                    Draw->SatelliteS       = &Frame->SatelliteS;
                    //Draw->SmogS            = &Frame->SmogS;
                    //Draw->EscortS          = &Frame->EscortS;
                    //Draw->BoltS            = &Frame->BoltS;
                    Draw->GunnerS          = &Frame->GunnerS;
                    //Draw->RogueS           = &Frame->RogueS;
                    Draw->MissileS         = &Frame->MissileS;
                    Draw->WormS           = &Frame->WormS;
                    Draw->FireballS         = &Frame->FireballS;
                    Draw->EntitySpawnS     = &Frame->EntitySpawnS;
                    Draw->ScoreMultiplierS = &Frame->ScoreMultiplierS;
                    Draw->ScoreParticleS   = &Frame->ScoreParticleS;
                    Draw->ParticleS        = &Frame->ParticleS;
                    //Draw->ProgressS        = &Frame->ProgressS;
                    Draw->EndLevel         = &Frame->EndLevel;
                }
                
                if( WasPressed( Keyboard, KeyCode_d, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                    Replay->Show = !Replay->Show;
                    
                    if( Replay->Show ) {
                        Replay->Show_HoldFrame = true;
                        Replay->Show_AtFrame   = ( Replay->AtFrame + REPLAY_FRAME_MAX_COUNT - 1 ) % REPLAY_FRAME_MAX_COUNT;
                        
                        Panel->Show     = true;
                        Mouse->ResetPos = false;
                        
                        InitCameraForLevel( AppState );
                    } else {
                        Panel->Show     = false;
                        Mouse->ResetPos = true;
                    }
                }
                
                if( ( Replay->Show ) && ( WasPressed( Keyboard, KeyCode_c ) ) ) {
                    InitCameraForLevel( AppState );
                }
                
                if( !Replay->Show ) {
                    InitCameraForLevel( AppState );
                    //Camera->Dim *= 1.5f;
                }
#else
                InitCameraForLevel( AppState );
#endif
                
                //DrawRect( Pass_Game, AppState->World_Bound, COLOR_GRAY( 0.1f ) );
                
                vec3 Light_Direction = GetNormal( Vec3( -0.1f, 0.1f, -1.0f ) );
#if 0
                {
                    PLAYER_STATE * Player = Draw->Player;
                    flo32 tX = GetT01( Player->Position.x, AppState->World_Bound.Left, AppState->World_Bound.Right );
                    flo32 tY = GetT01( Player->Position.y, AppState->World_Bound.Bottom, AppState->World_Bound.Top );
                    
                    flo32 Theta = tX * PI;
                    flo32 Phi   = tY * PI;
                    
                    Light_Direction = GetNormal( Vec3( cosf( Theta ) * cosf( Phi ), sinf( Phi ), -sinf( Theta ) ) );
                }
#endif
                
                { // Draw Background
                    LEVEL_INFO CurrentLevel_Info = AppState->CurrentLevel.Info;
                    
                    rect Bound = AddRadius( AppState->World_Bound, 2.0f );
                    DrawRect( Pass_Game, CurrentLevel_Info.BackgroundID, Bound, COLOR_WHITE );
                }
                
                DrawBayDoorSpawn   ( Pass_Game, Draw, Light_Direction );
                DrawEntitySpawn    ( Pass_Game, Draw );
                DrawRecon          ( Pass_Game, Draw );
                DrawTurret         ( Pass_Game, AppState, Camera, Draw );
                DrawSpitter        ( Pass_Game, AppState, Camera, Draw );
                DrawBoardRoom      ( Pass_Game, AppState, Camera, Draw );
                DrawBayDoors       ( Pass_Game, AppState, Camera, Draw );
                DrawAutoTurret     ( Pass_Game, Draw );
                DrawMiner          ( Pass_Game, AppState, Camera, Draw );
                DrawAsteroid       ( Pass_Game, AppState, Camera, Draw );
                DrawWorm           ( Pass_Game, AppState, Camera, Draw );
                DrawBombLauncher   ( Pass_Game, AppState, Camera, Draw );
                //DrawMissileLauncher( Pass_Game, AppState, Camera, Draw );
                DrawBlimp          ( Pass_Game, AppState, Camera, Draw );
                DrawDrifter        ( Pass_Game, Draw );
                DrawLooper         ( Pass_Game, Draw );
                DrawChaser         ( Pass_Game, Draw );
                DrawGunner         ( Pass_Game, AppState, Camera, Draw );
                
                DrawPlayer         ( Pass_Game, Pass_UI, AppState, Draw );
                DrawMissile        ( Pass_Game, Draw );
                DrawFireball       ( Pass_Game, AppState, Camera, Draw );
                DrawSatellite      ( Pass_Game, AppState, Camera, Draw );
                DrawSwarm          ( Pass_Game, Draw );
                DrawBomb           ( Pass_Game, Draw );
                DrawEnemyBullet    ( Pass_Game, Draw );
                DrawSpit           ( Pass_Game, Draw );
                DrawBullet         ( Pass_Game, Draw );
                DrawPowerup        ( Pass_Game, Draw );
                DrawScoreMultiplier( Pass_Game, Draw );
                DrawParticle       ( Pass_Game, Draw );
                DrawObservationDeckInnerShield( Pass_Game, Draw );
                
                DrawEntitySpawn( Pass_Game, Draw );
                
                DrawPlayerLastDamage( Pass_Game, Draw );
                
                DrawScoreParticle  ( Pass_Game, Draw, AppState );
                
                { // Draw Screen Flash
                    PLAYER_STATE * Player = Draw->Player;
                    if( Player->ScreenFlash_Timer > 0.0f ) {
                        flo32 tAlpha = Clamp01( ( Player->ScreenFlash_Timer + ( 2.0f / 60.0f ) ) / Player->ScreenFlash_TargetTime );
                        
                        TEXTURE_ID TextureID = TextureID_WhiteTexture;
                        rect       Bound     = AddRadius( AppState->World_Bound, 2.0f );
                        vec4       Color     = COLOR_WHITE;
                        
                        switch( Player->ScreenFlash_Type ) {
                            case ScreenFlashType_Powerup_DamageAll: {
                                Color = COLOR_WHITE * 0.35f;
                            } break;
                            
                            case ScreenFlashType_Player_Damage: {
                                Color = COLOR_RED * 0.5f;
                            } break;
                            
                            case ScreenFlashType_Player_SecondaryDamage: {
                                flo32 Radius = 20.0f;
                                
                                Bound     = RectCR( Player->Position, Radius );
                                TextureID = TextureID_Glow;
                                Color     = COLOR_RED * 0.35f;
                            } break;
                        }
                        
                        DrawRect( Pass_Game, TextureID, Bound, Color * tAlpha );
                    }
                }
                
#if ( ( ENABLE_DEBUG_REPLAY ) || ( DEBUG_BUILD ) )
                DrawRectOutline    ( Pass_Game, AppState->World_Bound, COLOR_RED );
#endif
                //DrawRectOutline    ( Pass_Game, AddRadius( AppState->World_Bound, -6.0f ), COLOR_RED );
                
                DrawEndLevel( Pass_UI, AppState, Draw );
                DrawUI      ( Pass_UI, AppState, Draw );
                
                if( DoFinalize ) {
                    FinalizeBullet         ( AppState );
                    FinalizeTurret         ( AppState );
                    FinalizeSpitter        ( AppState );
                    FinalizeChaser         ( AppState );
                    FinalizeSwarm          ( AppState );
                    FinalizeDrifter        ( AppState );
                    FinalizePowerup        ( AppState );
                    FinalizeLooper         ( AppState );
                    FinalizeAutoTurret     ( AppState );
                    FinalizeBoardRoom      ( AppState );
                    FinalizeBayDoors       ( AppState );
                    FinalizeAsteroid       ( AppState );
                    FinalizeMiner          ( AppState );
                    FinalizeBomb           ( AppState );
                    FinalizeBombLauncher   ( AppState );
                    //FinalizeMissileLauncher( AppState );
                    FinalizeBlimp          ( AppState );
                    FinalizeRecon          ( AppState );
                    FinalizeSatellite      ( AppState );
                    FinalizeGunner         ( AppState );
                    FinalizeMissile        ( AppState );
                    FinalizeWorm           ( AppState );
                    FinalizeFireball       ( AppState );
                    FinalizeEnemyBullet    ( AppState );
                    FinalizeEntitySpawn    ( AppState );
                    FinalizeScoreMultiplier( AppState );
                    
                    FinalizeScoreParticle  ( AppState );
                    FinalizeParticle       ( AppState );
                    
                    FinalizePlayer         ( AppState );
                    FinalizeWave           ( AppState, TempMemory );
                    
                    FinalizeScore          ( AppState, dT );
                    FinalizeEndLevel       ( AppState );
                }
                
#if ENABLE_DEBUG_REPLAY
                UpdateAndDrawDebugPanel( AppState, Mouse, Pass_Game, Pass_UI, Draw );
                if( ( Replay->Show ) && ( WasPressed( Keyboard, KeyCode_h, KEYBOARD_FLAGS__CONTROL ) ) ) {
                    Panel->Show = !Panel->Show;
                }
#endif
                
#if ENABLE_EDITOR
                if( WasPressed( Keyboard, KeyCode_e, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                    AppState->Mode  = AppMode_Editor;
                    Mouse->ResetPos = false;
                }
#endif
                
                END_LEVEL * EndLevel = &AppState->EndLevel;
                if( ( WasPressed( Keyboard, KeyCode_Escape ) ) && ( !EndLevel->Show ) ) {
                    AppState->InGameMenu_ShowMenu = true;
                    
                    Mouse->ResetPos     = false;
                    Mouse->DoShowCursor = true;
                }
            }
            
            if( AppState->EndLevelAction != EndLevelAction_Null ) {
                SCORE_STATE     * ScoreS     = &AppState->ScoreS;
                OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
                OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
                OVERWORLD_LEVEL * Level      = OverworldS->Level + OverworldS->AtLevel;
                OVERWORLD_MAP   * Map        = OverworldT->Map   + OverworldS->AtLevel;
                END_LEVEL       * EndLevel   = &AppState->EndLevel;
                
                if( AppState->EndLevelAction == EndLevelAction_ExitToOverworld ) {
                    RemoveCurrentMusicTrack( AppState );
                }
                
                flo32 Score = ScoreS->Score;
                if( ScoreS->DoBonusObjective ) {
                    Score *= 2.0f;
                }
                
                boo32 ScoreRating_WasFound = false;
                int32 ScoreRating          = 0;
                for( int32 iScoreRating = 0; ( !ScoreRating_WasFound ) && ( iScoreRating < SCORE_RATING_MAX_RATING ); iScoreRating++ ) {
                    if( ( int32 )Score < Map->Info.ScoreRating_Threshold[ iScoreRating ] ) {
                        ScoreRating          = iScoreRating;
                        ScoreRating_WasFound = true;
                    }
                }
                if( !ScoreRating_WasFound ) {
                    Assert( ( int32 )Score >= Map->Info.ScoreRating_Threshold[ SCORE_RATING_MAX_RATING - 1 ] );
                    ScoreRating = SCORE_RATING_MAX_RATING;
                }
                
                Level->HighScore      = MaxValue( Level->HighScore,      ( uint32 )Score              );
                Level->HighMultiplier = MaxValue( Level->HighMultiplier, ( uint32 )ScoreS->Multiplier );
                Level->ScoreRating    = MaxValue( Level->ScoreRating,              ScoreRating );
                if( ScoreS->DoBonusObjective ) {
                    Level->BonusObjectiveWasAcquired = true;
                }
                
#if 0                    
                MARKET_STATE_SAVE_DATA * MarketS = &AppState->MarketS.SaveData;
                //MarketS->nPoints_Positive += ( uint64 )( ScoreS->Score_Positive );
                //MarketS->nPoints_Negative += ( uint64 )( ScoreS->Score_Negative );
                MarketS->TotalPoints += ( uint64 )( ScoreS->Score_Positive );
                MarketS->TotalPoints += ( uint64 )( ScoreS->Score_Negative );
                
                for( uint32 iType = 0; iType < EntityType_Count; iType++ ) {
                    STOCK_ENTRY *           Stock           = MarketS->Stock + iType;
                    SCORE_ENTITY_KILL_COUNT EntityKillCount = ScoreS->EntityKillCount[ iType ];
                    
                    if( ( Stock->Flags & STOCK_ENTRY_FLAG__IS_AVAILABLE ) && ( EntityKillCount.nTotal > 0 ) ) {
                        flo32 PriceChange = ( Stock->PriceChange_StockRate * ( ( flo32 )( EntityKillCount.nKilled - EntityKillCount.nNegative ) / Stock->PriceChange_EntityRate ) );
                        
                        Stock->Price = ( uint32 )MaxValue( ( int32 )( ( flo32 )Stock->Price + PriceChange + 0.5f ), 1 );
                    }
                }
#endif
                
                if( EndLevel->ShowLevelComplete ) {
                    Level->IsComplete     = true;
                    for( int32 iLevelToUnlock = 0; iLevelToUnlock < Map->nLevelToUnlock; iLevelToUnlock++ ) {
                        OVERWORLD_LEVEL * LevelToUnlock = OverworldS->Level + Map->LevelToUnlock[ iLevelToUnlock ];
                        LevelToUnlock->IsAvailable = true;
                    }
                }
                
                flo32 AvgBonus      = 0.0f;
                if( ScoreS->BonusMultiplier_TotalTime > 0.0f ) {
                    AvgBonus = ScoreS->BonusMultiplier_Total / ScoreS->BonusMultiplier_TotalTime;
                }
                Level->HighAvgBonus = MaxValue( Level->HighAvgBonus, AvgBonus );
                
                if( ScoreS->BonusMultiplier_FullTime > Level->HighAvgBonus_FullTime ) {
                    Level->HighAvgBonus_FullTime  = ScoreS->BonusMultiplier_FullTime;
                    Level->HighAvgBonus_TotalTime = ScoreS->BonusMultiplier_TotalTime;
                }
                
                {
                    AppState->SaveGame.PowerupSave.TotalPoints += ( uint64 )ScoreS->Score;
                }
                
                SaveGame( Platform, AppState, TempMemory );
            }
            
            if( AppState->EndLevelAction == EndLevelAction_ReplayLevel ) {
                AppState->EndLevelAction = EndLevelAction_Null;
                
                StartLevel( Platform, AppState, TempMemory, AppState->CurrentLevel.LevelID );
                Mouse->ResetPos = true;
            }
            if( AppState->EndLevelAction == EndLevelAction_ExitToOverworld ) {
                AppState->EndLevelAction = EndLevelAction_Null;
                
                AppState->Mode = AppMode_Overworld;
                ZoomCameraToWorldBound( Camera, AppState->UI_Bound, AppState->UI_Bound, AppState->UI_Bound );
                
                Mouse->ResetPos = false;
                // TODO: Mouse Pos doesn't need to be reset to the center, but do we want to set the cursor pos to the position of the player avatar in the overworld? It's not necessary now, but think about this later.
                AppState->Overworld_Cow.IsInitialized = false;
                
                RemoveCurrentMusicTrack( AppState );
                AppState->FadeOut_Alpha = 0.0f;
            }
            if( AppState->EndLevelAction == EndLevelAction_ExitToDesktop ) {
                AppState->EndLevelAction = EndLevelAction_Null;
                Platform->IsRunning      = false;
                AppState->FadeOut_Alpha  = 0.0f;
            }
            
            if( AppState->Mode != AppMode_Game ) {
                Mouse->DoShowCursor             = true;
            }
        } break;
        
        case AppMode_Overworld: {
            {
                flo32 dAlpha = 1.0f / 0.5f;
                AppState->FadeOut_Alpha = Clamp01( AppState->FadeOut_Alpha + dAlpha * dT );
            }
            
            OVERWORLD_STATE_SAVE_DATA * OverworldS = &AppState->SaveGame.OverworldSave;
            OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
            CAMERA_STATE    * Camera     = &AppState->Camera;
            
            TITLE_MENU_COW * Cow = &AppState->Overworld_Cow;
            
            {
                {
                    OVERWORLD_MAP * Map = OverworldT->Map + OverworldS->AtLevel;
                    OverworldT->Player_Position = GetCenter( Map->Bound );
                }
                
                if( !Cow->IsInitialized ) {
                    InitCowForOverworld( AppState );
                }
            }
            
            {
                flo32 dRadians = ( TAU / 120.0f );
                OverworldT->GravityWell_Radians = WrapRadians( OverworldT->GravityWell_Radians + dRadians * dT );
            }
            
            {
                int32 ScoreRating = 0;
                for( int32 iLevel = 1; iLevel < OverworldS->nLevel; iLevel++ ) {
                    OVERWORLD_LEVEL Level = OverworldS->Level[ iLevel ];
                    ScoreRating += Level.ScoreRating;
                }
                int32 MaxScoreRating = ( OverworldS->nLevel - 1 ) * 3;
                flo32 tGravityWell = Clamp01( ( flo32 )ScoreRating / ( flo32 )MaxScoreRating );
                
                for( int32 iParticle = 0; iParticle < OVERWORLD_GRAVITY_WELL_PARTICLE_MAX_COUNT; iParticle++ ) {
                    GRAVITY_WELL_PARTICLE * Particle = OverworldT->GravityWell_Particle + iParticle;
                    
                    if( Particle->Timer < Particle->TargetTime ) {
                        Particle->Speed = ( PLAYER_ROTATE_SPEED_SLOW * 2.0f * Particle->tSpeed ) / sqrtf( Particle->Dist );
                        
                        flo32 Radius = OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_OUTER * 0.5f;
                        flo32 tDist = Clamp01( Particle->Dist / OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_OUTER );
                        tDist = tDist * tDist * tDist;
                        
                        //Speed = lerp( PLAYER_ROTATE_SPEED_SLOW, tDist, PLAYER_ROTATE_SPEED_SLOW * 0.1f );
                        //Speed = PLAYER_ROTATE_SPEED_SLOW * 2.0f;
                        
                        Particle->Radians  = WrapRadians( Particle->Radians - ( Particle->Speed * dT ) );
                        //Particle->Radians  = WrapRadians( Particle->Radians - ( Particle->Speed * dT / ( tDist * 5.0f ) ) );
                        Particle->Dist    -= ( Particle->Dist * 0.05f * dT );
                        
                        Particle->Timer += dT;
                    } else {
                        flo32 TargetTime_Lo = lerp( 1.0f, tGravityWell, 0.2f );
                        flo32 TargetTime_Hi = lerp( 5.0f, tGravityWell, 0.5f );
                        
                        Particle->Speed      = PLAYER_ROTATE_SPEED_SLOW;
                        Particle->TargetTime = RandomF32InRange( TargetTime_Lo, TargetTime_Hi );
                        Particle->Timer      = 0.0f;
                        Particle->Radians    = RandomF32() * TAU;
                        Particle->Dist       = RandomF32InRange( OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_INNER, OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_OUTER );
                        Particle->tSpeed     = RandomF32InRange( 0.75f, 1.25f );
                        Particle->iColor     = ( uint8 )RandomU32InRange( 0, 3 );
                    }
                }
            }
            
            { // Update Cow
                Cow->Radians = WrapRadians( Cow->Radians - PLAYER_ROTATE_SPEED_SLOW * dT * 0.5f );
                
                OVERWORLD_MAP * Map = OverworldT->Map + OverworldS->AtLevel;
                DISPLAY_VALUE( vec2, GetCenter( Map->Bound ) );
                DISPLAY_VALUE( vec3, Cow->Position );
                
                flo32 TargetTime = ( 180.0f / 60.0f );
                Cow->Timer += dT;
                if( Cow->Timer >= TargetTime ) {
                    Cow->Timer -= TargetTime;
                    
                    vec2 HalfDim = ( GetDim( Map->Bound ) - Vec2( 32.0f, 32.0f ) ) * 0.5f;
                    vec2 Pos     = Vec2( RandomF32InRange( -1.0f, 1.0f ), RandomF32InRange( -1.0f, 1.0f ) ) * HalfDim;
                    Cow->Destination = Vec3( Pos, 0.0f );
                }
                
                flo32 Speed    = COW_SPEED_FOR_OVERWORLD_MAP;
                flo32 Friction =  0.5f;
                
                vec3 V = Cow->Destination - Cow->Position;
                Cow->DestPhi   = ToRadians( Vec2( fabsf( V.x ),  V.y ) );
                
                flo32 dTheta  = -( PI / 4.0f ) * dT;
                flo32 dPhi    =  ( PI / 4.0f ) * dT;
                if( dot( Cow->Direction, V ) < 0.0f ) {
                    dTheta *= 2.0f;
                    dPhi   *= 2.0f;
                    Speed  *= 0.5f;
                }
                Cow->AtTheta  = WrapRadians( Cow->AtTheta + dTheta );
                Cow->AtPhi   += Clamp( -dPhi, GetSmallestDiffInRadians( Cow->AtPhi, Cow->DestPhi ), dPhi );
                
                Cow->Direction = Vec3( cosf( Cow->AtPhi ) * cosf( Cow->AtTheta ), sinf( Cow->AtPhi ), -sinf( Cow->AtTheta ) );
                
                vec3 Dir   = GetNormal( Cow->Destination - Cow->Position );
                Dir = Cow->Direction;
                vec3 Accel = ( Dir * Speed - Cow->Velocity ) * Friction;
                
                vec3 dPos = Accel * ( 0.5f * dT * dT ) + Cow->Velocity * dT;
                dPos.z = 0.0f;
                
                Cow->Position += dPos;
                Cow->Velocity += Accel * dT;
                
                Cow->Orientation = Quat( Vec3( 1.0f, 0.0f, 0.0f ), -PI * 0.5f );
                Cow->Orientation = Quat( Vec3( 0.0f, 1.0f, 0.0f ), Cow->Radians ) * Cow->Orientation;
                Cow->Orientation = Quat( Vec3( 0.0f, 0.0f, 1.0f ), Cow->AtPhi   ) * Cow->Orientation;
                Cow->Orientation = Quat( Vec3( 0.0f, 1.0f, 0.0f ), Cow->AtTheta ) * Cow->Orientation;
                
                //vec2  MouseP = Mouse->Position;
                //flo32 tX     = Clamp01( MouseP.x / AppState->App_Dim.x );
                //Cow->Position = Vec3( 0.0f, 0.0f, -2.0f );
                //Cow->Orientation = Quat( Vec3( 1.0f, 0.0f, 0.0f ), TAU * tX );
            }
            
            int32 iHover = -1;
            vec2  MouseP = GetWorldPos( Camera, AppState->UI_Dim, Mouse->Position );
            
            OverworldT->Player_Radians = WrapRadians( OverworldT->Player_Radians - ( TAU / 1.5f ) * dT );
            
            boo32 DoLoadLevel = false;
            
            if( OverworldT->Player_DoTravel ) {
                OverworldT->Player_DoTravel_Timer += dT;
                if( OverworldT->Player_DoTravel_Timer >= OverworldT->Player_DoTravel_TargetTime ) {
                    OverworldT->Player_DoTravel       = false;
                    OverworldT->Player_DoTravel_Timer = 0.0f;
                    OverworldS->AtLevel               = OverworldT->ToLevel;
                    
                    DoLoadLevel = true;
                }
                
                OVERWORLD_MAP MapA = OverworldT->Map[ OverworldS->AtLevel ];
                OVERWORLD_MAP MapB = OverworldT->Map[ OverworldT->ToLevel ];
                
                vec2 P = GetCenter( MapA.Bound );
                vec2 Q = GetCenter( MapB.Bound );
                
                flo32 t = Clamp01( OverworldT->Player_DoTravel_Timer / OverworldT->Player_DoTravel_TargetTime );
                
                vec2  bPos = lerp( P, t, Q );
                flo32 Offset  = sinf( t * PI ) * 8.0f;
                flo32 Radians = t * TAU * 2.75f;
                vec2  Dir     = ToDirection2D( Radians );
                
                OverworldT->Player_Position = bPos + Dir * Offset;
                OverworldT->Player_Position = bPos;
            } else {
                for( int32 iLevel = 0; iLevel < OverworldS->nLevel; iLevel++ ) {
                    OVERWORLD_LEVEL Level = OverworldS->Level[ iLevel ];
                    OVERWORLD_MAP   Map   = OverworldT->Map  [ iLevel ];
                    
                    if( ( Level.IsAvailable ) && ( IsInRect( MouseP, Map.Bound ) ) ) {
                        iHover = ( int32 )iLevel;
                    }
                }
                
                if( ( iHover > -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                    OverworldT->ToLevel = ( LEVEL_ID )iHover;
                    if( OverworldS->AtLevel == OverworldT->ToLevel ) {
                        DoLoadLevel = true;
                    } else {
                        OVERWORLD_MAP MapA = OverworldT->Map[ OverworldS->AtLevel ];
                        OVERWORLD_MAP MapB = OverworldT->Map[ OverworldT->ToLevel ];
                        
                        vec2  P    = GetCenter( MapA.Bound );
                        vec2  Q    = GetCenter( MapB.Bound );
                        flo32 Dist = GetLength( P - Q );
                        
                        flo32 MaxDist = 1200.0f;
                        flo32 MaxTime = ( 160.0f / 60.0f );
                        flo32 MinTime = (  60.0f / 60.0f );
                        
                        flo32 tDist      = Clamp01( Dist / MaxDist );
                        flo32 TargetTime = MaxValue( tDist * MaxTime, MinTime );
                        
                        OverworldT->Player_DoTravel            = true;
                        OverworldT->Player_DoTravel_TargetTime = TargetTime;
                    }
                }
            }
            
            if( ( iHover > -1 ) && ( iHover != LevelID_Market ) ) {
                OVERWORLD_LEVEL Level = OverworldS->Level[ iHover ];
                
                flo32 Panel_Margin = 8.0f;
                vec4  Panel_Background_Color = COLOR_GRAY( 0.3f );
                vec4  Panel_Outline_Color    = COLOR_GRAY( 0.1f );
                
                vec2 UI_Dim     = AppState->UI_Dim;
                vec2 Panel_bPos  = GetTR( AppState->UI_Bound ) - Vec2( 80.0f, 120.0f );
                vec2 Panel_Dim   = Vec2( 500.0f, 200.0f );
                rect Panel_Bound = RectTRD( Panel_bPos, Panel_Dim );
                
                DrawRect( Pass_UI, Panel_Bound, Panel_Background_Color );
                DrawRect( Pass_UI, AddRadius( Panel_Bound, -Panel_Margin ), Panel_Outline_Color );
                
                flo32 LabelX   = Panel_Bound.Left  + Panel_Margin + 24.0f;
                flo32 ValueX   = Panel_Bound.Right - Panel_Margin - 80.0f;
                flo32 ColonX   = lerp( Panel_Bound.Left, 0.49f, Panel_Bound.Right );
                flo32 AdvanceY = 40.0f;
                
                vec2 AtPos = Vec2( ColonX, Panel_Bound.Top - Panel_Margin - 16.0f );
                
                char Str[ 64 ] = {};
                sprintf( Str, "%u", Level.HighScore );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, ":", AtPos, TextAlign_TopCenter, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, "High Score", Vec2( LabelX, AtPos.y ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, Str, Vec2( ValueX, AtPos.y ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                
                AtPos.y -= AdvanceY;
                
                sprintf( Str, "%u", Level.HighMultiplier );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, ":", AtPos, TextAlign_TopCenter, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, "High Multiplier", Vec2( LabelX, AtPos.y ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, Str, Vec2( ValueX, AtPos.y ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                
                AtPos.y -= AdvanceY;
                
                sprintf( Str, "%.02f", Level.HighAvgBonus );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, ":", AtPos, TextAlign_TopCenter, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, "High Avg. Bonus", Vec2( LabelX, AtPos.y ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                DrawString( Pass_UI, AppState->FontList + FontID_1_75, Str, AtPos + Vec2( 92.0f, 0.0f ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                
                rect  Bonus_Bound   = RectTLD( AtPos + Vec2( 112.0f, 0.0f ), Vec2( 0.0f, 24.0f ) );
                Bonus_Bound.Right = Panel_Bound.Right - Panel_Margin - 24.0f;
                flo32 Bonus_Percent = Clamp01( Level.HighAvgBonus / BONUS_MULTIPLIER_MAX_VALUE );
                DrawBarForBonusMultiplier( Pass_UI, Bonus_Bound, 2.0f, Bonus_Percent );
                
                AtPos.y -= ( AdvanceY + 4.0f );
                AtPos.x  = Panel_Bound.Left + Panel_Margin + 36.0f;
                
                int32 Minutes = ( ( int32 )Level.HighAvgBonus_FullTime / 60 );
                int32 Seconds = ( ( int32 )Level.HighAvgBonus_FullTime % 60 );
                flo32 Percent = 0.0f;
                if( Level.HighAvgBonus_TotalTime > 0.0f ) {
                    Percent = Level.HighAvgBonus_FullTime / Level.HighAvgBonus_TotalTime * 100.0f;
                }
                
                sprintf( Str, "Time at Full Bonus: %2d:%02d  (%.02f%%)", Minutes, Seconds, Percent );
                DrawString( Pass_UI, AppState->FontList + FontID_1_50, Str, AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_GRAY( 0.9f ) );
            }
            
            { // DRAW
                for( int32 iLevel = 0; iLevel < OverworldS->nLevel; iLevel++ ) {
                    OVERWORLD_LEVEL Level = OverworldS->Level[ iLevel ];
                    OVERWORLD_MAP   Map   = OverworldT->Map  [ iLevel ];
                    
                    vec4 OutlineColor = Vec4( 0.125f, 0.125f, 0.125f, 1.0f );
                    if( Level.IsAvailable ) {
                        OutlineColor = Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
                    }
                    if( OverworldT->Player_DoTravel ) {
                        if( OverworldT->ToLevel == iLevel ) {
                            OutlineColor = COLOR_GRAY( 0.35f );
                        }
                    } else {
                        if( OverworldS->AtLevel == iLevel ) {
                            OutlineColor = COLOR_GRAY( 0.5f );
                        }
                        if( iHover == ( int32 )iLevel ) {
                            OutlineColor = COLOR_YELLOW;
                        }
                    }
                    
                    flo32 Margin = 8.0f;
                    
                    if( Level.IsComplete ) {
                        DrawRect( Pass_Game, AddRadius( Map.Bound, Margin ), Vec4( 0.15f, 0.125f, 0.125f, 1.0f ) );
                    }
                    
                    if( Level.IsAvailable ) {
                        vec4 BackgroundColor = COLOR_GRAY( 0.0375f );
                        
                        vec4 TextColor = COLOR_GRAY( 0.8f );
                        if( OverworldT->Player_DoTravel ) {
                            if( OverworldT->ToLevel == iLevel ) {
                                BackgroundColor = COLOR_GRAY( 0.125f );
                            }
                        } else {
                            if( OverworldS->AtLevel == iLevel ) {
                                BackgroundColor = COLOR_GRAY( 0.15f );
                            }
                            if( iHover == ( int32 )iLevel ) {
                                BackgroundColor = COLOR_GRAY( 0.1f );
                                TextColor       = COLOR_YELLOW;
                            }
                        }
                        
                        DrawRect( Pass_Game, Map.Bound, BackgroundColor );
                        vec2 Pos = GetT( Map.Bound, Vec2( 0.5f, 0.65f ) );
                        if( iLevel == LevelID_Market ) {
                            Pos = GetT( Map.Bound, Vec2( 0.5f, 0.5f ) );
                        }
                        DrawString( Pass_Game, AppState->FontList + FontID_Title_2_50, Map.DisplayName, Pos, TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), TextColor );
                    }
                    
                    DrawRectOutline( Pass_Game,            Map.Bound,           OutlineColor );
                    if( ( Level.IsAvailable ) && ( iLevel != 0 ) ) {
                        DrawRectOutline( Pass_Game, AddRadius( Map.Bound, Margin ), OutlineColor );
                    }
                    
                    { // Draw Score Rating
                        if( ( Level.IsAvailable ) && ( Map.ShowRating ) ) {
                            int32 Rating    = Level.ScoreRating;
                            int32 MaxRating = 5;
                            
                            flo32 Sca     = GetHeight( Map.Bound ) * 0.1f;
                            vec2  Scale   = Vec2( Sca * 1.2f, Sca );
                            flo32 OffsetX = ( Scale.x * 2.1f );
                            
                            vec4 ColorA = COLOR_BLACK;
                            vec4 ColorB = COLOR_GRAY( 0.75f );
                            
                            vec2  AtPos   = GetT( Map.Bound, Vec2( 0.5f, 0.25f ) );
                            AtPos.x -= ( OffsetX * 2.0f );
                            
                            //if( Level.BonusObjectiveWasAcquired ) {
                            //AtPos.x -= ( OffsetX * 0.5f );
                            //}
                            
                            for( int32 iRating = 0; iRating < MaxRating; iRating++ ) {
                                if( iRating <= ( Rating - 1 ) ) {
                                    DrawTriangle( Pass_Game, AtPos, Scale, 0.0f, ColorB );
                                } else {
                                    DrawTriangle( Pass_Game, AtPos, Scale, 0.0f, ColorA );
                                    DrawTriangleOutline( Pass_Game, AtPos, Scale, 0.0f, ColorB );
                                }
                                
                                AtPos.x += OffsetX;
                            }
                            
                            if( Level.IsComplete ) {
                                vec2 Pos   = GetT( Map.Bound, Vec2( 0.225f, 0.65f ) );
                                rect Bound = RectCR( Pos, GetHeight( Map.Bound ) * 0.2f );
                                DrawRect( Pass_Game, TextureID_Icon_LevelComplete, Bound, COLOR_GRAY( 0.75f ) );
                            }
                            
                            if( Level.BonusObjectiveWasAcquired ) {
                                vec4 ColorC = Vec4( 1.0f, 1.0f, 0.25f, 1.0f );
                                vec2 Pos    = GetT( Map.Bound, Vec2( 0.8f, 0.65f ) );
                                DrawString( Pass_Game, AppState->FontList + FontID_Title_2_00, "2X", WorldToUIPos( AppState, Camera, Pos ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), ColorC * 0.5f );
                            }
                        }
                    }
                }
                
#if 0                
                { // PLAYER
                    flo32 Radius = 7.0f;
                    DrawCircle( Pass_Game, OverworldT->Player_Position, Radius, ToColor( 0, 150, 0 ) );
                    
                    vec2  Gun_Dir   = ToDirection2D( OverworldT->Player_Radians );
                    vec2  Gun_Dim   = Vec2( Radius * 0.3f, Radius );
                    orect Gun_Bound = ORectBCD( OverworldT->Player_Position + Gun_Dir * Radius * 0.3f, Gun_Dim, OverworldT->Player_Radians );
                    
                    DrawORect( Pass_Game, Gun_Bound, COLOR_RED );
                }
#endif
                
                { // Draw Gravity Well
                    vec2 GravityWell_Offset = ToDirection2D( OverworldT->GravityWell_Radians ) * 250.0f;
                    GravityWell_Offset.y *= 0.6f;
                    
                    vec2  Center = Vec2( 920.0f, 600.0f ) + GravityWell_Offset;
                    
                    int32 ScoreRating = 0;
                    for( int32 iLevel = 1; iLevel < OverworldS->nLevel; iLevel++ ) {
                        OVERWORLD_LEVEL Level = OverworldS->Level[ iLevel ];
                        ScoreRating += Level.ScoreRating;
                    }
                    int32 MaxScoreRating = ( OverworldS->nLevel - 1 ) * 3;
                    
                    flo32 tGravityWell = Clamp01( 0.1f + ( ( flo32 )ScoreRating / ( flo32 )MaxScoreRating ) );
                    flo32 tAlpha       = lerp( 0.5f, tGravityWell, 1.0f );
                    
                    int32 nGravityWell_Particle = ( int32 )( ( flo32 )OVERWORLD_GRAVITY_WELL_PARTICLE_MAX_COUNT * tGravityWell );
                    for( int32 iParticle = 0; iParticle < nGravityWell_Particle; iParticle++ ) {
                        GRAVITY_WELL_PARTICLE Particle = OverworldT->GravityWell_Particle[ iParticle ];
                        
                        vec2 Offset = ToDirection2D( Particle.Radians ) * Particle.Dist;
                        Offset.y *= 0.5f;
                        
                        flo32 tCover = 1.0f;
                        
                        vec2 Position = Center + Offset;
                        for( int32 iLevel = 0; iLevel < OverworldS->nLevel; iLevel++ ) {
                            OVERWORLD_MAP Map = OverworldT->Map[ iLevel ];
                            flo32 yMax = lerp( Map.Bound.Bottom, 0.35f, Map.Bound.Top );
                            flo32 yMin = lerp( Map.Bound.Bottom, 0.0f, Map.Bound.Top );
                            
                            if( IsInRect( Position, Map.Bound ) ) {
                                tCover = 1.0f - GetT01( Position.y, yMin, yMax );
                            }
                        }
                        
                        flo32 Alpha = 1.0f;
                        
                        flo32 FadeIn_TargetTime  = ( 20.0f / 60.0f );
                        flo32 FadeOut_TargetTime = ( 30.0f / 60.0f );
                        
                        FadeIn_TargetTime  = MinValue( Particle.TargetTime * 0.3333f, FadeIn_TargetTime  );
                        FadeOut_TargetTime = MinValue( Particle.TargetTime * 0.5f,    FadeOut_TargetTime );
                        
                        if( Particle.Timer <= FadeIn_TargetTime ) {
                            Alpha = Clamp01( Particle.Timer / FadeIn_TargetTime );
                        }
                        flo32 TargetTimeA = ( Particle.TargetTime - FadeOut_TargetTime );
                        if( Particle.Timer >= TargetTimeA ) {
                            Alpha = 1.0f - Clamp01( ( Particle.Timer - TargetTimeA ) / FadeOut_TargetTime );
                        }
                        
                        flo32 Radius_Inner = OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_INNER;
                        flo32 Radius_Outer = OVERWORLD_GRAVITY_WELL_PARTICLES_RADIUS_OUTER;
                        
                        //flo32 tDist = Clamp01( Particle.Dist / Radius_Outer );
                        //Alpha = MinValue( Alpha, lerp( 1.0f, tDist * tDist, 0.25f ) );
                        //Alpha = MinValue( Alpha, Clamp01( Particle.Dist / 200.0f ) );
                        
                        vec4   Color = Vec4( 1.0f, 1.0f, 0.5f, 1.0f );
                        if( Particle.iColor == 1 ) {
                            Color = Vec4( 0.75f, 0.75f, 1.0f, 1.0f );
                        } else if( Particle.iColor == 2 ) {
                            Color = Vec4( 1.0f, 0.5f, 0.5f, 1.0f );
                        }
                        
                        flo32 tColor    = Clamp01( Particle.Dist / Radius_Outer );
                        vec4  TempColor = TowardWhite( Color, ( 1.0f - tColor ) * 0.25f );
                        if( Particle.tSpeed < 1.0f ) {
                            TempColor = TowardBlack( TempColor, 1.0f - Particle.tSpeed );
                        } else {
                            TempColor = TowardWhite( TempColor, Particle.tSpeed - 1.0f );
                        }
                        
                        Color = lerp( TempColor, tGravityWell, Color );
                        Color *= Alpha;
                        
                        flo32 tDim  = Clamp01( Particle.Dist / ( Radius_Outer * 0.2f ) );
                        flo32  Dim0 = lerp( 3.0f, tDim * tDim, 2.0f );
                        vec2   Dim  = Vec2( Dim0, Dim0 );
                        
                        Dim.y = MaxValue( 2.0f, Particle.Speed * 4.0f );
                        
                        orect Bound = ORectTD( Position, Vec2( 0.5f, 0.0f ), Dim, Particle.Radians );
                        DrawORect( Pass_Game, Bound, Color * tCover * tAlpha );
                        
                        //DrawPoint( Pass_Game, OverworldT->Position + Offset, Vec2( Dim, Dim ), Color );
                    }
                }
                
                {
                    vec3 Position = Vec3( OverworldT->Player_Position, -50.0f ) + Cow->Position;
                    vec3 Scale    = Vec3( 1.0f, 1.0f, 1.0f ) * 25.0f;
                    DrawModel( &Renderer->Pass_Cow_Overworld, MeshID_Cow, TextureID_Cow, Position, Scale, Cow->Orientation, COLOR_WHITE );
                    DrawModel( &Renderer->Pass_Cow_Overworld, MeshID_Spacesuit, TextureID_Spacesuit, Position, Scale, Cow->Orientation, COLOR_WHITE );
                }
                
#if 0                
                {
                    DrawRect( Pass_UI, AppState->UI_Bound, COLOR_BLUE );
                    vec3 Position = Vec3( 960.0f, 540.0f, -50.0f );
                    vec3 Scale    = Vec3( 1.0f, 1.0f, 0.1f ) * 250.0f;
                    quat Orientation = QuatDef();
                    DrawModel( &Renderer->Pass_Cow_Overworld, MeshID_Cow, TextureID_Cow, Position, Scale, Orientation, COLOR_WHITE );
                    DrawModel( &Renderer->Pass_Cow_Overworld, MeshID_Spacesuit, TextureID_Spacesuit, Position, Scale, Orientation, COLOR_WHITE );
                }
#endif
                
            }
            
            if( DoLoadLevel ) {
                OVERWORLD_LEVEL Level = OverworldS->Level[ OverworldS->AtLevel ];
                if( OverworldS->AtLevel == LevelID_Market ) {
                    AppState->Mode = AppMode_Market;
                    AppState->Market_iSelect = -1;
                    
                    ZoomCameraToWorldBound( &AppState->Camera, AppState->UI_Bound, AppState->UI_Bound, AppState->UI_Bound );
                    
                    Mouse->ResetPos     = false;
                    Mouse->DoShowCursor = true;
                } else {
                    //StartLevel( Platform, AppState, TempMemory, Level.FileName );
                    //Mouse->ResetPos = true;
                    AppState->Mode = AppMode_LevelInfo;
                    
                    OVERWORLD_MAP Map = OverworldT->Map[ OverworldS->AtLevel ];
                    AppState->CurrentLevel.Level = Level;
                    AppState->CurrentLevel.Info  = Map.Info;
                    AppState->CurrentLevel.LevelID = OverworldS->AtLevel;
                }
            }
            
#if ENABLE_EDITOR
            if( WasPressed( Keyboard, KeyCode_e, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode  = AppMode_Editor;
                Mouse->ResetPos = false;
            }
#endif
            
#if 0            
            OverworldT->ResetGameFile_Timer = MaxValue( OverworldT->ResetGameFile_Timer - dT, 0.0f );
            if( WasPressed( Keyboard, KeyCode_c, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                if( OverworldT->ResetGameFile_Timer > 0.0f ) {
                    OverworldT->ResetGameFile_Timer = 0.0f;
                    
                    if( Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, "MyGame", FILETAG__SAVEGAME_FILE ) ) {
                        Platform->DeleteFile( LEVEL_SAVE_DIRECTORY, "MyGame", FILETAG__SAVEGAME_FILE );
                    }
                    if( Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, "MyGame", "POWERUPS" ) ) {
                        Platform->DeleteFile( LEVEL_SAVE_DIRECTORY, "MyGame", "POWERUPS" );
                    }
                    
                    ResetGameData  ( AppState );
                    GenOverworld   ( AppState );
                    GenPowerupStats( AppState );
                } else {
                    OverworldT->ResetGameFile_Timer = 5.0f;
                }
            }
            if( OverworldT->ResetGameFile_Timer > 0.0f ) {
                DISPLAY_STRING( "Press Escape to Cancel", COLOR_RED );
                
                char Str[ 256 ] = {};
                sprintf( Str, "CONFIRM! To clear the save game file, press CTRL + SHIFT + C again before timer runs out: %.02f", OverworldT->ResetGameFile_Timer );
                DISPLAY_STRING( Str, COLOR_RED );
                
                if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                    OverworldT->ResetGameFile_Timer = 0.0f;
                }
            } else {
                DISPLAY_STRING( "To clear the save game file, press CTRL + SHIFT + C.", COLOR_GREEN );
            }
            
            DISPLAY_STRING( " over the level and press CTRL + SHIFT + V.", COLOR_GREEN );
            DISPLAY_STRING( "To reset the score of a level, hover the mouse", COLOR_GREEN );
            if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_v, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) ) {
                OVERWORLD_LEVEL * Level = OverworldS->Level + iHover;
                Level->IsComplete                = false;
                Level->HighScore                 = 0;
                Level->HighMultiplier            = 0;
                Level->HighAvgBonus              = 0.0f;
                Level->HighAvgBonus_FullTime     = 0.0f;
                Level->HighAvgBonus_TotalTime    = 0.0f;
                Level->ScoreRating               = 0;
                Level->BonusObjectiveWasAcquired = false;
            }
#endif
            
            if( DrawAndUpdateTitleButton( Pass_UI, Mouse, AppState->FontList + FontID_Title_2_00, "Back", RectBRD( GetBR( AppState->UI_Bound ) + Vec2( -20.0f, 20.0f ), Vec2( 200.0f, 80.0f ) ) ) ) {
                AppState->Mode = AppMode_TitleScreen;
                
                if( OverworldT->Player_DoTravel ) {
                    OverworldT->Player_DoTravel       = false;
                    OverworldT->Player_DoTravel_Timer = 0.0f;
                }
            }
        } break;
        
        case AppMode_Market: {
            {
                flo32 dAlpha = 1.0f / 0.5f;
                AppState->FadeOut_Alpha = Clamp01( AppState->FadeOut_Alpha + dAlpha * dT );
            }
            
            RENDER_PASS  * Pass    = Pass_UI;
            CAMERA_STATE * Camera  = &AppState->Camera;
            
            rect Select_Bound = AppState->UI_Bound;
            Select_Bound.Right = lerp( Select_Bound.Left,  0.5f, Select_Bound.Right  );
            
            rect Upgrade_Bound = AppState->UI_Bound;
            Upgrade_Bound.Left = lerp( Upgrade_Bound.Left, 0.5f, Upgrade_Bound.Right );
            
            flo32 OffsetFromSides      = 80.0f;
            flo32 OffsetBetweenEntries = 12.0f;
            
            int32 nEntryPerRow = 3;
            flo32 Entry_Width = ( GetWidth( Select_Bound ) - ( OffsetFromSides * 2.0f ) - ( OffsetBetweenEntries * 2.0f ) ) / ( flo32 )nEntryPerRow;
            vec2  Entry_Dim   = Vec2( Entry_Width, Entry_Width );
            rect  Entry_Bound[ PowerupType_Count ] = {};
            
            boo32 HasFunds = true;
            
            {
                // label
                // icon
                // percent
                //  percent upgrade cost +1, +5, +10
                
                int32 iHover  = -1;
                vec2  MouseP  = Mouse->Position;
                
                POWERUP_STATE_SAVE_DATA * PowerupSave = &AppState->SaveGame.PowerupSave;
                POWERUP_TYPE_LABELS;
                
                {
                    vec2 AtPos = GetTL( Select_Bound ) + Vec2( OffsetFromSides, -OffsetFromSides * 2.0f );
                    
                    DrawString( Pass, AppState->FontList + FontID_Title_2_00, "POWERUPS", AtPos + Vec2( 30.0f, 12.0f ), TextAlign_BottomLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    for( int32 iPowerupEntry = PowerupType_None + 1; iPowerupEntry < PowerupType_Count; iPowerupEntry++ ) {
                        rect Bound = RectTD( AtPos, Vec2( 0.0f, 1.0f ), Entry_Dim );
                        
                        Entry_Bound[ iPowerupEntry ] = Bound;
                        if( IsInRect( MouseP, Bound ) ) {
                            iHover = iPowerupEntry;
                        }
                        
                        AtPos.x += ( Entry_Dim.x + OffsetBetweenEntries );
                        if( iPowerupEntry == 3 ) {
                            AtPos.x  = Select_Bound.Left + OffsetFromSides;
                            AtPos.y -= ( Entry_Dim.y + OffsetBetweenEntries );
                        }
                    }
                    
                    if( ( iHover > -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                        AppState->Market_iSelect = iHover;
                    }
                }
                
                {
                    
                    flo32 SelectButton_Margin = 12.0f;
                    
                    for( int32 iPowerupEntry = PowerupType_None + 1; iPowerupEntry < PowerupType_Count; iPowerupEntry++ ) {
                        POWERUP_ENTRY * PowerupEntry = PowerupSave->PowerupEntry + iPowerupEntry;
                        
                        rect BoundA = Entry_Bound[ iPowerupEntry ];
                        rect BoundB = AddRadius( BoundA, -SelectButton_Margin );
                        
                        vec4 ColorA = COLOR_GRAY( 0.1f );
                        vec4 ColorB = COLOR_GRAY( 0.05f );
                        
                        if( iHover == iPowerupEntry ) {
                            ColorA = COLOR_GRAY( 0.35f );
                        }
                        if( AppState->Market_iSelect == iPowerupEntry ) {
                            ColorA = COLOR_YELLOW;
                        }
                        
                        DrawRect( Pass, BoundA, ColorA );
                        DrawRect( Pass, BoundB, ColorB );
                        
                        rect Icon_Bound = RectCD( GetT( BoundA, Vec2( 0.5f, 0.55f ) ), Entry_Dim * 0.25f );
                        
                        flo32 Scale = GetWidth( Icon_Bound ) * 0.5f;
                        if( ( POWERUP_TYPE )iPowerupEntry == PowerupType_PlayerBullets ) {
                            Scale *= 1.5f;
                        }
                        
                        DrawPowerupIcon( Pass, ( POWERUP_TYPE )iPowerupEntry, GetCenter( Icon_Bound ), Scale, 1.0f );
                        DrawString( Pass, AppState->FontList + FontID_1_50, PowerupTypeLabels[ iPowerupEntry ], GetTL( BoundA ) + Vec2( SelectButton_Margin + 12.0f, -SelectButton_Margin - 12.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        
                        char Str[ 128 ] = {};
                        sprintf( Str, "%d", PowerupEntry->bNumer );
                        DrawString( Pass, AppState->FontList + FontID_1_75, Str, GetT( BoundA, Vec2( 0.5f, 0.25f ) ), TextAlign_TopCenter, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    }
                }
                
                { // Instructions
                    flo32 Text_Margin = 120.0f;
                    flo32  LineY = 36.0f;
                    FONT * Font  = AppState->FontList + FontID_1_50;
                    
                    rect Instructions_Bound = Select_Bound;
                    Instructions_Bound.Bottom += 40.0f;
                    Instructions_Bound.Top     = Instructions_Bound.Bottom + 260.0f;
                    Instructions_Bound.Left   += Text_Margin;
                    Instructions_Bound.Right  -= Text_Margin;
                    
                    vec2 AtPos = GetTL( Instructions_Bound );
                    
                    DrawString( Pass, Font, " PURCHASE NOTES", AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_GRAY( 0.8f ) );
                    AtPos.y -= LineY;
                    
                    DrawString( Pass, Font, "   The VALUE of a POWERUP refers to the number of times", AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    AtPos.y -= LineY;
                    
                    DrawString( Pass, Font, "that it will appear during a level. For example, a VALUE of  250", AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    AtPos.y -= LineY;
                    
                    DrawString( Pass, Font, "means that a POWERUP will appear TWICE during a level", AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    AtPos.y -= LineY;
                    
                    DrawString( Pass, Font, "AND  has a 50% chance of appearing a THIRD time.", AtPos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                    AtPos.y -= LineY;
                }
                
                {
                    rect Purchase_Bound   = Upgrade_Bound;
                    Purchase_Bound.Bottom += 120.0f;
                    
                    vec4 Outline_Color    = COLOR_GRAY( 0.1f );
                    vec4 Background_Color = COLOR_GRAY( 0.05f );
                    
                    flo32 Purchase_Margin = 12.0f;
                    
                    flo32 AtY = Purchase_Bound.Top - Purchase_Margin;
                    
                    DrawRect( Pass,            Purchase_Bound,                     Outline_Color    );
                    DrawRect( Pass, AddRadius( Purchase_Bound, -Purchase_Margin ), Background_Color );
                    
                    char Str[ 128 ] = {};
                    
                    { // Total Points
                        AtY -= 20.0f;
                        
                        FONT * Font = AppState->FontList + FontID_3_00;
                        
                        flo32 ColonX   = lerp( Purchase_Bound.Left, 0.4f, Purchase_Bound.Right );
                        flo32 AdvanceY = 80.0f;
                        flo32 PosY     = AtY - ( AdvanceY * 0.5f );
                        
                        DrawString( Pass, Font, ":", Vec2( ColonX, PosY ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        DrawString( Pass, Font, "Total Points", Vec2( ColonX - 20.0f, PosY ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        
                        sprintf( Str, "%llu", PowerupSave->TotalPoints );
                        flo32 xOffset = MaxValue( 160.0f, GetDimOfString( Font, Str ).Width + 20.0f );
                        DrawString( Pass, Font, Str, Vec2( ColonX + xOffset, PosY ), TextAlign_CenterRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        
                        AtY -= AdvanceY;
                    }
                    
                    if( AppState->Market_iSelect > -1 ) {
                        { // Description Window
                            AtY -= 20.0f;
                            
                            rect Description_Bound = AddRadius( Purchase_Bound, -Purchase_Margin );
                            Description_Bound.Top    = AtY;
                            Description_Bound.Bottom = Description_Bound.Top - 200.0f;
                            
                            rect Icon_Bound  = Description_Bound;
                            Icon_Bound.Right = Icon_Bound.Left + GetHeight( Description_Bound );
                            Icon_Bound = AddRadius( Icon_Bound, -12.0f );
                            
                            rect Text_Bound = Description_Bound;
                            Text_Bound.Left  += GetHeight( Description_Bound );
                            Text_Bound.Left  += Purchase_Margin;
                            Text_Bound.Right -= Purchase_Margin;
                            
                            DrawRect( Pass,            Icon_Bound,          Outline_Color    );
                            DrawRect( Pass, AddRadius( Icon_Bound, -8.0f ), Background_Color );
                            
                            DrawPowerupIcon( Pass, ( POWERUP_TYPE )AppState->Market_iSelect, GetCenter( Icon_Bound ), GetWidth( Icon_Bound ) * 0.5f - 24.0f, 1.0f );
                            
                            DrawRect( Pass, Text_Bound, Outline_Color );
                            DrawString( Pass, AppState->FontList + FontID_1_75, PowerupTypeLabels[ AppState->Market_iSelect ], GetTL( Text_Bound ) + Vec2( 12.0f, -12.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            
                            switch( AppState->Market_iSelect ) {
                                case PowerupType_Health: {
                                    DrawString( Pass, AppState->FontList + FontID_1_50, "Recovers 1 Health Unit.", GetTL( Text_Bound ) + Vec2( 24.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    DrawString( Pass, AppState->FontList + FontID_1_25, "*will NOT increase maximum health unit capacity*", GetBL( Text_Bound ) + Vec2( 24.0f, 16.0f ), TextAlign_BottomLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                } break;
                                
                                case PowerupType_NoDamage: {
                                    DrawString( Pass, AppState->FontList + FontID_1_50, "Protects from taking damage for a limited time.", GetTL( Text_Bound ) + Vec2( 24.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                } break;
                                
                                case PowerupType_PlayerBullets: {
                                    DrawString( Pass, AppState->FontList + FontID_1_50, "Fire at a faster rate.", GetTL( Text_Bound ) + Vec2( 24.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    
                                } break;
                                
                                case PowerupType_ScoreMultipliers: {
                                    DrawString( Pass, AppState->FontList + FontID_1_50, "Multipliers are drawn towards you.", GetTL( Text_Bound ) + Vec2( 24.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    
                                } break;
                                
                                case PowerupType_DamageAllEnemies: {
                                    DrawString( Pass, AppState->FontList + FontID_1_50, "Damages all hostile units.", GetTL( Text_Bound ) + Vec2( 24.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    
                                } break;
                                
                                case PowerupType_SecondaryGadgets: {
                                    if( AppState->Market_ShowBrokenLink ) {
                                        DrawString( Pass, AppState->FontList + FontID_1_50, "Link broken.", GetTL( Text_Bound ) + Vec2( 24.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    } else {
                                        rect Click_Bound = Text_Bound;
                                        Click_Bound.Left   += 34.0f;
                                        Click_Bound.Right   = Click_Bound.Left + 110.0f;
                                        Click_Bound.Bottom += 14.0f;
                                        Click_Bound.Top     = Click_Bound.Bottom + 26.0f;
                                        
                                        if( IsInRect( MouseP, Click_Bound ) ) {
                                            DrawRect( Pass, Click_Bound, COLOR_GRAY( 0.125f ) );
                                            if( WasPressed( Mouse, MouseButton_Left ) ) {
                                                AppState->Market_ShowBrokenLink = true;
                                            }
                                        } else {
                                            DrawRect( Pass, Click_Bound, COLOR_GRAY( 0.05f ) );
                                        }
                                        DrawString( Pass, AppState->FontList + FontID_1_50, "Improves operation of Gadgets.", GetTL( Text_Bound ) + Vec2( 24.0f, -52.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                        DrawString( Pass, AppState->FontList + FontID_1_25, "*Applies to Collector and Auto-Turrets, only.", GetBL( Text_Bound ) + Vec2( 24.0f, 40.0f ), TextAlign_BottomLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                        DrawString( Pass, AppState->FontList + FontID_1_25, "**CLICK HERE for additional updates.", GetBL( Text_Bound ) + Vec2( 24.0f, 16.0f ), TextAlign_BottomLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    }
                                } break;
                            }
                        }
                    } else {
                        if( HasFunds ) {
                            DrawString( Pass, AppState->FontList + FontID_2_00, "Select a POWERUP", GetCenter( Purchase_Bound ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        } else {
                            DrawString( Pass, AppState->FontList + FontID_2_00, "You have no POINTS to spend.", GetCenter( Purchase_Bound ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            DrawString( Pass, AppState->FontList + FontID_2_00, "Have a nice day.", GetCenter( Purchase_Bound ) + Vec2( 0.0f, -40.0f ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        }
                    }
                    
                    if( AppState->Market_iSelect > -1 ) {
                        vec2 AtPos = GetT( Purchase_Bound, Vec2( 0.0f, 1.0f ) );
                        
                        POWERUP_ENTRY * PowerupEntry = PowerupSave->PowerupEntry + AppState->Market_iSelect;
                        
                        // upgrade button
                        // cost to upgrade
                        flo64 Interest = 0.01;
                        
                        int32 nToBuy = 0;
                        {
                            flo64  UpgradeCost = PowerupEntry->UpgradeCost;
                            flo64 bUpgradeCost = PowerupEntry->bUpgradeCost;
                            uint64 TotalPoints = PowerupSave->TotalPoints;
                            
                            while( TotalPoints > ( uint64 )UpgradeCost ) {
                                nToBuy += 1;
                                
                                TotalPoints -= ( uint64 )UpgradeCost;
                                UpgradeCost += ( bUpgradeCost * Interest );
                            }
                        }
                        
                        char * Button_Label[] = {
                            "Sell All", "-100", "-10", "-1", "+1", "+10", "+100", "Buy Max"
                        };
                        int32 nButton     = _ArrayCount( Button_Label );
                        vec2   Button_Dim = Vec2( 80.0f, 40.0f );
                        
                        flo32 ButtonOffsetFromSides = 28.0f;
                        flo32 ButtonOffsetBetween   = 8.0f;
                        Button_Dim.x  = GetWidth( Purchase_Bound ) - ( ButtonOffsetFromSides * 2.0f );
                        Button_Dim.x -= ButtonOffsetBetween * ( ( flo32 )( nButton - 1 ) );
                        Button_Dim.x /= ( flo32 )nButton;
                        
                        boo32  Button_IsAvailable[ _ArrayCount( Button_Label ) ] = {};
                        for( int32 iButton = 0; iButton < nButton; iButton++ ) {
                            Button_IsAvailable[ iButton ] = true;
                        }
                        
                        if( PowerupEntry->bNumer <= 0 ) {
                            Button_IsAvailable[ 0 ] = false;
                        }
                        if( PowerupEntry->bNumer <  100 ) {
                            Button_IsAvailable[ 1 ] = false;
                        }
                        if( PowerupEntry->bNumer <  10  ) {
                            Button_IsAvailable[ 2 ] = false;
                        }
                        if( PowerupEntry->bNumer <= 0   ) {
                            Button_IsAvailable[ 3 ] = false;
                        }
                        if( nToBuy < 1 ) {
                            Button_IsAvailable[ 4 ] = false;
                        }
                        if( nToBuy < 10 ) {
                            Button_IsAvailable[ 5 ] = false;
                        }
                        if( nToBuy < 100 ) {
                            Button_IsAvailable[ 6 ] = false;
                        }
                        if( nToBuy < 1 ) {
                            Button_IsAvailable[ 7 ] = false;
                        }
                        
                        int32 iTransaction = -1;
                        AtPos = GetT( Purchase_Bound, Vec2( 0.0f, 0.2f ) ) + Vec2( ButtonOffsetFromSides, 0.0f );
                        
                        int32 nAvailable = 0;
                        vec2  Label_Pos  = AtPos + Vec2( 12.0f, 4.0f );
                        
                        for( int32 iButton = 0; iButton < nButton; iButton++ ) {
                            Background_Color = COLOR_GRAY( 0.05f );
                            Outline_Color    = COLOR_GRAY( 0.10f );
                            
                            rect Outer_Bound = RectTD( AtPos, Vec2( 0.0f, 1.0f ), Button_Dim );
                            rect Inner_Bound = AddRadius( Outer_Bound, -4.0f );
                            
                            if( Button_IsAvailable[ iButton ] ) {
                                nAvailable += 1;
                                
                                if( IsInRect( MouseP, Outer_Bound ) ) {
                                    Outline_Color = COLOR_GRAY( 0.5f );
                                    
                                    if( WasPressed( Mouse, MouseButton_Left ) ) {
                                        iTransaction     = iButton;
                                        Background_Color = Vec4( 0.5f, 0.5f, 0.0f, 1.0f );
                                    }
                                }
                                
                                DrawRect( Pass, Outer_Bound, Outline_Color );
                                DrawRect( Pass, Inner_Bound, Background_Color );
                                
                                DrawString( Pass, AppState->FontList + FontID_1_25, Button_Label[ iButton ], GetCenter( Outer_Bound ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else {
                                DrawRect( Pass, Outer_Bound, Outline_Color );
                            }
                            
                            AtPos.x += ( Button_Dim.x + ButtonOffsetBetween );
                        }
                        
                        if( nAvailable > 0 ) {
                            DrawString( Pass, AppState->FontList + FontID_1_25, "Select Amount:", Label_Pos, TextAlign_BottomLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        }
                        
                        { // Units Owned
                            AtY          = lerp( Purchase_Bound.Bottom, 0.4f, Purchase_Bound.Top   );
                            flo32 ColonX = lerp( Purchase_Bound.Left,   0.6f, Purchase_Bound.Right );
                            DrawString( Pass, AppState->FontList + FontID_2_00, ":", Vec2( ColonX, AtY ), TextAlign_TopCenter, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            DrawString( Pass, AppState->FontList + FontID_2_00, "Units Owned", Vec2( ColonX - 20.0f, AtY ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            
                            sprintf( Str, "%d", ( int32 )PowerupEntry->bNumer );
                            DrawString( Pass, AppState->FontList + FontID_2_25, Str, Vec2( ColonX + 240.0f, AtY ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        }
                        
                        { // Unit Price
                            AtY          = lerp( Purchase_Bound.Bottom, 0.35f, Purchase_Bound.Top   );
                            flo32 ColonX = lerp( Purchase_Bound.Left,   0.6f, Purchase_Bound.Right );
                            DrawString( Pass, AppState->FontList + FontID_2_00, ":", Vec2( ColonX, AtY ), TextAlign_TopCenter, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            DrawString( Pass, AppState->FontList + FontID_2_00, "Unit Price*", Vec2( ColonX - 20.0f, AtY ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            
                            sprintf( Str, "%d", ( int32 )PowerupEntry->UpgradeCost );
                            DrawString( Pass, AppState->FontList + FontID_2_25, Str, Vec2( ColonX + 240.0f, AtY ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        }
                        
                        if( iTransaction > -1 ) {
                            uint64 LastTransaction_TotalCost = 0;
                            AppState->Market_LastTransaction_Timer = MARKET_LAST_TRANSACTION_TARGET_TIME;
                            
                            if( iTransaction <= 3 ) {
                                AppState->Market_LastTransaction_DoBuy = false;
                                
                                int32 nTransaction = PowerupEntry->bNumer;
                                if( iTransaction == 3 ) {
                                    nTransaction = 1;
                                }
                                if( iTransaction == 2 ) {
                                    nTransaction = 10;
                                }
                                if( iTransaction == 1 ) {
                                    nTransaction = 100;
                                }
                                for( int32 Iter = 0; Iter < nTransaction; Iter++ ) {
                                    // NOTE: These statements need to be in this order!
                                    PowerupEntry->UpgradeCost  -= ( PowerupEntry->bUpgradeCost * Interest );
                                    PowerupEntry->bNumer       -= 1;
                                    PowerupSave->TotalPoints  += ( uint64 )PowerupEntry->UpgradeCost;
                                    LastTransaction_TotalCost += ( uint64 )PowerupEntry->UpgradeCost;
                                }
                            } else {
                                AppState->Market_LastTransaction_DoBuy = true;
                                
                                int32 nTransaction = nToBuy;
                                if( iTransaction == 4 ) {
                                    nTransaction = 1;
                                }
                                if( iTransaction == 5 ) {
                                    nTransaction = 10;
                                }
                                if( iTransaction == 6 ) {
                                    nTransaction = 100;
                                }
                                for( int32 Iter = 0; Iter < nTransaction; Iter++ ) {
                                    // NOTE: These statements need to be in this order!
                                    LastTransaction_TotalCost += ( uint64 )PowerupEntry->UpgradeCost;
                                    PowerupSave->TotalPoints  -= ( uint64 )PowerupEntry->UpgradeCost;
                                    PowerupEntry->bNumer       += 1;
                                    PowerupEntry->UpgradeCost  += ( PowerupEntry->bUpgradeCost * Interest );
                                }
                            }
                            
                            AppState->Market_LastTransaction_TotalCost = LastTransaction_TotalCost;
                        }
                        
                        { // Last Transaction
                            AppState->Market_LastTransaction_Timer = MaxValue( AppState->Market_LastTransaction_Timer - dT, 0.0f );
                            if( AppState->Market_LastTransaction_Timer > 0.0f ) {
                                vec4 Color = Vec4( 1.0f, 0.5f, 0.5f, 1.0f );
                                if( AppState->Market_LastTransaction_DoBuy ) {
                                    Color = Vec4( 0.5f, 1.0f, 0.5f, 1.0f );
                                }
                                
                                flo32 Alpha = Clamp01( AppState->Market_LastTransaction_Timer / ( MARKET_LAST_TRANSACTION_TARGET_TIME - ( 40.0f / 60.0f ) ) );
                                AtY          = lerp( Purchase_Bound.Bottom, 0.3f, Purchase_Bound.Top   );
                                flo32 ColonX = lerp( Purchase_Bound.Left,   0.6f,  Purchase_Bound.Right );
                                DrawString( Pass, AppState->FontList + FontID_2_00, ":", Vec2( ColonX, AtY ), TextAlign_TopCenter, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                                DrawString( Pass, AppState->FontList + FontID_2_00, "Last Transaction", Vec2( ColonX - 20.0f, AtY ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_WHITE * Alpha );
                                
                                if( AppState->Market_LastTransaction_DoBuy ) {
                                    sprintf( Str, "-%llu", AppState->Market_LastTransaction_TotalCost );
                                } else {
                                    sprintf( Str, "+%llu", AppState->Market_LastTransaction_TotalCost );
                                }
                                DrawString( Pass, AppState->FontList + FontID_2_25, Str, Vec2( ColonX + 240.0f, AtY ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), Color * Alpha );
                            }
                        }
                        
                        { // Fine Print
                            rect FinePrint_Bound = RectBLD( GetBL( Purchase_Bound ) + Vec2( Purchase_Margin, Purchase_Margin ) + Vec2( 4.0f, 4.0f ), Vec2( 80.0f, 36.0f ) );
                            DrawRect( Pass, FinePrint_Bound, COLOR_GRAY( 0.075f ) );
                            //DrawString( Pass, AppState->FontList + FontID_1_50, "-", GetT( FinePrint_Bound, Vec2( 0.0f, 0.5f ) ) + Vec2( 48.0f, -2.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            if( IsInRect( MouseP, FinePrint_Bound ) ) {
                                FinePrint_Bound.Right = Purchase_Bound.Right - Purchase_Margin - 4.0f;
                                DrawRect( Pass, FinePrint_Bound, COLOR_GRAY( 0.1f ) );
                                DrawString( Pass, AppState->FontList + FontID_1_50, "- UNIT PRICE will increase/decrease by 1% with each unit sold.", GetT( FinePrint_Bound, Vec2( 0.0f, 0.5f ) ) + Vec2( 48.0f, -2.0f ), TextAlign_CenterLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else {
                                DrawTriangleBC( Pass, GetT( FinePrint_Bound, Vec2( 0.0f, 0.5f ) ) + Vec2( 60.0f, 0.0f ), Vec2( 10.0f, 6.0f ), -PI * 0.5f, COLOR_WHITE );
                            }
                            DrawString( Pass, AppState->FontList + FontID_2_50, "*", GetT( FinePrint_Bound, Vec2( 0.0f, 0.5f ) ) + Vec2( 18.0f, 0.0f ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                        }
                    }
                }
            }
            
            
            if( DrawAndUpdateTitleButton( Pass_UI, Mouse, AppState->FontList + FontID_Title_2_00, "Back", RectBRD( GetBR( AppState->UI_Bound ) + Vec2( -20.0f, 20.0f ), Vec2( 200.0f, 80.0f ) ) ) ) {
                AppState->Market_LastTransaction_Timer = 0.0f;
                AppState->Market_ShowBrokenLink        = false;
                
                AppState->Mode = AppMode_Overworld;
                
                ZoomCameraToWorldBound( &AppState->Camera, AppState->UI_Bound, AppState->UI_Bound, AppState->UI_Bound );
                
                Mouse->ResetPos = false;
                AppState->Overworld_Cow.IsInitialized = false;
                
                SaveGame( Platform, AppState, TempMemory );
            }
        } break;
        
#if ENABLE_EDITOR
        case AppMode_Editor: {
            EDITOR_STATE   * Editor   = &AppState->Editor;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !Editor->IsInitialized ) {
                Editor->IsInitialized = true;
                
                Editor->World_YUnits = WORLD_BOUND_BASE_Y_UNITS_FOR_TEXT_SCALE;
                Editor->World_Bound = GetWorldBound( AppState->App_Dim, Editor->World_YUnits );
                
                SaveOpen->FileList = StringBuffer( PermMemory, 4096, _KB( 256 ) );
                
                ResetEditor( Editor, AppState );
                InitEditorPanel( AppState );
                InitLayoutPanel( AppState );
                
                InitWaveEditor( Editor );
                InitCameraForWaveEditor  ( AppState );
                InitCameraForLayoutEditor( AppState );
            }
            
            EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
            DISPLAY_VALUE( uint32, WaveS->nWave );
            
            flo32 App_AspectRatio = AppState->App_Dim.x / AppState->App_Dim.y;
            vec2  Wave_Dim        = Vec2( App_AspectRatio * 3.0f, 3.0f );
            
            boo32 CanUpdateUndo = ( !Editor->ShowLayout_NextLink )
                && ( !Editor->ShowLayout_KillLink )
                && ( !Editor->ShowLayout_DoMove )
                && ( !Editor->ShowLayout_DoGroupMove )
                && ( !Editor->ShowLayout_DoGroupMove_DoSet )
                && ( !Editor->DoMove )
                && ( !Editor->DoCopy );
            UpdateUndo( Editor, Keyboard, CanUpdateUndo );
            
            if( WasPressed( Keyboard, KeyCode_q, KEYBOARD_FLAGS__CONTROL ) ) {
                Editor->ShowLayout = !Editor->ShowLayout;
                ResetUndo( Editor );
                
                if( Editor->ShowLayout ) {
                    PANEL * Panel = &Editor->Editor_Panel;
                    Panel->Show = true;
                }
            }
            
            boo32 CanSaveOpen = true;
            if( Editor->ShowLayout ) {
                // Delete Wave : Hover Wave + Delete
                // Set Link    :
                //  Set Src  : Hover + Right Click  ->  Set Dest : Hover + Right Click
                // Delete Link : Hover Link + Delete
                // Move        : Hover + Left Click + Drag
                // Set Starting Wave : Hover + F
                
                CAMERA_STATE * Camera = &Editor->Camera_LayoutEditor;
                UpdateCamera( Camera, Keyboard, dT );
                if( WasPressed( Keyboard, KeyCode_c, KEYBOARD_FLAGS__CONTROL ) ) {
                    InitCameraForLayoutEditor( AppState );
                }
                
                vec2 MousePrev = GetWorldPos( Camera, AppState->App_Dim, Mouse->PrevPosition );
                vec2 MouseP    = GetWorldPos( Camera, AppState->App_Dim, Mouse->Position );
                vec2 MouseDPos = MouseP - MousePrev;
                DISPLAY_VALUE( vec2, MouseP );
                
                PANEL * Panel = &Editor->Layout_Panel;
                rect View_Bound = AppState->App_Bound;
                if( Panel->Show ) {
                    View_Bound.Right = Panel->Bound.Left;
                }
                
                int32 iHover = -1;
                
                // Check links
                boo32 iHover_IsSetLink  = false;
                boo32 iHover_IsKillLink = false;
                int32 iHover_iLink      = -1;
                flo32 MinDist           = 0.35f * Camera->Scale_Ratio;
                flo32 MinDistSq         = MinDist * MinDist;
                vec2  iHover_iLink_A = {};
                vec2  iHover_iLink_B = {};
                
                if( IsInRect( Mouse->Position, View_Bound ) ) {
                    boo32 Mouse_IsActive = IsDown( Mouse, MouseButton_Left );
                    
                    if( Mouse_IsActive ) {
                        CanSaveOpen              = false;
                        SaveOpen->UnsavedChanges = true;
                    }
                    
                    boo32 CanCheckLinks = ( !Editor->ShowLayout_NextLink )
                        && ( !Editor->ShowLayout_KillLink )
                        && ( !Editor->ShowLayout_DoMove )
                        && ( !Editor->ShowLayout_DoGroupMove )
                        && ( !Editor->ShowLayout_DoGroupMove_DoSet )
                        && ( !IsInRect( MouseP, Editor->ShowLayout_DoGroupMove_Bound ) );
                    boo32 CanCheckWaves = ( !Editor->ShowLayout_DoMove )
                        && ( !Editor->ShowLayout_DoGroupMove )
                        && ( !Editor->ShowLayout_DoGroupMove_DoSet )
                        && ( !IsInRect( MouseP, Editor->ShowLayout_DoGroupMove_Bound ) );
                    
                    { // Check waves
                        if( ( CanCheckWaves ) && ( iHover == -1 ) ) {
                            for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                                WAVE Wave = WaveS->Wave[ iWave ];
                                
                                rect Wave_Bound = RectCD( Wave.Layout_Position, Wave_Dim );
                                if( IsInRect( MouseP, Wave_Bound ) ) {
                                    iHover = iWave;
                                }
                            }
                            
                            // NOTE: The Wave Editor assumes that Wave[ 0 ] is WaveType_EndLevel and cannot be deleted.
                            if( ( iHover > 0 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                                CaptureUndo( Editor );
                                
                                int32 DeleteGuid = WaveS->Wave[ iHover ].Guid;
                                
                                for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                                    WAVE * Wave = WaveS->Wave + iWave;
                                    
                                    { // Next Wave
                                        uint32 NewCount = 0;
                                        for( uint32 iNextWave = 0; iNextWave < Wave->nNextWave; iNextWave++ ) {
                                            if( Wave->NextWave_Guid[ iNextWave ] != DeleteGuid ) {
                                                Wave->NextWave     [ NewCount ] = Wave->NextWave     [ iNextWave ];
                                                Wave->NextWave_Guid[ NewCount ] = Wave->NextWave_Guid[ iNextWave ];
                                                NewCount++;
                                            }
                                        }
                                        Wave->nNextWave = NewCount;
                                    }
                                    { // Kill Wave
                                        int32 NewCount = 0;
                                        for( int32 iKillWave = 0; iKillWave < Wave->nKillWave; iKillWave++ ) {
                                            if( Wave->KillWave_Guid[ iKillWave ] != DeleteGuid ) {
                                                Wave->KillWave_Guid[ NewCount++ ] = Wave->KillWave_Guid[ iKillWave ];
                                            }
                                        }
                                        Wave->nKillWave = NewCount;
                                    }
                                }
                                
                                WaveS->Wave[ iHover ] = WaveS->Wave[ --WaveS->nWave ];
                                iHover = -1;
                                
                                WaveS->StartingWave = MinValue( WaveS->StartingWave, WaveS->nWave - 1 );
                                WaveS->CurrentWave  = MinValue( WaveS->StartingWave, WaveS->nWave - 1 );
                                
                                Assert( WaveS->nWave >= 1 );
                                if( WaveS->nWave == 1 ) { // Make New Blank Wave
                                    WaveS->CurrentGuid = 1;
                                    
                                    WAVE * Wave = Editor_NewWave( Editor );
                                    Wave->Type            = WaveType_SpawnEntities;
                                    Wave->Exit            = ExitType_TargetTime;
                                    Wave->Exit_TargetTime = 10.0f;
                                }
                            }
                        }
                        if( ( iHover == 0 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            CONSOLE_STRING( "ERROR! Cannot delete Wave[ 0 ]! Wave Editor assumes that Wave[ 0 ] is WaveType_EndLevel and must exist!" );
                            GlobalVar_DebugSystem.ErrorOccurred = true;
                        }
                        
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_f ) ) ) {
                            WaveS->StartingWave = iHover;
                        }
                    }
                    
                    // NOTE: ( iHover == -1 ) means that the cursor is not in a wave bound.
                    if( CanCheckLinks ) {
                        if( iHover == -1 ) { // Next Wave Links
                            for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                                WAVE Wave = WaveS->Wave[ iWave ];
                                for( uint32 iNextWave = 0; iNextWave < Wave.nNextWave; iNextWave++ ) {
                                    WAVE * Dest = GetWaveByGuid( WaveS, Wave.NextWave_Guid[ iNextWave ] );
                                    if( Dest ) {
                                        vec2 A = Wave.Layout_Position;
                                        vec2 B = Dest->Layout_Position;
                                        
                                        flo32 DistSq = DistSqPointToEdge( MouseP, A, B );
                                        if( DistSq < MinDistSq ) {
                                            iHover            = iWave;
                                            iHover_iLink      = iNextWave;
                                            iHover_IsSetLink  = true;
                                            
                                            iHover_iLink_A = A;
                                            iHover_iLink_B = B;
                                        }
                                    }
                                }
                            }
                            
                            if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                                CaptureUndo( Editor );
                                
                                WAVE * Wave = WaveS->Wave + iHover;
                                Wave->NextWave_Guid[ iHover_iLink ] = Wave->NextWave_Guid[ --Wave->nNextWave ];
                                
                                iHover = -1;
                            }
                        }
                        
                        if( iHover == -1 ) { // Kill Wave Links
                            for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                                WAVE Wave = WaveS->Wave[ iWave ];
                                for( int32 iKillWave = 0; iKillWave < Wave.nKillWave; iKillWave++ ) {
                                    WAVE * Dest = GetWaveByGuid( WaveS, Wave.KillWave_Guid[ iKillWave ] );
                                    if( Dest ) {
                                        vec2 A = Wave.Layout_Position;
                                        vec2 B = Dest->Layout_Position;
                                        
                                        flo32 DistSq = DistSqPointToEdge( MouseP, A, B );
                                        if( DistSq < MinDistSq ) {
                                            iHover            = iWave;
                                            iHover_iLink      = iKillWave;
                                            iHover_IsKillLink = true;
                                            
                                            iHover_iLink_A = A;
                                            iHover_iLink_B = B;
                                        }
                                    }
                                }
                            }
                            
                            if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                                CaptureUndo( Editor );
                                
                                WAVE * Wave = WaveS->Wave + iHover;
                                Wave->KillWave_Guid[ iHover_iLink ] = Wave->KillWave_Guid[ --Wave->nKillWave ];
                                
                                iHover = -1;
                            }
                        }
                    }
                    
                    { // Move Waves
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( IsInRect( MouseP, Editor->ShowLayout_DoGroupMove_Bound ) ) {
                                Editor->ShowLayout_DoGroupMove      = true;
                                Editor->ShowLayout_DoGroupMove_bPos = MouseP;
                            } else {
                                Editor->ShowLayout_DoGroupMove_DoSet = true;
                                Editor->ShowLayout_DoGroupMove_bPos  = MouseP;
                            }
                        }
                        if( WasReleased( Mouse, MouseButton_Left ) ) {
                            if( Editor->ShowLayout_DoGroupMove_DoSet ) {
                                Editor->ShowLayout_DoGroupMove_Bound = RectExtrema( Editor->ShowLayout_DoGroupMove_bPos, MouseP );
                            }
                            if( Editor->ShowLayout_DoGroupMove ) {
                                vec2 dPos      = MouseP - Editor->ShowLayout_DoGroupMove_bPos;
                                rect Src_Bound = Editor->ShowLayout_DoGroupMove_Bound;
                                
                                for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                                    WAVE * Wave = WaveS->Wave + iWave;
                                    
                                    rect Wave_Bound = RectCD( Wave->Layout_Position, Wave_Dim );
                                    if( DoesRectIntersectRectInc( Wave_Bound, Src_Bound ) ) {
                                        Wave->Layout_Position += dPos;
                                    }
                                }
                                
                                Editor->ShowLayout_DoGroupMove_Bound = AddOffset( Src_Bound, dPos );
                            }
                            
                            Editor->ShowLayout_DoGroupMove       = false;
                            Editor->ShowLayout_DoGroupMove_DoSet = false;
                        }
                        
                        if( ( iHover > -1 ) && ( !iHover_IsSetLink ) && ( !iHover_IsKillLink ) &&  ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->ShowLayout_DoMove       = true;
                            Editor->ShowLayout_DoMove_iWave = iHover;
                            
                            if( iHover > 0 ) {
                                WaveS->CurrentWave = iHover;
                            }
                        }
                        if( WasReleased( Mouse, MouseButton_Left ) ) {
                            Editor->ShowLayout_DoMove = false;
                        }
                        if( Editor->ShowLayout_DoMove ) {
                            WAVE * Wave = WaveS->Wave + Editor->ShowLayout_DoMove_iWave;
                            Wave->Layout_Position += MouseDPos;
                        }
                    }
                    
                    { // Set Links
                        { // Next Wave
                            if( ( Editor->ShowLayout_NextLink ) && ( iHover == Editor->ShowLayout_NextLink_iWave ) ) {
                                iHover = -1;
                            }
                            
                            if( ( iHover > -1 ) && ( !iHover_IsSetLink ) && ( !iHover_IsKillLink ) && ( WasPressed( Mouse, MouseButton_Right ) ) ) {
                                if( Editor->ShowLayout_NextLink ) {
                                    CaptureUndo( Editor );
                                    
                                    WAVE * Src  = WaveS->Wave + Editor->ShowLayout_NextLink_iWave;
                                    WAVE * Dest = WaveS->Wave + iHover;
                                    
                                    Src->NextWave_Guid[ Src->nNextWave++ ] = Dest->Guid;
                                    
                                    Editor->ShowLayout_NextLink       = false;
                                    
                                    iHover = -1;
                                } else {
                                    Editor->ShowLayout_NextLink       = true;
                                    Editor->ShowLayout_NextLink_iWave = iHover;
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->ShowLayout_NextLink = false;
                            }
                        }
                        
                        { // Kill Wave
                            if( ( Editor->ShowLayout_KillLink ) && ( iHover == Editor->ShowLayout_KillLink_iWave ) ) {
                                iHover = -1;
                            }
                            
                            if( ( iHover > -1 ) && ( !iHover_IsSetLink ) && ( !iHover_IsKillLink ) && ( WasPressed( Mouse, MouseButton_Middle ) ) ) {
                                if( Editor->ShowLayout_KillLink ) {
                                    CaptureUndo( Editor );
                                    
                                    WAVE * Src  = WaveS->Wave + Editor->ShowLayout_KillLink_iWave;
                                    WAVE * Dest = WaveS->Wave + iHover;
                                    
                                    Src->KillWave_Guid[ Src->nKillWave++ ] = Dest->Guid;
                                    
                                    Editor->ShowLayout_KillLink       = false;
                                    
                                    iHover = -1;
                                } else {
                                    Editor->ShowLayout_KillLink       = true;
                                    Editor->ShowLayout_KillLink_iWave = iHover;
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->ShowLayout_KillLink = false;
                            }
                        }
                    }
                }
                
                UpdateAndDrawEditorPanel( AppState, Mouse, Pass_Game, Pass_UI, Editor->ShowLayout );
                
                { // DRAW
                    { // Draw Links
                        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                            WAVE Wave = WaveS->Wave[ iWave ];
                            for( uint32 iNextWave = 0; iNextWave < Wave.nNextWave; iNextWave++ ) {
                                WAVE * Dest = GetWaveByGuid( WaveS, Wave.NextWave_Guid[ iNextWave ] );
                                if( Dest ) {
                                    vec2 A = Wave.Layout_Position;
                                    vec2 B = Dest->Layout_Position;
                                    vec2 Offset = GetNormal( B - A ) * ( Wave_Dim.y * 0.45f );
                                    
                                    A += Offset;
                                    B -= Offset;
                                    
                                    vec2  Center  = ( A + B ) * 0.5f;
                                    vec2  Scale   = Vec2( 0.3f, GetLength( B - A ) ) * 0.5f;
                                    flo32 Radians = ToRadians( B - A ) - ( PI * 0.5f );
                                    
                                    DrawTriangle( Pass_Game, Center, Scale, Radians, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                                }
                            }
                            for( int32 iKillWave = 0; iKillWave < Wave.nKillWave; iKillWave++ ) {
                                WAVE * Dest = GetWaveByGuid( WaveS, Wave.KillWave_Guid[ iKillWave ] );
                                if( Dest ) {
                                    vec2 A = Wave.Layout_Position;
                                    vec2 B = Dest->Layout_Position;
                                    vec2 Offset = GetNormal( B - A ) * ( Wave_Dim.y * 0.45f );
                                    
                                    A += Offset;
                                    B -= Offset;
                                    
                                    vec2  Center  = ( A + B ) * 0.5f;
                                    vec2  Scale   = Vec2( 0.3f, GetLength( B - A ) ) * 0.5f;
                                    flo32 Radians = ToRadians( B - A ) - ( PI * 0.5f );
                                    
                                    DrawTriangle( Pass_Game, Center, Scale, Radians, Vec4( 0.5f, 0.0f, 0.0f, 1.0f ) );
                                }
                            }
                        }
                        
                        if( ( iHover > -1 ) && ( iHover_IsSetLink ) ) {
                            vec2 A = iHover_iLink_A;
                            vec2 B = iHover_iLink_B;
                            
                            vec2  Center  = ( A + B ) * 0.5f;
                            vec2  Scale   = Vec2( 0.3f, GetLength( B - A ) ) * 0.5f;
                            flo32 Radians = ToRadians( B - A ) - ( PI * 0.5f );
                            
                            Scale *= 1.1f;
                            
                            DrawTriangle( Pass_Game, Center, Scale, Radians, COLOR_YELLOW );
                        }
                        if( ( iHover > -1 ) && ( iHover_IsKillLink ) ) {
                            vec2 A = iHover_iLink_A;
                            vec2 B = iHover_iLink_B;
                            
                            vec2  Center  = ( A + B ) * 0.5f;
                            vec2  Scale   = Vec2( 0.3f, GetLength( B - A ) ) * 0.5f;
                            flo32 Radians = ToRadians( B - A ) - ( PI * 0.5f );
                            
                            Scale *= 1.1f;
                            
                            DrawTriangle( Pass_Game, Center, Scale, Radians, COLOR_RED );
                        }
                    }
                    
                    { // Draw Waves
                        vec4  * OutlineColor      = _PushArray      ( TempMemory, vec4,  WaveS->nWave );
                        boo32 * DoesIntersectWave = _PushArray_Clear( TempMemory, boo32, WaveS->nWave );
                        
                        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                            OutlineColor[ iWave ] = Vec4( 0.1f, 0.1f, 0.1f, 1.0f );
                        }
                        
                        for( uint32 iWaveA = 0; iWaveA < WaveS->nWave; iWaveA++ ) {
                            WAVE WaveA = WaveS->Wave[ iWaveA ];
                            
                            rect WaveA_Bound = RectCD( WaveA.Layout_Position, Wave_Dim );
                            for( uint32 iWaveB = iWaveA + 1; iWaveB < WaveS->nWave; iWaveB++ ) {
                                WAVE WaveB = WaveS->Wave[ iWaveB ];
                                
                                rect WaveB_Bound = RectCD( WaveB.Layout_Position, Wave_Dim );
                                
                                if( DoesRectIntersectRectInc( WaveA_Bound, WaveB_Bound ) ) {
                                    DoesIntersectWave[ iWaveA ] = true;
                                    DoesIntersectWave[ iWaveB ] = true;
                                }
                            }
                        }
                        
                        { // Set Outline Color for Do Group Move
                            rect Src_Bound = Editor->ShowLayout_DoGroupMove_Bound;
                            if( ( iHover == -1 ) && ( IsInRect( MouseP, Src_Bound ) ) ) {
                                for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                                    WAVE Wave       = WaveS->Wave[ iWave ];
                                    rect Wave_Bound = RectCD( Wave.Layout_Position, Wave_Dim );
                                    
                                    if( DoesRectIntersectRectInc( Src_Bound, Wave_Bound ) ) {
                                        OutlineColor[ iWave ] = COLOR_GRAY( 0.25f );
                                    }
                                }
                            }
                        }
                        
                        if( Editor->ShowLayout_NextLink ) {
                            OutlineColor[ Editor->ShowLayout_NextLink_iWave ] = COLOR_YELLOW;
                            OutlineColor[ iHover                           ] = Vec4( 0.5f, 0.5f, 0.0f, 1.0f );
                        } else if( Editor->ShowLayout_KillLink ) {
                            OutlineColor[ Editor->ShowLayout_KillLink_iWave ] = COLOR_RED;
                            OutlineColor[ iHover                           ] = Vec4( 0.5f, 0.5f, 0.0f, 1.0f );
                        } else if( Editor->ShowLayout_DoMove ) {
                            OutlineColor[ Editor->ShowLayout_DoMove_iWave  ] = COLOR_YELLOW;
                        } else if( ( iHover > -1 )  && ( !iHover_IsSetLink ) && ( !iHover_IsKillLink ) ) {
                            OutlineColor[ iHover ] = Vec4( 0.6f, 0.6f, 0.6f, 1.0f );
                        }
                        
                        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
                            WAVE Wave = WaveS->Wave[ iWave ];
                            
                            rect Wave_Bound = RectCD( Wave.Layout_Position, Wave_Dim );
                            if( Wave.Type == WaveType_EndLevel ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_GRAY( 0.25f ) );
                                DrawString( Pass_Game, AppState->FontList + FontID_2_00, "END LEVEL", WorldToUIPos( AppState, Camera, Wave.Layout_Position ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else if( Wave.Type == WaveType_OpenBayDoors ) {
                                DrawRect( Pass_Game, TextureID_WaveEditorBackground_AutoTurret, Wave_Bound, COLOR_GRAY( 0.5f ) );
                                
                                vec2 PosA = GetT( Wave_Bound, Vec2( 0.5f, 0.6f ) );
                                vec2 PosB = GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) );
                                
                                BAY_DOOR_EVENT_TYPE_LABELS;
                                BAY_DOOR_EVENT * Event = &Wave.BayDoorEvent;
                                
                                char Str[ 128 ] = {};
                                sprintf( Str, "%s %u %u", BayDoorEventTypeLabels[ Event->Type ], ( uint32 )Event->iSrc, ( uint32 )Event->iDest );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_50, Str, WorldToUIPos( AppState, Camera, PosB ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else if( Wave.Type == WaveType_SetAsteroidSettings ) {
                                DrawRect( Pass_Game, TextureID_WaveEditorBackground_Asteroid, Wave_Bound, COLOR_GRAY( 0.5f ) );
                                
                                ASTEROID_SETTINGS_EVENT * Event = &Wave.AsteroidSettingsEvent;
                                
                                vec2  AtPos   = GetT( Wave_Bound, Vec2( 0.02f, 0.98f ) );
                                flo32 yOffset = 0.6f;
                                
                                char Str[ 128 ] = {};
                                sprintf( Str, "Spawn: %u:60fps", ( uint32 )Event->Spawn_nFrames );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                AtPos.y -= yOffset;
                                
                                if( Event->Spawn_MaxAsteroid < 0 ) {
                                    strcpy( Str, "Max: INF" );
                                } else if( Event->Spawn_MaxAsteroid == 0 ) {
                                    strcpy( Str, "Max: ---" );
                                } else {
                                    sprintf( Str, "Max: %u", ( uint32 )Event->Spawn_MaxAsteroid );
                                }
                                DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                AtPos.y -= yOffset;
                                
                                char * Type_Label[ AsteroidType_Count ] = { "DEF", "BMB", "LSR", "SWR" };
                                
                                for( uint32 iType = 0; iType < AsteroidType_Count; iType++ ) {
                                    int32 * Numer_Class = Event->Numer_TypeClass[ iType ];
                                    strcpy( Str, Type_Label[ iType ] );
                                    
                                    for( uint32 iClass = 0; iClass < AsteroidClass_Count; iClass++ ) {
                                        if( Numer_Class[ iClass ] > 0 ) {
                                            char StrA[ 8 ] = {};
                                            sprintf( StrA, " %2u", ( uint32 )Numer_Class[ iClass ] );
                                            strcat( Str, StrA );
                                        } else {
                                            strcat( Str, " --" );
                                        }
                                        
                                    }
                                    
                                    DrawString( Pass_Game, AppState->FontList + FontID_0_90, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, WORLD_TEXT_SCALE * 0.9f, COLOR_WHITE );
                                    AtPos.y -= yOffset * 0.9f;
                                }
                            } else if( Wave.Type == WaveType_SetExplosiveSettings ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_BLACK );
                                
                                //EXPLOSIVE Explosive = ExplosiveC( Wave.Layout_Position, Vec2( 1.0f, 0.0f ) );
                                //DrawExplosive( Pass_Game, Explosive, 0.5f, 0.6f );
                                
                                EXPLOSIVE_SETTINGS_EVENT * Event = &Wave.ExplosiveSettingsEvent;
                                
                                vec2  AtPos   = GetT( Wave_Bound, Vec2( 0.02f, 0.98f ) );
                                flo32 yOffset = 0.6f;
                                
                                char Str[ 128 ] = {};
                                sprintf( Str, "Spawn: %u/60fps", ( uint32 )Event->Spawn_nFrames );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                AtPos.y -= yOffset;
                            } else if( Wave.Type == WaveType_SetSwarmSettings ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_BLACK );
                                
                                SWARM Swarm = SwarmC( -1, GetT( Wave_Bound, Vec2( 0.9f, 0.8f ) ) );
                                DrawSwarm( Pass_Game, Swarm, 0.5f, 0.6f );
                                
                                SWARM_SETTINGS_EVENT * Event = &Wave.SwarmSettingsEvent;
                                
                                if( Event->IsActive ) {
                                    vec2  AtPos   = GetT( Wave_Bound, Vec2( 0.02f, 0.98f ) );
                                    flo32 yOffset = 0.6f;
                                    
                                    SWARM_SPAWN_STYLE_LABELS;
                                    char * Label = SwarmSpawnStyleLabels[ Event->Style ];
                                    DrawString( Pass_Game, AppState->FontList + FontID_0_90, Label, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    AtPos.y -= yOffset;
                                    
                                    char Str[ 128 ] = {};
                                    
                                    sprintf( Str, "Spawn:  %u, (%u)", Event->nMaxSpawn, Event->nRespawnAt );
                                    DrawString( Pass_Game, AppState->FontList + FontID_0_90, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    AtPos.y -= yOffset;
                                    
                                    sprintf( Str, "Radius: (%.1f, %.1f)", Event->InnerRadius, Event->OuterRadius );
                                    DrawString( Pass_Game, AppState->FontList + FontID_0_90, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    AtPos.y -= yOffset;
                                    
                                    sprintf( Str, "Delay:  %u/60fps", Event->Delay_TargetTime_nFrames );
                                    DrawString( Pass_Game, AppState->FontList + FontID_0_90, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    AtPos.y -= yOffset;
                                } else {
                                    DrawString( Pass_Game, AppState->FontList + FontID_1_00, "Inactive", GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_RED );
                                }
                            } else if( Wave.Type == WaveType_SetTurretSettings ) {
                                DrawRect( Pass_Game, TextureID_WaveEditorBackground_Turret, Wave_Bound, COLOR_GRAY( 0.5f ) );
                                
                                TURRET_SETTINGS_EVENT * Event = &Wave.TurretSettingsEvent;
                                
                                vec2  AtPos   = GetT( Wave_Bound, Vec2( 0.02f, 0.98f ) );
                                flo32 yOffset = 0.6f;
                                
                                char Str[ 128 ] = {};
                                sprintf( Str, "Spawn: %u:60fps", ( uint32 )Event->Spawn_nFrames );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                AtPos.y -= yOffset;
                                
                                if( Event->Spawn_MaxTurret < 0 ) {
                                    strcpy( Str, "Max: INF" );
                                } else if( Event->Spawn_MaxTurret == 0 ) {
                                    strcpy( Str, "Max: ---" );
                                } else {
                                    sprintf( Str, "Max: %u", ( uint32 )Event->Spawn_MaxTurret );
                                }
                                DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                AtPos.y -= yOffset;
                                
                                char * Type_Label[ TurretSettings_Speed_Count ] = { "SLOW", "FAST" };
                                
                                for( uint32 iType = 0; iType < TurretSettings_Speed_Count; iType++ ) {
                                    int32 * Numer_Class = Event->Numer_SpeedRadians[ iType ];
                                    strcpy( Str, Type_Label[ iType ] );
                                    
                                    for( uint32 iClass = 0; iClass < TurretSettings_Radians_Count; iClass++ ) {
                                        if( Numer_Class[ iClass ] > 0 ) {
                                            char StrA[ 8 ] = {};
                                            sprintf( StrA, " %2u", ( uint32 )Numer_Class[ iClass ] );
                                            strcat( Str, StrA );
                                        } else {
                                            strcat( Str, " --" );
                                        }
                                        
                                    }
                                    
                                    DrawString( Pass_Game, AppState->FontList + FontID_0_90, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    AtPos.y -= yOffset * 0.9f;
                                }
                            } else if( Wave.Type == WaveType_EnableBombLauncher ) {
                                DrawRect( Pass_Game, TextureID_WaveEditorBackground_BombLauncher, Wave_Bound, COLOR_GRAY( 0.5f ) );
                                
                                vec2 Pos = GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) );
                                
                                BOMB_LAUNCHER_EVENT * Event = &Wave.BombLauncherEvent;
                                
                                char Str[ 128 ] = {};
                                sprintf( Str, "%u", ( uint32 )Event->iSrc );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_50, Str, WorldToUIPos( AppState, Camera, Pos ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else if( Wave.Type == WaveType_EnableMegaTurret ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_GRAY( 0.25f ) );
                                
                                vec2 Pos = GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) );
                                
                                DrawString( Pass_Game, AppState->FontList + FontID_0_75, "Enable Mega-Turret", WorldToUIPos( AppState, Camera, Pos ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else if( Wave.Type == WaveType_EnableBlimp ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_GRAY( 0.25f ) );
                                
                                vec2 Pos = GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) );
                                
                                DrawString( Pass_Game, AppState->FontList + FontID_0_75, "Enable Blimp", WorldToUIPos( AppState, Camera, Pos ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else if( Wave.Type == WaveType_EnableMegaMiner ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_GRAY( 0.25f ) );
                                
                                vec2 Pos = GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) );
                                
                                DrawString( Pass_Game, AppState->FontList + FontID_0_75, "Enable Mega-Miner", WorldToUIPos( AppState, Camera, Pos ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else if( Wave.Type == WaveType_SetWormSettings ) {
                                DrawRect( Pass_Game, TextureID_WaveEditorBackground_Worm, Wave_Bound, COLOR_GRAY( 0.5f ) );
                                
                                WORM_SETTINGS_EVENT * Event = &Wave.WormSettingsEvent;
                                
                                vec2  AtPos   = GetT( Wave_Bound, Vec2( 0.02f, 0.98f ) );
                                flo32 yOffset = 0.6f;
                                
                                char Str[ 128 ] = {};
                                sprintf( Str, "Spawn: %u:60fps", ( uint32 )Event->Spawn_nFrames );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                AtPos.y -= yOffset;
                                
                                if( Event->Spawn_MaxWorm < 0 ) {
                                    strcpy( Str, "Max: INF" );
                                } else if( Event->Spawn_MaxWorm == 0 ) {
                                    strcpy( Str, "Max: ---" );
                                } else {
                                    sprintf( Str, "Max: %u", ( uint32 )Event->Spawn_MaxWorm );
                                }
                                DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                AtPos.y -= yOffset;
                            } else if ( Wave.Type == WaveType_EnableLoader ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_GRAY( 0.25f ) );
                                
                                vec2 Pos = GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) );
                                
                                DrawString( Pass_Game, AppState->FontList + FontID_0_75, "Enable Loader", Pos, TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else if( Wave.Type == WaveType_SetSpitterSettings ) {
                                DrawRect( Pass_Game, TextureID_WaveEditorBackground_Spitter, Wave_Bound, COLOR_GRAY( 0.5f ) );
                                
                                SPITTER_SETTINGS_EVENT * Event = &Wave.SpitterSettingsEvent;
                                
                                vec2  AtPos   = GetT( Wave_Bound, Vec2( 0.02f, 0.98f ) );
                                flo32 yOffset = 0.6f;
                                
                                char Str[ 128 ] = {};
                                
                                SPITTER_GROUP_ID_LABELS;
                                
                                for( int32 iGroupID = 0; iGroupID < SpitterGroupID_Count; iGroupID++ ) {
                                    boo32 IsActive = Event->Group_IsActive[ iGroupID ];
                                    if( IsActive ) {
                                        if( iGroupID == SpitterGroupID_Spiral ) {
                                            sprintf( Str, "%12s: Active (%u)", SpitterGroupIDLabels[ iGroupID ], Event->nSpiralSpittersToFire );
                                        } else {
                                            sprintf( Str, "%12s: Active", SpitterGroupIDLabels[ iGroupID ] );
                                        }
                                    } else {
                                        sprintf( Str, "%12s: ---",    SpitterGroupIDLabels[ iGroupID ] );
                                    }
                                    DrawString( Pass_Game, AppState->FontList + FontID_1_00, Str, WorldToUIPos( AppState, Camera, AtPos ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                    AtPos.y -= yOffset;
                                }
                            } else if( Wave.Type == WaveType_EnableLevelTimer ) {
                                DrawRect( Pass_Game, TextureID_WhiteTexture, Wave_Bound, COLOR_GRAY( 0.25f ) );
                                
                                vec2 Pos = GetT( Wave_Bound, Vec2( 0.5f, 0.5f ) );
                                
                                DrawString( Pass_Game, AppState->FontList + FontID_0_75, "Enable Level Timer", WorldToUIPos( AppState, Camera, Pos ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            } else {
                                DrawRect( Pass_Game, Wave.Layout_TextureID, Wave_Bound, COLOR_WHITE );
                            }
                            if( DoesIntersectWave[ iWave ] ) {
                                DrawRect( Pass_Game, Wave_Bound, Vec4( 0.25f, 0.0f, 0.0f, 0.25f ) );
                            }
                            DrawRectOutline( Pass_Game, Wave_Bound, OutlineColor[ iWave ] );
                        }
                        
                        if( WaveS->nWave > 0 ) {
                            WAVE StartingWave = WaveS->Wave[ WaveS->StartingWave ];
                            
                            rect Wave_Bound = RectCD( StartingWave.Layout_Position, Wave_Dim * 1.1f );
                            DrawRectOutline( Pass_Game, Wave_Bound, COLOR_GREEN );
                        }
                        if( WaveS->CurrentWave > 0 ) {
                            WAVE CurrentWave = WaveS->Wave[ WaveS->CurrentWave ];
                            
                            rect Wave_Bound = RectCD( CurrentWave.Layout_Position, Wave_Dim * 1.05f );
                            DrawRectOutline( Pass_Game, Wave_Bound, COLOR_CYAN );
                        }
                        
                        if( ( iHover > -1 ) && ( !iHover_IsSetLink ) && ( !iHover_IsKillLink ) ) {
                            WAVE Wave       = WaveS->Wave[ iHover ];
                            rect Wave_Bound = RectCD( Wave.Layout_Position, Wave_Dim );
                            
                            DrawRect( Pass_Game, Wave_Bound, Vec4( 0.0f, 0.0f, 0.0f, 0.75f ) );
                            for( uint32 iEntity = 0; iEntity < Wave.nEntity; iEntity++ ) {
                                ENTITY Entity = Wave.Entity[ iEntity ];
                                
                                vec2  World_Dim     = GetDim( Editor->World_Bound );
                                vec2  World_HalfDim = World_Dim * 0.5f;
                                
                                vec2 t = ( Entity.Position + World_HalfDim ) / World_Dim;
                                
                                vec2 Pos = GetT( Wave_Bound, t );
                                
                                char Str[ 8 ] = {};
                                sprintf( Str, "%d", Entity.nEntity );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_25, Str, Pos, TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            }
                            
                            { // Exit Type Tooltip
                                EXIT_TYPE_LABELS;
                                
                                vec2 Pos   = GetTR( Wave_Bound ) + Vec2( 0.2f, 0.0f ) * Camera->Scale_Ratio;
                                vec2 Dim   = Vec2( 8.0f, 3.0f ) * Camera->Scale_Ratio;
                                rect Bound = RectTLD( Pos, Dim );
                                
                                flo32 OffsetY = 0.6f * Camera->Scale_Ratio;
                                
                                DrawRect( Pass_Game, Bound, Vec4( 0.0f, 0.0f, 0.0f, 0.75f ) );
                                
                                char Str[ 128 ] = {};
                                
                                sprintf( Str, "Exit : %s", ExitTypeLabels[ Wave.Exit ] );
                                DrawString( Pass_Game, AppState->FontList + FontID_1_25, Str, Pos + Vec2( 0.0f, 0.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                
                                if( Wave.InDelay_TargetTime > 0.0f ) {
                                    sprintf( Str, "In Delay : %.01fs", Wave.InDelay_TargetTime );
                                } else {
                                    sprintf( Str, "In Delay : ------" );
                                }
                                DrawString( Pass_Game, AppState->FontList + FontID_1_25, Str, Pos + Vec2( 0.0f, -OffsetY * 1.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                
                                if( ( Wave.Exit == ExitType_TargetTime ) || ( Wave.Exit == ExitType_BayDoorEventIsComplete ) ) {
                                    sprintf( Str, "Target   : %.01fs", Wave.Exit_TargetTime );
                                } else {
                                    sprintf( Str, "Target   : ------" );
                                }
                                DrawString( Pass_Game, AppState->FontList + FontID_1_25, Str, Pos + Vec2( 0.0f, -OffsetY * 2.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                
                                if( Wave.OutDelay_TargetTime > 0.0f ) {
                                    sprintf( Str, "Out Delay: %.01fs", Wave.OutDelay_TargetTime );
                                } else {
                                    sprintf( Str, "Out Delay: ------" );
                                }
                                DrawString( Pass_Game, AppState->FontList + FontID_1_25, Str, Pos + Vec2( 0.0f, -OffsetY * 3.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                                
                                if( Wave.nRepeatWave > 0 ) {
                                    sprintf( Str, "nRepeat  : %d (%d Total)", Wave.nRepeatWave, Wave.nRepeatWave + 1 );
                                } else if( Wave.nRepeatWave <= -1 ) {
                                    sprintf( Str, "nRepeat  : Infinite Loop" );
                                } else {
                                    sprintf( Str, "nRepeat  : ------" );
                                }
                                DrawString( Pass_Game, AppState->FontList + FontID_1_25, Str, Pos + Vec2( 0.0f, -OffsetY * 4.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
                            }
                        }
                        
                        _PopArray( TempMemory, boo32, WaveS->nWave );
                        _PopArray( TempMemory, vec4,  WaveS->nWave );
                    }
                    
                    if( Editor->ShowLayout_NextLink ) {
                        WAVE Src = WaveS->Wave[ Editor->ShowLayout_NextLink_iWave ];
                        
                        vec2 A = Src.Layout_Position;
                        vec2 B = MouseP;
                        DrawLine( Pass_Game, A, B, COLOR_CYAN );
                    }
                    if( Editor->ShowLayout_KillLink ) {
                        WAVE Src = WaveS->Wave[ Editor->ShowLayout_KillLink_iWave ];
                        
                        vec2 A = Src.Layout_Position;
                        vec2 B = MouseP;
                        DrawLine( Pass_Game, A, B, COLOR_MAGENTA );
                    }
                    
                    if( Editor->ShowLayout_DoGroupMove ) {
                        vec2 A = Editor->ShowLayout_DoGroupMove_bPos;
                        vec2 B = MouseP;
                        
                        DrawLine( Pass_Game, A, B, Vec4( 0.25f, 0.25f, 0.0f, 1.0f ) );
                        
                        rect Bound = Editor->ShowLayout_DoGroupMove_Bound;
                        DrawRectOutline( Pass_Game, Bound, COLOR_GRAY( 0.15f ) );
                        
                        Bound = AddOffset( Bound, B - A );
                        DrawRectOutline( Pass_Game, Bound, COLOR_YELLOW );
                    } else if( Editor->ShowLayout_DoGroupMove_DoSet ) {
                        vec2 A = Editor->ShowLayout_DoGroupMove_bPos;
                        vec2 B = MouseP;
                        
                        rect Bound = RectExtrema( A, B );
                        DrawRectOutline( Pass_Game, Bound, COLOR_GRAY( 0.5f ) );
                    } else {
                        rect Bound = Editor->ShowLayout_DoGroupMove_Bound;
                        
                        if( ( iHover == -1 ) && ( IsInRect( MouseP, Bound ) ) ) {
                            DrawRectOutline( Pass_Game, Bound, COLOR_GRAY( 0.5f ) );
                            DrawRect       ( Pass_Game, Bound, Vec4( 0.05f, 0.05f, 0.05f, 0.05f ) );
                        } else {
                            DrawRectOutline( Pass_Game, Bound, COLOR_GRAY( 0.25f ) );
                            DrawRect       ( Pass_Game, Bound, Vec4( 0.025f, 0.025f, 0.025f, 0.025f ) );
                        }
                    }
                }
            } else { // Add Entities
                CAMERA_STATE * Camera = &Editor->Camera_WaveEditor;
                UpdateCamera( Camera, Keyboard, dT );
                if( WasPressed( Keyboard, KeyCode_c, KEYBOARD_FLAGS__CONTROL ) ) {
                    InitCameraForWaveEditor( AppState );
                }
                
                vec2 MouseP = GetWorldPos( Camera, AppState->App_Dim, Mouse->Position );
                DISPLAY_VALUE( vec2, MouseP );
                
                { // Draw World Bound
                    DrawRectOutline( Pass_Game, Editor->World_Bound, COLOR_RED );
                    DISPLAY_VALUE( rect, Editor->World_Bound );
                }
                
                flo32 Scale   = 0.5f;
                flo32 OffsetX = ( MouseP.x < 0.0f ) ? -0.5f : 0.5f;
                flo32 OffsetY = ( MouseP.y < 0.0f ) ? -0.5f : 0.5f;
                flo32 SnapX   = ( flo32 )( ( int32 )( ( MouseP.x / Scale ) + OffsetX ) ) * Scale;
                flo32 SnapY   = ( flo32 )( ( int32 )( ( MouseP.y / Scale ) + OffsetY ) ) * Scale;
                vec2  SnapP   = Vec2( SnapX, SnapY );
                DrawPoint( Pass_Game, SnapP, Vec2( 0.1f, 0.1f ), COLOR_GRAY( 0.5f ) );
                DISPLAY_VALUE( vec2, SnapP );
                
                MouseP = SnapP;
                
                PANEL * Panel = &Editor->Editor_Panel;
                if( WasPressed( Keyboard, KeyCode_h, KEYBOARD_FLAGS__CONTROL ) ) {
                    Panel->Show = !Panel->Show;
                }
                
                rect View_Bound  = AppState->App_Bound;
                if( Panel->Show ) {
                    View_Bound.Right = Panel->Bound.Left;
                }
                
                WAVE * CurrentWave = WaveS->Wave + WaveS->CurrentWave;
                
                DISPLAY_VALUE( flo32, CurrentWave->Exit_TargetTime );
                DISPLAY_VALUE( uint32, CurrentWave->nEntity );
                
                ENTITY_TYPE iHover_Type = ( ENTITY_TYPE )0;
                int32       iHover      = -1;
                
                int32   EntityPos_nEntity = Editor->EntityTemplate_nEntity;
                vec2  * EntityPos         = _PushArray( TempMemory, vec2,  EntityPos_nEntity );
                boo32 * EntityPos_IsValid = _PushArray( TempMemory, boo32, EntityPos_nEntity );
                for( int32 iEntityPos = 0; iEntityPos < EntityPos_nEntity; iEntityPos++ ) {
                    EntityPos_IsValid[ iEntityPos ] = true;
                }
                
                boo32 * Entity_IsSelected = _PushArray_Clear( TempMemory, boo32, WAVE_ENTITY_MAX_COUNT );
                if( ( Editor->Mode == EditorMode_Move ) || ( Editor->Mode == EditorMode_Copy ) ) {
                    memcpy( Entity_IsSelected, WaveS->Entity_IsSelected, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
                }
                
                if( IsInRect( Mouse->Position, View_Bound ) ) {
                    boo32 Mouse_IsActive = IsDown( Mouse, MouseButton_Left );
                    
                    if( Mouse_IsActive ) {
                        CanSaveOpen              = false;
                        SaveOpen->UnsavedChanges = true;
                    }
                    
                    switch( CurrentWave->Type ) {
                        case WaveType_SpawnEntities: {
                            for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                                ENTITY * Entity = CurrentWave->Entity + iEntity;
                                
                                flo32 Radius = Entity->Radius + 0.1f;
                                rect  Bound  = RectCR( Entity->Position, Radius );
                                if( IsInRect( MouseP, Bound ) ) {
                                    iHover      = iEntity;
                                    iHover_Type = Entity->Type;
                                }
                            }
                            
                            if( ( Editor->Mode == EditorMode_Move ) || ( Editor->Mode == EditorMode_Copy ) ) {
                                boo32 CanClear = ( iHover == -1 )
                                    || ( ( iHover > -1 ) && ( !WaveS->Entity_IsSelected[ iHover ] ) );
                                
                                if( ( CanClear ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                                    memset( WaveS->Entity_IsSelected, 0, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
                                    memset(        Entity_IsSelected, 0, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
                                }
                                if( iHover == -1 ) {
                                    if( ( WasPressed( Mouse, MouseButton_Left ) ) || ( WasPressed( Mouse, MouseButton_Left, KEYBOARD_FLAGS__SHIFT ) ) ) {
                                        Editor->DoBoxSelect_Add  = true;
                                        Editor->Mouse_bPos = MouseP;
                                    }
                                    if( ( WasPressed( Mouse, MouseButton_Left, KEYBOARD_FLAGS__CONTROL ) ) ) {
                                        Editor->DoBoxSelect_Remove = true;
                                        Editor->Mouse_bPos   = MouseP;
                                    }
                                }
                                if( iHover > -1 ) {
                                    if( WasPressed( Mouse, MouseButton_Left ) ) {
                                        WaveS->Entity_IsSelected[ iHover ] = true;
                                        
                                        Editor->Mouse_bPos = MouseP;
                                        if( Editor->Mode == EditorMode_Move ) {
                                            Editor->DoMove = true;
                                        }
                                        if( Editor->Mode == EditorMode_Copy ) {
                                            Editor->DoCopy = true;
                                        }
                                    }
                                    if( ( WasPressed( Mouse, MouseButton_Left, KEYBOARD_FLAGS__SHIFT ) ) || ( WasPressed( Mouse, MouseButton_Left, KEYBOARD_FLAGS__CONTROL ) ) ) {
                                        WaveS->Entity_IsSelected[ iHover ] = !WaveS->Entity_IsSelected[ iHover ];
                                    }
                                }
                                
                                rect Bound = RectExtrema( MouseP, Editor->Mouse_bPos );
                                for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                                    ENTITY Entity = CurrentWave->Entity[ iEntity ];
                                    
                                    rect R = RectCR( Entity.Position, Entity.Radius + 0.1f );
                                    if( DoesRectIntersectRectInc( Bound, R ) ) {
                                        if( Editor->DoBoxSelect_Add ) {
                                            Entity_IsSelected[ iEntity ] = true;
                                        }
                                        if( Editor->DoBoxSelect_Remove ) {
                                            Entity_IsSelected[ iEntity ] = false;
                                        }
                                    }
                                }
                                
                                if( ( Editor->DoBoxSelect_Add ) || ( Editor->DoBoxSelect_Remove ) ) {
                                    if( WasReleased( Mouse, MouseButton_Left ) ) {
                                        Editor->DoBoxSelect_Add    = false;
                                        Editor->DoBoxSelect_Remove = false;
                                        memcpy( WaveS->Entity_IsSelected, Entity_IsSelected, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
                                    }
                                    if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                        Editor->DoBoxSelect_Add    = false;
                                        Editor->DoBoxSelect_Remove = false;
                                    }
                                }
                                
                                boo32 CanDelete = ( !Editor->DoBoxSelect_Add )
                                    && ( !Editor->DoBoxSelect_Remove )
                                    && ( !Editor->DoMove )
                                    && ( !Editor->DoCopy );
                                
                                if( ( CanDelete ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                                    CaptureUndo( Editor );
                                    
                                    uint32 NewCount = 0;
                                    for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                                        if( !WaveS->Entity_IsSelected[ iEntity ] ) {
                                            CurrentWave->Entity[ NewCount++ ] = CurrentWave->Entity[ iEntity ];
                                        }
                                    }
                                    CurrentWave->nEntity = NewCount;
                                    
                                    memset( WaveS->Entity_IsSelected, 0, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
                                }
                                
                                if( Editor->DoMove ) {
                                    if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                        Editor->DoMove = false;
                                    }
                                    if( WasReleased( Mouse, MouseButton_Left ) ) {
                                        // TODO: Prevent movement if entity would be placed on top of another entity!
                                        Editor->DoMove = false;
                                        
                                        vec2 Offset = MouseP - Editor->Mouse_bPos;
                                        
                                        for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                                            if( WaveS->Entity_IsSelected[ iEntity ] ) {
                                                ENTITY * Entity = CurrentWave->Entity + iEntity;
                                                Entity->Position += Offset;
                                            }
                                        }
                                    }
                                }
                                if( Editor->DoCopy ) {
                                    if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                        Editor->DoCopy = false;
                                    }
                                    if( WasReleased( Mouse, MouseButton_Left ) ) {
                                        // TODO: Prevent movement if entity would be placed on top of another entity!
                                        Editor->DoCopy = false;
                                        
                                        vec2 Offset = MouseP - Editor->Mouse_bPos;
                                        
                                        uint32 nEntity = CurrentWave->nEntity;
                                        for( uint32 iEntity = 0; iEntity < nEntity; iEntity++ ) {
                                            if( WaveS->Entity_IsSelected[ iEntity ] ) {
                                                ENTITY Entity = CurrentWave->Entity[ iEntity ];
                                                Entity.Position += Offset;
                                                
                                                AddEntity( Editor, Entity );
                                                
                                                WaveS->Entity_IsSelected[ iEntity                  ] = false;
                                                WaveS->Entity_IsSelected[ CurrentWave->nEntity - 1 ] = true;
                                            }
                                        }
                                    }
                                }
                                
                                if( Mouse->WheelClicks != 0 ) {
                                    for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                                        if( WaveS->Entity_IsSelected[ iEntity ] ) {
                                            ENTITY Entity   = CurrentWave->Entity[ iEntity ];
                                            Entity.nEntity += Mouse->WheelClicks;
                                            Entity.nEntity  = MaxValue( Entity.nEntity, 1 );
                                            
                                            CurrentWave->Entity[ iEntity ] = GenValIdentity( Entity.WaveGuid, Entity.Type, Entity.Position, Entity.nEntity, Entity.Radius, Entity.Shield );
                                        }
                                    }
                                }
                            }
                            
                            if( Editor->Mode == EditorMode_Add ) {
                                if( iHover == -1 ) {
                                    switch( Editor->EntityTemplate_Mode ) {
                                        case EntityTemplateMode_Line: {
                                            vec2 bPos = MouseP;
                                            vec2 Offset = ToDirection2D( Editor->EntityTemplate_Radians ) * Editor->EntityTemplate_Spacing;
                                            for( int32 iEntityPos = 0; iEntityPos < EntityPos_nEntity; iEntityPos++ ) {
                                                vec2 P = bPos + Offset * ( flo32 )iEntityPos;
                                                // TODO: Round P to nearest snap Scale! If we leave Radians at the current 45 degree values, then we should remove Radians and use spacing along the x and y axes. (eg. 45 degrees would use spacing along both the x and y axis, 90 degrees would only use spacing along the y axis)
                                                EntityPos[ iEntityPos ] = P;
                                            }
                                        } break;
                                        
                                        case EntityTemplateMode_Circle: {
                                            vec2  bPos     = MouseP;
                                            flo32 dRadians = ( 2.0f * PI ) / ( flo32 )Editor->EntityTemplate_nEntity;
                                            
                                            for( int32 iEntityPos = 0; iEntityPos < EntityPos_nEntity; iEntityPos++ ) {
                                                vec2 V = ToDirection2D( Editor->EntityTemplate_Radians + dRadians * ( flo32 )iEntityPos );
                                                // TODO: Round P to nearest snap Scale!
                                                vec2 P = bPos + V * Editor->EntityTemplate_Spacing;
                                                EntityPos[ iEntityPos ] = P;
                                            }
                                        } break;
                                    }
                                    
                                    { // Validate entity positions
                                        // TODO: Draw potential entities? Meaning draw entities with some transparency.
                                        // TODO: How should I invalidate entities against other potential entities? Should I just check spacing against the radius and invalidate the whole group?
                                        ENTITY LastEntity = Editor->Entity_LastEntity;
                                        ENTITY EntityA = GenValIdentity( CurrentWave->Guid, LastEntity.Type, Vec2( 0.0f, 0.0f ), LastEntity.nEntity, LastEntity.Radius );
                                        for( int32 iEntityPos = 0; iEntityPos < EntityPos_nEntity; iEntityPos++ ) {
                                            vec2 P = EntityPos[ iEntityPos ];
                                            
                                            for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                                                ENTITY EntityB = CurrentWave->Entity[ iEntity ];
                                                
                                                flo32 Radius   = EntityA.Radius + EntityB.Radius;
                                                flo32 RadiusSq = Radius * Radius;
                                                flo32 DistSq   = GetLengthSq( EntityB.Position - P );
                                                if( DistSq <= RadiusSq ) {
                                                    EntityPos_IsValid[ iEntityPos ] = false;
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                                    for( int32 iEntityPos = 0; iEntityPos < EntityPos_nEntity; iEntityPos++ ) {
                                        if( EntityPos_IsValid[ iEntityPos ] ) {
                                            ENTITY LastEntity = Editor->Entity_LastEntity;
                                            ENTITY Entity = GenValIdentity( CurrentWave->Guid, LastEntity.Type, EntityPos[ iEntityPos ], LastEntity.nEntity, LastEntity.Radius, LastEntity.Shield );
                                            AddEntity( Editor, Entity );
                                        }
                                    }
                                }
                                if( ( iHover > -1 ) && ( WasPressed( Mouse, MouseButton_Right ) ) ) {
                                    ENTITY Src = CurrentWave->Entity[ iHover ];
                                    
                                    ENTITY * LastEntity = &Editor->Entity_LastEntity;
                                    LastEntity->Type = ( ENTITY_TYPE )UInt32Wrap( Src.Type, 1, EntityType_Count );
                                    while( ( LastEntity->Type != EntityType_Turret    ) && (  LastEntity->Type != EntityType_Chaser    ) &&
                                          (  LastEntity->Type != EntityType_Swarm     ) &&
                                          (  LastEntity->Type != EntityType_Drifter   ) &&
                                          (  LastEntity->Type != EntityType_Looper    ) &&
                                          (  LastEntity->Type != EntityType_Starburst ) &&
                                          (  LastEntity->Type != EntityType_Miner     ) &&
                                          (  LastEntity->Type != EntityType_Bomber    ) && (  LastEntity->Type != EntityType_Bumper    ) && (  LastEntity->Type != EntityType_Bank      ) ) {
                                        LastEntity->Type = ( ENTITY_TYPE )UInt32Wrap( LastEntity->Type, 1, EntityType_Count );
                                    }
                                    
                                    CurrentWave->Entity[ iHover ] = GenValIdentity( CurrentWave->Guid, LastEntity->Type, Src.Position, Src.nEntity, LastEntity->Radius );
                                }
                                if( ( iHover > -1 ) && ( Mouse->WheelClicks != 0 ) ) {
                                    ENTITY Entity = CurrentWave->Entity[ iHover ];
                                    switch( Editor->EntityMouseWheel_Mode ) {
                                        case EntityMouseWheelMode_nEntity: {
                                            Entity.nEntity += Mouse->WheelClicks;
                                            Entity.nEntity  = MaxValue( Entity.nEntity, 1 );
                                            
                                            Entity = GenValIdentity( Entity.WaveGuid, Entity.Type, Entity.Position, Entity.nEntity, Entity.Radius, Entity.Shield );
                                            
                                            ENTITY * LastEntity = &Editor->Entity_LastEntity;
                                            LastEntity->nEntity = Entity.nEntity;
                                        } break;
                                        
                                        case EntityMouseWheelMode_TargetRadiusOrHealth: {
                                            Entity.Radius += ( flo32 )Mouse->WheelClicks;
                                            
                                            Entity = GenValIdentity( Entity.WaveGuid, Entity.Type, Entity.Position, Entity.nEntity, Entity.Radius, Entity.Shield );
                                            
                                            ENTITY * LastEntity = &Editor->Entity_LastEntity;
                                            LastEntity->Radius = Entity.Radius;
                                        } break;
                                    }
                                    
                                    CurrentWave->Entity[ iHover ] = Entity;
                                }
                                if( ( iHover == -1 ) && ( Mouse->WheelClicks != 0 ) ) {
                                    Editor->EntityTemplate_Spacing += ( ( flo32 )Mouse->WheelClicks * 0.5f );
                                    Editor->EntityTemplate_Spacing  = MaxValue( Editor->EntityTemplate_Spacing, 1.0f );
                                }
                                if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                                    CaptureUndo( Editor );
                                    
                                    CurrentWave->Entity[ iHover ] = CurrentWave->Entity[ --CurrentWave->nEntity ];
                                    iHover = -1;
                                }
                            }
                            
                            if( Editor->Mode == EditorMode_ModifyShield ) {
                                if( iHover > -1 ) {
                                    ENTITY * Entity = CurrentWave->Entity + iHover;
                                    if( Entity->Type == EntityType_Turret ) {
                                        if( WasPressed( Mouse, MouseButton_Left ) ) {
                                            Editor->Entity_iEntity = iHover;
                                        }
                                    } else {
                                        iHover = -1;
                                    }
                                }
                            }
                        } break;
                    }
                }
                
                { // Draw
                    //DrawRect( Pass_Game, TextureID_WaveEditorBackground_AutoTurret, Editor->World_Bound, COLOR_WHITE );
                    
                    boo32 * DoesIntersectEntity = _PushArray_Clear( TempMemory, boo32, CurrentWave->nEntity );
                    
                    for( uint32 iEntityA = 0; iEntityA < CurrentWave->nEntity; iEntityA++ ) {
                        ENTITY EntityA = CurrentWave->Entity[ iEntityA ];
                        flo32  RadiusA = EntityA.Radius + 0.1f;
                        rect   BoundA  = RectCR( EntityA.Position, RadiusA );
                        
                        for( uint32 iEntityB = iEntityA + 1; ( !DoesIntersectEntity[ iEntityA ] ) && ( iEntityB < CurrentWave->nEntity ); iEntityB++ ) {
                            ENTITY EntityB = CurrentWave->Entity[ iEntityB ];
                            flo32  RadiusB = EntityB.Radius + 0.1f;
                            rect   BoundB  = RectCR( EntityB.Position, RadiusB );
                            
                            if( DoesRectIntersectRectInc( BoundA, BoundB ) ) {
                                DoesIntersectEntity[ iEntityA ] = true;
                                DoesIntersectEntity[ iEntityB ] = true;
                            }
                        }
                    }
                    
                    if( Editor->Mode == EditorMode_ModifyShield ) {
                        for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                            ENTITY Entity = CurrentWave->Entity[ iEntity ];
                            Editor_DrawEntity( Pass_Game, AppState->Font, Camera, Entity, ( ( int32 )iEntity == iHover ), ( ( int32 )iEntity == Editor->Entity_iEntity ), DoesIntersectEntity[ iEntity ] );
                        }
                    } else {
                        for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                            ENTITY Entity = CurrentWave->Entity[ iEntity ];
                            Editor_DrawEntity( Pass_Game, AppState->Font, Camera, Entity, ( ( int32 )iEntity == iHover ), Entity_IsSelected[ iEntity ], DoesIntersectEntity[ iEntity ] );
                        }
                    }
                    
                    _PopArray( TempMemory, boo32, CurrentWave->nEntity );
                    
                    if( ( Editor->Mode != EditorMode_Move ) && ( Editor->Mode != EditorMode_Copy ) && ( IsInRect( Mouse->Position, View_Bound ) ) && ( iHover == -1 ) ) {
                        for( int32 iEntityPos = 0; iEntityPos < EntityPos_nEntity; iEntityPos++ ) {
                            vec4 Color = ( EntityPos_IsValid[ iEntityPos ] ) ? COLOR_YELLOW : COLOR_RED;
                            DrawPoint( Pass_Game, EntityPos[ iEntityPos ], Vec2( 0.25f, 0.25f ), Color );
                        }
                    }
                    
                    if( ( IsInRect( Mouse->Position, View_Bound ) ) ) {
                        char StrA[ 32 ] = {};
                        sprintf( StrA, "%.02f, %.02f", MouseP.x, MouseP.y );
                        
                        vec2 dPos = MouseP - Editor->Mouse_bPos;
                        
                        char StrB[ 32 ] = {};
                        sprintf( StrB, "%.02f, %.02f", dPos.x,   dPos.y );
                        
                        vec2 DimA = GetDimOfString( AppState->Font, StrA, -1, Vec2( 1.5f, 1.5f ) ).Dim;
                        vec2 DimB = GetDimOfString( AppState->Font, StrB, -1, Vec2( 1.5f, 1.5f ) ).Dim;
                        
                        vec2 Dim = DimA;
                        if( ( Editor->DoMove ) || ( Editor->DoCopy ) ) {
                            Dim.x = MaxValue( DimA.x, DimB.x );
                            Dim.y = DimA.y + DimB.y;
                        }
                        //Dim += Vec2( 0.1f, 0.1f );
                        
                        vec2 Max = Mouse->Position + Dim;
                        Max.x = MinValue( Max.x, View_Bound.Right );
                        Max.y = MinValue( Max.y, View_Bound.Top   );
                        
                        vec2 A = Max;
                        vec2 B = A + Vec2( 0.0f, -DimB.y );
                        
                        DrawString( Pass_UI, AppState->Font, StrA, A, TextAlign_TopRight, Vec2( 1.5f, 1.5f ), COLOR_YELLOW );
                        
                        if( ( Editor->DoMove ) || ( Editor->DoCopy ) ) {
                            DrawString( Pass_UI, AppState->Font, StrB, B, TextAlign_TopRight, Vec2( 1.5f, 1.5f ), COLOR_CYAN );
                        }
                    }
                    
                    if( ( Editor->DoBoxSelect_Add ) || ( Editor->DoBoxSelect_Remove ) ) {
                        rect Bound = RectExtrema( MouseP, Editor->Mouse_bPos );
                        DrawRectOutline( Pass_Game, Bound, COLOR_GRAY( 0.5f ) );
                    }
                    
                    if( ( Editor->DoMove ) || ( Editor->DoCopy ) ) {
                        DrawLine( Pass_Game, MouseP, Editor->Mouse_bPos, COLOR_GRAY( 0.5f ) );
                        
                        vec2 Offset = MouseP - Editor->Mouse_bPos;
                        
                        for( uint32 iEntity = 0; iEntity < CurrentWave->nEntity; iEntity++ ) {
                            if( WaveS->Entity_IsSelected[ iEntity ] ) {
                                ENTITY Entity = CurrentWave->Entity[ iEntity ];
                                
                                rect Bound = RectCR( Entity.Position + Offset, Entity.Radius + 0.1f );
                                DrawRectOutline( Pass_Game, Bound, Vec4( 0.25f, 0.25f, 0.0f, 1.0f ) );
                            }
                        }
                    }
                }
                
                UpdateAndDrawEditorPanel( AppState, Mouse, Pass_Game, Pass_UI, Editor->ShowLayout );
                
                _PopArray( TempMemory, boo32, WAVE_ENTITY_MAX_COUNT );
                _PopArray( TempMemory, boo32, EntityPos_nEntity );
                _PopArray( TempMemory, vec2,  EntityPos_nEntity );
            }
            
            boo32 DoOpen   = WasPressed( Keyboard, KeyCode_o, KEYBOARD_FLAGS__CONTROL );
            boo32 DoSave   = WasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL );
            boo32 DoSaveAs = WasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT );
            boo32 DoNew    = WasPressed( Keyboard, KeyCode_n, KEYBOARD_FLAGS__CONTROL );
            
            if( SaveOpen->CurrentFile[ 0 ] != 0 ) {
                char Str[ 128 ] = {};
                sprintf( Str, "FILE: %s", SaveOpen->CurrentFile );
                DISPLAY_STRING( Str );
            } else {
                DISPLAY_STRING( "NO FILENAME!!", COLOR_RED );
            }
            if( SaveOpen->UnsavedChanges ) {
                DISPLAY_STRING( "UNSAVED CHANGES!!", COLOR_RED );
            }
            if( SaveOpen->DoRecentSave ) {
                char Str[ 128 ] = {};
                sprintf( Str, "File Saved!! %s", SaveOpen->CurrentFile );
                DISPLAY_STRING( Str, COLOR_YELLOW );
                
                SaveOpen->Timer += dT;
                if( SaveOpen->Timer >= 2.0f ) {
                    SaveOpen->Timer        = 0.0f;
                    SaveOpen->DoRecentSave = false;
                }
            }
            
            if( CanSaveOpen ) {
                INPUT_STRING * InputString = &SaveOpen->InputString;
                if( DoSave ) {
                    if( SaveOpen->CurrentFile[ 0 ] != 0 ) {
                        SaveLevel( Platform, AppState, TempMemory );
                        SaveBitmaps( Platform, AppState, TempMemory );
                        
                        EDITOR_LoadBitmaps( Platform, AppState, TempMemory );
                    } else {
                        AppState->Mode = AppMode_SaveLevel;
                        AppState->Mode_IsInitialized = false;
                        SaveOpen->DoOverwrite = false;
                        SaveOpen->Select_iSelect = 0;
                        SaveOpen->Select_iFile   = -1;
                        InitInputString( InputString );
                    }
                } else if( DoSaveAs ) {
                    AppState->Mode = AppMode_SaveLevel;
                    AppState->Mode_IsInitialized = false;
                    SaveOpen->DoOverwrite   = false;
                    SaveOpen->Select_iSelect = 0;
                    SaveOpen->Select_iFile   = -1;
                    InitInputString( InputString );
                } else if( DoOpen ) {
                    AppState->Mode               = AppMode_OpenLevel;
                    AppState->Mode_IsInitialized = false;
                    SaveOpen->Select_iSelect = 0;
                    SaveOpen->Select_iFile   = -1;
                    InitInputString( InputString );
                }
            }
            
            if( DoNew ) {
                if( !SaveOpen->UnsavedChanges ) {
                    SaveOpen->ConfirmNew = true;
                }
                
                if( SaveOpen->ConfirmNew ) {
                    ResetEditor( Editor, AppState );
                    
                    Editor_NewWave( Editor );
                } else if( SaveOpen->UnsavedChanges ) {
                    SaveOpen->ConfirmNew = true;
                }
            }
            if( SaveOpen->ConfirmNew ) {
                DISPLAY_STRING( "Careful! File contains unsaved changes!!" );
            }
            
            if( WasPressed( Keyboard, KeyCode_g, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode = AppMode_Overworld;
                
                ZoomCameraToWorldBound( &AppState->Camera, AppState->App_Bound, AppState->App_Bound, AppState->UI_Bound );
                
                Mouse->ResetPos = false;
                AppState->Overworld_Cow.IsInitialized = false;
            }
            if( WasPressed( Keyboard, KeyCode_w, KEYBOARD_FLAGS__CONTROL ) ) {
                Editor_NewWave( Editor );
                ResetUndo( Editor );
            }
            if( WasPressed( Keyboard, KeyCode_Left ) ) {
                WaveS->CurrentWave = UInt32Wrap( WaveS->CurrentWave, -1, WaveS->nWave );
                
                if( WaveS->CurrentWave == 0 ) {
                    WaveS->CurrentWave = ( WaveS->nWave - 1 );
                }
                
                ResetUndo( Editor );
                memset( WaveS->Entity_IsSelected, 0, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
            }
            if( WasPressed( Keyboard, KeyCode_Right ) ) {
                WaveS->CurrentWave = UInt32Wrap( WaveS->CurrentWave,  1, WaveS->nWave );
                
                if( WaveS->CurrentWave == 0 ) {
                    WaveS->CurrentWave = 1;
                }
                
                ResetUndo( Editor );
                memset( WaveS->Entity_IsSelected, 0, sizeof( boo32 ) * WAVE_ENTITY_MAX_COUNT );
            }
        } break;
        
        case AppMode_SaveLevel: {
            EDITOR_STATE   * Editor   = &AppState->Editor;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !AppState->Mode_IsInitialized ) {
                AppState->Mode_IsInitialized = true;
                GetFileList( &SaveOpen->FileList, FILETAG__LEVEL );
            }
            
            INPUT_STRING * InputString = &SaveOpen->InputString;
            UpdateKeyboardInputForFileSaveOpen( InputString, Keyboard, SaveOpen );
            DrawAndUpdateFileListForSaveOpen( Pass_UI, Keyboard, AppState, SaveOpen );
            
            INPUT_STRING FileSelect_String = {};
            if( SaveOpen->Select_iFile > -1 ) {
                FileSelect_String.maxChar = INPUT_STRING__CHAR_MAX_COUNT;
                
                STRING_BUFFER * FileList = &SaveOpen->FileList;
                char          * FileName = FileList->str[ SaveOpen->Select_iFile ];
                strcpy( FileSelect_String.String, FileName );
                
                FileSelect_String.nChar = ( uint32 )strlen( FileSelect_String.String );
                
                InputString = &FileSelect_String;
            }
            
            if( ( WasPressed( Keyboard, KeyCode_Enter ) ) && ( InputString->nChar > 0 ) ) {
                if( SaveOpen->DoOverwrite ) {
                    strcpy( SaveOpen->CurrentFile, InputString->String );
                    SaveLevel( Platform, AppState, TempMemory );
                    SaveBitmaps( Platform, AppState, TempMemory );
                    
                    EDITOR_LoadBitmaps( Platform, AppState, TempMemory );
                } else {
                    boo32 FileExists = Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, InputString->String, FILETAG__LEVEL );
                    if( FileExists ) {
                        SaveOpen->DoOverwrite = true;
                    } else {
                        strcpy( SaveOpen->CurrentFile, InputString->String );
                        SaveLevel( Platform, AppState, TempMemory );
                        SaveBitmaps( Platform, AppState, TempMemory );
                        
                        EDITOR_LoadBitmaps( Platform, AppState, TempMemory );
                    }
                }
            }
            
            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = AppMode_Editor;
            }
        } break;
        
        case AppMode_OpenLevel: {
            EDITOR_STATE   * Editor   = &AppState->Editor;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !AppState->Mode_IsInitialized ) {
                AppState->Mode_IsInitialized = true;
                GetFileList( &SaveOpen->FileList, FILETAG__LEVEL );
            }
            
            INPUT_STRING * InputString = &SaveOpen->InputString;
            UpdateKeyboardInputForFileSaveOpen( InputString, Keyboard, SaveOpen );
            DrawAndUpdateFileListForSaveOpen( Pass_UI, Keyboard, AppState, SaveOpen );
            
            INPUT_STRING FileSelect_String = {};
            if( SaveOpen->Select_iFile > -1 ) {
                FileSelect_String.maxChar = INPUT_STRING__CHAR_MAX_COUNT;
                
                STRING_BUFFER * FileList = &SaveOpen->FileList;
                char          * FileName = FileList->str[ SaveOpen->Select_iFile ];
                strcpy( FileSelect_String.String, FileName );
                
                FileSelect_String.nChar = ( uint32 )strlen( FileSelect_String.String );
                
                InputString = &FileSelect_String;
            }
            
            if( ( WasPressed( Keyboard, KeyCode_Enter ) ) && ( InputString->nChar > 0 ) ) {
                boo32 FileExists = Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, InputString->String, FILETAG__LEVEL );
                if( FileExists ) {
                    AppState->Mode = AppMode_Editor;
                    EDITOR_LoadLevel( Platform, AppState, TempMemory, LEVEL_SAVE_DIRECTORY, InputString->String );
                    strcpy( SaveOpen->CurrentFile, InputString->String );
                    
                    EDITOR_LoadBitmaps( Platform, AppState, TempMemory );
                    
                    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
                    WaveS->CurrentWave = 1;
                    
                    InitCameraForWaveEditor  ( AppState );
                    InitCameraForLayoutEditor( AppState );
                    
                    ResetUndo( Editor );
                }
            }
            
            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = AppMode_Editor;
            }
        } break;
#endif
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    DISPLAY_VALUE( uint64, PermMemory->Used );
    DISPLAY_VALUE( uint64, TempMemory->Used );
    Assert( TempMemory->Used < TempMemory->Size );
    
    
#if 0    
    {
        int32 nSamples = AudioSystem->DebugBuffer_Size / ( AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT );
        flo32  Width  = GetWidth( AppState->UI_Bound ) / ( flo32 )nSamples;
        flo32 bHeight = 100.0f;
        
        vec2 AtPos = Vec2( 0.0f, GetHeight( AppState->UI_Bound ) * 0.5f );
        
        for( int32 iSample = 0; iSample < ( nSamples - 1 ); iSample++ ) {
            int16 Sample0 = ( ( int16 * )AudioSystem->DebugBuffer )[   iSample       * AUDIO__CHANNEL_COUNT ];
            int16 Sample1 = ( ( int16 * )AudioSystem->DebugBuffer )[ ( iSample + 1 ) * AUDIO__CHANNEL_COUNT ];
            
            flo32 tSample0 = ( flo32 )Sample0 / ( 32767.0f );
            flo32 tSample1 = ( flo32 )Sample1 / ( 32767.0f );
            
            vec2 P = AtPos + Vec2( 0.0f,  bHeight * tSample0 );
            vec2 Q = AtPos + Vec2( Width, bHeight * tSample1 );
            DrawLine( Pass_UI, P, Q, COLOR_RED );
            
            AtPos.x += Width;
        }
    }
#endif
}