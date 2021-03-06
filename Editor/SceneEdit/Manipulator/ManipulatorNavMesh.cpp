#include "stdafx.h"
#include "ManipulatorNavMesh.h"
#include "EditorDefine.h"
#include "ManipulatorScene.h"
#include "Utility.h"
#include "OgreManager.h"
#include "Scene.h"

ManipulatorNavMesh::ManipulatorNavMesh()
:m_bShowNavMesh(false)
,m_bHasGenerate(false)
,m_debugRenderNode(nullptr)
,m_pInputGeom(nullptr)
{
	m_pRecast = new OgreRecast(RenderManager.m_pSceneMgr);
	m_pDetourTileCache = new OgreDetourTileCache(m_pRecast);

	ManipulatorScene::GetSingleton().AddCallback(this);
}

ManipulatorNavMesh::~ManipulatorNavMesh()
{
	ManipulatorScene::GetSingleton().RemoveCallback(this);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);
}

void ManipulatorNavMesh::Generate()
{
	Reset();

	ManipulatorTerrain& manTerrain = ManipulatorSystem.GetTerrain();
	try
	{
		//获取参与构建NavMesh的物体
		const auto& objects = ManipulatorSystem.GetObject().GetAllObjects();
		std::vector<Ogre::Entity*> vecNavMeshEnt;
		for (auto iter=objects.begin(); iter!=objects.end(); ++iter)
		{
			if((iter->second)->m_bAddToNavmesh)
				vecNavMeshEnt.push_back(iter->first);
		}

		m_pInputGeom = new InputGeom(ManipulatorSystem.GetScene()->GetTerrainGroup(), vecNavMeshEnt);
		//pGeom->debugMesh(pSceneMgr);

		if(!m_pDetourTileCache->TileCacheBuild(m_pInputGeom))
			throw std::bad_exception("Error! Build NavMesh Failed!");
	}
	catch(const std::exception& e)
	{
		MessageBoxA(0, e.what(), "ERROR", MB_ICONERROR);
		return;
	}

	m_pDetourTileCache->drawNavMesh();
	m_debugRenderNode = (Ogre::SceneNode*)RenderManager.m_pSceneMgr->getRootSceneNode()->getChild("RecastSN");
	assert(m_debugRenderNode);

	m_debugRenderNode->setVisible(false);
	m_bHasGenerate = true;
}

void ManipulatorNavMesh::Reset()
{
	if(!m_pRecast)
		m_pRecast = new OgreRecast(RenderManager.m_pSceneMgr);
	if(!m_pDetourTileCache )
		m_pDetourTileCache = new OgreDetourTileCache(m_pRecast);

	m_pRecast->RecastCleanup();
	SAFE_DELETE(m_pInputGeom);

	//销毁debug render
	std::vector<Ogre::MovableObject*> vecTobeDestroy;
	auto range = m_pRecast->m_pRecastSN->getAttachedObjectIterator();
	while (range.hasMoreElements())
		vecTobeDestroy.push_back(range.getNext());

	m_pRecast->m_pRecastSN->detachAllObjects();

	for(size_t i=0; i<vecTobeDestroy.size(); ++i)
		RenderManager.m_pSceneMgr->destroyMovableObject(vecTobeDestroy[i]);
	vecTobeDestroy.clear();

	//重新配置Recast
	OgreRecastConfigParams recastParams = OgreRecastConfigParams();
	recastParams.setCellSize(1);
	recastParams.setCellHeight(0.16f);
	recastParams.setAgentMaxSlope(15);
	recastParams.setAgentHeight(1.5f);
	recastParams.setAgentMaxClimb(0.5f);
	recastParams.setAgentRadius(0.6f);
	recastParams.setEdgeMaxLen(2);
	recastParams.setEdgeMaxError(1.3f);
	recastParams.setVertsPerPoly(6);
	recastParams.setRegionMinSize(2);
	recastParams.setRegionMergeSize(3);
	recastParams.setDetailSampleDist(6);
	recastParams.setDetailSampleMaxError(1);

	m_pRecast->configure(recastParams);

	m_bHasGenerate = false;
	m_bShowNavMesh = false;
}

void ManipulatorNavMesh::SaveObj()
{
	if(!m_bHasGenerate)
		return;
	
	assert(m_pRecast && m_pInputGeom);

	std::wstring filename = ManipulatorSystem.GenerateSceneFullPath();
	filename += ManipulatorSystem.GetSceneName() + L"_";
	filename += L"NavMesh.obj";
	m_pInputGeom->writeObj(Utility::UnicodeToEngine(filename));
}

void ManipulatorNavMesh::SaveNavMesh()
{
	if(!m_bHasGenerate)
		return;

	assert(m_pRecast && m_pInputGeom);

	std::wstring filename = ManipulatorSystem.GenerateSceneFullPath();
	filename += ManipulatorSystem.GetSceneName() + L"_";
	filename += L"NavMesh.Bin";
	try
	{
		if(!m_pDetourTileCache->saveAll(Utility::UnicodeToEngine(filename)))
			throw std::bad_exception("Error! Save NavMesh.Bin Failed!");
	}
	catch(const std::exception& e)
	{
		MessageBoxA(0, e.what(), "ERROR", MB_ICONERROR);
	}
}

void ManipulatorNavMesh::ShowNavMesh( bool bShow )
{
	m_debugRenderNode->setVisible(bShow);
	m_bShowNavMesh = bShow;
}

void ManipulatorNavMesh::OnSceneNew()
{
	Reset();
}

void ManipulatorNavMesh::OnSceneOpen()
{
	Reset();
}

void ManipulatorNavMesh::OnSceneClose()
{
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);
}
