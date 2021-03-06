#include <string>
#include <stdexcept>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../include/window.h"

//Initialize the unique_ptr's deleters here
std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> Window::mWindow 
	= std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>(nullptr, SDL_DestroyWindow);
std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)> Window::mRenderer
	= std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)>(nullptr, SDL_DestroyRenderer);
//other static members
SDL_Rect Window::mBox;

Window::Window(){
}
Window::~Window(){
}
void Window::Init(std::string title, bool fullscreen, int x, int y, int w, int h){
    //initialize all SDL subsystems
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		throw std::runtime_error("SDL Init Failed");
    if (TTF_Init() == -1)
		throw std::runtime_error("TTF Init Failed");

    //Setup our windo
    mBox.x = x;
    mBox.y = y;
    mBox.w = w;
    mBox.h = h;
    //Create our window
    mWindow.reset(SDL_CreateWindow(title.c_str(),
     mBox.x, mBox.y, 
        mBox.w, mBox.h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI));
    if(fullscreen){
        SDL_SetWindowFullscreen(mWindow.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    //Make sure it created ok
    if (mWindow == nullptr)
        throw std::runtime_error("Failed to create window");

    //Create the renderer
    mRenderer.reset(SDL_CreateRenderer(mWindow.get(), -1,
     SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));
    //Make sure it created ok
    if (mRenderer == nullptr)
        throw std::runtime_error("Failed to create renderer");
}
void Window::Quit(){
    TTF_Quit();
    SDL_Quit();
}
void Window::Draw(SDL_Texture *tex, SDL_Rect &dstRect,
                SDL_Rect *clip, float angle, 
                int xPivot, int yPivot, SDL_RendererFlip flip)
{
    //Convert pivot pos from relative to object's center to screen space
    xPivot += dstRect.w / 2;
    yPivot += dstRect.h / 2;
    //SDL expects an SDL_Point as the pivot location
    SDL_Point pivot = { xPivot, yPivot };
    //Draw the texture
    SDL_RenderCopyEx(mRenderer.get(), tex, clip, &dstRect, angle, &pivot, flip);
}
SDL_Texture* Window::LoadImage(const std::string &file){
    SDL_Texture* tex = nullptr;
	tex = IMG_LoadTexture(mRenderer.get(), file.c_str());
	if (tex == nullptr)
		throw std::runtime_error("Failed to load image: " + file + IMG_GetError());
	return tex;
}
SDL_Texture* Window::RenderText(const std::string &message,
         const std::string &fontFile, SDL_Color color, int fontSize){
    //Open the font
	TTF_Font *font = nullptr;
	font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr)
		throw std::runtime_error("Failed to load font: " + fontFile + TTF_GetError());
	
	//Render the message to an SDL_Surface, as that's what TTF_RenderText_X returns
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer.get(), surf);
	//Clean up unneeded stuff
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);

	return texture;
}
void Window::DrawRect(const SDL_Rect* rect, Uint8 r, Uint8 g, Uint8 b){
    SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, 255);
    SDL_RenderDrawRect(mRenderer.get(), rect);
}
void Window::Clear(){

    SDL_RenderClear(mRenderer.get());
}
void Window::Present(){

    SDL_RenderPresent(mRenderer.get());
    SDL_SetRenderDrawColor(mRenderer.get(), 0, 255, 255, 0);
}
SDL_Rect Window::Box(){
    return mBox;
}