/*
 * genptscl.c ---- generic, VERY SLOW putscanline routine
 *
 * Copyright (c) 1998 Hartmut Schirmer
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
 */

#include "globals.h"
#include "libgrx.h"
#include "grdriver.h"

/* will draw array of pixel values to current context          */

void _GrFrDrvGenericPutScanline(int x,int y,int w,
                                const GrxColor *scl, GrxColor op )
{
   GrxColor skipc;
   _GR_drawPix drawpixel;
   GRX_ENTER();
   drawpixel = CURC->gc_driver->drawpixel;
   DBGPRINTF(DBG_DRIVER,("x=%d, y=%d, w=%d, op=%lx\n",x,y,w,op));
   skipc = op ^ GRX_COLOR_MODE_IMAGE;
   op &= GRX_COLOR_MODE_MASK;
   for ( w += x; x < w; ++x) {
     GrxColor c = *(scl++);
     if (c != skipc) (*drawpixel)(x,y,(c|op));
   }
   GRX_LEAVE();
}
