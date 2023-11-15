
internal void
AddParticle( APP_STATE * AppState, PARTICLE Particle ) {
    PARTICLE_STATE * ParticleS = &AppState->ParticleS;
    if( ParticleS->nParticle < PARTICLE_MAX_COUNT ) {
        ParticleS->Particle[ ParticleS->nParticle++ ] = Particle;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add PARTICLE to PARTICLE_STATE: %u", PARTICLE_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateParticle( APP_STATE * AppState, flo32 dT ) {
    PARTICLE_STATE * ParticleS = &AppState->ParticleS;
    
    for( uint32 iParticle = 0; iParticle < ParticleS->nParticle; iParticle++ ) {
        PARTICLE * Particle = ParticleS->Particle + iParticle;
        Particle->Timer += dT;
        
        vec2 Accel = ( Particle->Accel - Particle->Velocity ) * Particle->Friction;
        Particle->Position += Accel * ( dT * dT * 0.5f ) + Particle->Velocity * dT;
        Particle->Velocity += Accel * dT;
        
        Particle->Dim += Particle->dDim * dT;
        Particle->Dim.x = MaxValue( Particle->Dim.x, 0.0f );
        Particle->Dim.y = MaxValue( Particle->Dim.y, 0.0f );
        
        Particle->Radians = WrapRadians( Particle->Radians + Particle->dRadians * dT );
        
        Particle->Color.elem[ 0 ] = Clamp01( Particle->Color.elem[ 0 ] + Particle->dColor.elem [ 0 ] * dT );
        Particle->Color.elem[ 1 ] = Clamp01( Particle->Color.elem[ 1 ] + Particle->dColor.elem [ 1 ] * dT );
        Particle->Color.elem[ 2 ] = Clamp01( Particle->Color.elem[ 2 ] + Particle->dColor.elem [ 2 ] * dT );
        Particle->Color.elem[ 3 ] = Clamp01( Particle->Color.elem[ 3 ] + Particle->dColor.elem [ 3 ] * dT );
    }
}

internal void
DrawParticle( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    PARTICLE_STATE * ParticleS = Draw->ParticleS;
    
    for( uint32 iParticle = 0; iParticle < ParticleS->nParticle; iParticle++ ) {
        PARTICLE Particle = ParticleS->Particle[ iParticle ];
        
        TEXTURE_ID TextureID = Particle.TextureID;
        if( TextureID == 0 ) {
            TextureID = TextureID_WhiteTexture;
        }
        
        orect R     = ORectCD( Particle.Position, Particle.Dim, Particle.Radians );
        vec4  Color = Particle.Color;
        Color.rgb  *= Color.a;
        
        DrawORect( Pass, TextureID, R, Color );
    }
}

internal void
FinalizeParticle( APP_STATE * AppState ) {
    PARTICLE_STATE * ParticleS = &AppState->ParticleS;
    
    uint32 nParticle = 0;
    for( uint32 iParticle = 0; iParticle < ParticleS->nParticle; iParticle++ ) {
        PARTICLE Particle = ParticleS->Particle[ iParticle ];
        
        if( Particle.Timer < Particle.TargetTime ) {
            ParticleS->Particle[ nParticle++ ] = Particle;
        }
    }
    ParticleS->nParticle = nParticle;
}