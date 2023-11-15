
internal void
PlayEntitySpawnAudio( APP_STATE * AppState ) {
    int32 nPlaying = 0;
    boo32 CanAdd   = true;
    
    AUDIO_SYSTEM * AudioSystem = AppState->AudioSystem;
    for( int32 iAudio = 0; ( CanAdd ) && ( iAudio < AudioSystem->nAudio ); iAudio++ ) {
        AUDIO_ENTRY * Audio = AudioSystem->Audio + iAudio;
        if( Audio->AudioID == AudioID_Entity_Spawn ) {
            nPlaying += 1;
            
            if( Audio->AtSample <= 0 ) {
                CanAdd = false;
            }
        }
    }
    
    int32 nMax = 3;
    
    if( CanAdd ) {
        if( nPlaying >= nMax ) {
            RemoveOldestAudio( AppState, AudioID_Entity_Spawn );
        }
        
        // PlayAudio( AppState, AudioID_Entity_Spawn, ENTITY_SPAWN_VOLUME );
    }
}

internal void
PlayEntityDeathAudio( APP_STATE * AppState, AUDIO_ID AudioID ) {
    int32 nPlaying = CountAudioPlaying( AppState, AudioID );
    
    switch( AudioID ) {
        case AudioID_Entity_Death_Large: {
            // PlayAudio( AppState, AudioID, ENTITY_DEATH_VOLUME );
        } break;
        
        case AudioID_Entity_Death_Medium: {
            // PlayAudio( AppState, AudioID, ENTITY_DEATH_VOLUME );
        } break;
        
        case AudioID_Entity_Death_Small: {
            if( nPlaying >= 2 ) {
                RemoveOldestAudio( AppState, AudioID );
            }
            flo32 Rate = RandomF32InRange( 0.95f, 1.05f );
            // PlayAudio( AppState, AudioID, ENTITY_DEATH_VOLUME * 0.25f, Rate );
        } break;
        
        case AudioID_Entity_Death_Tiny: {
            if( nPlaying >= 2 ) {
                RemoveOldestAudio( AppState, AudioID );
            }
            // PlayAudio( AppState, AudioID, ENTITY_DEATH_VOLUME * 0.25f );
        } break;
    }
}

