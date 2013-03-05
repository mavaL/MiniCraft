/********************************************************************
	created:	17:2:2013   1:23
	filename: 	F:\MiniCraft\Kratos\Src\SceneSerializer.h
	author:		maval
	
	purpose:	场景加载器基类.通过模板设计模式让客户端和编辑器
				通过私有继承它来实现某些加载内容的加载特化行为.
*********************************************************************/

#ifndef SceneSerializer_h__
#define SceneSerializer_h__

#include <OgrePrerequisites.h>
#include <rapidxml.hpp>       
#include <rapidxml_utils.hpp>
#include <rapidxml_print.hpp>

class Scene;

class SceneSerializer
{
public:
	SceneSerializer();
	virtual ~SceneSerializer() {}

public:
	void			LoadScene(const std::string& sceneName, const std::string& sceneGroup, Scene* pOwner);
	void			SaveScene(const std::string& fullPath, Scene* pOwner);

private:
	virtual	void	_LoadTerrain(rapidxml::xml_node<>* node);
	virtual void	_LoadObjects(rapidxml::xml_node<>* node) {}
	virtual void	_LoadEffect(rapidxml::xml_node<>* node);
	virtual void	_SaveTerrain(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode) {}
	virtual void	_SaveObjects(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode) {}
	virtual void	_SaveEffect(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode) {}

private:
	Scene*				m_pOwner;
	std::string			m_sceneGroup;
};

#endif // SceneSerializer_h__