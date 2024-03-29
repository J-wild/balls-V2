#include <iostream>
#include <SDL2/SDL.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>

using namespace std;



const int WIDTH = 1000, HEIGHT = 800;

// variabes and constants

const float gravity  = 0.1;
bool g_on = false;
const float velMultiplier = 1;
const int forceFactor = 100;
const float airRes = 0.98;
const float dampingFactor = 0.8;
const int nParticles =  20000;
bool Push = false;
bool Pull = false;
bool running = true;
int force_equation = 0;
SDL_Rect box;

int cursorX, cursorY;

// time shit 
float Deltatime;
int frameCounter = 0;




//create window renderer
SDL_Window *window = SDL_CreateWindow( "BALLS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI );
SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, 0 );



/*SDL_Texture* renderText(const std::string &message, const std::string &fontFile,
	SDL_Color color, int fontSize, SDL_Renderer *renderer)
{

    if (TTF_Init() < 0)
    {
        printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
        exit(1);
    }
    
	//Open the font
	TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
	
    //We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	
	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
}*/
class button
{

    public:
    SDL_Rect rect;
    SDL_Colour colour_on = {0, 255, 0};
    


};

class particle {       
  public:             
    float xPos = WIDTH/2;        
    float yPos = HEIGHT/2;
    float xVel;
    float yVel;
    int particleR = 255;  
    int particleG = 0;
    int particleB = 255;
    int mass = 1;
   
};

particle p[nParticles]; 
void initalize()
{
   	
   SDL_Init( SDL_INIT_EVERYTHING );
  
    if ( NULL == window )
    {
        std::cout << "Could not create window: " << SDL_GetError( ) << std::endl;
        return;
    }

    

    box.w = 500;
    box.h = 500;
    box.x = WIDTH/2 - 250;
    box.y = HEIGHT/2 - box.h/2 ;

    Push = false;
    Pull = false;


    for(int i = 0; i<nParticles; i++)
    {
        p[i].xPos = box.w/2 + box.x;
        p[i].yPos = HEIGHT/2;

        float randomNx =  -1000 +(rand() % (1000 - -1000 +1)); 
        float randomNy = -1000 +(rand() % (1000 - -1000 +1));
        
        p[i].yVel = randomNy/1000 * velMultiplier;
        p[i].xVel = randomNx/1000 * velMultiplier;
    }

}

void checkEvents()
{
    SDL_Event event;
    if ( SDL_PollEvent( &event ) )
    {
        // get the curser position
        SDL_GetMouseState( &cursorX, &cursorY );

        switch (event.type){

            case SDL_QUIT:

            running = false;
            SDL_Quit();
            break;

            //manage keybourd inputs
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                
                case SDLK_ESCAPE:
                    running = false;
                    break;

                case SDLK_r:
                    initalize();
                    break;

                case SDLK_g:
                    g_on ^= true;
                    break;

                case SDLK_1:
                    force_equation = 1;
                    break;

                case SDLK_2:
                    force_equation = 2;
                    break;
    
                }

            case SDL_MOUSEBUTTONDOWN:
             
                if (event.button.button == SDL_BUTTON_LEFT){
                    Pull = true;
                    Push = false;
                }

                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    Pull = false;
                    Push = true;
                }
            

            } 
        }
}

