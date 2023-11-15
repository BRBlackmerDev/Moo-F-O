
internal void
AddEntityToMissedCountForLevel( APP_STATE * AppState, ENTITY_TYPE Type, uint32 nEntity ) {
    SCORE_STATE             * ScoreS          = &AppState->ScoreS;
    SCORE_ENTITY_KILL_COUNT * EntityKillCount = ScoreS->EntityKillCount + Type;
    
    EntityKillCount->nMissed += nEntity;
    EntityKillCount->nTotal  += nEntity;
}

internal void
AddEntityToKilledCountForLevel( APP_STATE * AppState, ENTITY_TYPE Type, uint32 nEntity ) {
    SCORE_STATE             * ScoreS          = &AppState->ScoreS;
    SCORE_ENTITY_KILL_COUNT * EntityKillCount = ScoreS->EntityKillCount + Type;
    
    EntityKillCount->nKilled += nEntity;
    EntityKillCount->nTotal  += nEntity;
}

//----------
// SCORE PARTICLES
//----------

internal SCORE_PARTICLE
ScoreParticleC( boo32 IsMultiplier, vec2 Position, uint32 Value, vec2 Velocity = {} ) {
    SCORE_PARTICLE Result = {};
    Result.IsMultiplier = IsMultiplier;
    Result.Value        = Value;
    Result.Position     = Position;
    Result.Velocity     = Velocity;
    Result.Life_TargetTime = SCORE_PARTICLE_LIFE_TARGET_TIME;
    if( ( IsMultiplier ) && ( ( Value % 100 ) == 0 ) ) {
        Result.Life_TargetTime = SCORE_PARTICLE_MILESTONE_LIFE_TARGET_TIME;
    }
    
    return Result;
}

