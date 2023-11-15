
internal void
UpdateMinerLaserCollision( APP_STATE * AppState, flo32 dT ) {
    MINER_STATE    * MinerS    = &AppState->MinerS;
    
    ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
    BOMB_STATE   * BombS   = &AppState->BombS;
    
    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
        MINER * Miner = MinerS->Miner + iMiner;
        
        for( int32 iLaser = 0; ( Miner->Health > 0 ) && ( iLaser < Miner->nLaser ); iLaser++ ) {
            MINER_LASER * Laser = Miner->Laser + iLaser;
            
            if( Laser->IsActive ) {
                boo32       DoDamage         = false;
                ENTITY_TYPE DoDamage_Type    = {};
                int32       DoDamage_iEntity = 0;
                
                flo32 Radians = Miner->AtRadians + Laser->Radians;
                vec2  Dir    = ToDirection2D( Radians );
                vec2  V       = Dir * Miner->Laser_MaxLength;
                
                Laser->Dest = Miner->Position + V;
                
                RAY2 Ray = Ray2( Miner->Position, V );
                
                { // Intersect Laser
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    BestIntersect.t = 1.0f;
                    
                    { // PLAYER SHIELD
                        PLAYER_STATE * Player = &AppState->Player;
                        if( Player->Shield_IsActive ) {
                            circ Bound = Circ( Player->Position, PLAYER_SHIELD_RADIUS + MINER_LASER_WIDTH * 0.5f );
                            DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                        }
                    }
                    
#if 0                    
                    { // BUMPER
                        BUMPER_STATE   * BumperS   = &AppState->BumperS;
                        for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
                            BUMPER * Bumper = BumperS->Bumper + iBumper;
                            
                            if( Bumper->Health > 0 ) {
                                circ Bound = Circ( Bumper->Position, Bumper->Radius + MINER_LASER_WIDTH * 0.5f );
                                DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                            }
                        }
                    }
#endif
                    
                    { // MINER
                        for( int32 iMinerA = 0; iMinerA < MinerS->nMiner; iMinerA++ ) {
                            if( iMinerA != iMiner ) {
                                MINER * MinerA = MinerS->Miner + iMinerA;
                                
                                if( MinerA->Health > 0 ) {
                                    circ Bound = Circ( MinerA->Position, MinerA->Radius + MINER_LASER_WIDTH * 0.5f );
                                    
                                    boo32 NewIntersect = DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                                    if( NewIntersect ) {
                                        DoDamage         = true;
                                        DoDamage_Type    = EntityType_Miner;
                                        DoDamage_iEntity = iMinerA;
                                    }
                                }
                            }
                        }
                    }
                    
                    { // MEGA_MINER
                        for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
                            MEGA_MINER * MegaMiner = MinerS->MegaMiner + iMegaMiner;
                            
                            if( MegaMiner->Health > 0 ) {
                                { // Intersect Drill
                                    vec2 yAxis = GetNormal( ToDirection2D( MegaMiner->AtRadians ) );
                                    vec2 xAxis = -GetPerp( yAxis );
                                    
                                    vec2 bPos  = MegaMiner->Position + yAxis * MEGA_MINER_DRILL_OFFSET;
                                    vec2 Scale = MEGA_MINER_DRILL_SCALE * 1.2f;
                                    vec2 A     = bPos + xAxis * -Scale.x + yAxis * -Scale.y;
                                    vec2 B     = bPos                    + yAxis *  Scale.y;
                                    vec2 C     = bPos + xAxis *  Scale.x + yAxis * -Scale.y;
                                    
                                    DoesRayIntersectTriangle( &BestIntersect, Ray, A, B, C );
                                }
                                
                                
                                circ Bound = Circ( MegaMiner->Position, MegaMiner->Radius + MINER_LASER_WIDTH * 0.5f );
                                
                                boo32 NewIntersect = DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                                if( NewIntersect ) {
                                    DoDamage         = true;
                                    DoDamage_Type    = EntityType_MegaMiner;
                                    DoDamage_iEntity = iMegaMiner;
                                }
                            }
                        }
                    }
                    
#if 0                    
                    { // BOMBER
                        for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
                            BOMBER * Bomber = BombS->Bomber + iBomber;
                            
                            if( Bomber->Health > 0 ) {
                                circ Bound = Circ( Bomber->Position, Bomber->Radius + MINER_LASER_WIDTH * 0.5f );
                                boo32 NewIntersect = DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                                if( NewIntersect ) {
                                    DoDamage         = true;
                                    DoDamage_Type    = EntityType_Bomber;
                                    DoDamage_iEntity = iBomber;
                                }
                            }
                        }
                    }
#endif
                    
                    { // ASTEROID
                        for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                            ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                            
                            if( ( Asteroid->Health > 0 ) && ( Asteroid->Class > AsteroidClass_Tiny ) ) {
                                circ Bound = Circ( Asteroid->Position, Asteroid->Radius + MINER_LASER_WIDTH * 0.5f );
                                boo32 NewIntersect = DoesRayIntersectCircleExt( &BestIntersect, Ray, Bound );
                                if( NewIntersect ) {
                                    DoDamage         = true;
                                    DoDamage_Type    = EntityType_Asteroid;
                                    DoDamage_iEntity = iAsteroid;
                                }
                            }
                        }
                    }
                    
                    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                        Laser->Dest = BestIntersect.P;
                    }
                    
                    if( DoDamage ) {
                        switch( DoDamage_Type ) {
                            case EntityType_Miner: {
                                MINER * Target = MinerS->Miner + DoDamage_iEntity;
                                
                                Target->LaserDamage_Timer += dT;
                                while( Target->LaserDamage_Timer >= MINER_LASER_DAMAGE_TARGET_TIME ) {
                                    Target->Health             = MaxValue( Target->Health - 1, 0 );
                                    Target->LaserDamage_Timer -= MINER_LASER_DAMAGE_TARGET_TIME;
                                }
                                
                                AddMinerLaserHitParticles( AppState, Laser->Dest, -Dir );
                            } break;
                            
                            case EntityType_MegaMiner: {
                                MEGA_MINER * Target = MinerS->MegaMiner + DoDamage_iEntity;
                                
                                Target->LaserDamage_Timer += dT;
                                while( Target->LaserDamage_Timer >= MEGA_MINER_LASER_DAMAGE_TARGET_TIME ) {
                                    Target->Health             = MaxValue( Target->Health - 1, 0 );
                                    Target->LaserDamage_Timer -= MEGA_MINER_LASER_DAMAGE_TARGET_TIME;
                                }
                                
                                AddMinerLaserHitParticles( AppState, Laser->Dest, -Dir );
                            } break;
                            
#if 0                            
                            case EntityType_Bomber: {
                                BOMBER * Target = BombS->Bomber + DoDamage_iEntity;
                                
                                Target->LaserDamage_Timer += dT;
                                while( Target->LaserDamage_Timer >= MINER_LASER_DAMAGE_TARGET_TIME ) {
                                    Target->Health             = MaxValue( Target->Health - 1, 0 );
                                    Target->LaserDamage_Timer -= MINER_LASER_DAMAGE_TARGET_TIME;
                                }
                                
                                AddMinerLaserHitParticles( AppState, Laser->Dest, -Dir );
                            } break;
#endif
                            
                            case EntityType_Asteroid: {
                                ASTEROID * Target = AsteroidS->Asteroid + DoDamage_iEntity;
                                AddMinerLaserHitParticles( AppState, Laser->Dest, -Dir );
                                
                                if( Target->Flags & ASTEROID_FLAG__LASER_ONLY ) {
                                    Target->Health    = MaxValue( Target->Health - 1, 0 );
                                    if( Target->Health == 0 ) {
                                        Miner->Health                 = 0;
                                        Miner->DoLaserOnlyKill        = true;
                                        Miner->DoLaserOnlyKill_iLaser = iLaser;
                                    }
                                }
                            } break;
                        }
                    }
                }
                
                { // Kill AsteroidClass_Tiny
                    ASTEROID_CLASS_RADIUS;
                    
                    flo32 Radius = AsteroidClassRadius[ AsteroidClass_Tiny ] + MINER_LASER_WIDTH * 0.5f;
                    CAPSULE2 Cap = Capsule2( Miner->Position, Laser->Dest, Radius );
                    
                    for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                        ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                        
                        if( ( Asteroid->Class == AsteroidClass_Tiny ) && ( IsInCapsule( Asteroid->Position, Cap ) ) ) {
                            Asteroid->Health = 0;
                        }
                    }
                }
                
                { // Kill Swarm
                    SWARM_STATE * SwarmS = &AppState->SwarmS;
                    
                    flo32 Radius = SWARM_RADIUS + MINER_LASER_WIDTH * 0.5f;
                    CAPSULE2 Cap = Capsule2( Miner->Position, Laser->Dest, Radius );
                    
                    for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
                        SWARM * Swarm = SwarmS->Swarm + iSwarm;
                        
                        if( IsInCapsule( Swarm->Position, Cap ) ) {
                            Swarm->Health = 0;
                            
                            AddEntityDeathParticles( AppState, Swarm->Position, SWARM_BASE_COLOR );
                        }
                    }
                }
            }
        }
    }
}


