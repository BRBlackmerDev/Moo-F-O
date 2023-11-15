
internal BULLET
BulletC( vec2 Position, vec2 Direction, boo32 HideTailForReflectedBullet = false ) {
    flo32 InitOffset = 0.1f;
    
    BULLET Result = {};
    Result.PrevPos   = Position;
    Result.Position  = Position + Direction * InitOffset;
    Result.Direction = Direction;
    Result.Health    = 1;
    Result.HideTailForReflectedBullet = HideTailForReflectedBullet;
    
    return Result;
}

internal void
AddBullet( APP_STATE * AppState, BULLET Bullet ) {
    BULLET_STATE * BulletS = &AppState->BulletS;
    if( BulletS->nBullet < BULLET_MAX_COUNT ) {
        BulletS->Bullet[ BulletS->nBullet++ ] = Bullet;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BULLET to BULLET_STATE: %u", BULLET_MAX_COUNT );
        CONSOLE_ERROR( str );
    }
}

internal void
UpdateBullet( APP_STATE * AppState, flo32 dT ) {
    BULLET_STATE * BulletS = &AppState->BulletS;
    
    for( int32 iBullet = 0; iBullet < BulletS->nBullet; iBullet++ ) {
        BULLET * Bullet = BulletS->Bullet + iBullet;
        
        vec2 dPos = Bullet->Direction * ( BULLET_SPEED * dT );
        Bullet->PrevPos   = Bullet->Position;
        Bullet->Position += dPos;
    }
    
    //DISPLAY_VALUE( uint32, BulletS->nBullet );
}


