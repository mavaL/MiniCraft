#include "stdafx.h"
#include "LoginState.h"
#include "BattleState.h"
#include "GUIManager.h"

#define _USE_MATH_DEFINES
#include <math.h>

STRING LoginState::StateName = "LoginState";
const float LoginState::s_firstStartDelay = 5.0f;
const float LoginState::s_secondStartDelay = 11.0f;
const float LoginState::s_loginDisplayStartDelay = 15.77f;


LoginState::LoginState()
:m_windowMgr(CEGUI::WindowManager::getSingleton())
,d_root(nullptr)
{
	CEGUI::SchemeManager& schemeMgr = CEGUI::SchemeManager::getSingleton();
	schemeMgr.create("GameMenu.scheme");
	schemeMgr.create("Generic.scheme");
	schemeMgr.create("TaharezLook.scheme");

	CEGUI::FontManager::getSingleton().create("DejaVuSans-12.font");
	CEGUI::FontManager::getSingleton().create("Jura-13.font");
}

void LoginState::enter()
{
	CEGUI::System::getSingleton().setDefaultFont("Jura-13");

	d_root = GUIMANAGER.LoadWindowLayout("GameMenu.layout");
	GUIMANAGER.SetGUISheet(d_root);

	_SetupWindows();
	_SetupAnimations();

	d_interactiveElementsWereInitiallyBlendedOut = false;
	d_interactivePlanetElementsAreEnabled = false;
	d_navigationIsEnabled = false;
	d_loginWasAccepted = false;
	d_mouseIsHoveringNavi = false;

	d_navigationTravelIcon->setEnabled(false);

	d_timeSinceStart = 0.0f;

	d_interactiveElementsWereInitiallyBlendedOut = false;
	d_interactivePlanetElementsAreEnabled = false;
	d_loginWasAccepted = false;
	d_startButtonClicked = false;

	d_botBarLabel->setAlpha(1.0f);
	d_botNaviContainer->setAlpha(1.0f);

	d_centerButtonsBlendInInst->setPosition(d_centerButtonsBlendInInst->getDefinition()->getDuration());
	d_centerButtonsBlendInInst->apply();

	d_currentWriteFocus = WF_BotBar;

	//Reset labels and visibility
	d_topBarLabel->setText("");
	d_botBarLabel->setText("");

	m_windowMgr.getWindow("PopupLinesEnter")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesLoad")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesCharacters")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesQuit")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesOptions")->setVisible(false);

	_ResetAnimations();


	d_loginContainer->setVisible(false);
	d_startButtonClickArea->setVisible(false);

	//Start the animations
	_StartEntranceAnimations();

	_MakeAllSelectionIconsInvisible();

	GUIMANAGER.SetCursorMode(Kratos::eCursorMode_Normal);

	m_bQuit = false;
}

void LoginState::exit()
{
	GUIMANAGER.SetGUISheet(nullptr);
	GUIMANAGER.UnloadWindowLayout(d_root);
	d_root = nullptr;

	d_topBarAnimInst->stop();
	d_botBarAnimInst->stop();
	d_insideBlendInAnimInst->stop();
	d_insideImage3RotateInInst->stop();
	d_insideImage4RotateInInst->stop();
	d_insideImageRingsContainerSizeInInst->stop();

	d_buttonFadeInAnimInst1->stop();
	d_buttonFadeInAnimInst2->stop();
	d_buttonFadeInAnimInst3->stop();
	d_buttonFadeInAnimInst4->stop();
	d_buttonFadeInAnimInst5->stop();

	d_loginContainerMoveInInst->stop();
	d_loginContainerMoveOutInst->stop();
	d_naviButtonLeftMoveInInst->stop();
	d_naviButtonRightMoveInInst->stop();
	d_naviBotMoveInInst->stop();
	d_startButtonBlendInAnimInst->stop();
	d_navigationLabelBlendInAnimInst->stop();
	d_navigationLabelPartialBlendOutInst->stop();

	d_naviPartialBlendOutInst->stop();
	d_naviBlendInInst->stop();
	d_centerButtonsPartialBlendOutInst->stop();
	d_centerButtonsBlendInInst->stop();

	d_botBarLabelBlendOutInst->stop();

	d_popupLinesEnterAnimInst->stop();
	d_popupLinesLoadAnimInst->stop();
	d_popupLinesCharactersAnimInst->stop();
	d_popupLinesOptionsAnimInst->stop();
	d_popupLinesQuitAnimInst->stop();
	
	CEGUI::AnimationManager& animMgr = CEGUI::AnimationManager::getSingleton();
	for (size_t i=0; i<animMgr.getNumAnimations();/* ++i*/)
	{
		CEGUI::Animation* anim = animMgr.getAnimationAtIdx(i);
		animMgr.destroyAnimation(anim);
	}
}

