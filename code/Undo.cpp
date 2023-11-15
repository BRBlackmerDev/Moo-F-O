
// The data in APP_STATE is the current unique data state of the game. This data is not contained in UNDO_STATE. Before the Editor performs an action, the current data state is saved into UNDO_STATE (AtIndex is advanced), then the action is performed.
// Before an Undo action is performed, the current data state is saved into UNDO_STATE. Then the data in the previous undo slot is copied into the editor.
// Before a Redo action is perform, the current data state is saved into UNDO_STATE. Then the data in the next undo/redo slot is copied into the editor.
// Two copies per action? Meh, we'll try this for now and see if there are any performance issues or anything. There are many worse problems. For example, the fact that we are copying the whole state. If there are performance issues, first we should look at only copying the needed data.

internal void
ResetUndo( EDITOR_STATE * Editor ) {
    UNDO_STATE * UndoS = &Editor->UndoS;
    
    UndoS->AtIndex = 0;
    UndoS->nUndo   = 0;
    UndoS->nRedo   = 0;
}

internal void
CaptureUndo( EDITOR_STATE * Editor ) {
    UNDO_STATE   * UndoS  = &Editor->UndoS;
    
    EDITOR__WAVE_STATE * Src  = &Editor->WaveS;
    EDITOR__WAVE_STATE * Dest = UndoS->Undo + UndoS->AtIndex;
    
    memcpy( Dest, Src, sizeof( EDITOR__WAVE_STATE ) );
    
    UndoS->AtIndex = ( int32 )UInt32Wrap( ( uint32 )UndoS->AtIndex, 1, UNDO_MAX_COUNT );
    
    UndoS->nUndo = MinValue( UndoS->nUndo + 1, UNDO_MAX_COUNT );
    UndoS->nRedo = 0;
}

internal void
UpdateUndo( EDITOR_STATE * Editor, KEYBOARD_STATE * Keyboard, boo32 CanUpdate ) {
    UNDO_STATE * UndoS = &Editor->UndoS;
    
    //DISPLAY_VALUE( int32, UndoS->AtIndex );
    //DISPLAY_VALUE( int32, UndoS->nUndo );
    //DISPLAY_VALUE( int32, UndoS->nRedo );
    
    if( CanUpdate ) {
        if( ( WasPressed( Keyboard, KeyCode_z, KEYBOARD_FLAGS__CONTROL ) ) && ( UndoS->nUndo > 0 ) ) {
            { // Save Data Out
                EDITOR__WAVE_STATE * Src  = &Editor->WaveS;
                EDITOR__WAVE_STATE * Dest = UndoS->Undo + UndoS->AtIndex;
                
                memcpy( Dest, Src, sizeof( EDITOR__WAVE_STATE ) );
            }
            
            UndoS->AtIndex = ( int32 )UInt32Wrap( ( uint32 )UndoS->AtIndex, -1, UNDO_MAX_COUNT );
            
            { // Load Data In
                EDITOR__WAVE_STATE * Src  = UndoS->Undo + UndoS->AtIndex;
                EDITOR__WAVE_STATE * Dest = &Editor->WaveS;
                
                memcpy( Dest, Src, sizeof( EDITOR__WAVE_STATE ) ); 
            }
            
            UndoS->nUndo = MaxValue( UndoS->nUndo - 1, 0 );
            UndoS->nRedo = MinValue( UndoS->nRedo + 1, UNDO_MAX_COUNT );
        }
        if( ( WasPressed( Keyboard, KeyCode_y, KEYBOARD_FLAGS__CONTROL ) ) && ( UndoS->nRedo > 0 ) ) {
            { // Save Data Out
                EDITOR__WAVE_STATE * Src  = &Editor->WaveS;
                EDITOR__WAVE_STATE * Dest = UndoS->Undo + UndoS->AtIndex;
                
                memcpy( Dest, Src, sizeof( EDITOR__WAVE_STATE ) );
            }
            
            UndoS->AtIndex = ( int32 )UInt32Wrap( ( uint32 )UndoS->AtIndex, 1, UNDO_MAX_COUNT );
            
            { // Load Data In
                EDITOR__WAVE_STATE * Src  = UndoS->Undo + UndoS->AtIndex;
                EDITOR__WAVE_STATE * Dest = &Editor->WaveS;
                
                memcpy( Dest, Src, sizeof( EDITOR__WAVE_STATE ) ); 
            }
            
            UndoS->nUndo = MinValue( UndoS->nUndo + 1, UNDO_MAX_COUNT );
            UndoS->nRedo = MaxValue( UndoS->nRedo - 1, 0 );
        }
    }
}