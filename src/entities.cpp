#include "../include/CS.h"
#include "../include/components.h"
#include "../include/entities.h"
#include <cmath>

void playerAnimationUpdate(eId id){
	std::cout << CS::moveCS[id]->vel.y << std::endl;
	if(CS::moveCS[id]->vel.x == 0 && CS::moveCS[id]->vel.y == 0){
		std::vector<int> f = {0};
		CS::spriteCS[id]->playAnimation(f, 8, false, true);
	} else {
		if(CS::moveCS[id]->vel.y > 0){

		}
		std::vector<int> f = {1,2,3,4};
		CS::spriteCS[id]->playAnimation(f, 8, false);
	}
	
}

eId TEST(Vec2<float> const &pos) {
	float x = pos.x;
	float y = pos.y;
	eId id = CS::createEntityID();
	CS::moveCS[id] = new MoveComponent(x, y, id);
	CS::spriteCS[id] = new SpriteComponent("../data/player.png", CS::moveCS, id);
	CS::controllerCS[id] = new ControllerComponent(CS::moveCS, id);
	CS::funcQCS[id] = new FuncQComponent(id);
	void (*pFunc)(eId) = playerAnimationUpdate;
	CS::funcQCS[id]->add(pFunc);
	CS::spriteCS[id]->setScale(2,2);
	CS::spriteCS[id]->setFrame(30,27);
	// std::vector<int> f = {1,2,3,4};
	// CS::spriteCS[id]->playAnimation(f, 8, true);
	CS::moveCS[id]->maxV = {5,15};
	CS::moveCS[id]->drag = {0.5,0.1};
	CS::collisionCS[id] = new CollisionComponent(CS::spriteCS,CS::moveCS,id,true);
	CS::collisionCS[id]->moveable = true;
	CS::moveCS[id]->acc.y = 1;
	CS::collisionCS[id]->debugDraw = true;
	//CS::moveCS[id]->vel.y = sin(rand()%361)*CS::moveCS[id]->maxV.y;
	//CS::moveCS[id]->vel.x = cos(rand()%361)*CS::moveCS[id]->maxV.x;

	return id;
};


eId mBox(float x, float y){
	eId id = CS::createEntityID();
	CS::moveCS[id] = new MoveComponent(x, y, id);
	CS::spriteCS[id] = new SpriteComponent("../data/hello.png", CS::moveCS, id);
	//CS::moveCS[id]->vel.y = sin(rand()%361)*CS::moveCS[id]->maxV.y;
	//CS::moveCS[id]->vel.x = cos(rand()%361)*CS::moveCS[id]->maxV.x;
	//CS::moveCS[id]->maxV = {3,3};
	//CS::spriteCS[id]->setScale(0.2,0.2);
	CS::collisionCS[id] = new CollisionComponent(CS::spriteCS,CS::moveCS,id,true);
	CS::collisionCS[id]->moveable = false;
	CS::collisionCS[id]->debugDraw = true;
	float grid = CS::collisionCS[id]->rect.w;
	CS::moveCS[id]->pos = {floor(x/grid)*grid, floor(y/grid)*grid};
	return id;
}

eId GAMEOVER(){
	eId id = CS::createEntityID();
	CS::moveCS[id] = new MoveComponent(0, 0, id);
	CS::spriteCS[id] = new SpriteComponent("../data/gameover.bmp", CS::moveCS, id);


	return id;
}

eId createCamera(float x, float y){
	eId id = CS::createCameraID();
	CS::cameras[id] = new Camera(x, y, 400, 300, 1, id);

	return id;
}