/*
 * Bewitched texture blending, 'Isolated Alpha' method
 * This function has been written as part of the PSP Undocumented Sorcery project
 * It allows true blending of two textures while preserving the alpha component
 * Copyright mcidclan
 */
#define u16 unsigned short int
#define u32 unsigned int

struct Vertex {
  u16 u, v;
  u32 color;
  u16 x, y, z;
} __attribute__((aligned(4), packed));

/*
 * Bewitched blending between texture 0 and texture 1.
 * The alpha of the output texture is blended.
 */
struct BewitchedBlend {
  u16 width;
  u16 height;
  u32* texture0;
  u32* texture1;
  u16 intensity;
  u16 bufferWidth;
  u32 vramOffset;
  u32 byteCount;
  u32* buffer;
  void* list;
};

void produceBewitchedBlending(BewitchedBlend* const blend) {
  
  const u16 width = blend->width;
  const u16 height = blend->height;
  const u32* const tex0 = blend->texture0;
  const u32* const tex1 = blend->texture1;
  
  const u32 fbw = blend->bufferWidth = (width + 63) & ~63;
  const u32 width565 = width * 2;
  const u32 fbw565 = fbw * 2;
  
  blend->byteCount = fbw * height * 4;
  
  const u32 buffer[3] = {
    0x04000000 | (blend->vramOffset),
    0x04000000 | (blend->vramOffset + blend->byteCount),
    0x04000000 | (blend->vramOffset + blend->byteCount * 2),
  };

  blend->buffer = (u32*)(0x40000000 | buffer[0]);
  
  const Vertex __attribute__((aligned(4))) bSprite[2] = {
    { 0,     0,      0xffffffff, 0,     0,      0 },
    { width, height, 0xffffffff, width, height, 0 }
  };
  sceKernelDcacheWritebackRange(bSprite, (sizeof(Vertex) * 2 + 63) & ~63);
  
  PspGeContext context __attribute__((aligned(16)));
  sceGeSaveContext(&context);
  
  sceGuStart(GU_DIRECT, blend->list);
  
  // shift alpha components to green channel
  sceGuCopyImage(GU_PSM_5650,
    1, 0, width565 - 1, height, width565, (void*)tex0,
    0, 0, fbw565, (void*)buffer[0]
  );
  sceGuCopyImage(GU_PSM_5650,
    1, 0, width565 - 1, height, width565, (void*)tex1,
    0, 0, fbw565, (void*)buffer[1]
  );

  // enable common states
  sceGuClearColor(0);
  sceGuEnable(GU_BLEND);
  sceGuEnable(GU_SCISSOR_TEST);
  sceGuEnable(GU_TEXTURE_2D);
  sceGuScissor(0, 0, width, height);
  sceGuTexMode(GU_PSM_8888, 0, 1, 0);
  
  // forcing alpha before blending the green component as our produced real alpha
  // then blend alpha components of texture 0 and texture 1 through green channel  
  const u32 fix = (blend->intensity << 8);
  sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, fix, fix);

  sceGuDrawBuffer(GU_PSM_8888, (void*)(buffer[2]), fbw);
  sceGuClear(GU_COLOR_BUFFER_BIT);

  sceGuTexImage(0, width, height, fbw, (void*)buffer[0]);
  sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_COLOR_8888 |
  GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, NULL, bSprite);

  sceGuTexImage(0, width, height, fbw, (void*)buffer[1]);
  sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_COLOR_8888 |
  GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, NULL, bSprite);
  
  sceGuTexSync();

  // shift back the final alpha component
  sceGuCopyImage(GU_PSM_5650,
    0, 0, width565 - 1, height, fbw565, (void*)buffer[2],
    1, 0, fbw565, (void*)buffer[0]
  );

  // blend RGB components of texture 0 and texture 1
  sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

  sceGuDrawBuffer(GU_PSM_8888, (void*)buffer[0], fbw);
  sceGuClear(GU_COLOR_BUFFER_BIT);
  
  sceGuTexImage(0, width, height, width, tex0);
  sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_COLOR_8888 |
  GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, NULL, bSprite);

  sceGuTexImage(0, width, height, width, tex1);
  sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_COLOR_8888 |
  GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, NULL, bSprite);
  
  sceGuTexSync();

  sceGuFinish();
  sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
  
  sceGeRestoreContext(&context);
}
