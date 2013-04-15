/********************************************************************
	created:	15:2:2013   16:36
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Component\Component.h
	author:		maval
	
	purpose:	组件基类
*********************************************************************/

#ifndef Component_h__
#define Component_h__

enum eComponentType
{
	eComponentType_AI,
	eComponentType_Path,
	eComponentType_Harvest,
	eComponentType_Animated,
	eComponentType_Behevior,
	eComponentType_Ragoll
};

#define QueryComponent(object, type, classname)	(static_cast<classname*>(object->GetComponent(type)))

class SelectableObject;

class Component
{
public:
	Component(SelectableObject* pOwner):m_pOwner(pOwner) {}
	virtual ~Component() {}

public:
	virtual void	Update(float dt) = 0;

protected:
	SelectableObject*	m_pOwner;		//该组件所属对象
};

#endif // Component_h__