internal void
AddEntitySpawn( APP_STATE * AppState, ENTITY Entity ) {
    ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
    
    if( EntitySpawnS->nEntitySpawn < ENTITY_SPAWN_MAX_COUNT ) {
        EntitySpawnS->EntitySpawn      [ EntitySpawnS->nEntitySpawn ] = Entity;
        EntitySpawnS->EntitySpawn_Timer[ EntitySpawnS->nEntitySpawn ] = 0.0f;
        EntitySpawnS->nEntitySpawn++;
        
        PlayEntitySpawnAudio( AppState );
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add ENTITY to ENTITY_SPAWN_STATE: %u", ENTITY_SPAWN_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal ENTITY
GenValIdentity( int32 WaveGuid, ENTITY_TYPE Type, vec2 Position, int32 nEntity = 1, flo32 Radius = 1.0f, ENTITY_SHIELD Shield = {} ) {
    ENTITY Entity = {};
    Entity.WaveGuid = WaveGuid;
    Entity.Type     = Type;
    Entity.Position = Position;
    Entity.nEntity  = nEntity;
    
    if( Entity.Type == EntityType_Turret ) {
        Entity.nEntity = 1;
        Entity.Radius  = Radius;
        Entity.Shield  = Shield;
    } else {
        switch( Type ) {
            case EntityType_Chaser:    { Entity.Radius = CHASER_RADIUS;    } break;
            case EntityType_Swarm:     { Entity.Radius = SWARM_RADIUS;     } break;
            case EntityType_Drifter:   { Entity.Radius = DRIFTER_RADIUS;   } break;
            case EntityType_Looper:    { Entity.Radius = LOOPER_RADIUS;    } break;
            case EntityType_Starburst: { Entity.Radius = STARBURST_RADIUS; } break;
            case EntityType_Miner:     {
                Entity.Radius  = MINER_RADIUS_AUTONOMOUS;
                Entity.nEntity = Clamp( nEntity, 1, 4 );
            } break;
            case EntityType_Miner_Stationary:     {
                Entity.Radius  = MINER_RADIUS_STATIONARY;
                Entity.nEntity = Clamp( nEntity, 1, 4 );
            } break;
            case EntityType_Bomber:    {
                Entity.Radius  = BOMBER_RADIUS;
                Entity.nEntity = Clamp( nEntity, 1, 4 );
            } break;
            case EntityType_Bumper: {
                Entity.Radius  = BUMPER_RADIUS;
            } break;
            case EntityType_Bank: {
                Entity.Radius  = BANK_RADIUS;
                Entity.nEntity = 1;
            } break;
            case EntityType_Bumper_Stationary: {
                Entity.Radius  = BUMPER_STATIONARY_RADIUS;
                Entity.nEntity = 1;
            } break;
            case EntityType_MegaTurret: {
                Entity.nEntity = nEntity;
                Entity.Radius  = Radius;
                Entity.Shield  = Shield;
            } break;
            case EntityType_Gunner: {
                Entity.nEntity = 1;
                Entity.Radius  = GUNNER_RADIUS;
            } break;
            default: { InvalidCodePath; } break;
        }
    }
    
    return Entity;
}

//----------
// ENTITY GROUP SPAWN
//----------

internal ENTITY_GROUP_SPAWN
EntityGroupSpawnC( int32 WaveGuid, ENTITY_TYPE Type, flo32 TargetTime, uint32 Counter, vec2 Position ) {
    ENTITY_GROUP_SPAWN Result = {};
    Result.WaveGuid   = WaveGuid;
    Result.Type       = Type;
    Result.TargetTime = TargetTime;
    Result.Counter    = Counter;
    Result.Position   = Position;
    
    return Result;
}

internal ENTITY_GROUP_SPAWN
ToEntityGroupSpawn( ENTITY Entity ) {
    ENTITY_GROUP_SPAWN Result = EntityGroupSpawnC( Entity.WaveGuid, Entity.Type, 0.25f, Entity.nEntity, Entity.Position );
    return Result;
}

internal void
AddEntityGroupSpawn( APP_STATE * AppState, ENTITY_GROUP_SPAWN EntityGroupSpawn ) {
    WAVE_STATE * WaveS = &AppState->WaveS;
    if( WaveS->nEntityGroupSpawn < ENTITY_GROUP_SPAWN_MAX_COUNT ) {
        WaveS->EntityGroupSpawn[ WaveS->nEntityGroupSpawn++ ] = EntityGroupSpawn;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add ENTITY_GROUP_SPAWN to WAVE_STATE: %u", ENTITY_GROUP_SPAWN_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

//----------
// ENTITY PARTICLES
//----------

internal void
AddYachtExitParticles( APP_STATE * AppState, vec2 bPos, int32 nParticle, flo32 MaxAlpha, flo32 Scale ) {
    for( int32 iParticle = 0; iParticle < nParticle; iParticle++ ) {
        PARTICLE Particle = {};
        
        flo32 Radians   = RandomF32() * TAU;
        vec2  Direction = ToDirection2D( Radians );
        flo32 InitDist  = RandomF32InRange( 0.2f, 0.35f ) * Scale;
        flo32 Speed     = RandomF32InRange( 4.0f, 6.0f  );
        
        Particle.Position = bPos + Direction * InitDist;
        Particle.Velocity = Direction * Speed * Scale;
        Particle.Accel    = {};
        Particle.Friction = 1.0f;
        
        flo32 Radius      = RandomF32InRange( 0.3f, 0.45f ) * Scale;
        Particle.Dim      = Vec2( Radius, Radius );
        
        Particle.Radians  = RandomF32() * TAU;
        Particle.dRadians = RandomF32InRange( -( TAU / 4.0f ), ( TAU / 4.0f ) );
        
        Particle.Timer      = 0.0f;
        Particle.TargetTime = RandomF32InRange( ( 40.0f / 60.0f ), ( 60.0f / 60.0f ) );
        
        flo32 tColor = RandomF32InRange( 0.35f, 0.9f );
        Particle.Color      = COLOR_GRAY( tColor );
        Particle.Color     *= RandomF32InRange( 0.05f, MaxAlpha );
        
        AddParticle( AppState, Particle );
    }
}

internal void
AddBombLauncherSparks( APP_STATE * AppState, vec2 Position ) {
    int32 nParticle = RandomS32InRange( 12, 18 );
    
    for( int32 iParticle = 0; iParticle < nParticle; iParticle++ ) {
        flo32 tA = RandomF32();
        flo32 t  = tA * tA * tA * tA;
        
        flo32 Dim = lerp( 0.1f, t, 0.2f );
        
        flo32 Speed      = lerp( 16.0f, t, 20.0f );
        flo32 TargetTime = lerp( 3.0f / 60.0f, t, 6.0f / 60.0f );
        
        flo32 Rad = RandomF32() * TAU;
        vec2  Dir = ToDirection2D( Rad );
        
        PARTICLE Particle = {};
        Particle.Position   = Position;
        Particle.Velocity   = Dir * Speed;
        Particle.Friction   = RandomF32InRange( 4.0f, 12.0f );
        Particle.Dim        = Vec2( Dim, Dim );
        Particle.Radians    = RandomF32() * TAU;
        Particle.Timer      = 0.0f;
        Particle.TargetTime = TargetTime;
        Particle.Color      = COLOR_WHITE;
        
        AddParticle( AppState, Particle );
    }
}

internal void
AddPersonalPodDeathParticles( APP_STATE * AppState, vec2 Velocity, vec2 HitPosition, vec2 HitDirection ) {
    int32 nParticle = RandomS32InRange( 8, 12 );
    
    flo32 Range  = PI * 0.5f;
    vec4  bColor = COLOR_GRAY( 0.5f );
    
    vec2  V          = GetNormal( HitDirection + GetNormal( Velocity ) * 0.25f );
    flo32 Radians    = ToRadians( V );
    flo32 MinRadians = Radians - ( Range * 0.5f );
    
    for( int32 iParticle = 0; iParticle < nParticle; iParticle++ ) {
        flo32 tA = RandomF32();
        flo32 tB = RandomF32();
        flo32 tC = RandomF32();
        flo32 tD = RandomF32_2Dice();
        
        flo32 DimX = lerp( 0.1f, tA, 0.25f );
        flo32 DimY = lerp( 0.1f, tB, 0.25f );
        vec2  Dim  = Vec2( DimX, DimY );
        
        flo32 Speed      = lerp( 80.0f, tC, 10.0f );
        flo32 TargetTime = lerp( 4.0f / 60.0f, tC, 8.0f / 60.0f );
        
        flo32 Rad = MinRadians + tD * Range;
        vec2  Dir = ToDirection2D( Rad );
        
        flo32 tSpeed = Clamp01( dot( V, Dir ) );
        Speed *= lerp( 0.5f, tSpeed, 1.0f );
        
        PARTICLE Particle = {};
        Particle.Position   = HitPosition;
        Particle.Velocity   = Dir * Speed;
        Particle.Dim        = Dim;
        Particle.Timer      = 0.0f;
        Particle.TargetTime = TargetTime;
        
        int32 ColorSelect = RandomS32InRange( 0, 1 );
        if( ColorSelect == 0 ) {
            Particle.Color = TowardWhite( bColor, RandomF32() * 0.5f );
        } else {
            Particle.Color = TowardBlack( bColor, RandomF32() * 0.75f );
        }
        
        Particle.Color *= lerp( 0.2f, RandomF32(), 1.0f ); 
        
        AddParticle( AppState, Particle );
    }
}

internal void
AddBulletHitParticles( APP_STATE * AppState, vec2 Position, vec2 InDirection, vec2 Normal, vec4 bColor ) {
    int32 nParticle = RandomS32InRange( 5, 10 );
    
    flo32 Range      = PI * 1.25f;
    int32 nSegment   = 4;
    int32 Numer[ 4 ] = {};
    flo32 Segment = Range / ( flo32 )nSegment;
    
    for( int32 iSegment = 0; iSegment < nSegment; iSegment++ ) {
        Numer[ iSegment ] = 1;
    }
    
    vec2  V     = GetNormal( InDirection - ( 2.0f * dot( InDirection, Normal ) * Normal ) );
    flo32 DotVN = dot( GetPerp( Normal ), V );
    
    {
        int32 Index = ( int32 )( ( DotVN * 0.5f + 0.5f ) * ( flo32 )nSegment );
        Index = MinValue( Index, nSegment - 1 );
        
        Numer[ Index ] += 4;
        if( Index > 0 ) {
            Numer[ Index - 1 ] += 2;
        }
        if( Index < ( nSegment - 1 ) ) {
            Numer[ Index + 1 ] += 2;
        }
    }
    
    flo32 Radians    = ToRadians( Normal );
    flo32 MinRadians = Radians - ( Range * 0.5f );
    
    for( int32 iParticle = 0; iParticle < nParticle; iParticle++ ) {
        int32 Select = RandomSelectByOdds( Numer, nSegment );
        
        flo32 tA = RandomF32();
        flo32 tB = RandomF32();
        flo32 t  = tA * tB;
        
        flo32 DimX = lerp( 0.05f, tA, 0.25f );
        flo32 DimY = lerp( 0.1f,  tB, 0.25f );
        vec2  Dim  = Vec2( DimX, DimY );
        
        flo32 Speed      = lerp( 40.0f, t, 16.0f );
        flo32 TargetTime = lerp( 8.0f / 60.0f, t, 12.0f / 60.0f );
        
        flo32 LoRadians = MinRadians + ( flo32 )Select * Segment;
        flo32 HiRadians = MinRadians + ( flo32 )( Select + 1 ) * Segment;
        
        flo32 Rad = RandomF32InRange( LoRadians, HiRadians );
        vec2  Dir = ToDirection2D( Rad );
        
        flo32 tSpeed = Clamp01( dot( V, Dir ) );
        Speed *= lerp( 0.35f, tSpeed, 1.0f );
        
        PARTICLE Particle = {};
        Particle.Position   = Position;
        Particle.Velocity   = Dir * Speed;
        Particle.Friction   = RandomF32InRange( 2.0f, 6.0f );
        Particle.Dim        = Dim;
        Particle.Radians    = Rad;
        Particle.Timer      = 0.0f;
        Particle.TargetTime = TargetTime;
        
        int32 ColorSelect = RandomS32InRange( 0, 1 );
        if( ColorSelect == 0 ) {
            Particle.Color = TowardWhite( bColor, RandomF32() * 0.5f );
        } else {
            Particle.Color = TowardBlack( bColor, RandomF32() * 0.75f );
        }
        
        Particle.Color *= lerp( 0.2f, RandomF32(), 1.0f ); 
        Particle.Color = bColor;
        
        AddParticle( AppState, Particle );
    }
}

internal void
AddBulletHitParticles( APP_STATE * AppState, vec2 Position, vec2 InDirection, vec2 Normal, ENTITY_TYPE Type ) {
    int32 nParticle = RandomS32InRange( 3, 5 );
    
    InDirection -= ( 2.0f * dot( InDirection, Normal ) ) * Normal;
    flo32 Radians   = ToRadians( InDirection );
    flo32 HalfRange = PI * 0.5f;
    HalfRange = 0;
    
    for( int32 iParticle = 0; iParticle < nParticle; iParticle++ ) {
        flo32 tA = RandomF32();
        flo32 t  = tA * tA * tA * tA;
        
        flo32 Dim = lerp( 0.15f, t, 0.175f );
        
        flo32 Speed      = lerp( 60.0f, t, 44.0f );
        flo32 TargetTime = lerp( 3.0f / 60.0f, t, 6.0f / 60.0f );
        
        flo32 tB = RandomF32();
        if( tB > 0.5f ) {
            tB =  powf( ( tB - 0.5f ) * 2.0f, 0.2f );
        } else {
            tB = -powf( tB * 2.0f, 0.2f );
        }
        
        flo32 Rad = Radians + HalfRange * tB;
        vec2  Dir = ToDirection2D( Rad );
        
        PARTICLE Particle = {};
        Particle.Position   = Position;
        Particle.Velocity   = Dir * Speed;
        Particle.Dim        = Vec2( Dim, Dim );
        Particle.Timer      = 0.0f;
        Particle.TargetTime = TargetTime;
        
        switch( Type ) {
            case EntityType_Asteroid: {
                vec4 Color = lerp( Vec4( 0.4f, 0.0f, 0.5f, 1.0f ), RandomF32(), Vec4( 0.15f, 0.0f, 0.25f, 1.0f ) );
                Particle.Color = Color;
            } break;
            
            default: {
                Particle.Color = COLOR_WHITE;
            } break;
        }
        
        AddParticle( AppState, Particle );
    }
}

internal void
AddMinerLaserHitParticles( APP_STATE * AppState, vec2 Position, vec2 Direction ) {
    PARTICLE Particle = {};
    
    flo32 Range     = PI * 0.8f;
    flo32 HalfRange = Range * 0.5f;
    
    flo32 bRadians  = ToRadians( Direction );
    flo32 Radians   = bRadians + RandomF32InRange( -HalfRange, HalfRange );
    vec2  Dir       = ToDirection2D( Radians );
    flo32 Speed     = RandomF32InRange( 20.0f, 36.0f );
    
    Particle.Position = Position;
    Particle.Velocity = Dir * Speed;
    Particle.Accel    = {};
    Particle.Friction = RandomF32InRange( 4.0f, 12.0f );
    Particle.Dim      = Vec2( 0.1f, 0.2f ) * RandomF32InRange( 0.5f, 1.0f );
    Particle.Radians  = Radians;
    
    Particle.Timer      = 0.0f;
    Particle.TargetTime = RandomF32InRange( ( 10.0f / 60.0f ), ( 14.0f / 60.0f ) );
    
    flo32 ColorT = RandomF32InRange( 0.75f, 1.0f );
    Particle.Color      = Vec4( 1.0f, ColorT, ColorT, 1.0f );
    
    AddParticle( AppState, Particle );
}

internal vec4
GetAsteroidBaseColor( ASTEROID Asteroid ) {
    vec4 bColor = TowardBlack( ASTEROID_DEFAULT_BASE_COLOR, 0.15f );
    if( Asteroid.Flags & ASTEROID_FLAG__SWARM_ONLY ) {
        bColor = TowardBlack( ASTEROID_SWARM_ONLY_BASE_COLOR, 0.55f );
    }
    
    Assert( !( Asteroid.Flags & ASTEROID_FLAG__BOMB_ONLY  ) );
    Assert( !( Asteroid.Flags & ASTEROID_FLAG__LASER_ONLY ) );
    
    return bColor;
}

internal void
AddAsteroidDeathParticles( APP_STATE * AppState, ASTEROID Asteroid, boo32 VaryBrightness = true ) {
    int32 nParticles = 12 * ( int32 )Asteroid.Radius;
    
    vec4 bColor = GetAsteroidBaseColor( Asteroid );
    
    for( int32 iParticle = 0; iParticle < nParticles; iParticle++ ) {
        PARTICLE Particle = {};
        
        flo32 Radians   = RandomF32() * TAU;
        vec2  Direction = ToDirection2D( Radians );
        flo32 InitDist  = RandomF32InRange( 0.05f, 0.9f ) * Asteroid.Radius;
        flo32 Speed     = RandomF32InRange( 20.0f, 36.0f );
        
        Particle.Position = Asteroid.Position + Direction * InitDist;
        Particle.Velocity = Direction * Speed;
        Particle.Accel    = {};
        Particle.Friction = RandomF32InRange( 4.0f, 12.0f );
        
        Particle.Dim      = Vec2( 0.1f, 0.3f );
        Particle.Radians  = Radians;
        
        Particle.Timer      = 0.0f;
        Particle.TargetTime = RandomF32InRange( ( 10.0f / 60.0f ), ( 14.0f / 60.0f ) );
        Particle.Color      = bColor;
        
        if( VaryBrightness ) {
            int32 ColorSelect = RandomS32InRange( 0, 1 );
            if( ColorSelect == 0 ) {
                Particle.Color = TowardWhite( bColor, RandomF32() * 0.5f );
            } else {
                Particle.Color = TowardBlack( bColor, RandomF32() * 0.75f );
            }
            
            Particle.Color *= lerp( 0.2f, RandomF32(), 1.0f ); 
        }
        
        AddParticle( AppState, Particle );
    }
}

internal void
AddMissileDeathParticles( APP_STATE * AppState, vec2 Position ) {
    PARTICLE Particle = {};
    Particle.TextureID  = TextureID_Dome;
    Particle.Position   = Position;
    Particle.Dim        = Vec2( MISSILE_RADIUS, MISSILE_RADIUS ) * 1.5f;
    Particle.TargetTime = ( 4.0f / 60.0f );
    Particle.dDim       = ( Particle.Dim * 1.75f ) / ( Particle.TargetTime );
    Particle.Color      = COLOR_WHITE;
    
    AddParticle( AppState, Particle );
}

internal void
AddBigEntityDeathParticles( APP_STATE * AppState, vec2 Position, flo32 InitRadius, vec4 bColor, boo32 VaryBrightness = true ) {
    uint32 nParticles = 48;
    
    for( uint32 iParticle = 0; iParticle < nParticles; iParticle++ ) {
        PARTICLE Particle = {};
        
        flo32 Radians   = RandomF32() * TAU;
        vec2  Direction = ToDirection2D( Radians );
        flo32 InitDist  = RandomF32InRange( 0.05f, InitRadius * 0.9f );
        flo32 Speed     = RandomF32InRange( 20.0f, 36.0f );
        
        Particle.Position = Position + Direction * InitDist;
        Particle.Velocity = Direction * Speed;
        Particle.Accel    = {};
        Particle.Friction = RandomF32InRange( 4.0f, 12.0f );
        
        Particle.Dim      = Vec2( 0.1f, 0.3f );
        Particle.Radians  = Radians;
        
        Particle.Timer      = 0.0f;
        Particle.TargetTime = RandomF32InRange( ( 10.0f / 60.0f ), ( 14.0f / 60.0f ) );
        Particle.Color      = bColor;
        
        if( VaryBrightness ) {
            int32 ColorSelect = RandomS32InRange( 0, 1 );
            if( ColorSelect == 0 ) {
                Particle.Color = TowardWhite( bColor, RandomF32() * 0.5f );
            } else {
                Particle.Color = TowardBlack( bColor, RandomF32() * 0.75f );
            }
            
            Particle.Color *= lerp( 0.2f, RandomF32(), 1.0f ); 
        }
        
        AddParticle( AppState, Particle );
    }
}

internal void
AddEntityDeathParticles( APP_STATE * AppState, vec2 Position, vec4 bColor, boo32 VaryBrightness = true ) {
    uint32 nParticles = 24;
    
    for( uint32 iParticle = 0; iParticle < nParticles; iParticle++ ) {
        PARTICLE Particle = {};
        
        flo32 Radians   = RandomF32() * TAU;
        vec2  Direction = ToDirection2D( Radians );
        flo32 InitDist  = RandomF32InRange( 0.05f, 0.1f );
        flo32 Speed     = RandomF32InRange( 20.0f, 36.0f );
        
        Particle.Position = Position + Direction * InitDist;
        Particle.Velocity = Direction * Speed;
        Particle.Accel    = {};
        Particle.Friction = RandomF32InRange( 4.0f, 12.0f );
        
        Particle.Dim      = Vec2( 0.1f, 0.2f );
        Particle.Radians  = Radians;
        
        Particle.Timer      = 0.0f;
        Particle.TargetTime = RandomF32InRange( ( 10.0f / 60.0f ), ( 14.0f / 60.0f ) );
        Particle.Color      = bColor;
        
        if( VaryBrightness ) {
            int32 ColorSelect = RandomS32InRange( 0, 1 );
            if( ColorSelect == 0 ) {
                Particle.Color = TowardWhite( bColor, RandomF32() * 0.5f );
            } else {
                Particle.Color = TowardBlack( bColor, RandomF32() * 0.75f );
            }
            
            Particle.Color *= lerp( 0.2f, RandomF32(), 1.0f ); 
        }
        
        AddParticle( AppState, Particle );
    }
}

internal uint32
CountTotalEntitiesInEntity( ENTITY_TYPE Type, uint32 nEntity = 1 ) {
    uint32 nTotalEntitiesInEntity = 0;
    
    switch( Type ) {
        case EntityType_Starburst: {
            nTotalEntitiesInEntity += ( nEntity * 4 );
        } break;
        
        case EntityType_Miner:
        case EntityType_Bomber: {
            nTotalEntitiesInEntity += 1;
        } break;
        
        default: {
            nTotalEntitiesInEntity += nEntity;
        } break;
    }
    
    return nTotalEntitiesInEntity;
}

internal WAVE *
GetActiveWaveByGuid( APP_STATE * AppState, int32 WaveGuid ) {
    WAVE_STATE * WaveS = &AppState->WaveS;
    
    WAVE * Result = 0;
    
    if( WaveGuid > -1 ) {
        for( int32 iActiveWave = 0; ( !Result ) && ( iActiveWave < WaveS->nActiveWave ); iActiveWave++ ) {
            WAVE * ActiveWave = WaveS->ActiveWave + iActiveWave;
            if( ActiveWave->Guid == WaveGuid ) {
                Result = ActiveWave;
            }
        }
    }
    
    return Result;
}

internal void
AddKillToWave( APP_STATE * AppState, int32 WaveGuid ) {
    WAVE * ActiveWave = GetActiveWaveByGuid( AppState, WaveGuid );
    if( ActiveWave ) {
        ActiveWave->nKills += 1;
    }
}

internal void
AddKillsToWave( APP_STATE * AppState, ENTITY Entity ) {
    WAVE * ActiveWave = GetActiveWaveByGuid( AppState, Entity.WaveGuid );
    if( ActiveWave ) {
        ActiveWave->nKills += CountTotalEntitiesInEntity( Entity.Type, Entity.nEntity );
    }
}

internal void
AddKillsToWave( APP_STATE * AppState, ENTITY_GROUP_SPAWN EntityGroupSpawn ) {
    WAVE * ActiveWave = GetActiveWaveByGuid( AppState, EntityGroupSpawn.WaveGuid );
    if( ActiveWave ) {
        ActiveWave->nKills += CountTotalEntitiesInEntity( EntityGroupSpawn.Type, EntityGroupSpawn.Counter );
    }
}

internal void
KillAllEnemies( APP_STATE * AppState, boo32 EndLevel = false ) {
    // Kill all enemies on screen
    { // TURRET
        TURRET_STATE * TurretS = &AppState->TurretS;
        if( EndLevel ) {
            TurretS->Spawn_IsActive           = false;
            TurretS->SpawnMegaTurret_IsActive = false;
        }
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Turret, TurretS->nTurret );
        
        if( EndLevel ) {
            AddEntityToMissedCountForLevel( AppState, EntityType_MegaTurret, TurretS->BonusObjective_nMegaTurretPossibleKills - TurretS->BonusObjective_nMegaTurretKills );
            
            for( int32 iMegaTurret = 0; iMegaTurret < TurretS->nMegaTurret; iMegaTurret++ ) {
                TURRET MegaTurret = TurretS->MegaTurret[ iMegaTurret ];
                AddEntityDeathParticles( AppState, MegaTurret.Position, MEGA_TURRET_BASE_COLOR );
                AddEntityDeathParticles( AppState, MegaTurret.Position, MEGA_TURRET_BASE_COLOR );
                AddEntityDeathParticles( AppState, MegaTurret.Position, COLOR_WHITE );
                AddKillToWave( AppState, MegaTurret.WaveGuid );
            }
            TurretS->nMegaTurret = 0;
        }
        
        for( int32 iTurret = 0; iTurret < TurretS->nTurret; iTurret++ ) {
            TURRET Turret = TurretS->Turret[ iTurret ];
            AddBigEntityDeathParticles( AppState, Turret.Position, Turret.Radius, TURRET_BASE_COLOR );
            AddKillToWave( AppState, Turret.WaveGuid );
        }
        TurretS->nTurret = 0;
    }
    
    { // CHASER
        CHASER_STATE * ChaserS = &AppState->ChaserS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Chaser, ChaserS->nChaser );
        
        for( int32 iChaser = 0; iChaser < ChaserS->nChaser; iChaser++ ) {
            CHASER Chaser = ChaserS->Chaser[ iChaser ];
            
            AddEntityDeathParticles( AppState, Chaser.Position, CHASER_BASE_COLOR );
            AddEntityDeathParticles( AppState, Chaser.Position, CHASER_BASE_COLOR );
            
            AddKillToWave( AppState, Chaser.WaveGuid );
        }
        ChaserS->nChaser = 0;
    }
    
    { // SWARM
        SWARM_STATE * SwarmS = &AppState->SwarmS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Swarm, SwarmS->nSwarm );
        
        for( int32 iSwarm = 0; iSwarm < SwarmS->nSwarm; iSwarm++ ) {
            SWARM Swarm = SwarmS->Swarm[ iSwarm ];
            
            AddEntityDeathParticles( AppState, Swarm.Position, SWARM_BASE_COLOR );
            
            AddKillToWave( AppState, Swarm.WaveGuid );
        }
        SwarmS->nSwarm = 0;
        
        if( EndLevel ) {
            SWARM_SPAWN_SETTINGS * Spawn = &SwarmS->Spawn;
            Spawn->IsActive = false;
        }
    }
    
    { // DRIFTER
        DRIFTER_STATE * DrifterS = &AppState->DrifterS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Drifter, DrifterS->nDrifter );
        
        for( int32 iDrifter = 0; iDrifter < DrifterS->nDrifter; iDrifter++ ) {
            DRIFTER Drifter = DrifterS->Drifter[ iDrifter ];
            
            AddEntityDeathParticles( AppState, Drifter.Position, DRIFTER_BASE_COLOR );
            AddEntityDeathParticles( AppState, Drifter.Position, DRIFTER_BASE_COLOR );
            
            AddKillToWave( AppState, Drifter.WaveGuid );
        }
        DrifterS->nDrifter = 0;
    }
    
    { // LOOPER
        LOOPER_STATE * LooperS = &AppState->LooperS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Looper, LooperS->nLooper );
        
        for( int32 iLooper = 0; iLooper < LooperS->nLooper; iLooper++ ) {
            LOOPER Looper = LooperS->Looper[ iLooper ];
            
            flo32 bRadians = ( 1.0f - Clamp01( Looper.Loop_Timer / LOOPER_LOOP_CYCLE_TARGET_TIME ) ) * TAU - PI;
            vec2  bPos     = Looper.Position + ToDirection2D( bRadians ) * Looper.Loop_Radius;
            
            AddEntityDeathParticles( AppState, bPos, LOOPER_BASE_COLOR );
            AddEntityDeathParticles( AppState, bPos, LOOPER_BASE_COLOR );
            
            AddKillToWave( AppState, Looper.WaveGuid );
        }
        LooperS->nLooper = 0;
    }
    
