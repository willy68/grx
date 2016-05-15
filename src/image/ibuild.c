/*
 * ibuild.c ---- Source Image Utility
 *
 * by Michal Stencl Copyright (c) 1998
 * <e-mail>    - [stenclpmd@ba.telecom.sk]
 *
 * This file is part of the GRX graphics library.
 *
 * The GRX graphics library is free software; you can redistribute it
 * and/or modify it under some conditions; see the "copying.grx" file
 * for details.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * modifications by Hartmut Schirmer (c) 1998
 *
 */

#include "libgrx.h"
#include "clipping.h"
#include "mempeek.h"
#include "image.h"

/**
 * grx_image_create:
 * @pixels: (array):
 * @width:
 * @height:
 * @colors: an array of integers with the first element being the number of
 *          colors in the table and the color values themselves starting with
 *          the second element. NOTE: any color modifiers (GrXOR, GrOR, GrAND)
 *          OR-ed to the elements of the color table are ignored.
 *
 * Builds a pixmap from a two dimensional (width by height) array of characters.
 * The elements in this array are used as indices into the color table specified
 * with the argument @colors. (This means that pixmaps created this way can use
 * at most 256 colors.)
 *
 * Returns: (transfer full):
 */
GrxImage *grx_image_create(const unsigned char *pixels, int width, int height,
                           const GrxColorTable colors)
{
  GrxImage   *img;
  GRX_ENTER();
  img = NULL;
  if ( pixels ) do {
    GrxContext  ctx, save;
    GrxColor    col;
    int yy=0, xx;
    img = _GrImageAllocate(&ctx,width,height);
    if ( !img ) break;
    save = *CURC;
    *CURC = ctx;
    do {
      xx = 0;
      do {
        col = peek_b(pixels);
        ptrinc(pixels,1);
        if ( colors ) col = GRX_COLOR_TABLE_GET_COLOR(colors,col);
        (*CURC->gc_driver->drawpixel)(xx, yy, (col & C_COLOR));
      } while(++xx < width);
    } while(++yy < height);
    *CURC = save;
  } while(0);
  GRX_RETURN(img);
}
