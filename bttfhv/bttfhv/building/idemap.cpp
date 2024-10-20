#include "idemap.h"
#include <fstream>

map<string, set<int>> ideMap;
map<string, set<int>*> removeObjectQueue;

using namespace plugin;

set<int>* getModels(char* path) {
	string key(path);
	if (!ideMap.contains(key)) {
		ifstream in;
		char fullpath[128];
		char idepath[128];
		snprintf(fullpath, 128, ".\\bttfhv\\data\\%s.dat", path);
		config_file config(fullpath);
		for (auto param : config.paramLines) {
			// Only look up IDEs
			if (param.name == "IDE") {
				strncpy(idepath, param.asString().c_str(), 128);
				in.open(GAME_PATH(idepath));
				for (std::string line; getline(in, line); ) {
					// Find actual entries
					int end = line.find(",");
					if (end != std::string::npos) {
						ideMap[key].emplace(stoi(line.substr(0, end)));
					}
				}
				in.close();
			}
		}
	}
	return &ideMap[key];
}