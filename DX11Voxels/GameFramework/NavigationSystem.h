#pragma once
#include "Exports.h"

#include <memory>
#include <limits>

#include "ChunkyTriMesh.h"

#if 1

#include "DetourNavMesh.h"
#include <DetourCommon.h>
#include "DetourNavMeshQuery.h"
#include "DetourNavMeshBuilder.h"

#include "Recast.h"

#include "../GameFramework/Game.h"
#include "../ZMathLib/SimpleMath.h"
#include "../GameFramework/DebugRenderSystem.h"



struct GAMEFRAMEWORK_API GeometryInfo
{
	int VertexCount;
	int TrianglesCount;
	float* Verts;
	int* Triangles;

	DirectX::SimpleMath::Vector3 minBound;
	DirectX::SimpleMath::Vector3 maxBound;

	void CalculateBounds();;


	float* getVerts() { return Verts; }
	int getVertCount() { return VertexCount; }
	int* getTris() { return Triangles; }
	int getTriCount() { return TrianglesCount; }
};


static const int MAX_CONVEXVOL_PTS = 12;

struct ConvexVolume
{
	float verts[MAX_CONVEXVOL_PTS * 3];
	float hmin, hmax;
	int nverts;
	int area;
};


struct GAMEFRAMEWORK_API NavMeshInfo
{
	GeometryInfo* geometry;

	const float* offMeshConVerts;
	const float* offMeshConRad;
	const unsigned short* offMeshConFlags;
	const unsigned char* offMeshConAreas;
	const unsigned char* offMeshConDir;
	const unsigned int* offMeshConUserID;
	int offMeshConCount;

	rcChunkyTriMesh* m_chunkyMesh;

	bool Init(GeometryInfo* inGeom);

	GeometryInfo* getMesh()			{ return geometry; }
	float* getNavMeshBoundsMin()	{ return &geometry->minBound.x; }
	float* getNavMeshBoundsMax()	{ return &geometry->maxBound.x; }

	ConvexVolume* getConvexVolumes()	{ return nullptr; }
	int getConvexVolumeCount()			{ return 0; }


	const float* getOffMeshConnectionVerts()			{ return offMeshConVerts; }
	const float* getOffMeshConnectionRads()				{ return offMeshConRad; }
	const unsigned short* getOffMeshConnectionFlags()	{ return offMeshConFlags; }
	const unsigned char* getOffMeshConnectionAreas()	{ return offMeshConAreas; }
	const unsigned char* getOffMeshConnectionDirs()		{ return offMeshConDir; }
	const unsigned int* getOffMeshConnectionId()		{ return offMeshConUserID; }
	int getOffMeshConnectionCount()						{ return offMeshConCount; }


	bool raycastMesh(float* src, float* dst, float& tmin);
};


class GAMEFRAMEWORK_API NavigationSystem
{
public:
	enum SamplePartitionType
	{
		SAMPLE_PARTITION_WATERSHED,
		SAMPLE_PARTITION_MONOTONE,
		SAMPLE_PARTITION_LAYERS,
	};
	enum SamplePolyAreas
	{
		SAMPLE_POLYAREA_GROUND,
		SAMPLE_POLYAREA_WATER,
		SAMPLE_POLYAREA_ROAD,
		SAMPLE_POLYAREA_DOOR,
		SAMPLE_POLYAREA_GRASS,
		SAMPLE_POLYAREA_JUMP,
	};
	enum SamplePolyFlags
	{
		SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
		SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
		SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
		SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
		SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
		SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
	};

protected:

	rcContext* m_ctx = nullptr;

	NavMeshInfo* m_geom;
	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navQuery;
	class dtCrowd* m_crowd;
	rcHeightfield* m_solid;
	rcPolyMesh* m_pmesh;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;

	bool m_keepInterResults;

	unsigned char* m_triareas;

	/////////// Filtering Settings //////////////
	bool m_filterLowHangingObstacles	= true;
	bool m_filterLedgeSpans				= true;
	bool m_filterWalkableLowHeightSpans = true;


	//////// Rasterization Settings ///////////
	float m_cellSize	= 0.3f;
	float m_cellHeight	= 0.2f;

	//////// Agent Settings ////////////
	float m_agentHeight		= 2.0f;
	float m_agentRadius		= 0.6f;
	float m_agentMaxClimb	= 0.9f;
	float m_agentMaxSlope	= 45.0f;

	///////// Region Settings ////////////
	int m_regionMinSize		= 8;
	int m_regionMergeSize	= 20;

	////////// Polygonization Settings ///////////                                                                                                                                         
	float m_edgeMaxLen = 12.0f;
	float m_edgeMaxError = 1.3f;
	int m_vertsPerPoly = 6;

	//////// Detail Mesh Settings /////////////
	float m_detailSampleDist		= 6.0f;
	float m_detailSampleMaxError	= 1.0f;

	//////////// Partition Settings /////////////
	int m_partitionType = SAMPLE_PARTITION_WATERSHED;

public:
	NavigationSystem();

	bool HandleBuild(NavMeshInfo* m_geom);

	void cleanup()
	{
		delete[] m_triareas;
		m_triareas = 0;
		rcFreeHeightField(m_solid);
		m_solid = 0;
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
		rcFreePolyMesh(m_pmesh);
		m_pmesh = 0;
		rcFreePolyMeshDetail(m_dmesh);
		m_dmesh = 0;
		dtFreeNavMesh(m_navMesh);
		m_navMesh = 0;
	}

	//////////////////// Getters ///////////////////////////////
	NavMeshInfo*	getInputGeom()		{ return m_geom;		}
	dtCrowd*		getCrowd()			{ return m_crowd;		}
	dtNavMesh*		getNavMesh()		{ return m_navMesh;		}
	dtNavMeshQuery* getNavMeshQuery()	{ return m_navQuery;	}


	float getAgentRadius() { return m_agentRadius; }
	float getAgentHeight() { return m_agentHeight; }

	////////////// DEBUG DRAW /////////////////

	void DebugDrawPolyMesh();
};
#endif