#if 0
internal void
UpdateExplosiveCollision( APP_STATE * AppState ) {
    EXPLOSIVE_STATE * ExplosiveS = &AppState->ExplosiveS;
    TURRET_STATE    * TurretS    = &AppState->TurretS;
    ASTEROID_STATE  * AsteroidS  = &AppState->AsteroidS;
    MINER_STATE     * MinerS     = &AppState->MinerS;
    BOMB_STATE    * BombS    = &AppState->BombS;
    
    for( uint32 iExplosive = 0; iExplosive < ExplosiveS->nExplosive; iExplosive++ ) {
        EXPLOSIVE * Explosive = ExplosiveS->Explosive + iExplosive;
        if( ( Explosive->Health > 0 ) && ( Explosive->DoArmed ) ) {
            boo32 DoExplode = false;
            
            { // ASTEROID
                for( int32 iAsteroid = 0; ( !DoExplode ) && ( iAsteroid < AsteroidS->nAsteroid ); iAsteroid++ ) {
                    ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                    
                    if( Asteroid->Health > 0 ) {
                        flo32 DistSq   = GetLengthSq( Explosive->Position - Asteroid->Position );
                        flo32 Radius   = Asteroid->Radius + EXPLOSIVE_RADIUS - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        if( DistSq <= RadiusSq ) {
                            DoExplode = true;
                        }
                    }
                }
            }
            
            { // MINER
                for( int32 iMiner = 0; ( !DoExplode ) && ( iMiner < MinerS->nMiner ); iMiner++ ) {
                    MINER * Miner = MinerS->Miner + iMiner;
                    
                    if( Miner->Health > 0 ) {
                        flo32 DistSq   = GetLengthSq( Explosive->Position - Miner->Position );
                        flo32 Radius   = Miner->Radius + EXPLOSIVE_RADIUS - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        if( DistSq <= RadiusSq ) {
                            DoExplode = true;
                        }
                    }
                }
            }
            
            { // BOMBER
                for( int32 iBomber = 0; ( !DoExplode ) && ( iBomber < BombS->nBomber ); iBomber++ ) {
                    BOMBER * Bomber = BombS->Bomber + iBomber;
                    
                    if( Bomber->Health > 0 ) {
                        flo32 DistSq   = GetLengthSq( Explosive->Position - Bomber->Position );
                        flo32 Radius   = Bomber->Radius + EXPLOSIVE_RADIUS - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        if( DistSq <= RadiusSq ) {
                            DoExplode = true;
                        }
                    }
                }
            }
            
            { // BANK
                BANK_STATE * BankS = &AppState->BankS;
                for( int32 iBank = 0; ( !DoExplode ) && ( iBank < BankS->nBank ); iBank++ ) {
                    BANK * Bank = BankS->Bank + iBank;
                    
                    if( Bank->Health > 0 ) {
                        flo32 DistSq   = GetLengthSq( Explosive->Position - Bank->Position );
                        flo32 Radius   = Bank->Radius + EXPLOSIVE_RADIUS - 0.1f;
                        flo32 RadiusSq = Radius * Radius;
                        if( DistSq <= RadiusSq ) {
                            DoExplode = true;
                        }
                    }
                }
            }
            
            if( DoExplode ) {
                Explosive->Health = 0;
                
                { // ASTEROID
                    for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                        ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                        
                        if( Asteroid->Health > 0 ) {
                            flo32 DistSq   = GetLengthSq( Explosive->Position - Asteroid->Position );
                            flo32 Radius   = ( Asteroid->Radius - 0.25f ) + EXPLOSIVE_EXPLODE_RADIUS;
                            flo32 RadiusSq = Radius * Radius;
                            if( DistSq <= RadiusSq ) {
                                if( !( Asteroid->Flags & ASTEROID_FLAG__LASER_ONLY ) ) {
                                    Asteroid->Health = 0;
                                }
                                
                                vec2 Velocity = Asteroid->Direction * Asteroid->Speed;
                                vec2 Norm     = GetNormal( Asteroid->Position - Explosive->Position );
                                
                                flo32 DotVN   = dot( Velocity, Norm );
                                if( DotVN < 0.0f ) {
                                    Velocity  -= DotVN * Norm;
                                }
                                
                                Velocity     += Norm * 4.0f;
                                
                                Asteroid->Direction = GetNormal( Velocity );
                                Asteroid->Speed     = GetLength( Velocity );
                            }
                        }
                    }
                }
                
                { // MINER
                    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                        MINER * Miner = MinerS->Miner + iMiner;
                        
                        if( Miner->Health > 0 ) {
                            flo32 DistSq   = GetLengthSq( Explosive->Position - Miner->Position );
                            flo32 Radius   = ( Miner->Radius - 0.25f ) + EXPLOSIVE_EXPLODE_RADIUS;
                            flo32 RadiusSq = Radius * Radius;
                            if( DistSq <= RadiusSq ) {
                                Miner->Health = 0;
                            }
                        }
                    }
                }
                
                { // BOMBER
                    for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
                        BOMBER * Bomber = BombS->Bomber + iBomber;
                        
                        if( Bomber->Health > 0 ) {
                            flo32 DistSq   = GetLengthSq( Explosive->Position - Bomber->Position );
                            flo32 Radius   = ( Bomber->Radius - 0.25f ) + EXPLOSIVE_EXPLODE_RADIUS;
                            flo32 RadiusSq = Radius * Radius;
                            if( DistSq <= RadiusSq ) {
                                Bomber->Health = 0;
                            }
                        }
                    }
                }
                
                { // EXPLOSIVE
                    for( uint32 iTargetExplosive = 0; iTargetExplosive < ExplosiveS->nExplosive; iTargetExplosive++ ) {
                        EXPLOSIVE * TargetExplosive = ExplosiveS->Explosive + iTargetExplosive;
                        
                        if( ( iTargetExplosive != iExplosive ) && ( TargetExplosive->Health > 0 ) ) {
                            flo32 DistSq   = GetLengthSq( Explosive->Position - TargetExplosive->Position );
                            flo32 Radius   = ( EXPLOSIVE_RADIUS - 0.25f ) + EXPLOSIVE_EXPLODE_RADIUS;
                            flo32 RadiusSq = Radius * Radius;
                            if( DistSq <= RadiusSq ) {
                                TargetExplosive->DoArmed = true;
                                
                                vec2 Velocity = TargetExplosive->Direction * TargetExplosive->Speed;
                                vec2 Norm     = GetNormal( TargetExplosive->Position - Explosive->Position );
                                
                                flo32 DotVN   = dot( Velocity, Norm );
                                if( DotVN < 0.0f ) {
                                    Velocity  -= DotVN * Norm;
                                }
                                
                                Velocity     += Norm * 4.0f;
                                
                                TargetExplosive->Direction = GetNormal( Velocity );
                                TargetExplosive->Speed     = GetLength( Velocity );
                            }
                        }
                    }
                }
                
                { // BOMB
                    for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
                        BOMB * Bomb = BombS->Bomb + iBomb;
                        
                        if( !Bomb->IsDetonated ) {
                            flo32 Radius   = ( EXPLOSIVE_EXPLODE_RADIUS + Bomb->Radius );
                            flo32 RadiusSq = Radius * Radius;
                            flo32 DistSq   = GetLengthSq( Explosive->Position - Bomb->Position );
                            
                            if( DistSq <= RadiusSq ) {
                                if( !Bomb->IsTriggered ) {
                                    Bomb->IsTriggered = true;
                                    Bomb->Timer       = MaxValue( Bomb->Timer, BOMB_DETONATE_TARGET_TIME - BOMB_DETONATE_BOMB_TARGET_TIME );
                                }
                                
                                vec2 Velocity = Bomb->Velocity;
                                vec2 Norm     = GetNormal( Bomb->Position - Explosive->Position );
                                
                                flo32 DotVN   = dot( Velocity, Norm );
                                if( DotVN < 0.0f ) {
                                    Velocity  -= DotVN * Norm;
                                }
                                
                                Velocity     += Norm * 4.0f;
                                
                                Bomb->Velocity = Velocity;
                            }
                        }
                    }
                }
                
                { // BUMPER
                    BUMPER_STATE * BumperS = &AppState->BumperS;
                    for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
                        BUMPER * Bumper = BumperS->Bumper + iBumper;
                        
                        if( Bumper->Health > 0 ) {
                            flo32 DistSq   = GetLengthSq( Explosive->Position - Bumper->Position );
                            flo32 Radius   = ( Bumper->Radius - 0.25f ) + EXPLOSIVE_EXPLODE_RADIUS;
                            flo32 RadiusSq = Radius * Radius;
                            if( DistSq <= RadiusSq ) {
                                Bumper->Health = 0;
                            }
                        }
                    }
                }
                
                { // BANK
                    BANK_STATE * BankS = &AppState->BankS;
                    for( int32 iBank = 0; iBank < BankS->nBank; iBank++ ) {
                        BANK * Bank = BankS->Bank + iBank;
                        
                        if( Bank->Health > 0 ) {
                            flo32 DistSq   = GetLengthSq( Explosive->Position - Bank->Position );
                            flo32 Radius   = ( Bank->Radius - 0.25f ) + EXPLOSIVE_EXPLODE_RADIUS;
                            flo32 RadiusSq = Radius * Radius;
                            if( DistSq <= RadiusSq ) {
                                POINTS_ENTRY * PointsEntry      = GetPointsEntryForEntity( AppState, EntityType_Bank_Hit );
                                int32          nScoreMultiplier = PointsEntry->nScoreMultiplier;
                                
                                vec2 Pos = ClampToRect( Bank->Position, AppState->World_Bound );
                                for( int32 iScoreMultiplier = 0; iScoreMultiplier < nScoreMultiplier; iScoreMultiplier++ ) {
                                    AddScoreMultiplier( AppState, NewScoreMultiplier( Pos, AppState->World_Bound, true ) );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
#endif

internal void
UpdateMinerAndBomberRepel( APP_STATE * AppState ) {
    MINER_STATE    * MinerS    = &AppState->MinerS;
    BOMB_STATE   * BombS   = &AppState->BombS;
    
    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
        MINER * Miner = MinerS->Miner + iMiner;
        
        for( int32 iRepel = iMiner + 1; iRepel < MinerS->nMiner; iRepel++ ) {
            MINER * Repel = MinerS->Miner + iRepel;
            
            vec2 V = Repel->Position - Miner->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = MINER_REPEL_RADIUS + MINER_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * MINER_REPEL_SPEED;
                
                Miner->Repel -= Repel_Vector;
                Repel->Repel += Repel_Vector;
            }
        }
        
        for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
            MEGA_MINER * MegaMiner = MinerS->MegaMiner + iMegaMiner;
            
            vec2 V = MegaMiner->Position - Miner->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = MEGA_MINER_REPEL_RADIUS + MINER_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * MINER_REPEL_SPEED;
                
                Miner->Repel -= Repel_Vector;
            }
        }
    }
    
#if 0    
    for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
        BOMBER * Bomber = BombS->Bomber + iBomber;
        
        for( int32 iRepel = iBomber + 1; iRepel < BombS->nBomber; iRepel++ ) {
            BOMBER * Repel = BombS->Bomber + iRepel;
            
            vec2 V = Repel->Position - Bomber->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = BOMBER_REPEL_RADIUS + BOMBER_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Repel_Vector = GetNormal( V ) * BOMBER_REPEL_SPEED;
                
                Bomber->Repel -= Repel_Vector;
                Repel->Repel  += Repel_Vector;
            }
        }
    }
