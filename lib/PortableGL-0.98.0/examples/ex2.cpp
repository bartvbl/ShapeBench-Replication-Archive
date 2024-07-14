#include "rsw_math.h"


#define PGL_PREFIX_TYPES
#define PORTABLEGL_IMPLEMENTATION
#include "portablegl.h"


#include <iostream>
#include <stdio.h>


#define SDL_MAIN_HANDLED
#include <SDL.h>

#define WIDTH 640
#define HEIGHT 480

#ifndef FPS_EVERY_N_SECS
#define FPS_EVERY_N_SECS 1
#endif

#define FPS_DELAY (FPS_EVERY_N_SECS*1000)

using namespace std;

using rsw::vec4;
using rsw::mat4;

vec4 Red(1.0f, 0.0f, 0.0f, 0.0f);
vec4 Green(0.0f, 1.0f, 0.0f, 0.0f);
vec4 Blue(0.0f, 0.0f, 1.0f, 0.0f);

SDL_Window* window;
SDL_Renderer* ren;
SDL_Texture* tex;

u32* bbufpix;

glContext the_Context;

void cleanup();
void setup_context();

void smooth_vs(float* vs_output, pgl_vec4* vertex_attribs, Shader_Builtins* builtins, void* uniforms);
void smooth_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms);

int main(int argc, char** argv)
{
	setup_context();

	GLenum smooth[4] = { PGL_SMOOTH4 };

	float points[] = { -0.5, -0.5, 0,
	                    0.5, -0.5, 0,
	                    0,    0.5, 0 };


	float color_array[] = { 1.0, 0.0, 0.0, 1.0,
	                        0.0, 1.0, 0.0, 1.0,
	                        0.0, 0.0, 1.0, 1.0 };

	GLuint triangle, colors;
	glGenBuffers(1, &triangle);
	glBindBuffer(GL_ARRAY_BUFFER, triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*9, points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, color_array, GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//There's no reason to use a vec4 and vary the alpha in this case.  It'll just make
	//it marginally slower.  I can't think of very many cases where'd you want that effect.
	//TODO look into SDL's behavior/options
	//Note, I can't use SDL2 for my blending because my library has to be self-contained
	//and fulfill the specs on its own but I should know what SDL2 can do anyway
	GLuint myshader = pglCreateProgram(smooth_vs, smooth_fs, 4, smooth, GL_FALSE);
	glUseProgram(myshader);

	glClearColor(0, 0, 0, 1);

	SDL_Event e;
	bool quit = false;

	int old_time = 0, new_time=0, counter = 0;
	int ms;
		
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				quit = true;
			if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				quit = true;
			if (e.type == SDL_MOUSEBUTTONDOWN)
				quit = true;
		}

		new_time = SDL_GetTicks();

		counter++;
		ms = new_time - old_time;
		if (ms >= FPS_DELAY) {
			printf("%d  %f FPS\n", ms, counter*1000.0f/ms);
			old_time = new_time;
			counter = 0;
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Render the scene
		SDL_UpdateTexture(tex, NULL, bbufpix, WIDTH * sizeof(u32));

		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);
	}

	cleanup();

	return 0;
}

void smooth_vs(float* vs_output, pgl_vec4* vertex_attribs, Shader_Builtins* builtins, void* uniforms)
{
	((pgl_vec4*)vs_output)[0] = vertex_attribs[4]; //color

	builtins->gl_Position = vertex_attribs[0];
}

void smooth_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms)
{
	*(vec4*)&builtins->gl_FragColor = ((vec4*)fs_input)[0];
}


void setup_context()
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO)) {
		cout << "SDL_Init error: " << SDL_GetError() << "\n";
		exit(0);
	}

	window = SDL_CreateWindow("ex2", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) {
		cerr << "Failed to create window\n";
		SDL_Quit();
		exit(0);
	}

	ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	if (!init_glContext(&the_Context, &bbufpix, WIDTH, HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)) {
		puts("Failed to initialize glContext");
		exit(0);
	}
}

void cleanup()
{
	free_glContext(&the_Context);

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

