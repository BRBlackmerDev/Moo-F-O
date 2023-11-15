
internal boo32
AddStockPurchaseButton( MOUSE_STATE * Mouse, vec2 MouseP, RENDER_PASS * Pass, FONT * Font, char * Text, rect Bound ) {
    boo32 Result = false;
    vec4  Color_Outline    = COLOR_BLACK;
    vec4  Color_Background = COLOR_GRAY( 0.1f );
    
    if( IsInRect( MouseP, Bound ) ) {
        Color_Outline    = COLOR_GRAY( 0.5f );
        Color_Background = COLOR_GRAY( 0.2f );
        
        if( WasPressed( Mouse, MouseButton_Left ) ) {
            Result = true;
            
            Color_Background = Vec4( 0.5f, 0.3f, 0.3f, 1.0f );
        }
    }
    
    DrawRect       ( Pass, Bound, Color_Background );
    DrawRectOutline( Pass, Bound, Color_Outline );
    DrawString( Pass, Font, Text, GetCenter( Bound ), TextAlign_Center, WORLD_TEXT_SCALE, COLOR_WHITE );
    
    return Result;
}

internal void
InitStockEntry( MARKET_STATE_SAVE_DATA * MarketS, ENTITY_TYPE Type, boo32 IsEnabled, boo32 IsAvailable, uint32 Price, flo32 PriceChange_StockRate, flo32 PriceChange_EntityRate ) {
    STOCK_ENTRY * Stock = MarketS->Stock + Type;
    if( IsEnabled ) {
        Stock->Flags                 |= STOCK_ENTRY_FLAG__IS_ENABLED;
    }
    if( IsAvailable ) {
        Stock->Flags                 |= STOCK_ENTRY_FLAG__IS_AVAILABLE;
    }
    Stock->Price                  = Price;
    Stock->PriceChange_StockRate  = PriceChange_StockRate;
    Stock->PriceChange_EntityRate = PriceChange_EntityRate;
}

internal void
InitProductEntry( MARKET_STATE_SAVE_DATA * MarketS, ENTITY_TYPE Type, flo32 Percent ) {
    PRODUCT_ENTRY * Product = MarketS->Product + Type;
    Product->Flags      |= PRODUCT_ENTRY_FLAG__IS_AVAILABLE;
    Product->Percent     = Percent;
}

internal void
InitMarket( APP_STATE * AppState ) {
    MARKET_STATE_SAVE_DATA * MarketS = &AppState->MarketS.SaveData;
    MARKET_STATE_TEMP_DATA * MarketT = &AppState->MarketS.TempData;
    
    //MarketS->TotalPoints = 1000000000;
    
    InitStockEntry  ( MarketS, EntityType_Turret,  true, true, 250, 1.0f,  4.0f );
    InitStockEntry  ( MarketS, EntityType_Chaser,  true, true, 150, 1.0f,  5.0f );
    InitStockEntry  ( MarketS, EntityType_Drifter, true, true, 200, 1.0f, 10.0f );
    InitStockEntry  ( MarketS, EntityType_Looper,  true, true, 175, 1.0f,  8.0f );
    //InitProductEntry( MarketS, EntityType_Chaser, 1.0f );
    
    //InitStockEntry  ( MarketS, EntityType_Swarm, 1000, 1.0f, 40.0f );
    //InitProductEntry( MarketS, EntityType_Swarm, 1.0f );
    
    //InitProductEntry( MarketS, EntityType_Drifter, 1.0f );
    
    //InitProductEntry( MarketS, EntityType_Looper, 1.0f );
    
    //InitStockEntry  ( MarketS, EntityType_PersonalPod, 1000, 20.0f, 40.0f );
    //InitProductEntry( MarketS, EntityType_PersonalPod, 1.0f );
}