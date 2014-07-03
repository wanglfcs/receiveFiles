////////////////////////////////////////////////////////////////////////////
//                                                                        //
// display.c                                                              //
//                                                                        //
// Some basic graphics and painting support                               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "intercore.h"
#include "display.h"


//
// Native image format
//

Pixel alphaComposePixels(Pixel below, Pixel above) {
  // General alpha-composition.  Slow.  Very slow.  Think of it as
  // documentation.  "paintPixel" optimizes the fact cases.
  //
  // Returns the effect of painting "above" on "below", honoring the
  // transparency channel of both pixels.
  //
  // The code here works with opacity not transparency, because I find
  // that easier to think about.
  //
  unsigned int a1 = 255 - (above >> 24);
  unsigned int a2 = 255 - (below >> 24);
  unsigned int newA = a1 + a2 * (255 - a1) / 255;
  if (newA == 0) {
    // Result is 100% transparent; any RGB is OK.  Avoid dividing by 0.
    return createPixel(0, 0, 0, 255);
  } else {
    unsigned int r1 = (above >> 16) & 255;
    unsigned int g1 = (above >> 8) & 255;
    unsigned int b1 = above & 255;
    unsigned int r2 = (below >> 16) & 255;
    unsigned int g2 = (below >> 8) & 255;
    unsigned int b2 = below & 255;
    unsigned int newR = (r1 * a1 + (r2 * a2 * (255 - a1)) / 255) / newA;
    unsigned int newG = (g1 * a1 + (g2 * a2 * (255 - a1)) / 255) / newA;
    unsigned int newB = (b1 * a1 + (b2 * a2 * (255 - a1)) / 255) / newA;
    return createPixel(newR, newG, newB, 255 - newA);
  }
}

Image *createImage(int w, int h) {
  // Allocate memory for an image, with undefined pixels
  //
  int lineBytes = w * sizeof(Pixel);
  Image *res = malloc(sizeof(Image));
  res->w = w;
  res->h = h;
  res->lines = malloc(h * sizeof(Pixel *));
  res->pixelMem = malloc(h * lineBytes + 31);
  void *pixels = cacheAlign(res->pixelMem);
  for (int v = 0; v < h; v++) {
    res->lines[v] = pixels;
    pixels += lineBytes;
  }
  cache_flushMem(res, sizeof(Image));
  cache_flushMem(res->lines, h * sizeof(Pixel *));
  return res;
}

void flushImage(Image *image) {
  // Ensure image is up to date in real memory
  cache_flushMem(image->pixelMem,
		 image->h * image->w * sizeof(Pixel) + 31);
}

void invalidateImage(Image *image) {
  // Ensure our D-cache has no out-of-date data for image
  cache_invalidateMem(image, sizeof(Image));
  cache_invalidateMem(image->lines, image->h * sizeof(Pixel *));
  cache_invalidateMem(image->pixelMem,
		      image->h * image->w * sizeof(Pixel) + 31);
}

void freeImage(Image *image) {
  // Free an image that was allocated by createImage
  free(image->pixelMem); image->pixelMem = NULL;
  free(image->lines); image->lines = NULL;
  free(image);
}

Image *createFrame() {
  // Allocate and return a frame buffer
  //
  return createImage(DISPLAYWIDTH, DISPLAYHEIGHT);
}

void showFrame(Image *frame) {
  // Show the frame buffer on the display
  setFrameBuffer(frame->lines[0]);
}


// In the following, pixels are numbered from [0,0] at the top-left.
//
// All the painting functions clip to within the display.
//


