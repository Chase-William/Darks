#ifndef DARKS_IO_HOTKEY_H_
#define DARKS_IO_HOTKEY_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "VirtualInput.h"

struct HotKey
{
public:
	HotKey(Key key, Modifiers mods = Modifiers::None);

	Key key_;
	Modifiers mods_;
	int id_;
	// Key property
	/*class {
		Key value;
	public:
		Key& operator = (const Key& v) { return value = v; }
		operator Key () const { return value; }
	} Key;*/

	// Modifiers property
	/*class {
		Modifiers value;
	public:
		Modifiers& operator = (const Modifiers& v) { return value = v; }
		operator Modifiers () const { return value; }
	} Modifiers;*/
};

#endif