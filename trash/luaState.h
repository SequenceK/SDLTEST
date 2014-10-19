#ifndef LUASTATE_H
#define LUASTATE_H

extern "C" {
	# include "lua.h"
	# include "lauxlib.h"
	# include "lualib.h"
}

struct LS
{
	static lua_State* L;
	static int level;
	static void loadLuaState(const std::string& filename);
	static void loadfile(const std::string& filename);
	static void printError(const std::string& variableName, const std::string& reason);
	static void closeLuaState();


	template<typename T>
	static T get(const std::string& var);
	static bool lua_gettostack(const std::string& var);
	template<typename T>
	static T lua_get(const std::string& var);
	template<typename T>
	static T lua_getdefault(const std::string& var);
};

#endif