/*
    DUSK: SAVE UTIL
*/

#pragma once

// utilities for saving and loading vars

#define SV_LOAD_ITEM(var, type, offset)                                                                                \
    memcpy(&var, sram_mem + offset, sizeof(type));                                                                     \
    offset += sizeof(type);

#define SV_SAVE_ITEM(var, type, offset)                                                                                \
    memcpy(sram_mem + offset, &var, sizeof(type));                                                                     \
    offset += sizeof(type);

// utilities for saving and loading vars