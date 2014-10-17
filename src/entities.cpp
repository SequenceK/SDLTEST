#include "../include/CS.h"
#include "../include/components.h"
#include "../include/entities.h"
#include <cmath>

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
	CS::moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(pos.x,
		pos.y + CS::spriteCS[oid]->imgRect.h/2, id));
	CS::moveCS[id]->drag = {0,0};
	CS::moveCS[id]->maxV = {10, 10};
	CS::moveCS[id]->terV = {10, 10};
	float speed = 10;
	f *= -1;
	if(CS::spriteCS[oid]->facing == RIGHT){
		CS::moveCS[id]->acc.x = speed;
		CS::moveCS[id]->pos.x += CS::spriteCS[oid]->imgRect.w;
	}
	else{
		CS::moveCS[id]->acc.x = -speed;
		CS::moveCS[id]->pos.x -= 20;
	}
	CS::spriteCS[id] = std::shared_ptr<SpriteComponent>(new SpriteComponent("../data/hello.png", CS::moveCS, id));
	CS::spriteCS[id]->setScale(0.25, 0.1);
	CS::spriteCS[id]->setColor(255, 100, 255);
	CS::propCS[id] = std::shared_ptr<PropertiesComponent>(new PropertiesComponent(id));
	CS::propCS[id]->entities["shooter"] = oid;
	CS::propCS[id]->fProps["duration"] = 1*1600;
	CS::propCS[id]->fProps["flip"] = 1*f;
	CS::propCS[id]->stringProps["type"] == "bullet";
	CS::funcQCS[id] = std::shared_ptr<FuncQComponent>(new FuncQComponent(id));
	void (*Func)(eId) = bulletUpdate;
	CS::funcQCS[id]->add(Func);
	CS::collisionCS[id] = std::shared_ptr<CollisionComponent>(new CollisionComponent(CS::spriteCS,CS::moveCS,id,false));
	CS::collisionCS[id]->moveable = false;
	//CS::collisionCS[id]->debugDraw = true;
}

float shootTimer = 0;
bool holdFace = false;
void playerUpdate(eId id){		
	if(CS::moveCS[id]->vel.y > 0 && !(CS::collisionCS[id]->touching & FLOOR)){
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
	CS::propCS[id]->boolProps["holdFace"] = false;
}
bool shooting = false;
bool shot = false;

void playerEventUpdate(eId id, SDL_Event &e){
	float SPEED = 1;
	if (e.type == SDL_KEYDOWN){
		switch(e.key.keysym.sym){
			case SDLK_d:
				if(!holdFace)
					CS::spriteCS[id]->facing = RIGHT;
				CS::moveCS[id]->acc.x = SPEED;break;
			case SDLK_a:
				if(!holdFace)
					CS::spriteCS[id]->facing = LEFT;
				CS::moveCS[id]->acc.x = -SPEED;break;
			case SDLK_SPACE:
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
			case SDLK_LSHIFT:
				holdFace = true;
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
			case SDLK_LSHIFT:
				holdFace = false;
				break;
		}
	}
}

eId TEST(Vec2 const &pos) {
	float x = pos.x;
	float y = pos.y;
	eId id = CS::createEntityID();
	CS::moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(x, y, id));
	CS::spriteCS[id] = std::shared_ptr<SpriteComponent>(new SpriteComponent("../data/player.png", CS::moveCS, id));
	CS::collisionCS[id] = std::shared_ptr<CollisionComponent>(new CollisionComponent(CS::spriteCS,CS::moveCS,id,true));
	CS::collisionCS[id]->moveable = true;
	//CS::controllerCS[id] = new ControllerComponent(CS::moveCS, id);
	CS::propCS[id] = std::shared_ptr<PropertiesComponent>(new PropertiesComponent(id));
	CS::propCS[id]->groups["bullets"] = std::vector<eId>{};
	CS::propCS[id]->boolProps["shooting"] = false;
	CS::propCS[id]->fProps["fireRate"] = 100;
	//CS::propCS[id]->entities["edgeChecker"] = edgeChecker(id);
	//CS::propCS[id]->entities["wallChecker"] = wallChecker(id);
	CS::funcQCS[id] = std::shared_ptr<FuncQComponent>(new FuncQComponent(id));;
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
	CS::moveCS[id]->acc.y = 1;
	//CS::collisionCS[id]->debugDraw = true;
	//CS::moveCS[id]->vel.y = sin(rand()%361)*CS::moveCS[id]->maxV.y;
	//CS::moveCS[id]->vel.x = cos(rand()%361)*CS::moveCS[id]->maxV.x;


	return id;
};