internal void
DrawBullet( RENDER_PASS * Pass, BULLET Bullet, flo32 Scale = 1.0f ) {
    if( Bullet.DrawCollision ) {
        {
            orect A = ORectCD( Bullet.DrawCollision_P, Vec2( BULLET_WIDTH, BULLET_WIDTH ) * 2.0f, RandomF32() * TAU );
            DrawORect( Pass, A, COLOR_WHITE );
        }
        
        {
            flo32 bRadians = ToRadians( Bullet.DrawCollision_N );
            flo32 RadiansA = bRadians + PI * RandomF32InRange( 0.35f, 0.45f ) - PI * 0.5f;
            flo32 RadiansB = bRadians - PI * RandomF32InRange( 0.35f, 0.45f ) - PI * 0.5f;
            
            vec2  DimA = Vec2( BULLET_WIDTH * 0.75f, BULLET_HEIGHT * RandomF32InRange( 0.35f, 0.75f ) ) * 0.5f;
            vec2  DimB = Vec2( BULLET_WIDTH * 0.75f, BULLET_HEIGHT * RandomF32InRange( 0.35f, 0.75f ) ) * 0.5f;
            
            flo32 AdjHeight = dot( Bullet.Direction, GetPerp( Bullet.DrawCollision_N ) ) * ( BULLET_HEIGHT * 0.2f );
            DimA.y += AdjHeight;
            DimB.y -= AdjHeight;
            
            DrawTriangleBC( Pass, Bullet.DrawCollision_P, DimA, RadiansA, COLOR_WHITE );
            DrawTriangleBC( Pass, Bullet.DrawCollision_P, DimB, RadiansB, COLOR_WHITE );
        }
        
        flo32 Radians = ToRadians( Bullet.Direction ) - PI * 0.5f;
        flo32 Margin  = BULLET_WIDTH * 2.0f;
        
        vec2 V = Bullet.DrawCollision_P - Bullet.Position;
        if( dot( V, Bullet.Direction ) >= 0.0f ) {
            orect BoundA = ORectTD( Bullet.Position, Vec2( 0.5f, 0.0f ), Vec2( BULLET_WIDTH, GetLength( V ) ), Radians );
            orect BoundB = ORectTD( Bullet.Position, Vec2( 0.5f, 1.0f ), Vec2( BULLET_WIDTH, BULLET_HEIGHT ), Radians );
            
            BoundA = ORectAddDim( BoundA, Margin, 0.0f,   Margin, Margin );
            BoundB = ORectAddDim( BoundB, Margin, Margin, Margin, 0.0f   );
            
            DrawORect( Pass, TextureID_PlayerBullet_Head, BoundA, COLOR_WHITE );
            
            if( !Bullet.HideTailForReflectedBullet ) {
                DrawORect( Pass, TextureID_PlayerBullet_Tail, BoundB, COLOR_WHITE );
            }
        } else {
            orect BoundB = ORectTD( Bullet.DrawCollision_P, Vec2( 0.5f, 1.0f ), Vec2( BULLET_WIDTH, BULLET_HEIGHT ), Radians );
            BoundB = ORectAddDim( BoundB, Margin, Margin, Margin, 0.0f   );
            DrawORect( Pass, TextureID_PlayerBullet_Tail, BoundB, COLOR_WHITE );
        }
    } else {
        flo32 Radians = ToRadians( Bullet.Direction ) - PI * 0.5f;
        
        orect BoundA = ORectTD( Bullet.Position, Vec2( 0.5f, 0.0f ), Vec2( BULLET_WIDTH, BULLET_HEIGHT ) * Scale, Radians );
        orect BoundB = ORectTD( Bullet.Position, Vec2( 0.5f, 1.0f ), Vec2( BULLET_WIDTH, BULLET_HEIGHT ) * Scale, Radians );
        
        flo32 Margin = BULLET_WIDTH * 2.0f * Scale;
        
        BoundA = ORectAddDim( BoundA, Margin, 0.0f,   Margin, Margin );
        BoundB = ORectAddDim( BoundB, Margin, Margin, Margin, 0.0f   );
        
        DrawORect( Pass, TextureID_PlayerBullet_Head, BoundA, COLOR_WHITE );
        
        if( !Bullet.HideTailForReflectedBullet ) {
            DrawORect( Pass, TextureID_PlayerBullet_Tail, BoundB, COLOR_WHITE );
        }
        
        //orect Bound = ORectTD( Bullet.Position, Vec2( 0.5f, 0.0f ), BULLET_DIM, ToRadians( Bullet.Direction ) - PI * 0.5f );
        //Bound = ORectAddDim( Bound, 0.0f, BULLET_HEIGHT, 0.0f, 0.0f );
        //Bound.HalfDim += Vec2( BULLET_WIDTH, BULLET_WIDTH ) * 2.0f;
        //DrawORect( Pass, TextureID_Player_Bullet, Bound, COLOR_WHITE );
    }
    
    //DrawPoint( Pass, Bullet.Position + Bullet.Direction * BULLET_HEIGHT, Vec2( 0.25f, 0.25f ), COLOR_RED );
}

internal void
DrawBullet( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BULLET_STATE * BulletS = Draw->BulletS;
    
    for( int32 iBullet = 0; iBullet < BulletS->nBullet; iBullet++ ) {
        BULLET Bullet = BulletS->Bullet[ iBullet ];
        
        DrawBullet( Pass, Bullet );
    }
}

internal void
FinalizeBullet( APP_STATE * AppState ) {
    BULLET_STATE * BulletS = &AppState->BulletS;
    
    rect Remove_Bound = AddRadius( AppState->World_Bound, 2.0f );
    
    uint32 NewCount = 0;
    for( int32 iBullet = 0; iBullet < BulletS->nBullet; iBullet++ ) {
        BULLET Bullet = BulletS->Bullet[ iBullet ];
        Bullet.HideTailForReflectedBullet = false;
        
        if( ( IsInRect( Bullet.Position, Remove_Bound ) ) && ( Bullet.Health > 0 ) ) {
            BulletS->Bullet[ NewCount++ ] = Bullet;
        }
    }
    BulletS->nBullet = NewCount;
}