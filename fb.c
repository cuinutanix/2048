/*
 * Copyright (c) 2014 Nutanix Inc. All rights reserved.
 *
 * Author: cui@nutanix.com
 *
 */

#include "console.h"

#include "fb.h"
#include "assets/font10x20.h"

struct VbeInfoBlock {
  char VbeSignature[4];
  u16  VbeVersion;
  u16  OemStringPtr[2];
  u8   Capabilities[4];
  u16  VideoModePtr[2];
  u16  TotalMemory;
};

extern const struct VbeInfoBlock VBE_INFO;

struct ModeInfoBlock {
  u16 attributes;
  u8 winA,winB;
  u16 granularity;
  u16 winsize;
  u16 segmentA, segmentB;
  u16 real_fct_ptr[2];
  u16 pitch; // bytes per scanline

  u16 Xres, Yres;
  u8 Wchar, Ychar, planes, bpp, banks;
  u8 memory_model, bank_size, image_pages;
  u8 reserved0;

  u8 red_mask, red_position;
  u8 green_mask, green_position;
  u8 blue_mask, blue_position;
  u8 rsv_mask, rsv_position;
  u8 directcolor_attributes;

  u32 physbase;
  u32 reserved1;
  u16 reserved2;
};

extern const struct ModeInfoBlock VBE_MODE_INFO;
extern const u8 VBE_SUCCESS;

#define FRAMEBUFFER ((volatile u16 *)VBE_MODE_INFO.physbase)
#define X_RES       VBE_MODE_INFO.Xres
#define Y_RES       VBE_MODE_INFO.Yres

inline void set_pixel(int y, int x, u16 color)
{
  FRAMEBUFFER[y * X_RES + x] = color;
}

static void fb_draw_char(char c, u8 vga_color, int row, int col)
{
  int y;
  int x;
  u16 fg_color = rgb24to16(VGA_PALETTE[vga_color & 0xf]);
  u16 bg_color = rgb24to16(VGA_PALETTE[vga_color >> 4]);

  for (y = 0; y < FONT_Y_RES; y++) {
    for (x = 0; x < FONT_X_RES; x++) {
      u16 color = (FONT_BITMAP[(u8)c][y] >> x) & 1 ? fg_color : bg_color;
      set_pixel(row * FONT_Y_RES + y,  col * FONT_X_RES + x, color);
    }
  }
}

void fb_init()
{
  int i;
  u16 color = rgb24to16(VGA_PALETTE[0]);

  if (!VBE_SUCCESS) {
    print("Could not initialize VESA BIOS extensions for framebuffer "
          "console.\n");
    while(1) { asm("hlt"); }
  }

  if (VBE_MODE_INFO.bpp != 16) {
    print("VESA BIOS error: not 16-bit depth\n");
    while(1) { asm("hlt"); }
  }

  for (i = 0; i < X_RES * Y_RES; i++) {
    FRAMEBUFFER[i] = color;
  }

  console_upgrade(X_RES / FONT_X_RES, Y_RES / FONT_Y_RES, fb_draw_char);
}
