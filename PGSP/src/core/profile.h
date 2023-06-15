#pragma once

#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

struct TotalDuration {
	string message;
	steady_clock::time_point start;

	explicit TotalDuration(const string& msg);
	~TotalDuration();
};
