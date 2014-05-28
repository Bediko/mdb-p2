#include <queue>
#include <list>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

#include "tinydir.h"

#define PI 3.1415

#define HMAX 255
#define SMAX 255
#define VMAX 255

#define HBINS 16
#define SBINS 3
#define VBINS 3

const double DHMAX = 360;
const double DSMAX = 1;
const double DVMAX = 1;

const int HBIN_SIZE = HMAX / HBINS;
const int SBIN_SIZE = SMAX / SBINS;
const int VBIN_SIZE = VMAX / VBINS;


using namespace cv;
using namespace std;

list<Mat> readImagesFromDirectory(string dirpath = "bilder") {
	cout<<"huh"<<endl;
	list<Mat> imagelist;
	list<string> filenames;	
	// create directory object and open it
	tinydir_dir dir;
	tinydir_open(&dir, dirpath.c_str());

	while (dir.has_next)
	{
		// create file
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		
		
		// if file isnt a directory and if file isnt the DICOMDIR file
		if (!file.is_dir)
		{
			// parse 
			filenames.push_back(file.name);
		}
		
		// read next file
		tinydir_next(&dir);
	}
	
	// close directory object
	tinydir_close(&dir);
	
	// sort filenames
	filenames.sort();

	list<string>::iterator iter;
	int i = 0;
	for(iter = filenames.begin(); iter != filenames.end(); iter++)
	{
		// create image from file
		Mat img = imread(dirpath+"/"+(*iter), 0);
		if(img.data)
		{
			imagelist.push_back(img);
			ostringstream os;
			os << "original " << i << ".png";
			namedWindow(os.str(), CV_WINDOW_AUTOSIZE);
			imshow(os.str(), img);
		}
		i++;
	}

	return imagelist;
}

int main( int argc, char** argv )
{
	list<Mat> images=readImagesFromDirectory();
	int i=0;
	while(!images.empty()){
		Mat image=images.front();
		images.pop_front();
		ostringstream os;
		os << i;
		namedWindow(os.str(), CV_WINDOW_AUTOSIZE);
		imshow(os.str(), image);
		cout<<i<<endl;
		i++;
	}
	waitKey(0);

}