#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <map>
#include <SDL2/SDL.h>

template<typename T>
struct Vec2 {
	T x, y;
};

class Timer {
public:
	static float start, end, elapsed, frame, slice;
};

class QuadTree
{
public:
	QuadTree(int x, int y, int w, int h, unsigned int d=0);

	void split();
	void insert(unsigned long id);
	void remove(unsigned long id);
	void clear();
	bool overlap(unsigned long id1, unsigned long id2, SDL_Rect* result);
	bool getObject(unsigned long id);
	void draw();
	int getIndex(unsigned long id);
	
private:
	std::vector<QuadTree*> QTs;
	std::vector<unsigned long> objs;
	int depth;
	int maxDepth = 2;
	SDL_Rect bounds;
	unsigned int toSplit = 2;
};

class Grid {
public:
	float gridX, gridY, gridW, gridH, cellSize;
	Grid(int x, int y, int w, int h, int c): gridX(x), gridY(y), gridW(w), gridH(h), cellSize(c){};
	bool overlap(const unsigned long &id1,const unsigned long &id2, SDL_Rect* result);
	std::vector<int> getIndex(unsigned long id);
	void draw();
	SDL_Rect getRect(const int index);
};

bool checkOverlap(unsigned long id1, unsigned long id2, SDL_Rect* result);
bool outOfBounds(unsigned long id);

#endif