#if 0    
    { // STARBURST
        STARBURST_STATE * StarburstS = &AppState->StarburstS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Starburst, StarburstS->nStarburst );
        AddEntityToMissedCountForLevel( AppState, EntityType_Star,      StarburstS->nStar );
        
        for( uint32 iStarburst = 0; iStarburst < StarburstS->nStarburst; iStarburst++ ) {
            STARBURST Starburst = StarburstS->Starburst[ iStarburst ];
            AddEntityDeathParticles( AppState, Starburst.Position, ToColor( 255, 110, 145 ) );
            AddKillToWave( AppState, Starburst.WaveGuid );
        }
        StarburstS->nStarburst = 0;
        
        for( uint32 iStar = 0; iStar < StarburstS->nStar; iStar++ ) {
            STAR Star = StarburstS->Star[ iStar ];
            AddEntityDeathParticles( AppState, Star.Position, ToColor( 255, 110, 145 ) );
            AddKillToWave( AppState, Star.WaveGuid );
        }
        StarburstS->nStar = 0;
    }
#endif
    
    { // ENEMY_BULLET
        ENEMY_BULLET_STATE * EnemyBulletS = &AppState->EnemyBulletS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_EnemyBullet, EnemyBulletS->nEnemyBullet );
        
        for( int32 iEnemyBullet = 0; iEnemyBullet < EnemyBulletS->nEnemyBullet; iEnemyBullet++ ) {
            ENEMY_BULLET EnemyBullet = EnemyBulletS->EnemyBullet[ iEnemyBullet ];
            AddEntityDeathParticles( AppState, EnemyBullet.Position, ENEMY_BULLET_BASE_COLOR );
        }
        EnemyBulletS->nEnemyBullet = 0;
    }
    
    { // MISSILE
        MISSILE_STATE * MissileS = &AppState->MissileS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Missile, MissileS->nMissile );
        
        for( int32 iMissile = 0; iMissile < MissileS->nMissile; iMissile++ ) {
            MISSILE Missile = MissileS->Missile[ iMissile ];
            AddMissileDeathParticles( AppState, Missile.Position );
        }
        MissileS->nMissile = 0;
    }
    
    { // FIREBALL
        FIREBALL_STATE * FireballS = &AppState->FireballS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Fireball, FireballS->nFireball );
        
        for( int32 iFireball = 0; iFireball < FireballS->nFireball; iFireball++ ) {
            FIREBALL Fireball = FireballS->Fireball[ iFireball ];
            
            vec4 bColor = FIREBALL_POINTS_BASE_COLOR;
            if( Fireball.Type == FireballType_Multiplier ) {
                bColor = FIREBALL_MULTIPLIER_BASE_COLOR;
            }
            
            if( Fireball.Radius == FIREBALL_RADIUS_LARGE ) {
                AddBigEntityDeathParticles( AppState, Fireball.Position, Fireball.Radius, bColor );
                AddBigEntityDeathParticles( AppState, Fireball.Position, Fireball.Radius, bColor );
            } else {
                AddEntityDeathParticles( AppState, Fireball.Position, bColor );
                AddEntityDeathParticles( AppState, Fireball.Position, bColor );
            }
        }
        
        FireballS->nFireball = 0;
    }
    
    { // SPIT
        SPITTER_STATE * SpitterS = &AppState->SpitterS;
        
        SPIT_BASE_COLORS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Spit, SpitterS->nSpit );
        
        for( int32 iSpit = 0; iSpit < SpitterS->nSpit; iSpit++ ) {
            SPIT Spit = SpitterS->Spit[ iSpit ];
            
            AddEntityDeathParticles( AppState, Spit.Position, SpitBaseColors[ Spit.Type ] );
        }
        SpitterS->nSpit = 0;
    }
    
