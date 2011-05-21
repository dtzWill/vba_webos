// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <memory.h>

#include "../GBA.h"
#include "gbGlobals.h"
#include "gbSGB.h"

u8 gbInvertTab[256] = {
  0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,
  0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
  0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,
  0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
  0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,
  0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
  0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,
  0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
  0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,
  0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
  0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,
  0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
  0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,
  0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
  0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,
  0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
  0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,
  0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
  0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,
  0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
  0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,
  0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
  0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,
  0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
  0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,
  0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
  0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,
  0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
  0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,
  0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
  0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,
  0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
};

u16 gbLineMix[160];

// See gen_color_repack_LUT.cpp
uint16_t repackTable[256] = {
	0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015, 0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055, 0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115, 0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155, 0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415, 0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455, 0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515, 0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555, 0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015, 0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055, 0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115, 0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155, 0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415, 0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455, 0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515, 0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555, 0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015, 0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055, 0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115, 0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155, 0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415, 0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455, 0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515, 0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555, 0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015, 0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055, 0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115, 0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155, 0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415, 0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455, 0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515, 0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555
};
// Interleaves the high and low bits in the tile row, returning 8 contiguous
// 2-bit numbers packed into a 16-bit variable.
static u16 repack_colors(u8 tile_high, u8 tile_low)
{
  return (repackTable[tile_high] << 1) | repackTable[tile_low];
}

// Colors is 8 2-bit numbers packed into a u16. Extract the color for pixel x,
// where x counts from right to left (since thats how tile data is stored in GB
// VRAM).
static u8 extract_color(u16 colors, u8 x_rtl)
{
  return (colors >> 2*x_rtl) & 0x3;
}