void paintImage(Image *dest, Image *copy, Image *srce, int x, int y) {
  // Paint given image into given destination at given position.
  // Also, copy this range of pixels into corresponding pixels in "copy".
  // If you don't want the copy function, use dest == copy.
  //
  unsigned int clipX = (x < 0 ? 0 : x);
  unsigned int clipY = (y < 0 ? 0 : y);
  unsigned int clipR = (x + srce->w < clipX ? clipX : x + srce->w);
  if (clipR > dest->w) clipR = dest->w;
  unsigned int clipB = (y + srce->h < clipY ? clipY : y + srce->h);
  if (clipB > dest->h) clipB = dest->h;
  for (unsigned int v = clipY; v < clipB; v++) {
    Pixel *srceLine = srce->lines[v - y] + clipX - x;
    Pixel *destLine = dest->lines[v] + clipX;
    Pixel *copyLine = copy->lines[v] + clipX;
    for (unsigned int h = 0; h < clipR - clipX; h++) {
      paintPixel(&(destLine[h]), srceLine[h]);
      copyLine[h] = destLine[h];
    }
  }
}

void paintRepeat(Image *dest, Image *srce, int x, int y, int w, int h) {
  // Paint given source image tiled across given area, clipped to that area.
  // The source image is aligned to a grid based at [0,0], so multiple calls
  // at different offsets tile seamlessly.
  //
  unsigned int clipX = (x < 0 ? 0 : x);
  unsigned int clipY = (y < 0 ? 0 : y);
  unsigned int clipR = (x + w < clipX ? clipX : x + w);
  if (clipR > dest->w) clipR = dest->w;
  unsigned int clipB = (y + h < clipY ? clipY : y + h);
  if (clipB > dest->h) clipB = dest->h;
  unsigned int imgV = clipY % srce->h;
  unsigned int firstImgHor = clipX % srce->w;
  for (unsigned int v = clipY; v < clipB; v++) {
    Pixel *srceLine = srce->lines[imgV];
    Pixel *destLine = dest->lines[v] + clipX;
    int imgHor = firstImgHor;
    for (unsigned int hor = 0; hor < clipR - clipX; hor++) {
      paintPixel(&(destLine[hor]), srceLine[imgHor]);
      imgHor++;
      if (imgHor >= srce->w) imgHor = 0;
    }
    imgV++;
    if (imgV >= srce->h) imgV = 0;
  }
}

void fillRect(Image *dest, Pixel pixel, int x, int y, int w, int h) {
  // Fill given rectangle with given pixel
  //
  unsigned int clipX = (x < 0 ? 0 : x);
  unsigned int clipY = (y < 0 ? 0 : y);
  unsigned int clipR = (x + w < clipX ? clipX : x + w);
  if (clipR > dest->w) clipR = dest->w;
  unsigned int clipB = (y + h < clipY ? clipY : y + h);
  if (clipB > dest->h) clipB = dest->h;
  for (unsigned int v = clipY; v < clipB; v++) {
    Pixel *destLine = dest->lines[v] + clipX;
    for (unsigned int hor = 0; hor < clipR - clipX; hor++) {
      paintPixel(&(destLine[hor]), pixel);
    }
  }
}

void strokeRect(Image *dest, Pixel pixel, int x, int y, int w, int h,
		int penWidth) {
  // Stroke outside of given rectangle with given pixel and given penWidth
  fillRect(dest, pixel, x - penWidth, y - penWidth, w+penWidth, penWidth);
  fillRect(dest, pixel, x + w, y - penWidth, penWidth, h + penWidth);
  fillRect(dest, pixel, x, y + h, w + penWidth, penWidth);
  fillRect(dest, pixel, x - penWidth, y, penWidth, h + penWidth);
}


//
// .bmp layout (simplified)
//

#define BmpHeaderSize 14;

typedef struct { // Magic number header
  Octet a;
  Octet b;
} BmpMagic;

typedef struct { // BMP header after 2-byte magic number
  Uint32 fSize;
  Uint16 reserved1;
  Uint16 reserved2;
  Uint32 dataOffset;
  // Followed by 40-byte DIB header, probably
  Uint32 hSize;   // header byte count, 40 in this case
  Uint32 width;   // pixels
  Uint32 height;  // pixels
  Uint16 planes;  // color plane count, always 1
  Uint16 bpp;     // bits per pixel
  Uint32 method;  // compression method
  Uint32 dSize;   // size of the pixel data
  Uint32 hRes;    // horizontal resolution, pixels per meter (!)
  Uint32 vRes;    // vertical resolution
  Uint32 palette; // palette size, 2^n if 0
  Uint32 import;  // number of important colors in the palette
} BmpHeader;


