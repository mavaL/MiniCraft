#ifndef BATTLE_STATE_H
#define BATTLE_STATE_H

#include "AppState.h"
#include "InputManager.h"


namespace Ogre
{
	class Camera;
	class SceneManager;
	class Entity;
}

class OgreRecast;
class OgreDetourTileCache;
class OgreDetourCrowd;
class Unit;
class CommandBase;


/************************************************************************/
/*							游戏进行状态类                                */
/************************************************************************/

class CBattleState : public CAppState
{
public:
	CBattleState();
	~CBattleState();

	DECLARE_APPSTATE_CLASS(CBattleState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();
	void update(float timeSinceLastFrame);

	static std::string StateName;

private:
	bool OnInputSys_MousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool OnInputSys_MouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool OnInputSys_MouseMove(const OIS::MouseEvent& arg);
	bool OnInputSys_KeyPressed(const OIS::KeyEvent& arg);
	bool OnInputSys_KeyReleased(const OIS::KeyEvent& arg);

	void _QueryCursorPos(Ogre::Vector3& retPos);
	//获取右击目标点会产生的命令(移动,攻击...)
	CommandBase* _ComputeCommand(Unit* pUnit, const Ogre::Vector3& targetPos);

private:
	bool						m_bQuit;
	Ogre::Camera*				m_pCamera;

	bool						m_bCamMoveLeft, m_bCamMoveRight, m_bCamMoveUp, m_bCamMoveDown;
};


#endif