# Bewitched Texture Blending - Isolated Alpha Method
A technique to achieve true alpha blending between two textures on PlayStation Portable, working around the hardware constraint where the framebuffer's alpha channel is reserved for stencil and alpha test operations.

## The Problem
On PSP, performing true alpha blending between two textures with standard techniques is challenging because the framebuffer does not preserve this component during rendering.

## The Solution
This technique separates the alpha channel calculation and performs blending independently, then forces the result directly into the framebuffer output, enabling true alpha compositing between textures.

## How It Works
1. **Shift alpha to green channel**: Use `sceGuCopyImage` with `GU_PSM_5650` format to shift both textures one pixel right, moving the alpha component into the green channel
2. **Blend isolated alphas**: Blend the alpha components (now in green) from both textures separately using fixed blending
3. **Shift back the result**: Use `sceGuCopyImage` to shift the blended alpha one pixel left, restoring it to the alpha channel
4. **Blend RGB components**: Perform standard alpha blending on the RGB components using the preserved alpha values

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

