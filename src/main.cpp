
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>
#include <SDL2/SDL.h>
#include "../include/window.h"
#include "../include/cleanup.h"
#include "../include/CS.h"
#include "../include/system.h"
#include "../include/entities.h"
#include "../include/SimpleIni.h"


using namespace std;

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

int main(int argc, char **argv){
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile("../config.ini");
	const char* title = ini.GetValue("window","title",NULL);
	bool fullscreen = strToBool(ini.GetValue("window","fullscreen",NULL));
	try {
		Window::Init(title, fullscreen);
	}
	catch (const std::runtime_error &e){
		std::cout << e.what() << std::endl;
		Window::Quit();
		return -1;
	}
	SDL_Event e;
	bool quit=false;
	int controll = 1;
	// SDL_Rect r;
	// SDL_Texture* t;
	// t = Window::RenderText("FPS: " + std::to_string(1000/Timer::elapsed), "../data/fonts/PressSTart2P.ttf",{255,255,255,255}, 8);
	// SDL_QueryTexture(t, nullptr, nullptr, &r.w, &r.h);
	// r.x=1;r.y=1;
	eId c = createCamera(0,0);
	SDL_Rect textrect = {0,0,400,100};
	//eId c2 = createCamera(400,300);
	SDL_StartTextInput();
	//SDL_SetTextInputRect(&textrect);
	std::string text = "";
	while(!quit){
		text = "";
		auto timePoint1(chrono::high_resolution_clock::now());

		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT)
				quit = true;
			if(e.type == SDL_MOUSEWHEEL){
				CS::cameras[c]->zoom += 0.05*e.wheel.y;
			}
			if(e.type == SDL_MOUSEBUTTONDOWN){
				if(e.button.button == SDL_BUTTON_LEFT){
					Vec2 p = CS::cameras[c]->getWorldPos({e.button.x, e.button.y});
					mBox(p.x, p.y);
				}
				if(e.button.button == SDL_BUTTON_RIGHT){
					if(controll < 0)
							ENEMY(CS::cameras[c]->getWorldPos({e.button.x, e.button.y}));
					else
						CS::cameras[c]->follow(
							TEST(CS::cameras[c]->getWorldPos({e.button.x, e.button.y})));
				}
				
			}
			if(e.type == SDL_MOUSEBUTTONUP){
			}
			if (e.type == SDL_KEYDOWN){
				switch(e.key.keysym.sym){
					case SDLK_ESCAPE:
						quit = true;break;
				}
			}
			if (e.type == SDL_KEYUP){
				switch(e.key.keysym.sym){
					case SDLK_e:
						controll *= -1;
						break;
					case SDLK_r:
						CS::clear();
						c = createCamera(0,0);
						break;
				}
			}
			if(e.type == SDL_TEXTINPUT){
				text = e.text.text;
			}

			CS::eventUpdate(e);
		}
		std::cout << text << std::endl;
		//if(rand()%10 > 5)
		//mBox(rand()%800, rand()%600);
		// std::cout << CS::_E_INDEX << std::endl;
		Timer::slice += Timer::elapsed;
		Window::Clear();
		for(; Timer::slice >= Timer::frame; Timer::slice -= Timer::frame)
		{
			CS::update();
			//CS::collisionUpdate();
		}
		//Window::Draw(t, r);
		CS::draw();
		Window::Present();
		auto timePoint2(chrono::high_resolution_clock::now());
		auto elaspedTime(timePoint2 - timePoint1);

		Timer::elapsed = chrono::duration_cast<
			chrono::duration<float, milli>>(elaspedTime).count();
		// std::cout << (1000.f/ft) << std::endl;
		// if(Timer::elapsed > maxTime){
		// 	int loops = Timer::elapsed/maxTime;
		// 	for(int i=0;i<loops;i++){
		// 		CS::update();
		// 	}
		// } else {3
		// 	SDL_Delay(delay);
		// }r
	}
	Window::Quit();
	return 0;
}

// int main(int argc, char *argv[])
// {
//    // InitVideo();
//     /* ... */
// 	SDL_Event event;
//     SDL_StartTextInput();
//     char* text;
//     char* ptext;
//     while (true)
//     {
//         if (SDL_PollEvent(&event))
//         {
//             switch (event.type)
//             {
//                 case SDL_TEXTINPUT:
//                     /* Add new text onto the end of our text */
//                     strcat(text, event.text.text);
//                     break;
//                 case SDL_TEXTEDITING:
                    
//                     Update the composition text.
//                     Update the cursor position.
//                     Update the selection length (if any).
                    
//                     composition = event.edit.text;
//                     cursor = event.edit.start;
//                     selection_len = event.edit.length;
//                     break;
//             }
//         }
//         if(text == ptext){
//         	std::string str = text;
//         	std::cout << str << std::endl;
//         }
//         ptext = text;
//        // Redraw();
//     }
// }