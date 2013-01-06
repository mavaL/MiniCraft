/********************************************************************
	created:	20:12:2012   1:02
	filename: 	F:\MiniCraft\Editor\SceneEdit\EditorDefine.h
	author:		maval
	
	purpose:	常量参数配置
*********************************************************************/

#ifndef EditorDefine_h__
#define EditorDefine_h__




const	float	SCENE_VERSION	=	0.1f;	//当前.scene版本号,注意与客户端同步
const	std::string	SCENE_MANAGER_NAME	=	"DefaultSceneMgr";//默认场景管理器名字
const	int		EDITOR_CLIENT_W			=	1024;
const	int		EDITOR_CLIENT_H			=	768;
const	int		FPS					=	100;
const	float	TIME_PER_FRAME		=	1.0f / FPS;
const	int		MESH_ICON_SIZE		=	64;
const	int		RES_SELECTOR_COLUMN_WIDTH	=	80;
const	String	MAIN_CAMERA_NAME	=	"MainCamera";
const	int		TERRAIN_MAX_LAYER	=	5;	//最多5层地形纹理混合

//场景查询掩码
enum eQueryMask
{
	eQueryMask_Default	=	0,
	eQueryMask_Entity = 1 << 0
};

enum eAxis
{
	eAxis_X,
	eAxis_Y,
	eAxis_Z,
	eAxis_None
};


#endif // EditorDefine_h__