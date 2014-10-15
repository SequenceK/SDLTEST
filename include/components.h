#ifndef COMPONENTS_H
#define COMPONENTS_H
//Includes
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <iostream>
//#include "../include/system.h"
//Entities are just an unsinged long
typedef unsigned long eId;



//Enums
enum Direction {UP = 0x0001, TOP = 0x0001, DOWN = 0x0010, FLOOR = 0x0010, LEFT = 0x0100, RIGHT = 0x1000, NONE = 0x000};

//Basic component
class Component {
public:
	Component(eId id);
	eId owner;
};

//Movemenet component
class MoveComponent : public Component {
public:
	Vec2 pos;
	Vec2 deltaPos;
	Vec2 acc;
	Vec2 vel;
	Vec2 drag;
	Vec2 maxV;
	Vec2 terV; // Terminal velocity
	MoveComponent(float xx, float yy, eId id);

	void update();
	void setPosition(float x, float y);
	void resetVel();
};

//Sprite component
class SpriteComponent : public Component {
public:

	struct Animation {
		std::vector<int> frames;
		int currentFrame;
		unsigned int currentIt;
		float speed;
		bool loop;
		bool played;
	};

	struct Color
	{
		Uint8 r;
		Uint8 g;
		Uint8 b;
	};

	Animation currentAnimation;
	bool playingAnimation, render;
	int facing;
	float frameTimer;
	int frameWidth, frameHight;
	Color color;
	SDL_Texture* img;
	SDL_Rect imgRect;
	SDL_Rect clipRect;
	SDL_RendererFlip flip;
	MoveComponent* moveC;
	Vec2 scale;
	Vec2 offset;
	Vec2 texSize;
	SpriteComponent(const std::string &file, std::map<eId, MoveComponent*> &moveMap, eId id);

	void draw();
	void CameraDraw(Vec2 pos, Vec2 size, float zoom, Vec2 gamePos);
	void update();
	void setFrame(int Width, int Height);
	void setScale(float x, float y);
	void setColor(Uint8 r, Uint8 g, Uint8 b);
	void playAnimation(std::vector<int> frames, float speed=1, bool loop=false, bool force=false);
};

class CollisionComponent : public Component {
public:
	
	SDL_Rect rect;
	MoveComponent* moveC;
	SpriteComponent* spriteC;
	std::map<unsigned int, bool> collideGroups;
	std::map<eId, bool> checkedWith;
	std::vector<eId> collidingWith;
	eId collidedWith;
	bool overlaped;
	bool collided;
	bool solid;
	bool moveable;
	bool debugDraw;
	int touching;
	std::vector<int> gridIndex;
	CollisionComponent(int w, int h, std::map<eId, MoveComponent*> &moveMap, eId id, bool s);
	CollisionComponent(std::map<eId, SpriteComponent*> &spriteMap, std::map<eId, MoveComponent*> &moveMap, eId id, bool s);

	void update();
	void updatePosition();
	void getGridIndex(Grid &g);
	void CollideWith(eId e);
};

class ControllerComponent : public Component {
public:
	MoveComponent* moveC;
	ControllerComponent(std::map<eId, MoveComponent*> &moveMap, eId id);

	void eventUpdate(SDL_Event &e);
};

class FuncQComponent : public Component {
public:
	std::vector<void (*)(eId)> functions;
	std::vector<void (*)(eId, SDL_Event&)> eventFunctions;
	FuncQComponent(eId id);
	void add(void (*f)(eId));
	void addEventFunc(void (*f)(eId, SDL_Event&));
	void update();
	void eventUpdate(SDL_Event &e);
};

class PropertiesComponent : public Component {
public:
	std::map<std::string, std::vector<eId>> groups;
	std::map<std::string, eId> entities;
	std::map<std::string, float> fProps;
	std::map<std::string, bool> boolProps;
	std::map<std::string, std::string> stringProps;
	PropertiesComponent(eId id);
};

class Camera : public MoveComponent {
public:
	float zoom;
	Vec2 winPos;
	Vec2 winSize;
	Vec2 size;
	bool active;
	eId followE;
	Camera(float x, float y, float w, float h, float z, eId id);
	void update();
	Vec2 getWorldPos(Vec2 p);
	SDL_Rect getScreenRect(SDL_Rect r);
	void follow(eId mc);
};



#endif