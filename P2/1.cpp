#include "opencv2/opencv.hpp"
#include <iostream>
#include <bits/stdc++.h>

using namespace std;
using namespace cv;

int scanImage(Mat image) { 
  int channels = image.channels(); 
  int num_rows = image.rows; 
  int num_cols = (image.cols * channels); 
  
  int y=0;

  if (image.isContinuous()) { 
      num_cols *= num_rows; 
      num_rows = 1; 
  } 
 
  for (int i = 0; i < num_rows; ++i) { 
    uchar* row_ptr = image.ptr<uchar>(i); 
    for (int j = 0; j < num_cols; ++j) { 
        if(row_ptr[j]!=0) y++;
    } 
  }

  return y; 
} 


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
  

  pts_pro.push_back(Point2f(472,52));         // hardcodes the homography hpoints of the perspective image using riju ma'am's points
  pts_pro.push_back(Point2f(800,52));
  pts_pro.push_back(Point2f(800,830));
  pts_pro.push_back(Point2f(472,830));

  pts_dst.push_back(Point2f(0,0));            // homography points of the cropped image is the corner points of the image
  pts_dst.push_back(Point2f(dst_size.width-1, 0));
  pts_dst.push_back(Point2f(dst_size.width-1, dst_size.height-1));
  pts_dst.push_back(Point2f(0, dst_size.height-1));

  Mat im_tmp = im_src.clone();        
  
  Mat h1 = findHomography(p, pts_pro);                           // calculating homography using opencv function(original image, points chosen of that image)
  Mat h2 = findHomography(p, pts_dst);

  warpPerspective(im_src, im_pro, h1, pro_size);                  // outputs the im_src after applying homography using h1 and pro_size to im_pro
  warpPerspective(im_src, im_dst, h2, dst_size);                  

  return im_dst;
}


double process(Mat frame, int count){

  Mat ff = project_crop(frame);
  imshow("Projected-Image-WithBGsbr", ff);
  double d = (double) scanImage(ff)/(400000);
  //cout<<"Frame No: "<<count<<" Queue Density: "<<d<<endl;
  return d;

}

int main(){
  // Create a VideoCapture object and open the input file
  // If the input is the web camera, pass 0 instead of the video file name
  VideoCapture cap("trafficvideo.mp4"); 

  //Mat bgr = imread("empty_cropped.jpg", IMREAD_GRAYSCALE);

  Ptr<BackgroundSubtractor> pBackSub;
    
  pBackSub = createBackgroundSubtractorMOG2();

  ofstream file;
  file.open("out.txt");

  // Check if camera opened successfully
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    return -1;
  }

  time_t in = time(NULL);
  int count = 0;

  while(1){
    
    Mat frame,fgMask;
    // Capture frame-by-frame
    cap >> frame;
    
    // If the frame is empty, break immediately
    if (frame.empty()) break;

    pBackSub->apply(frame, fgMask,-1); //0 is the learning rate, if nothing is present or -1 it takes 1(useful for dynamic density)

    if (count%3==1) file << process(fgMask,count) << endl;
    count++;
    
    // Press  ESC on keyboard to exit
    char c=(char)waitKey(25);
    if(c==27)
      break;
  }

  time_t out = time(NULL);
  cout<<"frames: "<<count<<endl;
  cout<<"time in sec: "<<out-in<<endl; 
  // When everything done, release the video capture object
  cap.release();
  file.close();
  destroyAllWindows();
	
  return 0;
}
