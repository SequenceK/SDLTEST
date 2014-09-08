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

template<>
Vec2<float> Vec2<float>::operator*(float const& n){
	x *= n;
	y *= n;
	return *this;
}

template<>
Vec2<float> Vec2<float>::operator*=(float const& n){
	x *= n;
	y *= n;
	return *this;
}
template<>
Vec2<float> Vec2<float>::operator+=(Vec2<float> const& v){
	x += v.x;
	y += v.y;
	return *this;
}
template<>
Vec2<float> Vec2<float>::operator-=(Vec2<float> const& v){
	x -= v.x;
	y -= v.y;
	return *this;
}

int DELTA = 0;
bool outOfBounds(unsigned long id, SDL_Rect& bounds){
	CollisionComponent* c = CS::collisionCS[id];
	if(c->rect.x < bounds.x || c->rect.x + c->rect.w > bounds.x + bounds.w || c->rect.y < bounds.y || c->rect.y + c->rect.h > bounds.y + bounds.h){
		if(c->rect.x < bounds.x){
			c->moveC->pos.x = bounds.x+DELTA;
			c->touching |= RIGHT;
		} else if( c->rect.x + c->rect.w > bounds.x + bounds.w) {
			c->moveC->pos.x = bounds.x + bounds.w - c->rect.w-DELTA;
			c->touching |= LEFT;
		}
		if(c->rect.y < bounds.y){
			c->moveC->pos.y = bounds.y+DELTA; 
			c->touching |= TOP;
		} else if( c->rect.y + c->rect.h > bounds.y + bounds.h) {
			c->moveC->pos.y = bounds.y + bounds.h - c->rect.h-DELTA;
			c->touching |= FLOOR;
		}
		return true;
	}
	return false;
}

bool checkOverlap(unsigned long id1, unsigned long id2, SDL_Rect* result){
	SDL_Rect r1, r2;
	r1 = CS::collisionCS[id1]->rect;
	r2 = CS::collisionCS[id2]->rect;
	if(SDL_IntersectRect(&r1, &r2, result))
			return true;
	return false;

}

void Grid::clear(){
	activeIndexes.clear();
}

void Grid::draw(){
	SDL_Rect r;
	for(int i =0; i<((bounds.w*bounds.h)/(cellSize*cellSize));i++){
		r = getRect(i);
		if(!activeIndexes[i])
			Window::DrawRect(&r, 0, 0, 100);
		else
			Window::DrawRect(&r, 255, 0, 0);
	}
}

