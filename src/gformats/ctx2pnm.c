/*
 * ctx2pnm.c ---- saves a context in a PNM file
 *
 * Copyright (C) 2000,2001 Mariano Alvarez Fernandez
 * [e-mail: malfer@teleline.es]
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <grx/context.h>
#include <grx/draw.h>
#include <grx/extents.h>

/*
** grx_save_current_context_to_pbm - Dump a context in a PBM file (bitmap)
**
** This routine works both in RGB and palette modes
** If the pixel color isn't Black it asumes White
**
** Arguments:
**   ctx:   Context to be saved (NULL -> use current context)
**   pbmfn: Name of pbm file
**   docn:  string saved like a comment in the pbm file (can be NULL)
**
** Returns  0 on success
**         -1 on error
*/

int grx_save_current_context_to_pbm( GrxContext *grc, char *pbmfn, char *docn )
{
  FILE *f;
  GrxContext grcaux;
  char cab[81];
  int currentbyte = 0, currentbit = 7;
  int x, y;

  if( (f = fopen( pbmfn,"wb" )) == NULL ) return -1;
  
  grx_save_current_context( &grcaux );
  if( grc != NULL ) grx_set_current_context( grc );
  sprintf( cab,"P4\n#" );
  fwrite( cab,1,strlen( cab ),f );
  if( docn != NULL ) fwrite( docn,1,strlen( docn ), f );
  sprintf( cab,"\n%d %d\n",grx_get_width(),grx_get_height() );
  fwrite( cab,1,strlen( cab ),f );
  for( y=0; y<grx_get_height(); y++ ){
    for( x=0; x<grx_get_width(); x++ ){
      if( grx_get_pixel_at( x,y ) == grx_color_info_get_black() )
        currentbyte |= 1 << currentbit;
      currentbit--;
      if( currentbit < 0 ){
        fwrite( &currentbyte,1,1,f );
        currentbyte = 0;
        currentbit = 7;
        }
      }
    if( currentbit < 7 ){
      fwrite( &currentbyte,1,1,f );
      currentbyte = 0;
      currentbit = 7;
      }
    }
  grx_set_current_context( &grcaux );
  fclose( f );

  return 0;
}

/*
** grx_save_current_context_to_pgm - Dump a context in a PGM file (gray scale)
**
** This routine works both in RGB and palette modes
** The colors are quantized to gray scale using .299r + .587g + .114b
**
** Arguments:
**   ctx:   Context to be saved (NULL -> use current context)
**   pgmfn: Name of pgm file
**   docn:  string saved like a comment in the pgm file (can be NULL)
**
** Returns  0 on success
**         -1 on error
*/

int grx_save_current_context_to_pgm( GrxContext *grc, char *pgmfn, char *docn )
{
  FILE *f;
  GrxContext grcaux;
  char cab[81];
  unsigned char grey;
  unsigned char rgb[3];
  int x, y;

  if( (f = fopen( pgmfn,"wb" )) == NULL ) return -1;
  
  grx_save_current_context( &grcaux );
  if( grc != NULL ) grx_set_current_context( grc );
  sprintf( cab,"P5\n#" );
  fwrite( cab,1,strlen( cab ),f );
  if( docn != NULL ) fwrite( docn,1,strlen( docn ), f );
  sprintf( cab,"\n%d %d\n255\n",grx_get_width(),grx_get_height() );
  fwrite( cab,1,strlen( cab ),f );
  for( y=0; y<grx_get_height(); y++ )
    for( x=0; x<grx_get_width(); x++ ){
      grx_color_info_query_color( grx_get_pixel_at( x,y ),&rgb[0],&rgb[1],&rgb[2] );
      grey = (0.229 * rgb[0]) + (0.587 * rgb[1]) + (0.114 * rgb[2]);
      fwrite( &grey,1,1,f );
      }
  grx_set_current_context( &grcaux );
  fclose( f );

  return 0;
}

/*
** grx_save_current_context_to_ppm - Dump a context in a PPM file (real color)
**
** This routine works both in RGB and palette modes
**
** Arguments:
**   ctx:   Context to be saved (NULL -> use current context)
**   ppmfn: Name of ppm file
**   docn:  string saved like a comment in the ppm file (can be NULL)
**
** Returns  0 on success
**         -1 on error
*/

int grx_save_current_context_to_ppm( GrxContext *grc, char *ppmfn, char *docn )
{
  FILE *f;
  GrxContext grcaux;
  char cab[81];
  unsigned char brgb[3];
  int x, y;
  unsigned char r, g, b;

  if( (f = fopen( ppmfn,"wb" )) == NULL ) return -1;
  
  grx_save_current_context( &grcaux );
  if( grc != NULL ) grx_set_current_context( grc );
  sprintf( cab,"P6\n#" );
  fwrite( cab,1,strlen( cab ),f );
  if( docn != NULL ) fwrite( docn,1,strlen( docn ), f );
  sprintf( cab,"\n%d %d\n255\n",grx_get_width(),grx_get_height() );
  fwrite( cab,1,strlen( cab ),f );
  for( y=0; y<grx_get_height(); y++ )
    for( x=0; x<grx_get_width(); x++ ){
      grx_color_info_query_color( grx_get_pixel_at( x,y ),&r,&g,&b );
      brgb[0] = r;
      brgb[1] = g;
      brgb[2] = b;
      fwrite( brgb,1,3,f );
      }
  grx_set_current_context( &grcaux );
  fclose( f );

  return 0;
}
