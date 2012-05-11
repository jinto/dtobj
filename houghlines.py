#!/usr/bin/python
# This is a standalone program. Pass an image name as a first parameter of the program.

import sys
from math import sin,cos,sqrt
import cv

# toggle between CV_HOUGH_STANDARD and CV_HOUGH_PROBILISTIC
USE_STANDARD=0

if __name__ == "__main__":
    filename = "building.jpg"
    filename = "output/20120308_edit_00.jpg"

    if len(sys.argv)>1:
        filename = sys.argv[1]

    src=cv.LoadImage(filename, 0);
    if not src:
        print "Error opening image %s" % filename
        sys.exit(-1)

    dst = cv.CreateImage( cv.GetSize(src), 8, 1 );
    color_dst = cv.CreateImage( cv.GetSize(src), 8, 3 );
    storage = cv.CreateMemStorage(0);
    lines = 0;
    cv.Canny( src, dst, 50, 200, 3 );
    cv.CvtColor( dst, color_dst, cv.CV_GRAY2BGR );

    if USE_STANDARD:
        lines = cv.HoughLines2( dst, storage, cv.CV_HOUGH_STANDARD, 1, cv.CV_PI/180, 100, 0, 0 );

        for i in range(min(lines.total, 100)):
            line = lines[i]
            rho = line[0];
            theta = line[1];
            pt1 = cv.CvPoint();
            pt2 = cv.CvPoint();
            a = cos(theta);
            b = sin(theta);
            x0 = a*rho 
            y0 = b*rho
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            cv.Line( color_dst, pt1, pt2, cv.CV_RGB(255,0,0), 3, 8 );

    else:
        lines = cv.HoughLines2( dst, storage, cv.CV_HOUGH_PROBABILISTIC, 1, cv.CV_PI/180, 50, 50, 10 );
        for line in lines:
            cv.Line( color_dst, line[0], line[1], cv.CV_RGB(255,0,0), 3, 8 );

    cv.NamedWindow( "Source", 1 );
    cv.ShowImage( "Source", src );

    cv.NamedWindow( "Hough", 1 );
    cv.ShowImage( "Hough", color_dst );
    cv.WaitKey(0);
