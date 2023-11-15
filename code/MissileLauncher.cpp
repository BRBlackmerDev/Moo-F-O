
internal MISSILE_LAUNCHER
MissileLauncherC( int32 WaveGuid, vec2 Position, flo32 Radians ) {
    MISSILE_LAUNCHER Result = {};
    Result.Position     = Position;
    Result.Radius       = MISSILE_LAUNCHER_RADIUS;
    Result.Health       = MISSILE_LAUNCHER_HEALTH;
    Result.Radians      = Radians;
    
    return Result;
}

#if 0
internal void
AddMissileLauncher( APP_STATE * AppState, MISSILE_LAUNCHER MissileLauncher ) {
    MISSILE_LAUNCHER_STATE * MissileLauncherS = &AppState->MissileLauncherS;
    if( MissileLauncherS->nMissileLauncher < MISSILE_LAUNCHER_MAX_COUNT ) {
        MissileLauncherS->MissileLauncher[ MissileLauncherS->nMissileLauncher++ ] = MissileLauncher;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add MISSILE_LAUNCHER to MISSILE_LAUNCHER_STATE: %u", MISSILE_LAUNCHER_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateMissileLauncher( APP_STATE * AppState, flo32 dT ) {
    MISSILE_LAUNCHER_STATE * MissileLauncherS = &AppState->MissileLauncherS;
    
    for( int32 iMissileLauncher = 0; iMissileLauncher < MissileLauncherS->nMissileLauncher; iMissileLauncher++ ) {
        MISSILE_LAUNCHER * MissileLauncher = MissileLauncherS->MissileLauncher + iMissileLauncher;
        
        MissileLauncher->Launch_Timer += dT;
        flo32 TargetTime = 4.0f;
        if( MissileLauncher->Launch_Timer >= TargetTime ) {
            MissileLauncher->Launch_Timer -= TargetTime;
            
            flo32 RadiansA = MissileLauncher->Radians + PI * 5.0f / 6.0f;
            flo32 RadiansB = MissileLauncher->Radians - PI * 5.0f / 6.0f;
            
            vec2 DirA = ToDirection2D( RadiansA );
            vec2 DirB = ToDirection2D( RadiansB );
            
            vec2 PosA = MissileLauncher->Position + DirA * ( MISSILE_LAUNCHER_RADIUS * 0.8f );
            vec2 PosB = MissileLauncher->Position + DirB * ( MISSILE_LAUNCHER_RADIUS * 0.8f );
            
            flo32 Speed = MISSILE_SPEED;
            
            AddMissile( AppState, MissileC( -1, PosA, DirA * Speed ) );
            
            if( MissileLauncher->Health > 1 ) {
                AddMissile( AppState, MissileC( -1, PosB, DirB * Speed ) );
            }
        }
    }
}
#endif

internal void
DrawMissileLauncher( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, MISSILE_LAUNCHER MissileLauncher, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    
    {
        vec4 Barrel_Color       = TowardWhite( MISSILE_LAUNCHER_BASE_COLOR, 0.1f );
        vec4 Barrel_End_Color   = TowardBlack( MISSILE_LAUNCHER_BASE_COLOR, 0.2f );
        vec4 Launch_Color       = TowardWhite( MISSILE_LAUNCHER_BASE_COLOR, 0.35f );
        vec4 Stand_Color        = TowardBlack( MISSILE_LAUNCHER_BASE_COLOR, 0.55f );
        vec4 InsideBarrel_Color = COLOR_BLACK;
        
        vec4 Bound_Color  = MISSILE_LAUNCHER_BASE_COLOR;
        
        {
            flo32 tDamage = Clamp01( MissileLauncher.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
            Bound_Color = lerp( Bound_Color, tDamage, TowardWhite( Bound_Color, 0.5f ) );
        }
        
        vec2 bPos = MissileLauncher.Position;
        vec2 Dir  = ToDirection2D( MissileLauncher.Radians );
        
        flo32 Radius = MISSILE_LAUNCHER_RADIUS * Scale;
        
        // NOTE : Barrel_Pos is the back of the Missile Launcher.
        vec2  Barrel_Pos   = bPos - Dir * ( Radius * 0.8f );
        vec2  Barrel_Dim   = Vec2( 1.5f, 0.7f ) * Radius;
        orect Barrel_Bound = ORectTD( Barrel_Pos, Vec2( 0.0f, 0.5f ), Barrel_Dim, MissileLauncher.Radians );
        
        // Bound
        DrawRect( Pass, TextureID_RoundedDome, RectCR( bPos, Radius        ), Bound_Color * Alpha );
        DrawRect( Pass, TextureID_Dome,        RectCR( bPos, Radius * 0.7f ), TowardBlack( MISSILE_LAUNCHER_BASE_COLOR, 0.8f ) * Alpha );
        DrawRect( Pass, TextureID_Dome,        RectCR( bPos, Radius * 0.6f ), TowardBlack( Bound_Color, 0.1f ) * Alpha );
        
        // Stand
        DrawORect( Pass, ORectTD( GetT( Barrel_Bound, Vec2( 0.85f, 1.0f ) ), Vec2( 1.0f, 1.0f ), Barrel_Dim * Vec2( 0.85f, 0.6f ), MissileLauncher.Radians - ( PI / 12.0f ) ), Stand_Color * Alpha );
        DrawORect( Pass, ORectTD( GetT( Barrel_Bound, Vec2( 0.85f, 0.0f ) ), Vec2( 1.0f, 0.0f ), Barrel_Dim * Vec2( 0.85f, 0.6f ), MissileLauncher.Radians + ( PI / 12.0f ) ), Stand_Color * Alpha );
        
        vec2  Launch_Dim   = Barrel_Dim * Vec2( 0.25f, 1.15f );
        vec2  Launch_PosA  = Barrel_Pos + Dir * ( Barrel_Dim.x * 0.1f + Launch_Dim.x * 0.5f );
        vec2  Launch_PosB  = Barrel_Pos + Dir * ( Barrel_Dim.x * 0.9f - Launch_Dim.x * 0.5f );
        
        flo32 Launch_TargetTime = ( OBSERVATION_DECK_MISSILE_LAUNCHER_TARGET_TIME - MISSILE_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME );
        flo32 tLaunch = 1.0f - Clamp01( MissileLauncher.Launch_Timer / Launch_TargetTime );
        if( MissileLauncher.Launch_Timer < Launch_TargetTime ) {
            Barrel_End_Color = lerp( TowardWhite( Barrel_End_Color, 0.4f ), tLaunch, Barrel_End_Color );
        }
        if( MissileLauncher.Launch_Timer >= Launch_TargetTime ) {
            tLaunch = Clamp01( ( MissileLauncher.Launch_Timer - Launch_TargetTime ) / MISSILE_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME );
            Launch_Color = lerp( Launch_Color, tLaunch, TowardWhite( Launch_Color, 0.5f ) );
            InsideBarrel_Color = lerp( InsideBarrel_Color, tLaunch, TowardWhite( ENEMY_BULLET_BASE_COLOR, 0.35f ) );
            Barrel_End_Color = lerp( TowardWhite( Barrel_End_Color, 0.4f ), tLaunch, Barrel_End_Color );
        }
        vec2  Launch_Pos   = lerp( Launch_PosA, tLaunch, Launch_PosB );
        
        
        DrawORect( Pass, TextureID_MissileLauncher_Barrel_BackEnd, ORectTD( GetT( Barrel_Bound, Vec2( 0.4f, 0.0f ) ), Vec2( 0.0f, 0.5f ), Barrel_Dim * Vec2( 0.15f, 1.1f ), MissileLauncher.Radians - PI * 0.5f ), TowardWhite( Barrel_End_Color, 0.1f ) * Alpha );
        DrawORect( Pass, TextureID_MissileLauncher_Barrel_BackEnd, ORectTD( GetT( Barrel_Bound, Vec2( 0.4f, 1.0f ) ), Vec2( 0.0f, 0.5f ), Barrel_Dim * Vec2( 0.15f, 1.1f ), MissileLauncher.Radians + PI * 0.5f ), TowardWhite( Barrel_End_Color, 0.1f ) * Alpha );
        DrawORect( Pass, TextureID_MissileLauncher_Barrel, Barrel_Bound, Barrel_Color * Alpha );
        DrawORect( Pass, TextureID_MissileLauncher_Barrel_BackEnd, ORectTD( Barrel_Pos, Vec2( 0.0f, 0.5f ), Barrel_Dim * Vec2( 0.25f, 1.3f ), MissileLauncher.Radians + PI ), Barrel_End_Color * Alpha );
        DrawORect( Pass, TextureID_MissileLauncher_Barrel, ORectTD( Barrel_Pos, Vec2( 0.0f, 0.5f ), Barrel_Dim * Vec2( 0.05f, 1.3f ), MissileLauncher.Radians ), TowardBlack( Barrel_End_Color, 0.4f ) * Alpha );
        DrawORect( Pass, TextureID_MissileLauncher_Barrel, ORectCD( Launch_Pos, Launch_Dim, MissileLauncher.Radians ), Launch_Color * Alpha );
        DrawORect( Pass, TextureID_MissileLauncher_Barrel_FrontEnd, ORectCD( GetT( Barrel_Bound, Vec2( 1.0f, 0.5f ) ), Barrel_Dim * Vec2( 0.05f, 1.0f ), MissileLauncher.Radians ), TowardBlack( Barrel_Color, 0.5f ) * Alpha );
        DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( GetT( Barrel_Bound, Vec2( 1.0f, 0.5f ) ), Barrel_Dim * Vec2( 0.035f, 0.35f ), MissileLauncher.Radians + PI ), InsideBarrel_Color * Alpha );
        
        
        if( MissileLauncher.Health > 0 ) {
            vec2 Pos_Text = GetT( Barrel_Bound, Vec2( 0.0f, 0.5f ) );
            
            DrawORect( Pass, TextureID_MissileLauncher_Health, ORectCR( Pos_Text, 0.65f, MissileLauncher.Radians ), TowardBlack( MISSILE_LAUNCHER_BASE_COLOR, 0.75f ) );
            
            DrawEntityHealth( Pass, AppState, Camera, Pos_Text, MissileLauncher.Health, COLOR_WHITE );
        }
    }
}

#if 0
internal void
DrawMissileLauncher( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    MISSILE_LAUNCHER_STATE * MissileLauncherS = Draw->MissileLauncherS;
    
    for( int32 iMissileLauncher = 0; iMissileLauncher < MissileLauncherS->nMissileLauncher; iMissileLauncher++ ) {
        MISSILE_LAUNCHER MissileLauncher = MissileLauncherS->MissileLauncher[ iMissileLauncher ];
        
        DrawMissileLauncher( Pass, AppState, Camera, MissileLauncher );
    }
}
#endif

#if 0
internal void
FinalizeMissileLauncher( APP_STATE * AppState ) {
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    MISSILE_LAUNCHER_STATE * MissileLauncherS = &AppState->MissileLauncherS;
    
    { // MISSILE_LAUNCHER
        uint32 NewCount = 0;
        for( int32 iMissileLauncher = 0; iMissileLauncher < MissileLauncherS->nMissileLauncher; iMissileLauncher++ ) {
            MISSILE_LAUNCHER MissileLauncher = MissileLauncherS->MissileLauncher[ iMissileLauncher ];
            
            boo32 DoKeep = ( MissileLauncher.Health > 0 );
            
            if( DoKeep ) {
                MissileLauncherS->MissileLauncher[ NewCount++ ] = MissileLauncher;
            } else {
                AddBigEntityDeathParticles( AppState, MissileLauncher.Position, MissileLauncher.Radius, MISSILE_LAUNCHER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MissileLauncher.Position, MissileLauncher.Radius, MISSILE_LAUNCHER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MissileLauncher.Position, MissileLauncher.Radius, COLOR_WHITE, false );
                
                MissileLauncherS->BonusObjective_nMissileLauncherKills += 1;
            }
        }
        MissileLauncherS->nMissileLauncher = NewCount;
    }
}
#endif