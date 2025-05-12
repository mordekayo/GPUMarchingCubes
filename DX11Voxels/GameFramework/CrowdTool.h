#pragma once
#include "Exports.h"
#include <DetourCrowd.h>
#include <DetourNavMesh.h>

class NavigationSystem;
class dtCrowd;
class dtNavMesh;

struct CrowdToolParams
{
	bool m_expandSelectedDebugDraw;
	bool m_showCorners;
	bool m_showCollisionSegments;
	bool m_showPath;
	bool m_showVO;
	bool m_showOpt;
	bool m_showNeis;

	bool m_expandDebugDraw;
	bool m_showLabels;
	bool m_showGrid;
	bool m_showNodes;
	bool m_showPerfGraph;
	bool m_showDetailAll;

	bool m_expandOptions;
	bool m_anticipateTurns;
	bool m_optimizeVis;
	bool m_optimizeTopo;
	bool m_obstacleAvoidance;
	float m_obstacleAvoidanceType;
	bool m_separation;
	float m_separationWeight;
};

class GAMEFRAMEWORK_API CrowdToolState
{
	NavigationSystem* navSys;
	dtNavMesh* m_nav;
	dtCrowd* m_crowd;

	float m_targetPos[3];
	dtPolyRef m_targetRef;

	dtCrowdAgentDebugInfo m_agentDebug;
	dtObstacleAvoidanceDebugData* m_vod;

	static const int AGENT_MAX_TRAIL = 64;
	static const int MAX_AGENTS = 128;
	struct AgentTrail
	{
		float trail[AGENT_MAX_TRAIL * 3];
		int htrail;
	};
	AgentTrail m_trails[MAX_AGENTS];

	//ValueHistory m_crowdTotalTime;
	//ValueHistory m_crowdSampleCount;

	CrowdToolParams m_toolParams;

	bool m_run;

public:
	CrowdToolState();
	virtual ~CrowdToolState();

	virtual void init(NavigationSystem* inNavSys);
	virtual void reset();
	virtual void handleRender();
	virtual void handleRenderOverlay(double* proj, double* model, int* view);
	virtual void handleUpdate(const float dt);

	inline bool isRunning() const { return m_run; }
	inline void setRunning(const bool s) { m_run = s; }

	void addAgent(const float* pos);
	void removeAgent(const int idx);
	void hilightAgent(const int idx);
	void updateAgentParams();
	int hitTestAgents(const float* s, const float* p);
	void setMoveTarget(const float* p, bool adjust);
	void updateTick(const float dt);

	inline CrowdToolParams* getToolParams() { return &m_toolParams; }

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	CrowdToolState(const CrowdToolState&);
	CrowdToolState& operator=(const CrowdToolState&);
};

class GAMEFRAMEWORK_API CrowdTool
{
	NavigationSystem* navSys;
	CrowdToolState* m_state;

	enum ToolMode : int
	{
		TOOLMODE_CREATE = 1 << 0,
		TOOLMODE_MOVE_TARGET = 1 << 1,
		TOOLMODE_SELECT = 1 << 2,
		TOOLMODE_TOGGLE_POLYS = 1 << 3,
	};
	ToolMode m_mode;

public:
	CrowdTool();

	void init(NavigationSystem* inNavSys);
	void reset();
	void handleMenu();
	void handleClick(const float* s, const float* p, bool shift);
	void handleToggle();
	void handleStep();
	void handleUpdate(const float dt);
	void handleRender();
	void handleRenderOverlay(double* proj, double* model, int* view);
};