void enemyUpdate(eId id){
	// eId echecker = CS::propCS[id]->entities["edgeChecker"];
	// eId wchecker = CS::propCS[id]->entities["wallChecker"];
	// if(!CS::collisionCS[echecker]->overlaped || CS::collisionCS[wchecker]->overlaped){
	// 	CS::moveCS[id]->acc.x *= -1;
	// 	CS::moveCS[id]->vel.x = 0;
	// }
	if(CS::moveCS[id]->vel.x > 0){
		CS::spriteCS[id]->facing = RIGHT;
	}else if(CS::moveCS[id]->vel.x < 0){
		CS::spriteCS[id]->facing = LEFT;
	}
}

void edgeCheckerUpdate(eId id){
	eId parent = CS::propCS[id]->entities["parent"];
	//MoveComponent* pMoveC = CS::moveCS[parent];
	//CollisionComponent* pCollC = CS::collisionCS[parent];
	if(!CS::collisionCS[id]->overlaped){
		CS::moveCS[parent]->acc.x *= -1;
		CS::moveCS[parent]->vel.x = 0;
	}
	if(CS::moveCS[parent]->acc.x > 0){
		CS::moveCS[id]->pos.x = CS::moveCS[parent]->pos.x + CS::collisionCS[parent]->rect.w + CS::moveCS[id]->acc.x + CS::moveCS[id]->vel.x + 1;
	} else if(CS::moveCS[parent]->acc.x < 0){
		CS::moveCS[id]->pos.x = CS::moveCS[parent]->pos.x - CS::collisionCS[id]->rect.w + CS::moveCS[id]->acc.x + CS::moveCS[id]->vel.x - 1;
	}
	CS::moveCS[id]->pos.y = CS::moveCS[parent]->pos.y + CS::collisionCS[parent]->rect.h + CS::moveCS[id]->acc.y + CS::moveCS[id]->vel.y + 1;
}

eId edgeChecker(eId parent){
	Vec2 cPos = {CS::moveCS[parent]->pos.x + CS::collisionCS[parent]->rect.w,
	             CS::moveCS[parent]->pos.y + CS::collisionCS[parent]->rect.h};
	eId id = CS::createEntityID();
	CS::moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(cPos.x, cPos.y, id));
	CS::collisionCS[id] = std::shared_ptr<CollisionComponent>(new CollisionComponent(4, 4, CS::moveCS, id, false));
	CS::collisionCS[id]->debugDraw = true;
	CS::propCS[id] = std::shared_ptr<PropertiesComponent>(new PropertiesComponent(id));
	CS::propCS[id]->entities["parent"] = parent;
	CS::funcQCS[id] = std::shared_ptr<FuncQComponent>(new FuncQComponent(id));;
	void (*cFunc)(eId) = edgeCheckerUpdate; 
	CS::funcQCS[id]->add(cFunc);

	return id;
}

void wallCheckerUpdate(eId id){
	eId parent = CS::propCS[id]->entities["parent"];
	//MoveComponent* pMoveC = CS::moveCS[parent];
	//CollisionComponent* pCollC = CS::collisionCS[parent];
	if(CS::collisionCS[id]->overlaped){
		//CS::moveCS[parent]->acc.x *= -1;
		//CS::moveCS[parent]->vel.x = 0;
		if(CS::collisionCS[parent]->touching & FLOOR)
					CS::moveCS[parent]->vel.y = -20;
	}
	if(CS::moveCS[parent]->vel.x > 0){
		CS::moveCS[id]->pos.x = CS::moveCS[parent]->pos.x + CS::collisionCS[parent]->rect.w + CS::moveCS[id]->acc.x + CS::moveCS[id]->vel.x + 5;
	} else if(CS::moveCS[parent]->vel.x < 0){
		CS::moveCS[id]->pos.x = CS::moveCS[parent]->pos.x - CS::collisionCS[id]->rect.w + CS::moveCS[id]->acc.x + CS::moveCS[id]->vel.x - 5;
	}
	CS::moveCS[id]->pos.y = CS::moveCS[parent]->pos.y + CS::moveCS[id]->acc.y + CS::moveCS[id]->vel.y - 1;
}


