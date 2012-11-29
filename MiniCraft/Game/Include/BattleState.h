#ifndef BATTLE_STATE_H
#define BATTLE_STATE_H

#include "AppState.h"
#include "InputManager.h"


namespace Ogre
{
	class Camera;
	class SceneManager;
}
namespace OgreBites
{
	class SdkCameraMan;
}
class OgreRecast;
class OgreDetourTileCache;
class OgreDetourCrowd;
class Character;



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

private:
	bool						m_bQuit;
	Ogre::Camera*				m_pCamera;
	Ogre::SceneManager*			m_pSceneMgr;
	OgreBites::SdkCameraMan*	m_pCameraMan;

	OgreRecast*					m_pRecast;
	OgreDetourTileCache*		m_pDetourTileCache;
	OgreDetourCrowd*			m_pDetourCrowd;
	Character*					m_pCharacter;
};


#endif