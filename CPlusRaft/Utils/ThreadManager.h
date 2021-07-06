#pragma once
#include <climits>
#include <vector>
#include <ctype.h>
#include <thread>
#include <algorithm>
#include <mutex>

//Gestió dels fils
class ThreadManager {
private:
	static int cores; //N de cores

	static std::mutex mutex;

	ThreadManager() {}

public:

	static int getCoreCount();
	static void initialize();
};