void gbRenderLine()
{
  u8 * bank0;
  u8 * bank1;
  if(gbCgbMode) {
    bank0 = &gbVram[0x0000];
    bank1 = &gbVram[0x2000];
  } else {
    bank0 = &gbMemory[0x8000];
    bank1 = NULL;
  }
  
  // Address of the background tile map, relative to the start of video memory.
  // See "VRAM Background Maps" in the Pan Docs
  int tile_map = 0x1800;
  if((register_LCDC & 8) != 0)
    tile_map = 0x1c00;

  // Address of the tile pattern table containing the actual 4 color 8x8 tile
  // images, each of which occupies 16 bytes.
  int tile_pattern = 0x0800;
  if((register_LCDC & 16) != 0)
    tile_pattern = 0x0000;
    
  int x = 0;
  int y = register_LY;

  if(y >= 144)
    return;

  //  int yLine = (y + gbBorderRowSkip) * gbBorderLineSkip;

  // Scrolling offset used when displaying the BG Map
  int sx = register_SCX;
  int sy = register_SCY;

  sy+=y;

  sy &= 255;
    
  // The tile component of the scrolling offset
  int tx = sx >> 3;
  int ty = sy >> 3;

  // The pixel component of the scrolling offset. Each line of 8 pixels is
  // stored in two adjacent bytes. The least significant bit of each pixel is
  // in the first byte, while the MSB is in the second. The rightmost pixel in
  // the line is stored in the least significant bit of each byte, i.e. the
  // significance increases from right to left.
  int bx = 7 - (sx & 7);
  int by = sy & 7;

  int tile_map_line_y = tile_map + ty * 32;
  
  int tile_map_address = tile_map_line_y + tx;

  u8 attrs = 0;
  if(bank1 != NULL)
    attrs = bank1[tile_map_address];
  
  // Number of the first tile being rendered in this line
  u8 tile = bank0[tile_map_address];
  
  tile_map_address++;
  
  if((register_LCDC & 16) == 0) {
    if(tile < 128) tile += 128;
    else tile -= 128;
  }

  int tile_pattern_address = tile_pattern + tile * 16 + by*2;

  if(register_LCDC & 0x80) {
    if((register_LCDC & 0x01 || gbCgbMode) && (layerSettings & 0x0100)) {
      while(x < 160) {
        u8 tile_a = 0;
        u8 tile_b = 0;
        
        if(attrs & 0x40) {
          tile_pattern_address = tile_pattern + tile * 16 + (7-by)*2;
        }
        
        if(attrs & 0x08) {
          tile_a = bank1[tile_pattern_address++];
          tile_b = bank1[tile_pattern_address];
        } else {
          tile_a = bank0[tile_pattern_address++];
          tile_b = bank0[tile_pattern_address];
        }
        
        if(attrs & 0x20) {
          tile_a = gbInvertTab[tile_a];
          tile_b = gbInvertTab[tile_b];
        }

        u16 colors = repack_colors(tile_b, tile_a);

        while(bx >= 0) {
          u8 c = extract_color(colors, bx);
          
          gbLineBuffer[x] = c; // mark the gbLineBuffer color
          
          if(attrs & 0x80)
            gbLineBuffer[x] |= 0x300;
          
          if(gbCgbMode) {
            c = c + (attrs & 7)*4;
          } else {
            c = gbBgp[c];         
            if(gbSgbMode && !gbCgbMode) {
              int dx = x >> 3;
              int dy = y >> 3;
              
              int palette = gbSgbATF[dy * 20 + dx];
              
              if(c == 0)
                palette = 0;
              
              c = c + 4*palette;
            }
          }
          gbLineMix[x] = gbPalette[c];
          x++;
          --bx;
          if(x >= 160)
            break;
        }
        tx++;
        if(tx == 32)
          tx = 0;
        bx = 7;
        
        if(bank1)
          attrs = bank1[tile_map_line_y + tx];
        
        tile = bank0[tile_map_line_y + tx];
        
        if((register_LCDC & 16) == 0) {
          if(tile < 128) tile += 128;
          else tile -= 128;
        }
        tile_pattern_address = tile_pattern + tile * 16 + by * 2;
      }
      if (gbColorOption)
      {
        for (int i = 0; i < 160; ++i)
            gbLineMix[i] = gbColorFilter[gbLineMix[i]];
      }
    } else {
      for(int i = 0; i < 160; i++) {
        gbLineMix[i] = gbPalette[0];
        gbLineBuffer[i] = 0;
      }
    }
    
    // do the window display
    if((register_LCDC & 0x20) && (layerSettings & 0x2000)) {
      int wy = register_WY;
      
      if(y >= wy) {
        int wx = register_WX;
        wx -= 7;
        
        if( wx <= 159 && gbWindowLine <= 143) {
          
          tile_map = 0x1800;
          
          if((register_LCDC & 0x40) != 0)
            tile_map = 0x1c00;
          
          if(gbWindowLine == -1) {
            gbWindowLine = 0;
          }
          
          tx = 0;
          ty = gbWindowLine >> 3;
          
          bx = 7;
          by = gbWindowLine & 7;
          
          if(wx < 0) {
            bx -= (-wx);
            wx = 0;
          }
          
          tile_map_line_y = tile_map + ty * 32;
          
          tile_map_address = tile_map_line_y + tx;
          
          x = wx;
          
          tile = bank0[tile_map_address];
          u8 attrs = 0;
          if(bank1)
            attrs = bank1[tile_map_address];
          tile_map_address++;
          
          if((register_LCDC & 16) == 0) {    
            if(tile < 128) tile += 128;
            else tile -= 128;
          }
          
          tile_pattern_address = tile_pattern + tile * 16 + by*2;

          while(x < 160) {
            u8 tile_a = 0;
            u8 tile_b = 0;
            
            if(attrs & 0x40) {
              tile_pattern_address = tile_pattern + tile * 16 + (7-by)*2;
            }
            
            if(attrs & 0x08) {
              tile_a = bank1[tile_pattern_address++];
              tile_b = bank1[tile_pattern_address];
            } else {
              tile_a = bank0[tile_pattern_address++];
              tile_b = bank0[tile_pattern_address];
            }
            
            if(attrs & 0x20) {
              tile_a = gbInvertTab[tile_a];
              tile_b = gbInvertTab[tile_b];
            }

            u16 colors = repack_colors(tile_b, tile_a);

            while(bx >= 0) {
              u8 c = extract_color(colors, bx);

              if(attrs & 0x80)
                gbLineBuffer[x] = 0x300 + c;
              else
                gbLineBuffer[x] = 0x100 + c;
              
              if(gbCgbMode) {
                c = c + (attrs & 7) * 4;
              } else {
                c = gbBgp[c];         
                if(gbSgbMode && ! gbCgbMode) {
                  int dx = x >> 3;
                  int dy = y >> 3;
                  
                  int palette = gbSgbATF[dy * 20 + dx];
                  
                  if(c == 0)
                    palette = 0;
                  
                  c = c + 4*palette;            
                }
              }
              gbLineMix[x] = gbPalette[c];
              x++;
              --bx;
              if(x >= 160)
                break;
            }
            tx++;
            if(tx == 32)
              tx = 0;
            bx = 7;
            tile = bank0[tile_map_line_y + tx];
            if(bank1)
              attrs = bank1[tile_map_line_y + tx];
            
            if((register_LCDC & 16) == 0) {         
              if(tile < 128) tile += 128;
              else tile -= 128;
            }
            tile_pattern_address = tile_pattern + tile * 16 + by * 2;
          }
          gbWindowLine++;
          if (gbColorOption)
          {
            for (int i = wx; i < 160; ++i)
              gbLineMix[i] = gbColorFilter[gbLineMix[i]];
          }
        }
      }
    }
  } else {
    for(int i = 0; i < 160; i++) {
      gbLineMix[i] = gbPalette[0];
      gbLineBuffer[i] = 0;
    }
  }
}

