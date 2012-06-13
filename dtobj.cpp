/* 
 * Detect Object In the sky.
 * Last version will be on https://github.com/jinto/dtobj
 *
 * Author : jaypark@gmail.com
 */
#include <stdio.h>
#include <fitsio.h>
#include <highgui.h>
#include <cv.h>
#include "jpegsubs.h"


/* global variables */
char *infile = NULL;          /* input FITS file name */
char *outfile= NULL;         /* output jpeg file name */
unsigned char *g_jpegbuffer = NULL;         /* output jpeg file name */
float *DataArray=NULL; /* image data as 1-D array */
float fblank = 1.234567e25; 	/*  Set undefined value */
/* undefined pixel value */
float vmax,vmin;       /* Max. and Min. values to be displayed */
fitsfile *fptr;        /* cfitsio i/o file pointer */

/* internal prototypes */
void commandline (int argc, char **argv);
void Usage(void);
void jpegim (long inaxes[7], int *ierr);

void write_jpg(long inaxes[7], int *ierr);
bool find_st_lines(char* fname) ;


/*----------------------------------------------------------------------- */
/*   Program to convert a FITS image to a jpeg file */
/*----------------------------------------------------------------------- */
int main ( int argc, char **argv )
{
  int  i, iret=0, total;
  int  lenprefix;
  float g_vmax, g_vmin;
	char *g_outfile;
	long g_inaxes[7];        /* dimensions of axes */
  commandline (argc, argv); 	/* Startup */
  if (iret!=0) return iret;
  jpegim(g_inaxes, &iret); 								/* Convert to jpeg */
  if (iret!=0) return iret;
  fprintf(stderr,"vmax :%f\n", vmax);
  fprintf(stderr,"vmin :%f\n", vmin);
	g_vmax=vmax;
	g_vmin=vmin;

	//g_vmin=2344;
	//g_vmax=2346;
/*vmax :12682.000000
vmin :2275.000000

	 -max 2344 -min 2343
	outfile = argv[++ax];*/
	
	bool detected[(int)g_vmax+10];
	memset(detected, 0, g_vmax);
	g_outfile = (char*)malloc(strlen(infile)+40);

	for (i = (int)g_vmin; i<(int)g_vmax;i+=5) {
	//for (i = (int)g_vmin; i<(int)g_vmax;i++) {
		vmin=i;
		vmax=i+15;
		//vmax=i+2;
		if(g_jpegbuffer != NULL) {
			free(g_jpegbuffer);
			g_jpegbuffer=NULL;
		}
		outfile = g_outfile;
		sprintf(outfile, "%s_%04.0f_1.jpg", infile,vmin);
		printf("testing min:%d, range:%d...",(int)vmin, (int)(vmax-vmin));
		write_jpg(g_inaxes, &iret);

		if(find_st_lines(g_outfile)){
			detected[i] = true;
		}
	}

	for(i = 0; i < g_vmax-5; i++) {
		//printf("%d,%d\n",i,detected[i]);
		if (detected[i] ==true) {
			for(int j = i-5; j< i+5; j++)
				detected[j] = true;
			i+=7;
		}
	}	

	for (i = (int)g_vmin; i<(int)g_vmax;i++) {
		if(detected[i] == false)
			continue;

		vmin=i;
		vmax=i+2;
		if(g_jpegbuffer != NULL) {
			free(g_jpegbuffer);
			g_jpegbuffer=NULL;
		}
		outfile = g_outfile;
		sprintf(outfile, "%s_%04.0f_1.jpg", infile,vmin);
		printf("testing min:%d, range:%d...",(int)vmin, (int)(vmax-vmin));
		write_jpg(g_inaxes, &iret);

		find_st_lines(g_outfile);
	}
	free(g_outfile);
  return iret;
} 