internal void
AddScoreParticle( APP_STATE * AppState, SCORE_PARTICLE ScoreParticle ) {
    SCORE_PARTICLE_STATE * ScoreParticleS = &AppState->ScoreParticleS;
    if( ScoreParticleS->nScoreParticle < SCORE_PARTICLE_MAX_COUNT ) {
        ScoreParticleS->ScoreParticle[ ScoreParticleS->nScoreParticle++ ] = ScoreParticle;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SCORE_PARTICLE to SCORE_PARTICLE_STATE: %u", SCORE_PARTICLE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateScoreParticle( APP_STATE * AppState, flo32 dT ) {
    SCORE_PARTICLE_STATE * ScoreParticleS = &AppState->ScoreParticleS;
    
    for( uint32 iScoreParticle = 0; iScoreParticle < ScoreParticleS->nScoreParticle; iScoreParticle++ ) {
        SCORE_PARTICLE * ScoreParticle = ScoreParticleS->ScoreParticle + iScoreParticle;
        
        ScoreParticle->Life_Timer += dT;
        
        flo32 Speed    = 0.35f;
        flo32 Friction = 4.0f;
        vec2  Dir      = Vec2( 0.0f, 1.0f );
        
        vec2 Accel = ( Dir * Speed - ScoreParticle->Velocity ) * Friction;
        vec2 dPos  = Accel * ( 0.5f * dT * dT ) + ScoreParticle->Velocity * dT;
        // TODO: Collision!
        ScoreParticle->Position += dPos;
        ScoreParticle->Velocity += Accel * dT;
    }
    //DISPLAY_VALUE( uint32, ScoreParticleS->nScoreParticle );
}

internal vec2
WorldToUIPos( APP_STATE * AppState, CAMERA_STATE * World_Camera, vec2 World_Pos ) {
    vec2 tPos   = ( World_Pos - World_Camera->Pos ) / World_Camera->HalfDim;
    vec2 UI_Pos = GetCenter( AppState->UI_Bound ) + AppState->UI_HalfDim * tPos;
    vec2 Result = UI_Pos;
    return Result;
}

internal void
DrawScoreParticle( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, SCORE_PARTICLE ScoreParticle ) {
    vec4   Color = Vec4( 0.75f, 0.75f, 0.0f, 1.0f );
    FONT * Font  = AppState->FontList + FontID_1_25;
    
    char Score_String[ 16 + 1 ] = {};
    if( ScoreParticle.IsMultiplier ) {
        Color = Vec4( 0.0f, 0.75f, 0.0f, 1.0f );
        
        if( ( ScoreParticle.Value % 100 ) == 0 ) {
            Font = AppState->FontList + FontID_1_75;
            
            flo32 tLife   = Clamp01( ScoreParticle.Life_Timer / ScoreParticle.Life_TargetTime );
            flo32 Radians = tLife * TAU * 5.0f;
            flo32 tFlash  = cosf( Radians ) * 0.5f + 0.5f;
            Color = TowardWhite( Color, tFlash );
        }
        
        sprintf( Score_String, "%ux", ScoreParticle.Value );
        
    } else {
        sprintf( Score_String, "%u", ScoreParticle.Value );
    }
    
    DrawString( Pass, Font, Score_String, WorldToUIPos( AppState, Camera, ScoreParticle.Position ), Vec2( 1.0f, 1.0f ), Color );
}

internal void
DrawScoreParticle( RENDER_PASS * Pass, DRAW_STATE * Draw, APP_STATE * AppState ) {
    SCORE_PARTICLE_STATE * ScoreParticleS = Draw->ScoreParticleS;
    
    int32 RedrawMilestoneParticleOnTop = -1;
    
    for( uint32 iScoreParticle = 0; iScoreParticle < ScoreParticleS->nScoreParticle; iScoreParticle++ ) {
        SCORE_PARTICLE ScoreParticle = ScoreParticleS->ScoreParticle[ iScoreParticle ];
        if( ( ScoreParticle.Value % 100 ) == 0 ) {
            RedrawMilestoneParticleOnTop = iScoreParticle;
        }
        
        DrawScoreParticle( Pass, AppState, Draw->Camera, ScoreParticle );
    }
    
    if( RedrawMilestoneParticleOnTop > -1 ) {
        SCORE_PARTICLE ScoreParticle = ScoreParticleS->ScoreParticle[ RedrawMilestoneParticleOnTop ];
        DrawScoreParticle( Pass, AppState, Draw->Camera, ScoreParticle );
    }
}

internal void
FinalizeScoreParticle( APP_STATE * AppState ) {
    SCORE_PARTICLE_STATE * ScoreParticleS = &AppState->ScoreParticleS;
    
    rect Remove_Bound = AddRadius( AppState->World_Bound, 2.0f );
    
    uint32 NewCount = 0;
    for( uint32 iScoreParticle = 0; iScoreParticle < ScoreParticleS->nScoreParticle; iScoreParticle++ ) {
        SCORE_PARTICLE ScoreParticle = ScoreParticleS->ScoreParticle[ iScoreParticle ];
        
        boo32 DoRemove = ( ScoreParticle.Life_Timer >= ScoreParticle.Life_TargetTime );
        if( !DoRemove ) {
            ScoreParticleS->ScoreParticle[ NewCount++ ] = ScoreParticle;
        }
    }
    ScoreParticleS->nScoreParticle = NewCount;
}

//----------
// SCORE
//----------

internal void
AddKillToScore( APP_STATE * AppState, ENTITY_TYPE Type, vec2 Position, int32 CustomPoints = -1, vec2 Velocity = {}, boo32 DoTriggerBonusMultiplier = true ) {
    SCORE_STATE * ScoreS = &AppState->ScoreS;
    
    if( ScoreS->nKill < SCORE_KILL_MAX_COUNT ) {
        SCORE_KILL Kill = {};
        Kill.Type         = Type;
        Kill.Position     = Position;
        Kill.Velocity     = Velocity;
        Kill.CustomPoints = CustomPoints;
        Kill.DoTriggerBonusMultiplier = DoTriggerBonusMultiplier;
        ScoreS->Kill[ ScoreS->nKill++ ] = Kill;
        
        if( DoTriggerBonusMultiplier ) {
            ScoreS->nKill_Positive += 1;
        }
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add SCORE_KILL to SCORE_STATE: %u", SCORE_KILL_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
PlayerPickUpScoreMultipliers( APP_STATE * AppState, flo32 BonusMultiplier ) {
    SCORE_STATE            * ScoreS           = &AppState->ScoreS;
    PLAYER_STATE           * Player           = &AppState->Player;
    SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
    
    {
        uint32 NewCount = 0;
        for( int32 iScoreMultiplier = 0; iScoreMultiplier < ScoreMultiplierS->nScoreMultiplier; iScoreMultiplier++ ) {
            SCORE_MULTIPLIER ScoreMultiplier = ScoreMultiplierS->ScoreMultiplier[ iScoreMultiplier ];
            
            flo32 DistSq   = GetLengthSq( Player->Position - ScoreMultiplier.Position );
            flo32 Radius   = SCORE_MULTIPLIER_RADIUS + PLAYER_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                if( ScoreMultiplier.IsBonusPoints ) {
                    AddKillToScore( AppState, EntityType_BonusPoints, ScoreMultiplier.Position );
                } else {
                    ScoreS->Multiplier += 1.0f;
                    //ScoreS->Multiplier += ( 1.0f * BonusMultiplier );
                    
                    rect Bound = AddRadius( AppState->World_Bound, -1.0f );
                    vec2 Pos   = ClampToRect( Player->Position, Bound );
                    
                    AddScoreParticle( AppState, ScoreParticleC( true, Pos, ( uint32 )ScoreS->Multiplier ) );
                    
                    flo32 Rate = RandomF32InRange( 0.99f, 1.01f );
                    PlaySoundEffect( AppState, AudioID_ScoreMultiplier, 8, SCORE_MULTIPLIER_VOLUME, Rate );
                }
            } else {
                ScoreMultiplierS->ScoreMultiplier[ NewCount++ ] = ScoreMultiplier;
            }
        }
        ScoreMultiplierS->nScoreMultiplier = NewCount;
    }
}

internal void
FinalizeScore( APP_STATE * AppState, flo32 dT ) {
    SCORE_STATE               * ScoreS     = &AppState->ScoreS;
    OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
    
#if 0    
    if( ( ScoreS->nKill_Positive > 0 ) || ( ScoreS->nKill_Negative > 0 ) ) {
        for( int32 iKill = 0; iKill < ScoreS->nKill; iKill++ ) {
            SCORE_KILL Kill = ScoreS->Kill[ iKill ];
            
            STOCK_ENTRY * Stock = MarketS->Stock + Kill.Type;
            Stock->Flags |= STOCK_ENTRY_FLAG__IS_AVAILABLE;
        }
    }
#endif
    
    if( ScoreS->nKill_Positive > 0 ) {
        flo32 BaseValue = 0.025f;
        for( int32 iKill = 0; iKill < ScoreS->nKill; iKill++ ) {
            SCORE_KILL Kill = ScoreS->Kill[ iKill ];
            if( Kill.Type == EntityType_Turret ) {
                BaseValue = 0.125f;
            }
        }
        if( ( ScoreS->nKill_Positive > 1 ) || ( ScoreS->BonusMultiplier >= 1.0f ) )  {
            ScoreS->BonusMultiplier_LastKill_Timer = 0.0f;
        }
        if( ScoreS->BonusMultiplier_LastKill_Timer <= BONUS_MULTIPLIER_LAST_KILL_TARGET_TIME ) {
            BaseValue *= 2.0f;
        }
        BaseValue *= ( flo32 )ScoreS->nKill;
        ScoreS->BonusMultiplier_LastKill_Timer = 0.0f;
        
        ScoreS->BonusMultiplier = MinValue( ScoreS->BonusMultiplier + BaseValue, 1.0f );
        
        if( ScoreS->BonusMultiplier >= 1.0f ) {
            ScoreS->BonusMultiplier_Drain_CooldownTimer = 0.0f;
        } else {
            ScoreS->BonusMultiplier_Drain_CooldownTimer = BONUS_MULTIPLIER_DRAIN_COOLDOWN_TARGET_TIME_FULL - BONUS_MULTIPLIER_DRAIN_COOLDOWN_TARGET_TIME_PAUSE;
        }
    }
    
    flo32 BonusMultiplier = lerp( 1.0f, ScoreS->BonusMultiplier, 5.0f );
    PlayerPickUpScoreMultipliers( AppState, BonusMultiplier );
    
    for( int32 iKill = 0; iKill < ScoreS->nKill; iKill++ ) {
        SCORE_KILL Kill = ScoreS->Kill[ iKill ];
        AddEntityToKilledCountForLevel( AppState, Kill.Type, 1 );
        
        int32 bPoints = 0;
        
        bPoints = ( int32 )OverworldT->Points[ Kill.Type ].nPoints_Positive;
        if( Kill.CustomPoints > -1 ) {
            bPoints = Kill.CustomPoints;
        }
        
#if 0       
        switch( Kill.Type ) {
            case EntityType_Turret: {
                bPoints = TURRET_BASE_POINTS;
            } break;
            
            case EntityType_Chaser: {
                bPoints = CHASER_BASE_POINTS;
            } break;
            
            case EntityType_Swarm: {
                bPoints = SWARM_BASE_POINTS;
            } break;
            
            case EntityType_Drifter: {
                bPoints = DRIFTER_BASE_POINTS;
            } break;
            
            case EntityType_Looper: {
                bPoints = LOOPER_BASE_POINTS;
            } break;
            
            case EntityType_Starburst: {
                bPoints = STARBURST_BASE_POINTS;
            } break;
            
            case EntityType_Star: {
                bPoints = STAR_BASE_POINTS;
            } break;
            
            case EntityType_PersonalPod: {
                bPoints = PERSONAL_POD_BASE_POINTS;
            } break;
            
            case EntityType_Transport: {
                bPoints = TRANSPORT_BASE_POINTS;
            } break;
            
            case EntityType_MiniBoss: {
                bPoints = MINIBOSS_BASE_POINTS;
            } break;
            
            case EntityType_Asteroid: {
                bPoints = ASTEROID_BOMB_ONLY_BASE_POINTS;
            } break;
            
            case EntityType_Asteroid_Tiny: {
                bPoints = ASTEROID_TINY_BASE_POINTS;
            } break;
            
            case EntityType_Miner: {
                bPoints = MINER_BASE_POINTS;
            } break;
            
            case EntityType_Bomber: {
                bPoints = BOMBER_BASE_POINTS;
            } break;
            
            case EntityType_Bumper: {
                bPoints = BUMPER_BASE_POINTS;
            } break;
            
            case EntityType_Bumper_Bounce: {
                bPoints = ScoreS->Bumper_BasePoints++;
                //bPoints = BUMPER_BOUNCE_BASE_POINTS;
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
#endif
        
        flo32 Points = ( ( flo32 )bPoints * BonusMultiplier * ScoreS->Multiplier );
        //Points *= 0.1f;
        
#if 0        
        { // UPDATE SCORE LOG
            SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
            POINTS_ENTRY PointsEntry = MarketS->Points[ Kill.Type ];
            
            SCORE_LOG ScoreLog = {};
            ScoreLog.Type              = Kill.Type;
            ScoreLog.Points            = Points;
            ScoreLog.AtScoreMultiplier = ScoreS->Multiplier;
            ScoreLog.AtBonusMultiplier = BonusMultiplier;
            
            int32 nScoreMultiplierOnScreen = ScoreMultiplierS->nScoreMultiplier - PointsEntry.nScoreMultiplier;
            ScoreLog.nScoreMultiplierOnScreen = nScoreMultiplierOnScreen;
            ScoreLog.PointsMissed             = ( ( flo32 )bPoints * BonusMultiplier * ( flo32 )nScoreMultiplierOnScreen );
            //Assert( ScoreS->nScoreLog < SCORE_LOG_MAX_COUNT );
            //ScoreS->ScoreLog[ ScoreS->nScoreLog++ ] = ScoreLog;
        }
#endif
        
        ScoreS->Score += Points;
        
        rect Bound = AddRadius( AppState->World_Bound, -1.0f );
        vec2 Pos   = ClampToRect( Kill.Position, Bound );
        
        if( Points > 0 ) {
            AddScoreParticle( AppState, ScoreParticleC( false, Pos, ( uint32 )Points, Kill.Velocity ) );
        }
    }
    ScoreS->nKill          = 0;
    ScoreS->nKill_Positive = 0;
    ScoreS->nKill_Negative = 0;
    
    ScoreS->BonusMultiplier_LastKill_Timer += dT;
    
    ScoreS->BonusMultiplier_Drain_CooldownTimer += dT;
    if( ScoreS->BonusMultiplier_Drain_CooldownTimer >= BONUS_MULTIPLIER_DRAIN_COOLDOWN_TARGET_TIME_FULL ) {
        ScoreS->BonusMultiplier = MaxValue( ScoreS->BonusMultiplier - BONUS_MULTIPLIER_DRAIN_RATE * dT, 0.0f );
    }
    
    if( ScoreS->BonusMultiplier >= 0.7f ) {
        flo32 t      = ( ScoreS->BonusMultiplier - 0.7f ) / 0.3f;
        flo32 Speed  = lerp( 2.0f, t, 6.0f );
        ScoreS->BonusMultiplier_Radians += Speed * dT * PI * 2.0f;
    }
    
    PLAYER_STATE * Player   = &AppState->Player;
    END_LEVEL    * EndLevel = &AppState->EndLevel;
    if( ( !Player->IsDead ) && ( !EndLevel->Show ) ) {
        ScoreS->BonusMultiplier_TotalTime += dT;
        ScoreS->BonusMultiplier_Total     += ( lerp( 1.0f, ScoreS->BonusMultiplier, BONUS_MULTIPLIER_MAX_VALUE ) * dT );
        
        if( ScoreS->BonusMultiplier >= 1.0f ) {
            ScoreS->BonusMultiplier_FullTime += dT;
        }
    }
}