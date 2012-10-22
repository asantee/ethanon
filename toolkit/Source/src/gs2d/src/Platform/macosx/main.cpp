//
//  main.cpp
//  HelloSDL
//
//  Created by Richard Carter on 5/30/11.
//  Copyright 2011 Richard Carter. All rights reserved.
//

#include <Enml/Enml.h>

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "main.h"

#define WIDTH 640
#define HEIGHT 480
#define WINDOW_TITLE "Hello, SDL!"

SDL_Surface *screen;

void render() {
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
	
	static float angle = 0.1f;
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
    
    glBegin(GL_TRIANGLES);
    {
        glColor3f(1,0,0);
        glVertex2f(0,0);
        
        glColor3f(0,1,0);
        glVertex2f(.5,0);
        
        glColor3f(0,0,1);
        glVertex2f(.5,.5);
    }
    glEnd();
}

int SDL_main (int argc, char **argv) {

    // Initialize
    SDL_Init(SDL_INIT_VIDEO);
    
    // Enable double-buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create a OpenGL window
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_OPENGL | 
                              SDL_RESIZABLE);
    if(!screen) 
    {
        printf("Couldn't set %dx%d GL video mode: %s\n", WIDTH,
               HEIGHT, SDL_GetError());
        SDL_Quit();
        exit(2);
    }
    SDL_WM_SetCaption(WINDOW_TITLE, WINDOW_TITLE); 

    bool done = false;
    while(!done) 
    {
        SDL_Event event;
        // Rotates the triangle (this could be replaced with custom
        // processing code)
        // animate();
        
        // Respond to any events that occur
        while(SDL_PollEvent(&event))
        {
            switch(event.type) 
            {
                case SDL_VIDEORESIZE:
                    screen = SDL_SetVideoMode(event.resize.w, 
                                              event.resize.h, 0,
                                              SDL_OPENGL | SDL_RESIZABLE);
                    if(screen)
                    {
                        // reshape(screen->w, screen->h);
                    } 
                    else 
                    {
                        ; // Oops, we couldn't resize for some reason. 
                        // This should never happen
                    }
                    break;
                    
                case SDL_QUIT:
                    done = true;
                    break;		
                    
                    // ## INSERT CODE TO HANDLE ANY OTHER EVENTS HERE ##
            }
        }
        
        // Check for escape
        Uint8 *keys = SDL_GetKeyState(NULL);
        if( keys[SDLK_ESCAPE] ) {
            done = true;
        }
        
        // Draw the screen
        render();
        
        SDL_GL_SwapBuffers();
    }
    
    
    
    SDL_Quit();
    return 0;
}
