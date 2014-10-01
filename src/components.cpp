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

FuncQComponent::FuncQComponent(eId id) : Component(id){
};

void FuncQComponent::add(void (*f)(eId)){
	functions.push_back(f);
}

void FuncQComponent::update(){
	for(auto it = functions.begin(); it != functions.end(); ++it){
		(*it)(owner);
	}
}

CollisionComponent::CollisionComponent(std::map<eId, SpriteComponent*> &spriteMap,
		std::map<eId, MoveComponent*> &moveMap, eId id, bool s = true) : Component(id){
	spriteC = spriteMap[id];
	rect = spriteMap[id]->imgRect;
	moveC = moveMap[id];
	debugDraw = false;
	collided = false;
	solid = s;
	moveable = true;
	touching = NONE;
}

void CollisionComponent::update(){
	rect.x = moveC->pos.x;
	rect.y = moveC->pos.y;
	if(spriteC != nullptr){
		rect.w = spriteC->imgRect.w;
		rect.h = spriteC->imgRect.h;
	}
	touching = NONE;
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
				if(CS::getCollisionC(owner)->touching & FLOOR)
					moveC->vel.y = -20;break;	
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
	//imgRect.w *= 0.5;
	//clipRect.w *= 0.5;
	playingAnimation = false;
	flip = SDL_FLIP_NONE;
}

void SpriteComponent::draw(){
	Window::Draw(img, imgRect, &clipRect, 0,0,0, flip);
}

