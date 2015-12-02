/**
 ** imginlne.h ---- Image Utility
 **
 ** by Michal Stencl Copyright (c) 1998 for GRX
 **  <e-mail>      - [stenclpmd@ba.telecom.sk]
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
#include "image/image.h"

GrImage *(GrImageFromPattern)(GrxPattern *p) {
  return GrImageFromPattern(p);
}

GrImage *(GrImageFromContext)(GrxContext *c) {
  return GrImageFromContext(c);
}

GrxPattern *(GrPatternFromImage)(GrImage *p) {
  return GrPatternFromImage(p);
}

GrImage *(GrImageBuildUsedAsPattern)(const char *pixels,int w,int h,
                                     const GrxColorTable colors     ) {
  return GrImageBuildUsedAsPattern(pixels,w,h,colors);
}

void (GrImageDestroy)(GrImage *i) {
  GrImageDestroy(i);
}



