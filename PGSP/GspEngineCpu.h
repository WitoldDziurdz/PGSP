#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include "utils.h"
#include "IEngine.h"
#include <iostream>

namespace gsp {

	class GspEngineCpu : public IEngine{

	public:
		GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support);

		void calculate();
	};
}