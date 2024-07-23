#pragma once
#include "plugin.h"
#include <map>
#include <vector>

using namespace std;

enum Language {
	LANGUAGE_ENGLISH,
	LANGUAGE_FRENCH,
	LANGUAGE_GERMAN,
	LANGUAGE_ITALIAN,
	LANGUAGE_SPANISH,
};

void LoadTextFiles();

extern vector<map<string, string>> textFile;