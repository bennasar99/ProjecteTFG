#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>
#include <mutex>
#include <unordered_map>

using namespace std;

#define NMODELS 6
class Mesh;

//Gestió dels fils
class ModelManager {
private:

	static unordered_map<string, Mesh*> model;

	ModelManager() {}

public:

	static void addModel(string mod, int frames, string path);
	static void drawModel(string mod, int frame);
	static void initialize();

};

