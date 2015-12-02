/**
 ** ibuild.c ---- Source Image Utility
 **
 ** by Michal Stencl Copyright (c) 1998
 ** <e-mail>    - [stenclpmd@ba.telecom.sk]
 **
 ** This file is part of the GRX graphics library.
 **
 ** The GRX graphics library is free software; you can redistribute it
 ** and/or modify it under some conditions; see the "copying.grx" file
 ** for details.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** modifications by Hartmut Schirmer (c) 1998
 **
 **/

#include "libgrx.h"
#include "clipping.h"
#include "mempeek.h"
#include "image/image.h"

GrxImage *grx_image_create(const unsigned char *pixels,int w,int h,const GrxColorTable colors)
{
  GrxImage   *img;
  GRX_ENTER();
  img = NULL;
  if ( pixels ) do {
    GrxContext  ctx, save;
    GrxColor    col;
    int yy=0, xx;
    img = _GrImageAllocate(&ctx,w,h);
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
      } while(++xx < w);
    } while(++yy < h);
    *CURC = save;
  } while(0);
  GRX_RETURN(img);
}