bool find_st_lines(char* fname) {
	int removed=0;
	uchar *data;
	int i,j,k;
	int min,max;
			 
	cv::Mat imgbuf = cv::Mat(480, 640, CV_8U, g_jpegbuffer);
	//cv::Mat isrc = cv::imdecode(imgbuf, CV_LOAD_IMAGE_COLOR);
	cv::Mat isrc = cv::imdecode(imgbuf, CV_LOAD_IMAGE_GRAYSCALE);
  IplImage psrc(isrc);
  IplImage *src =&psrc;
  //IplImage* src=cvLoadImage(fname, CV_LOAD_IMAGE_GRAYSCALE) ;
  if( src != NULL) 
  {
    IplImage* dst = cvCreateImage( cvGetSize(src), 8, 1 );
    IplImage* color_dst = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 3 );
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    int i;

    //cvNot(src, dst);
		// 테스트삼아 먼저 엣지를 만든다음, 원본으로 다시 시도
		// 전처리가 끝난 파일의 경우에는, 구간을 300이상으로 잡고, 엣지디텍션을 먼저 수행하도록 해야한다.
    cvCanny(src, dst, 10, 30, 3);
    lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC,
                           2,// 픽셀 2
                           CV_PI/360,
                           //190,
                           80,
                           50, //50
                           10 ); //10

    for( i = 0; i < lines->total; i++ )
    {
      CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			//if(abs(line[0].y- line[1].y)<2 && (line[0].y < 10 || line[0].y > dst->height-10)) {
			if(abs(line[0].y- line[1].y)<2 && (line[0].y < min || line[0].y > max)) {
				removed++;
				continue;
			}
			if(abs(line[0].x- line[1].x)<2 && (line[0].x < min || line[0].x > max)) {
				removed++;
				continue;
			}
		}
		if(lines->total-removed <= 2 || lines->total-removed > 600) {
			unlink(fname);
			cvReleaseImage(&dst);
			cvReleaseImage(&color_dst);
			printf("lines: %d\n",lines->total - removed);
			return false;
		}
		printf("lines: %d\n",lines->total - removed);
		removed=0;

    //cvCanny(src, dst, 20, 200, 3);
    //cvCanny(src, dst, 250, 400, 3);
    //cvCanny(src, dst, 10, 30);
    //cvCvtColor( src, dst, CV_RGB2GRAY );
		cvThreshold(src, dst, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    cvCvtColor( dst, color_dst, CV_GRAY2BGR );

		printf("detecting... again\n");
    lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC,
                           2,// 픽셀
                           CV_PI/180,
                           //190,
                           80,
                           50,
                           10 );

		printf("line candidates:%d\n", lines->total);
		min = 10;
		max = dst->height-10;
		printf("image height:%d\n",max+10);
    for( i = 0; i < lines->total; i++ )
    {
      CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			if(abs(line[0].y- line[1].y)<2 && (line[0].y < min || line[0].y > max)) {
				removed++;
				continue;
			}
			if(abs(line[0].x- line[1].x)<2 && (line[0].x < min || line[0].x > max)) {
				removed++;
				continue;
			}
      cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 2, 8 );
			/*printf("\nline:%d:%d,%d:%d",line[0].x,line[0].y, line[1].x,line[1].y);
			CvLineIterator iter ;
			int l = cvInitLineIterator(dst,line[0],line[1],&iter,4,0);
			for( int j=0;j<l;j++)
			{
        CV_NEXT_LINE_POINT(iter);

        // print the pixel coordinates: demonstrates how to calculate the coordinates 
        {
        int offset, x, y;
        // assume that ROI is not set, otherwise need to take it into account. 
        offset = iter.ptr - (uchar*)(dst->imageData);
        y = offset/dst->widthStep;
        x = (offset - y*dst->widthStep)/(1*sizeof(uchar)); // size of pixel 
        //printf("(%d,%d)\n", x, y );
        }

				//printf("\npoint:%d:\n",((CvPoint*)iter.ptr)->x);
				CV_NEXT_LINE_POINT(iter);
			}*/


    }

		CvPoint line[2];
		line[0].x=0;
		line[0].y= 81.039624;
		line[1].x=500;
		line[1].y= 85.410814281408;
		char *tfname = (char*)malloc(strlen(fname)+40);
		char *sfname = (char*)malloc(strlen(fname)+1);
		strcpy(sfname, infile);
		if(strlen(sfname) > 5) {
			*(sfname + strlen(sfname)-4)=0;
		}
		sprintf(tfname, "output/%s_%04.0f.jpg", sfname,vmin);
		unlink(tfname);

		if (lines->total -removed> 0 && lines->total-removed < 30) {
			printf("lines :%d, saving..\n",lines->total - removed);
			cvSaveImage(tfname, color_dst);
			free(tfname);
			free(sfname);
			cvReleaseImage(&dst);
			cvReleaseImage(&color_dst);
			return true;
		}
		else printf("lines: %d\n",lines->total - removed);

		free(tfname);
		free(sfname);
		cvReleaseImage(&dst);
		cvReleaseImage(&color_dst);
		return false;
  }
}





void Usage()
{
    fprintf(stderr, "Usage: fits2jpeg fits_file\n");
    //fprintf(stderr, "        range :    range of ccd value for translating fits to jpeg (integer)\n");
    fprintf(stderr, "        fitsfile : filename of fits\n");
    fprintf(stderr, "                   must be ./samples/ subdirectory of current directory\n");
    exit(1); /* bail out */
}

