/**
 ** patfplot.c
 **
 ** Copyright (c) 1995 Csaba Biegl, 820 Stirrup Dr, Nashville, TN 37221
 ** [e-mail: csaba@vuse.vanderbilt.edu]
 **
 **  Copyright (C) 1992, Csaba Biegl
 **    820 Stirrup Dr, Nashville, TN, 37221
 **    csaba@vuse.vanderbilt.edu
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
 **/

#include "libgrx.h"
#include "clipping.h"

void _GrPatternFilledPlot(int x,int y,GrxPattern *p)
{
        int xp,yp;

        if(p->is_pixmap) {
            xp = x % p->gp_pxp_width;
            yp = y % p->gp_pxp_height;
            (*CURC->gc_driver->drawpixel)(x,y,
                (*p->gp_pxp_source.driver->readpixel)(&p->gp_pxp_source,xp,yp)
            );
        }
        else {
            xp = x & 7;
            yp = y % p->gp_bmp_height;
            (*CURC->gc_driver->drawpixel)(x,y,
                (p->gp_bmp_data[yp] & (0x80U >> xp)) ? p->gp_bmp_fgcolor : p->gp_bmp_bgcolor
            );
        }
}

void grx_draw_filled_point_with_pattern(int x,int y,GrxPattern *p)
{
        clip_dot(CURC,x,y);
        mouse_block(CURC,x,y,x,y);
        _GrPatternFilledPlot(x+CURC->x_offset,y+CURC->y_offset, p);
        mouse_unblock();
}

