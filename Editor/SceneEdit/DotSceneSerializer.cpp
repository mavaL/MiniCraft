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
	
	//保存文件
	String filename(path+"\\");
	filename += scene;
	filename += ".scene";
	std::ofstream out(filename);
	out << doc;
}