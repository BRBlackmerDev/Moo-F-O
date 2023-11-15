
internal void
CaptureReplay( APP_STATE * AppState ) {
    REPLAY_STATE * Replay = &AppState->Replay;
    
    if( !Replay->Show ) {
        REPLAY_FRAME * Frame = Replay->Frame + Replay->AtFrame;
        memcpy( &Frame->Camera,           &AppState->Camera,           sizeof( CAMERA_STATE           ) );
        
        memcpy( &Frame->ScoreS,           &AppState->ScoreS,           sizeof( SCORE_STATE            ) );
        
        memcpy( &Frame->Player,           &AppState->Player,           sizeof( PLAYER_STATE           ) );
        memcpy( &Frame->BulletS,          &AppState->BulletS,          sizeof( BULLET_STATE           ) );
        memcpy( &Frame->EnemyBulletS,     &AppState->EnemyBulletS,     sizeof( ENEMY_BULLET_STATE     ) );
        //memcpy( &Frame->ExplosiveS,            &AppState->ExplosiveS,            sizeof( EXPLOSIVE_STATE             ) );
        memcpy( &Frame->TurretS,          &AppState->TurretS,          sizeof( TURRET_STATE           ) );
        memcpy( &Frame->SpitterS,          &AppState->SpitterS,          sizeof( SPITTER_STATE           ) );
        memcpy( &Frame->ChaserS,          &AppState->ChaserS,          sizeof( CHASER_STATE           ) );
        memcpy( &Frame->SwarmS,           &AppState->SwarmS,           sizeof( SWARM_STATE            ) );
        //memcpy( &Frame->BumperS,          &AppState->BumperS,          sizeof( BUMPER_STATE           ) );
        memcpy( &Frame->DrifterS,         &AppState->DrifterS,         sizeof( DRIFTER_STATE          ) );
        memcpy( &Frame->PowerupS,         &AppState->PowerupS,         sizeof( POWERUP_STATE          ) );
        memcpy( &Frame->LooperS,          &AppState->LooperS,          sizeof( LOOPER_STATE           ) );
        //memcpy( &Frame->StarburstS,       &AppState->StarburstS,       sizeof( STARBURST_STATE        ) );
        memcpy( &Frame->AutoTurretS,      &AppState->AutoTurretS,      sizeof( AUTO_TURRET_STATE      ) );
        memcpy( &Frame->BoardRoomS,      &AppState->BoardRoomS,      sizeof( BOARD_ROOM_STATE      ) );
        memcpy( &Frame->BayDoorS,         &AppState->BayDoorS,         sizeof( BAY_DOOR_STATE         ) );
        memcpy( &Frame->AsteroidS,        &AppState->AsteroidS,        sizeof( ASTEROID_STATE         ) );
        memcpy( &Frame->MinerS,           &AppState->MinerS,           sizeof( MINER_STATE            ) );
        memcpy( &Frame->BombS,          &AppState->BombS,          sizeof( BOMB_STATE           ) );
        memcpy( &Frame->BombLauncherS,    &AppState->BombLauncherS,    sizeof( BOMB_LAUNCHER_STATE    ) );
        //memcpy( &Frame->MissileLauncherS,    &AppState->MissileLauncherS,    sizeof( MISSILE_LAUNCHER_STATE    ) );
        //memcpy( &Frame->BankS,            &AppState->BankS,            sizeof( BANK_STATE             ) );
        memcpy( &Frame->BlimpS,           &AppState->BlimpS,           sizeof( BLIMP_STATE            ) );
        memcpy( &Frame->ReconS,           &AppState->ReconS,           sizeof( RECON_STATE            ) );
        memcpy( &Frame->SatelliteS,       &AppState->SatelliteS,       sizeof( SATELLITE_STATE        ) );
        //memcpy( &Frame->SmogS,            &AppState->SmogS,            sizeof( SMOG_STATE             ) );
        //memcpy( &Frame->EscortS,          &AppState->EscortS,          sizeof( ESCORT_STATE           ) );
        //memcpy( &Frame->BoltS,            &AppState->BoltS,            sizeof( BOLT_STATE             ) );
        memcpy( &Frame->GunnerS,          &AppState->GunnerS,          sizeof( GUNNER_STATE           ) );
        //memcpy( &Frame->RogueS,           &AppState->RogueS,           sizeof( ROGUE_STATE            ) );
        memcpy( &Frame->MissileS,         &AppState->MissileS,         sizeof( MISSILE_STATE         ) );
        memcpy( &Frame->WormS,           &AppState->WormS,           sizeof( WORM_STATE            ) );
        memcpy( &Frame->FireballS,         &AppState->FireballS,         sizeof( FIREBALL_STATE         ) );
        memcpy( &Frame->EntitySpawnS,     &AppState->EntitySpawnS,     sizeof( ENTITY_SPAWN_STATE     ) );
        memcpy( &Frame->ScoreMultiplierS, &AppState->ScoreMultiplierS, sizeof( SCORE_MULTIPLIER_STATE ) );
        memcpy( &Frame->ScoreParticleS,   &AppState->ScoreParticleS,   sizeof( SCORE_PARTICLE_STATE   ) );
        memcpy( &Frame->ParticleS, &AppState->ParticleS, sizeof( PARTICLE_STATE ) );
        //memcpy( &Frame->ProgressS, &AppState->ProgressS, sizeof( PROGRESS_STATE ) );
        memcpy( &Frame->EndLevel,  &AppState->EndLevel,  sizeof( END_LEVEL ) );
        
        Replay->AtFrame = ( Replay->AtFrame + 1 ) % REPLAY_FRAME_MAX_COUNT;
    }
}