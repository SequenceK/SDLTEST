#include "../include/system.h"
#include "../include/window.h"
#include "../include/CS.h"
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>

float Timer::start{0};
float Timer::end{0};
float Timer::elapsed{0};
float Timer::frame{1000.f/60.f};
float Timer::slice{0.f};

bool checkOverlap(unsigned long id1, unsigned long id2, SDL_Rect* result){
	SDL_Rect r1, r2;
	r1 = CS::collisionCS[id1]->rect;
	r2 = CS::collisionCS[id2]->rect;
	if(SDL_IntersectRect(&r1, &r2, result))
			return true;
	return false;

}

void Grid::draw(){
	SDL_Rect r;
	for(int i =0; i<(gridW/cellSize)*(gridH/cellSize);i++)
		r = getRect(i);
		Window::DrawRect(&r, 225, 255, 255);
}

SDL_Rect Grid::getRect(const int index){
	SDL_Rect r;
	r.x = cellSize*index;
	r.y = (index/(gridW/cellSize))*cellSize;
	r.w = cellSize;
	r.h = cellSize;
	return r;
}

bool Grid::overlap(const unsigned long &id1,const unsigned long &id2, SDL_Rect* result){
	std::vector<int> v1 = CS::collisionCS[id1]->gridIndex;
	if(v1[0]==-1)
		return false;
	std::vector<int> v2 = CS::collisionCS[id2]->gridIndex;
	if(v2[0]==-1)
		return false;
	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());
	std::vector<int> v3(v1.size()+v2.size(), -1);
	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin());
	if(v3[0]!=-1){
		if(checkOverlap(id1, id2, result))
			return true;
	}
	else{
		return false;
	}

	return false;

}

std::vector<int> Grid::getIndex(unsigned long id){
	SDL_Rect r1 = CS::collisionCS[id]->rect;
	int xstart = int((floor(r1.x/cellSize)));
	if(r1.x < gridX)
		xstart = 0;
	int xend = int((floor((r1.x+r1.w)/cellSize)));
	int ystart = int((floor(r1.y/cellSize)));
	if(r1.y < gridY)
		ystart = 0;
	int yend = int((floor((r1.y+r1.h)/cellSize)));
	if(r1.x > gridX+gridW || r1.x +r1.w < gridX || r1.y > gridY+gridH || r1.y +r1.h < gridY){
		return std::vector<int>(1, -1);
	}
	std::vector<int> indexes;
	int index = xstart + ystart*floor(gridW/cellSize);
	//std::cout << index << std::endl;
	for(int i=0; i<(yend-ystart+1); i++){
		for(int g=0; g<(xend-xstart+1); g++){
			index = xstart+g+ystart*floor(gridW/cellSize) + floor(gridW/cellSize)*i;
			indexes.push_back(index);
		}

	}

	return indexes;
}

QuadTree::QuadTree(int x, int y, int w, int h, unsigned int d): QTs(4, nullptr){
	bounds.x = x;
	bounds.y = y;
	bounds.w = w;
	bounds.h = h;
	depth = d;
}

void QuadTree::split(){
		QTs[0] = new QuadTree(bounds.x, bounds.y, bounds.w/2, bounds.h/2, depth+1);
		QTs[1] = new QuadTree(bounds.x+bounds.w/2, bounds.y, bounds.w/2, bounds.h/2, depth+1);
		QTs[2] = new QuadTree(bounds.x, bounds.y+bounds.h/2, bounds.w/2, bounds.h/2, depth+1);
		QTs[3] = new QuadTree(bounds.x+bounds.w/2, bounds.y+bounds.h/2, bounds.w/2, bounds.h/2, depth+1);

};

int QuadTree::getIndex(unsigned long id){
	int index = -1;
	SDL_Rect* r = &CS::collisionCS[id]->rect;
	if(r->x < (bounds.x + bounds.w/2) && (r->x + r->w) < (bounds.x + bounds.w/2)){
		if(r->y < bounds.y + bounds.h/2 && r->y + r->h < bounds.y + bounds.h/2)
			index = 0;
		else if(r->y < bounds.y + bounds.h && r->y + r->h < bounds.y + bounds.h && r->y > bounds.y + bounds.h/2)
			index = 2;
			
	}
	else if(r->x < bounds.x + bounds.w && r->x + r->w < bounds.x + bounds.w && r->x > (bounds.x + bounds.w/2)){
		if(r->y < bounds.y + bounds.h/2 && r->y + r->h < bounds.y + bounds.h/2)
			index = 1;
		else if(r->y < bounds.y + bounds.h && r->y + r->h < bounds.y + bounds.h && r->y > bounds.y + bounds.h/2)
			index = 3;
	}
	if((r->x < bounds.x && r->x + r->w < bounds.x) || (r->x > bounds.x + bounds.w))
		index = -2;
	if((r->y < bounds.y && r->y + r->h < bounds.y) || (r->y > bounds.y + bounds.h))
		index = -2;

	return index;
}

void QuadTree::insert(unsigned long id){
	int index = getIndex(id);
	if(QTs[0] != nullptr){
		if(index != -1){
				QTs[index]->insert(id);
				return;
		}
	}
	if(index != -2)
	objs.push_back(id);
	if(objs.size() > toSplit && depth < maxDepth){
		if(QTs[0] == nullptr){
			split();
		}

		for(unsigned int i = 0; i < objs.size(); i++){
			index = getIndex(objs[i]);
			if(index > -1 && objs[i] != 0){
				QTs[index]->insert(objs[i]);
				objs.erase(objs.begin()+i);
				--i;
			}
		}
	}
}

void QuadTree::remove(unsigned long id){
	for(unsigned int i = 0; i < objs.size(); i++){
		if(objs[i] == id){
			objs.erase(objs.begin()+i);
			return;
		}
	}
	if(QTs[0] != nullptr){
		for (unsigned int i=0;i<QTs.size();i++){
			QTs[i]->remove(id);
		}
	}
}

void QuadTree::clear(){
	if(QTs[0] != nullptr){
		for (unsigned int i=0;i<QTs.size();i++){
			QTs[i]->clear();
			delete QTs[i];
		}
	}
	objs.clear();
}

bool QuadTree::overlap(unsigned long id1, unsigned long id2, SDL_Rect* result){
	
	if(!getObject(id1)) return false;
	if(!getObject(id2)) return false;
	std::cout << "ASFD" << std::endl;
	SDL_Rect *r1 = &CS::collisionCS[id1]->rect, *r2 = &CS::collisionCS[id2]->rect;
	if(SDL_IntersectRect(r1, r2, result)){
		std::cout << r1->x << std::endl;
		return true;
	}
	else{
		return false;

	}
}

bool QuadTree::getObject(unsigned long id){
	std::cout << objs.size() << std::endl;
	for(unsigned int i = 0; i < objs.size(); i++){

		if(objs[i] == id){
			return true;
		}
	}
	if(QTs[0] != nullptr) {
		std::cout << QTs[0] << std::endl;
			if(QTs[0]->getObject(id) || QTs[1]->getObject(id) || QTs[2]->getObject(id) || QTs[3]->getObject(id))
				return true;
	}
	return false;
}

void QuadTree::draw(){
	if(QTs[0] != nullptr) {
		for (unsigned int i=0;i<QTs.size();i++){
			QTs[i]->draw();
		}
	}
	Window::DrawRect(&bounds, 225, 255, 255);
}