#if 0    
    { // BOLT
        BOLT_STATE * BoltS = &AppState->BoltS;
        
        AddEntityToMissedCountForLevel( AppState, EntityType_Bolt, BoltS->nBolt );
        
        for( int32 iBolt = 0; iBolt < BoltS->nBolt; iBolt++ ) {
            BOLT Bolt = BoltS->Bolt[ iBolt ];
            AddEntityDeathParticles( AppState, Bolt.Position, COLOR_YELLOW );
            AddEntityDeathParticles( AppState, Bolt.Position, COLOR_CYAN );
        }
        BoltS->nBolt = 0;
    }
#endif
    
    if( !EndLevel ) {
        { // ASTEROID
            ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
            
            int32 NewCount = 0;
            for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                ASTEROID Asteroid = AsteroidS->Asteroid[ iAsteroid ];
                
                if( Asteroid.Class == AsteroidClass_Tiny ) {
                    AddEntityToMissedCountForLevel( AppState, EntityType_Asteroid, 1 );
                    AddAsteroidDeathParticles( AppState, Asteroid );
                } else {
                    AsteroidS->Asteroid[ NewCount++ ] = Asteroid;
                }
            }
            AsteroidS->nAsteroid = NewCount;
        }
    }
    
    if( EndLevel ) {
        { // ASTEROID
            ASTEROID_STATE * AsteroidS = &AppState->AsteroidS;
            AsteroidS->Spawn_IsActive = false;
            
            for( int32 iAsteroid = 0; iAsteroid < AsteroidS->nAsteroid; iAsteroid++ ) {
                ASTEROID Asteroid = AsteroidS->Asteroid[ iAsteroid ];
                
                ENTITY_TYPE Type = EntityType_Asteroid;
                if( Asteroid.Flags & ASTEROID_FLAG__BOMB_ONLY ) {
                    Type = EntityType_Asteroid_BombOnly;
                }
                if( Asteroid.Flags & ASTEROID_FLAG__LASER_ONLY ) {
                    Type = EntityType_Asteroid_LaserOnly;
                }
                if( Asteroid.Flags & ASTEROID_FLAG__SWARM_ONLY ) {
                    Type = EntityType_Asteroid_SwarmOnly;
                }
                
                AddEntityToMissedCountForLevel( AppState, Type, 1 );
                
                AddAsteroidDeathParticles( AppState, Asteroid );
            }
            AsteroidS->nAsteroid = 0;
        }
        
        { // MINER
            MINER_STATE * MinerS = &AppState->MinerS;
            
            for( int32 iMiner = 0; iMiner < MinerS->nMiner; iMiner++ ) {
                MINER Miner = MinerS->Miner[ iMiner ];
                
                if( Miner.IsAutonomous ) {
                    AddEntityToMissedCountForLevel( AppState, EntityType_Miner, 1 );
                } else {
                    AddEntityToMissedCountForLevel( AppState, EntityType_Miner_Stationary, 1 );
                }
                
                vec4 bColor = MINER_STATIONARY_BASE_COLOR;
                if( Miner.IsAutonomous ) {
                    bColor = MINER_AUTONOMOUS_BASE_COLOR;
                } else {
                    AddBigEntityDeathParticles( AppState, Miner.Position, Miner.Radius, bColor );
                }
                
                AddEntityDeathParticles( AppState, Miner.Position, bColor );
                AddEntityDeathParticles( AppState, Miner.Position, bColor );
                AddEntityDeathParticles( AppState, Miner.Position, bColor );
                AddEntityDeathParticles( AppState, Miner.Position, COLOR_WHITE, false );
            }
            MinerS->nMiner = 0;
        }
        
        { // MEGA_MINER
            MINER_STATE * MinerS = &AppState->MinerS;
            
            AddEntityToMissedCountForLevel( AppState, EntityType_MegaMiner, MinerS->nMegaMiner );
            
            for( int32 iMegaMiner = 0; iMegaMiner < MinerS->nMegaMiner; iMegaMiner++ ) {
                MEGA_MINER MegaMiner = MinerS->MegaMiner[ iMegaMiner ];
                
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, MEGA_MINER_BASE_COLOR );
                AddBigEntityDeathParticles( AppState, MegaMiner.Position, MegaMiner.Radius, COLOR_WHITE, false );
                
                AddEntityDeathParticles( AppState, MegaMiner.Position, MEGA_MINER_BASE_COLOR );
                AddEntityDeathParticles( AppState, MegaMiner.Position, MEGA_MINER_BASE_COLOR );
            }
            MinerS->nMegaMiner = 0;
        }
        
