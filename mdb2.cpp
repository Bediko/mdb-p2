#include <queue>
#include <list>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

#include "tinydir.h"

#define PI 3.1415

using namespace cv;
using namespace std;

list<pair<Mat, string> > readImagesFromDirectory(string dirpath = "bilder")
{
    list<pair<Mat, string> > imagelist;
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
    for (iter = filenames.begin(); iter != filenames.end(); iter++)
    {
        // create image from file
        Mat img = imread(dirpath + "/" + (*iter), 0);
        if (img.data)
        {
            imagelist.push_back(pair<Mat, string>(img, *iter));
            /* ostringstream os;
             os << "original " << i << ".png";
             namedWindow(os.str(), CV_WINDOW_AUTOSIZE);
             imshow(os.str(), img);*/
        }
        i++;
    }

    return imagelist;
}

double calculateCoarseness(Mat image)
{
    //double a[6][image.cols][image.rows];
    Mat a[6];
    Mat eh[6];
    Mat ev[6];
    int histo[5]={0};
    Mat sbest = Mat(image.rows, image.cols, CV_64F);
    Mat bin = Mat(image.rows, image.cols, CV_64F);
    for (int k = 0; k < 6; k++)
    {
        a[k] = Mat(image.rows, image.cols, CV_64F);
        eh[k] = Mat(image.rows, image.cols, CV_64F);
        ev[k] = Mat(image.rows, image.cols, CV_64F);
    }
    cout << "calculate a..." << endl;
    for (int k = 1; k < 6; k++)
    {
        //pow(2,k)/2 should be enough
        for (int x = pow(2, k -1); x < image.cols - pow(2, k -1) ; x++)
        {
            for (int y = pow(2, k -1) ; y < image.rows - pow(2, k -1) ; y++)
            {
                for (int i = x - pow(2, k - 1); i < x + pow(2, k - 1) - 1; i++)
                {
                    for (int j = y - pow(2, k - 1); j < y + pow(2, k - 1) - 1; j++)
                    {
                        a[k].at<double>(y, x) += ((int)image.at<uchar>(i, j)) / (pow(2, 2 * k));
                        //cout<<"a["<<k<<"]"<<a[k]<<" i "<<i<<" j "<<j<<" y "<<y<<endl;
                    }
                }//cout<<"tur"<<endl;
                //cout<<"hu"<<endl;
            }
        }
    }
    cout << "calculate eh and ev..." << endl;
    for (int k = 1; k < 6; k++)
    {
        //pow(2,k)/2 should be enough
        for (int x = pow(2, k -1) ; x < image.cols - pow(2, k -1) ; x++)
        {
            for (int y = pow(2, k -1) ; y < image.rows - pow(2, k -1) ; y++)
            {
                eh[k].at<double>(y, x) = fabs(a[k].at<double>(x + pow(2, k - 1), y) - a[k].at<double>(x - pow(2, k - 1), y));
                ev[k].at<double>(y, x) = fabs(a[k].at<double>(x, y + pow(2, k - 1)) - a[k].at<double>(x, y - pow(2, k - 1)));
                //cout<<"a["<<k<<"]"<<a[k]<<" i "<<i<<" j "<<j<<" y "<<y<<endl;
            }
        }
    }
    cout<<"calculate sbest..."<<endl;
    for (int x = 0; x < image.cols; x++)
    {
        for (int y = 0; y < image.rows; y++)
        {
            double max = 0;
            sbest.at<double>(x, y)=2;
            bin.at<double>(x, y) = 0;
            for (int k = 1; k < 6; k++)
            {
                if (eh[k].at<double>(x, y) >= max)
                {
                    max = eh[k].at<double>(x, y);
                    sbest.at<double>(x, y) = pow(2, k);
                    bin.at<double>(x, y) = k;

                }
                if (ev[k].at<double>(x, y) >= max)
                {
                    max = ev[k].at<double>(x, y);
                    sbest.at<double>(x, y) = pow(2, k);
                    bin.at<double>(x, y) = k;
                }
            }
        }
    }
    cout<<"calculate fcrs..."<<endl;
    double fcrs = 0;
    for (int x = 0; x < image.cols; x++)
    {
        for (int y = 0; y < image.rows; y++)
        {
            fcrs += sbest.at<double>(x, y);
        }
    }
    fcrs = fcrs / (image.cols * image.rows);
    //cout<<fcrs<<endl;
    for(int i=0;i<6;i++)
        histo[i]=0;
    for (int x = 0; x < image.cols; x++)
    {
        for (int y = 0; y < image.rows; y++)
        {
            histo[(int)bin.at<double>(x, y)]++;
        }
    }
    for(int i=0;i<6;i++)
        cout<<pow(2,i)<<": "<<histo[i]<<endl;
    return fcrs;
}

int main( int argc, char **argv )
{
    list<pair<Mat, string> > images = readImagesFromDirectory();
    int i = 0;
    double a;
    //a = calculateCoarseness(images.front());
    while (!images.empty())
    {
        pair<Mat, string>  picture = images.front();
        Mat image = picture.first;
        images.pop_front();
        a = calculateCoarseness(image);
        imshow(picture.second.c_str(), picture.first);
        cout << picture.second << ": " << a << endl;
    }
    waitKey(0);

}