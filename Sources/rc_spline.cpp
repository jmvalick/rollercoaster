/*** @file rc_spline.cpp
*
*   @brief Implementation of the rc_Spline classes
*
*   You should add some code here probably.
**/

#ifdef WIN32
/* get rid of ridiculous warnings */
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "rc_spline.h"


/* load a spline segment from a file */
void rc_Spline::loadSegmentFrom(std::string filename)
{	
	filename = folder + filename;
	FILE* fileSplineSegment = fopen(filename.c_str(), "r");
	if (fileSplineSegment == NULL) 
	{
		printf ("can't open file %s\n", filename.c_str());
		exit(1);
	}

	int iLength;

	/* gets length for spline segment */
	fscanf(fileSplineSegment, "%d", &iLength);

	glm::vec3 pt;

	/* add it to the control point list */
	while (fscanf(fileSplineSegment, "%f %f %f", 
				&pt.x, &pt.y,&pt.z) != EOF) 
	{
		m_vPoints.push_back(pt);
	}

	/* now close the file */
	fclose(fileSplineSegment);
}


/* load a spline from a file */
void rc_Spline::loadSplineFrom(std::string filename)
{	
	filename = folder + filename;
	/* load the track file */
	FILE* fileSpline = fopen(filename.c_str(), "r");

	if (fileSpline == NULL) 
	{
		printf ("can't open file %s\n", filename.c_str());
		exit(1);
	}
  
	/* stores the number of splines in a global variable */
	int nSegments;
	fscanf(fileSpline, "%d", &nSegments);

	/* reads through the spline files */
	for (int j = 0; j < nSegments; j++) 
	{
		/* define a variable of a reasonable size for a filename */
		char segmentfilename[1024];

		fscanf(fileSpline, "%s", segmentfilename);
		loadSegmentFrom(segmentfilename);
	}

	/* now close the file */
	fclose(fileSpline);


}