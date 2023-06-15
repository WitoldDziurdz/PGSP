#pragma once
#include <string>
#include <vector>

#include "../core/utils.h"
#include "../IEngine.h"

namespace gsp {

	class GspEngineCpu : public IEngine{

	public:
		GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, bool info_logs = false, bool debug_logs = false);

		void calculate() override;
	};
}