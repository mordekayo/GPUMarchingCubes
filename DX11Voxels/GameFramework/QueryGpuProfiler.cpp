#include "Game.h"
#include "QueryGpuProfiler.h"

#include <iostream> 


void ZGame::QueryGpuProfiler::BeginFrame()
{
	curFrame = std::make_shared<Frame>();
	frames.push_back(curFrame);

	if (frames.front() != curFrame)
		curFrame->stamps.reserve(frames.front()->stamps.size());

	curFrame->frameIndex = Game::Instance->frameIndex;


	D3D11_QUERY_DESC qDesc{
		D3D11_QUERY_TIMESTAMP_DISJOINT,
		0
	};
	Game::Instance->Device->CreateQuery(&qDesc, curFrame->queryDisjoint.GetAddressOf());
	Game::Instance->Context->Begin(curFrame->queryDisjoint.Get());


	std::stringstream str;
	str << "StartFrame: " << curFrame->frameIndex;
	AddTimestamp(str.view());
}


void ZGame::QueryGpuProfiler::EndFrame()
{
	auto ctx = Game::Instance->Context;
	std::stringstream str;
	str << "EndFrame: " << curFrame->frameIndex;
	AddTimestamp(str.view());

	ctx->End(curFrame->queryDisjoint.Get());


	std::for_each(frames.cbegin(), frames.cend(), [&](std::shared_ptr<Frame> frame) {
		if (frame->queryDisjoint == nullptr)
			return;
		if(ctx->GetData(frame->queryDisjoint.Get(), nullptr, 0, 0) == S_FALSE)
			return;

		frame->frameIndexEnd = Game::Instance->frameIndex;

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
		ctx->GetData(frame->queryDisjoint.Get(), &tsDisjoint, sizeof(tsDisjoint), 0);
		frame->queryDisjoint = nullptr;

		if (tsDisjoint.Disjoint) { // Frame failed
			frame->isError = true;
			//std::cout << "Frame " << frame->frameIndex << " FAILED at " << frame->frameIndexEnd << "\n";
			std::ranges::for_each(frame->stamps, [](Timestamp& stamp) { stamp.timestampQuery = nullptr; });
			return;
		}

		//std::cout << "Frame " << frame->frameIndex << " finished at " << frame->frameIndexEnd << "\n";

		uint64_t frameStartData = 0;
		ctx->GetData(frame->stamps[0].timestampQuery.Get(), &frameStartData, sizeof(uint64_t), 0);
		frame->stamps[0].timestampQuery = nullptr;

		for (uint32_t ind = 1; ind < frame->stamps.size(); ++ind) {
			Timestamp& stamp = frame->stamps[ind];

			uint64_t data = 0;
			ctx->GetData(stamp.timestampQuery.Get(), &data, sizeof(uint64_t), 0);
			stamp.timestampQuery = nullptr;

			stamp.gpuTime = static_cast<double>(data - frameStartData) / static_cast<double>(tsDisjoint.Frequency) * 1000.0;

			//std::cout << "Stamp: " << stamp.stampName << " \tcpu time: " << stamp.cpuTime << "\tgpu time: " << stamp.gpuTime << "\tdiff time: " << stamp.gpuTime - frame->stamps[ind - 1].gpuTime << "\n";
		}
		//std::cout << "------------------------------------------------------------\n";
	});
}


void ZGame::QueryGpuProfiler::PrintStatistics()
{
	requestStatistic = true;
}


void ZGame::QueryGpuProfiler::AddTimestamp(std::string_view name) const
{
	if (curFrame == nullptr)
		return;

	Microsoft::WRL::ComPtr<ID3D11Query> timestampQuery;
	D3D11_QUERY_DESC qDesc{
		D3D11_QUERY_TIMESTAMP,
		0
	};
	Game::Instance->Device->CreateQuery(&qDesc, timestampQuery.GetAddressOf());
	Game::Instance->Context->End(timestampQuery.Get());

	curFrame->stamps.emplace_back(name.data(), timestampQuery, Game::Instance->TotalTime, 0);
}
