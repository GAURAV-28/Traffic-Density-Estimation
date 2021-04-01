#include "opencv2/opencv.hpp"
#include <iostream>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;
using namespace cv;
using namespace std::chrono;

int X = 400;
int Y = 1000;
// This function is used to calculate number of white pixels in the frame matrix.
int countPixel(Mat image) { 

  //set values according to values of the image
  int channels = image.channels(); 
  int num_rows = image.rows; 
  int num_cols = (image.cols * channels); 
  
  int y=0;

  if (image.isContinuous()) { 
      num_cols *= num_rows; 
      num_rows = 1; 
  } 
  // use of nested loop for calculating count
  for (int i = 0; i < num_rows; ++i) { 
    uchar* row_ptr = image.ptr<uchar>(i); 
    for (int j = 0; j < num_cols; ++j) { 
        if(row_ptr[j]==255) y++;
    } 
  }

  return y; 
} 

// This is the function extracted from subtask 1 of the assignment.
// It takes input a matrix and outputs a matrix which is cropped and projected correctly.

Mat project_crop(Mat frame, double factor){
  Mat im_src=frame;
  Mat im_pro;
  Mat im_dst;
 
  Size pro_size(1280,875);                    
  Size dst_size(X*factor,Y*factor);                    

  vector<Point2f> pts_pro;
  vector<Point2f> pts_dst;
  vector<Point2f> p;

  p.push_back(Point2f(970,220));         
  p.push_back(Point2f(1270,220));
  p.push_back(Point2f(1520,1050));
  p.push_back(Point2f(530,1050));
  

  pts_pro.push_back(Point2f(472,52));        
  pts_pro.push_back(Point2f(800,52));
  pts_pro.push_back(Point2f(800,830));
  pts_pro.push_back(Point2f(472,830));

  pts_dst.push_back(Point2f(0,0));           
  pts_dst.push_back(Point2f(dst_size.width-1, 0));
  pts_dst.push_back(Point2f(dst_size.width-1, dst_size.height-1));
  pts_dst.push_back(Point2f(0, dst_size.height-1));

  Mat im_tmp = im_src.clone();        
  
  Mat h1 = findHomography(p, pts_pro);                           
  Mat h2 = findHomography(p, pts_dst);

  warpPerspective(im_src, im_pro, h1, pro_size);                  
  warpPerspective(im_src, im_dst, h2, dst_size);                  

  return im_dst;
}

// This is a caller function for counting number of white pixels and dividing by total pixels of cropped matrix (400x1000)
double process(Mat frame, double factor){

  Mat ff;
  Mat dst = frame;
  threshold(dst,ff, 100, 255, THRESH_BINARY);
  
  double d = ((double) countPixel(ff))/(X*Y*factor*factor);
  return d;

}

inline bool file_notexist (const std::string& name) {  // a function used to check a file with a given name exists in the working directory or not
  if (FILE *file = fopen(name.c_str(), "r")) {        // returns true if file doesn't exist
      fclose(file);                                   // returns false if a file with the given name exists
      return false;
    } else {
        return true;
    }   
}

// main function

int main(int argc, char const *argv[]){

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  if (argc!=3){       // unexpected no. of parameters
    cout << "Please run the program using this command format \"./output videofilename parameter for resolution\"\n";
    return 1;
  }
  string f = argv[1];  //file = the video's name
  string video = f+".mp4";

  if (file_notexist(video)){   // checks if file doesn't exist, prints error message if true
    cout << "Please enter a videofile that exists\n";
    cout << "Please run the program using this command format \"./output videofilename parameter for resolution\"\n";
    return 1;
  }

  VideoCapture cap(video);                     //capture the traffic video
  Ptr<BackgroundSubtractor> pBackSub1, pBackSub2;           //two background substractor pointer for queue and dynamic density
  pBackSub1 = createBackgroundSubtractorMOG2(500,32,false); //background substractor for queue density with suitable params

  Mat frame,fgMask1,crop,gray;

  

  if (file_notexist("bg.jpg")){   // checks if file doesn't exist, prints error message if true
    cout << "Please put the background file in the directory"<<endl;
    return 1;
  }

  double factor = stod(argv[2]);

  frame = imread("bg.jpg");             // empty background is taken as frame matrix, bg.jpg is extracted from the traffic video
  cvtColor(frame,gray,COLOR_BGR2GRAY);  // converting to gray scale
  crop = project_crop(gray,factor);            // apply projection and cropping this gray frame matrix (using subtask1 code)
  pBackSub1->apply(crop, fgMask1, 0);   // apply background substraction on initial frame to get fgMask1 keeping third param as 0 to fix this frame as background

  // if the video is not opened then suitable help is printed on the console
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return -1;
  }

  int count = 0; //initialize count for frames
  

  ofstream file;                        // for convience and making graphs easily, the output is saved in a csv file (output_x.txt)
  string outname = "M2output/out_with_factor_"+ to_string(factor) + ".txt";
  file.open(outname);

  while(1){

    cap >> frame; 
    if (frame.empty()) break; 
    count++;             
    
    cvtColor(frame,gray,COLOR_BGR2GRAY);  // converted to gray scale
    crop = project_crop(gray,factor);            // apply projection and cropping this gray frame matrix (using subtask1 code)

    pBackSub1->apply(crop, fgMask1, 0);   // apply background substraction on this frame to get fgMask1 keeping third param as 0 to fix the background
    double res = process(fgMask1,factor);
    file<< (count) <<" "<< res <<endl; // computed queue and dynamic density saved in csv file
    
  }

  // all windows and files are closed.
  cap.release();
  file.close();
  destroyAllWindows();

  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  cout << time_span.count() << endl;
	
  return 0;
}
