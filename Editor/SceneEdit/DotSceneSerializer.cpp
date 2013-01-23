#include "stdafx.h"
#include "DotSceneSerializer.h"
#include <fstream>
#include "Manipulator/ManipulatorScene.h"
#include "EditorDefine.h"


void DotSceneSerialezer::Serialize( const Ogre::String& path, const Ogre::String& scene )
{
	//创建场景文件夹
	::CreateDirectoryA(path.c_str(), NULL);

	using namespace rapidxml;

	xml_document<> doc;  
	//文件头
	xml_node<>* rot = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='utf-8'"));
	doc.append_node(rot);

	//scene节
	xml_node<>* sceneNode =   doc.allocate_node(node_element, "scene");
	String version = Ogre::StringConverter::toString(SCENE_VERSION);
	sceneNode->append_attribute(doc.allocate_attribute("formatVersion", doc.allocate_string(version.c_str())));
	doc.append_node(sceneNode);

	//terrain节
	xml_node<>* terrainNode =   doc.allocate_node(node_element, "terrain");
	ManipulatorSystem.GetTerrain().Serialize(doc, terrainNode);
	sceneNode->append_node(terrainNode);

	//object节
	auto objects = ManipulatorSystem.GetObject().GetAllObjects();
	String count = Ogre::StringConverter::toString(objects.size());
	xml_node<>* objsNode =   doc.allocate_node(node_element, "objects");
	objsNode->append_attribute(doc.allocate_attribute("count", doc.allocate_string(count.c_str())));
	sceneNode->append_node(objsNode);

	for (auto iter=objects.begin(); iter!=objects.end(); ++iter)
	{
		Ogre::Entity* pObj = iter->first;
		xml_node<>* objNode =   doc.allocate_node(node_element, "entity");

		//meshname
		const String& strMesh = pObj->getMesh()->getName();
		objNode->append_attribute(doc.allocate_attribute("meshname", doc.allocate_string(strMesh.c_str())));
		//add to navmesh
		const String& strIsNavMesh = Ogre::StringConverter::toString((iter->second)->m_bAddToNavmesh);
		objNode->append_attribute(doc.allocate_attribute("isnavmesh", doc.allocate_string(strIsNavMesh.c_str())));
		//is building
		const String& strIsBuilding = Ogre::StringConverter::toString((iter->second)->m_bIsBuilding);
		objNode->append_attribute(doc.allocate_attribute("isbuilding", doc.allocate_string(strIsBuilding.c_str())));
		//position
		String strPos = Ogre::StringConverter::toString(pObj->getParentSceneNode()->_getDerivedPosition());
		objNode->append_attribute(doc.allocate_attribute("position", doc.allocate_string(strPos.c_str())));
		//orientation
		String strOrient = Ogre::StringConverter::toString(pObj->getParentSceneNode()->_getDerivedOrientation());
		objNode->append_attribute(doc.allocate_attribute("orientation", doc.allocate_string(strOrient.c_str())));
		//scale
		String strScale = Ogre::StringConverter::toString(pObj->getParentSceneNode()->_getDerivedScale());
		objNode->append_attribute(doc.allocate_attribute("scale", doc.allocate_string(strScale.c_str())));

		objsNode->append_node(objNode);
	}
	
	//保存文件
	String filename(path+"\\");
	filename += scene;
	filename += ".scene";
	std::ofstream out(filename);
	out << doc;
}