Image *readBmp(char *imgBytes, int imgLen) {
  // Read a .bmp image and return an appropriate Image*
  // Only a few of the known possibilities are implemented
  //
  Image *res = createImage(0, 0); // default, for errors
  BmpMagic *magic = (BmpMagic *)imgBytes;
  BmpHeader hdr;
  bcopy(imgBytes + 2, &hdr, sizeof(BmpHeader));
  if (magic->a != 'B' || magic->b != 'M') {
    printf("Not a BMP file\n");
  } else  if (hdr.hSize != 40) {
    printf("Unsupported header size %d\n", hdr.hSize);
  } else if (hdr.method != 0) {
    printf("Unsupported compression algorithm %d\n", hdr.method);
  } else if (hdr.bpp != 24) {
    printf("Unsupported bits per pixel %d\n", hdr.bpp);
  } else {
    res = createImage(hdr.width, hdr.height);
    Octet *data = imgBytes + hdr.dataOffset;
    int bytesPerPixel = hdr.bpp / 8;
    int bytesPerLine = ((hdr.width * bytesPerPixel + 3) / 4) * 4;
    for (int v = hdr.height-1; v >= 0; v--) {
      int p = 0;
      Pixel *line = res->lines[v];
      for (int h = 0; h < hdr.width; h++) {
	// .bmp has byte order BGR
	line[h] = createPixel(data[p+1], data[p+1], data[p], 0); 
	p += bytesPerPixel;
      }
      data += bytesPerLine;
    }
  }
  return res;
}


//
// GIF
//

typedef struct { // GIF header, including "GIF89a"
  char sig1;       // 'G'
  char sig2;       // 'I'
  char sig3;       // 'F'
  char ver1;
  char ver2;
  char ver3;
  Octet wLS;       // canvas width
  Octet wMS;
  Octet hLS;       // canvas height
  Octet hMS;
  Octet packed;    // odds and ends
  Octet bkgdIndex; // background color for non-painted canvas
  Octet pixelAspectRatio;
} GifHeader;

typedef struct { // GIF image descriptor
  char separator;
  Octet leftLS;
  Octet leftMS;
  Octet topLS;
  Octet topMS;
  Octet wLS;
  Octet wMS;
  Octet hLS;
  Octet hMS;
  Octet packed;
} GifImageDesc;


// Specified limit on GIF symbols, 12 bits
//
#define GIFTOTALSYMBOLS 4096

