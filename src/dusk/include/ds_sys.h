/*
    DUSK: SYSTEM
*/

#pragma once

extern uint frame_count;

typedef struct Scene {
	void (*start)(void);
	void (*end)(void);
	void (*update)(void);
} Scene;

void dusk_init();  // initialize the system for using DUSK
void dusk_frame(); // vsync
void dusk_scene_set(Scene next);
void dusk_scene_update();