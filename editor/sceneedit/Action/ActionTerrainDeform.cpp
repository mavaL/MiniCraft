#include "stdafx.h"
#include "ActionTerrainDeform.h"
#include "Manipulator/ManipulatorScene.h"



void ActionTerrainDeform::Enter()
{
	ManipulatorSystem.GetTerrain().SetTerrainModifyEnabled(true);
}

void ActionTerrainDeform::Leave()
{
	ManipulatorSystem.GetTerrain().SetTerrainModifyEnabled(false);
}

void ActionTerrainDeform::OnMouseLButtonDown( const SActionParam& param )
{
	if(!param.m_bHitTerrain)
		return;
}

void ActionTerrainDeform::OnMouseLButtonUp( const SActionParam& param )
{

}

void ActionTerrainDeform::OnMouseMove( const SActionParam& param )
{
	if(param.m_bHitTerrain)
		ManipulatorSystem.GetTerrain().SetBrushPosition(param.m_ptTerrain);
}


