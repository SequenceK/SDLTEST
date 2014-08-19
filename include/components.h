#ifndef COMPONENTS_H
#define COMPONENTS_H
//Includes
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <iostream>
#include "../include/system.h"
//Entities are just an unsinged long
typedef unsigned long eId;

//Component map


//Basic component
class Component {
public:
	Component(eId id);
	eId owner;
};

//Movemenet component
class MoveComponent : public Component {
public:
	float x, y, vx, vy;
	MoveComponent(float xx, float yy, eId id);

	void update();
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

	enum Facing { LEFT = false, RIGHT = true };

	Animation currentAnimation;
	bool playingAnimation, facing, render;
	float frameTimer;
	int frameWidth, frameHight;
	SDL_Texture* img;
	SDL_Rect imgRect;
	SDL_Rect clipRect;
	MoveComponent* moveC;
	Vec2<float> scale;
	Vec2<float> offset;
	Vec2<float> texSize;
	SpriteComponent(const std::string &file, std::map<eId, MoveComponent*> moveMap, eId id);

	void draw();
	void update();
	void setFrame(int Width, int Height);
	void setScale(float x, float y);
	void playAnimation(std::vector<int> &frames, float speed=1, bool loop=false, bool force=false);
};

class ControllerComponent : public Component {
public:
	MoveComponent* moveC;
	ControllerComponent(std::map<eId, MoveComponent*> moveMap, eId id);

	void eventUpdate(SDL_Event &e);
};



#endif