#endif
    
#if 0    
    for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
        MINER * Miner = MinerS->Miner + iMiner;
        for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
            BOMBER * Bomber = BombS->Bomber + iBomber;
            
            vec2 V = Bomber->Position - Miner->Position;
            
            flo32 DistSq   = GetLengthSq( V );
            flo32 Radius   = MINER_REPEL_RADIUS + BOMBER_REPEL_RADIUS;
            flo32 RadiusSq = Radius * Radius;
            if( DistSq <= RadiusSq ) {
                vec2 Norm = GetNormal( V );
                
                Miner->Repel  -= Norm * BOMBER_REPEL_SPEED;
                Bomber->Repel += Norm * MINER_REPEL_SPEED;
            }
        }
    }
#endif
}

internal void
UpdateBulletCollision( APP_STATE * AppState ) {
    BULLET_STATE * BulletS = &AppState->BulletS;
    
    int32 nBullet = BulletS->nBullet;
    for( int32 iBullet = 0; iBullet < nBullet; iBullet++ ) {
        BULLET * Bullet = BulletS->Bullet + iBullet;
        
        vec2 A = Bullet->PrevPos;
        vec2 B = Bullet->Position + Bullet->Direction * BULLET_HEIGHT;
        CAPSULE2 bCap = Capsule2( A, B, BULLET_HALF_WIDTH * 2.0f );
        RAY2     Ray  = Ray2( A, B - A );
        
        { // TURRET
            TURRET_STATE * TurretS = &AppState->TurretS;
            
            for( int32 iMegaTurret = 0; ( Bullet->Health > 0 ) && ( iMegaTurret < TurretS->nMegaTurret ); iMegaTurret++ ) {
                TURRET * MegaTurret = TurretS->MegaTurret + iMegaTurret;
                
                if( MegaTurret->Health > 0 ) {
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    
                    boo32 HitShield            = false;
                    boo32 HitMegaTurret        = false;
                    boo32 PlayerOverlapsTurret = false;
                    
                    if( Bullet->Health > 0 ) {
                        flo32 Radius = MegaTurret->Radius + BULLET_WIDTH + 0.1f;
                        // TODO: Is this epsilon constant across all entities? Should I tune it bigger?
                        
                        HitMegaTurret = DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( MegaTurret->Position, Radius ) );
                        
                        if( ( !HitMegaTurret ) && ( IsInCapsule( MegaTurret->Position, Capsule2( A, B, Radius ) ) ) ) {
                            HitMegaTurret         = true;
                            BestIntersect.IsValid = true;
                            BestIntersect.t       = 0.0f;
                            PlayerOverlapsTurret  = true;
                        }
                    }
                    
                    if( Bullet->Health > 0 ) {
                        ENTITY_SHIELD Shield = MegaTurret->Shield;
                        if( Shield.HasShield ) {
                            HitShield = CheckShieldCollisionWithRay( &BestIntersect, Shield, MegaTurret->Shield_SegmentT, MegaTurret->Position, MegaTurret->Radius, A, B, BULLET_WIDTH );
                        }
                    }
                    
                    if( ( HitShield ) && ( BestIntersect.t <= 1.0f ) ) {
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision_N = BestIntersect.N;
                        Bullet->DrawCollision_P = BestIntersect.P;
                        Bullet->DrawCollision   = true;
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, COLOR_WHITE );
                    } else if( ( HitMegaTurret ) && ( BestIntersect.t <= 1.0f ) ) {
                        MegaTurret->Health       = MaxValue( MegaTurret->Health - 1, 0 );
                        MegaTurret->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( PlayerOverlapsTurret ) {
                            Bullet->DrawCollision_N = Bullet->Direction;
                            Bullet->DrawCollision_P = A;
                        } else {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - MegaTurret->Position );
                            Bullet->DrawCollision_P = MegaTurret->Position + Bullet->DrawCollision_N * MegaTurret->Radius;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, MEGA_TURRET_BASE_COLOR );
                    }
                }
            }
            
            for( int32 iTurret = 0; ( Bullet->Health > 0 ) && ( iTurret < TurretS->nTurret ); iTurret++ ) {
                TURRET * Turret = TurretS->Turret + iTurret;
                
                if( Turret->Health > 0 ) {
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    
                    boo32 HitShield            = false;
                    boo32 HitTurret            = false;
                    boo32 PlayerOverlapsTurret = false;
                    
                    if( Bullet->Health > 0 ) {
                        flo32 Radius = Turret->Radius + BULLET_WIDTH + 0.1f;
                        // TODO: Is this epsilon constant across all entities? Should I tune it bigger?
                        
                        HitTurret = DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Turret->Position, Radius ) );
                        if( ( !HitTurret ) && ( IsInCapsule( Turret->Position, Capsule2( A, B, Radius ) ) ) ) {
                            HitTurret             = true;
                            BestIntersect.IsValid = true;
                            BestIntersect.t       = 0.0f;
                            PlayerOverlapsTurret  = true;
                        }
                    }
                    
                    if( Bullet->Health > 0 ) {
                        ENTITY_SHIELD Shield = Turret->Shield;
                        if( Shield.HasShield ) {
                            HitShield = CheckShieldCollisionWithRay( &BestIntersect, Shield, Turret->Shield_SegmentT, Turret->Position, Turret->Radius, A, B, BULLET_WIDTH );
                        }
                    }
                    
                    if( ( HitShield ) && ( BestIntersect.t <= 1.0f ) ) {
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision_N = BestIntersect.N;
                        Bullet->DrawCollision_P = BestIntersect.P;
                        Bullet->DrawCollision   = true;
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, COLOR_WHITE );
                    } else if( ( HitTurret )  && ( BestIntersect.t <= 1.0f ) ) {
                        Turret->Health       = MaxValue( Turret->Health - 1, 0 );
                        Turret->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( PlayerOverlapsTurret ) {
                            Bullet->DrawCollision_N = Bullet->Direction;
                            Bullet->DrawCollision_P = A;
                        } else {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Turret->Position );
                            Bullet->DrawCollision_P = Turret->Position + Bullet->DrawCollision_N * Turret->Radius;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, TURRET_BASE_COLOR );
                    }
                }
            }
        }
        
        { // CHASER
            CHASER_STATE * ChaserS = &AppState->ChaserS;
            
            for( int32 iChaser = 0; ( Bullet->Health > 0 ) && ( iChaser < ChaserS->nChaser ); iChaser++ ) {
                CHASER * Chaser = ChaserS->Chaser + iChaser;
                
                if( Chaser->Health > 0 ) {
                    flo32 Radius = Chaser->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Chaser->Position, Radius ) );
                    
                    if( ( IsInCapsule( Chaser->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        Chaser->Health = MaxValue( Chaser->Health - 1, 0 );
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Chaser->Position );
                            Bullet->DrawCollision_P = Chaser->Position + Bullet->DrawCollision_N * Chaser->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, CHASER_BASE_COLOR );
                    }
                }
            }
        }
        
        { // DRIFTER
            DRIFTER_STATE * DrifterS = &AppState->DrifterS;
            
            for( int32 iDrifter = 0; ( Bullet->Health > 0 ) && ( iDrifter < DrifterS->nDrifter ); iDrifter++ ) {
                DRIFTER * Drifter = DrifterS->Drifter + iDrifter;
                
                if( Drifter->Health > 0 ) {
                    flo32 Radius = Drifter->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Drifter->Position, Radius ) );
                    
                    if( ( IsInCapsule( Drifter->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        int32 Drifter_Damage = MinValue( Drifter->Health, 1 );
                        Drifter->Health = MaxValue( Drifter->Health - 1, 0 );
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Drifter->Position );
                            Bullet->DrawCollision_P = Drifter->Position + Bullet->DrawCollision_N * Drifter->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, DRIFTER_BASE_COLOR );
                    }
                }
            }
        }
        
        { // LOOPER
            LOOPER_STATE * LooperS = &AppState->LooperS;
            
            for( int32 iLooper = 0; ( Bullet->Health > 0 ) && ( iLooper < LooperS->nLooper ); iLooper++ ) {
                LOOPER * Looper = LooperS->Looper + iLooper;
                
                if( Looper->Health > 0 ) {
                    flo32 bRadians = ( 1.0f - Clamp01( Looper->Loop_Timer / LOOPER_LOOP_CYCLE_TARGET_TIME ) ) * TAU - PI;
                    vec2  bPos     = Looper->Position + ToDirection2D( bRadians ) * Looper->Loop_Radius;
                    flo32 Radius   = LOOPER_RADIUS + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( bPos, Radius ) );
                    
                    if( ( IsInCapsule( Looper->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        Looper->Health = MaxValue( Looper->Health - 1, 0 );
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - bPos );
                            Bullet->DrawCollision_P = bPos + Bullet->DrawCollision_N * LOOPER_RADIUS;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, LOOPER_BASE_COLOR );
                    }
                }
            }
        }
        
        { // PERSONAL_POD
            BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
            
            // NOTE : Bullets will go through the Personal Pods if the Player is using the Shield Powerup to overlap the Personal Pod. It's fine. They are too insignificant to really care. Just making a note of it.
            
            for( int32 iPersonalPod = 0; ( Bullet->Health > 0 ) && ( iPersonalPod < BayDoorS->nPersonalPod ); iPersonalPod++ ) {
                PERSONAL_POD * PersonalPod = BayDoorS->PersonalPod + iPersonalPod;
                
                if( PersonalPod->Health > 0 ) {
                    vec2 Radius = PERSONAL_POD_RADIUS + Vec2( BULLET_WIDTH + 0.1f, BULLET_WIDTH + 0.1f );
                    vec2 Scale  = PERSONAL_POD_RADIUS / Radius;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectEllipse( &BestIntersect, Ray2( A, B - A ), PersonalPod->Position, Radius, ToRadians( PersonalPod->Velocity ) );
                    
                    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                        int32 PersonalPod_Damage = MinValue( PersonalPod->Health, 1 );
                        PersonalPod->Health = MaxValue( PersonalPod->Health - 1, 0 );
                        
                        vec2 V   = ( BestIntersect.P - PersonalPod->Position ) * Scale;
                        vec2 Pos = PersonalPod->Position + V;
                        
                        AddPersonalPodDeathParticles( AppState, PersonalPod->Velocity, Pos, GetNormal( B - A ) );
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision_N = BestIntersect.N;
                        Bullet->DrawCollision_P = Pos;
                        Bullet->DrawCollision   = true;
                    }
                }
            }
        }
        
        { // TRANSPORT
            BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
            
            for( int32 iTransport = 0; ( Bullet->Health > 0 ) && ( iTransport < BayDoorS->nTransport ); iTransport++ ) {
                TRANSPORT * Transport = BayDoorS->Transport + iTransport;
                
                if( Transport->Health > 0 ) {
                    flo32 Radius = Transport->Radius + BULLET_WIDTH * 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Transport->Position, Radius ) );
                    
                    if( ( IsInCapsule( Transport->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        int32 Transport_Damage = MinValue( Transport->Health, 1 );
                        Transport->Health = MaxValue( Transport->Health - 1, 0 );
                        
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Transport->Position );
                            Bullet->DrawCollision_P = Transport->Position + Bullet->DrawCollision_N * Transport->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                    }
                }
            }
        }
        
        { // MINER
            MINER_STATE * MinerS = &AppState->MinerS;
            
            for( int32 iMiner = 0; ( Bullet->Health > 0 ) && ( iMiner < MinerS->nMiner ); iMiner++ ) {
                MINER * Miner = MinerS->Miner + iMiner;
                
                if( Miner->Health > 0 ) {
                    flo32 Radius = Miner->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Miner->Position, Radius ) );
                    
                    if( ( IsInCapsule( Miner->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        Miner->Health       = MaxValue( Miner->Health - 1, 0 );
                        Miner->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Miner->Position );
                            Bullet->DrawCollision_P = Miner->Position + Bullet->DrawCollision_N * Miner->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        if( Miner->IsAutonomous ) {
                            AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, MINER_AUTONOMOUS_BASE_COLOR );
                        } else {
                            AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, MINER_STATIONARY_BASE_COLOR );
                        }
                    }
                }
            }
        }
        
        { // MEGA_MINER
            MINER_STATE * MinerS = &AppState->MinerS;
            
            for( int32 iMegaMiner = 0; ( Bullet->Health > 0 ) && ( iMegaMiner < MinerS->nMegaMiner ); iMegaMiner++ ) {
                MEGA_MINER * MegaMiner = MinerS->MegaMiner + iMegaMiner;
                
                if( MegaMiner->Health > 0 ) {
                    { // Drill Collision
                        vec2 yAxis = GetNormal( ToDirection2D( MegaMiner->AtRadians ) );
                        vec2 xAxis = -GetPerp( yAxis );
                        
                        vec2 bPos  = MegaMiner->Position + yAxis * MEGA_MINER_DRILL_OFFSET;
                        vec2 Scale = MEGA_MINER_DRILL_SCALE;
                        vec2 TriA  = bPos + xAxis * -Scale.x + yAxis * -Scale.y;
                        vec2 TriB  = bPos                    + yAxis *  Scale.y;
                        vec2 TriC  = bPos + xAxis *  Scale.x + yAxis * -Scale.y;
                        
                        RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                        DoesRayIntersectTriangle( &BestIntersect, Ray, TriA, TriB, TriC );
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->Health = MaxValue( Bullet->Health - 1, 0 );
                            
                            Bullet->Position      = BestIntersect.P;
                            
                            vec2 Norm = BestIntersect.N;
                            vec2 Dir  = Bullet->Direction - ( 2.0f * dot( Bullet->Direction, Norm ) ) * Norm;
                            AddBullet( AppState, BulletC( BestIntersect.P, Dir, true ) );
                            
                            Bullet->DrawCollision_N = BestIntersect.N;
                            Bullet->DrawCollision_P = BestIntersect.P;
                            Bullet->DrawCollision   = true;
                        }
                    }
                    
                    {
                        if( Bullet->Health > 0 ) {
                            flo32 Radius = MegaMiner->Radius + BULLET_WIDTH + 0.1f;
                            
                            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                            DoesRayIntersectCircleExt( &BestIntersect, Ray, Circ( MegaMiner->Position, Radius ) );
                            
                            if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                                Bullet->Health = MaxValue( Bullet->Health - 1, 0 );
                                
                                vec2 Pos = BestIntersect.P;
                                Bullet->Position      = Pos;
                                
                                vec2 Norm = BestIntersect.N;
                                vec2 Dir  = Bullet->Direction - ( 2.0f * dot( Bullet->Direction, Norm ) ) * Norm;
                                AddBullet( AppState, BulletC( Pos, Dir, true ) );
                                
                                Bullet->DrawCollision_N = Norm;
                                Bullet->DrawCollision_P = Pos;
                                Bullet->DrawCollision   = true;
                            }
                        }
                    }
                }
            }
        }
        
        { // ASTEROID
            ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
            
            for( int32 iAsteroid = 0; ( Bullet->Health > 0 ) && ( iAsteroid < AsteroidS->nAsteroid ); iAsteroid++ ) {
                ASTEROID * Asteroid = AsteroidS->Asteroid + iAsteroid;
                
                if( ( Asteroid->Health > 0 ) && ( !( Asteroid->Flags & ASTEROID_FLAG__SWARM_ONLY ) ) ) {
                    flo32 Radius = Asteroid->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Asteroid->Position, Radius ) );
                    
                    if( ( IsInCapsule( Asteroid->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        boo32 DoDamage =
                            ( !( Asteroid->Flags & ASTEROID_FLAG__BOMB_ONLY  ) ) &&
                            ( !( Asteroid->Flags & ASTEROID_FLAG__LASER_ONLY ) );
                        
                        if( DoDamage ) {
                            Asteroid->Health       = MaxValue( Asteroid->Health - 1, 0 );
                            Asteroid->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        }
                        
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Asteroid->Position );
                            Bullet->DrawCollision_P = Asteroid->Position + Bullet->DrawCollision_N * Asteroid->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, ASTEROID_DEFAULT_BASE_COLOR );
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, ASTEROID_DEFAULT_BASE_COLOR );
                    }
                }
            }
        }
        
        { // BLIMP
            BLIMP_STATE * BlimpS = &AppState->BlimpS;
            
            for( int32 iBlimp = 0; ( Bullet->Health > 0 ) && ( iBlimp < BlimpS->nBlimp ); iBlimp++ ) {
                BLIMP * Blimp = BlimpS->Blimp + iBlimp;
                
                if( Blimp->Health > 0 ) {
                    flo32 Radius = Blimp->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Blimp->Position, Radius ) );
                    
                    if( ( IsInCapsule( Blimp->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        Blimp->Health       = MaxValue( Blimp->Health - 1, 0 );
                        Blimp->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        Blimp->Display_iSegment = RandomS32InRange( 0, BLIMP_SEGMENT_MAX_COUNT - 1 );
                        Blimp->Display_Timer    = 0.0f;
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            flo32 tRadius = RandomF32InRange( 0.95f, 1.0f );
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Blimp->Position );
                            Bullet->DrawCollision_P = Blimp->Position + Bullet->DrawCollision_N * ( Blimp->Radius * tRadius );
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, BLIMP_BASE_COLORA );
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, BLIMP_BASE_COLORB );
                        
                        flo32 Rate = RandomF32InRange( 1.25f, 1.5f );
                        // PlayAudio( AppState, AudioID_Entity_Death_Tiny, ENTITY_DEATH_VOLUME * 0.375f, Rate );
                    }
                }
            }
        }
        
        { // SATELLITE
            SATELLITE_STATE * SatelliteS = &AppState->SatelliteS;
            
            if( ( SatelliteS->nSatellite > 0 ) && ( SatelliteS->ShieldDown_Timer <= 0.0f ) ) {
                vec2 P = GetBL( SatelliteS->Shield_Bound );
                vec2 Q = GetTL( SatelliteS->Shield_Bound );
                
                RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                DoesRayIntersectEdge( &BestIntersect, Ray, P, Q );
                if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                    Bullet->Health = MaxValue( Bullet->Health - 1, 0 );
                    vec2 Pos = BestIntersect.P;
                    
                    Bullet->Position        = Pos;
                    Bullet->DrawCollision_N = BestIntersect.N;
                    Bullet->DrawCollision_P = BestIntersect.P;
                    Bullet->DrawCollision   = true;
                    
                    vec2 Norm = BestIntersect.N;
                    
                    vec2 Dir = Bullet->Direction - ( 2.0f * dot( Bullet->Direction, Norm ) ) * Norm;
                    AddBullet( AppState, BulletC( Pos, Dir, true ) );
                    
                    SATELLITE_SHIELD_HIT ShieldHit = {};
                    ShieldHit.Pos   = BestIntersect.P;
                    ShieldHit.Scale = RandomF32InRange( 0.9f, 1.1f );
                    ShieldHit.Timer = SATELLITE_SHIELD_HIT_TARGET_TIME;
                    
                    SatelliteS->ShieldHit[ SatelliteS->iShieldHit ] = ShieldHit;
                    SatelliteS->iShieldHit = ( SatelliteS->iShieldHit + 1 ) % SATELLITE_SHIELD_HIT_MAX_COUNT;
                }
            }
        }
        
        { // YACHT
            BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
            
            for( int32 iYacht = 0; ( Bullet->Health > 0 ) && ( iYacht < BayDoorS->nYacht ); iYacht++ ) {
                YACHT * Yacht = BayDoorS->Yacht + iYacht;
                
                if( Yacht->Health > 0 ) {
                    flo32 Radius = YACHT_RADIUS + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray, Circ( Yacht->Position, Radius ) );
                    
                    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                        vec2 Pos = BestIntersect.P;
                        Bullet->Position      = BestIntersect.P;
                        vec2 Norm = GetNormal( Pos - Yacht->Position );
                        vec2 Dir = Bullet->Direction - ( 2.0f * dot( Bullet->Direction, Norm ) ) * Norm;
                        AddBullet( AppState, BulletC( Pos, Dir, true ) );
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        Bullet->DrawCollision_N = GetNormal( Pos - Yacht->Position );
                        Bullet->DrawCollision_P = Yacht->Position + Bullet->DrawCollision_N * YACHT_RADIUS;
                    }
                }
            }
        }
        
        { // BOMB_LAUNCHER
            BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
            
            for( int32 iBombLauncher = 0; ( Bullet->Health > 0 ) && ( iBombLauncher < BombLauncherS->nBombLauncher ); iBombLauncher++ ) {
                BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iBombLauncher;
                
                boo32 CanHit = ( BombLauncher->Health > 0 )
                    && ( ( BombLauncher->Stage == BombLauncherStage_EnableBombLauncher )
                        || ( BombLauncher->Stage == BombLauncherStage_SpawnBombs ) );
                if( CanHit ) {
                    flo32 Radius = BombLauncher->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( BombLauncher->Position, Radius ) );
                    
                    if( ( IsInCapsule( BombLauncher->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        BombLauncher->Health       = MaxValue( BombLauncher->Health - 1, 0 );
                        BombLauncher->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - BombLauncher->Position );
                            Bullet->DrawCollision_P = BombLauncher->Position + Bullet->DrawCollision_N * BombLauncher->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, BOMB_LAUNCHER_BASE_COLOR );
                    }
                }
            }
        }
        
        { // WORM
            WORM_STATE * WormS = &AppState->WormS;
            
            for( int32 iWorm = 0; ( Bullet->Health > 0 ) && ( iWorm < WormS->nWorm ); iWorm++ ) {
                WORM * Worm = WormS->Worm + iWorm;
                
                if( Worm->Health > 0 ) {
                    flo32 Radius = Worm->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Worm->Position, Radius ) );
                    
                    if( ( IsInCapsule( Worm->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        Worm->Health   = MaxValue( Worm->Health - 1, 0 );
                        
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Worm->Position );
                            Bullet->DrawCollision_P = Worm->Position + Bullet->DrawCollision_N * ( Worm->Radius - RandomF32InRange( 0.0f, 0.1f ) );
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        if( Worm->Class == WormClass_Large ) {
                            AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, WORM_BASE_COLOR );
                            AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, TowardBlack( COLOR_RED, 0.25f ) );
                        } else {
                            AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, TowardBlack( COLOR_RED, 0.5f ) );
                        }
                        
                        Worm->ShowDamage_Timer = WORM_SHOW_DAMAGE_TARGET_TIME;
                        
                        {
                            int32 Index     = -1;
                            flo32 MaxDistSq = 0.0f;
                            
                            vec2  bPos      = Worm->Position;
                            
                            flo32 Rad       = WORM_RADIUS_LARGE + 0.1f;
                            flo32 RadiusSq  = Rad * Rad;
                            
                            FIREBALL_STATE * FireballS = &AppState->FireballS;
                            
                            for( int32 iFireball = 0; iFireball < FireballS->nFireball; iFireball++ ) {
                                FIREBALL * Fireball = FireballS->Fireball + iFireball;
                                
                                flo32 DistSq = GetLengthSq( Fireball->Position - bPos );
                                if( ( DistSq <= RadiusSq ) && ( DistSq > MaxDistSq ) ) {
                                    Index     = iFireball;
                                    MaxDistSq = DistSq;
                                }
                            }
                            
                            if( Index > -1 ) {
                                FIREBALL * Fireball = FireballS->Fireball + Index;
                                Fireball->Health = MaxValue( Fireball->Health - 1, 0 );
                            }
                        }
                        
                        for( int32 iWormA = 0; iWormA < WormS->nWorm; iWormA++ ) {
                            if( iWormA != iWorm ) {
                                WORM * WormA = WormS->Worm + iWormA;
                                if( WormA->Health > 0 ) {
                                    flo32 Rad      = Worm->Radius + WormA->Radius - 0.2f;
                                    flo32 RadiusSq = Rad * Rad;
                                    flo32 DistSq   = GetLengthSq( Worm->Position - WormA->Position );
                                    if( DistSq <= RadiusSq ) {
                                        WormA->Health = MaxValue( WormA->Health - 1, 0 );
                                        WormA->ShowDamage_Timer = WORM_SHOW_DAMAGE_TARGET_TIME;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        { // FIREBALL
            FIREBALL_STATE * FireballS = &AppState->FireballS;
            
            for( int32 iFireball = 0; ( Bullet->Health > 0 ) && ( iFireball < FireballS->nFireball ); iFireball++ ) {
                FIREBALL * Fireball = FireballS->Fireball + iFireball;
                
                if( Fireball->Health > 0 ) {
                    flo32 Radius = Fireball->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Fireball->Position, Radius ) );
                    
                    if( ( IsInCapsule( Fireball->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        Fireball->Health = MaxValue( Fireball->Health - 1, 0 );
                        
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = BestIntersect.N;
                            Bullet->DrawCollision_P = Fireball->Position + Bullet->DrawCollision_N * Fireball->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        vec4 bColor = FIREBALL_POINTS_BASE_COLOR;
                        if( Fireball->Type == FireballType_Multiplier ) {
                            bColor = FIREBALL_MULTIPLIER_BASE_COLOR;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, bColor );
                    }
                }
            }
        }
        
        { // BARREL
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            for( int32 iBarrel = 0; ( Bullet->Health > 0 ) && ( iBarrel < SpitterS->nBarrel ); iBarrel++ ) {
                BARREL * Barrel = SpitterS->Barrel + iBarrel;
                
                vec2  Dim    = BARREL_DIM;
                flo32 Radius = Dim.x * 0.8f;
                
                if( Barrel->Health > 0 ) {
                    CAPSULE2 Cap = bCap;
                    Cap.Radius += ( Radius + 0.1f );
                    
                    if( IsInCapsule( Barrel->Position, Cap ) ) {
                        Barrel->Health  = MaxValue( Barrel->Health - 1, 0 );
                        Bullet->Health = MaxValue( Bullet->Health - 1, 0 );
                    }
                }
            }
        }
        
        { // LOADER
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            for( int32 iLoader = 0; ( Bullet->Health > 0 ) && ( iLoader < SpitterS->nLoader ); iLoader++ ) {
                LOADER * Loader = SpitterS->Loader + iLoader;
                
                vec2  Dim    = LOADER_DIM;
                orect Bound  = ORectCD( Loader->Position, Dim, Loader->Radians );
                
                RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                DoesRayIntersectORect( &BestIntersect, Ray, Bound );
                if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                    Bullet->Health = MaxValue( Bullet->Health - 1, 0 );
                    vec2 Pos = BestIntersect.P;
                    
                    Bullet->Position      = Pos;
                    vec2 Norm = BestIntersect.N;
                    vec2 Dir  = Bullet->Direction - ( 2.0f * dot( Bullet->Direction, Norm ) ) * Norm;
                    AddBullet( AppState, BulletC( Pos, Dir, true ) );
                    
                    Bullet->DrawCollision_N = BestIntersect.N;
                    Bullet->DrawCollision_P = BestIntersect.P;
                    Bullet->DrawCollision   = true;
                }
            }
        }
        
        { // SECURITY_DRONE
            BOARD_ROOM_STATE * BoardRoomS = &AppState->BoardRoomS;
            
            if( ( BoardRoomS->State_IsActive ) && ( Bullet->Health > 0 ) ) {
                OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
                SECURITY_DRONE   * SecurityDrone   = &BoardRoomS->SecurityDrone;
                
                flo32 Radius = SecurityDrone->Radius + BULLET_WIDTH + 0.1f;
                
                RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( SecurityDrone->Position, Radius ) );
                
                if( ( IsInCapsule( SecurityDrone->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                    SecurityDrone->Health   = MaxValue( SecurityDrone->Health - 1, 0 );
                    
                    if( SecurityDrone->Stage != SecurityDroneStage_Stunned ) {
                        SecurityDrone->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                    }
                    
                    
                    Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                    Bullet->DrawCollision   = true;
                    
                    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                        Bullet->DrawCollision_N = GetNormal( BestIntersect.P - SecurityDrone->Position );
                        Bullet->DrawCollision_P = SecurityDrone->Position + Bullet->DrawCollision_N * ( SecurityDrone->Radius - RandomF32InRange( 0.0f, 0.5f ) );
                    } else {
                        Bullet->DrawCollision_P = A;
                        Bullet->DrawCollision_N = Bullet->Direction;
                    }
                    
                    if( ( ObservationDeck->Health >= 2 ) && ( ObservationDeck->Health <= 3 ) ) {
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, SECURITY_DRONE_BASE_COLOR );
                    } else if( ( ObservationDeck->Health >= -1 ) && ( ObservationDeck->Health <= 1 ) ) {
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, SECURITY_DRONE_BASE_COLOR );
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, Bullet->Direction, Bullet->DrawCollision_N, SECURITY_DRONE_BASE_COLOR );
                    }
                }
            }
        }
        
        { // OBSERVATION_DECK
            BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
            OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
            
            if( ( BoardRoomS->State_IsActive ) && ( ObservationDeck->IsPresent ) && ( Bullet->Health > 0 ) ) {
                
                { // Observation Deck
                    flo32 Radius = ObservationDeck->Radius + 0.1f;
                    if( ObservationDeck->Shield_IsActive ) {
                        Radius += ObservationDeck->Shield_Dist;
                    }
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray, Circ( ObservationDeck->Position, Radius ) );
                    if( ( IsInCapsule( ObservationDeck->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = BestIntersect.N;
                            Bullet->DrawCollision_P = BestIntersect.P;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        if( ObservationDeck->Shield_IsActive ) {
                            vec2 Norm = BestIntersect.N;
                            vec2 Dir  = Bullet->Direction - ( 2.0f * dot( Bullet->Direction, Norm ) ) * Norm;
                            AddBullet( AppState, BulletC( BestIntersect.P, Dir, true ) );
                            
                            OBSERVATION_DECK_SHIELD_HIT ShieldHit = {};
                            ShieldHit.Pos     = BestIntersect.P;
                            ShieldHit.Scale   = RandomF32InRange( 0.9f, 1.1f );
                            ShieldHit.Timer   = OBSERVATION_DECK_SHIELD_HIT_TARGET_TIME;
                            ShieldHit.Radians = ToRadians( BestIntersect.P - ObservationDeck->Position );
                            
                            ObservationDeck->ShieldHit[ ObservationDeck->iShieldHit ] = ShieldHit;
                            ObservationDeck->iShieldHit = ( ObservationDeck->iShieldHit + 1 ) % OBSERVATION_DECK_SHIELD_HIT_MAX_COUNT;
                        }
                    }
                }
                
                if( ( Bullet->Health > 0 ) && ( ObservationDeck->Barrier_IsActive ) ) {
                    // NOTE : Bullets will not collide with the Barrier if the Player is overlapping the Barrier using the Shield Powerup. It's fine. I am intentionally ignoring this. I don't think it will matter or anyone will notice.
                    
                    CAPSULE2 Cap = bCap;
                    Cap.Radius += ( ObservationDeck->Radius + ObservationDeck->Barrier_Dist + OBSERVATION_DECK_BARRIER_WIDTH + 0.1f );
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    
                    int32 nBarrier = OBSERVATION_DECK_BARRIER_COUNT;
                    
                    flo32 Radius_Inner = ObservationDeck->Radius + ObservationDeck->Barrier_Dist;
                    flo32 Radius_Outer = Radius_Inner + OBSERVATION_DECK_BARRIER_WIDTH;
                    
                    for( int32 iBarrier = 0; iBarrier < nBarrier; iBarrier++ ) {
                        flo32 Radians    = ObservationDeck->Barrier_Radians + ( TAU * 0.25f * ( flo32 )iBarrier ) - OBSERVATION_DECK_BARRIER_RADIANS * 0.5f;
                        flo32 MinRadians_Inner = Radians;
                        flo32 MaxRadians_Inner = MinRadians_Inner + OBSERVATION_DECK_BARRIER_RADIANS;
                        
                        flo32 RadiansPerSegment = ( OBSERVATION_DECK_BARRIER_RADIANS / ( flo32 )OBSERVATION_DECK_BARRIER_SEGMENT_COUNT );
                        flo32 MinRadians_Outer = MinRadians_Inner + ( RadiansPerSegment * 0.5f );
                        flo32 MaxRadians_Outer = MaxRadians_Inner - ( RadiansPerSegment * 0.5f );
                        
                        DoesRayIntersectCircleSegmentExt( &BestIntersect, Ray, ObservationDeck->Position, Radius_Outer, MinRadians_Outer, MaxRadians_Outer );
                        DoesRayIntersectCircleSegmentInt( &BestIntersect, Ray, ObservationDeck->Position, Radius_Inner, MinRadians_Inner, MaxRadians_Inner );
                        
                        vec2 A0 = ObservationDeck->Position + ToDirection2D( MinRadians_Inner ) * Radius_Inner;
                        vec2 B0 = ObservationDeck->Position + ToDirection2D( MinRadians_Outer ) * Radius_Outer;
                        vec2 C0 = ObservationDeck->Position + ToDirection2D( MaxRadians_Outer ) * Radius_Outer;
                        vec2 D0 = ObservationDeck->Position + ToDirection2D( MaxRadians_Inner ) * Radius_Inner;
                        
                        DoesRayIntersectEdge( &BestIntersect, Ray, B0, A0 );
                        DoesRayIntersectEdge( &BestIntersect, Ray, D0, C0 );
                    }
                    
                    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                        Bullet->Health = MaxValue( Bullet->Health - 1, 0 );
                        vec2 Pos = BestIntersect.P;
                        
                        Bullet->Position        = Pos;
                        Bullet->DrawCollision_N = BestIntersect.N;
                        Bullet->DrawCollision_P = BestIntersect.P;
                        Bullet->DrawCollision   = true;
                    }
                }
                
                { // Observation Deck Missile Launcher
                    for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                        MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                        if( MissileLauncher->Health > 0 ) {
                            flo32 Radius = MissileLauncher->Radius + BULLET_WIDTH + 0.1f;
                            
                            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                            DoesRayIntersectCircleExt( &BestIntersect, Ray, Circ( MissileLauncher->Position, Radius ) );
                            if( ( IsInCapsule( MissileLauncher->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                                MissileLauncher->Health       = MaxValue( MissileLauncher->Health - 1, 0 );
                                MissileLauncher->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                                
                                
                                Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                                Bullet->DrawCollision   = true;
                                
                                if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                                    Bullet->DrawCollision_N = GetNormal( BestIntersect.P - MissileLauncher->Position );
                                    Bullet->DrawCollision_P = MissileLauncher->Position + Bullet->DrawCollision_N * MissileLauncher->Radius;
                                } else {
                                    Bullet->DrawCollision_P = A;
                                    Bullet->DrawCollision_N = Bullet->Direction;
                                }
                                
                                AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, MISSILE_LAUNCHER_BASE_COLOR );
                            }
                        }
                    }
                }
                
                { // Board Members
                    for( int32 iBoardMember = 0; iBoardMember < BoardRoomS->nBoardMember; iBoardMember++ ) {
                        BOARD_MEMBER * BoardMember = BoardRoomS->BoardMember + iBoardMember;
                        flo32 Radius = BOARD_MEMBER_RADIUS + BULLET_WIDTH + 0.1f;
                        
                        RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                        DoesRayIntersectCircleExt( &BestIntersect, Ray, Circ( BoardMember->Position, Radius ) );
                        
                        if( ( IsInCapsule( BoardMember->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                            
                            Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                            Bullet->DrawCollision   = true;
                            
                            if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                                Bullet->DrawCollision_N = GetNormal( BestIntersect.P - BoardMember->Position );
                                Bullet->DrawCollision_P = BoardMember->Position + Bullet->DrawCollision_N * BOARD_MEMBER_RADIUS;
                            } else {
                                Bullet->DrawCollision_P = A;
                                Bullet->DrawCollision_N = Bullet->Direction;
                            }
                            
                            BoardMember->Velocity += GetNormal( Ray.Vector ) * 2.0f;
                            BoardMember->Direction = GetNormal( BoardMember->Velocity );
                        }
                    }
                }
            }
        }
        
        { // GUNNER
            GUNNER_STATE * GunnerS = &AppState->GunnerS;
            
            for( int32 iGunner = 0; ( Bullet->Health > 0 ) && ( iGunner < GunnerS->nGunner ); iGunner++ ) {
                GUNNER * Gunner = GunnerS->Gunner + iGunner;
                
                if( Gunner->Health > 0 ) {
                    flo32 Radius = Gunner->Radius + BULLET_WIDTH + 0.1f;
                    
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesRayIntersectCircleExt( &BestIntersect, Ray2( A, B - A ), Circ( Gunner->Position, Radius ) );
                    
                    if( ( IsInCapsule( Gunner->Position, Capsule2( A, B, Radius ) ) ) || ( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) ) {
                        Gunner->Health       = MaxValue( Gunner->Health - 1, 0 );
                        Gunner->Damage_Timer = ENTITY_SHOW_DAMAGE_TARGET_TIME;
                        
                        
                        Bullet->Health          = MaxValue( Bullet->Health - 1, 0 );
                        Bullet->DrawCollision   = true;
                        
                        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                            Bullet->DrawCollision_N = GetNormal( BestIntersect.P - Gunner->Position );
                            Bullet->DrawCollision_P = Gunner->Position + Bullet->DrawCollision_N * Gunner->Radius;
                        } else {
                            Bullet->DrawCollision_P = A;
                            Bullet->DrawCollision_N = Bullet->Direction;
                        }
                        
                        AddBulletHitParticles( AppState, Bullet->DrawCollision_P, GetNormal( B - A ), Bullet->DrawCollision_N, GUNNER_BASE_COLOR );
                    }
                }
            }
        }
    }
}

