#include <iostream>
#include <SDL2/SDL.h>
#include <memory>
#include "../include/window.h"
#include "../include/system.h"
#include "../include/CS.h"
#include "../include/components.h"
#include "../include/entities.h"




//CS INIT//
eId CS::_E_INDEX{0};
eId CS::_C_INDEX{0};
Vec2 CS::worldMax{0,0};
Vec2 CS::worldMin{0,0};
std::map<eId, std::shared_ptr<MoveComponent>> moveCS{};
std::map<eId, std::shared_ptr<SpriteComponent>> CS::spriteCS{};
std::map<eId, std::shared_ptr<ControllerComponent>> CS::controllerCS{};
std::map<eId, std::shared_ptr<CollisionComponent>> CS::collisionCS{};
std::map<eId, std::shared_ptr<PropertiesComponent>> CS::propCS{};
std::map<eId, std::shared_ptr<FuncQComponent>> CS::funcQCS{};
std::map<const std::string, std::vector<eId>> CS::groups{};
std::map<const std::string, SDL_Texture*> CS::textures{};
std::map<eId, std::shared_ptr<Camera>> CS::cameras{};
std::vector<eId> CS::deletedEntities{};
//QuadTree CS::qt(0,0,800,600);
//CS INIT//

//CS CREATE//
eId CS::createEntityID(){
	if(deletedEntities.empty()){
		_E_INDEX++;
		return _E_INDEX; 
	}
	eId id = deletedEntities.back();
	deletedEntities.pop_back();
	return id;
}

eId CS::createCameraID(){
	_C_INDEX++;
	return _C_INDEX;
}

void CS::createMoveC(float xx, float yy, const eId &id){
	moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(xx, yy, id));
}
//CS CREATE//


void CS::clear(){
	moveCS.clear();
	CS::spriteCS.clear();
	CS::controllerCS.clear();
	CS::collisionCS.clear();
	CS::funcQCS.clear();
	CS::propCS.clear();
	CS::cameras.clear();
	deletedEntities.clear();
	CS::groups.clear();
	CS::_E_INDEX = 0;
	CS::_C_INDEX = 0;
	worldMin = {0,0};
	worldMax = {0,0};
}

//Grid CS::grid(0,0,1000,1000,50);
int nc=0;
void CS::collisionUpdate(){
	SDL_Rect worldArea;
	worldArea.x = worldMin.x;
	worldArea.y = worldMin.y;
	worldArea.w = worldMax.x - worldMin.x;
	worldArea.h = worldMax.y - worldMin.y;
	QuadTree qt{worldArea.x, worldArea.y, worldArea.w, worldArea.h};
	for(auto checking = collisionCS.begin(); checking != collisionCS.end(); checking++){
		qt.insert(checking->first);
	}
	SDL_Rect area;
	int n=0;
	for(auto checking = collisionCS.begin(); checking != collisionCS.end(); checking++){
		if(!checking->second->moveable)
			continue;
		std::map<eId, float> areas;
		float maxArea=0;
		eId maxAreaID;
		for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
			if(it->first == checking->first)
				continue;
			if(checking->second->checkedWith[it->first])
				continue;

			it->second->checkedWith[checking->first] = true;
			if(qt.overlap(checking->first, it->first, &area))
			{
				checking->second->overlaped = true;
				it->second->overlaped = true;
				if(checking->second->solid && it->second->solid)
				{
					checking->second->collided = true;
					it->second->collided = true;
					areas[it->first] = area.w*area.h;
				}
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
				checking->second->collidedWith = maxAreaID;
				CS::collisionCS[maxAreaID]->collidedWith = checking->first;
				areas[maxAreaID] = 0;
				maxArea = 0;
			}
			done++;
		}
	}
	qt.draw();
	qt.clear();
	Window::DrawRect(&worldArea, 255, 0, 0);
	if(nc != n)
	std::cout << n << std::endl;
	nc = n;
}

void CS::eventUpdate(SDL_Event &e){
	for(auto it = funcQCS.begin(); it != funcQCS.end(); it++){
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
	CS::collisionUpdate();
	for(auto it = collisionCS.begin(); it != collisionCS.end(); it++){
		it->second->postUpdate();
	}
	for(auto it = spriteCS.begin(); it != spriteCS.end(); it++){
		it->second->update();
	}
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
				it->second->CameraDraw(cIt->second->winPos, cIt->second->size, cIt->second->zoom, cIt->second->pos);
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

void CS::deleteEntity(eId id){
	moveCS.erase(id);
	CS::spriteCS.erase(id);
	CS::controllerCS.erase(id);
	CS::collisionCS.erase(id);
	CS::funcQCS.erase(id);
	CS::propCS.erase(id);
	CS::deletedEntities.push_back(id);
}

void CS::setGroup(const eId &id, const std::string &s){
	CS::groups[s].push_back(id);
}



std::shared_ptr<SpriteComponent> CS::getSpriteC(eId e){
	if(spriteCS[e] != nullptr){
		return spriteCS[e];
	} else {
		throw "spirte component not found";
		return nullptr;
	}
}



std::shared_ptr<CollisionComponent> CS::getCollisionC(eId e){
	if(collisionCS[e] != nullptr){
		return collisionCS[e];
	} else {
		throw "collision component not found";
		return nullptr;
	}
}