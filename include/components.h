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
	Vec2<float> pos;
	Vec2<float> deltaPos;
	Vec2<float> acc;
	Vec2<float> vel;
	Vec2<float> drag;
	Vec2<float> maxV;
	Vec2<float> terV; // Terminal velocity
	MoveComponent(float xx, float yy, eId id);

	void update();
	void setPosition(float x, float y);
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

	Animation currentAnimation;
	bool playingAnimation, render;
	int facing;
	float frameTimer;
	int frameWidth, frameHight;
	SDL_Texture* img;
	SDL_Rect imgRect;
	SDL_Rect clipRect;
	SDL_RendererFlip flip;
	MoveComponent* moveC;
	Vec2<float> scale;
	Vec2<float> offset;
	Vec2<float> texSize;
	SpriteComponent(const std::string &file, std::map<eId, MoveComponent*> &moveMap, eId id);

	void draw();
	void draw(Vec2<float> pos, Vec2<float> size, float zoom, Vec2<float> gamePos);
	void update();
	void setFrame(int Width, int Height);
	void setScale(float x, float y);
	void playAnimation(std::vector<int> &frames, float speed=1, bool loop=false, bool force=false);
};

class CollisionComponent : public Component {
public:

	

	SDL_Rect rect;
	MoveComponent* moveC;
	SpriteComponent* spriteC;
	std::map<unsigned int, bool> collideGroups;
	std::map<eId, bool> collidedWith;
	std::vector<eId> collidingWith;
	bool collided;
	bool solid;
	bool moveable;
	bool debugDraw;
	int touching;
	std::vector<int> gridIndex;
	CollisionComponent(int w, int h, std::map<eId, MoveComponent*> &moveMap, eId id, bool s);
	CollisionComponent(std::map<eId, SpriteComponent*> &spriteMap, std::map<eId, MoveComponent*> &moveMap, eId id, bool s);

	void update();
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
	FuncQComponent(eId id);
	void add(void (*f)(eId));
	void update();
};

class Camera : public MoveComponent {
public:
	float zoom;
	Vec2<float> winPos;
	Vec2<float> winSize;
	Vec2<float> size;
	bool active;
	MoveComponent* followC;
	Camera(float x, float y, float w, float h, float z, eId id);
	void update();
	Vec2<float> getWorldPos(Vec2<float> p);
	SDL_Rect getScreenRect(SDL_Rect r);
	void follow(MoveComponent* mc);
};

#endif