void update()
{
    frameCounter += 1 ;
    for(int i = 0; i< nParticles; i++)
    {
        float deltaX = p[i].xPos - cursorX;
        float deltaY = p[i].yPos - cursorY;
        float theta = abs(atan(deltaY/deltaX));
        
        float applied_force;

        //calculate the force
        switch(force_equation){
            case 0:
                applied_force = 0;
                break;
            
            case 1:
                applied_force =  (forceFactor* 0.000001)*(pow(sqrt(pow(deltaX,2) + pow(deltaY, 2)),2));
                break;
            case 2: 
                applied_force = forceFactor/(pow(sqrt(pow(deltaX,2) + pow(deltaY, 2)),2));
                break;
        }

    
        float applied_force_x = applied_force*cos(theta) ;
        float applied_force_Y = applied_force*sin(theta)  ;
        
        if (Push)
        {
            if(deltaX >= 0)
            {p[i].xVel += applied_force_x; }

            else if (deltaX <=0)
            {p[i].xVel -= applied_force_x;}
            
            if(deltaY >= 0)
            {p[i].yVel += applied_force_Y;}

            else if (deltaY <= 0)
            {p[i].yVel -= applied_force_Y;}
            
        }

        else if(Pull)
        {
            if(deltaX >= 0)
            {p[i].xVel -= applied_force_x;}

            else if (deltaX <=0)
            {p[i].xVel +=applied_force_x;}
            
            if(deltaY >= 0)
            {p[i].yVel -= applied_force_Y;}

            else if (deltaY <= 0)
            {p[i].yVel += applied_force_Y;}
            
        }
        
        if(g_on)
        {p[i].yVel = ((p[i].yVel + gravity ) * p[i].mass)*airRes;}
        else
        {p[i].yVel = ((p[i].yVel) * p[i].mass)* airRes;}
        
        p[i].xVel = ((p[i].xVel ) * p[i].mass) * airRes;
        p[i].xPos += p[i].xVel * Deltatime;
        p[i].yPos += p[i].yVel * Deltatime;

    //check floor
            
        if(p[i].yPos >= box.y + box.h){
            p[i].yPos = box.y + box.h;
            p[i].yVel = -dampingFactor*p[i].yVel;
        }

        else if (p[i].yPos <= box.y){
            p[i].yPos = box.y ;
            p[i].yVel = -dampingFactor*p[i].yVel;
        }
    
        // check walls
        if(p[i].xPos >= box.x + box.w ){
            p[i].xPos = box.x + box.w;
            p[i].xVel = -dampingFactor * p[i].xVel;
        }
        
        else if(p[i].xPos <= box.x ){
            p[i].xPos = box.x ;
            p[i].xVel = -dampingFactor * p[i].xVel;
        }
        // colour
        float speed = (sqrt(pow(p[i].xVel/velMultiplier,2) + pow(p[i].yVel/velMultiplier,2)))/sqrt(2) ;
        p[i].particleR= speed * 255;
        p[i].particleB = 255 - (speed * 255); 

        /*if (speed >= 0.5, speed > 0 )
        {
            p[i].particleG = speed * 512;
        } 
        else if (speed  > 0.5, speed > 0)
        {
            p[i].particleG =255 - (speed - 0.5 )* 512;
        }*/

    }  
}
    
void draw()
{
    SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderDrawRect(renderer, &box);

    for(int i = 0; i < nParticles; i++)
    {
        SDL_SetRenderDrawColor( renderer, p[i].particleR, p[i].particleG, p[i].particleB, 255 );
        SDL_RenderDrawPoint(renderer, p[i].xPos, p[i].yPos );
        //SDL_RenderDrawPoint(renderer, p[i].xPos, p[i].yPos+1);
        //SDL_RenderDrawPoint(renderer, p[i].xPos+1, p[i].yPos);
        //SDL_RenderDrawPoint(renderer, p[i].xPos +1, p[i].yPos +1);
    }
   
    SDL_RenderPresent(renderer);
}


    

int main( int argc, char *argv[] )
{
    
    initalize();

    while (running){
    
        Uint64 start = SDL_GetPerformanceCounter();

        //------------------------------ main loop (every frame)

        checkEvents();
        update();
        draw();

        Uint64 end = SDL_GetPerformanceCounter();
        
        Deltatime = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        cout << 1000/Deltatime << endl;
        
        //------------------------------
    }
    system("cls");
    
    SDL_DestroyWindow( window );
    SDL_Quit();

    
    
    

    return EXIT_SUCCESS;
}