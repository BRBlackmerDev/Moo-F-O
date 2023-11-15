
internal flo32
Powerup_GetChangeTargetTime() {
    flo32 Result = RandomF32InRange( 1.5f, 3.0f );
    return Result;
}

internal void
Powerup_SetRandomDirAndSpeed( POWERUP * Powerup, rect World_Bound ) {
    flo32 t = RandomF32();
    Powerup->Move_Speed = lerp( 1.0f, t * t, 2.5f );
    
    rect  Bound   = AddRadius( World_Bound, -POWERUP_RADIUS * 0.5f );
    flo32 Radians = WrapRadians( Powerup->Move_Radians + RandomF32InRange( -PI / 3.0f, PI / 3.0f ) );
    vec2  Dir     = ToDirection2D( Radians );
    vec2  Dest    = Powerup->Position + Dir * ( Powerup->Move_Speed * Powerup->Move_TargetTime );
    if( ( Dest.x < Bound.Left ) || ( Dest.x > Bound.Right ) ) {
        Dir.x = -Dir.x;
    }
    if( ( Dest.y < Bound.Bottom ) || ( Dest.y > Bound.Top ) ) {
        Dir.y = -Dir.y;
    }
    Powerup->Move_Radians = ToRadians( Dir );
}

internal POWERUP
PowerupC( POWERUP_TYPE Type, vec2 Position ) {
    POWERUP Result = {};
    Result.Type       = Type;
    Result.Position   = Position;
    Result.Radius     = POWERUP_RADIUS;
    Result.Health     = 1;
    
    Result.Exit_Timer      = POWERUP_EXIT_TARGET_TIME;
    Result.Exit_TargetTime = POWERUP_EXIT_TARGET_TIME;
    
    Result.Move_TargetTime = RandomF32InRange( 0.15f, 2.0f );
    
    return Result;
}

