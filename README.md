
![icon](media/icon.png)

# dusk

DUsK, a library for gba dev

## media!

<img src="media/duskdemo.webp" width="256">
<img src="media/fountain2.webp" width="256">

## features
+ simple, intuitive C API
+ library integration
    + built in support for TONC, GBFS
+ graphics
    + scene architecture
    + 8bpp texture atlas packing
    + sprite/animation helpers
    + tiled map exporter and loader (via Tiled2GBA)
+ (WIP) saves

## ideas

dusk is built all around the idea of simplicity, clarity, and readability.
the exposed api is minimal, but not limiting.

## samples

sample projects can be found here: https://github.com/redthing1/dusk/tree/main/demo

these demo projects are written simply and cleanly.

they have plenty of comments explaining what and why the code does what it does.

furthermore, they demonstrate many of the different features provided by dusk.

## hacking

### requirements
+ devkitARM's `gba-dev` ([setup](https://devkitpro.org/wiki/Getting_Started))
+ [Tiled2GBA](https://github.com/LucvandenBrand/Tiled2GBA/tree/master/converter) converter in path as `Tiled2GBA`
+ [crunch](https://github.com/xdrie/crunch) atlas packer in path as `crunch_gen`

### build

enter `demo/DuskDemo` and run:

```sh
make
```

this will output `DuskDemo.gba`, which can be loaded up in your favorite GBA emulator.
