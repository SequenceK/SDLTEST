
#include <iostream>
#include "../include/selene.h"

using namespace std;

struct Vec2 {
  float x, y;
  Vec2(float xx, float yy) : x(xx), y(yy) {};
};

void write(const char* str){
  cout << str << endl;
}

static int l_write(lua_State* L){
  const char* str = lua_tostring(L, 1);
  write(str);
  return 0;
}

using namespace sel;


int main() {
  State state{true};
  state.Load("../scripts/test.lua");
  if(state["foo"] == 4){
    cout << "SFASD";
  }
  state["write"]("ASDFASD");
  return 0;
  // // Create a new lua state
  // lua_State *L = luaL_newstate();
  // luaL_openlibs(L);
  // // Define a lua function that we can call

  // if (luaL_loadfile(L, "../scripts/test.lua")) {
  //       std::cout<<"Error loading script"<<std::endl;
  // }
  // lua_pushcfunction(L, l_write);
  // lua_setglobal(L, "write");
  // lua_pcall(L, 0, 0, 0);

  // lua_close(L);
}