#pragma once

#ifndef GAME_DEFINE_H
#define GAME_DEFINE_H


///!!!!这里要与UnitState.lua中的常量相对应
const std::string	SCENE_MANAGER_NAME	=	"DefaultSceneMgr";
const Ogre::Vector3	RES_POS(0, 0, 0);
const Ogre::Vector3	BASE_POS(0, 0, 30);
const Ogre::Vector3 RES_COLLECT_POS(-1.25f, 0, 9.64f);
const float UNIT_OFFSET_TO_GROURD		=	0.5f;

const	float	SCENE_VERSION	=	0.1f;	//当前.scene版本号,注意与编辑器同步



#endif

