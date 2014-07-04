////////////////////////////////////////////////////////////////////////////
//                                                                        //
// display.h                                                              //
//                                                                        //
// Some basic graphics and painting support                               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_H
#define _DISPLAY_H

// We use Uint32 and Octet from network.h
//
#include "network.h"


typedef unsigned int Pixel;

#define DISPLAYWIDTH 1280
#define DISPLAYHEIGHT 960
#define FRAMESIZE (DISPLAYWIDTH * DISPLAYHEIGHT * sizeof(Pixel))

typedef struct {
  Uint32 w;
  Uint32 h;
  Pixel **lines;  // array of scan lines, each an array of native pixels
  void *pixelMem; // memory block allocated for "lines"
} Image;

static Pixel createPixel(Octet r, Octet g, Octet b, Octet transparency) {
  // Return pixel value.
  //
  // "transparency" allows alpha-composing, with all-zero being opaque.
  // These pixels are compatible with the display controller's frame buffer.
  //
  return (transparency << 24) | (r << 16) | (g << 8) | b;
}

Pixel alphaComposePixels(Pixel below, Pixel above);
// General alpha-composition.  Slow.  Very slow.  Think of it as
// documentation.  "paintPixel" optimizes the fact cases.
//
// Returns the effect of painting "above" on "below", honoring the
// transparency channel of both pixels.

static inline void paintPixel(Pixel *dest, Pixel srce) {
  // Paint "srce" on top of "dest", honoring transparency.
  //
  unsigned int transparency = srce >> 24;
  if (transparency == 0) {
    // srce is fully opaque: ignore *dest
    *dest = srce;
  } else if (transparency == 255) {
    // srce is fully transparent: do nothing
  } else if ((*dest >> 24) == 255) {
    // *dest is fully transparent: ignore it
    *dest = srce;
  } else {
    // general (slow) case
    *dest = alphaComposePixels(*dest, srce);
  }
}

Image *createImage(int w, int h);
// Allocate memory for an image, with undefined pixels.

void flushImage(Image *image);
// Ensure image is up to date in real memory

void invalidateImage(Image *image);
// Ensure our D-cache has no out-of-date data for image

void freeImage(Image *image);
// Free an image that was allocated by createImage

Image *createFrame();
// Allocate and return a frame buffer usable by the display controller.

void showFrame(Image *frame);
// Show the frame buffer on the display.
// Caller is responsible for flushing the D-cache appropriately.

void paintImage(Image *dest, Image *copy, Image *srce, int x, int y);
// Paint given image into given destination at given position.
// Also, copy this range of pixels into corresponding pixels in "copy".
// If you don't want the copy function, use dest == copy.

void paintRepeat(Image *dest, Image *srce, int x, int y, int w, int h);
// Paint given image tiled across given area, clipped to that area.
// The image is aligned to a grid based at [0,0], so multiple calls
// at different offsets tile seamlessly.

void fillRect(Image *dest, Pixel pixel, int x, int y, int w, int h);
// Fill given rectangle with given pixel

void strokeRect(Image *dest, Pixel pixel, int x, int y, int w, int h,
		int penWidth);
// Stroke outside of given rectangle with given pixel and given penWidth


//
// Parsing and rendering .bmp files
//

Image *readBmp(char *imgBytes, int imgLen);
// Read a .bmp image and return an appropriate Image*
// Only a few of the known possibilities are implemented.


//
// Parsing and rendering GIF files
//

typedef struct { // Graphic control extension sub-block contents
  Octet packed;
  Octet delayLS;
  Octet delayMS;
  Octet transparent;        // index of transparent pixel
} GifGCE;

typedef struct OneGifImage { // One image within a GIF file
  int x;                    // position of this image on the GIF's canvas
  int y;
  int w;                    // size of this image
  int h;
  Pixel *colors;            // local color table, or NULL
  GifGCE gce;               // the image's GCE if any, defaulted to 0
  Octet *imgBytes;          // pointer into original file, for this image
  struct OneGifImage *next; // next image, or NULL
} OneGifImage;

typedef struct { // Parsed GIF file
  int w;                    // canvas size
  int h;
  Pixel *colors;            // global color table, or NULL
  Pixel bkgd;               // canvas background color
  OneGifImage *first;       // first image, or NULL
} GifImage;

Image *renderGif(OneGifImage *gif, Pixel *gColors);
// Given a constituent image from with a parse GIF file,
// use the LZW data bytes to reconstruct the image's pixels.
// Returns the appropriate image file.

GifImage *readGif(Octet *imgBytes, int imgLen);
// Parse a GIF into its constituent parts.
// Doesn't do the LZW decompression: that's in renderGif
//
// CAUTION: this is not robust against malformed GIF files

#endif
