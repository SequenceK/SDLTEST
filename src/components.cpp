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
#include "../include/entities.h"

CollisionComponent::CollisionComponent(std::map<eId, SpriteComponent*> &spriteMap,
		std::map<eId, MoveComponent*> &moveMap, eId id, bool s = true) : Component(id){
	spriteC = spriteMap[id];
	rect = spriteMap[id]->imgRect;
	moveC = moveMap[id];
	collided = false;
	solid = s;
	moveable = true;
	touching = NONE;
}

void CollisionComponent::update(){
	touching = NONE;
	rect.x = moveC->pos.x;
	rect.y = moveC->pos.y;
	if(spriteC != nullptr){
		rect.w = spriteC->imgRect.w;
		rect.h = spriteC->imgRect.h;
	}
}

void CollisionComponent::CollideWith(eId e){
	collidingWith.push_back(e);
}

void CollisionComponent::getGridIndex(Grid &g){
	gridIndex = g.getIndex(owner);
}

ControllerComponent::ControllerComponent(std::map<eId, MoveComponent*> &moveMap, eId id) : Component(id){
	moveC = moveMap[id];
}

void ControllerComponent::eventUpdate(SDL_Event &e){
	float SPEED = 1;
	if (e.type == SDL_KEYDOWN){
		switch(e.key.keysym.sym){
			case SDLK_d:
				CS::getSpriteC(owner)->facing = RIGHT;
				moveC->acc.x = SPEED;break;
			case SDLK_a:
				CS::getSpriteC(owner)->facing = LEFT;
				moveC->acc.x = -SPEED;break;
			case SDLK_w:
				moveC->vel.y = -SPEED*100;break;	
			case SDLK_s:
				moveC->vel.y = SPEED;break;
		}
	}
	if (e.type == SDL_KEYUP){
		switch(e.key.keysym.sym){
			case SDLK_d:
				if(moveC->acc.x>0)
					moveC->acc.x -= SPEED;break;
			case SDLK_a:
				if(moveC->acc.x<0)
					moveC->acc.x += SPEED;break;
			// case SDLK_w:
			// 	if(moveC->acc.y<0)
			// 		moveC->acc.y += SPEED;break;
			// case SDLK_s:
			// 	if(moveC->acc.y>0)
			// 		moveC->acc.y -= SPEED;break;
		}
	}
}

SpriteComponent::SpriteComponent(const std::string &file, 
		std::map<eId, MoveComponent*> &moveMap, eId id) : Component(id){
	if(CS::textures[file] == nullptr){

		img = Window::LoadImage(file);
		CS::textures[file] = img;
	}
	else{
		img = CS::textures[file];

	}
	SDL_QueryTexture(img, nullptr, nullptr, &imgRect.w, &imgRect.h);
	
	texSize.x = imgRect.w;
	texSize.y = imgRect.h;
	moveC = moveMap[id];
	imgRect.x = moveC->pos.x;
	imgRect.y = moveC->pos.y;
	scale = {1,1};
	offset = {0,0};
	clipRect.w = imgRect.w *= scale.x;
	clipRect.h = imgRect.h *= scale.y;
	clipRect.x = 0;
	clipRect.y = 0;
	playingAnimation = false;
	flip = SDL_FLIP_NONE;
}

void SpriteComponent::draw(){
	Window::Draw(img, imgRect, &clipRect, 0,0,0, flip);

}