internal void
AddPowerup( APP_STATE * AppState, POWERUP Powerup ) {
    POWERUP_STATE * PowerupS = &AppState->PowerupS;
    if( PowerupS->nPowerup < POWERUP_MAX_COUNT ) {
        PowerupS->Powerup[ PowerupS->nPowerup++ ] = Powerup;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add POWERUP to POWERUP_STATE: %u", POWERUP_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
GenPowerupStats( APP_STATE * AppState ) {
    POWERUP_STATE_SAVE_DATA * PowerupSave = &AppState->SaveGame.PowerupSave;
    POWERUP_ENTRY * PowerupEntry = 0;
    
    PowerupEntry               = PowerupSave->PowerupEntry + PowerupType_Health;
    PowerupEntry->bNumer       = 0;
    PowerupEntry->UpgradeCost  = 1000.0;
    PowerupEntry->bUpgradeCost = 1000.0;
    
    PowerupEntry               = PowerupSave->PowerupEntry + PowerupType_NoDamage;
    PowerupEntry->bNumer       = 0;
    PowerupEntry->UpgradeCost  = 1000.0;
    PowerupEntry->bUpgradeCost = 1000.0;
    PowerupEntry->TargetTime   = 5.0f;
    
    PowerupEntry               = PowerupSave->PowerupEntry + PowerupType_PlayerBullets;
    PowerupEntry->bNumer       = 0;
    PowerupEntry->UpgradeCost  = 1000.0;
    PowerupEntry->bUpgradeCost = 1000.0;
    PowerupEntry->Rate         = 1.5f;
    PowerupEntry->TargetTime   = 8.0f;
    
    PowerupEntry               = PowerupSave->PowerupEntry + PowerupType_ScoreMultipliers;
    PowerupEntry->bNumer       = 0;
    PowerupEntry->UpgradeCost  = 1000.0;
    PowerupEntry->bUpgradeCost = 1000.0;
    PowerupEntry->Rate         = 4.0f;
    PowerupEntry->TargetTime   = 4.0f;
    
    PowerupEntry               = PowerupSave->PowerupEntry + PowerupType_DamageAllEnemies;
    PowerupEntry->bNumer       = 0;
    PowerupEntry->UpgradeCost  = 1000.0;
    PowerupEntry->bUpgradeCost = 1000.0;
    PowerupEntry->Rate         = 10.0f; // Damage Dealt
    
    PowerupEntry               = PowerupSave->PowerupEntry + PowerupType_SecondaryGadgets;
    PowerupEntry->bNumer       = 0;
    PowerupEntry->UpgradeCost  = 1000.0;
    PowerupEntry->bUpgradeCost = 1000.0;
    PowerupEntry->Rate         = 1.25f;
    PowerupEntry->TargetTime   = 8.0f;
    
    //AppState->Market_TotalPoints = 10000000;
}

internal void
InitPowerups( APP_STATE * AppState, flo32 Level_TargetTime ) {
    Assert( Level_TargetTime > 0.0f );
    
    POWERUP_STATE_SAVE_DATA * PowerupSave = &AppState->SaveGame.PowerupSave;
    POWERUP_STATE           * PowerupS    = &AppState->PowerupS;
    
    int32 Sum = 0;
    for( int32 iPowerup = PowerupType_None + 1; iPowerup < PowerupType_Count; iPowerup++ ) {
        POWERUP_ENTRY * PowerupEntry = PowerupSave->PowerupEntry + iPowerup;
        
        if( PowerupEntry->bNumer > 0 ) {
            PowerupS->Powerup_NumerForLevel[ iPowerup ]  = PowerupEntry->bNumer;
            Sum                                         += PowerupEntry->bNumer;
        }
    }
    
    if( Sum > 0 ) {
        if( Sum < 100 ) {
            PowerupS->Powerup_NumerForLevel[ PowerupType_None ] = ( 100 - Sum );
            Sum                                                 = 100;
        }
        
        PowerupS->Spawn_IsActive = true;
        
        flo32 Denom     = ( flo32 )Sum / 100.0f;
        flo32 TotalTime = Level_TargetTime - POWERUP_TIME_DELAY_FOR_LEVEL_START - POWERUP_TIME_BUFFER_FOR_LEVEL_END;
        PowerupS->Spawn_Segment_TargetTime  = ( TotalTime / Denom );
        PowerupS->Spawn_Cooldown_TargetTime = PowerupS->Spawn_Segment_TargetTime * 0.5f;
        PowerupS->Spawn_TargetTime          = ( RandomF32() * PowerupS->Spawn_Segment_TargetTime );
        PowerupS->Spawn_Timer               = -POWERUP_TIME_DELAY_FOR_LEVEL_START;
    } else {
        PowerupS->Spawn_IsActive = false;
    }
}

internal void
UpdatePowerup( APP_STATE * AppState, flo32 dT ) {
    POWERUP_STATE_SAVE_DATA * PowerupSave = &AppState->SaveGame.PowerupSave;
    POWERUP_STATE           * PowerupS    = &AppState->PowerupS;
    PLAYER_STATE            * Player      = &AppState->Player;
    
    { // Update Powerup Spawn
        if( PowerupS->Spawn_IsActive ) {
            PowerupS->Spawn_Timer += dT;
            if( PowerupS->Spawn_Timer >= PowerupS->Spawn_TargetTime ) {
                flo32 TimeLeftInSegment = PowerupS->Spawn_Segment_TargetTime - PowerupS->Spawn_TargetTime;
                if( TimeLeftInSegment > PowerupS->Spawn_Cooldown_TargetTime ) {
                    PowerupS->Spawn_TargetTime = ( RandomF32() * PowerupS->Spawn_Segment_TargetTime );
                    PowerupS->Spawn_Timer      = -TimeLeftInSegment;
                } else {
                    flo32 Overflow = ( PowerupS->Spawn_TargetTime + PowerupS->Spawn_Cooldown_TargetTime ) - PowerupS->Spawn_Segment_TargetTime;
                    flo32 TimeLeftInNextSegment = PowerupS->Spawn_Segment_TargetTime - Overflow;
                    
                    PowerupS->Spawn_TargetTime = ( RandomF32() * TimeLeftInNextSegment ) + Overflow;
                    PowerupS->Spawn_Timer      = -( PowerupS->Spawn_Cooldown_TargetTime - Overflow );
                }
                
                int32 Sum = 0;
                int32 Numer[ PowerupType_Count ] = {};
                for( int32 iPowerup = 0; iPowerup < PowerupType_Count; iPowerup++ ) {
                    Numer[ iPowerup ] = PowerupS->Powerup_NumerForLevel[ iPowerup ];
                    Sum              += PowerupS->Powerup_NumerForLevel[ iPowerup ];
                }
                
                Assert( Sum >= 100 );
                
                int32 Select = RandomSelectByOdds( Numer, PowerupType_Count );
                if( Select > PowerupType_None ) {
                    rect Bound = AppState->World_Bound;
                    {
                        BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
                        if( BoardRoomS->State_IsActive ) {
                            OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                            flo32 Top = ObservationDeck->Position.y - ObservationDeck->Radius;
                            if( ObservationDeck->Shield_IsActive ) {
                                Top -= ObservationDeck->Shield_MaxDist;
                            }
                            Bound.Top = Top;
                        }
                    }
                    {
                        SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
                        if( SatelliteS->nSatellite > 0 ) {
                            Bound.Right = SatelliteS->Shield_Bound.Left;
                        }
                    }
                    
                    Bound = AddRadius( Bound, -4.0f );
                    vec2 Pos   = GetRandomPoint( Bound );
                    AddPowerup( AppState, PowerupC( ( POWERUP_TYPE )Select, Pos ) );
                    
                    {
                        PowerupS->Powerup_NumerForLevel[ Select ] = MaxValue( PowerupS->Powerup_NumerForLevel[ Select ] - 100, 0 );
                        
                        Sum -= 100;
                    }
                    
                    if( Sum < 100 ) {
                        PowerupS->Powerup_NumerForLevel[ PowerupType_None ] = ( 100 - Sum );
                    }
                }
            }
        }
    }
    
    { // Update Player Pickup
        if( Player->Health > 0 ) {
            for( int32 iPowerup = 0; iPowerup < PowerupS->nPowerup; iPowerup++ ) {
                POWERUP * Powerup = PowerupS->Powerup + iPowerup;
                
                if( Powerup->Health > 0 ) {
                    flo32 Radius   = PLAYER_RADIUS + Powerup->Radius;
                    flo32 RadiusSq = Radius * Radius;
                    flo32 DistSq   = GetLengthSq( Player->Position - Powerup->Position );
                    if( DistSq <= RadiusSq ) {
                        Powerup->Health          = 0;
                        Powerup->Exit_Pos        = Powerup->Position;
                        Powerup->Exit_Timer      = POWERUP_PICKUP_ANIM_TARGET_TIME;
                        Powerup->Exit_TargetTime = POWERUP_PICKUP_ANIM_TARGET_TIME;
                        
                        switch( Powerup->Type ) {
                            case PowerupType_Health: {
                                Player->Health = MinValue( Player->Health + 1, Player->MaxHealth );
                                
                                // PlayAudio( AppState, AudioID_Powerup, POWERUP_VOLUME, 0.65f );
                            } break;
                            
                            case PowerupType_NoDamage: {
                                POWERUP_ENTRY PowerupEntry = PowerupSave->PowerupEntry[ PowerupType_NoDamage ];
                                Player->NoDamage_Timer          = PowerupEntry.TargetTime;
                                Player->ShowShieldPowerup_Timer = PowerupEntry.TargetTime;
                            } break;
                            
                            case PowerupType_PlayerBullets: {
                                POWERUP_ENTRY PowerupEntry = PowerupSave->PowerupEntry[ PowerupType_PlayerBullets ];
                                Player->Powerup_Timer_PlayerBullets = PowerupEntry.TargetTime;
                                Player->Powerup_Rate_PlayerBullets  = PowerupEntry.Rate;
                            } break;
                            
                            case PowerupType_ScoreMultipliers: {
                                POWERUP_ENTRY PowerupEntry = PowerupSave->PowerupEntry[ PowerupType_ScoreMultipliers ];
                                
                                SCORE_MULTIPLIER_STATE * ScoreMultiplierS = &AppState->ScoreMultiplierS;
                                ScoreMultiplierS->Powerup_Timer       = PowerupEntry.TargetTime;
                                //ScoreMultiplierS->Powerup_TimeToAdd   = PowerupEntry.Rate;
                                //ScoreMultiplierS->Powerup_RadiusToAdd = PowerupEntry.Rate * 0.5f;
                                
                                //for( int32 iScoreMultiplier = 0; iScoreMultiplier < ScoreMultiplierS->nScoreMultiplier; iScoreMultiplier++ ) {
                                //SCORE_MULTIPLIER * ScoreMultiplier = ScoreMultiplierS->ScoreMultiplier + iScoreMultiplier;
                                //
                                //ScoreMultiplier->Life_Timer -= ScoreMultiplierS->Powerup_TimeToAdd;
                                //}
                            } break;
                            
                            case PowerupType_DamageAllEnemies: {
                                Powerup->Exit_Timer      = POWERUP_PICKUP_ANIM_TARGET_TIME - ( 2.0f / 60.0f );
                                Powerup->Exit_TargetTime = POWERUP_PICKUP_ANIM_TARGET_TIME - ( 2.0f / 60.0f );
                                
                                Player->ScreenFlash_Type       = ScreenFlashType_Powerup_DamageAll;
                                Player->ScreenFlash_TargetTime = SCREEN_FLASH_TARGET_TIME__POWERUP_DAMAGE_ALL;
                                Player->ScreenFlash_Timer      = Player->ScreenFlash_TargetTime;
                                
                                POWERUP_ENTRY PowerupEntry = PowerupSave->PowerupEntry[ PowerupType_DamageAllEnemies ];
                                int32 Damage = ( int32 )PowerupEntry.Rate;
                                
                                { // TURRET
                                    TURRET_STATE * TurretS = &AppState->TurretS;
                                    for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
                                        TURRET * Turret = TurretS->Turret + iTurret;
                                        Turret->Health = MaxValue( Turret->Health - Damage, 0 );
                                    }
                                    
                                    for( int32 iMegaTurret = 0; iMegaTurret < TurretS->nMegaTurret; iMegaTurret++ ) {
                                        TURRET * MegaTurret = TurretS->MegaTurret + iMegaTurret;
                                        MegaTurret->Health = MaxValue( MegaTurret->Health - Damage, 0 );
                                    }
                                }
                                { // CHASER
                                    CHASER_STATE * ChaserS = &AppState->ChaserS;
                                    for( int32 iChaser = 0; iChaser < ChaserS->nChaser; iChaser++ ) {
                                        CHASER * Chaser = ChaserS->Chaser + iChaser;
                                        Chaser->Health = MaxValue( Chaser->Health - Damage, 0 );
                                    }
                                }
                                { // SWARM
                                    SWARM_STATE * SwarmS = &AppState->SwarmS;
                                    for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
                                        SWARM * Swarm = SwarmS->Swarm + iSwarm;
                                        Swarm->Health = MaxValue( Swarm->Health - Damage, 0 );
                                    }
                                }
                                { // DRIFTER
                                    DRIFTER_STATE * DrifterS = &AppState->DrifterS;
                                    for( int32 iDrifter = 0; iDrifter < DrifterS->nDrifter; iDrifter++ ) {
                                        DRIFTER * Drifter = DrifterS->Drifter + iDrifter;
                                        Drifter->Health = MaxValue( Drifter->Health - Damage, 0 );
                                    }
                                }
                                { // LOOPER
                                    LOOPER_STATE * LooperS = &AppState->LooperS;
                                    for( int32 iLooper = 0; iLooper < LooperS->nLooper; iLooper++ ) {
                                        LOOPER * Looper = LooperS->Looper + iLooper;
                                        Looper->Health = MaxValue( Looper->Health - Damage, 0 );
                                    }
                                }
                                { // ENEMY_BULLET
                                    ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
                                    for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
                                        ENEMY_BULLET * EnemyBullet = EnemyBulletS->EnemyBullet + iEnemyBullet;
                                        EnemyBullet->Health = MaxValue( EnemyBullet->Health - Damage, 0 );
                                        EnemyBullet->DoHitPointDrop = true;
                                    }
                                }
                                { // BAY DOORS
                                    BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
                                    for( int32 iPersonalPod = 0; iPersonalPod < BayDoorS->nPersonalPod; iPersonalPod++ ) {
                                        PERSONAL_POD * PersonalPod = BayDoorS->PersonalPod + iPersonalPod;
                                        PersonalPod->Health = MaxValue( PersonalPod->Health - Damage, 0 );
                                    }
                                    
                                    for( int32 iTransport = 0; iTransport < BayDoorS->nTransport; iTransport++ ) {
                                        TRANSPORT * Transport = BayDoorS->Transport + iTransport;
                                        Transport->Health = MaxValue( Transport->Health - Damage, 0 );
                                    }
                                }
                                { // ASTEROID
                                    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
                                    for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                                        ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                                        Asteroid->Health = MaxValue( Asteroid->Health - Damage, 0 );
                                    }
                                }
                                { // MINER
                                    MINER_STATE * MinerS = &AppState->MinerS;
                                    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                                        MINER * Miner = MinerS->Miner + iMiner;
                                        Miner->Health = MaxValue( Miner->Health - Damage, 0 );
                                    }
                                }
                                
#if 0                                
                                { // BOMBER
                                    BOMB_STATE * BombS = &AppState->BombS;
                                    for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
                                        BOMBER * Bomber = BombS->Bomber + iBomber;
                                        Bomber->Health = MaxValue( Bomber->Health - Damage, 0 );
                                    }
                                }
#endif
                                { // BOMB
                                    BOMB_STATE * BombS = &AppState->BombS;
                                    for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
                                        BOMB * Bomb = BombS->Bomb + iBomb;
                                        if( !Bomb->IsDetonated ) {
                                            Bomb->IsTriggered = true;
                                            Bomb->Timer       = MaxValue( Bomb->Timer, BOMB_DETONATE_TARGET_TIME * 0.5f );
                                        }
                                    }
                                }
                                { // BOMB LAUNCHER
                                    BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
                                    for( int32 iBombLauncher = 0; iBombLauncher < BombLauncherS->nBombLauncher; iBombLauncher++ ) {
                                        BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iBombLauncher;
                                        BombLauncher->Health = MaxValue( BombLauncher->Health - Damage, 0 );
                                    }
                                }
                                { // BLIMP
                                    BLIMP_STATE * BlimpS = &AppState->BlimpS;
                                    for( int32 iBlimp = 0; iBlimp < BlimpS->nBlimp; iBlimp++ ) {
                                        BLIMP * Blimp = BlimpS->Blimp + iBlimp;
                                        Blimp->Health = MaxValue( Blimp->Health - Damage, 0 );
                                    }
                                }
                                { // MEGA_MINER
                                    MINER_STATE * MinerS = &AppState->MinerS;
                                    for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
                                        MEGA_MINER * MegaMiner = MinerS->MegaMiner + iMegaMiner;
                                        MegaMiner->Health = MaxValue( MegaMiner->Health - Damage, 0 );
                                    }
                                }
                                { // MISSILE
                                    MISSILE_STATE * MissileS = &AppState->MissileS;
                                    for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
                                        MISSILE * Missile = MissileS->Missile + iMissile;
                                        Missile->Health = MaxValue( Missile->Health - Damage, 0 );
                                    }
                                }
                                { // WORM
                                    WORM_STATE * WormS = &AppState->WormS;
                                    for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
                                        WORM * Worm = WormS->Worm + iWorm;
                                        Worm->Health = MaxValue( Worm->Health - Damage, 0 );
                                    }
                                }
                                { // FIREBALL
                                    FIREBALL_STATE * FireballS = &AppState->FireballS;
                                    for( int32 iFireball = 0; iFireball < FireballS->nFireball; iFireball++ ) {
                                        FIREBALL * Fireball = FireballS->Fireball + iFireball;
                                        Fireball->Health = MaxValue( Fireball->Health - Damage, 0 );
                                    }
                                }
                                { // SPIT
                                    SPITTER_STATE * SpitterS = &AppState->SpitterS;
                                    for( int32 iSpit = 0; iSpit < SpitterS->nSpit; iSpit++ ) {
                                        SPIT * Spit = SpitterS->Spit + iSpit;
                                        Spit->IsSterilized = true;
                                    }
                                }
                                { // BARREL
                                    SPITTER_STATE * SpitterS = &AppState->SpitterS;
                                    for( int32 iBarrel = 0; iBarrel < SpitterS->nBarrel; iBarrel++ ) {
                                        BARREL * Barrel = SpitterS->Barrel + iBarrel;
                                        Barrel->Health = MaxValue( Barrel->Health - Damage, 0 );
                                    }
                                }
                                { // GUNNER
                                    GUNNER_STATE * GunnerS = &AppState->GunnerS;
                                    for( int32 iGunner = 0; iGunner < GunnerS->nGunner; iGunner++ ) {
                                        GUNNER * Gunner = GunnerS->Gunner + iGunner;
                                        Gunner->Health = MaxValue( Gunner->Health - Damage / 2, 0 );
                                    }
                                }
                                { // BOARD ROOM
                                    BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
                                    OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                                    
                                    SECURITY_DRONE * SecurityDrone = &BoardRoomS->SecurityDrone;
                                    SecurityDrone->Health = MaxValue( SecurityDrone->Health - Damage, 0 );
                                    
                                    for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                                        MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                                        if( MissileLauncher->Health > -1 ) {
                                            MissileLauncher->Health = MaxValue( MissileLauncher->Health - Damage, 0 );
                                        }
                                    }
                                }
