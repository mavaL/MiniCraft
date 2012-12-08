#ifndef SCRIPT_SYSTEM
#define SCRIPT_SYSTEM

#include "Singleton.h"
#include "luna.h"


/************************************************************************/
/*								脚本系统类                                */
/************************************************************************/

class ScriptSystem : public CSingleton<ScriptSystem>
{
	ScriptSystem();
	~ScriptSystem(void) {}
	DECLEAR_SINGLETON(ScriptSystem);

public:
	void	Init();
	void	Shutdown();

public:
	///栈通信
	const Ogre::String	Get_String(int index);
	int					Get_Int(int index);
	float				Get_Float(int index);

	void				Push_Bool(bool b);
	void				Push_Float(float f);
	void				Push_Int(int i);

	///执行脚本
	void				DoFile(const Ogre::String& filename);

	//绑定对象到lua全局表(userdata)
	template<class T>
	void	BindObjectToLua(const Ogre::String& nameInLua, T* pObject)
	{
		Luna<T>::userdataType *ud = static_cast<Luna<T>::userdataType*>(lua_newuserdata(m_pLuaState, sizeof(Luna<T>::userdataType)));
		assert(ud);

		ud->pT = pObject;  // store pointer to object in userdata
		luaL_getmetatable(m_pLuaState, T::className);  // lookup metatable in Lua registry
		lua_setmetatable(m_pLuaState, -2);

		//拷贝栈顶userdata
		lua_pushvalue(m_pLuaState, -1);
		lua_setglobal(m_pLuaState, nameInLua.c_str());
	}

	template<class T>
	void	Push(T param);
	template<>
	void Push( float param ) { lua_pushnumber(m_pLuaState, param); }

	///
	///C函数方式调用lua中定义的函数
	///

	//无参,无返回值
	void Call(const char* name)
	{
		lua_pushcclosure(m_pLuaState, OnError, 0);
		int errfunc = lua_gettop(m_pLuaState);

		lua_pushstring(m_pLuaState, name);
		lua_gettable(m_pLuaState, LUA_GLOBALSINDEX);

		assert(lua_isfunction(m_pLuaState, -1));
		lua_pcall(m_pLuaState, 0, 0, errfunc);

		lua_remove(m_pLuaState, errfunc);
	}

	//1个参数,无返回值
	template<class T>
	void Call(const char* name, T p1)
	{
		lua_pushcclosure(m_pLuaState, OnError, 0);
		int errfunc = lua_gettop(m_pLuaState);

		lua_pushstring(m_pLuaState, name);
		lua_gettable(m_pLuaState, LUA_GLOBALSINDEX);

		assert(lua_isfunction(m_pLuaState, -1));
		Push(p1);
		lua_pcall(m_pLuaState, 1, 0, errfunc);

		lua_remove(m_pLuaState, errfunc);
	}

private:
	///debug辅助,取自 Lua_Tinker库
	static int	OnError(lua_State* L);
	static void	PrintError(lua_State *L, const char* fmt, ...);
	void		EnumStack(lua_State *L);
	static void CallStack(lua_State* L, int n);

private:
	lua_State*	m_pLuaState;
};



#endif