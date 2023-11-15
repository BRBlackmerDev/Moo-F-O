
#define VERSION__LEVEL  ( 1 )
internal void
SaveLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    EDITOR_STATE   * Editor = &AppState->Editor;
    FILE_SAVE_OPEN * Save   = &Editor->SaveOpen;
    
    AppState->Mode = AppMode_Editor;
    
    // filetag
    // version
    // nWaves
    // for each wave
    //  type
    //   data size? confirm match on load?
    //   ( type is an enum (ie. index). how do we make sure that the values in the files stay in sync with the correct enum? should i use a String id? guid?)
    //   type data
    //   ( how do we make sure that the file data stays in sync with the struct data? should i use an entity String id and version number for each entity type? )
    //  exit
    //   exit data
    //  next wave
    // TODO: Think about this problem! This will become clearer as we design more elements of the game. Currently, the save file format will only support WaveType_SpawnEntities, ExitType_Instant, and ExitType_TargetTime
    
    { // Match Guid to Index
        EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WAVE * Wave = WaveS->Wave + iWave;
            
            for( uint32 iNextWave = 0; iNextWave < Wave->nNextWave; iNextWave++ ) {
                int32 Guid  = Wave->NextWave_Guid[ iNextWave ];
                Assert( Guid > -1 );
                
                int32 Index = -1;
                for( uint32 Iter = 0; ( Index == -1 ) && ( Iter < WaveS->nWave ); Iter++ ) {
                    WAVE * W = WaveS->Wave + Iter;
                    if( W->Guid == Guid ) {
                        Index = Iter;
                    }
                }
                Assert( Index != ( int32 )iWave );
                Wave->NextWave[ iNextWave ] = Index;
            }
        }
    }
    
    { // Set Entity Wave Guid
        EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WAVE * Wave = WaveS->Wave + iWave;
            
            switch( Wave->Type ) {
                case WaveType_SpawnEntities: {
                    for( uint32 iEntity = 0; iEntity < Wave->nEntity; iEntity++ ) {
                        ENTITY * Entity = Wave->Entity + iEntity;
                        Entity->WaveGuid = Wave->Guid;
                    }
                } break;
            }
            
            Wave->nKills               = 0;
            Wave->nTotalEntitiesInWave = CountTotalEntitiesInWave( Wave );
        }
    }
    
    char * FileTag  = FILETAG__LEVEL;
    uint32 Version  = VERSION__LEVEL;
    char * SaveDir  = LEVEL_SAVE_DIRECTORY;
    char * FileName = Save->CurrentFile;
    
    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
    
    if( WaveS->nWave > 0 ) {
        MEMORY  _Output = SubArena( TempMemory );
        MEMORY * Output = &_Output;
        
        WriteEntityFileHeader( Output, FileTag, Version );
        
        _writem( Output, int32,  WaveS->CurrentGuid  );
        _writem( Output, uint32, WaveS->StartingWave );
        
        Editor->Level_Settings.World_YUnits = Editor->World_YUnits;
        _writem( Output, LEVEL_SETTINGS, Editor->Level_Settings );
        
        _writem( Output, uint32, WaveS->nWave );
        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WAVE Wave = WaveS->Wave[ iWave ];
            _writem( Output, WAVE, Wave );
        }
        
        boo32 IsValid = OutputFile( Platform, Output, SaveDir, FileName, FileTag );
        if( IsValid ) {
            Save->UnsavedChanges = false;
            Save->DoRecentSave   = true;
        }
        
        _PopSize( TempMemory, Output->Size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal WAVE
ReadWave( uint32 Version, uint8 ** Ptr ) {
    WAVE Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            WAVE Wave = {};
            Wave.Guid             = _read( ptr, int32 );
            Wave.Layout_TextureID = _read( ptr, TEXTURE_ID );
            Wave.Layout_Position  = _read( ptr, vec2 );
            
            Wave.nRepeatWave = _read( ptr, int32 );
            
            Wave.Type = _read( ptr, WAVE_TYPE );
            Wave.Exit = _read( ptr, EXIT_TYPE );
            
            Wave.nKills               = _read( ptr, int32 );
            Wave.nTotalEntitiesInWave = _read( ptr, int32 );
            
            Wave.nEntity = _read( ptr, uint32 );
            
            for( uint32 iEntity = 0; iEntity < WAVE_ENTITY_MAX_COUNT; iEntity++ ) {
                ENTITY Entity = {};
                Entity.WaveGuid = _read( ptr, int32 );
                Entity.Type     = _read( ptr, ENTITY_TYPE );
                Entity.nEntity  = _read( ptr, int32 );
                Entity.Position = _read( ptr, vec2 );
                Entity.Radius   = _read( ptr, flo32 );
                
                Entity.Shield = _read( ptr, ENTITY_SHIELD );
                
                Wave.Entity[ iEntity ] = Entity;
            }
            
            Wave.Exit_Timer          = _read( ptr, flo32 );
            Wave.Exit_TargetTime     = _read( ptr, flo32 );
            int32 Exit_Minutes       = _read( ptr, int32 );
            int32 Exit_Seconds       = _read( ptr, int32 );
            
            Wave.InDelay_Timer       = _read( ptr, flo32 );
            Wave.InDelay_TargetTime  = _read( ptr, flo32 );
            int32 InDelay_Minutes    = _read( ptr, int32 );
            int32 InDelay_Seconds    = _read( ptr, int32 );
            
            Wave.DoOutDelay          = _read( ptr, boo32 );
            Wave.OutDelay_Timer      = _read( ptr, flo32 );
            Wave.OutDelay_TargetTime = _read( ptr, flo32 );
            int32 OutDelay_Minutes   = _read( ptr, int32 );
            int32 OutDelay_Seconds   = _read( ptr, int32 );
            
            Wave.nNextWave = _read( ptr, uint32 );
            for( uint32 iNextWave = 0; iNextWave < NEXT_WAVE_MAX_COUNT; iNextWave++ ) {
                Wave.NextWave     [ iNextWave ] = _read( ptr, int32 );
            }
            for( uint32 iNextWave = 0; iNextWave < NEXT_WAVE_MAX_COUNT; iNextWave++ ) {
                Wave.NextWave_Guid[ iNextWave ] = _read( ptr, int32 );
            }
            
            Wave.nKillWave = _read( ptr, uint32 );
            for( uint32 iKillWave = 0; iKillWave < KILL_WAVE_MAX_COUNT; iKillWave++ ) {
                Wave.KillWave_Guid[ iKillWave ] = _read( ptr, int32 );
            }
            
            Result = Wave;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
EDITOR_LoadLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__LEVEL;
    
    EDITOR_STATE       * Editor = &AppState->Editor;
    EDITOR__WAVE_STATE * WaveS  = &Editor->WaveS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.Contents ) {
        uint8 * ptr = ( uint8 * )File.Contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        WaveS->CurrentGuid  = _read( ptr, int32 );
        WaveS->StartingWave = _read( ptr, uint32 );
        
        Editor->Level_Settings = _read( ptr, LEVEL_SETTINGS );
        
        Editor->World_YUnits = Editor->Level_Settings.World_YUnits;
        Editor->World_Bound  = GetWorldBound( AppState->App_Dim, Editor->World_YUnits );
        
        WaveS->nWave = _read( ptr, uint32 );
        for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
            WAVE Src = ReadWave( Version, &ptr );
            WaveS->Wave[ iWave ] = Src;
        }
        
        _PopSize( TempMemory, File.Size );
    }
}

internal void
EDITOR_LoadBitmaps( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    EDITOR_STATE   * Editor = &AppState->Editor;
    FILE_SAVE_OPEN * Save   = &Editor->SaveOpen;
    
    char * SaveDir  = LEVEL_SAVE_DIRECTORY;
    char * FileName = Save->CurrentFile;
    
    MEMORY * Bitmap_Memory = &Editor->LayoutBitmap_Memory;
    ResetMemory( Bitmap_Memory );
    
    EDITOR__WAVE_STATE * WaveS = &Editor->WaveS;
    for( uint32 iWave = 0; iWave < WaveS->nWave; iWave++ ) {
        WAVE * Wave = WaveS->Wave + iWave;
        
        TEXTURE_ID TextureID = ( TEXTURE_ID )( TextureID_Wave_Bitmap + iWave );
        Wave->Layout_TextureID = TextureID;
        
        char Name[ 128 ] = {};
        sprintf( Name, "%s-%04u", FileName, Wave->Guid );
        UploadBMPFromFile( Platform, Bitmap_Memory, Wave->Layout_TextureID, SaveDir, Name );
    }
}