/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Filename    : OMOUSEGE.CPP
//Description : Mouse handling Object GERMAN keyboard

#if(defined(GERMAN))

#include <ALL.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OPOWER.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <KEY.H>
#include <OVGALOCK.H>

// define the graphic key scan code
// if not defined, use RMENU
//#define CDIK_GRAPH 0x68
//static unsigned vk_graph = 0;		// virtual key code for Graph key


//--------- Begin of Mouse::init_keyboard ----------//
//
void Mouse::init_keyboard()
{
#ifdef CDIK_GRAPH
	vk_graph = MapVirtualKey(CDIK_GRAPH, 3 );		// convert scan code to virtual key code
	if( !vk_graph )
		vk_graph = MapVirtualKey(CDIK_GRAPH, 1 );		// if 3 fails, try 1
#endif

	update_skey_state();
}
//--------- End of Mouse::init_keyboard ----------//


//--------- Begin of Mouse::update_skey_state ----------//
// called after task switch to get the lastest state of ctrl/alt/shift key
void Mouse::update_skey_state()
{
	// ------- get initial keyboard state ----------//
	skey_state = 0;
	if(GetKeyState(VK_LSHIFT) < 0)
		skey_state |= LEFT_SHIFT_KEY_MASK;
	if(GetKeyState(VK_RSHIFT) < 0)
		skey_state |= RIGHT_SHIFT_KEY_MASK;
	if(GetKeyState(VK_LCONTROL) < 0)
		skey_state |= LEFT_CONTROL_KEY_MASK;
	if(GetKeyState(VK_RCONTROL) < 0)
		skey_state |= RIGHT_CONTROL_KEY_MASK;
	if(GetKeyState(VK_LMENU) < 0)
		skey_state |= LEFT_ALT_KEY_MASK;
#ifdef CDIK_GRAPH
	if(GetKeyState(VK_RMENU) < 0)
		skey_state |= RIGHT_ALT_KEY_MASK;
#else
	if(GetKeyState(VK_RMENU) < 0)
		skey_state |= GRAPH_KEY_MASK;
#endif
	if(GetKeyState(VK_NUMLOCK) & 1)
		skey_state |= NUM_LOCK_STATE_MASK;
	if(GetKeyState(VK_CAPITAL) & 1)
		skey_state |= CAP_LOCK_STATE_MASK;
	if(GetKeyState(VK_SCROLL) & 1)
		skey_state |= SCROLL_LOCK_STATE_MASK;
	skey_state |= INSERT_STATE_MASK;		// enable insert mode by default

#ifdef CDIK_GRAPH
	if( vk_graph )
	{
		if(GetKeyState(vk_graph) < 0)
			skey_state |= GRAPH_KEY_MASK;		
	}
#endif

	// -------- get initial arrow key state ----------//

	arrow_key_state = 0;
	if( GetKeyState(VK_LEFT) < 0 )
		arrow_key_state |= ARROW_LEFT_KEY_MASK;
	if( GetKeyState(VK_RIGHT) < 0 )
		arrow_key_state |= ARROW_RIGHT_KEY_MASK;
	if( GetKeyState(VK_UP) < 0 )
		arrow_key_state |= ARROW_UP_KEY_MASK;
	if( GetKeyState(VK_DOWN) < 0 )
		arrow_key_state |= ARROW_DOWN_KEY_MASK;
}
//--------- End of Mouse::update_skey_state ----------//


