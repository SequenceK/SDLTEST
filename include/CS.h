#ifndef CS_H
#define CS_H
//Includes
#include <vector>
#include <map>
#include <iostream>
#include "../include/system.h"
#include "../include/components.h"

class CS {
public:
	static std::map<eId, MoveComponent*> moveCS;
	static std::map<eId, SpriteComponent*> spriteCS;
	static std::map<eId, ControllerComponent*> controllerCS;
	static std::map<eId, CollisionComponent*> collisionCS;
	static std::map<const std::string, SDL_Texture*> textures;
	static QuadTree qt;

	static eId createID();

	static void eventUpdate(SDL_Event &e);
	static void update();
	static void draw();
	static void collisionUpdate();
	static void clear();

	static eId _INDEX;
};



#endif