void LoginState::update( float timeSinceLastFrame )
{
	if(m_bQuit)
	{
		m_pParent->shutdown();
		return;
	}

	d_timeSinceStart += timeSinceLastFrame;

	_UpdateIntroText();
	if(d_loginWasAccepted)
	{
		d_timeSinceLoginAccepted += timeSinceLastFrame;

		_UpdateLoginWelcomeText();
		_UpdateLoginStartButtonText();
	}

	if(d_timeSinceStart >= s_loginDisplayStartDelay && !d_loginContainer->isVisible())
		d_loginContainerMoveInInst->start();

	if(d_timeSinceStart >= s_secondStartDelay && !d_interactiveElementsWereInitiallyBlendedOut)
	{
		_DisableInteractivePlanetElements();
		_DisableNavigationBarElements();
	}
}

void LoginState::_SetupWindows()
{
	d_botBarLabel = m_windowMgr.getWindow("BotBarLabel");
	d_topBarLabel = m_windowMgr.getWindow("TopBarLabel");
	d_loginContainer = m_windowMgr.getWindow("LoginContainer");

	_SetupNaviArrowWindows();

	_SetupButtonClickHandlers();

	d_botNaviContainer = m_windowMgr.getWindow("BotNavigationContainer");
	d_botNaviCenter = m_windowMgr.getWindow("NaviCenterContainer");

	d_loginAcceptButton = m_windowMgr.getWindow("AcceptButton");
	d_loginAcceptButton->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleLoginAcceptButtonClicked, this));

	d_startButtonClickArea = m_windowMgr.getWindow("InsideStartClickArea");   
	d_startButtonClickArea->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleInnerPartStartClickAreaClick, this));

	d_navigationTravelIcon = m_windowMgr.getWindow("NavigationIcon");
	d_navigationSelectionIcon = m_windowMgr.getWindow("NaviBotSelectionIcon");

	d_navigationSelectionIcon->subscribeEvent(CEGUI::AnimationInstance::EventAnimationStarted, CEGUI::Event::Subscriber(&LoginState::handleNaviSelectionIconAnimStart, this));

	_SetupNaviIconAnimationEventHandlers();

	_SetupInnerButtonsSubOptionsLabels();
}

void LoginState::_SetupNaviArrowWindows()
{
	d_botNaviLeftArrowArea = m_windowMgr.getWindow("LeftArrowArea");
	d_botNaviRightArrowArea = m_windowMgr.getWindow("RightArrowArea");
	d_botNaviLeftArrowArea->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleMouseEntersLeftArrowArea, this));
	d_botNaviLeftArrowArea->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleMouseLeavesLeftArrowArea, this));
	d_botNaviRightArrowArea->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleMouseEntersRightArrowArea, this));
	d_botNaviRightArrowArea->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleMouseLeavesRightArrowArea, this));
}

void LoginState::_SetupButtonClickHandlers()
{
	CEGUI::Window* buttonEnter = m_windowMgr.getWindow("ButtonEnter");
	buttonEnter->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleStartPopupLinesEnterDisplay, this));
	CEGUI::Window* btnEnterYes = m_windowMgr.getWindow("EnterSelectionIcon");
	btnEnterYes->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleBtnEnterYesClicked, this));
	CEGUI::Window* buttonLoad = m_windowMgr.getWindow("ButtonLoad");
	buttonLoad->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleStartPopupLinesLoadDisplay, this));
	CEGUI::Window* buttonCharacters = m_windowMgr.getWindow("ButtonCharacters");
	buttonCharacters->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleStartPopupLinesCharactersDisplay, this));
	CEGUI::Window* buttonOptions = m_windowMgr.getWindow("ButtonOptions");
	buttonOptions->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleStartPopupLinesOptionsDisplay, this));
	CEGUI::Window* buttonQuit = m_windowMgr.getWindow("ButtonQuit");
	buttonQuit->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleStartPopupLinesQuitDisplay, this));	CEGUI::Window* btnQuitYes = m_windowMgr.getWindow("YesSelectionIcon");
	btnQuitYes->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LoginState::handleBtnQuitYesClicked, this));
}

void LoginState::_SetupNaviIconAnimationEventHandlers()
{
	d_botNaviLeftArrowArea->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	d_botNaviLeftArrowArea->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	d_botNaviRightArrowArea->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	d_botNaviRightArrowArea->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));

	CEGUI::Window* window = nullptr;

	window = m_windowMgr.getWindow("NavigationLabel");
	window->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	window->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	window = m_windowMgr.getWindow("NavigationIcon");
	window->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	window->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	window = m_windowMgr.getWindow("NaviBotSelectionIcon");
	window->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
	window->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleCheckIfNaviIconAnimationNeedsChange, this));
}

void LoginState::_SetupInnerButtonsSubOptionsLabels()
{
	CEGUI::Window* label = nullptr;

	label = m_windowMgr.getWindow("LabelLoad");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelEnter");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelName2");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelNew");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelSelect");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelDelete");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelVideo");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelAudio");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelControls");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelYes");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
	label = m_windowMgr.getWindow("LabelNo");
	label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelEntered, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&LoginState::handleInnerButtonsLabelLeft, this));
}

