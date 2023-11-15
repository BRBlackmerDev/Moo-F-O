
internal GUNNER
GunnerC( vec2 Position ) {
    GUNNER Result = {};
    Result.Position = Position;
    Result.Radius   = GUNNER_RADIUS;
    Result.Health   = GUNNER_HEALTH;
    
    Result.Move_TargetTime = RandomF32InRange( 0.15f, 2.0f );
    
    Result.Fire_Timer      = -GUNNER_FIRE_COOLDOWN_TARGET_TIME;
    Result.Fire_TargetTime = RandomF32InRange( 2.0f, 3.0f ) - GUNNER_FIRE_COOLDOWN_TARGET_TIME;
    
    Result.Kill_Counter    = 5;
    
    return Result;
}

internal void
AddGunner( APP_STATE * AppState, GUNNER Gunner ) {
    GUNNER_STATE * GunnerS = &AppState->GunnerS;
    if( GunnerS->nGunner < GUNNER_MAX_COUNT ) {
        GunnerS->Gunner[ GunnerS->nGunner++ ] = Gunner;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add GUNNER to GUNNER_STATE: %u", GUNNER_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateGunner( APP_STATE * AppState, flo32 dT ) {
    GUNNER_STATE * GunnerS = &AppState->GunnerS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    for( int32 iGunner = 0; iGunner < GunnerS->nGunner; iGunner++ ) {
        GUNNER * Gunner = GunnerS->Gunner + iGunner;
        
        Gunner->Damage_Timer = MaxValue( Gunner->Damage_Timer - dT, 0.0f );
        
        { // Collide Missiles with Gunners
            MISSILE_STATE * MissileS = &AppState->MissileS;
            for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
                MISSILE * Missile = MissileS->Missile + iMissile;
                
                if( IsInCircle( Missile->Position, Circ( Gunner->Position, Gunner->Radius ) ) ) {
                    Missile->Health = 0;
                    Gunner->Health  = MaxValue( Gunner->Health - 5, 0 );
                    
                    flo32 BounceSpeed = 20.0f;
                    Gunner->Velocity += GetNormal( Missile->Velocity ) * BounceSpeed;
                }
            }
            
        }
        
        if( Gunner->Health > 0 ) {
            { // Rotate Toward the Player
                flo32 dRadians = TAU / 2.0f;
                
                flo32 Dest = ToRadians( Player->Position - Gunner->Position );
                flo32 At   = Gunner->Radians;
                if( Dest != At ) {
                    flo32 Diff = GetSmallestDiffInRadians( At, Dest );
                    
                    flo32 dRadiansHi = ( dRadians * dT );
                    flo32 dRadiansLo = -dRadiansHi;
                    if( ( Diff < dRadiansLo ) || ( Diff > dRadiansHi ) ) {
                        At += Clamp( dRadiansLo, Diff, dRadiansHi );
                    } else {
                        At  = Dest;
                    }
                    
                    Gunner->Radians = WrapRadians( At );
                }
            }
            
            { // Fire Bolts at Player
                Gunner->Fire_Timer += dT;
                if( Gunner->Fire_Timer >= Gunner->Fire_TargetTime ) {
                    Gunner->Fire_Timer     -= ( Gunner->Fire_TargetTime + GUNNER_FIRE_COOLDOWN_TARGET_TIME );
                    Gunner->Fire_TargetTime = RandomF32InRange( 2.0f, 3.0f ) - GUNNER_FIRE_COOLDOWN_TARGET_TIME;
                    
                    vec2 Dir = ToDirection2D( Gunner->Radians );
                    vec2 Pos = Gunner->Position + Dir * ( Gunner->Radius * 0.6f );
                    
                    AddEnemyBullet( AppState, EnemyBulletC( Pos, Dir ) );
                }
            }
        } else {
            Gunner->Fire_Timer = 0.0f;
        }
        
        { // Update Repel
            for( int32 iRepel = iGunner + 1; iRepel < GunnerS->nGunner; iRepel++ ) {
                GUNNER * Repel = GunnerS->Gunner + iRepel;
                
                vec2 V = Repel->Position - Gunner->Position;
                
                flo32 DistSq   = GetLengthSq( V );
                flo32 Radius   = GUNNER_REPEL_RADIUS + GUNNER_REPEL_RADIUS;
                flo32 RadiusSq = Radius * Radius;
                if( DistSq <= RadiusSq ) {
                    vec2 Repel_Vector = GetNormal( V ) * GUNNER_REPEL_SPEED;
                    
                    Gunner->Repel -= Repel_Vector;
                    Repel->Repel  += Repel_Vector;
                }
            }
        }
        
        { // Update Move Timer
            rect BoundA = AddRadius( AppState->World_Bound, -1.0f );
            circ BoundB = Circ( Vec2( 0.0f, 100.0f ), 0.0f );
            {
                BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
                if( BoardRoomS->State_IsActive ) {
                    OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                    
                    flo32 Radius = ObservationDeck->Radius;
                    if( ObservationDeck->Shield_IsActive ) {
                        Radius += ObservationDeck->Shield_Dist;
                    }
                    
                    BoundB = Circ( ObservationDeck->Position, Radius );
                }
            }
            
            boo32 IsInBoundA = IsInRect  ( Gunner->Position, BoundA );
            boo32 IsInBoundB = IsInCircle( Gunner->Position, BoundB );
            
            flo32 TargetTime_Min = 0.25f;
            flo32 TargetTime_Max = 2.0f;
            
            Gunner->Move_Timer += dT;
            if( ( !IsInBoundA ) || ( IsInBoundB ) ) {
                if( Gunner->Move_Timer >= 0.0f ) {
                    Gunner->Move_Timer = Gunner->Move_TargetTime;
                }
            }
            
            if( Gunner->Move_Timer >= Gunner->Move_TargetTime ) {
                Gunner->Move_Timer -= Gunner->Move_TargetTime;
                if( Gunner->Move_DoMove ) {
                    Gunner->Move_TargetTime = RandomF32InRange( TargetTime_Min, TargetTime_Max );
                    Gunner->Move_DoMove     = false;
                } else {
                    Gunner->Move_TargetTime = ( ( flo32 )RandomS32InRange( 6, 12 ) / 60.0f );
                    Gunner->Move_DoMove     = true;
                    vec2 Dir                = RandomDirection2D();
                    
                    { // If far away from Player, then attempt to move toward the Player.
                        flo32 Radius   = 16.0f;
                        flo32 RadiusSq = Radius * Radius;
                        flo32 DistSq   = GetLengthSq( Player->Position - Gunner->Position );
                        if( DistSq > RadiusSq ) {
                            if( ( ( Gunner->Position.x < Player->Position.x ) && ( Dir.x < 0.0f ) ) || (  ( Gunner->Position.x > Player->Position.x ) && ( Dir.x > 0.0f ) ) ) {
                                Dir.x = -Dir.x;
                            }
                            if( ( ( Gunner->Position.y < Player->Position.y ) && ( Dir.y < 0.0f ) ) || (  ( Gunner->Position.y > Player->Position.y ) && ( Dir.y > 0.0f ) ) ) {
                                Dir.y = -Dir.y;
                            }
                            
                            //vec2 DirToPlayer = GetNormal( Player->Position - Gunner->Position );
                            //if( Dir != -DirToPlayer ) {
                            //Dir = GetNormal( Dir + DirToPlayer );
                            //}
                        }
                    }
                    
                    if( ( ( Gunner->Position.x < BoundA.Left  ) && ( Dir.x < 0.0f ) ) ||  (  ( Gunner->Position.x > BoundA.Right ) && ( Dir.x > 0.0f ) ) ) {
                        Dir.x = -Dir.x;
                    }
                    if( ( ( Gunner->Position.y < BoundA.Bottom ) && ( Dir.y < 0.0f ) ) || (  ( Gunner->Position.y > BoundA.Top    ) && ( Dir.y > 0.0f ) ) ) {
                        Dir.y = -Dir.y;
                    }
                    
                    { // Move out of the Observation Deck shield range.
                        if( IsInBoundB ) {
                            if( Dir.y > 0.0f ) {
                                Dir.y = -Dir.y;
                            }
                            
                            vec2 N = GetNormal( Gunner->Position - BoundB.Center );
                            N = GetNormal( N + Vec2( 0.0f, -1.0f ) );
                            if( Dir != -N ) {
                                Dir = GetNormal( Dir + N );
                            }
                        }
                    }
                    
                    Gunner->Move_Dir = Dir;
                }
            }
        }
        
        vec2  Dir      = {};
        flo32 Friction = 2.0f;
        
        if( ( Gunner->Health > 0 ) && ( Gunner->Move_DoMove ) ) {
            Dir = Gunner->Move_Dir;
            Friction = 8.0f;
        }
        vec2 Accel = ( Dir * GUNNER_MOVE_SPEED - Gunner->Velocity + Gunner->Repel ) * Friction;
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Gunner->Velocity * dT;
        // TODO: Collision!
        Gunner->Position += dPos;
        Gunner->Velocity += Accel * dT;
        
        Gunner->Repel = {};
        
        {
            if( Gunner->Health <= 0 ) {
                Gunner->Kill_Timer += dT;
                if( Gunner->Kill_Timer >= GUNNER_KILL_ANIM_TARGET_TIME ) {
                    Gunner->Kill_Timer   -= GUNNER_KILL_ANIM_TARGET_TIME;
                    Gunner->Kill_Counter -= 1;
                    
                    vec2  DirA = RandomDirection2D();
                    flo32 Dist = RandomF32InRange( 0.25f, 0.75f ) * Gunner->Radius;
                    vec2  Pos  = Gunner->Position + DirA * Dist;
                    AddMissileDeathParticles( AppState, Pos );
                }
            }
        }
    }
}

