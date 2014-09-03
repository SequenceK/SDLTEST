#include "../include/CS.h"
#include "../include/components.h"
#include "../include/entities.h"
#include <cmath>



eId TEST(float x, float y) {
	eId id = CS::createID();
	CS::moveCS[id] = new MoveComponent(x, y, id);
	CS::spriteCS[id] = new SpriteComponent("../data/player.png", CS::moveCS, id);
	CS::controllerCS[id] = new ControllerComponent(CS::moveCS, id);
	//CS::spriteCS[id]->setScale(0.2,0.2);
	CS::spriteCS[id]->setFrame(30,27);
	std::vector<int> f = {1,2,3,4};
	CS::spriteCS[id]->playAnimation(f, 8, true);
	CS::moveCS[id]->maxV = {4,10};
	//CS::moveCS[id]->drag = {0.1,0.1};
	CS::collisionCS[id] = new CollisionComponent(CS::spriteCS,CS::moveCS,id,true);
	CS::collisionCS[id]->moveable = true;
	CS::moveCS[id]->acc.y = 1;
	//CS::moveCS[id]->vel.y = sin(rand()%361)*CS::moveCS[id]->maxV.y;
	//CS::moveCS[id]->vel.x = cos(rand()%361)*CS::moveCS[id]->maxV.x;

	return id;
};
eId mBox(float x, float y){
	eId id = CS::createID();
	CS::moveCS[id] = new MoveComponent(x, y, id);
	CS::spriteCS[id] = new SpriteComponent("../data/hello.png", CS::moveCS, id);
	//CS::moveCS[id]->vel.y = sin(rand()%361)*CS::moveCS[id]->maxV.y;
	//CS::moveCS[id]->vel.x = cos(rand()%361)*CS::moveCS[id]->maxV.x;
	//CS::moveCS[id]->maxV = {3,3};
	//CS::spriteCS[id]->setScale(0.2,0.2);
	CS::collisionCS[id] = new CollisionComponent(CS::spriteCS,CS::moveCS,id,true);
	CS::collisionCS[id]->moveable = false;
	float grid = CS::collisionCS[id]->rect.w;
	CS::moveCS[id]->pos = {floor(x/grid)*grid, floor(y/grid)*grid};
	return id;
}

eId GAMEOVER(){
	eId id = CS::createID();
	CS::moveCS[id] = new MoveComponent(0, 0, id);
	CS::spriteCS[id] = new SpriteComponent("../data/gameover.bmp", CS::moveCS, id);


	return id;
}