void LoginState::_SetupAnimations()
{
	CEGUI::AnimationManager& animMgr = CEGUI::AnimationManager::getSingleton();

	animMgr.loadAnimationsFromXML("GameMenu.anims");

	CEGUI::Animation* startButtAnim = animMgr.getAnimation("StartButtonPulsating");
	CEGUI::AnimationInstance* startButtAnimInstance = animMgr.instantiateAnimation(startButtAnim);
	CEGUI::Window* startButtWindow = m_windowMgr.getWindow("StartButtonImage");
	startButtAnimInstance->setTargetWindow(startButtWindow);
	startButtAnimInstance->start();

	CEGUI::Animation* insideImg1Anim = animMgr.getAnimation("InsideImage1Pulsating");
	CEGUI::AnimationInstance* insideImg1AnimInst = animMgr.instantiateAnimation(insideImg1Anim);
	CEGUI::Window* insideImg1 = m_windowMgr.getWindow("InsideImage1");
	insideImg1AnimInst->setTargetWindow(insideImg1);
	insideImg1AnimInst->start();

	CEGUI::Animation* topBarAnim = animMgr.getAnimation("TopBarMoveInAnimation");
	d_topBarAnimInst = animMgr.instantiateAnimation(topBarAnim);
	CEGUI::Window* topBarWindow = m_windowMgr.getWindow("TopBar");
	d_topBarAnimInst->setTargetWindow(topBarWindow);

	CEGUI::Animation* botBarAnim = animMgr.getAnimation("BotBarMoveInAnimation");
	d_botBarAnimInst = animMgr.instantiateAnimation(botBarAnim);
	CEGUI::Window* botBarWindow = m_windowMgr.getWindow("BotBar");
	d_botBarAnimInst->setTargetWindow(botBarWindow);

	CEGUI::Animation* insideBlendInAnim = animMgr.getAnimation("InsideBlendIn");
	d_insideBlendInAnimInst = animMgr.instantiateAnimation(insideBlendInAnim);
	CEGUI::Window* innerPartContainer = m_windowMgr.getWindow("InnerPartContainer");
	d_insideBlendInAnimInst->setTargetWindow(innerPartContainer);

	CEGUI::Animation* insideImage3RotateIn = animMgr.getAnimation("InsideImage3RotateIn");
	d_insideImage3RotateInInst = animMgr.instantiateAnimation(insideImage3RotateIn);
	CEGUI::Window* insideImage3 = m_windowMgr.getWindow("InsideImage3");
	d_insideImage3RotateInInst->setTargetWindow(insideImage3);

	CEGUI::Animation* insideImage4RotateIn = animMgr.getAnimation("InsideImage4RotateIn");
	d_insideImage4RotateInInst = animMgr.instantiateAnimation(insideImage4RotateIn);
	CEGUI::Window* insideImage4 = m_windowMgr.getWindow("InsideImage4");
	d_insideImage4RotateInInst->setTargetWindow(insideImage4);

	CEGUI::Animation* insideImageRingsContainerSizeIn = animMgr.getAnimation("RingsContainerSizeIn");
	d_insideImageRingsContainerSizeInInst = animMgr.instantiateAnimation(insideImageRingsContainerSizeIn);
	CEGUI::Window* insideImageContainer = m_windowMgr.getWindow("OuterRingsContainer");
	d_insideImageRingsContainerSizeInInst->setTargetWindow(insideImageContainer);

	CEGUI::Animation* buttonFadeInAnim = animMgr.getAnimation("ButtonFadeIn");
	d_buttonFadeInAnimInst1 = animMgr.instantiateAnimation(buttonFadeInAnim);
	CEGUI::Window* window = m_windowMgr.getWindow("ButtonOptions");
	d_buttonFadeInAnimInst1->setTargetWindow(window);
	d_buttonFadeInAnimInst2 = animMgr.instantiateAnimation(buttonFadeInAnim);
	window = m_windowMgr.getWindow("ButtonLoad");
	d_buttonFadeInAnimInst2->setTargetWindow(window);
	d_buttonFadeInAnimInst3 = animMgr.instantiateAnimation(buttonFadeInAnim);
	window = m_windowMgr.getWindow("ButtonEnter");
	d_buttonFadeInAnimInst3->setTargetWindow(window);
	d_buttonFadeInAnimInst4 = animMgr.instantiateAnimation(buttonFadeInAnim);
	window = m_windowMgr.getWindow("ButtonCharacters");
	d_buttonFadeInAnimInst4->setTargetWindow(window);
	d_buttonFadeInAnimInst5 = animMgr.instantiateAnimation(buttonFadeInAnim);
	window = m_windowMgr.getWindow("ButtonQuit");
	d_buttonFadeInAnimInst5->setTargetWindow(window);

	CEGUI::Animation* loginContainerMoveInAnim = animMgr.getAnimation("LoginMoveIn");
	d_loginContainerMoveInInst = animMgr.instantiateAnimation(loginContainerMoveInAnim);
	d_loginContainerMoveInInst->setTargetWindow(d_loginContainer);

	CEGUI::Animation* loginContainerMoveOutAnim = animMgr.getAnimation("LoginMoveOut");
	d_loginContainerMoveOutInst = animMgr.instantiateAnimation(loginContainerMoveOutAnim);
	d_loginContainerMoveOutInst->setTargetWindow(d_loginContainer);


	CEGUI::Animation* naviButtonRightMoveInAnim = animMgr.getAnimation("NaviButtonRightMoveIn");
	d_naviButtonRightMoveInInst = animMgr.instantiateAnimation(naviButtonRightMoveInAnim);
	window = m_windowMgr.getWindow("RightArrow");
	d_naviButtonRightMoveInInst->setTargetWindow(window);

	CEGUI::Animation* naviButtonLeftMoveInAnim = animMgr.getAnimation("NaviButtonLeftMoveIn");
	d_naviButtonLeftMoveInInst = animMgr.instantiateAnimation(naviButtonLeftMoveInAnim);
	window = m_windowMgr.getWindow("LeftArrow");
	d_naviButtonLeftMoveInInst->setTargetWindow(window);

	CEGUI::Animation* naviBotMoveInAnim = animMgr.getAnimation("NaviBotCenterMoveIn");
	d_naviBotMoveInInst = animMgr.instantiateAnimation(naviBotMoveInAnim);
	window = m_windowMgr.getWindow("NaviCenterContainer");
	d_naviBotMoveInInst->setTargetWindow(window);

	CEGUI::Animation* blendInAnim = animMgr.getAnimation("BlendIn");
	d_startButtonBlendInAnimInst = animMgr.instantiateAnimation(blendInAnim);
	d_startButtonBlendInAnimInst->setTargetWindow(d_startButtonClickArea);

	CEGUI::Animation* partialBlendOutAnim = animMgr.getAnimation("PartialBlendOut");

	window = m_windowMgr.getWindow("BotNavigationContainer");
	d_naviPartialBlendOutInst = animMgr.instantiateAnimation(partialBlendOutAnim);
	d_naviPartialBlendOutInst->setTargetWindow(window);
	d_naviBlendInInst = animMgr.instantiateAnimation(blendInAnim);
	d_naviBlendInInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("InnerButtonsContainer");
	d_centerButtonsPartialBlendOutInst = animMgr.instantiateAnimation(partialBlendOutAnim);
	d_centerButtonsPartialBlendOutInst->setTargetWindow(window);
	d_centerButtonsBlendInInst = animMgr.instantiateAnimation(blendInAnim);
	d_centerButtonsBlendInInst->setTargetWindow(window);


	window = m_windowMgr.getWindow("NavigationLabel");
	d_navigationLabelPartialBlendOutInst = animMgr.instantiateAnimation(partialBlendOutAnim);
	d_navigationLabelPartialBlendOutInst->setTargetWindow(window);
	d_navigationLabelBlendInAnimInst = animMgr.instantiateAnimation(blendInAnim);
	d_navigationLabelBlendInAnimInst->setTargetWindow(window);


	CEGUI::Animation* travelRotateInAnim = animMgr.getAnimation("TravelRotateIn");
	CEGUI::AnimationInstance* travelRotateInInst = animMgr.instantiateAnimation(travelRotateInAnim);
	travelRotateInInst->setTargetWindow(d_navigationTravelIcon);
	CEGUI::Animation* travelRotateOutAnim = animMgr.getAnimation("TravelRotateOut");
	CEGUI::AnimationInstance* travelRotateOutInst = animMgr.instantiateAnimation(travelRotateOutAnim);
	travelRotateOutInst->setTargetWindow(d_navigationTravelIcon);

	CEGUI::Animation* blendOutAnim = animMgr.getAnimation("FullBlendOut");
	d_botBarLabelBlendOutInst = animMgr.instantiateAnimation(blendOutAnim);
	d_botBarLabelBlendOutInst->setTargetWindow(d_botBarLabel);

	CEGUI::Animation* loopRotateRightAnim = animMgr.getAnimation("LoopRotateRight");
	CEGUI::Animation* stopRotateAnim = animMgr.getAnimation("StopRotate");
	CEGUI::AnimationInstance* loopRotateRightAnimInst = animMgr.instantiateAnimation(loopRotateRightAnim);
	loopRotateRightAnimInst->setTargetWindow(d_navigationSelectionIcon);
	CEGUI::AnimationInstance* loopRotateLeftAnimInst = animMgr.instantiateAnimation(stopRotateAnim);
	loopRotateLeftAnimInst->setTargetWindow(d_navigationSelectionIcon);

	window = m_windowMgr.getWindow("RightArrow");
	CEGUI::Animation* naviArrowRightTwitch = animMgr.getAnimation("NaviArrowRightTwitch");
	CEGUI::AnimationInstance* naviArrowRightTwitchInst = animMgr.instantiateAnimation(naviArrowRightTwitch);
	naviArrowRightTwitchInst->setTargetWindow(window);
	CEGUI::Animation* naviArrowRightReturn = animMgr.getAnimation("NaviArrowRightReturn");
	CEGUI::AnimationInstance* naviArrowRightReturnInst = animMgr.instantiateAnimation(naviArrowRightReturn);
	naviArrowRightReturnInst->setTargetWindow(window);

	window = m_windowMgr.getWindow("LeftArrow");
	CEGUI::Animation* naviArrowLeftTwitch = animMgr.getAnimation("NaviArrowLeftTwitch");
	CEGUI::AnimationInstance* naviArrowLeftTwitchInst = animMgr.instantiateAnimation(naviArrowLeftTwitch);
	naviArrowLeftTwitchInst->setTargetWindow(window);
	CEGUI::Animation* naviArrowLeftReturn = animMgr.getAnimation("NaviArrowLeftReturn");
	CEGUI::AnimationInstance* naviArrowLeftReturnInst = animMgr.instantiateAnimation(naviArrowLeftReturn);
	naviArrowLeftReturnInst->setTargetWindow(window);

	_SetupPopupLinesAnimations();
	_SetupSelectionIconAnimations();
}