internal void
DrawGunner( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, GUNNER Gunner, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec2  Dir    = ToDirection2D( Gunner.Radians );
    flo32 Radius = Gunner.Radius * Scale;
    
    vec4  Bound_Color        = TowardBlack( GUNNER_BASE_COLOR, 0.2f );
    vec4  Ring_Color         = TowardBlack( GUNNER_BASE_COLOR, 0.8f );
    vec4  Mount_Color        = TowardBlack( GUNNER_BASE_COLOR, 0.5f );
    vec4  Barrel_Color       = TowardBlack( GUNNER_BASE_COLOR, 0.1f );
    vec4  InsideBarrel_Color = TowardBlack( GUNNER_BASE_COLOR, 0.9f );
    vec4  Glow_Color         = Vec4( 0.35f, 0.35f, 0.0f, 0.35f );
    
    flo32 tFire = 0.0f;
    {
        flo32 TargetTime = Gunner.Fire_TargetTime - ( 40.0f / 60.0f );
        if( Gunner.Fire_Timer >= TargetTime ) {
            tFire = Clamp01( ( Gunner.Fire_Timer - TargetTime ) / ( Gunner.Fire_TargetTime - TargetTime - ( 8.0f / 60.0f ) ) );
            
            Bound_Color        = lerp( Bound_Color,  tFire, TowardWhite( Bound_Color,  0.45f ) );
            Barrel_Color       = lerp( Barrel_Color, tFire, TowardWhite( Barrel_Color, 0.45f ) );
            InsideBarrel_Color = lerp( InsideBarrel_Color, tFire, TowardWhite( ENEMY_BULLET_BASE_COLOR, 0.35f ) );
        }
    }
    
    {
        flo32 tDamage = Clamp01( Gunner.Damage_Timer / ENTITY_SHOW_DAMAGE_TARGET_TIME );
        if( tDamage > tFire ) {
            Bound_Color = lerp( Bound_Color, tDamage, TowardWhite( Bound_Color, 0.35f ) );
        }
        Mount_Color = lerp( Mount_Color, tDamage, TowardWhite( Mount_Color, 0.2f ) );
    }
    
    { // Movement Glow
        vec2   Move_Dir = GetNormal( Gunner.Velocity );
        flo32  Speed    = GetLength( Gunner.Velocity );
        flo32 tSpeed    = Clamp01( Speed / GUNNER_MOVE_SPEED );
        
        vec2 Pos = Gunner.Position - Move_Dir * ( Radius * 1.35f * tSpeed );
        vec2 Dim = Vec2( 1.5f, 1.5f ) * Radius;
        DrawRect( Pass, TextureID_Glow, RectCD( Pos, Dim ), Glow_Color * Alpha );
    }
    
    { // Bound
        DrawRect( Pass, TextureID_RoundedDome, RectCR( Gunner.Position, Radius        ), Bound_Color * Alpha );
        DrawRect( Pass, TextureID_Dome,        RectCR( Gunner.Position, Radius * 0.8f ), Ring_Color  * Alpha  );
        DrawRect( Pass, TextureID_Dome,        RectCR( Gunner.Position, Radius * 0.7f ), Bound_Color * Alpha );
    }
    
    { // Mount
        flo32 bRadians  = Gunner.Radians + PI * 0.5f;
        flo32  Range    = PI / 6.0f;
        
        vec2  Mount_Dim = Vec2( 1.8f, 0.2f ) * ( Radius );
        vec2  Mount_Pos = Gunner.Position - Dir * ( Radius * 0.05f );
        
        DrawORect( Pass, TextureID_Barrel, ORectCD( Mount_Pos, Mount_Dim, bRadians - Range ), Mount_Color * Alpha );
        DrawORect( Pass, TextureID_Barrel, ORectCD( Mount_Pos, Mount_Dim, bRadians         ), Mount_Color * Alpha );
        DrawORect( Pass, TextureID_Barrel, ORectCD( Mount_Pos, Mount_Dim, bRadians + Range ), Mount_Color * Alpha );
    }
    
    { // Barrel
        vec2  Barrel_Pos   = Gunner.Position - Dir * ( Radius * 0.5f );
        
        vec2  Barrel_Dim   = Vec2( 1.4f, 1.4f ) * ( Radius );
        orect Barrel_Bound = ORectTD( Barrel_Pos, Vec2( 0.0f, 0.5f ), Barrel_Dim, Gunner.Radians );
        
        DrawORect( Pass, TextureID_Gunner_Barrel, Barrel_Bound, Barrel_Color * Alpha );
        DrawORect( Pass, TextureID_Barrel_End, ORectTD( GetT( Barrel_Bound, Vec2( 0.0f, 0.5f ) ), Vec2( 0.0f, 0.5f ), Vec2( Barrel_Dim.y * 0.5f, Barrel_Dim.y ), Gunner.Radians + PI ), Barrel_Color * Alpha );
        
        DrawEllipse( Pass, GetT( Barrel_Bound, Vec2( 1.0f, 0.5f ) ), Barrel_Dim * Vec2( 0.25f, 0.75f ) * 0.5f, Gunner.Radians, TowardBlack( Barrel_Color, 0.2f ) * Alpha );
        DrawORect( Pass, TextureID_Turret_InsideBarrel, ORectCD( GetT( Barrel_Bound, Vec2( 1.0f, 0.5f ) ), Barrel_Dim * Vec2( 0.15f, 0.65f ), Gunner.Radians + PI ), InsideBarrel_Color * Alpha );
        
        vec2 Text_Pos = GetT( Barrel_Bound, Vec2( 0.0f, 0.5f ) );
        
        DrawRect( Pass, TextureID_Dome, RectCR( Text_Pos, 0.55f * Scale ), TowardBlack( GUNNER_BASE_COLOR, 0.75f ) * Alpha );
        
        if( Gunner.Health > 0 ) {
            DrawEntityHealth( Pass, AppState, Camera, Text_Pos, Gunner.Health, COLOR_WHITE );
        }
    }
}

