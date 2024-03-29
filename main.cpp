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

const float velMultiplier = 10;
const int forceFactor = 1000;
const float airRes = 0.98;
const float dampingFactor = 0.8;
const int nParticles =  20000;

bool g_on = false;
bool Push = false; 
bool Pull = false;
bool running = true;
bool collide = false;
int force_equation = 0;




int cursorX, cursorY;

// time shit 
float Deltatime;
int frameCounter = 0;




//create window renderer
SDL_Window *window = SDL_CreateWindow( "BALLS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI );
SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, 0 );

class Button {
    public:
    SDL_Rect rect;
    int r = 255, g = 255, b = 255;
    int a = 255;
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
SDL_Rect box;
particle p[nParticles];

Button refreshB;

void initalize()
{
   	
    SDL_Init( SDL_INIT_EVERYTHING );
  

    if ( NULL == window ){
        std::cout << "Could not create window: " << SDL_GetError( ) << std::endl;
        return;
    }
    
    //set container box
    box.w = 1000;
    box.h = 800;
    box.x = WIDTH/2 - box.w/2;
    box.y = HEIGHT/2 - box.h/2 ;
    
    // set refresh button
    refreshB.rect.x = 900;
    refreshB.rect.y = 200;
    refreshB.rect.w = 50;
    refreshB.rect.h = 25;

    Push = false;
    Pull = false;

    for(int i = 0; i<nParticles; i++){
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

        if(cursorX >= refreshB.rect.x && cursorX <= refreshB.rect.x + refreshB.rect.w && cursorY >=refreshB.rect.y && cursorY <= refreshB.rect.y + refreshB.rect.h ){
            refreshB.r = 0;
            refreshB.g = 255;
            refreshB.b = 0;
        }

        else{
            refreshB.r = 255;
            refreshB.g = 255;
            refreshB.b = 255;
        }


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
                    
                case SDLK_c:
                collide ^= true;
                break;
                
                
    
                }

            case SDL_MOUSEBUTTONDOWN:
             
                if (event.button.button == SDL_BUTTON_LEFT){
                    if(cursorX >= refreshB.rect.x && cursorX <= refreshB.rect.x + refreshB.rect.w && cursorY >=refreshB.rect.y && cursorY <= refreshB.rect.y + refreshB.rect.h ){
                        initalize();
                    }
                    else{
                        Pull = true;
                        Push = false;  
                    }
                    
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
                applied_force =  (forceFactor*0.0000001)*(pow(sqrt(pow(deltaX,2) + pow(deltaY, 2)),2));
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

        if (collide){
          for(int i = 0; i<nParticles; i++){

            for(int j = 0; j<nParticles; j++){
                
                if(j != i && (p[j].xPos == p[i].xPos) && (p[j].yPos == p[i].yPos))
                {
                   
                    cout<< "collide" << endl;
                
                    p[j].xVel  = -dampingFactor * p[j].xVel;
                    p[j].yVel = -dampingFactor*p[j].yVel;

                    

                }
        
                
            }  
            }
        }
        // colour
        float speed = (sqrt(pow(p[i].xVel,2) + pow(p[i].yVel,2)))/sqrt(2) ;
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
    // background
    SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
    SDL_RenderClear(renderer);
    
    // points
    for(int i = 0; i < nParticles; i++)
    {
        SDL_SetRenderDrawColor( renderer, p[i].particleR, p[i].particleG, p[i].particleB, 255 );
        SDL_RenderDrawPoint(renderer, p[i].xPos, p[i].yPos );
        //SDL_RenderDrawPoint(renderer, p[i].xPos, p[i].yPos+1);
        //SDL_RenderDrawPoint(renderer, p[i].xPos+1, p[i].yPos);
        //SDL_RenderDrawPoint(renderer, p[i].xPos +1, p[i].yPos +1);
    }
    //box
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderDrawRect(renderer, &box);
    
    //buttons
    SDL_SetRenderDrawColor( renderer, refreshB.r, refreshB.g, refreshB.b, refreshB.a );
    SDL_RenderDrawRect(renderer, &refreshB.rect);

    

    

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
        std:cout << 1000/Deltatime << endl;
        
        //------------------------------
    }
    system("cls");
    
    SDL_DestroyWindow( window );
    SDL_Quit();

    
    
    

    return EXIT_SUCCESS;
}