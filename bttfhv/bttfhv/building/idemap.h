#pragma once
#include <map>
#include <set>
#include <string>
#include "plugin.h"

using  namespace std;

extern map<string, set<int>> ideMap;
extern map<string, set<int>*> removeObjectQueue;

set<int>* getModels(char* path);