#if 0        
        { // BOMBER
            BOMB_STATE * BombS = &AppState->BombS;
            
            AddEntityToMissedCountForLevel( AppState, EntityType_Bomber, BombS->nBomber );
            
            for( int32 iBomber = 0; iBomber < BombS->nBomber; iBomber++ ) {
                BOMBER Bomber = BombS->Bomber[ iBomber ];
                
                AddEntityDeathParticles( AppState, Bomber.Position, COLOR_GRAY( 0.2f ) );
                AddEntityDeathParticles( AppState, Bomber.Position, COLOR_RED );
                AddEntityDeathParticles( AppState, Bomber.Position, COLOR_WHITE );
            }
            BombS->nBomber = 0;
        }
#endif
        
        { // BOMB
            BOMB_STATE * BombS = &AppState->BombS;
            
            AddEntityToMissedCountForLevel( AppState, EntityType_Bomb, BombS->nBomb );
            
            for( int32 iBomb = 0; iBomb < BombS->nBomb; iBomb++ ) {
                BOMB Bomb = BombS->Bomb[ iBomb ];
                
                AddEntityDeathParticles( AppState, Bomb.Position, BOMB_BASE_COLOR );
            }
            BombS->nBomb = 0;
        }
        
#if 0        
        { // BUMPER
            BUMPER_STATE * BumperS = &AppState->BumperS;
            BumperS->Spawn_IsActive = false;
            
            for( int32 iBumper = 0; iBumper < BumperS->nBumper; iBumper++ ) {
                BUMPER Bumper = BumperS->Bumper[ iBumper ];
                
                vec4 ColorA = ToColor( 0, 100, 0 );
                if( Bumper.IsStationary ) {
                    ColorA = ToColor( 100, 100, 0 );
                    AddEntityToMissedCountForLevel( AppState, EntityType_Bumper_Stationary, 1 );
                } else {
                    AddEntityToMissedCountForLevel( AppState, EntityType_Bumper,            1 );
                }
                
                AddEntityDeathParticles( AppState, Bumper.Position, ColorA );
                AddEntityDeathParticles( AppState, Bumper.Position, ColorA );
                AddEntityDeathParticles( AppState, Bumper.Position, COLOR_WHITE );
            }
            BumperS->nBumper = 0;
        }
#endif
        
