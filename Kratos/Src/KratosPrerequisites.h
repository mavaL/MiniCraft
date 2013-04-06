#ifndef KratosPrerequisites_h__
#define KratosPrerequisites_h__

#include <OgrePrerequisites.h>

typedef Ogre::Vector3		POS;
typedef Ogre::Quaternion	ORIENT;
typedef	Ogre::Vector3		SCALE;
typedef Ogre::ColourValue	COLOR;
typedef Ogre::String		STRING;
typedef Ogre::Vector3		FLOAT3;
typedef Ogre::Vector2		FLOAT2;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete p; p=NULL; } }
#endif

namespace Kratos
{
	class Scene;
	class SceneSerializer;
	class EffectController;
	class ParticleEffect;
	class CAppStateManager;
	class CInputManager;
	class COgreManager;
	class CGUIManager;
	class aiBehaviorTreeTemplateManager;
	class aiBehaviorTreeNode;
	class aiBehaviorTreeTemplate;
	class aiBlackBoard;
	class aiBehavior;
}

///×¢²áOgreStringInterface
#define DECL_PARAM_COMMAND(name)						\
class Cmd##name : public Ogre::ParamCommand				\
{														\
public:													\
	STRING doGet(const void* target) const;				\
	void doSet(void* target, const STRING& val);		\
};														\
	static Cmd##name	m_sCmd##name;						

#define IMPL_PARAM_COMMAND(classname, cmdname, paramtype)								\
classname::Cmd##cmdname	classname::m_sCmd##cmdname;										\
STRING classname::Cmd##cmdname::doGet(const void* target) const							\
{																						\
	const classname* obj = static_cast<const classname*>(target);						\
	return Ogre::StringConverter::toString(obj->Get##cmdname());						\
}																						\
	\
void classname::Cmd##cmdname::doSet(void* target, const STRING& val)					\
{																						\
	classname* obj = static_cast<classname*>(target);									\
	obj->Set##cmdname(Ogre::StringConverter::parse##paramtype(val));					\
}

#define IMPL_PARAM_COMMAND_STR(classname, cmdname)										\
classname::Cmd##cmdname	classname::m_sCmd##cmdname;										\
STRING classname::Cmd##cmdname::doGet(const void* target) const							\
{																						\
	const classname* obj = static_cast<const classname*>(target);						\
	return obj->Get##cmdname();															\
}																						\
	\
void classname::Cmd##cmdname::doSet(void* target, const STRING& val)					\
{																						\
	classname* obj = static_cast<classname*>(target);									\
	obj->Set##cmdname(val);																\
}


#define GUIMANAGER		Kratos::CGUIManager::GetSingleton()
#define INPUTMANAGER	Kratos::CInputManager::GetSingleton()
#define SCRIPTNAMAGER	Kratos::ScriptSystem::GetSingleton()

#endif // KratosPrerequisites_h__
