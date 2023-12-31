//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#ifndef VGUI_KEY_TRANSLATION_H
#define VGUI_KEY_TRANSLATION_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/KeyCode.h>

// Convert from Windows scan codes to VGUI key codes.
vgui2::KeyCode KeyCode_VirtualKeyToVGUI( int key );
int			  KeyCode_VGUIToVirtualKey( vgui2::KeyCode keycode );
vgui2::KeyCode KeyCode_EngineKeyToVGUI(int key);


#endif // VGUI_KEY_TRANSLATION_H
