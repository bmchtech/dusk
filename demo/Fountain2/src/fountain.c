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

void fountain_start() {
    dusk_init_graphics();

    memset(rooms, 0, sizeof(rooms));
    rooms[1][1] = "central";
    rooms[1][0] = "north";

    // set up background
    current_map = dusk_load_map(rooms[1][1]);
    map_init_registers();
    map_set_onscreen(current_map);

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_chars");
    dusk_sprites_upload_atlas(&atlas);

    // make eggcat sprite (&sprites[0])
    eggcat = dusk_sprites_make(0, 16, 16,
                               (Sprite){
                                   .x = SCREEN_WIDTH / 2 - 8,
                                   .y = SCREEN_HEIGHT / 2 - 8,
                                   .tid = 8,
                                   .page = 0,
                               });
    walk = MAKE_ANIM(0, 4);

    // initialize
    bg_shift = (BackgroundPoint){128, 248};
    eggcat_vpos = (VPos){128, 248};
    room_pos = (VPos){1, 1};
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
    const int BG_CUT_Y = 512 - SCREEN_HEIGHT / 2;
    const int BG_CUT_X = 512 - SCREEN_WIDTH / 2;
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
    if (eggcat_vpos.x > 512) {
        new_room_pos.x += 1;
        door_r = true;
    }
    if (eggcat_vpos.y > 512) {
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
                eggcat_vpos.x = (eggcat_vpos.x + UL_CUT_X + 512) % 512;
            }
            if (door_ud) {
                if (door_u) {
                    eggcat_vpos.y = 512 + UL_CUT_Y;
                }
                if (door_d) {
                    eggcat_vpos.y = -UL_CUT_Y;
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
}

void fountain_end() {}

Scene fountain_scene = {
    .start = fountain_start,
    .update = fountain_update,
    .end = fountain_end,
};