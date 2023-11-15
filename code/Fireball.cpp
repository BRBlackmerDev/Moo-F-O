
internal FIREBALL
FireballC_Small( int32 WaveGuid, FIREBALL_TYPE Type, vec2 Position, vec2 Direction ) {
    FIREBALL Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Type     = Type;
    Result.Position = Position;
    Result.Velocity = Direction * FIREBALL_SPEED_SMALL;
    Result.Radius   = FIREBALL_RADIUS_SMALL;
    Result.Health   = 1;
    
    return Result;
}

internal FIREBALL
FireballC_Large( int32 WaveGuid, FIREBALL_TYPE Type, vec2 Position, vec2 Direction ) {
    FIREBALL Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Type     = Type;
    Result.Position = Position;
    Result.Velocity = Direction * FIREBALL_SPEED_LARGE;
    Result.Radius   = FIREBALL_RADIUS_LARGE;
    Result.Health   = FIREBALL_HEALTH_LARGE;
    
    return Result;
}

internal void
AddFireball( APP_STATE * AppState, FIREBALL Fireball ) {
    FIREBALL_STATE * FireballS = &AppState->FireballS;
    if( FireballS->nFireball < FIREBALL_MAX_COUNT ) {
        FireballS->Fireball[ FireballS->nFireball++ ] = Fireball;
        
        AUDIO_ID AudioID = AudioID_Fireball_01;
        if( RandomTF() ) {
            AudioID = ( AUDIO_ID )( ( int32 )AudioID + 1 );
        }
        flo32 Rate = RandomF32InRange( 0.8f, 1.2f );
        PlaySoundEffect( AppState, AudioID, 2, FIREBALL_VOLUME, Rate );
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add FIREBALL to FIREBALL_STATE: %u", FIREBALL_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateFireball( APP_STATE * AppState, flo32 dT ) {
    FIREBALL_STATE  * FireballS = &AppState->FireballS;
    PLAYER_STATE * Player = &AppState->Player;
    
    for( int32 iFireball = 0; iFireball < FireballS->nFireball; iFireball++ ) {
        FIREBALL * Fireball = FireballS->Fireball + iFireball;
        
        {
            flo32 dRange = ( 1.0f / 2.0f ) * dT;
            flo32 dColor = Clamp( Fireball->tColorDest - Fireball->tColor, -dRange, dRange );
            
            Fireball->tColor += dColor;
            if( Fireball->tColor == Fireball->tColorDest ) {
                Fireball->tColorDest = RandomF32InRange( -0.15f, 0.15f );
            }
        }
        
        { // Update Movement
            vec2 dPos = Fireball->Velocity * dT;
            Fireball->Position += dPos;
        }
    }
}

internal void
DrawFireball( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, FIREBALL Fireball, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    
    {
        flo32 Radius = Fireball.Radius * Scale;
        flo32 ScaA = Radius * RandomF32InRange( 1.25f, 1.65f );
        flo32 ScaB = Radius * RandomF32InRange( 1.0f, 1.35f );
        flo32 ScaC = Radius * RandomF32InRange( 1.0f, 1.35f );
        
        vec2  yAxis   = -GetNormal( Fireball.Velocity );
        vec2  xAxis   = -GetPerp( yAxis );
        flo32 Radians = ToRadians( yAxis );
        vec2  bPos    = Fireball.Position + yAxis * ( Radius * 0.35f );
        
        flo32 OffsetX = Radius * 0.3f;
        vec2 PosA = bPos;
        vec2 PosB = bPos + xAxis * OffsetX;
        vec2 PosC = bPos - xAxis * OffsetX;
        
        vec2 DimA = Vec2( Radius * 2.0f * 0.95f, ScaA ) * 0.5f;
        vec2 DimB = Vec2( Radius * 2.0f * 0.65f, ScaB ) * 0.5f;
        vec2 DimC = Vec2( Radius * 2.0f * 0.65f, ScaC ) * 0.5f;
        
        vec4 bColor = FIREBALL_POINTS_BASE_COLOR;
        if( Fireball.Type == FireballType_Multiplier ) {
            bColor = FIREBALL_MULTIPLIER_BASE_COLOR;
        }
        
        vec4 Color_Surface = bColor;
        vec4 Color_Core    = TowardWhite( bColor, Fireball.tColor );
        if( Fireball.tColor < 0.0f ) {
            Color_Core = TowardBlack( bColor, fabsf( Fireball.tColor ) );
        }
        
        vec4 Color_TailA = TowardBlack( bColor, 0.20f  );
        vec4 Color_TailB = TowardBlack( bColor, 0.15f );
        
        DrawTriangleBC( Pass, PosA, DimA, Radians - PI * 0.5f, Color_TailA * 0.85f * Alpha );
        DrawTriangleBC( Pass, PosB, DimB, Radians - PI * 0.5f, Color_TailB * 0.85f * Alpha );
        DrawTriangleBC( Pass, PosC, DimC, Radians - PI * 0.5f, Color_TailB * 0.85f * Alpha );
        
        rect Bound = RectCR( Fireball.Position, Radius );
        DrawRect( Pass, TextureID_EnemyBullet_Core,    Bound, Color_Core    * Alpha );
        DrawRect( Pass, TextureID_EnemyBullet_Surface, Bound, Color_Surface * Alpha );
    }
    
    if( ( Fireball.Radius == FIREBALL_RADIUS_LARGE ) && ( Fireball.Health > 0 ) ) {
        DrawEntityHealth( Pass, AppState, Camera, Fireball.Position, Fireball.Health, COLOR_BLACK );
    }
}

internal void
DrawFireball( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, DRAW_STATE * Draw ) {
    FIREBALL_STATE * FireballS = Draw->FireballS;
    
    for( int32 iFireball = 0; iFireball < FireballS->nFireball; iFireball++ ) {
        FIREBALL Fireball = FireballS->Fireball[ iFireball ];
        
        DrawFireball( Pass, AppState, Camera, Fireball );
    }
}

internal void
FinalizeFireball( APP_STATE * AppState ) {
    FIREBALL_STATE * FireballS = &AppState->FireballS;
    
    rect Remove_Bound = AddRadius( AppState->World_Bound, 2.0f );
    
    uint32 NewCount = 0;
    for( int32 iFireball = 0; iFireball < FireballS->nFireball; iFireball++ ) {
        FIREBALL Fireball = FireballS->Fireball[ iFireball ];
        
        if( Fireball.Health <= 0 ) {
            vec4 bColor = FIREBALL_POINTS_BASE_COLOR;
            
            switch( Fireball.Type ) {
                case FireballType_Points: {
                    AddKillToScore( AppState, EntityType_Fireball, Fireball.Position );
                } break;
                
                case FireballType_Multiplier: {
                    AddEntityToKilledCountForLevel( AppState, EntityType_Fireball, 1 );
                    
                    POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Fireball );
                    int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                    
                    if( Fireball.Radius == FIREBALL_RADIUS_LARGE ) {
                        nScoreMultiplier = 5;
                    }
                    
                    vec2 Pos = ClampToRect( Fireball.Position, AppState->World_Bound );
                    for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                        AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                    }
                    
                    bColor = FIREBALL_MULTIPLIER_BASE_COLOR;
                }
            }
            
            if( Fireball.Radius == FIREBALL_RADIUS_LARGE ) {
                AddBigEntityDeathParticles( AppState, Fireball.Position, Fireball.Radius, bColor );
                AddBigEntityDeathParticles( AppState, Fireball.Position, Fireball.Radius, bColor );
            } else {
                AddEntityDeathParticles( AppState, Fireball.Position, bColor );
                AddEntityDeathParticles( AppState, Fireball.Position, bColor );
            }
            
            AddKillToWave( AppState, Fireball.WaveGuid );
            
            flo32 Rate = RandomF32InRange( 0.8f, 1.2f );
            PlaySoundEffect( AppState, AudioID_Entity_Death_Tiny, 2, ENTITY_DEATH_VOLUME * 0.25f, Rate );
        } else {
            if( IsInRect( Fireball.Position, Remove_Bound ) ) {
                FireballS->Fireball[ NewCount++ ] = Fireball;
            } else {
                AddEntityToMissedCountForLevel( AppState, EntityType_Fireball, 1 );
            }
        }
    }
    FireballS->nFireball = NewCount;
}