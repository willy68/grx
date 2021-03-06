/*
 * upixelc.c
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
 */

#include <grx/context.h>

#include "libgrx.h"
#include "usercord.h"

/**
 * grx_context_get_pixel_at_user:
 * @context: the context
 * @x: the X coordinate
 * @y: the Y coordinate
 *
 * Gets the color value of the pixel in the context at the specified
 * coordinates.
 *
 * Also see grx_get_pixel_at() for operating on the current context.
 */
GrxColor grx_context_get_pixel_at_user(GrxContext *c,int x,int y)
{
        U2SX(x,c);
        U2SY(y,c);
        return(grx_context_get_pixel_at(c,x,y));
}