internal void
DrawLabelColonS32( RENDER_PASS * Pass, FONT * Font, char * Label, int32 Value, vec2 AtPos, flo32 ColonX, flo32 ValueX, vec4 Color ) {
    char Str[ 128 ] = {};
    sprintf( Str, "%d", Value );
    
    DrawString( Pass, Font, Label, AtPos,                   TextAlign_TopLeft,   Color );
    DrawString( Pass, Font, ":",   Vec2( ColonX, AtPos.y ), TextAlign_TopCenter, Color );
    DrawString( Pass, Font, Str,   Vec2( ValueX, AtPos.y ), TextAlign_TopRight,  Color );
}

internal void
DrawEntityIcon( RENDER_PASS * Pass, ENTITY_TYPE Type, rect Bound, boo32 DrawInEntityKillList = false ) {
    vec2  Center = GetCenter( Bound );
    
    flo32 Icon_MaxScaleRatio = 0.6f;
    vec2  Icon_HalfDim       = GetDim( Bound ) * 0.5f;
    
    switch( Type ) {
        case EntityType_Turret: {
            TURRET Turret = TurretC( -1, Center, 2.0f, 0 );
            Turret.Fire_Radians = PI;
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / Turret.Radius;
            DrawTurret( Pass, 0, 0, Turret, 1.1f, Scale );
        } break;
        
        case EntityType_Chaser: {
            CHASER Chaser = ChaserC( -1, Center );
            Chaser.DirToPlayer = Vec2( -1.0f, 0.0f );
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.8f ) / Chaser.Radius;
            DrawChaser( Pass, Chaser, 1.0f, Scale );
        } break;
        
        case EntityType_Swarm: {
            SWARM Swarm = SwarmC( -1, Center );
            Swarm.Velocity = Vec2( 2.0f, 0.0f );
            Swarm.CloakT   = 0.8f;
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.4f ) / Swarm.Radius;
            DrawSwarm( Pass, Swarm, 1.0f, Scale );
        } break;
        
        case EntityType_Drifter: {
            DRIFTER Drifter = DrifterC( -1, Center );
            Drifter.Velocity   = Vec2( 1.0f, 0.0f );
            Drifter.Anim_Timer = 0.0f;
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.6f ) / Drifter.Radius;
            DrawDrifter( Pass, Drifter, 1.0f, Scale );
        } break;
        
        case EntityType_Looper: {
            LOOPER Looper = LooperC( -1, Center );
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.7f ) / LOOPER_RADIUS;
            DrawLooper( Pass, Looper, 1.0f, Scale );
        } break;
        