#if 0        
        { // EXPLOSIVE
            EXPLOSIVE_STATE * ExplosiveS = &AppState->ExplosiveS;
            ExplosiveS->Spawn_IsActive = false;
            
            for( uint32 iExplosive = 0; iExplosive < ExplosiveS->nExplosive; iExplosive++ ) {
                EXPLOSIVE Explosive = ExplosiveS->Explosive[ iExplosive ];
                
                AddEntityDeathParticles( AppState, Explosive.Position, COLOR_WHITE );
            }
            ExplosiveS->nExplosive = 0;
        }
#endif
        
        { // BOMB_LAUNCHER
            BOMB_LAUNCHER_STATE * BombLauncherS = &AppState->BombLauncherS;
            
            for( int32 iBombLauncher = 0; iBombLauncher < BombLauncherS->nBombLauncher; iBombLauncher++ ) {
                BOMB_LAUNCHER * BombLauncher = BombLauncherS->BombLauncher + iBombLauncher;
                
                if( BombLauncher->Stage != BombLauncherStage_IsDestroyed ) {
                    AddEntityToMissedCountForLevel( AppState, EntityType_BombLauncher, 1 );
                    
                    AddBigEntityDeathParticles( AppState, BombLauncher->Position, BombLauncher->Radius, BOMB_LAUNCHER_BASE_COLOR );
                    AddBigEntityDeathParticles( AppState, BombLauncher->Position, BombLauncher->Radius, BOMB_LAUNCHER_BASE_COLOR );
                    AddBigEntityDeathParticles( AppState, BombLauncher->Position, BombLauncher->Radius, COLOR_WHITE, false );
                }
                
                BombLauncher->Stage = BombLauncherStage_IsDestroyed;
            }
        }
        
        { // BAY DOORS
            BAY_DOOR_STATE * BayDoorS = &AppState->BayDoorS;
            
            BayDoorS->SpawnPersonalPod_IsActive = false;
            BayDoorS->SpawnTransport_IsActive   = false;
            
            for( int32 iBayDoorSpawn = 0; iBayDoorSpawn < BayDoorS->nBayDoorSpawn; iBayDoorSpawn++ ) {
                BAY_DOOR_SPAWN * BayDoorSpawn = BayDoorS->BayDoorSpawn + iBayDoorSpawn;
                BayDoorSpawn->Stage = BayDoorEventStage_EndLevel;
            }
            
            AddEntityToMissedCountForLevel( AppState, EntityType_PersonalPod, BayDoorS->nPersonalPod );
            AddEntityToMissedCountForLevel( AppState, EntityType_Transport,   BayDoorS->nTransport   );
            AddEntityToMissedCountForLevel( AppState, EntityType_Yacht,       BayDoorS->nYacht       );
            
            for( int32 iPersonalPod = 0; iPersonalPod < BayDoorS->nPersonalPod; iPersonalPod++ ) {
                PERSONAL_POD PersonalPod = BayDoorS->PersonalPod[ iPersonalPod ];
                
                AddEntityDeathParticles( AppState, PersonalPod.Position, PERSONAL_POD_BASE_COLOR );
            }
            BayDoorS->nPersonalPod = 0;
            
            for( int32 iTransport = 0; iTransport < BayDoorS->nTransport; iTransport++ ) {
                TRANSPORT Transport = BayDoorS->Transport[ iTransport ];
                
                AddEntityDeathParticles( AppState, Transport.Position, TRANSPORT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Transport.Position, TRANSPORT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Transport.Position, TRANSPORT_BASE_COLOR );
            }
            BayDoorS->nTransport = 0;
            
            for( int32 iYacht = 0; iYacht < BayDoorS->nYacht; iYacht++ ) {
                YACHT Yacht = BayDoorS->Yacht[ iYacht ];
                
                AddEntityDeathParticles( AppState, Yacht.Position, YACHT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Yacht.Position, YACHT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Yacht.Position, YACHT_BASE_COLOR );
                AddEntityDeathParticles( AppState, Yacht.Position, COLOR_WHITE );
                AddEntityDeathParticles( AppState, Yacht.Position, COLOR_RED );
            }
            BayDoorS->nYacht = 0;
        }
        
        { // BLIMP
            BLIMP_STATE * BlimpS = &AppState->BlimpS;
            
            AddEntityToMissedCountForLevel( AppState, EntityType_Blimp, BlimpS->nBlimp );
            
            for( int32 iBlimp = 0; iBlimp < BlimpS->nBlimp; iBlimp++ ) {
                BLIMP Blimp = BlimpS->Blimp[ iBlimp ];
                
                AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORA );
                AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORA );
                AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORB );
                AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, BLIMP_BASE_COLORB );
                AddBigEntityDeathParticles( AppState, Blimp.Position, Blimp.Radius, COLOR_WHITE, false );
                
                AddEntityDeathParticles( AppState, Blimp.Position, BLIMP_BASE_COLORA );
                AddEntityDeathParticles( AppState, Blimp.Position, BLIMP_BASE_COLORB );
            }
            BlimpS->nBlimp = 0;
        }
        
        { // WORM
            WORM_STATE * WormS = &AppState->WormS;
            
            WormS->Spawn_IsActive      = false;
            WormS->SpawnLarge_IsActive = false;
            
            for( int32 iWorm = 0; iWorm < WormS->nWorm; iWorm++ ) {
                WORM * Worm = WormS->Worm + iWorm;
                
                if( Worm->Health > 0 ) {
                    AddEntityToMissedCountForLevel( AppState, EntityType_Worm, 1 );
                    
                    Worm->Health                   = 0;
                    Worm->CanSpawnPoints           = false;
                    Worm->CanSpawnScoreMultipliers = false;
                }
            }
        }
        
        { // LOADER
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            AddEntityToMissedCountForLevel( AppState, EntityType_Loader, SpitterS->nLoader );
            
            for( int32 iLoader = 0; iLoader < SpitterS->nLoader; iLoader++ ) {
                LOADER Loader = SpitterS->Loader[ iLoader ];
                
                AddEntityDeathParticles( AppState, Loader.Position, LOADER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Loader.Position, LOADER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Loader.Position, LOADER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Loader.Position, COLOR_WHITE );
            }
            SpitterS->nLoader = 0;
        }
        
        { // SPITTER
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            SpitterS->SpitBonus_IsActive = false;
            SpitterS->Barrel_IsActive    = false;
            SpitterS->Spiral_nSpitters   = 0;
            
            for( int32 iSpitter = 0; iSpitter < SpitterS->nSpitter; iSpitter++ ) {
                SPITTER * Spitter = SpitterS->Spitter + iSpitter;
                Spitter->IsActive = false;
            }
        }
        
        { // BARREL
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            AddEntityToMissedCountForLevel( AppState, EntityType_Barrel, SpitterS->nBarrel );
            
            for( int32 iBarrel = 0; iBarrel < SpitterS->nBarrel; iBarrel++ ) {
                BARREL Barrel = SpitterS->Barrel[ iBarrel ];
                
                AddEntityDeathParticles( AppState, Barrel.Position, BARREL_BASE_COLOR );
            }
            SpitterS->nBarrel = 0;
        }
        
        { // DAMAGE_ZONE
            SPITTER_STATE * SpitterS = &AppState->SpitterS;
            
            for( int32 iDamageZone = 0; iDamageZone < SpitterS->nDamageZone; iDamageZone++ ) {
                DAMAGE_ZONE * DamageZone = SpitterS->DamageZone + iDamageZone;
                DamageZone->IsEnabled = false;
            }
        }
        
        { // POWERUPS
            POWERUP_STATE * PowerupS = &AppState->PowerupS;
            PowerupS->Spawn_IsActive = false;
        }
        
        { // GUNNER
            GUNNER_STATE * GunnerS = &AppState->GunnerS;
            
            AddEntityToMissedCountForLevel( AppState, EntityType_Gunner, GunnerS->nGunner );
            
            for( int32 iGunner = 0; iGunner < GunnerS->nGunner; iGunner++ ) {
                GUNNER Gunner = GunnerS->Gunner[ iGunner ];
                
                AddEntityDeathParticles( AppState, Gunner.Position, GUNNER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Gunner.Position, GUNNER_BASE_COLOR );
                AddEntityDeathParticles( AppState, Gunner.Position, COLOR_WHITE );
            }
            GunnerS->nGunner = 0;
        }
        
        { // BOARD ROOM
            BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
            OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
            
            if( BoardRoomS->State_IsActive ) {
                BoardRoomS->Spawn_HitObservationDeck = false;
                BoardRoomS->Spawn_HitMissileLauncher = false;
                BoardRoomS->Spawn_nGunners           = 0;
                BoardRoomS->Spawn_nEntities          = 0;
                BoardRoomS->nSpawnLayout             = 0;
                
                if( ObservationDeck->Health >= 0 ) {
                    PLAYER_STATE * Player = &AppState->Player;
                    if( Player->Health > 0 ) {
                        ObservationDeck->Teleport_tFlash = 4.0f;
                        ObservationDeck->IsPresent       = false;
                    }
                    
                    AddEntityToMissedCountForLevel( AppState, EntityType_ObservationDeck, 1 );
                }
            }
        }
        
        { // MISSILE LAUNCHER
            BOARD_ROOM_STATE * BoardRoomS      = &AppState->BoardRoomS;
            OBSERVATION_DECK * ObservationDeck = &BoardRoomS->ObservationDeck;
            
            if( BoardRoomS->State_IsActive ) {
                int32 nMissileLauncher = 0;
                for( int32 iMissileLauncher = 0; iMissileLauncher < OBSERVATION_DECK_MISSILE_LAUNCHER_COUNT; iMissileLauncher++ ) {
                    MISSILE_LAUNCHER * MissileLauncher = ObservationDeck->MissileLauncher + iMissileLauncher;
                    
                    AddBigEntityDeathParticles( AppState, MissileLauncher->Position, MissileLauncher->Radius, MISSILE_LAUNCHER_BASE_COLOR );
                    AddBigEntityDeathParticles( AppState, MissileLauncher->Position, MissileLauncher->Radius, MISSILE_LAUNCHER_BASE_COLOR );
                    AddBigEntityDeathParticles( AppState, MissileLauncher->Position, MissileLauncher->Radius, COLOR_WHITE, false );
                    
                    if( MissileLauncher->Health > 0 ) {
                        nMissileLauncher += 1;
                    }
                    
                    MissileLauncher->Health = -1;
                }
                
                AddEntityToMissedCountForLevel( AppState, EntityType_MissileLauncher, nMissileLauncher );
            }
        }
        
        { // RECON
            RECON_STATE * ReconS = &AppState->ReconS;
            AddEntityToMissedCountForLevel( AppState, EntityType_Recon, ReconS->nRecon );
        }
    }
    
    WAVE_STATE * WaveS = &AppState->WaveS;
    { // Entity Group Spawn
        for( uint32 iEntityGroupSpawn = 0; iEntityGroupSpawn < WaveS->nEntityGroupSpawn; iEntityGroupSpawn++ ) {
            ENTITY_GROUP_SPAWN EntityGroupSpawn = WaveS->EntityGroupSpawn[ iEntityGroupSpawn ];
            AddKillsToWave( AppState, EntityGroupSpawn );
            
            int32 nEntity = CountTotalEntitiesInEntity( EntityGroupSpawn.Type, EntityGroupSpawn.Counter );
            AddEntityToMissedCountForLevel( AppState, EntityGroupSpawn.Type, nEntity );
        }
        WaveS->nEntityGroupSpawn = 0;
    }
    
    { // Entity Spawn
        ENTITY_SPAWN_STATE * EntitySpawnS = &AppState->EntitySpawnS;
        if( EndLevel ) {
            for( int32 iEntitySpawn = 0; iEntitySpawn < EntitySpawnS->nEntitySpawn; iEntitySpawn++ ) {
                ENTITY EntitySpawn = EntitySpawnS->EntitySpawn[ iEntitySpawn ];
                
                uint32 nEntity = EntitySpawn.nEntity;
                if( ( EntitySpawn.Type == EntityType_Miner ) ||
                   ( EntitySpawn.Type == EntityType_Bomber ) || ( EntitySpawn.Type == EntityType_Miner_Stationary ) ) {
                    nEntity = 1;
                }
                
                if( EntitySpawn.Type != EntityType_Miner_Stationary ) {
                    AddEntityToMissedCountForLevel( AppState, EntitySpawn.Type, nEntity );
                }
            }
            
            EntitySpawnS->nEntitySpawn = 0;
        } else {
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
                    ( EntitySpawn.Type == EntityType_Worm ) ||
                    ( EntitySpawn.Type == EntityType_Gunner );
                
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
}

//----------
// SHIELD
//----------

internal void
HitShield( ENTITY_SHIELD Shield, flo32 * Shield_SegmentT, flo32 InRadians ) {
    flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
    int32 iSegment   = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t );
    int32 MinSegment = MaxValue( iSegment - 1, 0 );
    
    for( int32 Iter = 0; Iter <= 2; Iter++ ) {
        int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
        Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
    }
}

