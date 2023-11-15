
internal FILE_DATA
Win32_ReadFile( MEMORY * Memory, char * SaveDir, char * FileName, char * extension ) {
	FILE_DATA Result = {};
	
	char FilePath[ 2048 ] = {};
    GenFilePath( FilePath, SaveDir, FileName, extension );
	
	HANDLE File = CreateFile( FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if( File != INVALID_HANDLE_VALUE ) {
		LARGE_INTEGER _FileSize = {};
		if( GetFileSizeEx( File, &_FileSize ) ) {
            uint32 FileSize = ( uint32 )_FileSize.QuadPart;
            uint8 * dest = ( uint8 * )_PushSize( Memory, FileSize );
			
			DWORD bytesRead = {};
            BOOL  Status    = ReadFile( File, dest, FileSize, &bytesRead, 0 );
            if( ( Status ) && ( FileSize == bytesRead ) ) {
                Result.Contents = dest;
                Result.Size     = FileSize;
			} else {
				_PopSize( Memory, Result.Size );
                
                char Debug[ 128 ] {};
                sprintf( Debug, "ERROR! File: %s : unable to read contents!\n", FilePath );
                OutputDebugString( Debug );
			}
			CloseHandle( File );
		} else {
            char Debug[ 128 ] = {};
            sprintf( Debug, "ERROR! File: %s : does not contain any data!\n", FilePath );
            OutputDebugString( Debug );
		}
	} else {
        char Debug[ 128 ] = {};
        sprintf( Debug, "ERROR! File: %s : unable to open!\n", FilePath );
        OutputDebugString( Debug );
	}
	
	return Result;
}

internal boo32
Win32_WriteFile( char * SaveDir, char * FileName, char * extension, void * data, uint32 size ) {
	boo32 Result = false;
	
	char FilePath[ 2048 ] = {};
    GenFilePath( FilePath, SaveDir, FileName, extension );
    
	HANDLE FileHandle = CreateFile( FilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
	if( FileHandle != INVALID_HANDLE_VALUE ) {
		DWORD bytesWritten;
		if( WriteFile( FileHandle, data, size, &bytesWritten, 0 ) ) {
			Result = ( bytesWritten == size );
		}
        
		CloseHandle( FileHandle );
	}
    
	return Result;
}

internal boo32
Win32_DoesFileExist( char * SaveDir, char * FileName, char * extension ) {
	char FilePath[ 2048 ] = {};
    GenFilePath( FilePath, SaveDir, FileName, extension );
    
    boo32 Result = PathFileExists( FilePath );
    return Result;
}

internal boo32
Win32_DeleteFile( char * SaveDir, char * FileName, char * extension ) {
	char FilePath[ 2048 ] = {};
    GenFilePath( FilePath, SaveDir, FileName, extension );
    
    boo32 Result = DeleteFile( FilePath );
    return Result;
}

uint32 Frame_Counter = 0;
internal void
Win32_outputMessage( char * header, char * message ) {
    char str[ 64 ] = {};
    sprintf( str, "%10u : %8s : %s\n", Frame_Counter, header, message );
    OutputDebugString( str );
}

internal void
Win32_logMessage( uint32 message, boo32 fromWndProc ) {
    char * header0 = "WndProc";
    char * header1 = "my";
    
    char * header = ( fromWndProc ? header0 : header1 );
    
    switch( message ) {
        case WM_ACTIVATE: {
            // Sent to both the window being activated and the window being deactivated. If the windows use the same input queue, the message is sent synchronously, first to the window procedure of the Top-level window being deactivated, then to the window procedure of the Top-level window being activated. If the windows use different input queues, the message is sent asynchronously, so the window is activated immediately.
            Win32_outputMessage( header, "WM_ACTIVATE" );
        } break;
        
        case WM_ACTIVATEAPP: {
            // Sent when the active window changes. Sent to both the activated window and the deactivated window.
            Win32_outputMessage( header, "WM_ACTIVATEAPP" );
        } break;
        
        case WM_CAPTURECHANGED: {
            // Sent to the window that is losing the Mouse capture.
            Win32_outputMessage( header, "WM_CAPTURECHANGED" );
        } break;
        
        case WM_CLOSE: {
            // Sent as a signal that a window or an application should terminate.
            Win32_outputMessage( header, "WM_CLOSE" );
        } break;
        
        case WM_DESTROY: {
            // Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
            Win32_outputMessage( header, "WM_DESTROY" );
        } break;
        
        case WM_GETMINMAXINFO: {
            // Sent to a window when its size or Position is about to change.
            Win32_outputMessage( header, "WM_GETMINMAXINFO" );
        } break;
        
        case WM_GETICON: {
            // Sent to a window to retrieve the handle to the large or small icon associated with the window.
            Win32_outputMessage( header, "WM_GETICON" );
        } break;
        
        case WM_IME_SETCONTEXT: {
            // Sent to an application when a window is activated. A window receives this message through its WindowProc function.
            Win32_outputMessage( header, "WM_IME_SETCONTEXT" );
        } break;
        
        case WM_IME_NOTIFY: {
            // Sent to an application to notify it of changes to the IME window. A window receives this message through its WindowProc function.
            Win32_outputMessage( header, "WM_IME_NOTIFY" );
        } break;
        
        case WM_KILLFOCUS: {
            // Sent to a window immediately before it loses the Keyboard focus.
            Win32_outputMessage( header, "WM_KILLFOCUS" );
        } break;
        
        case WM_SETFOCUS: {
            // Sent to a window after it has gained the Keyboard focus.
            Win32_outputMessage( header, "WM_KILLFOCUS" );
        } break;
        
        case WM_MOUSEMOVE: {
            // Posted to a window when the cursor moves. If the Mouse is not captured, the message is posted to the window that contains the cursor. Otherwise, the message is posted to the window that has captured the Mouse.
            // Win32_outputMessage( header, "WM_MOUSEMOVE" );
        } break;
        
        case WM_NCACTIVATE: {
            // Sent to a window when its nonclient area needs to be changed to indicate an active or inactive State.
            Win32_outputMessage( header, "WM_NCACTIVATE" );
        } break;
        
        case WM_NCCREATE: {
            // Sent when a window is first created. Sent prior to the WM_CREATE message.
            Win32_outputMessage( header, "WM_NCCREATE" );
        } break;
        
        case WM_NCCALCSIZE: {
            // Sent when the size and Position of a window's client area must be calculated.
            Win32_outputMessage( header, "WM_NCCALCSIZE" );
        } break;
        
        case WM_NCDESTROY: {
            // Notifies a window that its nonclient area is being destroyed. The DestroyWindow function sends the WM_NCDESTROY message to the window following the WM_DESTROY message.WM_DESTROY is used to free the allocated Memory object associated with the window. The WM_NCDESTROY message is sent after the child windows have been destroyed. In contrast, WM_DESTROY is sent before the child windows are destroyed. A window receives this message through its WindowProc function.
            Win32_outputMessage( header, "WM_NCDESTROY" );
        } break;
        
        case WM_NCHITTEST: {
            // Sent to a window to determine what part of the window corresponds to a particular screen coordinate. Sent, for example, in response to Mouse cursor movement or when a Mouse button is pressed/released.
            // Win32_outputMessage( header, "WM_NCHITTEST" );
        } break;
        
        case WM_NCLBUTTONDOWN: {
            // Sent to a window when the user presses the Left Mouse button while the cursor is within the nonclient area of the window.
            Win32_outputMessage( header, "WM_NCLBUTTONDOWN" );
        } break;
        
        case WM_NCMOUSEMOVE: {
            // Posted to a window when the cursor is moved within the nonclient area of the window. This message is posted to the window that contains the cursor. If a window has captured the Mouse, this message is not posted.
            // Win32_outputMessage( header, "WM_NCMOUSEMOVE" );
        } break;
        
        case WM_PAINT: {
            // Sent when the system or another application makes a request to paint a portion of the application's window.
            Win32_outputMessage( header, "WM_PAINT" );
        } break;
        
        case WM_QUIT: {
            // Indicates a request to terminate an application, and is generated when the application calls the PostQuitMessage function.
            Win32_outputMessage( header, "WM_QUIT" );
        } break;
        
        case WM_SETCURSOR: {
            // Sent if the Mouse cursor moves, but the input is not captured by the window.
            //Win32_outputMessage( header, "WM_SETCURSOR" );
        } break;
        
        case WM_SYSCOMMAND: {
            // A window receives this message when the user chooses a command from the Window menu (formerly known as the system or control menu) or when the user chooses the maximize button, minimize button, restore button, or close button.
            Win32_outputMessage( header, "WM_SYSCOMMAND" );
        } break;
		
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            Win32_outputMessage( header, "Windows Key Message" );
            // ERROR! Keyboard message was Passed directly to WindowProc! All Keyboard messages should be handled directly by the Win32_processPendingMessages()!
        } break;
        
        case WM_TIMER: {
            // Posted to the installing thread's message queue when a Timer expires. The message is posted by the GetMessage or PeekMessage function.
            // Win32_outputMessage( header, "WM_TIMER" );
        } break;
        
        case WM_WINDOWPOSCHANGED: {
            // Sent to a window whose size, Position, or place in the Z order has changed as a Result of a call to the SetWindowPos function or another window-management function.
            Win32_outputMessage( header, "WM_WINDOWPOSCHANGED" );
        } break;
        
        case WM_WINDOWPOSCHANGING: {
            // Sent to a window whose size, Position, or place in the Z order is about to change as a Result of a call to the SetWindowPos function or another window-management function.
            Win32_outputMessage( header, "WM_WINDOWPOSCHANGING" );
        } break;
        
        default: {
            char str[ 8 ] = {};
            sprintf( str, "0x%04X", ( uint32 )message );
            Win32_outputMessage( header, str );
        } break;
    }
}

global_variable boo32 global_restoreFocus = false;

LRESULT CALLBACK
Win32_WindowProc( HWND window, uint32 message, WPARAM wParam, LPARAM lParam ) {
    //Win32_logMessage( message, true );
	LRESULT Result = 0;
    switch( message ) {
        case WM_ACTIVATEAPP: {
            global_restoreFocus = true;
        } break;
        
        case WM_DESTROY: {
            PostQuitMessage( 0 );
        } break;
        
        default: {
            Result = DefWindowProc( window, message, wParam, lParam );
        } break;
    }
    return Result;
}

internal void
Win32_processPendingMessages( PLATFORM * Platform, HWND windowHandle ) {
    KEYBOARD_STATE * Keyboard = &Platform->Keyboard;
    Keyboard->nEvents = 0;
    
    MOUSE_STATE * Mouse = &Platform->Mouse;
    MOUSE_endOfFrame( Mouse );
    
	MSG message = {};
    while( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) ) {
        //Win32_logMessage( message.message, false );
        
        switch( message.message ) {
            case WM_QUIT: {
                Platform->IsRunning = false;
            } break;
            
			case WM_MOUSEWHEEL: {
				int16 wParam_hi = ( ( message.wParam >> 16 ) & 0xFFFF );
				int32 WheelClicks = wParam_hi / 120;
				Mouse->WheelClicks = WheelClicks;
			} break;
			
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				addButtonEvent( Mouse, MouseButton_Left, ( message.wParam & MK_LBUTTON ), Keyboard->flags );
			} break;
			
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP: {
				addButtonEvent( Mouse, MouseButton_Middle, ( message.wParam & MK_MBUTTON ), Keyboard->flags );
			} break;
			
			case WM_RBUTTONUP:
			case WM_RBUTTONDOWN: {
				addButtonEvent( Mouse, MouseButton_Right, ( message.wParam & MK_RBUTTON ), Keyboard->flags );
			} break;
			
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP: {
                boo32 wasDown = ( ( message.lParam & ( 1 << 30 ) ) != 0 );
				boo32 IsDown  = ( ( message.lParam & ( 1 << 31 ) ) == 0 );
				if( IsDown != wasDown ) {
                    switch( message.wParam ) {
						case VK_BACK: { addKeyEvent( Keyboard, KeyCode_Backspace, IsDown, Keyboard->flags ); } break;
						case VK_TAB: 		{ addKeyEvent( Keyboard, KeyCode_tab, IsDown, Keyboard->flags ); } break;
						case VK_RETURN:	{ addKeyEvent( Keyboard, KeyCode_Enter, IsDown, Keyboard->flags ); } break;
						case VK_SHIFT: 	{
                            if( IsDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__SHIFT;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__SHIFT;
                            }
							addKeyEvent( Keyboard, KeyCode_shift, IsDown, Keyboard->flags );
						} break;
						case VK_CONTROL: {
                            if( IsDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__CONTROL;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__CONTROL;
                            }
							addKeyEvent( Keyboard, KeyCode_control, IsDown, Keyboard->flags );
						} break;
						case VK_MENU: 		{
                            if( IsDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__ALT;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__ALT;
                            }
                            addKeyEvent( Keyboard, KeyCode_alt, IsDown, Keyboard->flags );
                        } break;
						case VK_CAPITAL: 	{ addKeyEvent( Keyboard, KeyCode_capsLock, 	IsDown, Keyboard->flags ); } break;
						case VK_ESCAPE: 	{ addKeyEvent( Keyboard, KeyCode_Escape, 		IsDown, Keyboard->flags ); } break;
						case VK_SPACE: 	{ addKeyEvent( Keyboard, KeyCode_space, 		IsDown, Keyboard->flags ); } break;
						case VK_PRIOR: 	{ addKeyEvent( Keyboard, KeyCode_pageUp, 		IsDown, Keyboard->flags ); } break;
						case VK_NEXT: 		{ addKeyEvent( Keyboard, KeyCode_pageDown, 	IsDown, Keyboard->flags ); } break;
						case VK_END: 		{ addKeyEvent( Keyboard, KeyCode_end, 			IsDown, Keyboard->flags ); } break;
						case VK_HOME: 		{ addKeyEvent( Keyboard, KeyCode_home, 		IsDown, Keyboard->flags ); } break;
						case VK_LEFT: 		{ addKeyEvent( Keyboard, KeyCode_Left, 		IsDown, Keyboard->flags ); } break;
						case VK_RIGHT: 	{ addKeyEvent( Keyboard, KeyCode_Right, 		IsDown, Keyboard->flags ); } break;
						case VK_UP: 		{ addKeyEvent( Keyboard, KeyCode_Up, 			IsDown, Keyboard->flags ); } break;
						case VK_DOWN: 		{ addKeyEvent( Keyboard, KeyCode_Down,       IsDown, Keyboard->flags ); } break;
						case VK_INSERT: 	{ addKeyEvent( Keyboard, KeyCode_insert, 		IsDown, Keyboard->flags ); } break;
						case VK_DELETE: 	{ addKeyEvent( Keyboard, KeyCode_delete, 		IsDown, Keyboard->flags ); } break;
						case 0x30: { addKeyEvent( Keyboard, KeyCode_0, IsDown, Keyboard->flags ); } break;
						case 0x31: { addKeyEvent( Keyboard, KeyCode_1, IsDown, Keyboard->flags ); } break;
						case 0x32: { addKeyEvent( Keyboard, KeyCode_2, IsDown, Keyboard->flags ); } break;
						case 0x33: { addKeyEvent( Keyboard, KeyCode_3, IsDown, Keyboard->flags ); } break;
						case 0x34: { addKeyEvent( Keyboard, KeyCode_4, IsDown, Keyboard->flags ); } break;
						case 0x35: { addKeyEvent( Keyboard, KeyCode_5, IsDown, Keyboard->flags ); } break;
						case 0x36: { addKeyEvent( Keyboard, KeyCode_6, IsDown, Keyboard->flags ); } break;
						case 0x37: { addKeyEvent( Keyboard, KeyCode_7, IsDown, Keyboard->flags ); } break;
						case 0x38: { addKeyEvent( Keyboard, KeyCode_8, IsDown, Keyboard->flags ); } break;
						case 0x39: { addKeyEvent( Keyboard, KeyCode_9, IsDown, Keyboard->flags ); } break;
						case 0x41: { addKeyEvent( Keyboard, KeyCode_a, IsDown, Keyboard->flags ); } break;
						case 0x42: { addKeyEvent( Keyboard, KeyCode_b, IsDown, Keyboard->flags ); } break;
						case 0x43: { addKeyEvent( Keyboard, KeyCode_c, IsDown, Keyboard->flags ); } break;
						case 0x44: { addKeyEvent( Keyboard, KeyCode_d, IsDown, Keyboard->flags ); } break;
						case 0x45: { addKeyEvent( Keyboard, KeyCode_e, IsDown, Keyboard->flags ); } break;
						case 0x46: { addKeyEvent( Keyboard, KeyCode_f, IsDown, Keyboard->flags ); } break;
						case 0x47: { addKeyEvent( Keyboard, KeyCode_g, IsDown, Keyboard->flags ); } break;
						case 0x48: { addKeyEvent( Keyboard, KeyCode_h, IsDown, Keyboard->flags ); } break;
						case 0x49: { addKeyEvent( Keyboard, KeyCode_i, IsDown, Keyboard->flags ); } break;
						case 0x4A: { addKeyEvent( Keyboard, KeyCode_j, IsDown, Keyboard->flags ); } break;
						case 0x4B: { addKeyEvent( Keyboard, KeyCode_k, IsDown, Keyboard->flags ); } break;
						case 0x4C: { addKeyEvent( Keyboard, KeyCode_l, IsDown, Keyboard->flags ); } break;
						case 0x4D: { addKeyEvent( Keyboard, KeyCode_m, IsDown, Keyboard->flags ); } break;
						case 0x4E: { addKeyEvent( Keyboard, KeyCode_n, IsDown, Keyboard->flags ); } break;
						case 0x4F: { addKeyEvent( Keyboard, KeyCode_o, IsDown, Keyboard->flags ); } break;
						case 0x50: { addKeyEvent( Keyboard, KeyCode_p, IsDown, Keyboard->flags ); } break;
						case 0x51: { addKeyEvent( Keyboard, KeyCode_q, IsDown, Keyboard->flags ); } break;
						case 0x52: { addKeyEvent( Keyboard, KeyCode_r, IsDown, Keyboard->flags ); } break;
						case 0x53: { addKeyEvent( Keyboard, KeyCode_s, IsDown, Keyboard->flags ); } break;
						case 0x54: { addKeyEvent( Keyboard, KeyCode_t, IsDown, Keyboard->flags ); } break;
						case 0x55: { addKeyEvent( Keyboard, KeyCode_u, IsDown, Keyboard->flags ); } break;
						case 0x56: { addKeyEvent( Keyboard, KeyCode_v, IsDown, Keyboard->flags ); } break;
						case 0x57: { addKeyEvent( Keyboard, KeyCode_w, IsDown, Keyboard->flags ); } break;
						case 0x58: { addKeyEvent( Keyboard, KeyCode_x, IsDown, Keyboard->flags ); } break;
						case 0x59: { addKeyEvent( Keyboard, KeyCode_y, IsDown, Keyboard->flags ); } break;
						case 0x5A: { addKeyEvent( Keyboard, KeyCode_z, IsDown, Keyboard->flags ); } break;
						case VK_OEM_1: { addKeyEvent( Keyboard, KeyCode_semicolon, IsDown, Keyboard->flags ); } break;
						case VK_OEM_PLUS: { addKeyEvent( Keyboard, KeyCode_equal, IsDown, Keyboard->flags ); } break;
						case VK_OEM_COMMA: { addKeyEvent( Keyboard, KeyCode_comma, IsDown, Keyboard->flags ); } break;
						case VK_OEM_MINUS: { addKeyEvent( Keyboard, KeyCode_dash, IsDown, Keyboard->flags ); } break;
						case VK_OEM_PERIOD: { addKeyEvent( Keyboard, KeyCode_period, IsDown, Keyboard->flags ); } break;
						case VK_OEM_2: { addKeyEvent( Keyboard, KeyCode_forwardSlash, IsDown, Keyboard->flags ); } break;
						case VK_OEM_3: { addKeyEvent( Keyboard, KeyCode_tilde, IsDown, Keyboard->flags ); } break;
						case VK_OEM_4: { addKeyEvent( Keyboard, KeyCode_openBracket, IsDown, Keyboard->flags ); } break;
						case VK_OEM_5: { addKeyEvent( Keyboard, KeyCode_BackSlash, IsDown, Keyboard->flags ); } break;
						case VK_OEM_6: { addKeyEvent( Keyboard, KeyCode_closeBracket, IsDown, Keyboard->flags ); } break;
						case VK_OEM_7: { addKeyEvent( Keyboard, KeyCode_quote, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD0: { addKeyEvent( Keyboard, KeyCode_num0, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD1: { addKeyEvent( Keyboard, KeyCode_num1, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD2: { addKeyEvent( Keyboard, KeyCode_num2, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD3: { addKeyEvent( Keyboard, KeyCode_num3, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD4: {addKeyEvent( Keyboard, KeyCode_num4, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD5: { addKeyEvent( Keyboard, KeyCode_num5, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD6: { addKeyEvent( Keyboard, KeyCode_num6, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD7: { addKeyEvent( Keyboard, KeyCode_num7, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD8: { addKeyEvent( Keyboard, KeyCode_num8, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD9: { addKeyEvent( Keyboard, KeyCode_num9, IsDown, Keyboard->flags ); } break;
						case VK_MULTIPLY: { addKeyEvent( Keyboard, KeyCode_numMul, IsDown, Keyboard->flags ); } break;
						case VK_ADD: { addKeyEvent( Keyboard, KeyCode_numAdd, IsDown, Keyboard->flags ); } break;
						case VK_DECIMAL: { addKeyEvent( Keyboard, KeyCode_numPeriod, IsDown, Keyboard->flags ); } break;
						case VK_SUBTRACT: { addKeyEvent( Keyboard, KeyCode_numSub, IsDown, Keyboard->flags ); } break;
						case VK_DIVIDE: { addKeyEvent( Keyboard, KeyCode_numDiv, IsDown, Keyboard->flags ); } break;
						case VK_F1:      { addKeyEvent( Keyboard, KeyCode_F1,   IsDown, Keyboard->flags ); } break;
						case VK_F2:      { addKeyEvent( Keyboard, KeyCode_F2,   IsDown, Keyboard->flags ); } break;
						case VK_F3:      { addKeyEvent( Keyboard, KeyCode_F3,   IsDown, Keyboard->flags ); } break;
						case VK_F4:      { addKeyEvent( Keyboard, KeyCode_F4,   IsDown, Keyboard->flags ); } break;
						case VK_F5:      { addKeyEvent( Keyboard, KeyCode_F5,   IsDown, Keyboard->flags ); } break;
						case VK_F6:      { addKeyEvent( Keyboard, KeyCode_F6,   IsDown, Keyboard->flags ); } break;
						case VK_F7:      { addKeyEvent( Keyboard, KeyCode_F7,   IsDown, Keyboard->flags ); } break;
						case VK_F8:      { addKeyEvent( Keyboard, KeyCode_F8,   IsDown, Keyboard->flags ); } break;
						case VK_F9:      { addKeyEvent( Keyboard, KeyCode_F9,   IsDown, Keyboard->flags ); } break;
						case VK_F10:     { addKeyEvent( Keyboard, KeyCode_F10,  IsDown, Keyboard->flags ); } break;
						case VK_F11:     { addKeyEvent( Keyboard, KeyCode_F11,  IsDown, Keyboard->flags ); } break;
						case VK_F12:     { addKeyEvent( Keyboard, KeyCode_F12,  IsDown, Keyboard->flags ); } break;
						default: {
							char String[ 128 ] = {};
							sprintf( String, "Key message received, but not processed: %lu %s\n", (uint32)message.wParam, ( IsDown ? "PRESSED" : "RELEASED" ) );
							OutputDebugString( String );
						} break;
					};
				}
			} break;
            
            default: {
                TranslateMessage( &message );
                DispatchMessage ( &message );
            } break;
        }
	}
}

internal void
InitPass( RENDERER * Renderer, RENDER_PASS * Pass, MEMORY * Memory, uint32 nTri, uint32 nLine, uint32 nText ) {
    Pass->TriObjectList.maxObjects  = nTri;
    Pass->LineObjectList.maxObjects = nLine;
    Pass->TextList.maxObjects       = nText;
    
    Pass->TriObjectList.object  = _PushArray( Memory,      RENDER_OBJECT, nTri  );
    Pass->LineObjectList.object = _PushArray( Memory,      RENDER_OBJECT, nLine );
    Pass->TextList.object       = _PushArray( Memory, TEXT_RENDER_OBJECT, nText + DEBUG_SYSTEM__SAFETY_OBJECT_COUNT );
    
    Pass->ModelBuffer = &Renderer->ModelBuffer;
    Pass->textBuffer  = &Renderer->textBuffer;
}

internal void
RenderPass( RENDERER * Renderer, RENDER_PASS * Pass, boo32 EnableDepth = false, boo32 EnableBlend = false ) {
    ID3D12GraphicsCommandList * CommandList = Renderer->CommandList;
    
    { // Draw Models
        VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
        CommandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        CommandList->IASetIndexBuffer  ( &buffer->indexView );
        
        CommandList->SetGraphicsRoot32BitConstants( 0, 16, Pass->CameraTransform_Tri.elem, 0 );
        
        { // triangle Models
            RENDER_OBJECT_LIST * objectList = &Pass->TriObjectList;
            
            if( EnableDepth ) {
                if( EnableBlend ) {
                    CommandList->SetPipelineState( Renderer->PSS_tri_Depth_Blend );
                } else {
                    CommandList->SetPipelineState( Renderer->PSS_tri_depth_noBlend );
                }
            } else {
                CommandList->SetPipelineState( Renderer->PSS_tri_noDepth_blend );
            }
            CommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
            
            //D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID_WhiteTexture );
            //CommandList->SetGraphicsRootDescriptorTable( 2, handle );
            
            TEXTURE_ID PrevTextureID = TextureID_Count;
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    ModelData = Renderer->ModelData[ object.ModelID ];
                
                CommandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                TEXTURE_ID TextureID = TextureID_DefaultTexture;
                if( Renderer->Texture_isLoaded[ object.TextureID ] ) {
                    TextureID = object.TextureID;
                }
                
                if( PrevTextureID != TextureID ) {
                    D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID );
                    CommandList->SetGraphicsRootDescriptorTable( 2, handle );
                    
                    PrevTextureID = TextureID;
                }
                
                CommandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                CommandList->DrawIndexedInstanced( ModelData.nIndex, 1, ModelData.bIndex, ModelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END triangle Models
        
        CommandList->SetGraphicsRoot32BitConstants( 0, 16, Pass->CameraTransform_Line.elem, 0 );
        
        { // line Models
            RENDER_OBJECT_LIST * objectList = &Pass->LineObjectList;
            
            CommandList->SetPipelineState( Renderer->PSS_line_noDepth_blend );
            CommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP );
            
            D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID_WhiteTexture );
            CommandList->SetGraphicsRootDescriptorTable( 2, handle );
            
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    ModelData = Renderer->ModelData[ object.ModelID ];
                
                CommandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                CommandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                CommandList->DrawIndexedInstanced( ModelData.nIndex, 1, ModelData.bIndex, ModelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END line Models
    }
    
    CommandList->SetGraphicsRoot32BitConstants( 0, 16, Pass->CameraTransform_Text.elem, 0 );
    
    { // Draw overlay text
        
        CommandList->SetPipelineState( Renderer->PSS_tri_noDepth_blend );
        
        VERTEX1_BUFFER * buffer = &Renderer->textBuffer;
        buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
        
        CommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        CommandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        
        //D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID_Font );
        //CommandList->SetGraphicsRootDescriptorTable( 2, handle );
        
        mat4 Model_transform = mat4_Identity();
        CommandList->SetGraphicsRoot32BitConstants( 0, 16, Model_transform.elem, 16 );
        
        TEXTURE_ID PrevTextureID = TextureID_Count;
        
        
        TEXT_RENDER_OBJECT_LIST * objectList = &Pass->TextList;
        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
            TEXT_RENDER_OBJECT object = objectList->object[ iObject ];
            
            TEXTURE_ID TextureID = TextureID_DefaultTexture;
            if( Renderer->Texture_isLoaded[ object.TextureID ] ) {
                TextureID = object.TextureID;
            }
            
            if( PrevTextureID != TextureID ) {
                D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID );
                CommandList->SetGraphicsRootDescriptorTable( 2, handle );
                
                PrevTextureID = TextureID;
            }
            
            CommandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
            
            CommandList->DrawInstanced( object.nVertex, 1, object.bVertex, 0 );
        }
        objectList->nObjects = 0;
    }
}

internal int64
Win32_GetPerfCount() {
	LARGE_INTEGER PerfCounter = {};
	QueryPerformanceCounter( &PerfCounter );
	
	int64 Result = *( ( int64 * )&PerfCounter );
	return Result;
}


internal flo32
Win32_GetMSElapsed( int64 StartCounter, int64 EndCounter, int64 PerfFrequency ) {
	LARGE_INTEGER CounterA  = *( ( LARGE_INTEGER * )&StartCounter  );
	LARGE_INTEGER CounterB  = *( ( LARGE_INTEGER * )&EndCounter    );
	LARGE_INTEGER Frequency = *( ( LARGE_INTEGER * )&PerfFrequency );
	flo32 Result = 1000.0f * ( ( flo32 )( CounterB.QuadPart - CounterA.QuadPart ) / ( flo32 )( Frequency.QuadPart ) );
	return Result;
}

int32 CALLBACK
WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int32 windowShowCode ) {
    srand( ( uint32 )time( 0 ) );
    
	boo32 SleepFreqIsHiRes = ( timeBeginPeriod( 1 ) == TIMERR_NOERROR );
    Assert( SleepFreqIsHiRes );
    
    LARGE_INTEGER PerfFrequency;
    QueryPerformanceFrequency( &PerfFrequency );
    
    uint64 Memory_size = 0;
    Memory_size += sizeof( APP_STATE );
    Memory_size += sizeof( PLATFORM  );
    Memory_size += APP_PermMemorySize;
    Memory_size += APP_TempMemorySize;
    
    MEMORY PlatformMemory = {};
    PlatformMemory.Size = Memory_size;
    PlatformMemory.Base = VirtualAlloc( 0, Memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
    
    APP_STATE * AppState = _PushType( &PlatformMemory, APP_STATE );
    PLATFORM  * Platform = _PushType( &PlatformMemory, PLATFORM  );
    Platform->TargetSec  = 1.0f / APP_TargetFPS;
    
    Platform->PermMemory = SubArena( &PlatformMemory, APP_PermMemorySize );
    Platform->TempMemory = SubArena( &PlatformMemory, APP_TempMemorySize );
    Platform->ReadFile      = Win32_ReadFile;
    Platform->WriteFile     = Win32_WriteFile;
    Platform->DoesFileExist = Win32_DoesFileExist;
    Platform->DeleteFile    = Win32_DeleteFile;
    
    int32 Screen_Width       = GetSystemMetrics( SM_CXSCREEN );
    int32 Screen_Height      = GetSystemMetrics( SM_CYSCREEN );
    flo32 Screen_FPS         = 60.0f;
    flo32 Screen_AspectRatio = ( flo32 )Screen_Width / ( flo32 )Screen_Height;
    
    D3D12_EnableDebug();
    DIRECTX_12_DISPLAY_SETTINGS DisplaySettings = D3D12_GetDisplay( &Platform->TempMemory, Screen_Width, Screen_Height, Screen_FPS );
    
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc   = Win32_WindowProc;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = "WindowClass";
    windowClass.hCursor       = LoadCursor( 0, IDC_ARROW );
    
    if( RegisterClass( &windowClass ) ) {
        // uint32 window_flags = WS_OVERLAPPEDWINDOW; // Windowed
        uint32 window_flags = ( WS_VISIBLE | WS_EX_TOPMOST | WS_POPUP ); // Fullscreen
        HWND window = CreateWindowEx( 0, "WindowClass", "Caption", window_flags, 0, 0, DisplaySettings.Width, DisplaySettings.Height, 0, 0, instance, 0 );
        
        if( window ) {
            Platform->Renderer = InitDirectX12( DisplaySettings, window );
            
            RENDERER     * Renderer    = &Platform->Renderer;
            MEMORY       * PermMemory  = &Platform->PermMemory;
            MEMORY       * TempMemory  = &Platform->TempMemory;
            AUDIO_SYSTEM * AudioSystem = &Platform->AudioSystem;
            
            CreateShader( Renderer, DisplaySettings );
            
            ShowWindow( window, SW_SHOW );
            
            Platform->IsRunning = true;
            
            ResetCommandList( Renderer );
            
            int64 FontPerf_Start = Win32_GetPerfCount();
            
            flo32 Dev_Width       = 1920.0f;
            flo32 Dev_Height      = 1080.0f;
            flo32 Dev_AspectRatio = Dev_Width / Dev_Height;
            
            flo32 Screen_xScale = 1.0f;
            flo32 Screen_yScale = 1.0f;
            
            if( Screen_AspectRatio < Dev_AspectRatio ) {
                Screen_yScale = Dev_AspectRatio / Screen_AspectRatio;
            } else {
                Screen_xScale = Dev_AspectRatio / Screen_AspectRatio;
            }
            
            
            // NOTE: FontScale can only be used after text is being drawn to a render buffer that matches the screen dimensions, not the dev dimensions.
            flo32 FontScale = ( flo32 )Screen_Height / Dev_Height;
            flo32 bFontSize = 16.0f;
            
            AppState->FontList[ FontID_0_75 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 0.75f );
            AppState->FontList[ FontID_0_90 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 0.90f );
            AppState->FontList[ FontID_1_00 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 1.00f );
            AppState->FontList[ FontID_1_25 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 1.25f );
            AppState->FontList[ FontID_1_35 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 1.35f );
            AppState->FontList[ FontID_1_50 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 1.50f );
            AppState->FontList[ FontID_1_75 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 1.75f );
            AppState->FontList[ FontID_2_00 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 2.00f );
            AppState->FontList[ FontID_2_25 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 2.25f );
            AppState->FontList[ FontID_2_50 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 2.50f );
            AppState->FontList[ FontID_2_75 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 2.75f );
            AppState->FontList[ FontID_3_00 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 3.00f );
            AppState->FontList[ FontID_7_00 ] = LoadFont( FONT_SAVE_DIRECTORY, "Egosta", bFontSize * 7.00f );
            
            AppState->FontList[ FontID_Title_1_00 ]  = LoadFont( FONT_SAVE_DIRECTORY, "EgostaBold", bFontSize * 1.0f );
            AppState->FontList[ FontID_Title_2_00 ]  = LoadFont( FONT_SAVE_DIRECTORY, "EgostaBold", bFontSize * 2.0f );
            AppState->FontList[ FontID_Title_2_50 ]  = LoadFont( FONT_SAVE_DIRECTORY, "EgostaBold", bFontSize * 2.5f );
            AppState->FontList[ FontID_Title_3_00 ]  = LoadFont( FONT_SAVE_DIRECTORY, "EgostaBold", bFontSize * 3.0f );
            AppState->FontList[ FontID_Title_3_50 ]  = LoadFont( FONT_SAVE_DIRECTORY, "EgostaBold", bFontSize * 3.5f );
            AppState->FontList[ FontID_Title_4_00 ]  = LoadFont( FONT_SAVE_DIRECTORY, "EgostaBold", bFontSize * 4.0f );
            AppState->FontList[ FontID_Title_14_00 ] = LoadFont( FONT_SAVE_DIRECTORY, "EgostaBold", bFontSize * 14.0f );
            
            for( int32 iFont = 0; iFont < FontID_Count; iFont++ ) {
                FONT * Font = AppState->FontList + iFont;
                Font->TextureID = TextureID_Font_0_75 + iFont;
                UploadTexture( Renderer, ( TEXTURE_ID )Font->TextureID, Font->Texture_Width, Font->Texture_Height, Font->Texture_Data );
            }
            
#if ( ( ENABLE_DEBUG_REPLAY ) || ( DEBUG_BUILD ) )
            FONT Font  = LoadFont( "LiberationMono.ttf", bFontSize );
            Font.TextureID = TextureID_Font;
            UploadTexture( Renderer, TextureID_Font, Font.Texture_Width, Font.Texture_Height, Font.Texture_Data );
            AppState->Font        = &Font;
#endif
            
            int64 FontPerf_End = Win32_GetPerfCount();
            flo32 FontPerf_MSElapsed = Win32_GetMSElapsed( FontPerf_Start, FontPerf_End, PerfFrequency.QuadPart );
            CONSOLE_VALUE( flo32, FontPerf_MSElapsed );
            
            RENDER_PASS * Pass_Game            = &Renderer->Pass_Game;
            RENDER_PASS * Pass_UI              = &Renderer->Pass_UI;
            RENDER_PASS * Pass_Cow_TitleScreen = &Renderer->Pass_Cow_TitleScreen;
            RENDER_PASS * Pass_Cow_Overworld   = &Renderer->Pass_Cow_Overworld;
            RENDER_PASS * Pass_FadeOut         = &Renderer->Pass_FadeOut;
            
            InitPass( Renderer, Pass_Game,            PermMemory, 32768, 4096, 512 );
            InitPass( Renderer, Pass_UI,              PermMemory, 4096,  4096, 512 );
            InitPass( Renderer, Pass_Cow_TitleScreen, PermMemory,    2,     0,   0 );
            InitPass( Renderer, Pass_Cow_Overworld,   PermMemory,    2,     0,   0 );
            InitPass( Renderer, Pass_FadeOut,         PermMemory,    1,     0,   0 );
            
#if 0            
            Renderer->ModelData[ MeshID_Line            ] = GenLine           ( Renderer );
            Renderer->ModelData[ MeshID_Rect            ] = GenRect           ( Renderer );
            Renderer->ModelData[ MeshID_RectOutline     ] = GenRectOutline    ( Renderer );
            Renderer->ModelData[ MeshID_Circle          ] = GenCircle         ( Renderer );
            Renderer->ModelData[ MeshID_CircleOutline   ] = GenCircleOutline  ( Renderer );
            Renderer->ModelData[ MeshID_Triangle        ] = GenTriangle       ( Renderer );
            Renderer->ModelData[ MeshID_Triangle_FlipUV ] = GenTriangleFlipUV ( Renderer );
            Renderer->ModelData[ MeshID_TriangleOutline ] = GenTriangleOutline( Renderer );
            //Renderer->ModelData[ MeshID_Hexagon         ] = GenHexagon        ( Renderer );
            //Renderer->ModelData[ MeshID_Octagon         ] = GenOctagon        ( Renderer );
            
            Renderer->ModelData[ MeshID_ObservationDeck_Barrier ] = GenModel_ObservationDeckBarrier( Renderer, TempMemory );
            Renderer->ModelData[ MeshID_BlimpA ] = GenModel_BlimpA( Renderer, TempMemory );
            Renderer->ModelData[ MeshID_BlimpB ] = GenModel_BlimpB( Renderer, TempMemory );
            Renderer->ModelData[ MeshID_BlimpC ] = GenModel_BlimpC( Renderer, TempMemory );
            Renderer->ModelData[ MeshID_Turret_Shield_EndCap ] = GenModel_Turret_Shield_EndCap( Renderer );
            //Renderer->ModelData[ MeshID_Cow ] = GenModel_Cow( Renderer, TempMemory );
            
            
            {
                MESH Mesh = LoadXFile( Platform, PermMemory, TempMemory, ART_SAVE_DIRECTORY, "Cow" );
                Renderer->ModelData[ MeshID_Cow ] = UploadModel( &Renderer->ModelBuffer, Model.Vertex, Model.nVertex, ( uint32 * )Model.Index, Model.nIndex );
            }
            {
                MODEL Model = LoadXFile( Platform, PermMemory, TempMemory, ART_SAVE_DIRECTORY, "Spacesuit" );
                Renderer->ModelData[ MeshID_Spacesuit ] = UploadModel( &Renderer->ModelBuffer, Model.Vertex, Model.nVertex, ( uint32 * )Model.Index, Model.nIndex );
            }
#endif
            
            int64 TexturePerf_Start = Win32_GetPerfCount();
            
            {
                FILE_DATA File = Win32_ReadFile( TempMemory, 0, "Asset", "pak" );
                if( File.Contents ) {
                    uint8 * Ptr = ( uint8 * )File.Contents;
                    
                    { // TEXTURES
                        int32 nTexture = _read( Ptr, int32 );
                        for( int32 iTexture = 0; iTexture < nTexture; iTexture++ ) {
                            int32 Width  = _read( Ptr, int32 );
                            int32 Height = _read( Ptr, int32 );
                            
                            if( ( Width > 0 ) && ( Height > 0 ) ) {
                                uint32 * Texture_Data = _addra( Ptr, uint32, Width * Height );
                                UploadTexture( Renderer, ( TEXTURE_ID )iTexture, Width, Height, Texture_Data );
                            }
                        }
                        
                        _PopSize( TempMemory, File.Size );
                    }
                    
                    { // MESHES
                        int32 nMesh = _read( Ptr, int32 );
                        for( int32 iMesh = 0; iMesh < nMesh; iMesh++ ) {
                            MODEL_ID MeshID = _read( Ptr, MODEL_ID );
                            
                            int32 nVertex = _read( Ptr, int32 );
                            int32 nIndex  = _read( Ptr, int32 );
                            
                            VERTEX1 * Vertex = _addra( Ptr, VERTEX1, nVertex );
                            int32   * Index  = _addra( Ptr, int32,   nIndex  );
                            
                            Renderer->ModelData[ MeshID ] = UploadModel( &Renderer->ModelBuffer, Vertex, nVertex, ( uint32 * )Index, nIndex );
                        }
                    }
                    
                    { // LEVELS
                        int32 nLevel = _read( Ptr, int32 );
                        for( int32 iLevel = 0; iLevel < nLevel; iLevel++ ) {
                            LEVEL_ID LevelID = _read( Ptr, LEVEL_ID );
                            
                            ASSET_LEVEL Level = {};
                            Level.StartingWave = ( int32 )_read( Ptr, uint32 );
                            Level.World_YUnits = _read( Ptr, flo32 );
                            Level.nWave        = ( int32 )_read( Ptr, uint32 );
                            Level.Wave         = _PushArray_Clear( PermMemory, WAVE, Level.nWave );
                            for( int32 iWave = 0; iWave < Level.nWave; iWave++ ) {
                                Level.Wave[ iWave ] = _read( Ptr, WAVE );
                            }
                            
                            AppState->LevelList[ LevelID ] = Level;
                        }
                    }
                    
                    { // AUDIO
                        int32 nAudio = _read( Ptr, int32 );
                        for( int32 iAudio = 0; iAudio < nAudio; iAudio++ ) {
                            AUDIO_ID AudioID = _read( Ptr, AUDIO_ID );
                            
                            int32 nSamples   = _read( Ptr, int32 );
                            int32 SampleSize = AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT;
                            
                            int32 Size = nSamples * SampleSize;
                            
                            uint8 * Dest = ( uint8 * )_PushSize( PermMemory, Size );
                            memcpy( Dest, Ptr, nSamples * SampleSize );
                            
                            Ptr += ( nSamples * SampleSize );
                            
                            AUDIO_DATA Audio = {};
                            Audio.Audio    = Dest;
                            Audio.nSamples = nSamples;
                            AppState->AudioList[ AudioID ] = Audio;
                        }
                    }
                }
            }
            
#if ENABLE_EDITOR
            { // Load Wave Editor Background
                UploadBMPFromFile( Platform, &Platform->PermMemory, TextureID_WaveEditorBackground_AutoTurret, ART_SAVE_DIRECTORY, "WaveEditorBackground_AutoTurret" );
                UploadBMPFromFile( Platform, &Platform->PermMemory, TextureID_WaveEditorBackground_Asteroid, ART_SAVE_DIRECTORY, "WaveEditorBackground_Asteroid" );
                UploadBMPFromFile( Platform, &Platform->PermMemory, TextureID_WaveEditorBackground_Turret, ART_SAVE_DIRECTORY, "WaveEditorBackground_Turret" );
                UploadBMPFromFile( Platform, &Platform->PermMemory, TextureID_WaveEditorBackground_BombLauncher, ART_SAVE_DIRECTORY, "WaveEditorBackground_BombLauncher" );
                UploadBMPFromFile( Platform, &Platform->PermMemory, TextureID_WaveEditorBackground_Worm, ART_SAVE_DIRECTORY, "WaveEditorBackground_Worm" );
                UploadBMPFromFile( Platform, &Platform->PermMemory, TextureID_WaveEditorBackground_Spitter, ART_SAVE_DIRECTORY, "WaveEditorBackground_Spitter" );
            }
#endif
            
            int64 TexturePerf_End = Win32_GetPerfCount();
            flo32 TexturePerf_MSElapsed = Win32_GetMSElapsed( TexturePerf_Start, TexturePerf_End, PerfFrequency.QuadPart );
            CONSOLE_VALUE( flo32, TexturePerf_MSElapsed );
            
            executeCommandList( Renderer );
            
#if 0            
            { // TEMP : Generate shading texture for Spacesuit
                MODEL Model = LoadXFile( Platform, PermMemory, TempMemory, ART_SAVE_DIRECTORY, "Jetpack_ForShading" );
                Assert( Model.nIndex > 0 );
                Assert( ( Model.nIndex % 3 ) == 0 );
                
                int32 Width     = 1024;
                int32 Height    = 1024;
                int32 nTexel    = Width * Height;
                
                vec2  Dim = Vec2( ( flo32 )Width, ( flo32 )Height );
                
                uint32 * Texture = _PushArray_Clear( TempMemory, uint32, nTexel );
                
                for( int32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
                    Texture[ iTexel ] = 0xFFFFFFFF;
                }
                
                flo32 GrayA = 1.0f;
                flo32 GrayB = 0.0f;
                
                for( int32 iIndex = 0; iIndex < Model.nIndex; iIndex += 3 ) {
                    VERTEX1 V0 = Model.Vertex[ Model.Index[ iIndex + 0 ] ];
                    VERTEX1 V1 = Model.Vertex[ Model.Index[ iIndex + 1 ] ];
                    VERTEX1 V2 = Model.Vertex[ Model.Index[ iIndex + 2 ] ];
                    
                    vec3   N = GetNormal( Cross( V1.Position - V0.Position, V2.Position - V0.Position ) );
                    flo32 tN = dot( N, Vec3( 0.0f, 0.0f, 1.0f ) ) * 0.5f + 0.5f;
                    tN = fabsf( dot( N, Vec3( 0.0f, 0.0f, 1.0f ) ) );
                    
                    vec2 P0 = V0.TexCoord * Dim;
                    vec2 P1 = V1.TexCoord * Dim;
                    vec2 P2 = V2.TexCoord * Dim;
                    
                    vec2 Min = {};
                    vec2 Max = {};
                    Min.x = MinValue( MinValue( P0.x, P1.x ), P2.x );
                    Min.y = MinValue( MinValue( P0.y, P1.y ), P2.y );
                    Max.x = MaxValue( MaxValue( P0.x, P1.x ), P2.x );
                    Max.y = MaxValue( MaxValue( P0.y, P1.y ), P2.y );
                    
                    int32 Row_Min = RoundToInt32( Min.y );
                    int32 Col_Min = RoundToInt32( Min.x );
                    int32 Row_Max = RoundToInt32( Max.y );
                    int32 Col_Max = RoundToInt32( Max.x );
                    
                    for( int32 iRow = Row_Min; iRow <= Row_Max; iRow++ ) {
                        for( int32 iCol = Col_Min; iCol <= Col_Max; iCol++ ) {
                            vec2  Pos    = Vec2( ( flo32 )iCol, ( flo32 )iRow ) + Vec2( 0.5f, 0.5f );
                            flo32 Dist   = DistPointToTri( Pos, P0, P1, P2 );
                            if( ( IsInTri( Pos, P0, P1, P2 ) ) || ( Dist <= 0.5f ) ) {
                                flo32 tGray  = lerp( GrayB, tN, GrayA );
                                int32 iTexel = iRow * Width + iCol;
                                Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
                            }
                        }
                    }
                    
                    //flo32 tGray  = lerp( GrayA, tN, GrayB );
                    //int32 iTexel = RoundToInt32( P0.y ) * Width + RoundToInt32( P0.x );
                    //Texture[ iTexel ] = ToU32Color( COLOR_GRAY( tGray ) );
                }
                
                BMP_WriteFile( Platform, TempMemory, ART_SAVE_DIRECTORY, "Jetpack_Shading", Width, Height, Texture );
                
                _PopArray( TempMemory, uint32, nTexel );
            }
#endif
            
            WIN32_AUDIO Win32_Audio = InitAudio( window, AudioSystem, PermMemory );
            
            { // Load Audio Files
                //AppState->AudioList[ AudioID_Music_TitleScreen_Intro ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_TitleScreen_Intro" );
                //AppState->AudioList[ AudioID_Music_TitleScreen_Loop ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_TitleScreen_Loop" );
                //AppState->AudioList[ AudioID_Music_Level_Track01_Intro ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track01_Intro" );
                //AppState->AudioList[ AudioID_Music_Level_Track01_Loop ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track01_Loop" );
                //AppState->AudioList[ AudioID_Music_Level_Track03_Intro ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track03_Intro" );
                //AppState->AudioList[ AudioID_Music_Level_Track03_Loop ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track03_Loop" );
                //AppState->AudioList[ AudioID_Music_Level_Track04_Intro ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track04_Intro" );
                //AppState->AudioList[ AudioID_Music_Level_Track04_Loop ] = WAV_ReadFile( Platform->ReadFile, PermMemory, TempMemory, MUSIC_SAVE_DIRECTORY, "Music_Level_Track04_Loop" );
                
                //AppState->AudioList[ AudioID_SpitBonus_Red ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "SpitBonus_Red" );
                //AppState->AudioList[ AudioID_SpitBonus_Yellow ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "SpitBonus_Yellow" );
                //AppState->AudioList[ AudioID_SpitBonus_Blue ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "SpitBonus_Blue" );
                
                //AppState->AudioList[ AudioID_Button_Yes ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Button_Yes" );
                //AppState->AudioList[ AudioID_Button_No ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Button_No" );
                
                //AppState->AudioList[ AudioID_Bullet_Slow ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Bullet_Slow" );
                //AppState->AudioList[ AudioID_Bullet_Fast ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Bullet_Fast" );
                
                //AppState->AudioList[ AudioID_Asteroid_Collide_Large_01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Asteroid_Collide_Large_01" );
                //AppState->AudioList[ AudioID_Asteroid_Collide_Large_02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Asteroid_Collide_Large_02" );
                
                //AppState->AudioList[ AudioID_Asteroid_Collide_Small_01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Asteroid_Collide_Small_01" );
                //AppState->AudioList[ AudioID_Asteroid_Collide_Small_02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Asteroid_Collide_Small_02" );
                
                //AppState->AudioList[ AudioID_Miner_Laser ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Miner_Laser" );
                
                //AppState->AudioList[ AudioID_Entity_Spawn ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Entity_Spawn" );
                
                //AppState->AudioList[ AudioID_Entity_Death_Large ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Entity_Death_Large" );
                //AppState->AudioList[ AudioID_Entity_Death_Medium ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Entity_Death_Medium" );
                //AppState->AudioList[ AudioID_Entity_Death_Small ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Entity_Death_Small" );
                //AppState->AudioList[ AudioID_Entity_Death_Tiny ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Entity_Death_Tiny" );
                //AppState->AudioList[ AudioID_Worm_Death_Large ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Worm_Death_Large" );
                //AppState->AudioList[ AudioID_Worm_Death_Small ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Worm_Death_Small" );
                
                //AppState->AudioList[ AudioID_Powerup ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Powerup" );
                
                //AppState->AudioList[ AudioID_EnemyBullet_01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "EnemyBullet_01" );
                //AppState->AudioList[ AudioID_EnemyBullet_02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "EnemyBullet_02" );
                //AppState->AudioList[ AudioID_Fireball_01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Fireball_01" );
                //AppState->AudioList[ AudioID_Fireball_02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Fireball_02" );
                
                //AppState->AudioList[ AudioID_Player_Damage ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Player_Damage" );
                //AppState->AudioList[ AudioID_Player_SecondaryDamage ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Player_SecondaryDamage" );
                
                //AppState->AudioList[ AudioID_ScoreMultiplier ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "ScoreMultiplier" );
            }
            
            vec2 App_Dim     = Vec2( Dev_Width, Dev_Height );
            vec2 App_HalfDim = App_Dim * 0.5f;
            rect App_Bound   = RectBLD( Vec2( 0.0f, 0.0f ), App_Dim );
            
            AppState->App_Dim     = App_Dim;
            AppState->App_HalfDim = App_HalfDim;
            AppState->App_Bound   = App_Bound;
            
            AppState->Mode = AppMode_TitleScreen;
            
#if ENABLE_EDITOR
            if( START_IN_EDITOR ) {
                AppState->Mode = AppMode_Editor;
            }
            
            { // Allocate Memory for Layout Editor Bitmaps
                // NOTE: Currently, Layout Bitmap size is dependant on WORLD_CAMERA_Y_UNITS, max 48. If a larger value is needed, then we probably want to rethink how layout bitmaps are stored, or change to a smaller format.
                
                int32 MaxCameraYUnits = 48;
                
                flo32 AspectRatio   = AppState->App_Dim.x / AppState->App_Dim.y;
                int32 Bitmap_Height = MaxCameraYUnits * ( int32 )LAYOUT_BITMAPS_PIXELS_PER_UNIT;
                int32 Bitmap_Width  = ( int32 )( ( flo32 )Bitmap_Height * AspectRatio );
                
                Bitmap_Height += ( 4 - ( Bitmap_Height % 4 ) );
                Bitmap_Width  += ( 4 - ( Bitmap_Width  % 4 ) );
                
                int32  Bitmap_nPixels = Bitmap_Width * Bitmap_Height;
                uint64 Memory_Size    = Bitmap_nPixels * sizeof( uint32 ) * WAVE_MAX_COUNT;
                
                EDITOR_STATE * Editor = &AppState->Editor;
                Editor->LayoutBitmap_Memory = SubArena( &Platform->PermMemory, Memory_Size );
            }
#endif
            
            Assert( WAVE_MAX_COUNT   <= TEXTURE_ID_MAX_COUNT );
            Assert( EntityType_Count <= ENTITY_TYPE_MAX_COUNT );
            
#if DEBUG_BUILD
            vec2 DebugSystem_BasePos = GetTL( App_Bound ) + Vec2( 10.0f, -14.0f );
            GlobalVar_DebugSystem.Pass_Game     = Pass_Game;
            GlobalVar_DebugSystem.Pass_UI       = Pass_UI;
            GlobalVar_DebugSystem.Font          = &Font;
            GlobalVar_DebugSystem.AdvanceHeight = Font.AdvanceHeight;
            GlobalVar_DebugSystem.ShowDebug     = true;
            Frame_Counter++;
#endif
            
            MOUSE_STATE      * Mouse      = &Platform->Mouse;
            KEYBOARD_STATE   * Keyboard   = &Platform->Keyboard;
            Mouse->BasePos = App_HalfDim;
            
            ShowCursor( true );
            
            //AppState->Collision_Memory   = SubArena( PermMemory, _MB( 1 ) );
            
#if START_IN_BLANK_LEVEL
            {
                int32 X = ( int32 )Mouse->BasePos.x;
                int32 Y = DisplaySettings.Height - 1 - ( int32 )Mouse->BasePos.y;
                SetCursorPos( X, Y );
                
                Mouse->Position = Mouse->BasePos;
                Mouse->ResetPos = true;
            }
#endif
            
            uint64 CycleCounter_Wrap = __rdtsc();
            while( Platform->IsRunning ) {
                {
                    flo64  Interest    = 0.01;
                    flo64  UpgradeCost = 1000.0;
                    flo64 bUpgradeCost = 1000.0;
                    int32 nTransaction = 6000;
                    
                    for( int32 Iter = 0; Iter < nTransaction; Iter++ ) {
                        UpgradeCost += ( bUpgradeCost * Interest );
                    }
                    
                    uint32 BreakHere = 10;
                }
                {
                    flo64  Interest    = 0.01;
                    flo64  UpgradeCost = 1000.0;
                    flo64 bUpgradeCost = 1000.0;
                    int32 nTransaction = 100;
                    
                    for( int32 Iter = 0; Iter < nTransaction; Iter++ ) {
                        UpgradeCost += ( bUpgradeCost * Interest );
                    }
                    
                    uint32 BreakHere = 10;
                }
                
                
                int64  PerfCounter_Start  = Win32_GetPerfCount();
                uint64 CycleCounter_Start = __rdtsc();
                
                uint64 CycleCounter_Prev = CycleCounter_Wrap;
                CycleCounter_Wrap = __rdtsc();
                uint64 CycleCounter_Temp = CycleCounter_Wrap - CycleCounter_Prev;
                //CONSOLE_VALUE( uint64, CycleCounter_Temp );
                
#if DEBUG_BUILD
                GlobalVar_DebugSystem.AtPos = GetBL( AppState->App_Bound ) + Vec2( 10.0f, 14.0f );
#endif
                
                Win32_processPendingMessages( Platform, window );
                if( global_restoreFocus ) {
                    boo32 ResetPos = Mouse->ResetPos;
                    vec2  BasePos  = Mouse->BasePos;
                    
                    global_restoreFocus = false;
                    memset( &Platform->Mouse,    0, sizeof( MOUSE_STATE    ) );
                    memset( &Platform->Keyboard, 0, sizeof( KEYBOARD_STATE ) );
                    
                    Mouse->ResetPos = ResetPos;
                    Mouse->BasePos  = BasePos;
                }
                
                ResetCommandList( Renderer );
                if( WasPressed( Keyboard, KeyCode_F4, ( KEYBOARD_FLAGS__ALT ) ) ) {
                    PostQuitMessage( 0 );
                }
                
                {
                    POINT Mouse_Position = {};
                    GetCursorPos( &Mouse_Position );
                    flo32 MouseX = ( flo32 )Mouse_Position.x;
                    flo32 MouseY = ( flo32 )( DisplaySettings.Height - 1 - Mouse_Position.y );
                    
                    vec2  Screen_Center = Vec2( ( flo32 )Screen_Width, ( flo32 )Screen_Height ) * 0.5f;
                    vec2  Dev_Center    = Vec2( ( flo32 )Dev_Width,    ( flo32 )Dev_Height    ) * 0.5f;
                    
                    vec2  V = Vec2( MouseX, MouseY ) - Screen_Center;
                    V.x *= ( ( flo32 )Dev_Width  / ( flo32 )Screen_Width  ) * Screen_xScale;
                    V.y *= ( ( flo32 )Dev_Height / ( flo32 )Screen_Height ) * Screen_yScale;
                    
                    Mouse->PrevPosition = Mouse->Position;
                    Mouse->Position     = Dev_Center + V;
                    Mouse->dPos         = Mouse->Position - Mouse->PrevPosition;
                }
                
                APP_UpdateAndRender( AppState, Platform );
                if( WasPressed( Keyboard, KeyCode_space ) ) {
                    int32 BreakHere = 10;
                }
                UpdateAudio( &Win32_Audio, AppState, AudioSystem );
                
                if( Mouse->DoHideCursor ) {
                    Mouse->DoHideCursor = false;
                    
                    // NOTE: Leaving this commented code here so I don't have to look it up again on msdn.
                    //CURSORINFO CursorInfo = {};
                    //CursorInfo.cbSize = sizeof( CURSORINFO );
                    //GetCursorInfo( &CursorInfo );
                    
                    int32 DisplayCount = 0;
                    do {
                        DisplayCount = ( int32 )ShowCursor( false );
                    } while( DisplayCount > -1 );
                    
                    //CURSORINFO CursorInfo = {};
                    //CursorInfo.cbSize = sizeof( CURSORINFO );
                    //GetCursorInfo( &CursorInfo );
                    
                    //uint32 BreakHere = 10;
                }
                if( Mouse->DoShowCursor ) {
                    Mouse->DoShowCursor = false;
                    
                    int32 DisplayCount = 0;
                    do {
                        DisplayCount = ( int32 )ShowCursor( true );
                    } while( DisplayCount < 0 );
                }
                
                //DISPLAY_VALUE( boo32, Mouse->ResetPos );
                //DISPLAY_VALUE( vec2,  Mouse->BasePos  );
                
                if( Mouse->ResetPos ) {
                    vec2  Screen_Center = Vec2( ( flo32 )Screen_Width, ( flo32 )Screen_Height ) * 0.5f;
                    vec2  Dev_Center    = Vec2( ( flo32 )Dev_Width,    ( flo32 )Dev_Height    ) * 0.5f;
                    
                    vec2 V = Dev_Center - Mouse->BasePos;
                    V.x *= ( ( flo32 )Screen_Width  / ( flo32 )Dev_Width  ) / Screen_xScale;
                    V.y *= ( ( flo32 )Screen_Height / ( flo32 )Dev_Height ) / Screen_yScale;
                    
                    flo32 MouseX = Screen_Center.x + V.x;
                    flo32 MouseY = Screen_Center.y + V.y;
                    
                    int32 X = ( int32 )MouseX;
                    int32 Y = DisplaySettings.Height - 1 - ( int32 )MouseY;
                    SetCursorPos( X, Y );
                    
                    Mouse->Position = Mouse->BasePos;
                }
                
#if DEBUG_BUILD
                if( WasPressed( Keyboard, KeyCode_d, KEYBOARD_FLAGS__CONTROL ) ) {
                    GlobalVar_DebugSystem.ShowDebug = !GlobalVar_DebugSystem.ShowDebug;
                }
                if( GlobalVar_DebugSystem.ErrorOccurred ) {
                    DrawString_noErrorCheck( GlobalVar_DebugSystem.Pass_UI, GlobalVar_DebugSystem.Font, "ERROR OCCURRED!! Check console for details!", GlobalVar_DebugSystem.AtPos, Vec2( 1.0f, 1.0f ), COLOR_RED );
                    GlobalVar_DebugSystem.AtPos.y += GlobalVar_DebugSystem.AdvanceHeight;
                }
#endif
                
                uint64 CycleCounter_App = __rdtsc();
                
                ID3D12GraphicsCommandList * CommandList = Renderer->CommandList;
                
                ID3D12Resource * BackBuffer = Renderer->BackBuffers[ Renderer->currentBackBufferIndex ];
                D3D12_CPU_DESCRIPTOR_HANDLE RTV = CD3DX12_CPU_DESCRIPTOR_HANDLE( Renderer->RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), Renderer->currentBackBufferIndex, Renderer->RTVDescriptorSize );
                D3D12_CPU_DESCRIPTOR_HANDLE DSV = Renderer->DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                
                D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( BackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET );
                CommandList->ResourceBarrier( 1, &barrier );
                
                flo32 ClearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
                CommandList->ClearRenderTargetView( RTV, ClearColor, 0, 0 );
                CommandList->ClearDepthStencilView( DSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0 );
                
                CommandList->SetPipelineState        ( Renderer->PSS_tri_depth_noBlend );
                CommandList->SetGraphicsRootSignature( Renderer->rootSignature );
                
                CommandList->SetDescriptorHeaps( 1, &Renderer->SRVHeap );
                
                {
                    flo32 Width  = ( flo32 )DisplaySettings.Width;
                    flo32 Height = ( flo32 )DisplaySettings.Height;
                    
                    rect Scissor_Bound = Rect( 0.0f, 0.0f, Width, Height );
                    vec2 Scissor_Dim   = GetDim( Scissor_Bound );
                    
                    Scissor_Dim.x /= Screen_xScale;
                    Scissor_Dim.y /= Screen_yScale;
                    
                    Scissor_Bound = RectCD( GetCenter( Scissor_Bound ), Scissor_Dim );
                    
                    //Width  = ( flo32 )Screen_Width;
                    //Height = ( flo32 )Screen_Height;
                    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, Width, Height, 0.0f, 1.0f };
                    //D3D12_RECT     scissor  = { 0, 0, ( LONG )Width, ( LONG )Height };
                    D3D12_RECT     scissor  = { ( LONG )Scissor_Bound.Left, ( LONG )Scissor_Bound.Bottom, ( LONG )Scissor_Bound.Right, ( LONG )Scissor_Bound.Top };
                    
                    CommandList->RSSetViewports   ( 1, &viewport );
                    CommandList->RSSetScissorRects( 1, &scissor  );
                }
                
                CommandList->OMSetRenderTargets( 1, &RTV, 0, &DSV );
                
                mat4 ui_transform   = mat4_orthographic( App_Bound );
                
                CAMERA_STATE * Camera = &AppState->Camera;
#if ENABLE_EDITOR
                if( AppState->Mode == AppMode_Editor ) {
                    if( AppState->Editor.ShowLayout ) {
                        Camera = &AppState->Editor.Camera_LayoutEditor;
                    } else {
                        Camera = &AppState->Editor.Camera_WaveEditor;
                    }
                }
#endif
                
                rect Camera_Bound   = RectCD( Camera->Pos, Camera->Dim );
                mat4 game_transform = mat4_orthographic( Camera_Bound );
                // NOTE : UI_Dim is not really set here. It's set in the init setup in App_UpdateAndRender(), which might not be the best place for it.
                mat4 Cow_Transform  = mat4_perspective( AppState->UI_Dim, PI * 0.333333f, 0.001f, 1000.0f );
                mat4 Cow_Overworld_Transform = mat4_orthographic( Camera_Bound, -100.0f );
                
                mat4 Screen_Transform = mat4_Scale( Screen_xScale, 1.0f / Screen_yScale, 1.0f );
                
                //game_transform = Screen_Transform * game_transform;
                //ui_transform   = Screen_Transform * ui_transform;
                
                Renderer->Pass_Game.CameraTransform_Tri  = Screen_Transform * game_transform;
                Renderer->Pass_Game.CameraTransform_Line = Screen_Transform * game_transform;
                Renderer->Pass_Game.CameraTransform_Text = Screen_Transform * ui_transform;
                Renderer->Pass_UI.CameraTransform_Tri    = Screen_Transform * ui_transform;
                Renderer->Pass_UI.CameraTransform_Line   = Screen_Transform * ui_transform;
                Renderer->Pass_UI.CameraTransform_Text   = Screen_Transform * ui_transform;
                Renderer->Pass_Cow_TitleScreen.CameraTransform_Tri   = Screen_Transform * Cow_Transform;
                Renderer->Pass_Cow_TitleScreen.CameraTransform_Line  = Screen_Transform * Cow_Transform;
                Renderer->Pass_Cow_TitleScreen.CameraTransform_Text  = Screen_Transform * Cow_Transform;
                Renderer->Pass_Cow_Overworld.CameraTransform_Tri     = Screen_Transform * Cow_Overworld_Transform;
                Renderer->Pass_Cow_Overworld.CameraTransform_Line    = Screen_Transform * Cow_Overworld_Transform;
                Renderer->Pass_Cow_Overworld.CameraTransform_Text    = Screen_Transform * Cow_Overworld_Transform;
                Renderer->Pass_FadeOut.CameraTransform_Tri     = mat4_Identity();
                Renderer->Pass_FadeOut.CameraTransform_Line    = mat4_Identity();
                Renderer->Pass_FadeOut.CameraTransform_Text    = mat4_Identity();
                
                {
                    flo32 Alph  = 1.0f - AppState->FadeOut_Alpha;
                    vec4  Color = Vec4( AppState->FadeOut_RGB * Alph, Alph );
                    
                    DrawRect( &Renderer->Pass_FadeOut, RectCD( Vec2( 0.0f, 0.0f ), Vec2( 2.0f, 2.0f ) ), Color );
                }
                
                RenderPass( Renderer, Pass_Game                         );
                RenderPass( Renderer, Pass_Cow_TitleScreen,        true );
                RenderPass( Renderer, Pass_UI                           );
                RenderPass( Renderer, Pass_Cow_Overworld,   true,  true );
                RenderPass( Renderer, Pass_FadeOut,         false, true );
                
                
                Renderer->textBuffer.nVertex = 0;
                
#if DEBUG_BUILD
                int64  PerfCounter_End  = Win32_GetPerfCount();
                uint64 CycleCounter_End = __rdtsc();
                
                char   PerfString[ 128 ] = {};
                uint64 CycleElapsed_App  = CycleCounter_App - CycleCounter_Start;
                { // PROFILE MAIN LOOP
                    flo32  MSElapsed = Win32_GetMSElapsed( PerfCounter_Start, PerfCounter_End, PerfFrequency.QuadPart );
                    uint64 CycleElapsed = CycleCounter_End - CycleCounter_Start;
                    
                    sprintf( PerfString, "FRAME PERF: %6.02fms, %10lluMC", MSElapsed, CycleElapsed );
                    DISPLAY_STRING( PerfString );
                    
                    flo32 Percent = ( flo32 )( CycleElapsed_App ) / ( flo32 )( CycleElapsed ) * 100.0f;
                    sprintf( PerfString, "FRAME PERF: APP: %10llu, %.02f%%, DRAW: %.02f%%", CycleElapsed_App, Percent, 100.0f - Percent );
                    DISPLAY_STRING( PerfString );
                }
                
                { // OTHER PROFILING
                    PROFILE_STATE * State = &AppState->Profile;
                    for( uint32 iProfileBlock = 0; iProfileBlock < State->nProfileBlock; iProfileBlock++ ) {
                        PROFILE_BLOCK Profile = State->ProfileBlock[ iProfileBlock ];
                        
                        uint64 CycleElapsed = Profile.CycleCounter_End - Profile.CycleCounter_Start;
                        flo32  Percent = ( flo32 )CycleElapsed / ( flo32 )CycleElapsed_App * 100.0f;
                        
                        sprintf( PerfString, "%s: %10llu, %.02f%%", Profile.Label, CycleElapsed, Percent );
                        DISPLAY_STRING( PerfString );
                    }
                    State->nProfileBlock = 0;
                }
#endif
                
                Present( Renderer );
            }
        }
    }
    
    return 0;
}