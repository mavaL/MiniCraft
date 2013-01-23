/********************************************************************
	created:	24:1:2013   0:34
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Object\Building.h
	author:		maval
	
	purpose:	½¨ÖþÀà
*********************************************************************/

#ifndef Building_h__
#define Building_h__

#include "SelectableObject.h"

class Building : public SelectableObject
{
public:
	Building():SelectableObject() {}
	~Building() {}

	const static STRING BUILDING_NAME_PREFIX;

public:
	virtual eObjectType GetType() const { return eObjectType_Building; }
	virtual void	Update(float dt) {}
	virtual	void	CreateRenderInstance(const STRING& meshname);
};


#endif // Building_h__