#include "opcodes.h"
#include "CMenuManager.h"
#include "CFileMgr.h"
#include "CMessages.h"
#include "CHud.h"
#include "../utils/text.h"
#include <map>
#include <vector>
#include <iostream>

eOpcodeResult __stdcall textBox(CScript* script) {
	script->Collect(1);
	int language = FrontendMenuManager.m_nPrefsLanguage;
	try {
		string text = textFile.at(language).at(Params[0].cVar);
		CHud::SetHelpMessage(text.c_str(), false, false);
	}
	catch (out_of_range& e) {
		// do nothing
	}
	return OR_CONTINUE;
}