internal void
DrawGunner( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    GUNNER_STATE * GunnerS = Draw->GunnerS;
    
    for( int32 iGunner = 0; iGunner < GunnerS->nGunner; iGunner++ ) {
        GUNNER Gunner = GunnerS->Gunner[ iGunner ];
        
        DrawGunner( Pass, AppState, Camera, Gunner );
    }
}

internal void
FinalizeGunner( APP_STATE * AppState ) {
    SCORE_STATE   * ScoreS   = &AppState->ScoreS;
    GUNNER_STATE * GunnerS = &AppState->GunnerS;
    
    uint32 NewCount = 0;
    for( int32 iGunner = 0; iGunner < GunnerS->nGunner; iGunner++ ) {
        GUNNER Gunner = GunnerS->Gunner[ iGunner ];
        
        boo32 DoKeep = ( Gunner.Kill_Counter > 0 );
        
        if( DoKeep ) {
            GunnerS->Gunner[ NewCount++ ] = Gunner;
        } else {
            vec2 Pos = ClampToRect( Gunner.Position, AppState->World_Bound );
            
            AddKillToScore( AppState, EntityType_Gunner, Pos );
            
            POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Gunner );
            int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
            for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound ) );
            }
            
            AddEntityDeathParticles( AppState, Gunner.Position, GUNNER_BASE_COLOR );
            AddEntityDeathParticles( AppState, Gunner.Position, GUNNER_BASE_COLOR );
            AddEntityDeathParticles( AppState, Gunner.Position, COLOR_WHITE );
        }
    }
    GunnerS->nGunner = NewCount;
}