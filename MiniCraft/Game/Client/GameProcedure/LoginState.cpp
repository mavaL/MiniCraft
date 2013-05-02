#include "stdafx.h"
#include "LoginState.h"
#include "GUIManager.h"

STRING LoginState::StateName = "LoginState";


LoginState::LoginState()
:d_root(nullptr)
{
	CEGUI::SchemeManager& schemeMgr = CEGUI::SchemeManager::getSingleton();
	schemeMgr.create("GameMenu.scheme");
	schemeMgr.create("Generic.scheme");
	schemeMgr.create("TaharezLook.scheme");
}

void LoginState::enter()
{
	CEGUI::FontManager::getSingleton().create("DejaVuSans-12.font");
	CEGUI::Font& defaultFont = CEGUI::FontManager::getSingleton().create("Jura-13.font");
	CEGUI::System::getSingleton().setDefaultFont(&defaultFont);

	d_root = GUIMANAGER.LoadWindowLayout("GameMenu.layout");
	CEGUI::System::getSingleton().setGUISheet(d_root);

	_setupWindows();
	//setupAnimations();

	GUIMANAGER.SetCursorMode(Kratos::eCursorMode_Normal);
}

void LoginState::exit()
{

}

void LoginState::update( float timeSinceLastFrame )
{

}

void LoginState::_setupWindows()
{
// 	d_botBarLabel = d_root->getChild("BotBar/BotBarLabel");
// 	d_topBarLabel = d_root->getChild("TopBar/TopBarLabel");
// 
// 	d_loginContainer = d_root->getChild("LoginContainer");
// 
// 	_SetupNaviArrowWindows();
// 
// 	_SetupButtonClickHandlers();
// 
// 	d_botNaviContainer = d_root->getChild("BotNavigationContainer");
// 	d_botNaviCenter = d_root->getChild("BotNavigationContainer/NaviCenterContainer");
// 
// 	d_loginAcceptButton = d_loginContainer->getChild("AcceptButton");
// 	//d_loginAcceptButton->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&LoginState::handleLoginAcceptButtonClicked, this));
// 
// 	d_startButtonClickArea = d_root->getChild("InnerPartContainer/InsideStartClickArea");   
// 	//d_startButtonClickArea->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&LoginState::handleInnerPartStartClickAreaClick, this));
// 
// 	d_navigationTravelIcon = d_root->getChild("BotNavigationContainer/NaviCenterContainer/NavigationIcon");
// 	d_navigationSelectionIcon = d_root->getChild("BotNavigationContainer/NaviCenterContainer/NaviBotSelectionIcon");
// 
// 	//d_navigationSelectionIcon->subscribeEvent(CEGUI::AnimationInstance::EventAnimationStarted, Event::Subscriber(&LoginState::handleNaviSelectionIconAnimStart, this));
// 
// 	_SetupNaviIconAnimationEventHandlers();
// 
// 	_SetupInnerButtonsSubOptionsLabels();
}

void LoginState::_SetupNaviArrowWindows()
{

}

void LoginState::_SetupButtonClickHandlers()
{

}

void LoginState::_SetupNaviIconAnimationEventHandlers()
{

}

void LoginState::_SetupInnerButtonsSubOptionsLabels()
{

}
