// #include <vector>
// #include <map>
// #include <string>
// #include <memory>
// #include <cmath>
#include <iostream>
#include <SDL2/SDL.h>
#include "../include/window.h"
#include "../include/system.h"
#include "../include/CS.h"
#include "../include/components.h"

ControllerComponent::ControllerComponent(std::map<eId, MoveComponent*> moveMap,
		eId id) : Component(id) {
	moveC = moveMap[id];
}


void ControllerComponent::eventUpdate(SDL_Event &e){
	float SPEED = 3;
	if (e.type == SDL_KEYDOWN){
		switch(e.key.keysym.sym){
			case SDLK_d:
				moveC->vx = SPEED;break;
			case SDLK_a:
				moveC->vx = -SPEED;break;
			case SDLK_w:
				moveC->vy = -SPEED;break;	
			case SDLK_s:
				moveC->vy = SPEED;break;
		}
	}
	if (e.type == SDL_KEYUP){
		switch(e.key.keysym.sym){
			case SDLK_d:
				if(moveC->vx>0)
					moveC->vx -= SPEED;break;
			case SDLK_a:
				if(moveC->vx<0)
					moveC->vx += SPEED;break;
			case SDLK_w:
				if(moveC->vy<0)
					moveC->vy += SPEED;break;
			case SDLK_s:
				if(moveC->vy>0)
					moveC->vy -= SPEED;break;
		}
	}asdf
}

SpriteComponent::SpriteComponent(const std::string &file, 
		std::map<eId, MoveComponent*> moveMap, eId id) : Component(id){
	if(CS::textures[file] == nullptr){
		img = Window::LoadImage(file);
		CS::textures[file] = img;
	}
	else{
		std::cout << "CREATING TEXTURE!" << std::endl;
		img = CS::textures[file];
	}
	SDL_QueryTexture(img, NULL, NULL, &imgRect.w, &imgRect.h);
	texSize.x = imgRect.w;
	texSize.y = imgRect.h;
	moveC = moveMap[id];
	imgRect.x = moveC->x;
	imgRect.y = moveC->y;
	scale = {1,1};
	offset = {0,0};
	clipRect.w = imgRect.w *= scale.x;
	clipRect.h = imgRect.h *= scale.y;
	clipRect.x = 0;
	clipRect.y = 0;
}

void SpriteComponent::draw(){
	Window::Draw(img, imgRect, &clipRect);
}

void SpriteComponent::update(){
	imgRect.x = moveC->x + offset.x;
	imgRect.y = moveC->y + offset.y;
	if(playingAnimation){
		frameTimer += Timer::frame;
		if(frameTimer >= 1000/currentAnimation.speed){
			currentAnimation.currentFrame = currentAnimation.frames[currentAnimation.currentIt];
			currentAnimation.currentIt++;
			int fY = int(currentAnimation.currentFrame/(texSize.x/(clipRect.w/scale.x)));
			clipRect.x = currentAnimation.currentFrame*clipRect.w;
			clipRect.y = fY*clipRect.h;
			if(currentAnimation.currentIt >= currentAnimation.frames.size())
			{
				currentAnimation.currentIt = 0;
				if(!currentAnimation.loop) {
					playingAnimation = false;
				}
			}
			frameTimer = 0;
		}
	}

}

void SpriteComponent::setFrame(int w, int h){
	if(w > imgRect.w || h > imgRect.h){
		return;
	}
	clipRect.w = w;
	clipRect.h = h;
	imgRect.w = clipRect.w*scale.x;
	imgRect.h = clipRect.h*scale.y;
}

void SpriteComponent::setScale(float x, float y){
	int w = clipRect.w, h = clipRect.h;
	scale = {x,y};

	clipRect.w = imgRect.w *= scale.x;
	clipRect.h = imgRect.h *= scale.y;
	setFrame(w, h);
}

void SpriteComponent::playAnimation(std::vector<int> &frames, float speed, bool loop, bool force){
	if(!playingAnimation | force){
		currentAnimation.frames = frames;
		currentAnimation.speed = speed;
		currentAnimation.loop = loop;
		currentAnimation.currentIt = 0;
		frameTimer = 1000/speed;
		playingAnimation = true;
	}
}

MoveComponent::MoveComponent(float xx, float yy, eId id) : Component(id), x(xx), y(yy){
	vx = 0;
	vy = 0;
}

void MoveComponent::update(){
	x += vx;//*Timer::elapsed;
	y += vy;//*Timer::elapsed;
}

Component::Component(eId id) : owner(id){}

eId CS::_INDEX{0};
std::map<eId, MoveComponent*> CS::moveCS{};
std::map<eId, SpriteComponent*> CS::spriteCS{};
std::map<eId, ControllerComponent*> CS::controllerCS{};
std::map<std::string, SDL_Texture*> CS::textures{};

eId CS::createID(){
	_INDEX++;
	return _INDEX; 
}

void CS::eventUpdate(SDL_Event &e){
	for(auto it = controllerCS.begin(); it != controllerCS.end(); it++){
		it->second->eventUpdate(e);
	}
}

void CS::update(){
	for(auto it = moveCS.begin(); it != moveCS.end(); it++){
		it->second->update();
	}
	for(auto it = spriteCS.begin(); it != spriteCS.end(); it++){
		it->second->update();
	}
}

void CS::draw(){
	for(auto it = spriteCS.begin(); it != spriteCS.end(); it++){
		it->second->draw();
	}	
}

