#include "opcodes.h"
#include "CMenuManager.h"
#include "CFileMgr.h"
#include "CMessages.h"
#include "CHud.h"
#include "CTheScripts.h"
#include "../utils/text.h"
#include <map>
#include <vector>
#include <iostream>

#define SCRIPT_TEXT_MAX_LENGTH 100

int format(CScript* script, char* str, size_t len, const char* format) {
	unsigned int written = 0;
	const char* iter = format;
	char bufa[256], fmtbufa[64], * fmta;

	while (*iter)
	{
		while (*iter && *iter != '%')
		{
			if (written++ >= len)
				return -1;
			*str++ = *iter++;
		}
		if (*iter == '%')
		{
			if (iter[1] == '%')
			{
				if (written++ >= len)
					return -1;
				*str++ = '%'; /* "%%"->'%' */
				iter += 2;
				continue;
			}

			//get flags and width specifier
			fmta = fmtbufa;
			*fmta++ = *iter++;
			while (*iter == '0' ||
				*iter == '+' ||
				*iter == '-' ||
				*iter == ' ' ||
				*iter == '*' ||
				*iter == '#')
			{
				if (*iter == '*')
				{
					char* buffiter = bufa;
					//get width
					script->Collect(1);
					_itoa(Params[0].nVar, buffiter, 10);
					while (*buffiter)
						*fmta++ = *buffiter++;
				}
				else
					*fmta++ = *iter;
				iter++;
			}

			//get immidiate width value
			while (isdigit(*iter))
				*fmta++ = *iter++;

			//get precision
			if (*iter == '.')
			{
				*fmta++ = *iter++;
				if (*iter == '*')
				{
					char* buffiter = bufa;
					script->Collect(1);
					_itoa(Params[0].nVar, buffiter, 10);
					while (*buffiter)
						*fmta++ = *buffiter++;
				}
				else
					while (isdigit(*iter))
						*fmta++ = *iter++;
			}
			//get size
			if (*iter == 'h' || *iter == 'l')
				*fmta++ = *iter++;

			switch (*iter)
			{
				case 's':
				{
					script->Collect(1);
					static const char none[] = "(null)";
					const char* astr = Params[0].cVar;
					const char* striter = astr ? astr : none;
					while (*striter)
					{
						if (written++ >= len)
							return -1;
						*str++ = *striter++;
					}
					iter++;
					break;
				}

				case 'c':
					if (written++ >= len)
						return -1;
					script->Collect(1);
					*str++ = (char)Params[0].nVar;
					iter++;
					break;

				default:
				{
					/* For non wc types, use system sprintf and append to wide char output */
					/* FIXME: for unrecognised types, should ignore % when printing */
					char* bufaiter = bufa;
					if (*iter == 'p' || *iter == 'P')
					{
						script->Collect(1);
						sprintf(bufaiter, "%08X", Params[0].nVar);
					}
					else
					{
						*fmta++ = *iter;
						*fmta = '\0';
						if (*iter == 'a' || *iter == 'A' ||
							*iter == 'e' || *iter == 'E' ||
							*iter == 'f' || *iter == 'F' ||
							*iter == 'g' || *iter == 'G')
						{
							script->Collect(1);
							sprintf(bufaiter, fmtbufa, Params[0].fVar);
						}
						else
						{
							script->Collect(1);
							sprintf(bufaiter, fmtbufa, Params[0].pVar);
						}
					}
					while (*bufaiter)
					{
						if (written++ >= len)
							return -1;
						*str++ = *bufaiter++;
					}
					iter++;
					break;
				}
			}
		}
	}
	if (written >= len)
		return -1;
	*str++ = 0;
	return (int)written;
}

void getText(CScript* script, char* key, wchar_t* message_buf) {
	int language = FrontendMenuManager.m_nPrefsLanguage;
	char fmt[MAX_PATH]; char text[MAX_PATH];
	try {
		strcpy(fmt, textFile.at(language).at(key).c_str());
		format(script, text, sizeof(text), fmt);
		swprintf(message_buf, MAX_PATH, L"%hs", text);
	}
	catch (out_of_range& e) {
		sprintf(text, "'%s' not found", key);
		swprintf(message_buf, MAX_PATH, L"%hs", text);
	}
}

eOpcodeResult __stdcall textBox(CScript* script) {
	script->Collect(1);
	static wchar_t message_buf[MAX_PATH];
	getText(script, Params[0].cVar, message_buf);
	CHud::SetHelpMessage(message_buf, false, false);
	script->m_dwIp++;
	return OR_CONTINUE;
}

eOpcodeResult __stdcall textStyled(CScript* script) {
	script->Collect(3);
	int language = FrontendMenuManager.m_nPrefsLanguage;
	static wchar_t message_buf[MAX_PATH];
	int time = Params[1].nVar;
	int style = Params[2].nVar;
	getText(script, Params[0].cVar, message_buf);
	CMessages::AddBigMessage(message_buf, time, style - 1);
	script->m_dwIp++;
	return OR_CONTINUE;
}

eOpcodeResult __stdcall textLowPriority(CScript* script) {
	script->Collect(2);
	int language = FrontendMenuManager.m_nPrefsLanguage;
	static wchar_t message_buf[MAX_PATH];
	int time = Params[1].nVar;
	getText(script, Params[0].cVar, message_buf);
	CMessages::AddMessage(message_buf, time, 1);
	script->m_dwIp++;
	return OR_CONTINUE;
}

eOpcodeResult __stdcall textHighPriority(CScript* script) {
	script->Collect(2);
	int language = FrontendMenuManager.m_nPrefsLanguage;
	static wchar_t message_buf[MAX_PATH];
	int time = Params[1].nVar;
	getText(script, Params[0].cVar, message_buf);
	CMessages::AddMessageJumpQ(message_buf, time, 1);
	script->m_dwIp++;
	return OR_CONTINUE;
}

eOpcodeResult __stdcall textDraw(CScript* script) {
	script->Collect(3);
	int language = FrontendMenuManager.m_nPrefsLanguage;
	static wchar_t message_buf[MAX_PATH];
	CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame].m_fX = Params[0].fVar;
	CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame].m_fY = Params[1].fVar;
	getText(script, Params[2].cVar, message_buf);
	uint16_t len = CMessages::GetWideStringLength(message_buf);
	for (uint16_t i = 0; i < len; i++)
		CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame].m_Text[i] = message_buf[i];
	for (uint16_t i = len; i < SCRIPT_TEXT_MAX_LENGTH; i++)
		CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame].m_Text[i] = 0;
	++CTheScripts::NumberOfIntroTextLinesThisFrame;
	script->m_dwIp++;
	return OR_CONTINUE;
}