void jpegim (long inaxes[7], int *ierr)
/*----------------------------------------------------------------------- */
/*   Copy FITS file  to jpeg */
/*   Inputs in common: */
/*      infile    Input FITS file name */
/*      outfil    Output jpeg file name */
/*   Output: */
/*      ierr      Error code: 0 => ok */
/*----------------------------------------------------------------------- */
{
  int   naxis, iln,  donon, lname;

	char* iinfile = (char*)malloc(strlen(infile)+40);
	strcpy(iinfile, "samples/");
	strcat(iinfile, infile);
  *ierr = 0;
/*                                       Open */
  if ( fits_open_file(&fptr, iinfile, READWRITE, ierr) ) {
    fprintf(stderr,"ERROR  opening input FITS file %s \n", iinfile);
    *ierr = 1;
		free(iinfile);
    return;
  }
	free(iinfile);

/*                                       Get header information */
	{
		int _bitpix, _simple, _extend, _maxdim=7;
		long _pcount, _gcount;

		fits_read_imghdr (fptr, _maxdim, &_simple, &_bitpix, &naxis, inaxes, &_pcount, &_gcount, &_extend, ierr);
		
		fprintf(stderr,"fits_read_imghdr naxis: %d\n", naxis);
		if (*ierr!=0) {
			fprintf(stderr,"fits_read_imghdr error %d reading FITS header \n", *ierr);
			return;
		}
	}

/*                                       Read FITS image */
	{
		long size,trc[7];
		int i, anyf;

		size = inaxes[0]; 															/* How big is the array? */
		size = size * inaxes[1];
		if (DataArray) free(DataArray); 								/* free any old allocations */ 
		DataArray = (float*)malloc(sizeof(float)*size);	
		if (!DataArray) { 															
			fprintf(stderr,"Cannot allocate memory for image array \n");
			*ierr = 1;
			return;
		}
	/*                                       Take all of image */
		trc[0] = inaxes[0];
		trc[1] = inaxes[1];
	/*                                       but only first plane */
		//for (i=2;i<naxis;i++) trc[i] = 1;
		for (i=2;i< naxis;i++) trc[i] = 0;
	/*                                       Read selected portion of input */
		int group=0;
		long incs[7]={1,1,1,1,1,1,1},blc[7]={1,1,1,1,1,1,1};
		fits_read_subset_flt (fptr, group, naxis, inaxes, blc, trc, incs, fblank, DataArray, &anyf, ierr);
		if (*ierr!=0) {
			fprintf(stderr,"fits_read_subset_flt error reading input file \n");
			return;
		}
	/*                                      Update data max, min */
		float tmax,tmin;
		tmin = 1.0e20;
		tmax = -1.0e20;
		for (i=0;i<size;i++) {
			if ((!anyf) || (DataArray[i]!=fblank)) {
				if (DataArray[i]>tmax) tmax = DataArray[i];
				if (DataArray[i]<tmin) tmin = DataArray[i];
			}
		}

		vmin = tmin;
		vmax = tmax;
	} /* end getdat */
  if (*ierr!=0) {
    fprintf(stderr,"ERROR getting image pixel values \n");
    return;
  }
/*                                       Close FITS file */
  fits_close_file (fptr, ierr);
}



void write_jpg(long inaxes[7], int *ierr)
{
  int   i, irow,iptr, nx, ny, nrow, lrow;
/*                                       Initialize output */
  nx = inaxes[0];
  ny = inaxes[1];
  jpgini (nx, ny, vmax, vmin, ierr);
  if (*ierr!=0) {
    fprintf(stderr,"error %d initializing jpeg output \n", 
	    *ierr);
    return;
  }

/*                                       Write, loop over image */
/*                             write backwards to get right side up */
  lrow = inaxes[0];
  nrow = inaxes[1];
  iptr = (nrow-1) * lrow;
  irow = nrow;
  for (i=0;i<nrow;i++) {
    jpgwri(&DataArray[iptr], fblank, ierr);
    if (*ierr!=0) {
      fprintf(stderr,"ERROR %d compressing/writing row %d \n", 
	      *ierr, irow);
      return;
    }
    iptr = iptr - lrow;
    irow = irow - 1;
  } /* end loop writing rows */
/*                                       Close output */
  jpgclo (ierr);
    if (*ierr!=0) {
      fprintf(stderr,"ERROR %d closing output \n", *ierr);
      return;
    }
} /* end jpegim */

void commandline (int argc, char **argv)
{
  int ax;
  char *arg;

  if (argc<=1) Usage(); 
	infile = argv[1];
} 

