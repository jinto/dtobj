/*--------------------------------------------------------------------*/
/*;  Copyright (C) 1996                                               */
/*;  Associated Universities, Inc. Washington DC, USA.                */
/*;                                                                   */
/*;  This program is free software; you can redistribute it and/or    */
/*;  modify it under the terms of the GNU General Public License as   */
/*;  published by the Free Software Foundation; either version 2 of   */
/*;  the License, or (at your option) any later version.              */
/*;                                                                   */
/*;  This program is distributed in the hope that it will be useful,  */
/*;  but WITHOUT ANY WARRANTY; without even the implied warranty of   */
/*;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    */
/*;  GNU General Public License for more details.                     */
/*;                                                                   */
/*;  You should have received a copy of the GNU General Public        */
/*;  License along with this program; if not, write to the Free       */
/*;  Software Foundation, Inc., 675 Massachusetts Ave, Cambridge,     */
/*;  MA 02139, USA.                                                   */
/*;                                                                   */
/*;  Correspondence  should be addressed as follows:                  */
/*;         Internet email: bcotton@nrao.edu.                         */
/*;         Postal address: William Cotton                            */
/*;                         National Radio Astronomy Observatory      */
/*;                         520 Edgemont Road                         */
/*;                         Charlottesville, VA 22903-2475 USA        */
/*--------------------------------------------------------------------*/

/*
* added in memory compression for faster execution. 
*                                          - jaypark@gmail.com 2012.6.13
*/
#include <stdio.h>
#include <stdlib.h>
#include "jerror.h"
#include "jpeglib.h"

#include "jpegsubs.h"

extern unsigned char *g_jpegbuffer;         /* jpeg buffer for in memory conversion */
int nx, ny;       /* size of image */
int nonlinear = 0;    /* if true use nonlinear mapping */

static float vmax, vmin; /* max and min unscaled values */
JSAMPROW idata=NULL; /* buffer for scaled version of row */
struct jpeg_compress_struct cinfo; /* jpeg compress structure */
struct jpeg_error_mgr jerr;        /* jpeg error handler structure */
JSAMPROW row_pointer[1];	   /* pointer to a single row */

//************* routines for in memory conversion ***********/
#define OUTPUT_BUF_SIZE 4096 /* choose an efficiently fwrite’able size */

/* Expanded data destination object for memory output */

typedef struct {
	struct jpeg_destination_mgr pub; /* public fields */

	unsigned char ** outbuffer; /* target buffer */
	unsigned long * outsize;
	unsigned char * newbuffer; /* newly allocated buffer */
	JOCTET * buffer; /* start of buffer */
	size_t bufsize;
} my_mem_destination_mgr;

typedef my_mem_destination_mgr * my_mem_dest_ptr;

void init_mem_destination (j_compress_ptr cinfo)
{
	/* no work necessary here */
}

boolean empty_mem_output_buffer (j_compress_ptr cinfo)
{
	size_t nextsize;
	JOCTET * nextbuffer;
	my_mem_dest_ptr dest = (my_mem_dest_ptr) cinfo->dest;

	/* Try to allocate new buffer with double size */
	nextsize = dest->bufsize * 2;
	nextbuffer = (JOCTET *)malloc(nextsize);

	if (nextbuffer == NULL)
	ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 10);

	memcpy(nextbuffer, dest->buffer, dest->bufsize);

	if (dest->newbuffer != NULL)
	free(dest->newbuffer);

	dest->newbuffer = nextbuffer;

	dest->pub.next_output_byte = nextbuffer + dest->bufsize;
	dest->pub.free_in_buffer = dest->bufsize;

	dest->buffer = nextbuffer;
	dest->bufsize = nextsize;

	return TRUE;
}

void term_mem_destination (j_compress_ptr cinfo)
{
	my_mem_dest_ptr dest = (my_mem_dest_ptr) cinfo->dest;

	*dest->outbuffer = dest->buffer;
	*dest->outsize=dest->bufsize - dest->pub.free_in_buffer;
}

void jpeg_mem_dest (j_compress_ptr cinfo, unsigned char ** outbuffer, unsigned long * outsize)
{
	my_mem_dest_ptr dest;

	if (outbuffer == NULL || outsize == NULL) /* sanity check */
	ERREXIT(cinfo, JERR_BUFFER_SIZE);

	/* The destination object is made permanent so that multiple JPEG images
	* can be written to the same buffer without re-executing jpeg_mem_dest.
	*/
	if (cinfo->dest == NULL) { /* first time for this JPEG object? */
	cinfo->dest = (struct jpeg_destination_mgr *)
	(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
	sizeof(my_mem_destination_mgr));
	}

	dest = (my_mem_dest_ptr) cinfo->dest;
	dest->pub.init_destination = init_mem_destination;
	dest->pub.empty_output_buffer = empty_mem_output_buffer;
	dest->pub.term_destination = term_mem_destination;
	dest->outbuffer = outbuffer;
	dest->outsize = outsize;
	dest->newbuffer = NULL;

	if (*outbuffer == NULL || *outsize == 0) {
	/* Allocate initial buffer */
	dest->newbuffer = *outbuffer = (unsigned char*)malloc(OUTPUT_BUF_SIZE);
	if (dest->newbuffer == NULL)
	ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 10);
	*outsize = OUTPUT_BUF_SIZE;
	}

	dest->pub.next_output_byte = dest->buffer = *outbuffer;
	dest->pub.free_in_buffer = dest->bufsize = *outsize;
}
//************* end of routines for in memory conversion ***********/


