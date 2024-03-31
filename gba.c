#include "gba.h"
#define TRANSPARENT_COLOR WHITE

// 1D array
volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;

/*
  Wait until the start of the next VBlank. This is useful to avoid tearing.
  Completing this function is required.
*/
void waitForVBlank(void)
{
  // Height of GBA Screen
  while (SCANLINECOUNTER > 160)
  {
  }
  // While loop that keeps going until we're in vBlank:
  while (SCANLINECOUNTER < 160)
  {
  }
  // Increment the vBlank counter:
  vBlankCounter++; // 60fps, so if 60 vBlank pass, we know one second has passed
}

static int __qran_seed = 42;
static int qran(void)
{
  __qran_seed = 1664525 * __qran_seed + 1013904223;
  return (__qran_seed >> 16) & 0x7FFF;
}

int randint(int min, int max) { return (qran() * (max - min) >> 15) + min; }

/*
  Sets a pixel in the video buffer to a given color.
*/
void setPixel(int row, int col, u16 color)
{
  videoBuffer[OFFSET(row, col, WIDTH)] = color; // Offset converts row and col into color
}

/*
  Draws a rectangle of a given color to the video buffer.
  The width and height, as well as the top left corner of the rectangle, are passed as parameters.
*/
void drawRectDMA(int row, int col, int width, int height, volatile u16 color)
{
  // Draws top to bottom
  for (int i = 0; i < height; i++)
  {
    DMA[3].src = &color; // src and dst are ptr
    DMA[3].dst = &videoBuffer[OFFSET(row + i, col, WIDTH)];
    DMA[3].cnt = width | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
  }
}

/*
  Draws a fullscreen image to the video buffer.
  The image passed in must be of size WIDTH * HEIGHT.
*/
void drawFullScreenImageDMA(const u16 *image)
{
  DMA[3].src = image;
  DMA[3].dst = videoBuffer;
  DMA[3].cnt = WIDTH * HEIGHT | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT;
}

/*
  Draws an image to the video buffer.
  The width and height, as well as the top left corner of the image, are passed as parameters.
  The image passed in must be of size width * height.
*/
void drawImageDMA(int row, int col, int width, int height, const u16 *image)
{

  for (int i = 0; i < height; i++)
  {
    DMA[3].src = &image[i * width]; // Adjust to use the correct offset within the image
    DMA[3].dst = &videoBuffer[(row + i) * WIDTH + col];
    DMA[3].cnt = width | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT;
  }
}

/*
  Draws a rectangular chunk of a fullscreen image to the video buffer.
  The width and height, as well as the top left corner of the chunk to be drawn, are passed as parameters.
  The image passed in must be of size WIDTH * HEIGHT.
*/
void undrawImageDMA(int row, int col, int width, int height, const u16 *image)
{
  UNUSED(image); // The image parameter is not needed for filling a color
  // We draw with the background color
  volatile u16 color = WHITE;
  for (int i = 0; i < height; i++)
  {
    DMA[3].src = &color;
    DMA[3].dst = &videoBuffer[(row + i) * WIDTH + col];
    DMA[3].cnt = width * height | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT;
  }
}

/*
  Fills the video buffer with a given color.
*/
void fillScreenDMA(volatile u16 color)
{
  // DMA[3] is our controller struct (we work with 4th channel based on lg)
  // DMA copy and pastes chunks of pixels
  DMA[3].src = &color;
  DMA[3].dst = &videoBuffer[0];
  // Use bitwise operations to maniulate DMA
  // WIDTH * HEIGHT will be total number of bits to be transformed
  // Turn DMA on, and incr to get next pixel in the width
  DMA[3].cnt = WIDTH * HEIGHT | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
}

/* STRING-DRAWING FUNCTIONS (provided) */
void drawChar(int row, int col, char ch, u16 color)
{
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (fontdata_6x8[OFFSET(j, i, 6) + ch * 48])
      {
        setPixel(row + j, col + i, color);
      }
    }
  }
}

void drawString(int row, int col, char *str, u16 color)
{
  while (*str)
  {
    drawChar(row, col, *str++, color);
    col += 6;
  }
}

void drawCenteredString(int row, int col, int width, int height, char *str, u16 color)
{
  u32 len = 0;
  char *strCpy = str;
  while (*strCpy)
  {
    len++;
    strCpy++;
  }

  u32 strWidth = 6 * len;
  u32 strHeight = 8;

  int new_row = row + ((height - strHeight) >> 1);
  int new_col = col + ((width - strWidth) >> 1);
  drawString(new_row, new_col, str, color);
}

// Added
void drawImageWithTransparency(int row, int col, int width, int height, const u16 *image)
{
  for (int r = 0; r < height; ++r)
  {
    for (int c = 0; c < width; ++c)
    {
      u16 color = image[r * width + c];
      if (color != TRANSPARENT_COLOR)
      {
        setPixel(row + r, col + c, color);
      }
    }
  }
}
void drawColorWithTransparency(int row, int col, int width, int height, u16 color)
{
  for (int r = 0; r < height; ++r)
  {
    for (int c = 0; c < width; ++c)
    {
      if (color != TRANSPARENT_COLOR)
      {
        setPixel(row + r, col + c, color);
      }
    }
  }
}