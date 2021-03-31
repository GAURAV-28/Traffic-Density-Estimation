#include "opencv2/opencv.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
// #include <bits/stdc++.h>

using namespace std;
using namespace cv;
using namespace std::chrono;

int NUM_THREADS = 1;

typedef struct thread_data {
   int id;
   string video;
   Mat frame;
   Ptr<BackgroundSubtractor> pBackSub1;
   map<int,double> out; 

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

} thread_data;

void *execute(void *t){

    thread_data *tdata = (thread_data *)t;
    int id = tdata->id;
    Ptr<BackgroundSubtractor> pBackSub1 = tdata->pBackSub1;
    VideoCapture cap(tdata->video);
    //cout << "in thread "<<id<<endl;
    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        cout << "Please run the program using this command format \"./output videofilename\"\n";
    }
    Mat frame = tdata->frame;
    
    Mat gray,crop,fgMask1;
    cvtColor(frame,gray,COLOR_BGR2GRAY);  
    
    crop = tdata->project_crop(gray);           
    
    pBackSub1->apply(crop, fgMask1, 0); 
    //cout<<id<<endl;
    //cout<<tdata->process(fgMask1)<<endl;
    int num = cap.get(CAP_PROP_FRAME_COUNT)/NUM_THREADS + 1;
    //cout<<cap.get(CAP_PROP_FRAME_COUNT)<<endl;
    

    int start = id*num;
    int end = (id+1)*num;
    int count = 0;

    while(1){
        
        cap >> frame;               
        if (frame.empty()){
          cap.release();
          destroyAllWindows();
          pthread_exit(NULL);
        }
        count++;

      if(count % NUM_THREADS == id){
        cvtColor(frame,gray,COLOR_BGR2GRAY);  
        crop = tdata->project_crop(gray);           
        pBackSub1->apply(crop, fgMask1, 0); 
        tdata->out[count] = tdata->process(fgMask1);
      }

    }
    //cout<<count<<endl;
    cap.release();
    destroyAllWindows();
    pthread_exit(NULL);
}

/*void *initialize(void *t){
    thread_data *tdata = (thread_data *)t;
    tdata->pBackSub1 = createBackgroundSubtractorMOG2(500,32,false);
    pthread_exit(NULL);
}*/

//------------------------------------------------------------------------------------------------------------------------------//

// This function is used to calculate number of white pixels in the frame matrix.


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
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return 1;
  }
  string f = argv[1];  //file = the video's name
  string video = f+".mp4";

  if (file_notexist(video)){   // checks if file doesn't exist, prints error message if true
    cout << "Please enter a videofile that exists\n";
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return 1;
  }

  int rc;
  int i;
  NUM_THREADS = stoi(argv[2]);
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  void *status;

  // Initialize and set thread joinable
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  thread_data tdata[NUM_THREADS];

  if (file_notexist("bg.jpg")){   // checks if file doesn't exist, prints error message if true
    cout << "Please put the background file in the directory"<<endl;
    return 1;
  }

   Mat frame = imread("bg.jpg");             // empty background is taken as frame matrix, bg.jpg is extracted from the traffic video
   // if the video is not opened then suitable help is printed on the console
  
/*  for( i = 0; i < NUM_THREADS; i++ ) {  
         
      rc = pthread_create(&threads[i], &attr, initialize, (void *) &tdata[i]);
      if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
      }
  }
*/
  for( i = 0; i < NUM_THREADS; i++ ) {   
    tdata[i].frame = frame;   
    tdata[i].video = video;   
    tdata[i].id = i;
    tdata[i].pBackSub1 = createBackgroundSubtractorMOG2(500,32,false);
    rc = pthread_create(&threads[i], &attr, execute, (void *) &tdata[i]);
    if (rc) {
      cout << "Error:unable to create thread," << rc << endl;
      exit(-1);
    }
  }

  pthread_attr_destroy(&attr);

  ofstream file;   
  string outf = "M4output/num_"+to_string(NUM_THREADS)+".txt";                  
  file.open(outf);
  
  map<int,double> final_out;
  for( i = 0; i < NUM_THREADS; i++ ) {
    rc = pthread_join(threads[i], &status);
    if (rc) {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
    //cout << "Main: completed thread id : " << i << " output : ";
    //cout << tdata[i].out.size() << endl;
    final_out.insert(tdata[i].out.begin(),tdata[i].out.end());
    //for(auto x : tdata[i].out) cout<<x.first<<" "<<x.second<<endl;
  }

  for(auto x : final_out) {
      
      //cout<<x.first<<" "<<x.second<<endl;
      file<<x.first<<" "<<x.second<<endl;
      
  }

  //cout << "Main: program exiting." << endl;

  
  file.close();

  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  cout << time_span.count() << endl;
  return 0;

}

