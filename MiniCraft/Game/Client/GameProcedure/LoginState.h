/********************************************************************
	created:	2:5:2013   1:04
	filename	LoginState.h
	author:		maval

	purpose:	游戏登陆状态.
				声明:UI资源和代码全部来自CEGUI仓库的GameMenu demo!
*********************************************************************/
#ifndef LoginState_h__
#define LoginState_h__

#include "AppState.h"
#include "InputManager.h"
#include "KratosPrerequisites.h"

class LoginState : public Kratos::CAppState
{
public:
	LoginState();
	~LoginState() {}

	DECLARE_APPSTATE_CLASS(LoginState)

	virtual void enter();
	virtual void exit();
	virtual void update(float timeSinceLastFrame);

	static STRING StateName;

private:
// 	bool OnInputSys_MousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
// 	bool OnInputSys_MouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
// 	bool OnInputSys_MouseMove(const OIS::MouseEvent& arg);
// 	bool OnInputSys_KeyPressed(const OIS::KeyEvent& arg);
// 	bool OnInputSys_KeyReleased(const OIS::KeyEvent& arg);

	void		_setupWindows();
	void		_SetupNaviArrowWindows();
	void		_SetupButtonClickHandlers();
	void		_SetupNaviIconAnimationEventHandlers();
	void		_SetupInnerButtonsSubOptionsLabels();

	enum WriteFocus
	{
		WF_TopBar,
		WF_BotBar,

		WF_Count
	};

private:
	CEGUI::Window* d_root;

	float d_timeSinceStart;

	float d_timeSinceLoginAccepted;

	bool d_interactiveElementsWereInitiallyBlendedOut;
	bool d_interactivePlanetElementsAreEnabled;
	bool d_navigationIsEnabled;
	bool d_loginWasAccepted;
	bool d_mouseIsHoveringNavi;
	bool d_startButtonClicked;

	WriteFocus d_currentWriteFocus;

	CEGUI::String d_userName;

	CEGUI::AnimationInstance* d_topBarAnimInst;
	CEGUI::AnimationInstance* d_botBarAnimInst;
	CEGUI::AnimationInstance* d_insideBlendInAnimInst;
	CEGUI::AnimationInstance* d_insideImage3RotateInInst;
	CEGUI::AnimationInstance* d_insideImage4RotateInInst;
	CEGUI::AnimationInstance* d_insideImageRingsContainerSizeInInst;

	CEGUI::AnimationInstance* d_buttonFadeInAnimInst1;
	CEGUI::AnimationInstance* d_buttonFadeInAnimInst2;
	CEGUI::AnimationInstance* d_buttonFadeInAnimInst3;
	CEGUI::AnimationInstance* d_buttonFadeInAnimInst4;
	CEGUI::AnimationInstance* d_buttonFadeInAnimInst5;

	CEGUI::AnimationInstance* d_loginContainerMoveInInst;
	CEGUI::AnimationInstance* d_loginContainerMoveOutInst;
	CEGUI::AnimationInstance* d_naviButtonLeftMoveInInst;
	CEGUI::AnimationInstance* d_naviButtonRightMoveInInst;
	CEGUI::AnimationInstance* d_naviBotMoveInInst;
	CEGUI::AnimationInstance* d_startButtonBlendInAnimInst;
	CEGUI::AnimationInstance* d_navigationLabelBlendInAnimInst;
	CEGUI::AnimationInstance* d_navigationLabelPartialBlendOutInst;

	CEGUI::AnimationInstance* d_naviPartialBlendOutInst;
	CEGUI::AnimationInstance* d_naviBlendInInst;
	CEGUI::AnimationInstance* d_centerButtonsPartialBlendOutInst;
	CEGUI::AnimationInstance* d_centerButtonsBlendInInst;

	CEGUI::AnimationInstance* d_botBarLabelBlendOutInst;

	CEGUI::AnimationInstance* d_popupLinesSaveAnimInst;
	CEGUI::AnimationInstance* d_popupLinesLoadAnimInst;
	CEGUI::AnimationInstance* d_popupLinesCharactersAnimInst;
	CEGUI::AnimationInstance* d_popupLinesOptionsAnimInst;
	CEGUI::AnimationInstance* d_popupLinesQuitAnimInst;

	CEGUI::Window* d_topBarLabel;
	CEGUI::Window* d_botBarLabel;
	CEGUI::Window* d_loginContainer;
	CEGUI::Window* d_botNaviLeftArrowArea;
	CEGUI::Window* d_botNaviRightArrowArea;
	CEGUI::Window* d_botNaviCenter;
	CEGUI::Window* d_loginAcceptButton;
	CEGUI::Window* d_startButtonClickArea;
	CEGUI::Window* d_navigationTravelIcon;
	CEGUI::Window* d_navigationSelectionIcon;
	CEGUI::Window* d_botNaviContainer;
};


#endif // LoginState_h__