#if 0                                
                                {
                                    WAVE_STATE * WaveS = &AppState->WaveS;
                                    { // Entity Group Spawn
                                        for( uint32 iEntityGroupSpawn = 0; iEntityGroupSpawn < WaveS->nEntityGroupSpawn; iEntityGroupSpawn++ ) {
                                            ENTITY_GROUP_SPAWN EntityGroupSpawn = WaveS->EntityGroupSpawn[ iEntityGroupSpawn ];
                                            AddKillsToWave( AppState, EntityGroupSpawn );
                                        }
                                        WaveS->nEntityGroupSpawn = 0;
                                    }
                                    
                                    { // Entity Spawn
                                        ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
                                        uint32 NewCount = 0;
                                        for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
                                            ENTITY EntitySpawn       = EntitySpawnS->EntitySpawn      [ iEntitySpawn ];
                                            flo32  EntitySpawn_Timer = EntitySpawnS->EntitySpawn_Timer[ iEntitySpawn ];
                                            
                                            boo32  SaveSpawn = ( EntitySpawn.Type == EntityType_Miner ) ||
                                                ( EntitySpawn.Type == EntityType_Bomber ) ||
                                                ( EntitySpawn.Type == EntityType_Bumper ) ||
                                                ( EntitySpawn.Type == EntityType_Bumper_Stationary ) || 
                                                ( EntitySpawn.Type == EntityType_Miner_Stationary ) ||
                                                ( EntitySpawn.Type == EntityType_MegaTurret ) ||
                                                ( EntitySpawn.Type == EntityType_Worm );
                                            
                                            if( SaveSpawn ) {
                                                EntitySpawnS->EntitySpawn      [ NewCount ] = EntitySpawn;
                                                EntitySpawnS->EntitySpawn_Timer[ NewCount ] = EntitySpawn_Timer;
                                                NewCount++;
                                            } else {
                                                AddKillsToWave( AppState, EntitySpawn );
                                            }
                                        }
                                        EntitySpawnS->nEntitySpawn = NewCount;
                                    }
                                }