internal void
UpdateShieldSegmentT( flo32 * Shield_SegmentT, flo32 dT ) {
    flo32 Temp[ TURRET_SHIELD_SEGMENT_MAX_COUNT ] = {};
    memcpy( Temp, Shield_SegmentT, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
    
    for( int32 iSegment = 0; iSegment < TURRET_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
        int32 iSegmentA =   iSegment;
        int32 iSegmentB = ( iSegment + 1 ) % TURRET_SHIELD_SEGMENT_MAX_COUNT;
        
        flo32 SegmentTA = Shield_SegmentT[ iSegmentA ];
        flo32 SegmentTB = Shield_SegmentT[ iSegmentB ];
        
        flo32 tA = MaxValue( ( SegmentTB - SegmentTA ) * 0.65f, 0.0f );
        flo32 tB = MaxValue( ( SegmentTA - SegmentTB ) * 0.65f, 0.0f );
        
        Temp[ iSegmentA ] = MinValue( Temp[ iSegmentA ] + tA, TURRET_SHIELD_MAX_T );
        Temp[ iSegmentB ] = MinValue( Temp[ iSegmentB ] + tB, TURRET_SHIELD_MAX_T );
    }
    
    memcpy( Shield_SegmentT, Temp, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
    
    flo32 t = dT * 4.0f;
    for( int32 iSegment = 0; iSegment < TURRET_SHIELD_SEGMENT_MAX_COUNT; iSegment++ ) {
        Shield_SegmentT[ iSegment ] = MaxValue( Shield_SegmentT[ iSegment ] - t, 0.0f );
    }
}

internal boo32
CheckShieldCollisionWithCircle( ENTITY_SHIELD Shield, flo32 * Shield_SegmentT, vec2 Shield_bPos, flo32 Shield_bRadius, vec2 Target_Pos, flo32 Target_Radius, boo32 DoClearOnHit = false ) {
    Assert( Shield.HasShield );
    
    boo32 DoDamage = false;
    
    { // Shield Arc
        flo32 Radius_Inner = Shield_bRadius + Shield.Dist - Target_Radius + 0.1f;
        Radius_Inner = MaxValue( Radius_Inner, 0.0f );
        
        flo32 Radius_Outer = Shield_bRadius + Shield.Dist + Shield.Width + Target_Radius - 0.1f;
        
        flo32 RadiusSq_Inner = Radius_Inner * Radius_Inner;
        flo32 RadiusSq_Outer = Radius_Outer * Radius_Outer;
        flo32 DistSq         = GetLengthSq( Target_Pos - Shield_bPos );
        if( ( DistSq >= RadiusSq_Inner ) && ( DistSq <= RadiusSq_Outer ) ) {
            flo32 MinRadians = Shield.bRadians;
            flo32 MaxRadians = MinRadians + Shield.Radians;
            
            flo32 InRadians = ToRadians( Target_Pos - Shield_bPos );
            if( InRadians < MinRadians ) {
                InRadians += TAU;
            }
            
            if( ( InRadians >= MinRadians ) && ( InRadians <= MaxRadians ) ) {
                if( DoClearOnHit ) {
                    memset( Shield_SegmentT, 0, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
                }
                
                flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
                int32 iSegment   = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t );
                int32 MinSegment = MaxValue( iSegment - 1, 0 );
                
                for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                    int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
                    Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
                }
                
                DoDamage = true;
            }
        }
    }
    
    if( !DoDamage ) { // End Capsules
        flo32 Radius = PLAYER_RADIUS - 0.1f;
        vec2 DirA = ToDirection2D( Shield.bRadians                  );
        vec2 DirB = ToDirection2D( Shield.bRadians + Shield.Radians );
        
        flo32 OffsetA = Shield_bRadius + Shield.Dist;
        flo32 OffsetB = OffsetA + Shield.Width;
        
        CAPSULE2 CapA = Capsule2( Shield_bPos + DirA * OffsetA, Shield_bPos + DirA * OffsetB, Radius );
        CAPSULE2 CapB = Capsule2( Shield_bPos + DirB * OffsetA, Shield_bPos + DirB * OffsetB, Radius );
        
        boo32 IsInCapsuleA = IsInCapsule( Target_Pos, CapA );
        boo32 IsInCapsuleB = IsInCapsule( Target_Pos, CapB );
        if( ( IsInCapsuleA ) || ( IsInCapsuleB ) ) {
            if( DoClearOnHit ) {
                memset( Shield_SegmentT, 0, sizeof( flo32 ) * TURRET_SHIELD_SEGMENT_MAX_COUNT );
            }
            
            int32 iSegment = 0;
            if( IsInCapsuleB ) {
                flo32 t = Clamp01( Shield.Radians / TAU );
                iSegment = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t ) - 1;
            }
            
            int32 MinSegment = MaxValue( iSegment - 1, 0 );
            
            for( int32 Iter = 0; Iter <= 2; Iter++ ) {
                int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
                Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
            }
            
            DoDamage = true;
        }
    }
    
    return DoDamage;
}

