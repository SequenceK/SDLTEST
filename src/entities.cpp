#include "../include/CS.h"
#include "../include/components.h"
#include "../include/entities.h"


eId TEST(float x, float y) {
	eId id = CS::createID();
	CS::moveCS[id] = new MoveComponent(x, y, id);
	CS::spriteCS[id] = new SpriteComponent("../data/player.png", CS::moveCS, id);
	CS::controllerCS[id] = new ControllerComponent(CS::moveCS, id);
	CS::spriteCS[id]->setScale(2,2);
	CS::spriteCS[id]->setFrame(30,27);
	std::vector<int> f = {1,2,3,4};
	CS::spriteCS[id]->playAnimation(f, 8, true);

	return id;
};

eId mBox(float x, float y){
	eId id = CS::createID();
	CS::moveCS[id] = new MoveComponent(x, y, id);
	CS::spriteCS[id] = new SpriteComponent("../data/hello.bmp", CS::moveCS, id);
	CS::moveCS[id]->vx = -2;
	
	return id;
}

eId GAMEOVER(){
	eId id = CS::createID();
	CS::moveCS[id] = new MoveComponent(0, 0, id);
	CS::spriteCS[id] = new SpriteComponent("../data/gameover.bmp", CS::moveCS, id);


	return id;
}