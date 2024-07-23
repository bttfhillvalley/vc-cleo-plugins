#include "text.h"
#include "CMenuManager.h"
#include "CFileMgr.h"
#include <iostream>

vector<map<string, string>> textFile;


string getLanguageFile(int language) {
	switch (language) {
	case LANGUAGE_ENGLISH:
		return "english.fxt";
	case LANGUAGE_FRENCH:
		return "french.fxt";
	case LANGUAGE_GERMAN:
		return "german.fxt";
	case LANGUAGE_ITALIAN:
		return "italian.fxt";
	case LANGUAGE_SPANISH:
		return "spanish.fxt";
	default:
		return "";
	}
}

void LoadTextFiles() {
	int fd;
	char line[256];
	int start, end;
	string key;
	string text;
	size_t pos;
	string language;
	string lineConverted;
	CFileMgr::SetDir("TEXT\\BTTF");
	textFile.clear();

	for (int i = 0; i < 5; i++) {
		language = getLanguageFile(i);
		cout << "Loading " << language << endl;
		fd = CFileMgr::OpenFile(language.c_str(), "r");
		map<string, string> textMap;

		while (CFileMgr::ReadLine(fd, line, sizeof(line))) {

			for (start = 0; ; start++)
				if (line[start] > ' ' || line[start] == '\0' || line[start] == '\n')
					break;
			// find end of line
			for (end = start; ; end++) {
				if (line[end] == '\0' || line[end] == '\n')
					break;
				if (line[end] == '=' || line[end] == '\r')
					line[end] = ' ';
			}
			line[end] = '\0';

			// empty line
			if (line[start] == '#' || line[start] == '\0')
				continue;

			lineConverted = string(line);

			pos = lineConverted.find(' ');
			if (pos == string::npos)
				continue;
			key = lineConverted.substr(0, pos);
			text = lineConverted.substr(pos + 1);
			textMap[key] = text;

		}
		textFile.push_back(textMap);
	}

	for (size_t n = 0; n < textFile.size(); n++) {
		cout << getLanguageFile(n) << endl;
		for (const auto& m : textFile.at(n)) {
			cout << m.first << ": " << m.second << endl;
		}
	}
}