int Mouse::process_key_message( DWORD keyOfs, DWORD keyData, DWORD keyTimeStamp )
{
	int rc = 0;

	// for english to emulate <alt graph> key
	//if( keybMsg->dwOfs == DIK_RMENU )
	//	keybMsg->dwOfs = CDIK_GRAPH;

	switch( keyOfs )
	{
	case DIK_LSHIFT:
		if( keyData & 0x80 )
			skey_state |= LEFT_SHIFT_KEY_MASK;
		else
			skey_state &= ~LEFT_SHIFT_KEY_MASK;
		break;
	case DIK_RSHIFT:
		if( keyData & 0x80 )
			skey_state |= RIGHT_SHIFT_KEY_MASK;
		else
			skey_state &= ~RIGHT_SHIFT_KEY_MASK;
		break;
	case DIK_LCONTROL:
		if( keyData & 0x80 )
			skey_state |= LEFT_CONTROL_KEY_MASK;
		else
			skey_state &= ~LEFT_CONTROL_KEY_MASK;
		break;
	case DIK_RCONTROL:
		if( keyData & 0x80 )
			skey_state |= RIGHT_CONTROL_KEY_MASK;
		else
			skey_state &= ~RIGHT_CONTROL_KEY_MASK;
		break;
	case DIK_LMENU:
		if( keyData & 0x80 )
			skey_state |= LEFT_ALT_KEY_MASK;
		else
			skey_state &= ~LEFT_ALT_KEY_MASK;
		break;
	case DIK_RMENU:
#ifdef CDIK_GRAPH
		if( keyData & 0x80 )
			skey_state |= RIGHT_ALT_KEY_MASK;
		else
			skey_state &= ~RIGHT_ALT_KEY_MASK;
#else
		if( keyData & 0x80 )
			skey_state |= GRAPH_KEY_MASK;
		else
			skey_state &= ~GRAPH_KEY_MASK;
#endif
		break;
	case DIK_CAPITAL:
		if(keyData & 0x80)
			skey_state ^= CAP_LOCK_STATE_MASK;
		break;
	case DIK_NUMLOCK:
		if(keyData & 0x80)
			skey_state ^= NUM_LOCK_STATE_MASK;
		break;
	case DIK_SCROLL:
		if(keyData & 0x80)
			skey_state ^= SCROLL_LOCK_STATE_MASK;
		break;
	case DIK_INSERT:
		if(keyData & 0x80)
		{
			// insert is a special case, it toggle skey_state and
			// append event queue
			skey_state ^= INSERT_STATE_MASK;
			add_key_event(keyOfs, keyTimeStamp);
			rc = 1;
		}
		break;
#ifdef CDIK_GRAPH
	case CDIK_GRAPH:		// graphic key
		if( keyData & 0x80 )
			skey_state |= GRAPH_KEY_MASK;
		else
			skey_state &= ~GRAPH_KEY_MASK;
		break;
#endif
	default:
		if( keyData & 0x80 )
		{
			add_key_event(keyOfs, keyTimeStamp);
			rc = 1;
			// capture screen
			if(keyOfs == DIK_F11 && (skey_state & CONTROL_KEY_MASK))
			{
				sys.capture_screen();
			}
		}
		else
		{
			add_key_release_event(keyOfs, keyTimeStamp);
			rc = 1;
		}
	}

	// -------- update arrow_key_state ---------//

	switch( keyOfs )
	{
	case DIK_LEFT:
		if( keyData & 0x80 )
			arrow_key_state |= ARROW_LEFT_KEY_MASK;
		else
			arrow_key_state &= ~ARROW_LEFT_KEY_MASK;
		break;
	case DIK_RIGHT:
		if( keyData & 0x80 )
			arrow_key_state |= ARROW_RIGHT_KEY_MASK;
		else
			arrow_key_state &= ~ARROW_RIGHT_KEY_MASK;
		break;
	case DIK_UP:
		if( keyData & 0x80 )
			arrow_key_state |= ARROW_UP_KEY_MASK;
		else
			arrow_key_state &= ~ARROW_UP_KEY_MASK;
		break;
	case DIK_DOWN:
		if( keyData & 0x80 )
			arrow_key_state |= ARROW_DOWN_KEY_MASK;
		else
			arrow_key_state &= ~ARROW_DOWN_KEY_MASK;
		break;
	}

	return rc;
}