#if 0        
        case EntityType_Starburst: {
            STARBURST Starburst = StarburstC( -1, Center );
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.8f ) / STARBURST_RADIUS;
            DrawStarburst( Pass, Starburst, 1.0f, Scale );
        } break;
#endif
        
        case EntityType_EnemyBullet: {
            ENEMY_BULLET EnemyBullet = EnemyBulletC( Center, Vec2( 1.0f, 0.0f ) );
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.7f ) / ENEMY_BULLET_RADIUS;
            DrawEnemyBullet( Pass, EnemyBullet, 1.0f, Scale );
        } break;
        
        case EntityType_PersonalPod: {
            PERSONAL_POD PersonalPod = {};
            PersonalPod.Position = Center;
            PersonalPod.Velocity = Vec2( 1.0f, 0.0f );
            PersonalPod.ColorT   = 1.0f;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.6f ) / PERSONAL_POD_RADIUS_Y;
            DrawPersonalPod( Pass, PersonalPod, 1.0f, Scale );
        } break;
        
        case EntityType_Transport: {
            TRANSPORT Transport = {};
            Transport.Position = Center;
            Transport.Velocity = Vec2( 1.0f, 0.0f );
            Transport.ColorT   = 1.0f;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.8f ) / TRANSPORT_RADIUS;
            DrawTransport( Pass, 0, 0, Transport, 1.0f, Scale );
        } break;
        
        case EntityType_Asteroid: {
            ASTEROID Asteroid = {};
            Asteroid.TextureID = TextureID_Asteroid00;
            Asteroid.Radius    = 1.5f;
            Asteroid.Position  = Center;
            Asteroid.tColor    = 1.0f;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / Asteroid.Radius;
            DrawAsteroid( Pass, 0, 0, Asteroid, 1.0f, Scale );
        } break;
        
        case EntityType_Miner: {
            MINER Miner = MinerC_Autonomous( -1, Center );
            Miner.Health             = 0;
            Miner.Fire_TargetTime    = FLT_MAX;
            Miner.AtRadians          = PI;
            Miner.Laser[ 0 ].Radians = 0.0f;
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / Miner.Radius;
            DrawMiner( Pass, 0, 0, Miner, 1.0f, Scale );
        } break;
        
        case EntityType_Bomb: {
            BOMB  Bomb  = BombC( -1, Center, Vec2( 1.0f, 0.0f ) );
            flo32 Scale = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.65f ) / BOMB_RADIUS;
            DrawBomb( Pass, Bomb, 1.0f, Scale );
        } break;
        
        case EntityType_BombLauncher: {
            BOMB_LAUNCHER BombLauncher = InitBombLauncher( Center, Vec2( 1.0f, 0.0f ) );
            BombLauncher.Health     = 0;
            BombLauncher.Radians    = PI;
            BombLauncher.Anim_Timer = BOMB_LAUNCHER_SPAWN_ANIM_TARGET_TIME;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / BOMB_LAUNCHER_RADIUS;
            DrawBombLauncher( Pass, 0, 0, BombLauncher, false, 1.0f, Scale );
        } break;
        
        case EntityType_Blimp: {
            BLIMP Blimp = {};
            Blimp.Position = Center;
            Blimp.Radius   = BLIMP_RADIUS;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / BLIMP_RADIUS;
            DrawBlimp( Pass, 0, 0, Blimp, 1.0f, Scale );
        } break;
        
        case EntityType_Yacht: {
            YACHT Yacht = {};
            Yacht.Position = Center;
            Yacht.ColorT   = 1.0f;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.9f ) / YACHT_RADIUS;
            DrawYacht( Pass, 0, 0, Yacht, 1.0f, Scale );
        } break;
        
        case EntityType_MegaTurret: {
            TURRET MegaTurret = {};
            MegaTurret.Position = Center;
            MegaTurret.Radius   = MEGA_TURRET_RADIUS;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / MEGA_TURRET_RADIUS;
            DrawMegaTurret( Pass, 0, 0, MegaTurret, 1.0f, Scale );
        } break;
        
        case EntityType_Miner_Stationary: {
            MINER Miner = MinerC_Stationary( -1, Center, 0.0f );
            Miner.Health             = 0;
            Miner.Fire_TargetTime    = FLT_MAX;
            Miner.AtRadians          = PI;
            Miner.Laser[ 0 ].Radians = 0.0f;
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / Miner.Radius;
            DrawMiner( Pass, 0, 0, Miner, 1.0f, Scale );
        } break;
        
        case EntityType_Asteroid_BombOnly: {
            ASTEROID Asteroid = {};
            Asteroid.Radius   = 1.5f;
            Asteroid.Position = Center;
            Asteroid.Flags    = ASTEROID_FLAG__BOMB_ONLY;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / Asteroid.Radius;
            DrawAsteroid( Pass, 0, 0, Asteroid, 1.0f, Scale );
        } break;
        
        case EntityType_Asteroid_LaserOnly: {
            ASTEROID Asteroid = {};
            Asteroid.Radius   = 1.5f;
            Asteroid.Position = Center;
            Asteroid.Flags    = ASTEROID_FLAG__LASER_ONLY;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / Asteroid.Radius;
            DrawAsteroid( Pass, 0, 0, Asteroid, 1.0f, Scale );
        } break;
        
        case EntityType_Asteroid_SwarmOnly: {
            ASTEROID Asteroid = {};
            Asteroid.TextureID = TextureID_Asteroid00;
            Asteroid.Radius   = 1.5f;
            Asteroid.Position = Center;
            Asteroid.Flags    = ASTEROID_FLAG__SWARM_ONLY;
            Asteroid.tColor   = 1.0f;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / Asteroid.Radius;
            DrawAsteroid( Pass, 0, 0, Asteroid, 1.0f, Scale );
        } break;
        
        case EntityType_Missile: {
            MISSILE Missile = MissileC( -1, Center, Vec2( -1.0f, 0.0f ) );
            Missile.Exhaust_Timer = 0.0f;
            flo32   Scale   = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.3f ) / MISSILE_RADIUS;
            DrawMissile( Pass, Missile, 1.0f, Scale );
        } break;
        
        case EntityType_MegaMiner: {
            MEGA_MINER MegaMiner = MegaMinerC( -1, Center, PI );
            MegaMiner.Health = 0;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.9f ) / MEGA_MINER_RADIUS;
            DrawMegaMiner( Pass, 0, 0, MegaMiner, 1.0f, Scale );
        } break;
        
        case EntityType_Worm: {
            WORM Worm = {};
            Worm.Position = Center;
            Worm.Radius   = WORM_RADIUS_SMALL;
            Worm.Spawn_Length     = 4.0f;
            Worm.Spawn_MaxLength  = 4.0f;
            flo32   Scale   = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 1.0f ) / Worm.Radius;
            DrawWormHead( Pass, 0, 0, Worm, 0.99f, Scale );
        } break;
        
        case EntityType_Fireball: {
            FIREBALL Fireball = {};
            Fireball.Type     = FireballType_Points;
            Fireball.Position = Center;
            Fireball.Velocity = Vec2( -1.0f, 0.0f );
            Fireball.Radius   = FIREBALL_RADIUS_SMALL;
            flo32   Scale   = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.7f ) / Fireball.Radius;
            DrawFireball( Pass, 0, 0, Fireball, 1.0f, Scale );
        } break;
        
        case EntityType_Spit: {
            flo32 OffsetX = GetWidth( Bound ) * 0.175f;
            
            vec2 A = Center + Vec2( -OffsetX, 0.0f );
            vec2 B = Center;
            vec2 C = Center + Vec2(  OffsetX, 0.0f );
            
            SPIT Spit0 = SpitC( Center + Vec2( -OffsetX, 0.0f ), Vec2( 0.0f, 0.0f ), SpitType_Red    );
            SPIT Spit1 = SpitC( Center,                          Vec2( 0.0f, 0.0f ), SpitType_Yellow );
            SPIT Spit2 = SpitC( Center + Vec2(  OffsetX, 0.0f ), Vec2( 0.0f, 0.0f ), SpitType_Blue   );
            
            Spit0.iTexture = 0;
            Spit1.iTexture = 0;
            Spit2.iTexture = 0;
            Spit0.Radians  = 0.0f;
            Spit1.Radians  = 0.0f;
            Spit2.Radians  = 0.0f;
            
            flo32   Scale   = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.6f ) / Spit0.Radius;
            DrawSpit( Pass, Spit0, 1.0f, Scale );
            DrawSpit( Pass, Spit1, 1.0f, Scale );
            DrawSpit( Pass, Spit2, 1.0f, Scale );
        } break;
        
        case EntityType_Loader: {
            LOADER Loader = LoaderC( Center, PI );
            Loader.Health = 0;
            Loader.Damage_Cooldown_Timer = 2.0f;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 1.1f ) / LOADER_DIM.y;
            DrawLoader( Pass, 0, 0, Loader, 1.0f, Scale );
        } break;
        
        case EntityType_Barrel: {
            vec2 Dim = BARREL_DIM;
            
            BARREL Barrel = {};
            Barrel.Position = Center;
            Barrel.Radians  = -PI / 32.0f;
            
            flo32 Scale = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / ( Dim.x * 0.8f );
            DrawBarrel( Pass, 0, 0, Barrel, 1.0f, Scale );
        } break;
        
        case EntityType_Gunner: {
            GUNNER Gunner  = GunnerC( Center );
            Gunner.Radians = PI;
            Gunner.Health  = 0;
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.75f ) / Gunner.Radius;
            DrawGunner( Pass, 0, 0, Gunner, 1.0f, Scale );
        } break;
        
        case EntityType_MissileLauncher: {
            MISSILE_LAUNCHER MissileLauncher = {};
            MissileLauncher.Position = Center;
            MissileLauncher.Radius   = MISSILE_LAUNCHER_RADIUS;
            MissileLauncher.Radians  = PI;
            MissileLauncher.Launch_Timer = ( OBSERVATION_DECK_MISSILE_LAUNCHER_TARGET_TIME - MISSILE_LAUNCHER_LAUNCH_WARMUP_TARGET_TIME ) * 0.25f;
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.9f ) / MissileLauncher.Radius;
            DrawMissileLauncher( Pass, 0, 0, MissileLauncher, 1.0f, Scale );
        } break;
        
        case EntityType_Recon: {
            RECON Recon = ReconC( -1, Center, PI );
            flo32 Scale = ( Icon_HalfDim.y * Icon_MaxScaleRatio * 0.8f ) / ( RECON_SIDE_LENGTH * 0.5f );
            DrawRecon( Pass, Recon, 1.0f, Scale );
        } break;
        
        case EntityType_ObservationDeck: {
            if( DrawInEntityKillList ) {
                DrawModel( Pass, MeshID_ObservationDeck_Icon, TextureID_ObservationDeck_Icon, GetCenter( Bound ), GetDim( Bound ) * Vec2( 1.0f, 0.5f ) * 0.5f, COLOR_WHITE );
            } else {
                DrawRect( Pass, TextureID_ObservationDeck_Icon, Bound, COLOR_WHITE );
            }
        } break;
        
        case EntityType_Satellite: {
            SATELLITE Satellite = SatelliteC( -1, Center );
            Satellite.Health = 0;
            
            flo32  Scale  = ( Icon_HalfDim.y * Icon_MaxScaleRatio ) / SATELLITE_RADIUS;
            DrawSatellite( Pass, 0, 0, Satellite, 1.0f, Scale );
        } break;
        
        default: {
            //InvalidCodePath;
        } break;
    }
}