Image *renderGif(OneGifImage *gif, Pixel *gColors) {
  // Use the LZW data bytes to reconstruct the image's pixels.
  // Returns the appropriate image file.
  //
  Image *image = createImage(gif->w, gif->h);
  Octet *imgBytes = gif->imgBytes;
  Pixel *colors = (gif->colors ? gif->colors : gColors);

  // Parameters of this decompression
  //
  int minSymbolSize = *imgBytes;
  imgBytes++;
  int clearSymbol = 1 << minSymbolSize;
  int eofSymbol = clearSymbol + 1;

  // State of the decompression
  //
  int symbolBits = minSymbolSize+1; // current symbol size in bits
  int symbolMask = (1 << symbolBits) - 1;
  int prevSymbol = clearSymbol;     // previous symbol processed

  // State of the input sub-block machinery
  //
  int bytesInSubBlock = 0;          // current data remaining in sub-block
  unsigned int data = 0;            // current unconsumed bits
  int bitsInData = 0;               // amount of unconsumed bits

  // State of the output
  //
  Pixel* destPixel = image->lines[0];
  int destDone = 0;                 // protective count of pixels written
  int destLimit = image->w * image->h;

  // The code table
  //
  // The value of symbol i consists of codeLeft[i] followed by codeRight[i],
  // where codeLeft is a previous symbol and codeRight is the appended
  // pixel.  The value contains codeLength[i] total pixels.
  //
  int *codeLeft = malloc(GIFTOTALSYMBOLS * sizeof(int));
  Pixel *codeRight = malloc(GIFTOTALSYMBOLS * sizeof(Pixel));
  int *codeLength = malloc(GIFTOTALSYMBOLS * sizeof(int));
  int nextFree = eofSymbol + 1;     // next free code table entry
  for (int i = 0; i < clearSymbol; i++) {
    codeLeft[i] = -1;
    codeRight[i] = colors[i];
    codeLength[i] = 1;
  }
  if (gif->gce.packed & 1) {
    // Setup transparent color.  We use bright red so it's obvious.
    codeRight[gif->gce.transparent] = createPixel(255, 0, 0, 255);
  }


  // Main loop, one iteration per code symbol
  //
  for (;;) {

    // Get the symbol
    //
    while (bitsInData < symbolBits) {
      if (bytesInSubBlock == 0) {
	bytesInSubBlock = *imgBytes;
	imgBytes++;
	if (bytesInSubBlock == 0) break;
      }
      data += *imgBytes << bitsInData;
      imgBytes++;
      bytesInSubBlock--;
      bitsInData += 8;
    }
    if (bitsInData < symbolBits) break; // no more data: give up
    int symbol = data & symbolMask;
    data = data >> symbolBits;
    bitsInData -= symbolBits;

    // Process it
    //
    if (symbol == clearSymbol) {
      // re-initialise the table
      nextFree = eofSymbol + 1;
      symbolBits = minSymbolSize + 1;
      symbolMask = (1 << symbolBits) - 1;

    } else if (symbol == eofSymbol) {
      // skip any remaining data, including terminator sub-block
      imgBytes += bytesInSubBlock;
      while (*imgBytes != 0) imgBytes += 1 + *imgBytes;
      imgBytes++;
      break;

    } else if (prevSymbol == clearSymbol) {
      // start of decompression: just output the pixel value
      if (destDone < destLimit) {
	*destPixel = codeRight[symbol];
	destPixel++;
	destDone++;
      }

    } else {  
      // normal case

      // (1) distinguish the awkward (symbol not yet defined) and easy cases
      //
      int outSym = (symbol >= nextFree ? prevSymbol : symbol);
      Pixel nextPix; // first pixel in outSym, for defining new symbol

      // (2) copy outSym to the output
      //
      int outLength = codeLength[outSym];
      if (destDone + outLength <= destLimit) {
	int s = outSym;
	Pixel *p = destPixel + outLength;
	for (;;) {
	  p--;
	  *p = codeRight[s];
	  if ((s = codeLeft[s]) < 0) break;
	}
	nextPix = *destPixel;
	destPixel += outLength;
	destDone += outLength;
      }

      // (3) in the awkward case, copy outPix to the output
      //
      if (symbol >= nextFree && destDone < destLimit) {
	*destPixel = nextPix;
	destPixel++;
	destDone++;
      }

      // (4) update our code table
      //
      if (nextFree < GIFTOTALSYMBOLS) {
	codeLeft[nextFree] = prevSymbol;
	codeRight[nextFree] = nextPix;
	codeLength[nextFree] = codeLength[prevSymbol] + 1;
	nextFree++;
      }

      // (5) adjust the symbol size
      //
      if (nextFree > symbolMask && nextFree < GIFTOTALSYMBOLS) {
	symbolBits++;
	symbolMask = (1 << symbolBits) - 1;
      }

    }
    prevSymbol = symbol;
  }

  cache_invalidateMem(codeLeft, GIFTOTALSYMBOLS * sizeof(int));
  cache_invalidateMem(codeRight, GIFTOTALSYMBOLS * sizeof(Pixel));
  cache_invalidateMem(codeLength, GIFTOTALSYMBOLS * sizeof(int));
  free(codeLeft);
  free(codeRight);
  free(codeLength);

  return image;
}

