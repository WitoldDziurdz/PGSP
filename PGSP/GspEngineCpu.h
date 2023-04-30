#pragma once
#include <string>
#include <vector>

#include "utils.h"
#include "IEngine.h"

namespace gsp {

	class GspEngineCpu : public IEngine{

	public:
		GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support);

		void calculate() override;
	};
}