eId wallChecker(eId parent){
	eId id = CS::createEntityID();
	Vec2 cPos =
	{CS::moveCS[parent]->pos.x + CS::collisionCS[parent]->rect.w,
		CS::moveCS[parent]->pos.y + CS::collisionCS[parent]->rect.h};
	CS::moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(cPos.x, cPos.y, id));
	CS::collisionCS[id] = std::shared_ptr<CollisionComponent>(new CollisionComponent(1, CS::collisionCS[parent]->rect.h - 1, CS::moveCS, id, false));
	CS::collisionCS[id]->debugDraw = true;
	CS::propCS[id] = std::shared_ptr<PropertiesComponent>(new PropertiesComponent(id));
	CS::propCS[id]->entities["parent"] = parent;
	CS::funcQCS[id] = std::shared_ptr<FuncQComponent>(new FuncQComponent(id));;
	void (*cFunc)(eId) = wallCheckerUpdate; 
	CS::funcQCS[id]->add(cFunc);

	return id;
}

eId ENEMY(Vec2 const &pos) {
	float x = pos.x;
	float y = pos.y;
	eId id = CS::createEntityID();
	CS::moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(x, y, id));
	CS::spriteCS[id] = std::shared_ptr<SpriteComponent>(new SpriteComponent("../data/player.png", CS::moveCS, id));
	CS::spriteCS[id]->setScale(2,2);
	CS::spriteCS[id]->setFrame(30,27);
	std::vector<int> f = {1,2,3,4};
	CS::spriteCS[id]->playAnimation(f, 8, true);

	CS::moveCS[id]->maxV = {5,15};
	CS::moveCS[id]->drag = {0.5,0.1};
	CS::moveCS[id]->acc = {1, 1};

	CS::collisionCS[id] = std::shared_ptr<CollisionComponent>(new CollisionComponent(CS::spriteCS,CS::moveCS,id,true));
	CS::collisionCS[id]->moveable = true;
	//CS::collisionCS[id]->debugDraw = true;

	CS::propCS[id] = std::shared_ptr<PropertiesComponent>(new PropertiesComponent(id));
	CS::propCS[id]->entities["edgeChecker"] = edgeChecker(id);
	CS::propCS[id]->entities["wallChecker"] = wallChecker(id);

	CS::funcQCS[id] = std::shared_ptr<FuncQComponent>(new FuncQComponent(id));;
	void (*eFunc)(eId) = enemyUpdate;
	CS::funcQCS[id]->add(eFunc);
	
	return id;
};


eId mBox(float x, float y){
	eId id = CS::createEntityID();
	CS::moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(x, y, id));
	CS::spriteCS[id] = std::shared_ptr<SpriteComponent>(new SpriteComponent("../data/hello.png", CS::moveCS, id));
	//CS::moveCS[id]->vel.y = sin(rand()%361)*CS::moveCS[id]->maxV.y;
	//CS::moveCS[id]->vel.x = cos(rand()%361)*CS::moveCS[id]->maxV.x;
	//CS::moveCS[id]->maxV = {3,3};
	//CS::spriteCS[id]->setScale(0.1,0.1);
	CS::collisionCS[id] = std::shared_ptr<CollisionComponent>(new CollisionComponent(CS::spriteCS,CS::moveCS,id,true));
	CS::collisionCS[id]->moveable = false;
	//CS::collisionCS[id]->debugDraw = true;
	float grid = CS::spriteCS[id]->imgRect.w;
	CS::moveCS[id]->pos = {floor(x/grid)*grid, floor(y/grid)*grid};
	CS::setGroup(id, "tiles");
	return id;
}

eId GAMEOVER(){
	eId id = CS::createEntityID();
	CS::moveCS[id] = std::shared_ptr<MoveComponent>(new MoveComponent(0, 0, id));
	CS::spriteCS[id] = std::shared_ptr<SpriteComponent>(new SpriteComponent("../data/gameover.bmp", CS::moveCS, id));


	return id;
}

eId createCamera(float x, float y){
	eId id = CS::createCameraID();
	CS::cameras[id] = std::shared_ptr<Camera>(new Camera(x, y, 800, 600, 1, id));

	return id;
}