#include <string.h>
#include <tonc.h>
#include "contrib/gbfs.h"
#include "contrib/gbamap.h"
#include "dusk.h"

Map load_map()
{
    u32 map_data_sz = 0;
    const u16 *map_data = gbfs_get_obj(gbfs_dat, "fountain.bin", &map_data_sz);

    return map_load_from_rom(map_data);
}

int main()
{
    dusk_init();

    map_init_registers();
    Map map = load_map();
    map_set_onscreen(map);

    dusk_sprites_init();

    SpriteAtlas atlas = dusk_load_atlas("atlas0");
    memcpy(&tile_mem[4][0], atlas.tiles, atlas.tile_sz);
    memcpy(&pal_obj_bank[0], atlas.pal, atlas.pal_sz);

    int px = SCREEN_WIDTH / 2 - 8, py = SCREEN_HEIGHT / 2 - 8;
    u32 bpp = 8;
    u32 tid = 8, pb = 0;
    OBJ_ATTR *player = &obj_buffer[0];
    int player_attr0 = ATTR0_SQUARE | ATTR0_8BPP;
    obj_set_attr(player,
                 player_attr0,             // Square, regular sprite
                 ATTR1_SIZE_16,            // 16x16p,
                 ATTR2_PALBANK(pb) | tid * bpp); // palbank 0, tile 0

    const int SHIFT_SPEED = 1;
    BackgroundPoint bg_shift = {128, 248};
    u32 frames = 0;
    int player_frame = 0;
    while (TRUE)
    {
        while(KEY_DOWN_NOW(KEY_START)); // pause with start
        vid_vsync();
        frames++;

        key_poll();
        int ymove = key_tri_vert();
        int xmove = key_tri_horz();
        bg_shift.y += ymove * SHIFT_SPEED;
        bg_shift.x += xmove * SHIFT_SPEED;
        map_shift(map, bg_shift);

        bool moving = (ymove != 0 || xmove != 0);
        if (moving)
        {
            // when running
            if (frames % 6 == 0)
            {
                player_frame = (player_frame + 1) % 4;
            }
        }
        else
        {
            player_frame = 0;
        }

        obj_set_attr(player, player_attr0, ATTR1_SIZE_16, ATTR2_PALBANK(pb) | ((tid + player_frame) * bpp));

        obj_set_pos(player, px, py);
        oam_copy(oam_mem, obj_buffer, 1); // only need to update one
    }
}