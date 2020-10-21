#include <stddef.h>
#include <tonc_types.h>
#include "ds_load.h"

void dusk_load_init() {
    // locate the main GBFS archive
    gbfs_dat = find_first_gbfs_file(find_first_gbfs_file);
}