// ------ Begin of Mouse::is_key -------//
// compare key with key code
// e.g. to test a key is alt-a,
// call mouse.is_key(mouse.scan_code, mouse.event_skey_state, 'a', K_CHAR_KEY | K_IS_ALT)
//
// pass 0 as charValue to disable checking in charValue
// e.g pressed key is 'a'
// mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_CHAR_KEY) returns 'a'
// but if key pressed is alt-a
// the same function call returns 0
// use mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_CHAR_KEY | K_IS_ALT ) instead
//
int Mouse::is_key(unsigned scanCode, unsigned short skeyState, unsigned short charValue, unsigned flags)
{
	unsigned short priChar = 0, shiftChar = 0, capitalChar = 0, altChar = 0;
	unsigned onNumPad = 0;

	switch(scanCode)
	{
	case DIK_ESCAPE: priChar = shiftChar = capitalChar = KEY_ESC; break;
	case DIK_1: priChar = capitalChar = '1'; shiftChar = '!'; break;
	case DIK_2: priChar = capitalChar = '2'; shiftChar = '\"'; altChar = (UCHAR)'²'; break;
	case DIK_3: priChar = capitalChar = '3'; shiftChar = (UCHAR)'§'; altChar = (UCHAR)'³'; break;
	case DIK_4: priChar = capitalChar = '4'; shiftChar = '$'; break;
	case DIK_5: priChar = capitalChar = '5'; shiftChar = '%'; break;
	case DIK_6: priChar = capitalChar = '6'; shiftChar = '&'; break;
	case DIK_7: priChar = capitalChar = '7'; shiftChar = '/'; altChar = '{'; break;
	case DIK_8: priChar = capitalChar = '8'; shiftChar = '('; altChar = '['; break;
	case DIK_9: priChar = capitalChar = '9'; shiftChar = ')'; altChar = ']'; break;
	case DIK_0: priChar = capitalChar = '0'; shiftChar = '='; altChar = '}'; break;
//	case DIK_MINUS: priChar = capitalChar = '-'; shiftChar = '_'; break;
	case 0x0c:  priChar = capitalChar = (UCHAR)'ß'; shiftChar = '\?'; altChar = '\\'; break;
//	case DIK_EQUALS: priChar = capitalChar = '='; shiftChar = '+'; break;
	case 0x0d:  priChar = capitalChar = (UCHAR)'´'; shiftChar = (UCHAR)'`'; break;
	case DIK_BACK: priChar = capitalChar = shiftChar = KEY_BACK_SPACE; break;   // backspace
	case DIK_TAB: priChar = capitalChar = shiftChar = KEY_TAB; break;
	case DIK_Q: priChar = 'q'; capitalChar = shiftChar = 'Q'; altChar = '@'; break;
	case DIK_W: priChar = 'w'; capitalChar = shiftChar = 'W'; break;
	case DIK_E: priChar = 'e'; capitalChar = shiftChar = 'E'; break;
	case DIK_R: priChar = 'r'; capitalChar = shiftChar = 'R'; break;
	case DIK_T: priChar = 't'; capitalChar = shiftChar = 'T'; break;
//	case DIK_Y: priChar = 'y'; capitalChar = shiftChar = 'Y'; break;
	case 0x15:  priChar = 'z'; capitalChar = shiftChar = 'Z'; break;
	case DIK_U: priChar = 'u'; capitalChar = shiftChar = 'U'; break;
	case DIK_I: priChar = 'i'; capitalChar = shiftChar = 'I'; break;
	case DIK_O: priChar = 'o'; capitalChar = shiftChar = 'O'; break;
	case DIK_P: priChar = 'p'; capitalChar = shiftChar = 'P'; break;
//	case DIK_LBRACKET: priChar = capitalChar = '['; shiftChar = '{'; break;
	case 0x1a:  priChar = (UCHAR)'ü'; capitalChar = shiftChar = (UCHAR)'Ü'; break;
//	case DIK_RBRACKET: priChar = capitalChar = ']'; shiftChar = '}'; break;
	case 0x1b:  priChar = capitalChar = shiftChar = '+'; altChar = '~'; break;
	case DIK_NUMPADENTER:		// Enter on numeric keypad
		onNumPad = 1;			// fall through
	case DIK_RETURN: priChar = capitalChar = shiftChar = KEY_RETURN;	break;
	case DIK_A: priChar = 'a'; capitalChar = shiftChar = 'A'; break;
	case DIK_S: priChar = 's'; capitalChar = shiftChar = 'S'; break;
	case DIK_D: priChar = 'd'; capitalChar = shiftChar = 'D'; break;
	case DIK_F: priChar = 'f'; capitalChar = shiftChar = 'F'; break;
	case DIK_G: priChar = 'g'; capitalChar = shiftChar = 'G'; break;
	case DIK_H: priChar = 'h'; capitalChar = shiftChar = 'H'; break;
	case DIK_J: priChar = 'j'; capitalChar = shiftChar = 'J'; break;
	case DIK_K: priChar = 'k'; capitalChar = shiftChar = 'K'; break;
	case DIK_L: priChar = 'l'; capitalChar = shiftChar = 'L'; break;
//	case DIK_SEMICOLON: priChar = capitalChar = ';'; shiftChar = ':'; break;
	case 0x27:  priChar = (UCHAR)'ö'; capitalChar = shiftChar = (UCHAR)'Ö'; break;
//	case DIK_APOSTROPHE: priChar = capitalChar = '\''; shiftChar = '\"'; break;
	case 0x28:  priChar = (UCHAR)'ä'; capitalChar = shiftChar = (UCHAR)'Ä'; break;
//	case DIK_GRAVE: priChar = capitalChar = '`'; shiftChar = '~'; break;
	case 0x29:  priChar = capitalChar = '^'; shiftChar = (UCHAR)'°'; break;
//	case DIK_BACKSLASH: priChar = capitalChar = '\\'; shiftChar = '|'; break;
	case 0x2b:  priChar = capitalChar = '#'; shiftChar = '\''; break;
//	case DIK_Z: priChar = 'z'; capitalChar = shiftChar = 'Z'; break;
	case 0x2c:  priChar = 'y'; capitalChar = shiftChar = 'Y'; break;
	case DIK_X: priChar = 'x'; capitalChar = shiftChar = 'X'; break;
	case DIK_C: priChar = 'c'; capitalChar = shiftChar = 'C'; break;
	case DIK_V: priChar = 'v'; capitalChar = shiftChar = 'V'; break;
	case DIK_B: priChar = 'b'; capitalChar = shiftChar = 'B'; break;
	case DIK_N: priChar = 'n'; capitalChar = shiftChar = 'N'; break;
	case DIK_M: priChar = 'm'; capitalChar = shiftChar = 'M'; altChar = (UCHAR)'µ'; break;
//	case DIK_COMMA: priChar = capitalChar = ','; shiftChar = '<'; break;
	case DIK_COMMA: priChar = capitalChar = ','; shiftChar = ';'; break;
//	case DIK_PERIOD: priChar = capitalChar = '.'; shiftChar = '>'; break;
	case DIK_PERIOD: priChar = capitalChar = '.'; shiftChar = ':'; break;
//	case DIK_SLASH: priChar = capitalChar = '/'; shiftChar = '\?'; break;
	case 0x35:  priChar = capitalChar = '-'; shiftChar = '_'; break;
	case DIK_MULTIPLY: priChar = capitalChar = shiftChar = '*'; onNumPad = 1; break; // * on numeric keypad
	case DIK_SPACE: priChar = capitalChar = shiftChar = ' '; break;
	case DIK_ADD: priChar = capitalChar = shiftChar = '+'; onNumPad = 1; break; // + on numeric keypad
	case DIK_DIVIDE: priChar = capitalChar = shiftChar = '/'; onNumPad = 1; break;		// / on numeric keypad
	case DIK_SUBTRACT: priChar = capitalChar = shiftChar = '-'; onNumPad = 1; break;	// - on numeric keypad
	case 0x56:  priChar = capitalChar = '<'; shiftChar = '>'; altChar = '|'; break;

	case DIK_NUMPAD7: priChar = shiftChar = capitalChar = '7'; onNumPad = 1; break;
	case DIK_NUMPAD8: priChar = shiftChar = capitalChar = '8'; onNumPad = 1; break;
	case DIK_NUMPAD9: priChar = shiftChar = capitalChar = '9'; onNumPad = 1; break;
	case DIK_NUMPAD4: priChar = shiftChar = capitalChar = '4'; onNumPad = 1; break;
	case DIK_NUMPAD5: priChar = shiftChar = capitalChar = '5'; onNumPad = 1; break;
	case DIK_NUMPAD6: priChar = shiftChar = capitalChar = '6'; onNumPad = 1; break;
	case DIK_NUMPAD1: priChar = shiftChar = capitalChar = '1'; onNumPad = 1; break;
	case DIK_NUMPAD2: priChar = shiftChar = capitalChar = '2'; onNumPad = 1; break;
	case DIK_NUMPAD3: priChar = shiftChar = capitalChar = '3'; onNumPad = 1; break;
	case DIK_NUMPAD0: priChar = shiftChar = capitalChar = '0'; onNumPad = 1; break;
	case DIK_DECIMAL: priChar = shiftChar = capitalChar = '.'; onNumPad = 1; break;

	// function keys
	case DIK_F1: priChar = shiftChar = capitalChar = KEY_F1; break;
	case DIK_F2: priChar = shiftChar = capitalChar = KEY_F2; break;
	case DIK_F3: priChar = shiftChar = capitalChar = KEY_F3; break;
	case DIK_F4: priChar = shiftChar = capitalChar = KEY_F4; break;
	case DIK_F5: priChar = shiftChar = capitalChar = KEY_F5; break;
	case DIK_F6: priChar = shiftChar = capitalChar = KEY_F6; break;
	case DIK_F7: priChar = shiftChar = capitalChar = KEY_F7; break;
	case DIK_F8: priChar = shiftChar = capitalChar = KEY_F8; break;
	case DIK_F9: priChar = shiftChar = capitalChar = KEY_F9; break;
	case DIK_F10: priChar = shiftChar = capitalChar = KEY_F10; break;
	case DIK_F11: priChar = shiftChar = capitalChar = KEY_F11; break;
	case DIK_F12: priChar = shiftChar = capitalChar = KEY_F12; break;

	// arrow keys
	case 0x67:		// fall through, German keyboard called "Pos 1"
	case DIK_HOME: priChar = shiftChar = capitalChar = KEY_HOME; break;
	case DIK_UP: priChar = shiftChar = capitalChar = KEY_UP; break;
	case DIK_PRIOR: priChar = shiftChar = capitalChar = KEY_PGUP; break;
	case DIK_LEFT: priChar = shiftChar = capitalChar = KEY_LEFT; break;
	case DIK_RIGHT: priChar = shiftChar = capitalChar = KEY_RIGHT; break;
	case DIK_END: priChar = shiftChar = capitalChar = KEY_END; break;
	case DIK_DOWN: priChar = shiftChar = capitalChar = KEY_DOWN; break;
	case DIK_NEXT: priChar = shiftChar = capitalChar = KEY_PGDN; break;
	case DIK_INSERT: priChar = shiftChar = capitalChar = KEY_INS; break;
	case DIK_DELETE: priChar = shiftChar = capitalChar = KEY_DEL; break;

	// other keys found in Japanese keyboard
	case DIK_NUMPADCOMMA: priChar = shiftChar = capitalChar = ','; break;
	case DIK_NUMPADEQUALS: priChar = shiftChar = capitalChar = '='; break;
	case DIK_AT: priChar = shiftChar = capitalChar = '@'; break;
	case DIK_COLON: priChar = shiftChar = capitalChar = ':'; break;
	case DIK_UNDERLINE: priChar = shiftChar = capitalChar = '_'; break;
	}

	// BUGHERE : numpad key is not translated when numlock is off

	// check flags
	int retFlag = 1;

	// check shift key state
	if( !(flags & K_IGNORE_SHIFT) )
	{
		if( flags & K_IS_SHIFT )
		{
			if( !(skeyState & SHIFT_KEY_MASK) )
				retFlag = 0;
		}
		else
		{
			if( skeyState & SHIFT_KEY_MASK )
				retFlag = 0;
		}
	}

	// check contrl key state
	if( !(flags & K_IGNORE_CTRL) )
	{
		if( flags & K_IS_CTRL )
		{
			if( !(skeyState & CONTROL_KEY_MASK) )
				retFlag = 0;
		}
		else
		{
			if( skeyState & CONTROL_KEY_MASK )
				retFlag = 0;
		}
	}

	// check alt key state
	if( !(flags & K_IGNORE_ALT) )
	{
		if( flags & K_IS_ALT )
		{
			if( !(skeyState & ALT_KEY_MASK) )
				retFlag = 0;
		}
		else
		{
			if( skeyState & ALT_KEY_MASK )
				retFlag = 0;
		}
	}

	// check numpad state
	if( !(flags & K_IGNORE_NUMPAD) )
	{
		if( flags & K_ON_NUMPAD )
		{
			if( !onNumPad )
				retFlag = 0;
		}
		else
		{
			if( onNumPad )
				retFlag = 0;
		}
	}

	unsigned outChar = priChar;
	if( flags & K_TRANSLATE_KEY ) 
	{
		if( (skeyState & GRAPH_KEY_MASK) && altChar )
		{
			outChar = altChar;
		}
		else
		{
			if( priChar == capitalChar )
			{
				// non-letter
				outChar = skeyState & SHIFT_KEY_MASK ? shiftChar : priChar;
			}
			else
			{
				// letter
				outChar = skeyState & CAP_LOCK_STATE_MASK ? 
					(skeyState & SHIFT_KEY_MASK ? priChar : capitalChar) :
					(skeyState & SHIFT_KEY_MASK ? shiftChar : priChar) ;
			}
		}
	}

	if(!retFlag)
		return 0;

	int retFlag2 = (charValue == 0) || outChar == charValue
		|| ((flags & K_IGNORE_SHIFT) && shiftChar == charValue)
		|| ((flags & K_IGNORE_CAP_LOCK) && capitalChar == charValue)
		|| ((flags & K_CASE_INSENSITIVE) && outChar == (unsigned short) tolower(charValue));

	if(retFlag2)
		return outChar;
	else
		return 0;
}
// ------ End of Mouse::is_key -------//


