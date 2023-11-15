
internal flo32
Explosive_GetSpawnTargetTime() {
    flo32 Result = RandomF32InRange( 2.0f, 8.0f );
    return Result;
}

internal EXPLOSIVE
ExplosiveC( vec2 Position, vec2 Direction ) {
    EXPLOSIVE Result = {};
    Result.Position  = Position;
    Result.Direction = Direction;
    Result.Speed     = EXPLOSIVE_SPEED;
    Result.Health    = 1;
    
    return Result;
}

internal void
AddExplosive( APP_STATE * AppState, EXPLOSIVE Explosive ) {
    EXPLOSIVE_STATE * ExplosiveS = &AppState->ExplosiveS;
    if( ExplosiveS->nExplosive < EXPLOSIVE_MAX_COUNT ) {
        ExplosiveS->Explosive[ ExplosiveS->nExplosive++ ] = Explosive;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add EXPLOSIVE to EXPLOSIVE_STATE: %u", EXPLOSIVE_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal EXPLOSIVE
NewExplosive( rect World_Bound ) {
    rect Spawn_Bound = AddRadius( World_Bound,  1.0f );
    rect Dest_Bound  = AddRadius( World_Bound, -2.0f );
    
    vec2 Pos  = GetRandomPointOnPerimeter( Spawn_Bound );
    vec2 Dest = GetRandomPoint           ( Dest_Bound  );
    vec2 Dir  = GetNormal( Dest - Pos );
    
    EXPLOSIVE Result = ExplosiveC( Pos, Dir );
    return Result;
}

internal void
UpdateExplosive( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE * Player = &AppState->Player;
    EXPLOSIVE_STATE   * ExplosiveS  = &AppState->ExplosiveS;
    
    if( ExplosiveS->Spawn_IsActive ) {
        ExplosiveS->Spawn_Timer += dT;
        if( ExplosiveS->Spawn_Timer >= ExplosiveS->Spawn_TargetTime ) {
            ExplosiveS->Spawn_Timer      = 0.0f;
            ExplosiveS->Spawn_TargetTime = Explosive_GetSpawnTargetTime();
            AddExplosive( AppState, NewExplosive( AppState->World_Bound ) );
        }
    }
    
    for( uint32 iExplosive = 0; iExplosive < ExplosiveS->nExplosive; iExplosive++ ) {
        EXPLOSIVE * Explosive = ExplosiveS->Explosive + iExplosive;
        vec2 dPos = Explosive->Direction * ( Explosive->Speed * dT );
        
        if( !Player->ZeroPoint_IsActive ) {
            Explosive->ZeroPoint_IsActive = false;
        }
        if( Player->IsDead ) {
            Explosive->ZeroPoint_ShowUI   = false;
        }
        if( Explosive->ZeroPoint_IsActive ) {
            Explosive->DoArmed = true;
        }
        
        if( Explosive->Health > 0 ) {
            if( Explosive->ZeroPoint_IsActive ) {
                Explosive->ZeroPoint_Radians = WrapRadians( Explosive->ZeroPoint_Radians - Player->Rotate_Speed * dT );
                
                vec2 Dir = ToDirection2D( Explosive->ZeroPoint_Radians );
                
                Explosive->Position  = Player->Position + Dir * Explosive->ZeroPoint_Dist;
                Explosive->Direction = -GetPerp( ToDirection2D( Explosive->ZeroPoint_Radians ) );
                Explosive->Speed     = Player->Rotate_Speed * Explosive->ZeroPoint_Dist;
            } else {
                Explosive->Position += dPos;
            }
            
            flo32 dT_Scale = 1.0f;
            if( Explosive->DoArmed ) {
                dT_Scale = EXPLOSIVE_ANIM_CYCLE_TARGET_TIME_UNARMED / EXPLOSIVE_ANIM_CYCLE_TARGET_TIME_ARMED;
            }
            
            Explosive->DoArmed_Timer += dT * dT_Scale;
            if( Explosive->DoArmed_Timer >= EXPLOSIVE_ANIM_CYCLE_TARGET_TIME_UNARMED ) {
                Explosive->DoArmed_Timer = 0.0f;
            }
        } else {
            Explosive->DoExplode_Timer += dT;
        }
    }
    //DISPLAY_VALUE( uint32, ExplosiveS->nExplosive );
}

internal void
DrawExplosive( RENDER_PASS * Pass, EXPLOSIVE Explosive, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    if( Explosive.Health > 0 ) {
        if( ( Explosive.ZeroPoint_ShowUI ) || ( Explosive.ZeroPoint_IsActive ) ) {
            vec4  Color  = Vec4( 0.0f, 0.5f, 0.5f, 1.0f );
            flo32 Radius = EXPLOSIVE_RADIUS + 0.1f;
            
            if( Explosive.ZeroPoint_IsActive ) {
                Color  = COLOR_CYAN;
                Radius = EXPLOSIVE_RADIUS + 0.2f;
            }
            
            DrawCircle( Pass, Explosive.Position, Radius, Color );
        }
        
        flo32 RadiusA = ( EXPLOSIVE_RADIUS        ) * Scale;
        flo32 RadiusB = ( EXPLOSIVE_RADIUS - 0.1f ) * Scale;
        
        vec4  ColorA  = COLOR_BLACK * Alpha;
        vec4  ColorB  = Vec4( 0.0f, 0.35f, 0.1f, 1.0f ) * Alpha;
        DrawCircle( Pass, Explosive.Position, RadiusA, ColorA );
        DrawCircle( Pass, Explosive.Position, RadiusB, ColorB );
        
        
        {
            int32 nSegment    = 5;
            flo32  Radians    = TAU / ( flo32 )nSegment;
            flo32  Offset     = EXPLOSIVE_RADIUS * 0.45f * Scale;
            vec4   Color[ 5 ] = {};
            
            if( Explosive.DoArmed ) {
                for( int32 iColor = 0; iColor < nSegment; iColor++ ) {
                    Color[ iColor ] = COLOR_RED;
                }
            } else {
                for( int32 iColor = 0; iColor < nSegment; iColor++ ) {
                    Color[ iColor ] = COLOR_BLACK;
                }
            }
            
            flo32 Cycle_TargetTime = EXPLOSIVE_ANIM_CYCLE_TARGET_TIME_UNARMED / ( flo32 )nSegment;
            int32 Select           = ( int32 )( Explosive.DoArmed_Timer / Cycle_TargetTime );
            
            if( Explosive.DoArmed ) {
                Color[ Select ] = COLOR_WHITE;
            } else {
                Color[ Select ] = COLOR_RED;
            }
            
            for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
                flo32 Rad = Radians * -( flo32 )iSegment;
                vec2  Dir = ToDirection2D( Rad );
                vec2  Pos = Explosive.Position + Dir * Offset;
                
                DrawCircle( Pass, Pos, 0.1f * Scale, Color[ iSegment ] * Alpha );
            }
        }
    } else {
        flo32 RadiusT = Clamp01( Explosive.DoExplode_Timer / EXPLOSIVE_EXPLODE_TARGET_TIME );
        flo32 Radius  = lerp( EXPLOSIVE_EXPLODE_RADIUS, RadiusT, EXPLOSIVE_EXPLODE_RADIUS * 1.2f );
        
        DrawCircle( Pass, Explosive.Position, Radius, COLOR_WHITE );
    }
}

internal void
DrawExplosive( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    EXPLOSIVE_STATE * ExplosiveS = Draw->ExplosiveS;
    
    for( uint32 iExplosive = 0; iExplosive < ExplosiveS->nExplosive; iExplosive++ ) {
        EXPLOSIVE Explosive = ExplosiveS->Explosive[ iExplosive ];
        
        DrawExplosive( Pass, Explosive );
    }
}

internal void
FinalizeExplosive( APP_STATE * AppState ) {
    EXPLOSIVE_STATE * ExplosiveS = &AppState->ExplosiveS;
    
    rect Remove_Bound = AddRadius( AppState->World_Bound, EXPLOSIVE_ZERO_POINT_RADIUS + 1.0f );
    
    uint32 NewCount = 0;
    for( uint32 iExplosive = 0; iExplosive < ExplosiveS->nExplosive; iExplosive++ ) {
        EXPLOSIVE Explosive = ExplosiveS->Explosive[ iExplosive ];
        
        boo32 DoKeep = ( IsInRect( Explosive.Position, Remove_Bound ) ) && ( ( Explosive.Health > 0 ) || ( Explosive.DoExplode_Timer < EXPLOSIVE_EXPLODE_TARGET_TIME ) );
        
        if( DoKeep ) {
            ExplosiveS->Explosive[ NewCount++ ] = Explosive;
        } else if( Explosive.ZeroPoint_IsActive ) {
            PLAYER_STATE * Player = &AppState->Player;
            Player->ZeroPoint_IsActive = false;
        }
    }
    ExplosiveS->nExplosive = NewCount;
}