SDL_Rect Grid::getRect(const int index){
	SDL_Rect r;
	r.x = cellSize*index - bounds.w*floor(index/(bounds.w/cellSize));
	r.y = floor(index/(bounds.w/cellSize))*cellSize;
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
	if(r1.x < bounds.x)
		xstart = 0;
	int xend = int((floor((r1.x+r1.w)/cellSize)));
	int ystart = int((floor(r1.y/cellSize)));
	if(r1.y < bounds.y)
		ystart = 0;
	int yend = int((floor((r1.y+r1.h)/cellSize)));
	if(r1.x > bounds.x+bounds.w || r1.x +r1.w < bounds.x || r1.y > bounds.y+bounds.h || r1.y +r1.h < bounds.y){
		return std::vector<int>(1, -1);
	}
	std::vector<int> indexes;
	int index = xstart + ystart*floor(bounds.w/cellSize);
	//std::cout << index << std::endl;
	for(int i=0; i<(yend-ystart+1); i++){
		for(int g=0; g<(xend-xstart+1); g++){
			index = xstart+g+ystart*floor(bounds.w/cellSize) + floor(bounds.w/cellSize)*i;
			activeIndexes[index] = true;
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
			if(QTs[0]->getObject(id) || QTs[1]->getObject(id) 
					|| QTs[2]->getObject(id) || QTs[3]->getObject(id))
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

void collide(eId e1, eId e2){
	CollisionComponent* c1 = CS::collisionCS[e1];
	CollisionComponent* c2 = CS::collisionCS[e2];
	if(!c1->moveable && !c2->moveable)
	{
		return;
	}

	//create 2 rects. They are used to determine which direction the collision has taken place.
	SDL_Rect r1, r2;
	float dx1 = -c1->moveC->deltaPos.x + c1->moveC->pos.x;
	float dy1 = -c1->moveC->deltaPos.y + c1->moveC->pos.y;
	float dx2 = -c2->moveC->deltaPos.x + c2->moveC->pos.x;
	float dy2 = -c2->moveC->deltaPos.y + c2->moveC->pos.y;
	float absDX1 = (dx1>0)?dx1:-dx1;
	float absDY1 = (dy1>0)?dy1:-dy1;
	float absDX2 = (dx2>0)?dx2:-dx2;
	float absDY2 = (dy2>0)?dy2:-dy2;
	r1.x = c1->moveC->pos.x - (dx1>0?dx1:0);
	r2.x = c2->moveC->pos.x - (dx2>0?dx2:0);
	r1.y = c1->moveC->pos.y - (dy1>0?dy1:0);
	r2.y = c2->moveC->pos.y - (dy2>0?dy2:0);
	r1.w = c1->rect.w+absDX1;
	r2.w = c2->rect.w+absDX2;
	r1.h = c1->rect.h+absDY1;
	r2.h = c2->rect.h+absDY2;

	// SDL_Rect overlapRect;
	// SDL_IntersectRect(&r1, &r2, &overlapRect);
	float overlapX=0, overlapY=0;
	int BIAS = 4;
	SDL_Rect a;

	if(r1.x != r2.x)
	{
		float maxOverlapX = absDX1 + absDX2 + BIAS;
		if((r1.x < r2.x + r2.w) && (r1.x + r1.w > r2.x) 
			&& (r1.y < r2.y + r2.h) && ( r1.y + r1.h > r2.y))
		if(r2.x > r1.x)
		{
			overlapX = r1.x + r1.w - r2.x;
			if(overlapX > maxOverlapX)
				overlapX = 0;
			else{
				c1->touching |= LEFT;
				c2->touching |= RIGHT;
			}
		} else  {
			overlapX = -(r2.x + r2.w - r1.x);
			if(-overlapX > maxOverlapX)
				overlapX = 0;
			else{
				c1->touching |= RIGHT;
				c2->touching |= LEFT;
			}
		}
		//std::cout << maxOverlapX << " " << overlapX << std::endl;
	}

	if(overlapX != 0){
		if(!c2->moveable){
			c1->moveC->setPosition(c1->moveC->pos.x - overlapX, c1->moveC->pos.y);
			c1->moveC->vel.x = c2->moveC->vel.x ;//- c1->moveC->acc.x;
		}
		else if(!c1->moveable){
			c2->moveC->setPosition(c2->moveC->pos.x + overlapX, c2->moveC->pos.y);
			c2->moveC->vel.x = c1->moveC->vel.x ;//- c2->moveC->acc.x;
		}
	}
	r1.x = c1->moveC->pos.x - (dx1>0?dx1:0);
	r2.x = c2->moveC->pos.x - (dx2>0?dx2:0);
	r1.y = c1->moveC->pos.y - (dy1>0?dy1:0);
	r2.y = c2->moveC->pos.y - (dy2>0?dy2:0);
	r1.w = c1->rect.w+absDX1;
	r2.w = c2->rect.w+absDX2;
	r1.h = c1->rect.h+absDY1;
	r2.h = c2->rect.h+absDY2;
	if(r1.y != r2.y)
	{
		float maxOverlapY = absDY1 + absDY2 + BIAS;
		if((r1.x < r2.x + r2.w) && (r1.x + r1.w > r2.x) 
			&& (r1.y < r2.y + r2.h) && ( r1.y + r1.h > r2.y))
		if(r2.y > r1.y)
		{
			overlapY = r1.y + r1.h - r2.y;
			if(overlapY > maxOverlapY)
				overlapY = 0;
			else{
				c1->touching |= FLOOR;
				c2->touching |= TOP;
			}
		} else  {
			overlapY = -(r2.y + r2.h - r1.y);
			if(-overlapY > maxOverlapY)
				overlapY = 0;
			else{
				c1->touching |= TOP;
				c2->touching |= FLOOR;
			}
		}
		//std::cout << absDY2 << "<-2 1->" << absDY1 << std::endl;
	}

	if(overlapY != 0){
		if(!c2->moveable){
			c1->moveC->setPosition(c1->moveC->pos.x, c1->moveC->pos.y-overlapY);
			c1->moveC->vel.y = c2->moveC->vel.y;// - c1->moveC->acc.y;
		}
		else if(!c1->moveable){
			c2->moveC->setPosition(c2->moveC->pos.x, c2->moveC->pos.y+overlapY);
			c2->moveC->vel.y = c1->moveC->vel.y;// - c2->moveC->acc.y;
		}
	}
	Window::DrawRect(&r1, 255,255,0);
	Window::DrawRect(&r2, 255,255,0);
	//c1->moveC->pos.y -= overlapY;
	//c2->moveC->pos.x = ;
	// if(r1.x < r2.x){
	// 	//SIDE TOUCHING CODE HERE
	// 	//c1 is touching left side
	// 	//c2 is touching right side
	// 	overlapX = overlapRect.w;
	// 	c1->moveC->pos.x = 
		
	// } else if(r1.x > r2.x) {
	// 	//opposite
	// 	overlapX = -overlapRect.w;
	// 	c1->touching |= RIGHT;
	// 	c2->touching |= LEFT;
	// }
	// if(r1.y < r2.y) {
	// 	//UP AND DOWN TOUCHING CODE
	// 	//c1 is touching down or floor
	// 	//c2 is touching up or ceiling
	// 	overlapY = overlapRect.h;
	// 	c1->touching |= FLOOR;
	// 	c1->touching |= TOP;
	// } else if (r1.y > r2.y) {
	// 	//opposite
	// 	overlapY = -overlapRect.h;
	// 	c1->touching |= TOP;
	// 	c1->touching |= FLOOR;
	// }
	// c1->moveC->pos = c1->moveC->deltaPos;
	// c2->moveC->pos = c2->moveC->deltaPos;
	// Vec2<float> placeholderVel = c1->moveC->vel;
		
}

void seperateX(eId e1, eId e2){

}

void seperateY(eId e1, eId e2){

}