internal boo32
CheckShieldCollisionWithRay( RAY2_INTERSECT * BestIntersect, ENTITY_SHIELD Shield, flo32 * Shield_SegmentT, vec2 Shield_bPos, flo32 Shield_bRadius, vec2 Target_PosA, vec2 Target_PosB, flo32 Target_Radius = 0.0f ) {
    Assert( Shield.HasShield );
    
    boo32 Result = false;
    
    flo32 Radius_Outer = Shield_bRadius + Shield.Dist + Shield.Width + Target_Radius + 0.1f;
    flo32 Radius_Inner = Shield_bRadius + Shield.Dist - ( Target_Radius + 0.1f );
    
    RAY2 Ray = Ray2( Target_PosA, Target_PosB - Target_PosA );
    
    RAY2_INTERSECT Intersect = Ray2IntersectInit();
    DoesRayIntersectCircleSegmentExt( &Intersect, Ray, Shield_bPos, Radius_Outer, Shield.bRadians, Shield.bRadians + Shield.Radians );
    
    if( Shield.Radians < TAU ) {
        DoesRayIntersectCircleSegmentInt( &Intersect, Ray, Shield_bPos, Radius_Inner, Shield.bRadians, Shield.bRadians + Shield.Radians );
        flo32 MinRadians = Shield.bRadians;
        flo32 MaxRadians = MinRadians + Shield.Radians;
        
        vec2 A = Shield_bPos + ToDirection2D( MinRadians ) * Radius_Inner;
        vec2 B = Shield_bPos + ToDirection2D( MinRadians ) * Radius_Outer;
        vec2 C = Shield_bPos + ToDirection2D( MaxRadians ) * Radius_Outer;
        vec2 D = Shield_bPos + ToDirection2D( MaxRadians ) * Radius_Inner;
        
        CAPSULE2 CapA = Capsule2( B, A, Target_Radius + 0.1f );
        CAPSULE2 CapB = Capsule2( D, C, Target_Radius + 0.1f );
        
        DoesRayIntersectCapsule( &Intersect, Ray, CapA );
        DoesRayIntersectCapsule( &Intersect, Ray, CapB );
    }
    
    if( ( Intersect.IsValid ) && ( Intersect.t <= BestIntersect->t ) && ( Intersect.t <= 1.0f ) ) {
        flo32 InRadians = ToRadians( Intersect.P - Shield_bPos );
        if( InRadians < Shield.bRadians ) {
            InRadians += TAU;
        }
        
        *BestIntersect = Intersect;
        Result         = true;
        
        flo32 t          = Clamp01( ( InRadians - Shield.bRadians ) / TAU );
        int32 iSegment   = ( int32 )( ( flo32 )TURRET_SHIELD_SEGMENT_MAX_COUNT * t );
        int32 MinSegment = MaxValue( iSegment - 1, 0 );
        
        for( int32 Iter = 0; Iter <= 2; Iter++ ) {
            int32 iSeg = ( MinSegment + Iter ) % TURRET_SHIELD_SEGMENT_MAX_COUNT; 
            Shield_SegmentT[ iSeg ] = TURRET_SHIELD_MAX_T;
        }
    }
    
    return Result;
}

internal void
DrawEntityHealth( RENDER_PASS * Pass, APP_STATE * AppState, CAMERA_STATE * Camera, vec2 Position, int32 Health, vec4 Color ) {
    Assert( AppState );
    Assert( Camera );
    
    char Str[ 4 ] = {};
    sprintf( Str, "%u", Health );
    DrawString( Pass, AppState->FontList + FontID_1_25, Str, WorldToUIPos( AppState, Camera, Position ), TextAlign_Pixel_Center, Vec2( 1.0f, 1.0f ), Color );
}

//----------
// POINTS
//----------

internal void
InitPointsEntry( OVERWORLD_STATE_TEMP_DATA * OverworldT, ENTITY_TYPE Type, int32 nPoints_Positive, int32 nPoints_Negative, int32 nScoreMultiplier ) {
    POINTS_ENTRY * Entry = OverworldT->Points + Type;
    Entry->nPoints_Positive  = nPoints_Positive;
    Entry->nPoints_Negative  = nPoints_Negative;
    Entry->nScoreMultiplier  = nScoreMultiplier;
}

internal void
InitPoints( APP_STATE * AppState ) {
    OVERWORLD_STATE_TEMP_DATA * OverworldT = &AppState->OverworldS.TempData;
    InitPointsEntry( OverworldT, EntityType_Turret,           50, 35, 18 );
    InitPointsEntry( OverworldT, EntityType_Chaser,           10, 15,  2 );
    InitPointsEntry( OverworldT, EntityType_Swarm,             4, -1,  0 );
    InitPointsEntry( OverworldT, EntityType_Drifter,           5, -1,  2 );
    InitPointsEntry( OverworldT, EntityType_Looper,            5, -1,  3 );
    InitPointsEntry( OverworldT, EntityType_EnemyBullet,       2, -1,  1 );
    InitPointsEntry( OverworldT, EntityType_PersonalPod,       2, -1,  0 );
    InitPointsEntry( OverworldT, EntityType_Transport,        10, -1,  3 );
    InitPointsEntry( OverworldT, EntityType_Asteroid,          2, -1,  1 );
    InitPointsEntry( OverworldT, EntityType_Asteroid_Tiny,     2, -1,  1 );
    InitPointsEntry( OverworldT, EntityType_Miner,            50, -1, 20 );
    InitPointsEntry( OverworldT, EntityType_Bank_Hit,          1, -1, 20 );
    InitPointsEntry( OverworldT, EntityType_Bomb,              5, -1,  0 );
    InitPointsEntry( OverworldT, EntityType_BombLauncher,      0, -1, 35 );
    InitPointsEntry( OverworldT, EntityType_Bumper,            0, -1, 16 );
    InitPointsEntry( OverworldT, EntityType_Bumper_Stationary, 0, -1,  4 );
    InitPointsEntry( OverworldT, EntityType_Yacht_Hit,         0, -1,  5 );
    InitPointsEntry( OverworldT, EntityType_BonusPoints,       1, -1,  0 );
    InitPointsEntry( OverworldT, EntityType_Fireball,          2, -1,  1 );
    InitPointsEntry( OverworldT, EntityType_Gunner,           15, -1,  5 );
    InitPointsEntry( OverworldT, EntityType_Missile,          10, -1,  0 );
    InitPointsEntry( OverworldT, EntityType_MissileLauncher, 100, -1, 35 );
    
#if 0    
    InitPointsEntry( MarketS, EntityType_Starburst );
    InitPointsEntry( MarketS, EntityType_Star );
    InitPointsEntry( MarketS, EntityType_MiniBoss );
    InitPointsEntry( MarketS, EntityType_Asteroid_Small );
    InitPointsEntry( MarketS, EntityType_Asteroid_Medium );
    InitPointsEntry( MarketS, EntityType_Asteroid_Large );
    InitPointsEntry( MarketS, EntityType_Bomber );
    InitPointsEntry( MarketS, EntityType_Bomb );
    InitPointsEntry( MarketS, EntityType_Bumper );
    InitPointsEntry( MarketS, EntityType_Bumper_Bounce );
#endif
}

internal POINTS_ENTRY *
GetPointsEntryForEntity( APP_STATE * AppState, ENTITY_TYPE Type ) {
    POINTS_ENTRY * Result = AppState->OverworldS.TempData.Points + Type;
    return Result;
}