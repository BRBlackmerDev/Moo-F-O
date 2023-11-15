
internal ENEMY_BULLET
EnemyBulletC( vec2 Position, vec2 Direction ) {
    ENEMY_BULLET Result = {};
    Result.Position  = Position;
    Result.Velocity  = Direction * ENEMY_BULLET_SPEED;
    Result.Health    = 1;
    
    return Result;
}

internal void
AddEnemyBullet( APP_STATE * AppState, ENEMY_BULLET EnemyBullet ) {
    ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
    if( EnemyBulletS->nEnemyBullet < ENEMY_BULLET_MAX_COUNT ) {
        EnemyBulletS->EnemyBullet[ EnemyBulletS->nEnemyBullet++ ] = EnemyBullet;
        
        AUDIO_ID AudioID = AudioID_EnemyBullet_01;
        if( RandomTF() ) {
            AudioID = ( AUDIO_ID )( ( int32 )AudioID + 1 );
        }
        flo32 Rate = 1.0f;
        PlaySoundEffect( AppState, AudioID, 2, ENEMY_BULLET_VOLUME, Rate );
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add ENEMY_BULLET to ENEMY_BULLET_STATE: %u", ENEMY_BULLET_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateEnemyBullet( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE       * Player       = &AppState->Player;
    ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
    
    for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
        ENEMY_BULLET * EnemyBullet = EnemyBulletS->EnemyBullet + iEnemyBullet;
        
        {
            flo32 dRange = ( 1.0f / 0.75f ) * dT;
            flo32 dColor = Clamp( EnemyBullet->tColorDest - EnemyBullet->tColor, -dRange, dRange );
            
            EnemyBullet->tColor += dColor;
            if( EnemyBullet->tColor == EnemyBullet->tColorDest ) {
                EnemyBullet->tColorDest = RandomF32InRange( -0.5f, 0.35f );
            }
        }
        
        EnemyBullet->IsTractorBeamed = false;
        if( Player->TractorBeam_IsActive ) {
            flo32 dSpeed      = ENEMY_BULLET_SPEED;
            vec2  DirToPlayer = GetNormal( Player->Position - EnemyBullet->Position );
            
            EnemyBullet->IsTractorBeamed = true;
            EnemyBullet->IsTractorBeamed_Radians = ToRadians( DirToPlayer );
            
            EnemyBullet->Velocity += DirToPlayer * ( dSpeed * dT );
            if( GetLength( EnemyBullet->Velocity ) >= ENEMY_BULLET_SPEED ) {
                EnemyBullet->Velocity = GetNormal( EnemyBullet->Velocity ) * ENEMY_BULLET_SPEED;
            }
        }
        
        vec2 dPos = EnemyBullet->Velocity * dT;
        EnemyBullet->Position += dPos;
    }
    
    //DISPLAY_VALUE( uint32, EnemyBulletS->nEnemyBullet );
}

internal void
DrawEnemyBullet( RENDER_PASS * Pass, ENEMY_BULLET EnemyBullet, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4 ColorA = ENEMY_BULLET_BASE_COLOR;
    vec4 ColorB = TowardWhite( ENEMY_BULLET_BASE_COLOR, EnemyBullet.tColor );
    if( EnemyBullet.tColor < 0.0f ) {
        ColorB = TowardBlack( ENEMY_BULLET_BASE_COLOR, fabsf( EnemyBullet.tColor ) );
    }
    
    rect Bound = RectCR( EnemyBullet.Position, ENEMY_BULLET_RADIUS * Scale );
    DrawRect( Pass, TextureID_EnemyBullet_Core,    Bound, ColorB * Alpha );
    DrawRect( Pass, TextureID_EnemyBullet_Surface, Bound, ColorA * Alpha );
    //DrawCircle( Pass, EnemyBullet.Position, ENEMY_BULLET_RADIUS * Scale, Color * Alpha );
    
#if 0    
    if( EnemyBullet.IsTractorBeamed ) {
        vec2 Dir = ToDirection2D( EnemyBullet.IsTractorBeamed_Radians );
        vec2 Pos = EnemyBullet.Position + Dir * 0.9f;
        vec2 Sca = Vec2( 0.15f, 0.2f );
        
        DrawTriangle( Pass, Pos, Sca, EnemyBullet.IsTractorBeamed_Radians - PI * 0.5f, Vec4( 0.0f, 0.65f, 0.65f, 1.0f ) );
    }
#endif
}

internal void
DrawEnemyBullet( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    ENEMY_BULLET_STATE * EnemyBulletS = Draw->EnemyBulletS;
    
    for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
        ENEMY_BULLET EnemyBullet = EnemyBulletS->EnemyBullet[ iEnemyBullet ];
        
        DrawEnemyBullet( Pass, EnemyBullet );
    }
}

internal void
FinalizeEnemyBullet( APP_STATE * AppState ) {
    ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
    
    rect Remove_Bound = AddRadius( AppState->World_Bound, 2.0f );
    
    uint32 NewCount = 0;
    for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
        ENEMY_BULLET EnemyBullet = EnemyBulletS->EnemyBullet[ iEnemyBullet ];
        
        if( EnemyBullet.Health <= 0 ) {
            Assert( ( EnemyBullet.DoShieldAbsorb ) || ( EnemyBullet.DoHitPointDrop ) || ( EnemyBullet.DoExplodePointDrop ) );
            Assert( ( EnemyBullet.DoShieldAbsorb + EnemyBullet.DoHitPointDrop + EnemyBullet.DoExplodePointDrop ) <= 1 );
            
            if( EnemyBullet.DoHitPointDrop ) {
                AddKillToScore( AppState, EntityType_EnemyBullet, EnemyBullet.Position, 1 );
                
                AddEntityDeathParticles( AppState, EnemyBullet.Position, ENEMY_BULLET_BASE_COLOR );
            }
            if( EnemyBullet.DoExplodePointDrop ) {
                AddKillToScore( AppState, EntityType_EnemyBullet, EnemyBullet.Position );
                
                POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_EnemyBullet );
                int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                
                vec2 Pos = ClampToRect( EnemyBullet.Position, AppState->World_Bound );
                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                    AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                }
                
                AddEntityDeathParticles( AppState, EnemyBullet.Position, COLOR_WHITE );
                AddEntityDeathParticles( AppState, EnemyBullet.Position, ENEMY_BULLET_BASE_COLOR );
            }
        }
        
        if( ( IsInRect( EnemyBullet.Position, Remove_Bound ) ) && ( EnemyBullet.Health > 0 ) ) {
            EnemyBulletS->EnemyBullet[ NewCount++ ] = EnemyBullet;
        }
    }
    EnemyBulletS->nEnemyBullet = NewCount;
}