GifImage *readGif(Octet *imgBytes, int imgLen) {
  // Parse a GIF into its constituent parts.
  // Doesn't do the LZW decompression: that's in renderGif
  //
  // CAUTION: this is not robust against malformed GIF files
  //
  Octet *base = imgBytes;
  GifImage *res = malloc(sizeof(GifImage));
  res->w = 0; // default, for errors
  res->h = 0;
  res->colors = NULL;
  res->bkgd = createPixel(0, 0, 0, 0);
  res->first = NULL;
  GifHeader hdr;
  bcopy(imgBytes, &hdr, sizeof(GifHeader));
  imgBytes += 13; // "sizeof" rounds up to 0 mod 4
  if (hdr.sig1 != 'G' || hdr.sig2 != 'I' || hdr.sig3 != 'F') {
    printf("Not a GIF file\n");
  } else {
    res->w = (hdr.wMS << 8) + hdr.wLS;
    res->h = (hdr.hMS << 8) + hdr.hLS;
    if (hdr.packed >> 7) {
      // File has a global color table
      int gColors = 1 << (1 + (hdr.packed & 7));
      res->colors = malloc(gColors * sizeof(Pixel));
      for (int i = 0; i < gColors; i++) {
	res->colors[i] =
	  createPixel(imgBytes[0], imgBytes[1], imgBytes[2], 0);
	imgBytes += 3;
      }
      res->bkgd = res->colors[hdr.bkgdIndex];
      cache_flushMem(res->colors, gColors * sizeof(Pixel));
    }
    OneGifImage **tail = &(res->first);
    int notDone = 1;
    GifGCE gce; // records most recent graphic control extension
    bzero(&gce, 4);
    while (notDone) {
      switch (*imgBytes) {
      case 0x21: // Extension
	imgBytes++;
	int introducer = *imgBytes;
	imgBytes++;
	int subLen = *imgBytes;
	// The branches of the switch don't advance imgBytes
	switch (introducer) {
	case 0x01: // Plain text
	  break;
	case 0xF9: // Graphic control
	  bcopy(imgBytes+1, &gce, 4);
	  break;
	case 0xFE: // Comment
	  break;
	case 0xFF: // Application
	  break;
	default: // Unknown
	  break;
	}
	// Skip the extension's data blocks
	while (*imgBytes != 0) imgBytes += 1 + *imgBytes;
	imgBytes++;
	break;
      case 0x2C: { // Image
	GifImageDesc desc;
	bcopy(imgBytes, &desc, sizeof(GifImageDesc));
	imgBytes += 10; // "sizeof" rounds up to 0 mod 4
	int interlace = (desc.packed >> 6) & 1;
	OneGifImage *this = malloc(sizeof(OneGifImage));
	this->x = (desc.leftMS << 8) + desc.leftLS;
	this->y = (desc.topMS << 8) + desc.topLS;
	this->w = (desc.wMS << 8) + desc.wLS;
	this->h = (desc.hMS << 8) + desc.hLS;
	bcopy(&gce, &(this->gce), 4);
	this->next = NULL;
	*tail = this;
	tail = &(this->next);
	int localColors = desc.packed >> 7;
	if (localColors) {
	  // Image has a local color table
	  int lColors = 1 << (1 + (desc.packed & 7));
	  this->colors = malloc(lColors * sizeof(Pixel));
	  for (int i = 0; i < lColors; i++) {
	    this->colors[i] =
	      createPixel(imgBytes[0], imgBytes[1], imgBytes[2], 0);
	    imgBytes += 3;
	  }
	  cache_flushMem(this->colors, lColors * sizeof(Pixel));
	} else {
	  this->colors = NULL;
	}
	this->imgBytes = imgBytes;
	imgBytes++;
	while (*imgBytes != 0) imgBytes += 1 + *imgBytes;
	imgBytes++;
	cache_flushMem(this, sizeof(OneGifImage));
	bzero(&gce, 4); // default for next image
	break;
      }
      case 0x3B: // EOF
	imgBytes++;
	notDone = 0;
	break;
      default:
	printf("Unknown marker %d at %d\n", *imgBytes, imgBytes - base);
	imgBytes++;
	notDone = 0;
	break;
      }
    }
  }
  
  cache_flushMem(res, sizeof(GifImage));
  return res;
}
