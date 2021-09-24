
# gfx building

in GIMP, set color mode to Indexed, and select 16 or 256 depending on your bpp

# iwram/ewram

by default, devkitarm puts stack, heap, and static

to put a var in `EWRAM`:
`__attribute__((section(".ewram")))`

to put a function in `IWRAM` (instead of ROM):
add `IWRAM_CODE`

# arm/thumb

todo