#endif
                            } break;
                            
                            case PowerupType_SecondaryGadgets: {
                                POWERUP_ENTRY PowerupEntry = PowerupSave->PowerupEntry[ PowerupType_SecondaryGadgets ];
                                
                                AUTO_TURRET_STATE * AutoTurretS = &AppState->AutoTurretS;
                                AutoTurretS->Powerup_Timer = PowerupEntry.TargetTime;
                                AutoTurretS->Powerup_Rate  = PowerupEntry.Rate;
                                
                                Player->Powerup_Timer_SecondaryGadgets = PowerupEntry.TargetTime;
                            } break;
                            
                            default: {
                                InvalidCodePath;
                            } break;
                        }
                    }
                }
            }
        }
    }
    
    for( int32 iPowerup = 0; iPowerup < PowerupS->nPowerup; iPowerup++ ) {
        POWERUP * Powerup = PowerupS->Powerup + iPowerup;
        
        flo32 dRadians = ( TAU * 20.0f );
        Powerup->Flash_Radians = WrapRadians( Powerup->Flash_Radians + dRadians * dT );
        
        Powerup->Exit_Timer = MaxValue( Powerup->Exit_Timer - dT, 0.0f );
        
        Powerup->Move_Timer += dT;
        if( Powerup->Move_Timer >= Powerup->Move_TargetTime ) {
            Powerup->Move_Timer      = 0.0f;
            Powerup->Move_TargetTime = Powerup_GetChangeTargetTime();
            
            Powerup_SetRandomDirAndSpeed( Powerup, AppState->World_Bound );
        }
        
        for( int32 iRepel = iPowerup + 1; iRepel < PowerupS->nPowerup; iRepel++ ) {
            POWERUP * Repel = PowerupS->Powerup + iRepel;
            
            vec2 V = Repel->Position - Powerup->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = POWERUP_REPEL_RADIUS + POWERUP_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * POWERUP_REPEL_SPEED;
                
                Powerup->Repel -= Repel_Vector;
                Repel->Repel   += Repel_Vector;
            }
        }
        
        vec2 DirToPlayer = GetNormal( Player->Position - Powerup->Position );
        if( Player->TractorBeam_IsActive ) {
            flo32 Speed = 16.0f;
            Powerup->Velocity += DirToPlayer * ( Speed * dT );
        }
        
        vec2 Dir   = ToDirection2D( Powerup->Move_Radians );
        vec2 Accel = ( Dir * Powerup->Move_Speed - Powerup->Velocity + Powerup->Repel ) * POWERUP_FRICTION;
        
        { // Update Seek Player
            if( Player->Health > 0 ) {
                flo32 Radius = PLAYER_RADIUS + POWERUP_SEEK_PLAYER_RADIUS + Powerup->Radius;
                flo32 Dist   = GetLength( Player->Position - Powerup->Position );
                if( Dist <= Radius ) {
                    flo32 tSeek    = Clamp01( Dist / Radius );
                    flo32 Speed    = lerp( 12.0f, tSeek, 4.0f );
                    
                    Powerup->Velocity += DirToPlayer * ( Speed * dT );
                }
            }
        }
        
        vec2 dPos = Accel * ( 0.5f * dT * dT ) + Powerup->Velocity * dT;
        // TODO: Collision!
        Powerup->Position += dPos;
        Powerup->Velocity += Accel * dT;
        
        Powerup->Repel = {};
        
        if( Powerup->Health <= 0 ) {
            flo32 t = Clamp01( Powerup->Exit_Timer / Powerup->Exit_TargetTime );
            Powerup->Position = lerp( Player->Position, t, Powerup->Exit_Pos );
        }
    }
}

