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

/** initialize the system for using DUSK */
void dusk_init_all();
/** initialize mode0 graphics */
void dusk_init_graphics_mode0();
/** initialize mode3 graphics */
void dusk_init_graphics_mode3();
/** vsync */
void dusk_frame();
void dusk_scene_set(Scene next);
void dusk_scene_update();