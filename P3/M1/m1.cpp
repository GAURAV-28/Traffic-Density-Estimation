#include "opencv2/opencv.hpp"
#include <iostream>
#include <bits/stdc++.h>

using namespace std;
using namespace cv;


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

Mat project_crop(Mat frame){
  Mat im_src=frame;
  Mat im_pro;
  Mat im_dst;
 
  Size pro_size(1280,875);                    
  Size dst_size(400,1000);                    

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
double process(Mat frame){

  Mat ff;
  Mat dst = frame;
  threshold(dst,ff, 100, 255, THRESH_BINARY);
  
  double d = (double) countPixel(ff)/(400000);
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

  time_t start = time(NULL);

  if (argc!=3){       // unexpected no. of parameters
    cout << "Please run the program using this command format \"./output videofilename paramater x\"\n";
    return 1;
  }
  string f = argv[1];  //file = the video's name
  string video = f+".mp4";
  int x = stoi(argv[2]);

  if (file_notexist(video)){   // checks if file doesn't exist, prints error message if true
    cout << "Please enter a videofile that exists\n";
    cout << "Please run the program using this command format \"./output videofilename parameter x\"\n";
    return 1;
  }

  VideoCapture cap(video);                     //capture the traffic video
  Ptr<BackgroundSubtractor> pBackSub1, pBackSub2;           //two background substractor pointer for queue and dynamic density
  pBackSub1 = createBackgroundSubtractorMOG2(500,32,false); //background substractor for queue density with suitable params
  pBackSub2 = createBackgroundSubtractorMOG2(500,16,true);  //background substractor for dynamic density with suitable params

  Mat frame,fgMask1,fgMask2,crop,gray,r,s;

  

  if (file_notexist("bg.jpg")){   // checks if file doesn't exist, prints error message if true
    cout << "Please put the background file in the directory"<<endl;
    return 1;
  }

  frame = imread("bg.jpg");             // empty background is taken as frame matrix, bg.jpg is extracted from the traffic video
  cvtColor(frame,gray,COLOR_BGR2GRAY);  // converting to gray scale
  crop = project_crop(gray);            // apply projection and cropping this gray frame matrix (using subtask1 code)
  pBackSub1->apply(crop, fgMask1, 0);   // apply background substraction on initial frame to get fgMask1 keeping third param as 0 to fix this frame as background
  pBackSub2->apply(frame , r, -1);      // apply background substraction on initial frame to get fgMask2 keeping third param as -1 to dynamically update the frame as background
  fgMask2 = project_crop(r);            // apply projection and cropping dynamic density mask matrix (using subtask1 code)
  

  // if the video is not opened then suitable help is printed on the console
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return -1;
  }

  int count = 0; //initialize count for frames


  ofstream file;                        // for convience and making graphs easily, the output is saved in a csv file (output_x.txt)
  string outname = "M1output/output_"+ to_string(x) + ".txt";
  file.open(outname);

  while(1){

    cap >> frame;                         // frame is taken from the video, count is increased
    if (frame.empty()) break; 
    count++;                
    cvtColor(frame,gray,COLOR_BGR2GRAY);  // converted to gray scale
    crop = project_crop(gray);            // apply projection and cropping this gray frame matrix (using subtask1 code)

    pBackSub1->apply(crop, fgMask1, 0);   // apply background substraction on this frame to get fgMask1 keeping third param as 0 to fix the background
    pBackSub2->apply(frame, r, -1);       // apply background substraction on this frame to get fgMask2 keeping third param as -1 to dynamically update the background
    fgMask2 = project_crop(r);            // // apply projection and cropping dynamic density mask matrix (using subtask1 code)
    double res = process(fgMask1);
    file<< (count) <<" "<< res <<endl; // computed queue and dynamic density saved in csv file
    //cout<< (count) <<" "<< res <<endl; // computed queue and dynamic density printed along with frame number
    

    for(int i=0; i<x-1;i++){
        cap >> frame;             // this frame is skipped for computation
        if (frame.empty()) break;
        count++;
        file<< (count) <<" "<< res <<endl; // computed queue and dynamic density saved in csv file
        //cout<< (count) <<" "<< res <<endl; // computed queue and dynamic density printed along with frame number
    }

    char c=(char)waitKey(10); // to update the video frames
    if(c==27)
      break;
  }

  // all windows and files are closed.
  cap.release();
  file.close();
  destroyAllWindows();

  time_t end = time(NULL);
  cout<< end-start << endl;
	
  return 0;
}