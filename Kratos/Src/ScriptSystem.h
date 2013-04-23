#ifndef SCRIPT_SYSTEM
#define SCRIPT_SYSTEM

#include "Singleton.h"
#include "luna.h"
#include "KratosPrerequisites.h"

namespace Kratos
{

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
		void	Reset() { Shutdown(); Init(); }
		lua_State*	GetLuaState() { return m_pLuaState; }

	public:
		///栈通信
		const STRING		Get_String(int index);
		int					Get_Int(int index);
		float				Get_Float(int index);
		bool				Get_Bool(int index);

		void				Push_Bool(bool b);
		void				Push_Float(float f);
		void				Push_Int(int i);

		///执行脚本
		void				DoFile(const STRING& filename);

		///注册C函数供lua调用
		void				RegisterFunction(const STRING& name);

		//绑定对象到lua全局表(userdata)
		template<class T>
		void	BindObjectToLua(const STRING& nameInLua, T* pObject)
		{
			Luna<T>::userdataType *ud = static_cast<Luna<T>::userdataType*>(lua_newuserdata(m_pLuaState, sizeof(Luna<T>::userdataType)));
			assert(ud);

			int userdata = lua_gettop(m_pLuaState);

			ud->pT = pObject;  // store pointer to object in userdata
			luaL_getmetatable(m_pLuaState, T::className);  // lookup metatable in Lua registry
			lua_setmetatable(m_pLuaState, -2);

			//拷贝栈顶userdata
			lua_pushvalue(m_pLuaState, -1);
			lua_setglobal(m_pLuaState, nameInLua.c_str());

			lua_remove(m_pLuaState, userdata);
		}

		//绑定C++对象到lua的对象数组(table)中,如table Unit[0] = userdata0, Unit[1] = ...
		template<class T>
		void	BindObjectToLua(const STRING& tableName, int index, T* pObject)
		{
			Luna<T>::userdataType *ud = static_cast<Luna<T>::userdataType*>(lua_newuserdata(m_pLuaState, sizeof(Luna<T>::userdataType)));
			assert(ud);

			int userdata = lua_gettop(m_pLuaState);

			ud->pT = pObject;  // store pointer to object in userdata
			luaL_getmetatable(m_pLuaState, T::className);  // lookup metatable in Lua registry
			lua_setmetatable(m_pLuaState, userdata);

			//获取对象table
			lua_getglobal(m_pLuaState, tableName.c_str());
			
			//没有则创建
			if(lua_istable(m_pLuaState, -1) == 0)
			{
				lua_newtable(m_pLuaState);
				lua_pushvalue(m_pLuaState, -1);
				lua_setglobal(m_pLuaState, tableName.c_str());
			}

			int table = lua_gettop(m_pLuaState);
			//将userdata放入表中
			lua_pushnumber(m_pLuaState, index);
			lua_pushvalue(m_pLuaState, userdata);
			lua_settable(m_pLuaState, table);

			// fix.防止堆栈一直增长 [4/23/2013 mavaL]
			lua_remove(m_pLuaState, userdata);
			lua_remove(m_pLuaState, table);
		}

		template<class T>
		void	Push(T p);
		template<>
		void	Push( float p ) { lua_pushnumber(m_pLuaState, p); }
		template<>
		void	Push(int p)		{ lua_pushnumber(m_pLuaState, p); }

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

		//2个参数,无返回值
		template<class T1, class T2>
		void Call(const char* name, T1 p1, T2 p2)
		{
			lua_pushcclosure(m_pLuaState, OnError, 0);
			int errfunc = lua_gettop(m_pLuaState);

			lua_pushstring(m_pLuaState, name);
			lua_gettable(m_pLuaState, LUA_GLOBALSINDEX);

			assert(lua_isfunction(m_pLuaState, -1));
			Push(p1);
			Push(p2);
			lua_pcall(m_pLuaState, 2, 0, errfunc);

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

}


#endif