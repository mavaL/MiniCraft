#include "stdafx.h"
#include "Building.h"
#include "World.h"


const STRING Building::sNamePrefix = "EntBuilding";

void Building::OnSelected( bool bSelected )
{
	//更新命令面板
	if (bSelected)
	{
		const auto& selObjs = World::GetSingleton().GetSelectedObjects();
		//TODO: 多选状态下命令面板应该怎样?
		if(selObjs.size() > 1)
			return;


	}
	else
	{

	}
}