void LoginState::_ResetAnimations()
{
	//Set animations to end and apply their changes to the windows
	//For every animation instance that uses source properties
	//so the values will be taken correctly after starting them again
	d_buttonFadeInAnimInst1->setPosition(d_buttonFadeInAnimInst1->getDefinition()->getDuration());
	d_buttonFadeInAnimInst1->apply();
	d_buttonFadeInAnimInst2->setPosition(d_buttonFadeInAnimInst2->getDefinition()->getDuration());
	d_buttonFadeInAnimInst2->apply();
	d_buttonFadeInAnimInst3->setPosition(d_buttonFadeInAnimInst3->getDefinition()->getDuration());
	d_buttonFadeInAnimInst3->apply();
	d_buttonFadeInAnimInst4->setPosition(d_buttonFadeInAnimInst4->getDefinition()->getDuration());
	d_buttonFadeInAnimInst4->apply();
	d_buttonFadeInAnimInst5->setPosition(d_buttonFadeInAnimInst5->getDefinition()->getDuration());
	d_buttonFadeInAnimInst5->apply();
	d_loginContainerMoveInInst->setPosition(d_loginContainerMoveInInst->getDefinition()->getDuration());
	d_loginContainerMoveInInst->apply();
	d_naviButtonRightMoveInInst->setPosition(d_naviButtonRightMoveInInst->getDefinition()->getDuration());
	d_naviButtonRightMoveInInst->apply();
	d_naviButtonLeftMoveInInst->setPosition(d_naviButtonLeftMoveInInst->getDefinition()->getDuration());
	d_naviButtonLeftMoveInInst->apply();
	d_naviBotMoveInInst->setPosition(d_naviBotMoveInInst->getDefinition()->getDuration());
	d_naviBotMoveInInst->apply();
}

