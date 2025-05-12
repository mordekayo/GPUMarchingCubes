#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include "RingBuffer.h"

#include "Exports.h"


namespace ZGame {

	class GAMEFRAMEWORK_API QueryGpuProfiler
	{
		struct Timestamp
		{
			std::string stampName{};
			Microsoft::WRL::ComPtr<ID3D11Query> timestampQuery = nullptr;
			double cpuTime = 0;
			double gpuTime = 0;

			~Timestamp()
			{
				int t = 0;
			}
		};

		struct Frame
		{
			Microsoft::WRL::ComPtr <ID3D11Query> queryDisjoint = nullptr;
			Microsoft::WRL::ComPtr <ID3D11Query> statisticsQuery = nullptr;
			uint64_t frameIndex = 0;
			uint64_t frameIndexEnd = 0;
			bool isError = false;
			std::vector<Timestamp> stamps{};
		};

		std::shared_ptr<Frame> curFrame = nullptr;
		buffers::ring_buffer<std::shared_ptr<Frame>, 100, true> frames;
		bool requestStatistic = false;

	public:
		void BeginFrame();
		void EndFrame();

		void PrintStatistics();

		void AddTimestamp(std::string_view name) const;
	};

}