void gbDrawSpriteTile(int tile, int x,int y,int t, int flags,
                      int size,int spriteNumber)
{
  u8 * bank0;
  u8 * bank1;
  if(gbCgbMode) {
    if(register_VBK & 1) {
      bank0 = &gbVram[0x0000];
      bank1 = &gbVram[0x2000];
    } else {
      bank0 = &gbVram[0x0000];
      bank1 = &gbVram[0x2000];
    }
  } else {
    bank0 = &gbMemory[0x8000];
    bank1 = NULL;
  }
  
  int init = 0x0000;

  //  int yLine = (y+gbBorderRowSkip) * gbBorderLineSkip;

  u8 *pal = gbObp0;

  int flipx = (flags & 0x20);
  int flipy = (flags & 0x40);
  
  if((flags & 0x10))
    pal = gbObp1;

  if(flipy) {
    t = (size ? 15 : 7) - t;
  }

  int prio =  flags & 0x80;
  
  int address = init + tile * 16 + 2*t;
  int a = 0;
  int b = 0;

  if(gbCgbMode && flags & 0x08) {
    a = bank1[address++];
    b = bank1[address++];
  } else {
    a = bank0[address++];
    b = bank0[address++];
  }
  
  u16 colors = repack_colors(b, a);

  for(int xx = 0; xx < 8; xx++) {
    u8 i = (7-xx);
    u8 c = extract_color(colors, i);
    
    if(c==0) continue;

    int xxx = xx+x;
    if(flipx)
      xxx = (7-xx+x);

    if(xxx < 0 || xxx > 159)
      continue;

    u16 color = gbLineBuffer[xxx];
    
    if(prio) {
      if(color < 0x200 && ((color & 0xFF) != 0))
        continue;
    }
    if(color >= 0x300 && color != 0x300)
      continue;
    else if(color >= 0x200 && color < 0x300) {
      int sprite = color & 0xff;

      int spriteX = gbMemory[0xfe00 + 4 * sprite + 1] - 8;

      if(spriteX == x) {
        if(sprite < spriteNumber)
          continue;
      } else {
        if(gbCgbMode) {
          if(sprite < spriteNumber)
            continue;
        } else {
          if(spriteX < x+8)
            continue;
        }
      }
    } 
    

    gbLineBuffer[xxx] = 0x200 + spriteNumber;

    // make sure that sprites will work even in CGB mode
    if(gbCgbMode) {
      c = c + (flags & 0x07)*4 + 32;
    } else {
      c = pal[c];

      if(gbSgbMode && !gbCgbMode) {
        int dx = xxx >> 3;
        int dy = y >> 3;
        
        int palette = gbSgbATF[dy * 20 + dx];
        
        if(c == 0)
          palette = 0;
        
        c = c + 4*palette;              
      } else {
        c += 4;
      }
    }

    gbLineMix[xxx] = gbColorOption ? gbColorFilter[gbPalette[c]] :
      gbPalette[c];
  }
}

void gbDrawSprites()
{
  int x = 0;
  int y = 0;
  int count = 0;
  
  int size = (register_LCDC & 4);

  if(!(register_LCDC & 0x80))
    return;
  
  if((register_LCDC & 2) && (layerSettings & 0x1000)) {
    int yc = register_LY;
      
    int address = 0xfe00;
    for(int i = 0; i < 40; i++) {
      y = gbMemory[address++];
      x = gbMemory[address++];
      int tile = gbMemory[address++];
      if(size)
        tile &= 254;
      int flags = gbMemory[address++];

      if(x > 0 && y > 0 && x < 168 && y < 160) {
        // check if sprite intersects current line
        int t = yc -y + 16;
        if(size && t >=0 && t < 16) {
          gbDrawSpriteTile(tile,x-8,yc,t,flags,size,i);
          count++;
        } else if(!size && t >= 0 && t < 8) {
          gbDrawSpriteTile(tile, x-8, yc, t, flags,size,i);
          count++;
        }
      }
      // sprite limit reached!
      if(count >= 10)
        break;
    }   
  }
}

