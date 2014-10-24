#include "../include/system.h"
#include "../include/window.h"
#include "../include/CS.h"
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <SDL2/SDL.h>

float Timer::start{0};
float Timer::end{0};
float Timer::elapsed{0};
float Timer::frame{1000.f/60.f};
float Timer::slice{0.f};
double Timer::t{0.0};
double Timer::dt{0.01};
double Timer::currentTime{0.0};
double Timer::accumulator{0.0};
float Timer::alpha{0.0};



Vec2 Vec2::operator*(float const& n){
	x *= n;
	y *= n;
	return *this;
}

Vec2 Vec2::operator*=(float const& n){
	x *= n;
	y *= n;
	return *this;
}

Vec2 Vec2::operator+=(Vec2 const& v){
	x += v.x;
	y += v.y;
	return *this;
}

Vec2 Vec2::operator-=(Vec2 const& v){
	x -= v.x;
	y -= v.y;
	return *this;
}

bool Vec2::operator==(Vec2 const& v){
	return (x == v.x) && (y == v.y);
}

float Vec2::getX() const {return x;}
float Vec2::getY() const {return y;}
void Vec2::setX(float xx){x = xx;}
void Vec2::setY(float yy){y = yy;}

bool inCamBounds(Vec2 pos, Camera* c){
	if(pos.x < c->pos.x || pos.x > c->pos.x + c->size.x || pos.y < c->pos.y || pos.y > c->pos.y + c->size.y)
		return false;
	return true;
}

int DELTA = 0;
bool outOfBounds(unsigned long id, SDL_Rect& bounds){
	std::shared_ptr<CollisionComponent> c = CS::collisionCS[id];
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

void Grid::updateBounds(SDL_Rect *r){
	bounds = *r;
}

void Grid::clear(){
	activeIndexes.clear();
}

void Grid::draw(){
	SDL_Rect r;
	for(int i =0; i<((bounds.w*bounds.h)/(cellSize*cellSize));i++){
		r = getRect(i);
		if(activeIndexes[i].size()>0)
			Window::DrawRect(&r, 0, 0, 255);
		else
			Window::DrawRect(&r, 200, 0, 0);
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


std::vector<unsigned long> Grid::getEntities(std::vector<int> indexes){
	std::vector<unsigned long> result{};
	for (auto i = indexes.begin(); i != indexes.end(); ++i)
	{

		result.insert(result.end(), activeIndexes[*i].begin(), activeIndexes[*i].end());
	}
	return result;
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
	// if(r1.x > bounds.x+bounds.w || r1.x +r1.w < bounds.x || r1.y > bounds.y+bounds.h || r1.y +r1.h < bounds.y){
	// 	return std::vector<int>(1, -1);
	// }
	std::vector<int> indexes;
	int index = xstart + ystart*floor(bounds.w/cellSize);
	//std::cout << index << std::endl;
	for(int i=0; i<(yend-ystart+1); i++){
		for(int g=0; g<(xend-xstart+1); g++){
			index = xstart+g+ystart*floor(bounds.w/cellSize) + floor(bounds.w/cellSize)*i;
			activeIndexes[index].push_back(id);
			indexes.push_back(index);
		}

	}

	return indexes;
}



void collide(eId e1, eId e2){
	std::shared_ptr<CollisionComponent> c1 = CS::collisionCS[e1];
	std::shared_ptr<CollisionComponent> c2 = CS::collisionCS[e2];
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
	float absDX1 = (dx1>0)?dx1:(-dx1);
	float absDY1 = (dy1>0)?dy1:-dy1;
	float absDX2 = (dx2>0)?dx2:(-dx2);
	float absDY2 = (dy2>0)?dy2:-dy2;
	r1.x = c1->moveC->pos.x - (dx1>0?dx1:0);
	r2.x = c2->moveC->pos.x - (dx2>0?dx2:0);
	r1.y = c1->moveC->pos.y - (dy1>0?dy1:0);
	r2.y = c2->moveC->pos.y - (dy2>0?dy2:0);
	r1.w = c1->rect.w+absDX1;
	r2.w = c2->rect.w+absDX2;
	r1.h = c1->rect.h+absDY1;
	r2.h = c2->rect.h+absDY2;
	Window::DrawRect(&r1, 255,255,0);
	Window::DrawRect(&r2, 255,255,0);
	// SDL_Rect overlapRect;
	// SDL_IntersectRect(&r1, &r2, &overlapRect);
	float overlapX=0, overlapY=0;
	int BIAS = 4;
	SDL_Rect a;

	if(dx1 != dx2)
	{
		float maxOverlapX = absDX1 + absDX2 + BIAS;
		if((r1.x < r2.x + r2.w) && (r1.x + r1.w > r2.x) 
			&& (r1.y < r2.y + r2.h) && ( r1.y + r1.h > r2.y))
		if(dx1 > dx2)
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
			c1->moveC->pos.x = c1->moveC->pos.x - overlapX;
			c1->moveC->vel.x = 0;
		}
		else if(!c1->moveable){
			c2->moveC->pos.x = c2->moveC->pos.x + overlapX;
			c2->moveC->vel.x = 0;
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

	if(dy1 != dy2)
	{
		float maxOverlapY = absDY1 + absDY2 + BIAS;
		if((r1.x < r2.x + r2.w) && (r1.x + r1.w > r2.x) 
			&& (r1.y < r2.y + r2.h) && ( r1.y + r1.h > r2.y))
		if(dy1 > dy2)
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
			c1->moveC->vel.y = 0;
		}
		else if(!c1->moveable){
			c2->moveC->setPosition(c2->moveC->pos.x, c2->moveC->pos.y+overlapY);
			c2->moveC->vel.y = 0;
		}
	}
	c1->updatePosition();
	c2->updatePosition();
}

void seperateX(eId e1, eId e2){

}

void seperateY(eId e1, eId e2){

}

bool strToBool(std::string str){
	if(str == "False" || str == "0" || str == "false")
		return false;
	else if(str == "True" || str == "1" || str == "true")
		return true;
	else{
		std::cout << "Value recieved not true nor false! returning false" << std::endl;
		return false;
	}
}