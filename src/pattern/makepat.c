/*
 * makepat.c
 *
 * Copyright (c) 1995 Csaba Biegl, 820 Stirrup Dr, Nashville, TN 37221
 * [e-mail: csaba@vuse.vanderbilt.edu]
 *
 * Copyright (C) 1992, Csaba Biegl
 *   820 Stirrup Dr, Nashville, TN, 37221
 *   csaba@vuse.vanderbilt.edu
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
 */

#include <glib-object.h>

#include <grx/pattern.h>

#include "colors.h"
#include "globals.h"
#include "libgrx.h"
#include "allocate.h"

#define  MY_MEMORY             1        /* set if my context memory */
#define  MY_CONTEXT            2        /* set if my context structure */

#define  BEST_MAX_LINE         128
#define  BEST_MAX_CONTEXT      2048L

G_DEFINE_BOXED_TYPE(GrxPattern, grx_pattern, grx_pattern_copy, grx_pattern_free);

/*
 * try to replicate a pixmap for faster bitblt-s
 * in bitplane modes it is especially desirable to replicate until
 * the width is a multiple of 8
 */
static int _GrBestPixmapWidth(int wdt,int hgt)
{
        long total   = grx_get_context_size(wdt,hgt);
        int  linelen = grx_get_line_offset(wdt);
        int  factor  = 1;
        int  test;

        if(total == 0L) return(0);
#ifdef _MAXMEMPLANESIZE
        if(total > _MAXMEMPLANESIZE) return(0);
#endif
        if((test = (int)(BEST_MAX_CONTEXT / total)) > factor)
            factor = test;
        if((test = (BEST_MAX_LINE / linelen)) < factor)
            factor = test;
        while((factor >>= 1) != 0)
            wdt <<= 1;
        return(wdt);
}

GrxPattern *grx_pattern_create_pixmap(const unsigned char *pixels,int w,int h,const GrxColorTable ct)
{
        GrxContext csave,cwork;
        GrxPixmap  *result;
        unsigned  char *src;
        int  wdt,wdt2,fullw;
        int  hgt;
        GrxColor color;

        if((fullw = _GrBestPixmapWidth(w,h)) <= 0) return(NULL);
        result = (GrxPixmap *)malloc(sizeof(GrxPixmap));
        if (result == NULL) return(NULL);

        if (!grx_context_create(fullw,h,NULL,&cwork)) {
          free(result);
          return NULL;
        }
        csave = *CURC;
        *CURC = cwork;
        for(hgt = 0; hgt < h; hgt++) {
            for(wdt2 = fullw; (wdt2 -= w) >= 0; ) {
                src = (unsigned char *)pixels;
                for(wdt = 0; wdt < w; wdt++) {
                    color = *src++;
                    if(ct != NULL) color = GRX_COLOR_TABLE_GET_COLOR(ct,color);
                    (*CURC->gc_driver->drawpixel)(wdt2+wdt,hgt,(color & C_COLOR));
                }
            }
            pixels += w;
        }
        *CURC = csave;
        result->source = cwork.frame;
        result->source.memory_flags = (MY_CONTEXT | MY_MEMORY);
        result->is_pixmap = TRUE;
        result->width  = fullw;
        result->height = h;
        result->mode   = 0;
        return((GrxPattern *)result);
}

GrxPattern *grx_pattern_create_pixmap_from_bits(const unsigned char *bits,int w,int h,GrxColor fgc,GrxColor bgc)
{
        GrxContext csave,cwork;
        GrxPixmap  *result;
        unsigned  char *src;
        int  wdt,wdt2,fullw;
        int  hgt,mask,byte;

        if((fullw = _GrBestPixmapWidth(w,h)) <= 0) return(NULL);
        result = (GrxPixmap *)malloc(sizeof(GrxPixmap));
        if(result == NULL) return(NULL);

        if (!grx_context_create(fullw,h,NULL,&cwork)) {
          free(result);
          return NULL;
        }
        csave = *CURC;
        *CURC = cwork;
        fgc &= C_COLOR;
        bgc &= C_COLOR;
        for(hgt = 0; hgt < h; hgt++) {
            for(wdt2 = fullw; (wdt2 -= w) >= 0; ) {
                src  = (unsigned char *)bits;
                mask = byte = 0;
                for(wdt = w; --wdt >= 0; ) {
                    if((mask >>= 1) == 0) { mask = 0x80; byte = *src++; }
                    (*CURC->gc_driver->drawpixel)(wdt2+wdt,hgt,((byte & mask) ? fgc : bgc));
                }
            }
            bits += (w + 7) >> 3;
        }
        *CURC = csave;
        result->source = cwork.frame;
        result->source.memory_flags = (MY_CONTEXT | MY_MEMORY);
        result->is_pixmap = TRUE;
        result->width  = fullw;
        result->height = h;
        result->mode   = 0;
        return((GrxPattern *)result);
}

GrxPattern *grx_pattern_create_pixmap_from_context(GrxContext *src)
{
        GrxPixmap *result;

        if(src->gc_is_on_screen) return(NULL);
        result = malloc(sizeof(GrxPixmap));
        if(result == NULL) return(NULL);
        result->source = src->frame;
        result->source.memory_flags = MY_CONTEXT;
        result->is_pixmap = TRUE;
        result->width  = src->x_max + 1;
        result->height = src->y_max + 1;
        result->mode   = 0;
        return((GrxPattern *)result);
}

/**
 * grx_pattern_copy:
 * @pattern:
 *
 * FIXME: This is not implemented and calling it will cause the program to abort.
 */
GrxPattern *grx_pattern_copy(GrxPattern *p)
{
    g_error("grx_pattern_copy is not implemented.\n");
    return p;
}

void grx_pattern_free(GrxPattern *p)
{
  if (!p) return;
  if (p->is_pixmap) {
    if ( p->gp_pxp_source.memory_flags & MY_MEMORY) {
      int ii;
      for ( ii = p->gp_pxp_source.driver->num_planes; ii > 0; ii-- )
         free(p->gp_pxp_source.base_address[ii - 1]);
    }
    if ( p->gp_pxp_source.memory_flags & MY_CONTEXT )
      free(p);
    return;
  }
  if ( p->bitmap.free_on_pattern_destroy ) free(p);
}