// ------ Begin of Mouse::is_key -------//
int Mouse::is_key(unsigned scanCode, unsigned short skeyState, char *keyStr, unsigned flags)
{
	int len = strlen(keyStr);
	if( len == 0)
		return 0;
	if( len == 1)
		return is_key(scanCode, skeyState, keyStr[0], flags);

	char *priChar = NULL;
	char *numLockChar = NULL;
	int onNumPad = 0;

	switch(scanCode)
	{
	case DIK_F1: numLockChar = priChar = "F1"; break;
	case DIK_F2: numLockChar = priChar = "F2"; break;
	case DIK_F3: numLockChar = priChar = "F3"; break;
	case DIK_F4: numLockChar = priChar = "F4"; break;
	case DIK_F5: numLockChar = priChar = "F5"; break;
	case DIK_F6: numLockChar = priChar = "F6"; break;
	case DIK_F7: numLockChar = priChar = "F7"; break;
	case DIK_F8: numLockChar = priChar = "F8"; break;
	case DIK_F9: numLockChar = priChar = "F9"; break;
	case DIK_F10: numLockChar = priChar = "F10"; break;
	case DIK_F11: numLockChar = priChar = "F11"; break;
	case DIK_F12: numLockChar = priChar = "F12"; break;

	case DIK_NUMPAD7: priChar = "HOME"; numLockChar = "7"; onNumPad = 1; break;
	case DIK_NUMPAD8: priChar = "UP"; numLockChar = "8"; onNumPad = 1; break;
	case DIK_NUMPAD9: priChar = "PAGE UP"; numLockChar = "9"; onNumPad = 1; break;
	case DIK_NUMPAD4: priChar = "LEFT"; numLockChar = "4"; onNumPad = 1; break;
	case DIK_NUMPAD5: priChar = "CENTER"; numLockChar = "5"; onNumPad = 1; break;
	case DIK_NUMPAD6: priChar = "RIGHT"; numLockChar = "6"; onNumPad = 1; break;
	case DIK_NUMPAD1: priChar = "END"; numLockChar = "1"; onNumPad = 1; break;
	case DIK_NUMPAD2: priChar = "DOWN"; numLockChar = "2"; onNumPad = 1; break;
	case DIK_NUMPAD3: priChar = "PAGE DOWN"; numLockChar = "3"; onNumPad = 1; break;
	case DIK_NUMPAD0: priChar = "INSERT"; numLockChar = "0"; onNumPad = 1; break;
	case DIK_DECIMAL: priChar = "DELETE"; numLockChar = "."; onNumPad = 1; break;

	// keys above arrow keys
	case DIK_HOME: priChar = numLockChar = "HOME"; break;
	case DIK_UP: priChar = numLockChar = "UP"; break;
	case DIK_PRIOR: priChar = numLockChar = "PAGE UP"; break;
	case DIK_LEFT: priChar = numLockChar = "LEFT"; break;
	case DIK_RIGHT: priChar = numLockChar = "RIGHT"; break;
	case DIK_END: priChar = numLockChar = "END"; break;
	case DIK_DOWN: priChar = numLockChar = "DOWN"; break;
	case DIK_NEXT: priChar = numLockChar = "PAGE DOWN"; break;
	case DIK_INSERT: priChar = numLockChar = "INSERT"; break;
	case DIK_DELETE: priChar = numLockChar = "DELETE"; break;

	// 104-key only
	case DIK_LWIN: priChar = numLockChar = "LEFT WINDOW"; break;
	case DIK_RWIN: priChar = numLockChar = "RIGHT WINDOW"; break;
	case DIK_APPS: priChar = numLockChar = "APP MENU"; break;
	}

	// check flags
	int retFlag = 1;

	// check shift key state
	if( !(flags & K_IGNORE_SHIFT) )
	{
		if( flags & K_IS_SHIFT )
		{
			if( !(skeyState & SHIFT_KEY_MASK) )
				retFlag = 0;
		}
		else
		{
			if( skeyState & SHIFT_KEY_MASK )
				retFlag = 0;
		}
	}

	// check contrl key state
	if( !(flags & K_IGNORE_CTRL) )
	{
		if( flags & K_IS_CTRL )
		{
			if( !(skeyState & CONTROL_KEY_MASK) )
				retFlag = 0;
		}
		else
		{
			if( skeyState & CONTROL_KEY_MASK )
				retFlag = 0;
		}
	}

	// check alt key state
	if( !(flags & K_IGNORE_ALT) )
	{
		if( flags & K_IS_ALT )
		{
			if( !(skeyState & ALT_KEY_MASK) )
				retFlag = 0;
		}
		else
		{
			if( skeyState & ALT_KEY_MASK )
				retFlag = 0;
		}
	}

	// check numpad state
	if( !(flags & K_IGNORE_NUMPAD) )
	{
		if( flags & K_ON_NUMPAD )
		{
			if( !onNumPad )
				retFlag = 0;
		}
		else
		{
			if( onNumPad )
				retFlag = 0;
		}
	}

	char *outChar = skeyState & NUM_LOCK_STATE_MASK ? numLockChar : priChar;
	int retFlag2 = outChar ? !strcmp(outChar, keyStr) : 0;

	return retFlag && retFlag2;
}
// ------ End of Mouse::is_key -------//

#endif