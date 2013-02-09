/********************************************************************
	created:	24:1:2013   21:54
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Object\Resource.h
	author:		maval
	
	purpose:	可采集资源类
*********************************************************************/

#ifndef Resource_h__
#define Resource_h__


#include "SelectableObject.h"


class Resource : public SelectableObject
{
public:
	Resource():SelectableObject() {}
	~Resource() {}

public:
	virtual eObjectType GetType() const { return eObjectType_Resource; }
	virtual void	Update(float dt) {}
};


#endif // Resource_h_