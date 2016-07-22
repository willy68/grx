/*
 * ctx2jpg.c ---- saves a context to a JPEG file
 *
 * Copyright (C) 2001 Mariano Alvarez Fernandez
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

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <jpeglib.h>

#include <grx/context.h>
#include <grx/draw.h>
#include <grx/extents.h>

static int writejpeg( FILE *f, GrxContext *grc, int quality, int grayscale );

/*
** grx_save_current_context_to_jpeg - Dump a context in a JPEG file
**
** This routine works both in RGB and palette modes
**
** Arguments:
**   grc:     Context to be saved (NULL -> use current context)
**   jpegfn:  Name of jpeg file
**   quality: quality scaling (1 to 100) the normal value is 75
**
** Returns  0 on success
**         -1 on error
*/

int grx_save_current_context_to_jpeg( GrxContext *grc, char *jpegfn, int quality )
{
  GrxContext grcaux;
  FILE *f;
  int r;
  
  f = fopen( jpegfn,"wb" );
  if( f == NULL ) return -1;

  grx_save_current_context( &grcaux );
  if( grc != NULL ) grx_set_current_context( grc );
  r = writejpeg( f,grc,quality,0 );
  grx_set_current_context( &grcaux );

  fclose( f );

  return r;
}

/*
** grx_save_current_context_to_jpeg_grayscale - Dump a context in a Gray JPEG file
**
** This routine works both in RGB and palette modes
**
** Arguments:
**   grc:     Context to be saved (NULL -> use current context)
**   jpegfn:  Name of jpeg file
**   quality: quality scaling (1 to 100) the normal value is 75
**
** Returns  0 on success
**         -1 on error
*/

int grx_save_current_context_to_jpeg_grayscale( GrxContext *grc, char *jpegfn, int quality )
{
  GrxContext grcaux;
  FILE *f;
  int r;
  
  f = fopen( jpegfn,"wb" );
  if( f == NULL ) return -1;

  grx_save_current_context( &grcaux );
  if( grc != NULL ) grx_set_current_context( grc );
  r = writejpeg( f,grc,quality,1 );
  grx_set_current_context( &grcaux );

  fclose( f );

  return r;
}

/**/
/**/

struct my_error_mgr{
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
  };

typedef struct my_error_mgr *my_error_ptr;

/**/

METHODDEF(void) my_error_exit( j_common_ptr cinfo )
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /*(*cinfo->err_>output_message)( cinfo );*/

  longjmp( myerr->setjmp_buffer,1 );
}

/**/

static int writejpeg( FILE *f, GrxContext *grc, int quality, int grayscale )
{
  struct jpeg_compress_struct cinfo;
  struct my_error_mgr jerr;
  JSAMPROW row_pointer[1];
  static unsigned char *buffer = NULL;
  unsigned char *pix_ptr;
  const GrxColor *pColors;
  int row_stride;
  int x, y;
  unsigned char r, g, b;

  cinfo.err = jpeg_std_error( &jerr.pub );
  jerr.pub.error_exit = my_error_exit;
  if( setjmp( jerr.setjmp_buffer ) ) {
    if( buffer ) free( buffer );
    jpeg_destroy_compress( &cinfo );
    return -1;
  }

  jpeg_create_compress( &cinfo );
  jpeg_stdio_dest( &cinfo,f );

  cinfo.image_width = grx_get_width();
  cinfo.image_height = grx_get_height();
  if( grayscale ){
    cinfo.input_components = 1;
    cinfo.in_color_space = JCS_GRAYSCALE;
    }
  else{
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    }

  jpeg_set_defaults( &cinfo );
  jpeg_set_quality( &cinfo,quality,TRUE );

  jpeg_start_compress( &cinfo, TRUE );

  row_stride = cinfo.image_width * 3;
  buffer = malloc( row_stride );
  if( buffer == NULL ) longjmp( jerr.setjmp_buffer,1 );
  row_pointer[0] = buffer;

  for( y=0; y<cinfo.image_height; y++ ){
    pColors = grx_get_scanline( 0,cinfo.image_width-1,y,NULL );
    pix_ptr = buffer;
    if( grayscale ){
      for( x=0; x<cinfo.image_width; x++ ){
        grx_color_info_query_color( pColors[x],&r,&g,&b );
        *pix_ptr++ = (0.229 * r) + (0.587 * g) + (0.114 * b);
        }
      }
    else{
      for( x=0; x<cinfo.image_width; x++ ){
        grx_color_info_query_color( pColors[x],&r,&g,&b );
        *pix_ptr++ = r;
        *pix_ptr++ = g;
        *pix_ptr++ = b;
        }
      }
    jpeg_write_scanlines( &cinfo,row_pointer,1 );
  }

  jpeg_finish_compress( &cinfo );
  jpeg_destroy_compress( &cinfo );

  return 0;
}
