#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>
#include <mutex>

#define NMODELS 6
class Mesh;

enum class Model {
	OV_COS,
	OV_DVDR,
	OV_DVES,
	OV_DRDR,
	OV_DRES,
	ESTRUC
};

//Gestió dels fils
class ModelManager {
private:

	static Mesh *model[NMODELS];

	ModelManager() {}

public:

	static void addModel(Model mod, int frames, std::string path);
	static void drawModel(Model mod, int frame);
	static void initialize();

};

