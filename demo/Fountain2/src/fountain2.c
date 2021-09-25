#include <string.h>
#include "dusk.h"
#include "contrib/gbamap.h"

Map current_map;
Sprite* eggcat;
Anim walk;
const int WALK_SPEED = 1;
BackgroundPoint bg_shift;

#define ROOMS_GRID_SZ 3
char* rooms[ROOMS_GRID_SZ][ROOMS_GRID_SZ];

/** virtual position */
typedef struct VPos {
    short x, y;
} VPos;

VPos eggcat_vpos;
VPos room_pos;

#define SAVE_MAGIC 0xF0
void load_save() {
    // check if save valid
    if (sram_mem[0] != SAVE_MAGIC)
        return;

    // load data
    int p = 1;

    SV_LOAD_ITEM(eggcat_vpos, p);
    SV_LOAD_ITEM(room_pos, p);
}

void write_save() {
    // set magic flag
    sram_mem[0] = SAVE_MAGIC;

    // save data
    int p = 1;

    SV_SAVE_ITEM(eggcat_vpos, p);
    SV_SAVE_ITEM(room_pos, p);
}

void fountain_start() {
    dusk_init_graphics_mode0();

    memset(rooms, 0, sizeof(rooms));
    rooms[1][1] = "central";
    rooms[1][0] = "north";
    rooms[0][1] = "west";
    rooms[2][1] = "east";
    rooms[0][0] = "northwest";

    // vars
    bg_shift = (BackgroundPoint){128, 248};
    eggcat_vpos = (VPos){128, 248};
    room_pos = (VPos){1, 1};

    // read save data
    load_save();

    // set up background
    current_map = dusk_load_map(rooms[room_pos.x][room_pos.y]);
    map_init_registers();
    map_set_onscreen(current_map);

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("a_chars");
    dusk_sprites_upload_atlas(&atlas);

    // make eggcat sprite (&sprites[0])
    eggcat = dusk_sprites_make(0, 16, 16,
                               (Sprite){.x = SCREEN_WIDTH / 2 - 8,
                                        .y = SCREEN_HEIGHT / 2 - 8,
                                        .base_tid = 32,
                                        .page = 0,
                                        .flags = SPRITEFLAG_PRIORITY(3)});
    walk = MAKE_ANIM(0, 4);
}

/** update background and sprite from pos */
void adjust_background_to_player_vpos() {
    // update bg scroll
    bg_shift.x = eggcat_vpos.x;
    bg_shift.y = eggcat_vpos.y;

    /** keeps track of shift that goes beyond shifting bg.
     * negative means in the UL corner, positive is in the DR corner. */
    VPos bg_overlap = (VPos){0, 0};

    // attempt to clamp (512x512 maps)
    if (eggcat_vpos.y < 0) {
        bg_overlap.y = eggcat_vpos.y;
        bg_shift.y = 0;
    }

    if (eggcat_vpos.x < 0) {
        bg_overlap.x = eggcat_vpos.x;
        bg_shift.x = 0;
    }
    const int BG_CUT_Y = 512 - SCREEN_HEIGHT;
    const int BG_CUT_X = 512 - SCREEN_WIDTH;
    if (eggcat_vpos.y > BG_CUT_Y) {
        bg_overlap.y = eggcat_vpos.y - BG_CUT_Y;
        bg_shift.y = BG_CUT_Y;
    }
    if (eggcat_vpos.x > BG_CUT_X) {
        bg_overlap.x = eggcat_vpos.x - BG_CUT_X;
        bg_shift.x = BG_CUT_X;
    }

    // now apply the remaining overlap to the sprite position
    eggcat->x = SCREEN_WIDTH / 2 - 8;
    eggcat->y = SCREEN_HEIGHT / 2 - 8;
    if (ABS(bg_overlap.x) > 0) {
        eggcat->x += bg_overlap.x;
    }
    if (ABS(bg_overlap.y) > 0) {
        eggcat->y += bg_overlap.y;
    }
}

void check_room_doors() {
    // check room doors
    VPos new_room_pos = room_pos;
    bool door_l = false, door_u = false, door_r = false, door_d = false;

    const int UL_CUT_X = -120;
    const int UL_CUT_Y = -80;

    if (eggcat_vpos.x < UL_CUT_X) {
        new_room_pos.x -= 1;
        door_l = true;
    }
    if (eggcat_vpos.y < UL_CUT_Y) {
        new_room_pos.y -= 1;
        door_u = true;
    }
    if (eggcat_vpos.x > 512 + UL_CUT_X) {
        new_room_pos.x += 1;
        door_r = true;
    }
    if (eggcat_vpos.y > 512 + UL_CUT_Y) {
        new_room_pos.y += 1;
        door_d = true;
    }
    bool door_lr = door_l || door_r;
    bool door_ud = door_u || door_d;
    bool entered = door_lr || door_ud;
    // make sure it's a valid index
    if (entered && (new_room_pos.x >= 0 && new_room_pos.x < ROOMS_GRID_SZ) &&
        (new_room_pos.y >= 0 && new_room_pos.y < ROOMS_GRID_SZ)) {
        // make sure the room exists
        char* room_name = rooms[new_room_pos.x][new_room_pos.y];
        if (room_name) {
            // switch room
            room_pos = new_room_pos;
            current_map = dusk_load_map(rooms[room_pos.x][room_pos.y]);
            map_set_onscreen(current_map);

            // set new eggcat vpos
            if (door_lr) {
                if (door_l) {
                    eggcat_vpos.x = 512 + UL_CUT_X;
                }
                if (door_r) {
                    eggcat_vpos.x = UL_CUT_X;
                }
            }
            if (door_ud) {
                if (door_u) {
                    eggcat_vpos.y = 512 + UL_CUT_Y;
                }
                if (door_d) {
                    eggcat_vpos.y = UL_CUT_Y;
                }
            }
        }
    }
}

void fountain_update() {
    dusk_frame();

    // input
    int y_move = key_tri_vert();
    int x_move = key_tri_horz();
    bool moving = (y_move != 0 || x_move != 0);

    // update pos
    eggcat_vpos.x += x_move * WALK_SPEED;
    eggcat_vpos.y += y_move * WALK_SPEED;

    adjust_background_to_player_vpos();

    check_room_doors();

    if (moving)
        dusk_sprites_anim_play(eggcat, &walk);
    else
        eggcat->page = 0;

    // update map position
    map_shift(current_map, bg_shift);

    // update sprites
    dusk_sprites_update();

    // write save data
    if (frame_count % 60 == 0) {
        write_save();
    }
}

void fountain_end() {}

Scene fountain_scene = {
    .start = fountain_start,
    .update = fountain_update,
    .end = fountain_end,
};