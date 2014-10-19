#include "../include/selene.h"
#include "../include/luaState.h"

extern "C" {
	# include "lua.h"
	# include "lauxlib.h"
	# include "lualib.h"
}

struct Bar {
    int x;
    Bar(int x_) : x(x_) {}
    int AddThis(int y) { return x + y; }
};

lua_State* LS::L = nullptr;
int LS::level = 0;

void LS::loadLuaState(const std::string& filename){
	L = luaL_newstate();
	luaL_openlibs(L);
	if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
        std::cout<<"Error: script not loaded ("<<filename<<")"<<std::endl;
        L = 0;
    }
}

void LS::printError(const std::string& variableName, const std::string& reason) {
    std::cout<<"Error: can't get ["<<variableName<<"]. "<<reason<<std::endl;
}

void LS::closeLuaState(){
	if(L) lua_close(L);
}

template <>
bool LS::lua_get(const std::string& variableName) {
    return (bool)lua_toboolean(L, -1);
}
 
template <>
float LS::lua_get(const std::string& variableName) {
    if(!lua_isnumber(L, -1)) {
      printError(variableName, "Not a number");
    }
    return (float)lua_tonumber(L, -1);
}
 
template <>
int LS::lua_get(const std::string& variableName) {
    if(!lua_isnumber(L, -1)) {
      printError(variableName, "Not a number");
    }
    return (int)lua_tonumber(L, -1);
}
 
template <>
std::string LS::lua_get(const std::string& variableName) {
    std::string s = "null";
    if(lua_isstring(L, -1)) {
      s = std::string(lua_tostring(L, -1));
    } else {
      printError(variableName, "Not a string");
    }
    return s;
}

template<typename T>
T LS::lua_get(const std::string& var){
	return 0;
}

template<typename T>
T LS::lua_getdefault(const std::string& var){
	return 0;
}

template<>
std::string LS::lua_getdefault(const std::string& var){
	return "null";
}

template<typename T>
T LS::get(const std::string& variableName) {
  if(!L) {
    printError(variableName, "Script is not loaded");
    return lua_getdefault<T>();
  }
 
  T result;
  if(lua_gettostack(variableName)) { // variable succesfully on top of stack
    result = lua_get<T>(variableName);
  } else {
    result = lua_getdefault<T>();
  }
 
  lua_pop(L, level + 1); // pop all existing elements from stack
  return result;
}
 
bool LS::lua_gettostack(const std::string& variableName) {
  level = 0;
  std::string var = "";
    for(unsigned int i = 0; i < variableName.size(); i++) {
      if(variableName.at(i) == '.') {
        if(level == 0) {
          lua_getglobal(L, var.c_str());
        } else {
          lua_getfield(L, -1, var.c_str());
        }
 
        if(lua_isnil(L, -1)) {
          printError(variableName, var + " is not defined");
          return false;
        } else {
          var = "";
          level++;
        }
      } else {
        var += variableName.at(i);
      }
    }
    if(level == 0) {
      lua_getglobal(L, var.c_str());
    } else {
      lua_getfield(L, -1, var.c_str());
    }
    if(lua_isnil(L, -1)) {
        printError(variableName, var + " is not defined");
        return false;
    }
 
    return true;
}

