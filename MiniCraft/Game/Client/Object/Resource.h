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
	Resource();
	~Resource() {}

public:
	virtual eObjectType GetType() const { return eObjectType_Resource; }
	virtual void	Update(float dt) {}

	void			DecreaseRes(int num) { m_resLeft -= num; }
	//该资源被采集一次的数量
	int				GetGatherOnceNum()	{ return 5; }

private:
	int			m_resLeft;		//剩余资源数
};


#endif // Resource_h_