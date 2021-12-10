#include "opencv2/opencv.hpp"
#include <iostream>
#include <bits/stdc++.h>

using namespace std;
using namespace cv;

#define NUM_THREADS 4

typedef struct thread_data {
   int id;
   Mat frame;
   int count;
   Ptr<BackgroundSubtractor> pBackSub1;
   map<int,double> out; 
   vector<Mat> frame_seq;

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
    vector<Mat> frame_seq = tdata->frame_seq;
    Mat frame = tdata->frame;

    Mat gray,crop,fgMask1;
    cvtColor(frame,gray,COLOR_BGR2GRAY);  
    crop = tdata->project_crop(gray);           

    pBackSub1->apply(crop, fgMask1, 0); 
    //cout<< "in" << id << endl;

    int c = 0;
    for( c=0 ; c<frame_seq.size() ; c ++ ){
      if(c % NUM_THREADS == id){
      cvtColor(frame_seq[c],gray,COLOR_BGR2GRAY);  
      crop = tdata->project_crop(gray);           

      pBackSub1->apply(crop, fgMask1, 0); 
      tdata->out[c] = tdata->process(fgMask1);
      //cout << tdata->out[c] << endl;
      }
    }
    //cout<<c<<endl;
    pthread_exit(NULL);
}

void *initialize(void *t){
    thread_data *tdata = (thread_data *)t;
    tdata->pBackSub1 = createBackgroundSubtractorMOG2(500,32,false);
    tdata->count = 0;

    pthread_exit(NULL);
}

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

  time_t start = time(NULL);

  if (argc!=2){       // unexpected no. of parameters
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

  VideoCapture cap(video);                     //capture the traffic video
  
  int rc;
  int i;
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  void *status;

  // Initialize and set thread joinable
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  thread_data tdata[NUM_THREADS];

  Mat frame;
  vector<Mat> frame_seq;

  if (file_notexist("bg.jpg")){   // checks if file doesn't exist, prints error message if true
    cout << "Please put the background file in the directory"<<endl;
    return 1;
  }

  frame = imread("bg.jpg");             // empty background is taken as frame matrix, bg.jpg is extracted from the traffic video
  
  for( i = 0; i < NUM_THREADS; i++ ) {  
      tdata[i].frame = frame;        
      rc = pthread_create(&threads[i], &attr, initialize, (void *) &tdata[i]);
      if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
      }
  }

  /*for( i = 0; i < NUM_THREADS; i++ ) {  
            
      rc = pthread_create(&threads[i], &attr, execute, (void *) &tdata[i]);
      if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
      }
  }*/

  ofstream file;                        // for convience and making graphs easily, the output is saved in a csv file (output.csv)
  file.open("r.txt");
  //file << "output" << endl;

  // if the video is not opened then suitable help is printed on the console
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return -1;
  }

  int count = 0; //initialize count for frames

  Mat fr;
  cout << "video_cap" << endl;
  while(1){

    cap >> fr;                 // frame is taken from the video, count is increased
    if (fr.empty()) break; count++;
    
    /*int k = count%NUM_THREADS;

    tdata[k].frame = frame;
    tdata[k].count = count;
    rc = pthread_create(&threads[k], &attr, execute, (void *) &tdata[k]);

    if (rc) {
      cout << "Error:unable to create thread," << rc << endl;
      exit(-1);
    }*/
    Mat in = fr.clone();
    frame_seq.push_back(in);
    imshow("Input", fr);

    cap >> fr;             // this frame is skipped for computation
    if (fr.empty()) break; count++;
    cap >> fr;             // this frame is skipped for computation 
    if (fr.empty()) break; count++;

    char c=(char) waitKey(20); 
    if(c==27) break;
  }
  // all windows are closed.
  cap.release();
  destroyAllWindows();

 
  
  /*cout<<frame_seq.size()<<endl;
  for(auto x : frame_seq){
    imshow("Output", x);
    char c=(char)waitKey(10); 
    if(c==27) break;
  }*/
  cout << "thread_create" << endl;

  for( i = 0; i < NUM_THREADS; i++ ) {  
    
    tdata[i].frame_seq = frame_seq;    
    tdata[i].id = i;
    tdata[i].frame = frame;
    rc = pthread_create(&threads[i], &attr, execute, (void *) &tdata[i]);
    if (rc) {
      cout << "Error:unable to create thread," << rc << endl;
      exit(-1);
    }
  }

  pthread_attr_destroy(&attr);
 
  map<int,double> final_out;
  for( i = 0; i < NUM_THREADS; i++ ) {
    rc = pthread_join(threads[i], &status);
    if (rc) {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
    //cout << "Main: completed thread id : " << i << " output : ";
    
    final_out.insert(tdata[i].out.begin(),tdata[i].out.end());
    //for(auto x : tdata[i].out) cout<<x.first<<" "<<x.second<<endl;
  }

  for(auto x : final_out) {
      
      cout<<x.first<<" "<<x.second<<endl;
      file<<x.first<<" "<<x.second<<endl;
      
  }

  cout << "Main: program exiting." << endl;

  
  file.close();

  time_t end = time(NULL);
  cout<< end-start << endl;
  return 0;
  //pthread_exit(NULL);

}


