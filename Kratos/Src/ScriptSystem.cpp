#include "stdafx.h"
#include "ScriptSystem.h"
#include <windows.h>

namespace Kratos
{
	ScriptSystem::ScriptSystem()
		:m_pLuaState(nullptr)
	{

	}

	void ScriptSystem::Init()
	{
		m_pLuaState = luaL_newstate();
		assert(m_pLuaState);
		luaL_openlibs(m_pLuaState);
	}

	void ScriptSystem::Shutdown()
	{
		lua_close(m_pLuaState);
		m_pLuaState = nullptr;
	}

	int ScriptSystem::OnError( lua_State* L )
	{
		PrintError(L, "%s", lua_tostring(L, -1));
		CallStack(L, 0);

		return 0;
	}

	void ScriptSystem::PrintError( lua_State *L, const char* fmt, ... )
	{
		char text[4096];

		va_list args;
		va_start(args, fmt);
		vsprintf_s(text, fmt, args);
		va_end(args);

		MessageBox(0, text, nullptr, MB_ICONERROR);
	}

	void ScriptSystem::EnumStack( lua_State *L )
	{

	}

	void ScriptSystem::CallStack( lua_State* L, int n )
	{
		lua_Debug ar;
		if(lua_getstack(L, n, &ar) == 1)
		{
			lua_getinfo(L, "nSlu", &ar);

			const char* indent;
			if(n == 0)
			{
				indent = "->\t";
				PrintError(L, "\t<call stack>");
			}
			else
			{
				indent = "\t";
			}

			if(ar.name)
				PrintError(L, "%s%s() : line %d [%s : line %d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
			else
				PrintError(L, "%sunknown : line %d [%s : line %d]", indent, ar.currentline, ar.source, ar.linedefined);

			CallStack(L, n+1);
		}
	}

	void ScriptSystem::DoFile( const STRING& filename )
	{
		lua_pushcclosure(m_pLuaState, OnError, 0);
		int errfunc = lua_gettop(m_pLuaState);

		if(luaL_loadfile(m_pLuaState, filename.c_str()) == 0)
			lua_pcall(m_pLuaState, 0, 1, errfunc);
		else
			PrintError(m_pLuaState, "%s", lua_tostring(m_pLuaState, -1));
	}

	const std::string ScriptSystem::Get_String(int index)
	{
		assert(lua_isstring(m_pLuaState, index));
		return std::string(lua_tostring(m_pLuaState, index));
	}


	int ScriptSystem::Get_Int( int index )
	{
		assert(lua_isnumber(m_pLuaState, index));
		return (int)lua_tonumber(m_pLuaState, index);
	}

	float ScriptSystem::Get_Float( int index )
	{
		assert(lua_isnumber(m_pLuaState, index));
		return (float)lua_tonumber(m_pLuaState, index);
	}

	bool ScriptSystem::Get_Bool( int index )
	{
		assert(lua_isboolean(m_pLuaState, index));
		return lua_toboolean(m_pLuaState, index) == 1 ? true : false;
	}

	void ScriptSystem::Push_Bool( bool b )
	{
		lua_pushboolean(m_pLuaState, b);
	}

	void ScriptSystem::Push_Float( float f )
	{
		lua_pushnumber(m_pLuaState, f);
	}

	void ScriptSystem::Push_Int( int i )
	{
		lua_pushnumber(m_pLuaState, i);
	}
}