internal void
DrawPowerupIcon( RENDER_PASS * Pass, POWERUP_TYPE Type, vec2 bPos, flo32 bRadius, flo32 tAlpha ) {
    vec2 Icon_Dim = Vec2( bRadius, bRadius ) * 2.0f;
    
    switch( Type ) {
        case PowerupType_Health: {
            //vec2 Dim  = Icon_Dim * Vec2( 1.0f, 0.45f );
            
            //rect BoundA = RectCD( bPos, Dim );
            //rect BoundB = RectCD( bPos, Vec2( Dim.y, Dim.x ) );
            
            vec4 Color = COLOR_RED;
            //DrawRect( Pass, BoundA, Color * tAlpha );
            //DrawRect( Pass, BoundB, Color * tAlpha );
            
            DrawRect( Pass, TextureID_Powerup_Health, RectCD( bPos, Icon_Dim ), Color * tAlpha );
        } break;
        
        case PowerupType_NoDamage: {
            // Rainbow Shield
            
            //vec2 Dim = Icon_Dim * Vec2( 0.333333f, 0.75f );
            //rect BoundA = RectCD( bPos + Vec2( -Dim.x, 0.0f ), Dim );
            //rect BoundB = RectCD( bPos + Vec2(   0.0f, 0.0f ), Dim );
            //rect BoundC = RectCD( bPos + Vec2(  Dim.x, 0.0f ), Dim );
            
            //DrawRect( Pass, BoundA, COLOR_RED    * tAlpha );
            //DrawRect( Pass, BoundB, COLOR_BLUE   * tAlpha );
            //DrawRect( Pass, BoundC, COLOR_YELLOW * tAlpha );
            
            DrawRect( Pass, TextureID_Powerup_Shield, RectCD( bPos, Icon_Dim ), COLOR_WHITE * tAlpha );
        } break;
        
        case PowerupType_PlayerBullets: {
            BULLET Bullet = {};
            Bullet.Position  = bPos;
            Bullet.Direction = ToDirection2D( PI * 0.9f );
            Bullet.Health    = 1;
            
            DrawBullet( Pass, Bullet, Icon_Dim.x * 0.25f );
        } break;
        
        case PowerupType_ScoreMultipliers: {
            SCORE_MULTIPLIER ScoreMultiplier = {};
            ScoreMultiplier.Position = bPos;
            ScoreMultiplier.Radians  = -PI / 32.0f;
            DrawScoreMultiplier( Pass, ScoreMultiplier, Icon_Dim.x * 1.6f, tAlpha );
        } break;
        
        case PowerupType_DamageAllEnemies: {
            flo32 Radius = Icon_Dim.x * 0.325f;
            vec4  Color  = COLOR_RED * tAlpha;
            DrawRect( Pass, TextureID_RoundedDome, RectCR( bPos, Radius ), Color );
        } break;
        
        case PowerupType_SecondaryGadgets: {
            vec2 Pos0 = bPos + Vec2( -Icon_Dim.x * 0.2f, 0.0f );
            vec2 Pos1 = bPos + Vec2(  Icon_Dim.x * 0.2f, 0.0f );
            
            flo32 Radius = Icon_Dim.x * 0.2f;
            flo32 Dist   = -Radius * 0.25f;
            
            SPIT_ALT_COLORS;
            
            {
                int32 nCollector = SpitType_Count;
                for( int32 iCollector = 0; iCollector < nCollector; iCollector++ ) {
                    flo32 t   = ( ( flo32 )iCollector / ( flo32 )nCollector );
                    flo32 Rad = TAU * t - PI * 0.1f;
                    
                    vec2  Pos   = Pos1 + ToDirection2D( Rad ) * ( Radius + Dist );
                    orect Bound = ORectCR( Pos, Radius, Rad );
                    vec4  Color = SpitAltColors[ iCollector ] * tAlpha;
                    DrawORect( Pass, TextureID_Collector, Bound, Color );
                }
            }
            {
                AUTO_TURRET AutoTurret = {};
                AutoTurret.Position = Pos0;
                AutoTurret.Radians  = PI;
                
                DrawAutoTurret( Pass, AutoTurret, 1.0f, Icon_Dim.x * 0.25f, tAlpha, false );
            }
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
}

internal void
DrawPowerup( RENDER_PASS * Pass, POWERUP Powerup, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    flo32 tAlpha = 1.0f;
    flo32 tScale = 1.0f;
    
    flo32 tFadeIn_TargetTime  = 1.0f;
    flo32 tFadeOut_TargetTime = 2.0f;
    
    if( Powerup.Exit_Timer > ( Powerup.Exit_TargetTime - tFadeIn_TargetTime ) ) {
        flo32 t = Clamp01( ( Powerup.Exit_Timer - ( Powerup.Exit_TargetTime - tFadeIn_TargetTime ) ) / tFadeIn_TargetTime );
        tAlpha = 1.0f - t;
        tScale = lerp( 1.0f, t, 1.1f );
    }
    if( Powerup.Exit_Timer < tFadeOut_TargetTime ) {
        tAlpha = Clamp01( Powerup.Exit_Timer / tFadeOut_TargetTime );
    }
    if( Powerup.Health <= 0 ) {
        flo32 t = Clamp01( Powerup.Exit_Timer / POWERUP_PICKUP_ANIM_TARGET_TIME );
        
        tAlpha  = t;
        tScale  = lerp( ( PLAYER_RADIUS / Powerup.Radius ) * 0.8f, t, tScale );
    }
    
    flo32 tFlash = Clamp01( cosf( Powerup.Flash_Radians ) * 0.5f + 0.5f );
    flo32 tFlash_Alpha = lerp( 0.75f, tFlash, 0.9f );
    DrawRect( Pass, TextureID_Powerup_Body, RectCR( Powerup.Position, Powerup.Radius * tScale ), COLOR_WHITE * tAlpha * tFlash_Alpha );
    
    DrawPowerupIcon( Pass, Powerup.Type, Powerup.Position, Powerup.Radius * 0.6f * tScale, tAlpha );
}

internal void
DrawPowerup( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    POWERUP_STATE * PowerupS = Draw->PowerupS;
    
    for( int32 iPowerup = 0; iPowerup < PowerupS->nPowerup; iPowerup++ ) {
        POWERUP Powerup = PowerupS->Powerup[ iPowerup ];
        
        DrawPowerup( Pass, Powerup );
    }
}

internal void
FinalizePowerup( APP_STATE * AppState ) {
    POWERUP_STATE * PowerupS = &AppState->PowerupS;
    
    uint32 NewCount = 0;
    for( int32 iPowerup = 0; iPowerup < PowerupS->nPowerup; iPowerup++ ) {
        POWERUP Powerup = PowerupS->Powerup[ iPowerup ];
        
        if( Powerup.Exit_Timer > 0.0f ) {
            PowerupS->Powerup[ NewCount++ ] = Powerup;
        }
    }
    PowerupS->nPowerup = NewCount;
}