void SpriteComponent::update(){
	imgRect.x = moveC->pos.x + offset.x;
	imgRect.y = moveC->pos.y + offset.y;
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
	if(facing == LEFT)
	{
		flip = SDL_FLIP_HORIZONTAL;
	} else if(facing == RIGHT) {
		flip = SDL_FLIP_NONE;
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

	imgRect.w *= scale.x;
	imgRect.h *= scale.y;
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

MoveComponent::MoveComponent(float xx, float yy, eId id) : Component(id) {
	pos = {xx, yy};
	vel = {0.f,0.f};
	acc = {0.f,0.f};
	drag = {0.3f,0.3f};
	maxV = {8,8};
}

void MoveComponent::update(){
	//vel = {vel.x+acc.x,vel.y+acc.y};
	if(acc.x != 0){
		vel.x += acc.x;
	}
	else if(drag.x != 0){
		if(vel.x - drag.x > 0) {
			vel.x = vel.x - drag.x;
		} else if (vel.x + drag.x < 0) {
			vel.x += drag.x;
		} else {
			vel.x = 0;
		}
	}
	if(acc.y != 0){
		vel.y += acc.y;
	}
	else if(drag.y != 0){
		if(vel.y - drag.y > 0) {
			vel.y = vel.y - drag.y;
		} else if (vel.y + drag.y < 0) {
			vel.y += drag.y;
		} else {
			vel.y = 0;
		}
	}
	if (vel.x > maxV.x || vel.x < -maxV.x){
		vel.x = (vel.x>0)?maxV.x:-maxV.x;
	}
	if (vel.y > maxV.y || vel.y < -maxV.y){
		vel.y = (vel.y>0)?maxV.y:-maxV.y;
	}
	deltaPos = pos;
	pos += vel;
}

void MoveComponent::setPosition(float x, float y){
	//deltaPos = pos;
	pos = {x, y};
}

Component::Component(eId id) : owner(id){}

eId CS::_INDEX{0};
std::map<eId, MoveComponent*> CS::moveCS{};
std::map<eId, SpriteComponent*> CS::spriteCS{};
std::map<eId, ControllerComponent*> CS::controllerCS{};
std::map<eId, CollisionComponent*> CS::collisionCS{};
std::map<const std::string, SDL_Texture*> CS::textures{};
//QuadTree CS::qt(0,0,800,600);

eId CS::createID(){
	_INDEX++;
	return _INDEX; 
}


Grid CS::grid(0,0,800,600,50);
int nc=0;
void CS::collisionUpdate(){
	CS::grid.clear();

	for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
		it->second->getGridIndex(CS::grid);
	}

	SDL_Rect area;
	int n=0;

	for(auto checking = collisionCS.begin(); checking != collisionCS.end(); checking++){
		outOfBounds(checking->first, grid.bounds);
		//if(checking->first == 1)
		if(!checking->second->moveable)
			continue;
		bool collided = false;
		std::map<eId, float> areas;
		float maxArea=0;
		eId maxAreaID;
		for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
			if(it->first == checking->first)
				continue;
			if(!it->second->moveable && !checking->second->moveable)
				continue;
			else if(it->second->moveable && checking->second->moveable)
				continue;
			if(checking->second->collidedWith[it->first])
				continue;

			it->second->collidedWith[checking->first] = true;
			if(CS::grid.overlap(checking->first, it->first, &area))
			{
				// //collide(checking->first, it->first);
				// if(area.w*area.h > maxArea){
				// 	maxAreaID = it->first;
				// 	maxArea = area.w*area.h;
				// }
				areas[it->first] = area.w*area.h;
				collided = true;
			}
			n++;	
		}
		int done=0;
		while(done!=2){
			bool foundM = false;
			done=1;
			for(auto it = areas.begin(); it != areas.end(); it++){
				if(it->second > maxArea){
					maxAreaID = it->first;
					maxArea = it->second;
					foundM = true;
				}
				if(it->second != 0){
					done--;
				}
			}
			if(foundM){
				collide(checking->first, maxAreaID);
				areas[maxAreaID] = 0;
				maxArea = 0;
			}
			done++;
		}
	}
	
	if(nc != n)
	std::cout << n << std::endl;
	nc = n;
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
	for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
		it->second->update();
	}
	for(auto it = spriteCS.begin(); it != spriteCS.end(); it++){
		it->second->update();
	}
	CS::collisionUpdate();
}

void CS::draw(){

	for(auto it = spriteCS.begin(); it != spriteCS.end(); it++){
		it->second->draw();
	}
	for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
		Window::DrawRect(&(it->second->rect), 100, 150, 100);
	}
	//grid.draw();
}

void CS::clear(){
	CS::moveCS.clear();
	CS::spriteCS.clear();
	CS::controllerCS.clear();
	CS::collisionCS.clear();

	CS::_INDEX = 0;
}

SpriteComponent* CS::getSpriteC(eId e){
	if(spriteCS[e] != nullptr){
		return spriteCS[e];
	}
}