void LoginState::_StartEntranceAnimations()
{
	d_topBarAnimInst->start();
	d_botBarAnimInst->start();
	d_insideBlendInAnimInst->start();
	d_insideImage3RotateInInst->start();
	d_insideImage4RotateInInst->start();
	d_insideImageRingsContainerSizeInInst->start();
	d_buttonFadeInAnimInst1->start();
	d_buttonFadeInAnimInst2->start();
	d_buttonFadeInAnimInst3->start();
	d_buttonFadeInAnimInst4->start();
	d_buttonFadeInAnimInst5->start();
	d_naviButtonRightMoveInInst->start();
	d_naviButtonLeftMoveInInst->start();
	d_naviBotMoveInInst->start();	
}

void LoginState::_MakeAllSelectionIconsInvisible()
{
	m_windowMgr.getWindow("EnterSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("NoSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("SelectSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("YesSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("AudioSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("VideoSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("ControlsSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("DeleteSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("Name2SelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("LoadSelectionIcon")->setVisible(false);
	m_windowMgr.getWindow("NewSelectionIcon")->setVisible(false);
}

void LoginState::_SetupPopupLinesAnimations()
{
	CEGUI::Window* window = nullptr;
	CEGUI::AnimationManager& animMgr = CEGUI::AnimationManager::getSingleton();

	CEGUI::Animation* sizeGrowth = animMgr.getAnimation("SizeGrowth");
	window = m_windowMgr.getWindow("PopupLinesEnter");
	d_popupLinesEnterAnimInst = animMgr.instantiateAnimation(sizeGrowth);
	d_popupLinesEnterAnimInst->setTarget(window);
	window = m_windowMgr.getWindow("PopupLinesLoad");
	d_popupLinesLoadAnimInst = animMgr.instantiateAnimation(sizeGrowth);
	d_popupLinesLoadAnimInst->setTarget(window);
	window = m_windowMgr.getWindow("PopupLinesCharacters");
	d_popupLinesCharactersAnimInst = animMgr.instantiateAnimation(sizeGrowth);
	d_popupLinesCharactersAnimInst->setTarget(window);
	window = m_windowMgr.getWindow("PopupLinesOptions");
	d_popupLinesOptionsAnimInst = animMgr.instantiateAnimation(sizeGrowth);
	d_popupLinesOptionsAnimInst->setTarget(window);
	window = m_windowMgr.getWindow("PopupLinesQuit");
	d_popupLinesQuitAnimInst = animMgr.instantiateAnimation(sizeGrowth);
	d_popupLinesQuitAnimInst->setTarget(window);
}

void LoginState::_SetupSelectionIconAnimations()
{
	CEGUI::Window* window = nullptr;
	CEGUI::AnimationManager& animMgr = CEGUI::AnimationManager::getSingleton();

	CEGUI::Animation* iconAnimationLoop = animMgr.getAnimation("LoopRotateRight");
	CEGUI::Animation* iconAnimationStop = animMgr.getAnimation("StopRotate");
	CEGUI::AnimationInstance* iconAnimInst;

	window = m_windowMgr.getWindow("LoadSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("EnterSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("NoSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("YesSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("SelectSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("EnterSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("AudioSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("VideoSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("ControlsSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("Name2SelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("NewSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
	window = m_windowMgr.getWindow("DeleteSelectionIcon");
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationLoop);
	iconAnimInst->setTargetWindow(window);
	iconAnimInst = animMgr.instantiateAnimation(iconAnimationStop);
	iconAnimInst->setTargetWindow(window);
}

void LoginState::_UpdateIntroText()
{
	static const CEGUI::String firstPart = "Connection established...";
	static const CEGUI::String secondPart = "Warning! User Authentication required!";

	CEGUI::String finalText;

	int firstPartTypeProgress = static_cast<int>((d_timeSinceStart - s_firstStartDelay) / 0.08f);
	if(firstPartTypeProgress > 0)
		finalText += firstPart.substr(0, std::min<unsigned int>(firstPart.length(), firstPartTypeProgress));

	int secondPartTypeProgress = static_cast<int>((d_timeSinceStart - s_secondStartDelay) / 0.08f);
	if(secondPartTypeProgress > 0)
		finalText += "\n" + secondPart.substr(0, std::min<unsigned int>(secondPart.length(), secondPartTypeProgress));

	finalText += "[font='DejaVuSans-12']";

	static double blinkStartDelay = 3.6f;
	double blinkPeriod = 0.8;
	double blinkTime = std::modf(static_cast<double>(d_timeSinceStart), &blinkPeriod);
	if(blinkTime > 0.55 || d_timeSinceStart < blinkStartDelay || d_currentWriteFocus != WF_BotBar)
		finalText += "[colour='00000000']";

	d_botBarLabel->setText(finalText);
}

void LoginState::_UpdateLoginWelcomeText()
{
	if(d_timeSinceLoginAccepted <= 0.0f)
		return;

	static const CEGUI::String firstPart = "Welcome ";
	CEGUI::String displayText = firstPart + d_userName;
	CEGUI::String finalText;

	int progress = static_cast<int>(d_timeSinceLoginAccepted / 0.08f);
	if(progress > 0)
		finalText += displayText.substr(0, std::min<unsigned int>(displayText.length(), progress));

	finalText += "[font='DejaVuSans-12']";

	double blinkPeriod = 0.8;
	double blinkTime = std::modf(static_cast<double>(d_timeSinceStart), &blinkPeriod);
	if(blinkTime > 0.55 || d_currentWriteFocus != WF_TopBar)
		finalText += "[colour='00000000']";

	d_topBarLabel->setText(finalText);
}

void LoginState::_UpdateLoginStartButtonText()
{
	if(d_timeSinceLoginAccepted <= 0.0f)
		return;

	static const float writeDelay = 1.7f;
	static const CEGUI::String displayText = "Proceed by selecting a planet";

	CEGUI::String finalText;

	int progress = static_cast<int>((d_timeSinceLoginAccepted - writeDelay) / 0.08f);
	if(progress > 0)
		finalText += displayText.substr(0, std::min<unsigned int>(displayText.length(), progress));

	finalText += "[font='DejaVuSans-12']";


	double blinkPeriod = 0.8;
	double blinkTime = std::modf(static_cast<double>(d_timeSinceStart), &blinkPeriod);
	if(blinkTime > 0.55 || d_currentWriteFocus != WF_BotBar)
		finalText += "[colour='00000000']";

	if(d_timeSinceLoginAccepted >= writeDelay)
		d_currentWriteFocus = WF_BotBar;

	d_botBarLabel->setText(finalText);
}

void LoginState::_DisableInteractivePlanetElements()
{
	d_centerButtonsBlendInInst->pause();
	d_centerButtonsPartialBlendOutInst->start();
	d_interactiveElementsWereInitiallyBlendedOut = true;
}

void LoginState::_DisableNavigationBarElements()
{
	d_naviBlendInInst->pause();
	d_naviPartialBlendOutInst->start();

	d_navigationLabelBlendInAnimInst->stop();
	d_navigationLabelPartialBlendOutInst->start();

	d_navigationIsEnabled = false;
}

void LoginState::_EnableInteractivePlanetElements()
{
	d_centerButtonsPartialBlendOutInst->pause();
	d_centerButtonsBlendInInst->start();

	d_interactivePlanetElementsAreEnabled = true;
}

void LoginState::_EnableNavigationBarElements()
{
	d_naviPartialBlendOutInst->pause();
	d_naviBlendInInst->start();

	d_navigationLabelPartialBlendOutInst->stop();
	d_navigationLabelBlendInAnimInst->start();

	d_navigationIsEnabled = true;
}

bool LoginState::handleLoginAcceptButtonClicked( const CEGUI::EventArgs& args )
{
	d_startButtonClickArea->setAlpha(0.0f);
	d_startButtonBlendInAnimInst->start();

	_EnableNavigationBarElements();

	d_loginContainerMoveOutInst->start();
	d_navigationTravelIcon->setEnabled(true);

	CEGUI::Editbox* loginEditbox = static_cast<CEGUI::Editbox*>(m_windowMgr.getWindow("LoginEditbox"));
	d_userName = loginEditbox->getText();

	d_timeSinceLoginAccepted = 0.0f;
	d_loginWasAccepted = true;
	d_currentWriteFocus = WF_TopBar;

	d_botNaviContainer->setEnabled(true);

	return false;
}

bool LoginState::handleInnerPartStartClickAreaClick( const CEGUI::EventArgs& args )
{
	if(!d_interactivePlanetElementsAreEnabled)
		_EnableInteractivePlanetElements();

	d_startButtonBlendInAnimInst->pause();
	d_startButtonClickArea->setVisible(false);

	d_startButtonClicked = true;

	return false;
}

bool LoginState::handleCheckIfNaviIconAnimationNeedsChange( const CEGUI::EventArgs& args )
{
	if(!d_loginWasAccepted)
		return false;

	bool mouseIsHovering = false;
	CEGUI::Window* window = nullptr;

	window = m_windowMgr.getWindow("NavigationLabel");
	mouseIsHovering |= window->isMouseContainedInArea();
	window = m_windowMgr.getWindow("NavigationIcon");
	mouseIsHovering |= window->isMouseContainedInArea();
	window = m_windowMgr.getWindow("NaviBotSelectionIcon");
	mouseIsHovering |= window->isMouseContainedInArea();
	mouseIsHovering |= d_botNaviRightArrowArea->isMouseContainedInArea();
	mouseIsHovering |= d_botNaviLeftArrowArea->isMouseContainedInArea();

	//We fire an event to trigger the animation depending on if the mouse hovers a critical
	//window or not. Additionally we perform a check to not fire an event for an animation that is already running
	if(d_mouseIsHoveringNavi != mouseIsHovering)
	{
		d_mouseIsHoveringNavi = mouseIsHovering;
		CEGUI::EventArgs args;

		if(mouseIsHovering)
		{
			d_navigationSelectionIcon->fireEvent("StartRotate", args);
			d_navigationTravelIcon->fireEvent("StartRotate", args);
		}
		else
		{
			d_navigationSelectionIcon->fireEvent("StopRotate", args);
			d_navigationTravelIcon->fireEvent("StopRotate", args);
		}
	}

	return false;
}

static float getAngle(const CEGUI::Quaternion& quat)
{
	return 2.0f * std::acos(quat.d_w);
}

bool LoginState::handleNaviSelectionIconAnimStart( const CEGUI::EventArgs& args )
{
	const CEGUI::AnimationEventArgs& animArgs = static_cast<const CEGUI::AnimationEventArgs&>(args);

	CEGUI::AnimationInstance* animInst = animArgs.instance;

	if(animInst->getDefinition() == CEGUI::AnimationManager::getSingleton().getAnimation("LoopRotateRight"))
	{
		CEGUI::Quaternion curRotation = CEGUI::PropertyHelper::stringToQuaternion(animInst->getTarget()->getProperty("Rotation"));

		float curAngle = getAngle(curRotation);
		float progress = curAngle / (2.0f * static_cast<float>(M_PI));
		animInst->setPosition(progress * animInst->getDefinition()->getDuration());
	}

	return false;
}

bool LoginState::handleMouseEntersLeftArrowArea( const CEGUI::EventArgs& args )
{
	CEGUI::EventArgs fireArgs;
	if(d_loginWasAccepted)
		m_windowMgr.getWindow("LeftArrow")->fireEvent("StartTwitching", fireArgs);

	return false;
}

bool LoginState::handleMouseLeavesLeftArrowArea( const CEGUI::EventArgs& args )
{
	CEGUI::EventArgs fireArgs;
	if(d_loginWasAccepted)
		m_windowMgr.getWindow("LeftArrow")->fireEvent("EndTwitching", fireArgs);

	return false;
}

bool LoginState::handleMouseEntersRightArrowArea( const CEGUI::EventArgs& args )
{
	CEGUI::EventArgs fireArgs;
	if(d_loginWasAccepted)
		m_windowMgr.getWindow("RightArrow")->fireEvent("StartTwitching", fireArgs);

	return false;
}

bool LoginState::handleMouseLeavesRightArrowArea( const CEGUI::EventArgs& args )
{
	CEGUI::EventArgs fireArgs;
	if(d_loginWasAccepted)
		m_windowMgr.getWindow("RightArrow")->fireEvent("EndTwitching", fireArgs);

	return false;
}

bool LoginState::handleStartPopupLinesEnterDisplay( const CEGUI::EventArgs& args )
{
	if(!d_startButtonClicked)
		return false;

	_MakeAllSelectionIconsInvisible();
	_StopStartPopupLinesAnimations();
	d_popupLinesEnterAnimInst->start();

	m_windowMgr.getWindow("EnterSelectionIcon")->setVisible(true);

	return false;
}

bool LoginState::handleStartPopupLinesLoadDisplay( const CEGUI::EventArgs& args )
{
	if(!d_startButtonClicked)
		return false;

	_MakeAllSelectionIconsInvisible();
	_StopStartPopupLinesAnimations();
	d_popupLinesLoadAnimInst->start();

	m_windowMgr.getWindow("LoadSelectionIcon")->setVisible(true);

	return false;
}

bool LoginState::handleStartPopupLinesCharactersDisplay( const CEGUI::EventArgs& args )
{
	if(!d_startButtonClicked)
		return false;

	_MakeAllSelectionIconsInvisible();

	m_windowMgr.getWindow("DeleteSelectionIcon")->setVisible(true);
	m_windowMgr.getWindow("Name2SelectionIcon")->setVisible(true);
	m_windowMgr.getWindow("SelectSelectionIcon")->setVisible(true);
	m_windowMgr.getWindow("NewSelectionIcon")->setVisible(true);

	_StopStartPopupLinesAnimations();
	d_popupLinesCharactersAnimInst->start();

	return false;
}

bool LoginState::handleStartPopupLinesOptionsDisplay( const CEGUI::EventArgs& args )
{
	if(!d_startButtonClicked)
		return false;

	_MakeAllSelectionIconsInvisible();
	_StopStartPopupLinesAnimations();
	d_popupLinesOptionsAnimInst->start();

	m_windowMgr.getWindow("AudioSelectionIcon")->setVisible(true);
	m_windowMgr.getWindow("VideoSelectionIcon")->setVisible(true);
	m_windowMgr.getWindow("ControlsSelectionIcon")->setVisible(true);

	return false;
}

bool LoginState::handleStartPopupLinesQuitDisplay( const CEGUI::EventArgs& args )
{
	_MakeAllSelectionIconsInvisible();
	_StopStartPopupLinesAnimations();
	d_popupLinesQuitAnimInst->start();

	m_windowMgr.getWindow("YesSelectionIcon")->setVisible(true);
	m_windowMgr.getWindow("NoSelectionIcon")->setVisible(true);

	return false;
}

bool LoginState::handleInnerButtonsLabelEntered( const CEGUI::EventArgs& args )
{
	const CEGUI::MouseEventArgs& mouseArgs = static_cast<const CEGUI::MouseEventArgs&>(args);

	CEGUI::Window* iconWindow = getIconWindowFromLabel(mouseArgs.window);

	CEGUI::EventArgs fireArgs;
	iconWindow->fireEvent("StartRotate", fireArgs);

	return false;
}

CEGUI::Window* LoginState::getIconWindowFromLabel( CEGUI::Window* window )
{
	const CEGUI::String windowPrefix = "Label";
	const CEGUI::String iconPostfix = "SelectionIcon";

	CEGUI::String windowName = window->getName();

	windowName = windowName.substr(windowPrefix.length());
	return m_windowMgr.getWindow(windowName + iconPostfix);
}

bool LoginState::handleInnerButtonsLabelLeft( const CEGUI::EventArgs& args )
{
	const CEGUI::MouseEventArgs& mouseArgs = static_cast<const CEGUI::MouseEventArgs&>(args);

	CEGUI::Window* iconWindow = getIconWindowFromLabel(mouseArgs.window);

	CEGUI::EventArgs fireArgs;
	iconWindow->fireEvent("StopRotate", fireArgs);

	return false;
}

void LoginState::_StopStartPopupLinesAnimations()
{
	d_popupLinesCharactersAnimInst->setPosition(d_popupLinesLoadAnimInst->getDefinition()->getDuration());
	d_popupLinesCharactersAnimInst->apply();
	d_popupLinesCharactersAnimInst->pause();
	d_popupLinesLoadAnimInst->setPosition(d_popupLinesLoadAnimInst->getDefinition()->getDuration());
	d_popupLinesLoadAnimInst->apply();
	d_popupLinesLoadAnimInst->pause();
	d_popupLinesEnterAnimInst->setPosition(d_popupLinesEnterAnimInst->getDefinition()->getDuration());
	d_popupLinesEnterAnimInst->apply();
	d_popupLinesEnterAnimInst->pause();
	d_popupLinesQuitAnimInst->setPosition(d_popupLinesQuitAnimInst->getDefinition()->getDuration());
	d_popupLinesQuitAnimInst->apply();
	d_popupLinesQuitAnimInst->pause();
	d_popupLinesOptionsAnimInst->setPosition(d_popupLinesOptionsAnimInst->getDefinition()->getDuration());
	d_popupLinesOptionsAnimInst->apply();
	d_popupLinesOptionsAnimInst->pause();

	m_windowMgr.getWindow("PopupLinesEnter")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesLoad")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesCharacters")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesOptions")->setVisible(false);
	m_windowMgr.getWindow("PopupLinesQuit")->setVisible(false);
}

bool LoginState::handleBtnQuitYesClicked( const CEGUI::EventArgs& args )
{
	m_bQuit = true;
	return false;
}

bool LoginState::handleBtnEnterYesClicked( const CEGUI::EventArgs& args )
{
	//Ω¯»Î”Œœ∑
	changeAppState(findByName(CBattleState::StateName));
	return false;
}
