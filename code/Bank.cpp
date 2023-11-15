
internal BANK
BankC( int32 WaveGuid, vec2 Position ) {
    BANK Result = {};
    Result.WaveGuid = WaveGuid;
    Result.Position = Position;
    Result.Radius   = BANK_RADIUS;
    Result.Health   = 1;
    Result.Points   = 1;
    
    return Result;
}

internal void
AddBank( APP_STATE * AppState, BANK Bank ) {
    BANK_STATE * BankS = &AppState->BankS;
    if( BankS->nBank < BANK_MAX_COUNT ) {
        BankS->Bank[ BankS->nBank++ ] = Bank;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to add BANK to BANK_STATE: %u", BANK_MAX_COUNT );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
}

internal void
UpdateBank( APP_STATE * AppState, flo32 dT ) {
    BANK_STATE * BankS = &AppState->BankS;
    PLAYER_STATE  * Player   = &AppState->Player;
    
    for( int32 iBank = 0; iBank < BankS->nBank; iBank++ ) {
        BANK * Bank = BankS->Bank + iBank;
        
        { // Radius
            flo32 TargetTimeA = 0.0f;
            
            flo32 Numer = Bank->Exit_Timer - TargetTimeA;
            flo32 Denom = BANK_EXIT_TARGET_TIME - TargetTimeA;
            flo32 t = Clamp01( Numer / Denom );
            t = t * t;
            
            Bank->Radius = lerp( BANK_RADIUS, t, 0.4f );
        }
        
        Bank->Exit_Timer += dT;
        if( Bank->Exit_Timer >= BANK_EXIT_TARGET_TIME ) {
            Bank->Health = 0;
        }
    }
}

internal void
DrawBank( RENDER_PASS * Pass, BANK Bank, flo32 Alpha = 1.0f, flo32 Scale = 1.0f ) {
    vec4 ColorA = BANK_BASE_COLOR * Alpha;
    
    flo32 RadiusA = Bank.Radius * Scale;
    flo32 RadiusB = MaxValue( RadiusA - 0.2f, 0.0f );
    
    if( RadiusA > 0.0f ) {
        DrawCircle( Pass, Bank.Position, RadiusA, ColorA );
    }
    if( RadiusB > 0.0f ) {
        DrawCircle( Pass, Bank.Position, RadiusB, COLOR_BLACK );
    }
}

internal void
DrawBank( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    BANK_STATE * BankS = Draw->BankS;
    
    for( int32 iBank = 0; iBank < BankS->nBank; iBank++ ) {
        BANK Bank = BankS->Bank[ iBank ];
        
        DrawBank( Pass, Bank );
    }
}

internal void
FinalizeBank( APP_STATE * AppState ) {
    BANK_STATE * BankS = &AppState->BankS;
    
    uint32 NewCount = 0;
    for( int32 iBank = 0; iBank < BankS->nBank; iBank++ ) {
        BANK Bank = BankS->Bank[ iBank ];
        
        if( Bank.Health > 0 ) {
            BankS->Bank[ NewCount++ ] = Bank;
        } else {
            AddEntityDeathParticles( AppState, Bank.Position, BANK_BASE_COLOR );
            AddKillToWave( AppState, Bank.WaveGuid );
        }
    }
    BankS->nBank = NewCount;
}