void SpriteComponent::draw(Vec2<float> pos, Vec2<float> size, float zoom, Vec2<float> gamePos){
	SDL_Rect b1, b2, cBounds, area;
	b1 = imgRect;
	b2 = clipRect;
	cBounds = {pos.x, pos.y, size.x, size.y};
	imgRect.x = (imgRect.x - gamePos.x);
	imgRect.y = (imgRect.y - gamePos.y);
	if(SDL_IntersectRect(&imgRect, &cBounds, &area)){
		if(imgRect.x + imgRect.w > pos.x + size.x){
			if((flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_NONE){
				imgRect.w = area.w;
				clipRect.w = area.w/scale.x;
			}
			else if((flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_HORIZONTAL){
				clipRect.x += (imgRect.x - (pos.x+size.x))/scale.x;
				imgRect.w = area.w;
				clipRect.w = area.w/scale.x;
			}
		}
		else if(imgRect.x < pos.x ){
			if((flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_NONE){
				clipRect.x += (pos.x - imgRect.x)/(scale.x);
				imgRect.x = area.x;
				imgRect.w = area.w;
				clipRect.w = area.w/scale.x;
			}
			else if((flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_HORIZONTAL){
				imgRect.x = area.x;
				imgRect.w = area.w;
				clipRect.w = area.w/scale.x;
			}
		}
		if(imgRect.y + imgRect.h > pos.y + size.y){
			if((flip & SDL_FLIP_VERTICAL) == SDL_FLIP_NONE){
				imgRect.h = area.h;
				clipRect.h = area.h/scale.y;
			}
			else if((flip & SDL_FLIP_VERTICAL) == SDL_FLIP_VERTICAL){
				clipRect.y += (imgRect.y - (pos.y+size.y))/(scale.y);
				imgRect.h = area.h;
				clipRect.h = area.h/scale.y;
			}
		}
		else if(imgRect.y < pos.y ){
			if((flip & SDL_FLIP_VERTICAL) == SDL_FLIP_NONE ){
				clipRect.y += (pos.y - imgRect.y)/scale.y;
				imgRect.y = area.y;
				imgRect.h = area.h;
				clipRect.h = area.h/scale.y;
			}
			else if((flip & SDL_FLIP_VERTICAL) == SDL_FLIP_VERTICAL){
				imgRect.y = area.y;
				imgRect.h = area.h;
				clipRect.h = area.h/scale.y;
			}
		}
		imgRect.x *= zoom;
		imgRect.y *= zoom;
		imgRect.h *= zoom;
		imgRect.w *= zoom;
		Window::Draw(img, imgRect, &clipRect, 0,0,0,flip);
	}
	imgRect = b1;
	clipRect = b2;
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

	// imgRect.w *= scale.x;
	// imgRect.h *= scale.y;
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
	maxV = {10,10};
	terV = {10,10};
}

void MoveComponent::update(){
	//vel = {vel.x+acc.x,vel.y+acc.y};
	if(acc.x != 0 && vel.x < terV.x){
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
	if(acc.y != 0 && vel.y < terV.y){
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

//CS init

eId CS::_E_INDEX{0};
eId CS::_C_INDEX{0};
std::map<eId, MoveComponent*> CS::moveCS{};
std::map<eId, SpriteComponent*> CS::spriteCS{};
std::map<eId, ControllerComponent*> CS::controllerCS{};
std::map<eId, CollisionComponent*> CS::collisionCS{};
std::map<eId, FuncQComponent*> CS::funcQCS{};
std::map<const std::string, SDL_Texture*> CS::textures{};
std::map<eId, Camera*> CS::cameras{};
//QuadTree CS::qt(0,0,800,600);

eId CS::createEntityID(){
	_E_INDEX++;
	return _E_INDEX; 
}

eId CS::createCameraID(){
	_C_INDEX++;
	return _C_INDEX;
}

Grid CS::grid(0,0,1000,1000,50);
int nc=0;
void CS::collisionUpdate(){
	CS::grid.clear();

	for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
		it->second->getGridIndex(CS::grid);
	}

	SDL_Rect area;
	int n=0;

	for(auto checking = collisionCS.begin(); checking != collisionCS.end(); checking++){
		//outOfBounds(checking->first, grid.bounds);
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
			if(checkOverlap(checking->first, it->first, &area))
			{
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
	for(auto it = funcQCS.begin(); it != funcQCS.end(); it++){
		it->second->update();
	}
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
	CS::cameraUpdate();
}

void CS::cameraUpdate(){
	for(auto it = cameras.begin(); it != cameras.end(); it++){
			it->second->update();
	}
}


void CS::draw(){

	for(auto it = spriteCS.begin(); it != spriteCS.end(); it++){		
		if(cameras.size()>0){
			for(auto cIt = cameras.begin(); cIt != cameras.end(); cIt++)
				it->second->draw(cIt->second->winPos, cIt->second->size, cIt->second->zoom, cIt->second->pos);
		} else
			it->second->draw();
	}
	for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
		if(it->second->debugDraw)
			for(auto cIt = cameras.begin(); cIt != cameras.end(); cIt++){
				SDL_Rect r = cameras.begin()->second->getScreenRect(it->second->rect);
				Window::DrawRect(&r, 100, 150, 100);
			}
	}
	//grid.draw();
}

void CS::clear(){
	CS::moveCS.clear();
	CS::spriteCS.clear();
	CS::controllerCS.clear();
	CS::collisionCS.clear();
	CS::cameras.clear();

	CS::_E_INDEX = 0;
}

SpriteComponent* CS::getSpriteC(eId e){
	if(spriteCS[e] != nullptr){
		return spriteCS[e];
	} else {
		throw "spirte component not found";
		return nullptr;
	}
}

CollisionComponent* CS::getCollisionC(eId e){
	if(collisionCS[e] != nullptr){
		return collisionCS[e];
	} else {
		throw "collision component not found";
		return nullptr;
	}
}

Camera::Camera(float x, float y, float w, float h, float z, eId id) : MoveComponent(0, 0, id) {
	size = {w, h};
	winPos = {x,y};
	winSize = {w,h};
	zoom = z;
	followC = nullptr;
}

void Camera::follow(MoveComponent* mc){
	followC = mc;
}

Vec2<float> Camera::getWorldPos(Vec2<float> p){
	return {((pos.x + p.x/zoom) - winPos.x), ((pos.y + p.y/zoom) - winPos.y)}; 
}

SDL_Rect Camera::getScreenRect(SDL_Rect r){
	SDL_Rect result;
	result.x = (r.x - pos.x)*zoom;
	result.y = (r.y - pos.y)*zoom;
	result.w = r.w*zoom;
	result.h = r.h*zoom;
	return result;
}

void Camera::update(){
	size.x = winSize.x/zoom;
	size.y = winSize.y/zoom;
	if(followC != nullptr){
		pos.x = followC->pos.x - winSize.x/(2*zoom);
		pos.y = followC->pos.y - winSize.y/(2*zoom);
	}
}