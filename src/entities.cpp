#include "../include/CS.h"
#include "../include/components.h"
#include "../include/entities.h"
#include <cmath>

float shootTimer = 0;
void playerUpdate(eId id){		
	if(CS::moveCS[id]->vel.y > 0){
		CS::spriteCS[id]->playAnimation(std::vector<int>{6}, 8, false, true);
	}
	else if(CS::moveCS[id]->vel.y < 0){
		CS::spriteCS[id]->playAnimation(std::vector<int>{5}, 8, false, true);		
	}
	else {
		if(CS::moveCS[id]->vel.x == 0){
			CS::spriteCS[id]->playAnimation(std::vector<int>{0}, 8, false, true);
		}
		else {
			CS::spriteCS[id]->playAnimation(std::vector<int>{1,2,3,4}, 8, false);
		}
	}
	if(CS::propCS[id]->boolProps["shooting"] && CS::propCS[id]->fProps["fireRate"] <= shootTimer)
	{
		bullet(CS::moveCS[id]->pos, id);
		//CS::propCS[id]->boolProps["shooting"] = false;
		shootTimer = 0;
	}
	shootTimer += Timer::elapsed;
}
bool shooting = false;
bool shot = false;
void playerEventUpdate(eId id, SDL_Event &e){
	float SPEED = 1;
	
	if (e.type == SDL_KEYDOWN){
		switch(e.key.keysym.sym){
			case SDLK_d:
				CS::spriteCS[id]->facing = RIGHT;
				CS::moveCS[id]->acc.x = SPEED;break;
			case SDLK_a:
				CS::spriteCS[id]->facing = LEFT;
				CS::moveCS[id]->acc.x = -SPEED;break;
			case SDLK_w:
				if(CS::collisionCS[id]->touching & FLOOR)
					CS::moveCS[id]->vel.y = -20;break;	
			case SDLK_s:
				CS::moveCS[id]->vel.y = SPEED;break;
			case SDLK_RETURN:
				// if(shot){
					CS::propCS[id]->boolProps["shooting"]=true;
				// 	shot=true;
				// }else{
				// 	CS::propCS[id]->boolProps["shooting"]=false;
				// }
				break;
		}
	}
	if (e.type == SDL_KEYUP){
		switch(e.key.keysym.sym){
			case SDLK_d:
				if(CS::moveCS[id]->acc.x>0)
					CS::moveCS[id]->acc.x -= SPEED;break;
			case SDLK_a:
				if(CS::moveCS[id]->acc.x<0)
					CS::moveCS[id]->acc.x += SPEED;break;
			case SDLK_RETURN:
				// if(shot)
				// 	shot=false;
				CS::propCS[id]->boolProps["shooting"]=false;
				break;
		}
	}
}

bool testCollision(eId owner, eId id, std::string type){
	if(CS::collisionCS[owner]->collided){
		// if(CS::propCS[id] == nullptr)
		// 	return false;
		if(CS::collisionCS[owner]->collidedWith 
					!= CS::propCS[owner]->entities["shooter"]){
			if(CS::propCS[owner]->stringProps["type"] != type){
				return true;
			}
		}
	}
	return false;
}
int f = 1;
void bulletUpdate(eId id){
	// eId firstColl = CS::collisionCS[id]->collidingWith[0];
	// eId shooter = CS::propCS[id]->entities["shooter"];
	const eId shooter = CS::propCS[id]->entities["shooter"];
	int flip = CS::propCS[id]->fProps["flip"];
	CS::moveCS[id]->vel.y = flip*cos(CS::propCS[id]->fProps["duration"]*0.01)*4;// - 3.14/2*flip;
	if(CS::propCS[id]->fProps["duration"] < 0 ){
		//	|| testCollision(id, CS::collisionCS[id]->collidedWith, "bullet")){
		CS::deleteEntity(id);
		return;
	}
	CS::propCS[id]->fProps["duration"] -= Timer::elapsed;
}

eId bullet(Vec2 const &pos, eId const &oid){
	eId id = CS::createEntityID();
	CS::moveCS[id] = new MoveComponent(pos.x,
		pos.y + CS::spriteCS[oid]->imgRect.h/2, id);
	CS::moveCS[id]->drag = {0,0};
	CS::moveCS[id]->maxV = {10, 20};
	CS::moveCS[id]->terV = {10, 20};
	float speed = 1;
	f *= -1;
	if(CS::spriteCS[oid]->facing == RIGHT){
		CS::moveCS[id]->acc.x = speed;
		CS::moveCS[id]->pos.x += CS::spriteCS[oid]->imgRect.w;
	}
	else{
		CS::moveCS[id]->acc.x = -speed;
		CS::moveCS[id]->pos.x -= 20;
	}
	CS::spriteCS[id] = new SpriteComponent("../data/hello.png", CS::moveCS, id);
	CS::spriteCS[id]->setScale(0.5, 0.2);
	CS::spriteCS[id]->setColor(255, 0, 0);
	CS::propCS[id] = new PropertiesComponent(id);
	CS::propCS[id]->entities["shooter"] = oid;
	CS::propCS[id]->fProps["duration"] = 1*1600;
	CS::propCS[id]->fProps["flip"] = 1*f;
	CS::propCS[id]->stringProps["type"] == "bullet";
	CS::funcQCS[id] = new FuncQComponent(id);
	void (*Func)(eId) = bulletUpdate;
	CS::funcQCS[id]->add(Func);
	CS::collisionCS[id] = new CollisionComponent(CS::spriteCS,CS::moveCS,id,false);
	CS::collisionCS[id]->moveable = false;
	//CS::collisionCS[id]->debugDraw = true;
}


eId TEST(Vec2 const &pos) {
	float x = pos.x;
	float y = pos.y;
	eId id = CS::createEntityID();
	CS::moveCS[id] = new MoveComponent(x, y, id);
	CS::spriteCS[id] = new SpriteComponent("../data/player.png", CS::moveCS, id);
	CS::controllerCS[id] = new ControllerComponent(CS::moveCS, id);
	CS::propCS[id] = new PropertiesComponent(id);
	CS::propCS[id]->groups["bullets"] = std::vector<eId>{};
	CS::propCS[id]->boolProps["shooting"] = false;
	CS::propCS[id]->fProps["fireRate"] = 50;
	CS::funcQCS[id] = new FuncQComponent(id);
	void (*pFunc)(eId) = playerUpdate;
	void (*pUpdate)(eId, SDL_Event&) = playerEventUpdate;
	CS::funcQCS[id]->add(pFunc);
	CS::funcQCS[id]->addEventFunc(pUpdate);
	CS::spriteCS[id]->setScale(2,2);
	CS::spriteCS[id]->setFrame(30,27);
	// std::vector<int> f = {1,2,3,4};
	// CS::spriteCS[id]->playAnimation(f, 8, true);
	CS::moveCS[id]->maxV = {5,15};
	CS::moveCS[id]->drag = {0.5,0.1};
	CS::collisionCS[id] = new CollisionComponent(CS::spriteCS,CS::moveCS,id,true);
	CS::collisionCS[id]->moveable = true;
	CS::moveCS[id]->acc.y = 1;
	//CS::collisionCS[id]->debugDraw = true;
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
	//CS::collisionCS[id]->debugDraw = true;
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
	CS::cameras[id] = new Camera(x, y, 800, 600, 1, id);

	return id;
}