void jpgini (int inx, int iny, float ivmax,  float ivmin, int *ierr)
/*--------------------------------------------------------------------*/
/*  Initializes i/o to jpeg output routines                           */
/*  Inputs:                                                           */
/*     iname    Name of output file                                   */
/*     ilname   Length of name                                        */
/*     inx      number of columns in image                            */
/*     iny      Number of rows in image                               */
/*     ivmax    Maximum image value (values larger get this value)    */
/*     ivmin    Minimum image value (values smaller get this value)   */
/*     nonLin   if > 0.0 use non linear function                      */
/*  Output:                                                           */
/*     ierr     0.0 => OK                                             */
/*--------------------------------------------------------------------*/
{
  int i;

	unsigned long outlen;

  /* get values */
  nx = inx;
  ny = iny;
  vmax = ivmax;
  vmin = ivmin;

  /* row buffer */
  if (idata) free(idata);
  idata = (JSAMPROW)malloc(nx); /* allocate for gray scale */
  if (idata == NULL) {
    fprintf(stderr, "can't allocate row buffer");
    *ierr = 2; /* set error return */
    return; /* failed */
  }
  for (i=0;i<nx;i++) idata[i] = 0;

  /* create jpeg structures */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  /* initialize jpeg output routines */

	jpeg_mem_dest (&cinfo,&g_jpegbuffer,&outlen );

/* set image parameters */
  cinfo.image_width = nx; 	/* image width and height, in pixels */
  cinfo.image_height = ny;
  cinfo.input_components = 1;	/* # of color components per pixel */
  cinfo.in_color_space = JCS_GRAYSCALE; /* colorspace of input image */

  jpeg_set_defaults(&cinfo); /* get default settings */

  /* Make optional parameter settings here */
  jpeg_set_quality (&cinfo, 100, TRUE); /* set quality factor */

  /* initialize compression */
  jpeg_start_compress(&cinfo, TRUE);

  *ierr = 0; /* OK */
} /* end of jpgini */

void jpgwri (float *data, float blank, int *ierr)
/*--------------------------------------------------------------------*/
/*  Write row of a jpeg image, floating values scaled and converted   */
/*  as specified to jpgini.  Grayscale only.                          */
/*  Only does gray scale at present                                   */
/*  Pure black reserved for blanked pixels.                           */
/*  Inputs:                                                           */
/*     data    floating values                                        */
/*     blank   value of undefined pixel                               */
/*  Output:                                                           */
/*     ierr     0.0 => OK                                             */
/*--------------------------------------------------------------------*/
{
  int i, icol, maxcolor=255;
  float val, irange, c1, c2;
  double arg;

  /* scaling parameters */
   irange = vmax - vmin;
   if (fabs(irange)<1.0e-25)
   	  irange = 1.0;
   else
      irange = 1.0 / irange;
   c1 = (maxcolor - 1.0) * irange;
   c2 = vmin * c1 - 0.5;

  /* convert row */
  for (i=0;i<nx;i++) {
    val = data[i];
    if (val==blank) { /* blanked */
      idata[i] = 0;
    }
    else
      { /* non blanked */
	if (val<vmin) val=vmin;
	if (val>vmax) val=vmax;
	if (nonlinear)  {/* nonlinear */
	  arg = ((val-vmin) * irange);
	  icol =  0.5 + ((maxcolor-1.0) * sqrt(arg));
	}
	else  /* Linear */
	  icol = c1 * val - c2;
	if (icol<1) icol = 1;  /* minimum color = 1 */
	if (icol>=maxcolor) icol = maxcolor-1;
	idata[i] = GETJSAMPLE(icol);
      }
  } /* end loop over row pixels */

  row_pointer[0] = (JSAMPROW)idata; /* set row pointer */

  /* compress/write row */
  jpeg_write_scanlines(&cinfo, row_pointer, 1);

  *ierr = 0; /* OK */
} /* end of jpgwri */

void jpgclo (int *ierr)
/*--------------------------------------------------------------------*/
/*  Close/flush i/o to jpeg image file                                */
/*  Output:                                                           */
/*     ierr     0.0 => OK                                             */
/*--------------------------------------------------------------------*/
{
  /* finish compression/ flush output */
  jpeg_finish_compress(&cinfo);

  jpeg_destroy_compress(&cinfo); /* delete jpeg structures */

  /* row buffer */
  if (idata) free(idata);
	idata=NULL;

  *ierr = 0; /* OK */
} /* end of jpgclo */

