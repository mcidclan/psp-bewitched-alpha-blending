# Bewitched Texture Blending - Isolated Alpha Method

## The Problem

On PSP, it's normally impossible to perform true alpha blending between two textures because the framebuffer does not preserve the alpha component. This channel is reserved for stencil buffer operations and alpha blending tests.

## The Solution

This technique separates the alpha channel and calculates the blending independently, then forces the alpha component directly into the framebuffer output.

## How It Works

1. Extract and blend the alpha components from both textures separately
2. Blend the RGB components using standard alpha blending
3. Force the calculated alpha back into the framebuffer

## Usage
```c
BewitchedBlend blend;
blend.width = 256;
blend.height = 256;
blend.texture0 = texture0;
blend.texture1 = texture1;
blend.intensity = 0x80;  // blend intensity (0x00-0xFF)
blend.vramOffset = vramOffset;
blend.list = displayList;

produceBewitchedBlending(&blend);
```

Full sample available in PSP Undocumented Sorcery Project  
[See more](https://github.com/mcidclan/psp-undocumented-sorcery/tree/main/bewitched-texture-blending)

*m-c/d*

