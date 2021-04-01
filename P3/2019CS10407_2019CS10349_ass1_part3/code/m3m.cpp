#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace cv;
using namespace std::chrono;


int NUM_THREADS = 1;

typedef struct thread_data {
   int id;
   Mat frame;
   int count;
   Ptr<BackgroundSubtractor> pBackSub1;
   double out; 
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
    // vector<Mat> frame_seq = tdata->frame_seq;
    Mat frame = tdata->frame;

    Mat fgMask1;
    pBackSub1->apply(frame, fgMask1, 0); 
    tdata->out = tdata->process(fgMask1);
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

std::vector<Mat> split (Mat frame, void *t){
    thread_data *tdata = (thread_data *)t;
    Mat gray,crop,fgMask1;
    cvtColor(frame,gray,COLOR_BGR2GRAY);  
    
    crop = tdata->project_crop(gray); 
    int cell_Height = crop.rows / NUM_THREADS;
    int cell_Width = crop.cols;

    std::vector<cv::Mat> imageSections;
    int x = 0;
    int y = 0;

    for(y=0; y+cell_Height<=crop.rows; y+=cell_Height)
    {   
        // cout << y << endl;
        imageSections.push_back(crop(cv::Rect(x, y, cell_Width, cell_Height)));
    }

    return imageSections;
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
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return 1;
  }
  string f = argv[1];  //file = the video's name
  string video = f+".mp4";

  int num_thread = stoi(argv[2]);
  NUM_THREADS = num_thread;

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
  Mat gray,crop,fgMask1;

  if (file_notexist("bg.jpg")){   // checks if file doesn't exist, prints error message if true
    cout << "Please put the background file in the directory"<<endl;
    return 1;
  }

  frame = imread("bg.jpg");             // empty background is taken as frame matrix, bg.jpg is extracted from the traffic video
  
 
  std::vector<cv::Mat> imageSections = split(frame, (void *) &tdata[0]);
    
  if(NUM_THREADS == 1){
    cvtColor(frame,gray,COLOR_BGR2GRAY);  
    crop = tdata[0].project_crop(gray); 
    tdata[0].frame = crop;  
    tdata[0].pBackSub1 = createBackgroundSubtractorMOG2(500,32,false);     
    tdata[0].id = 0;
    rc = pthread_create(&threads[0], &attr, execute, (void *) &tdata[0]);
    if (rc) {
      cout << "Error:unable to create thread," << rc << endl;
      exit(-1);
    }
    rc = pthread_join(threads[0], &status);
    if (rc) {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
  }

  else{
  for( i = 0; i < NUM_THREADS; i++ ) {  
      tdata[i].frame = imageSections[i];     
      tdata[i].pBackSub1 = createBackgroundSubtractorMOG2(500,32,false);     
      tdata[i].id = i;   
      rc = pthread_create(&threads[i], &attr, execute, (void *) &tdata[i]);
      if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
      }
  }
  }

  ofstream file;                        // for convience and making graphs easily, the output is saved in a csv file (output.csv)
  string outfile = "M3output/m3_num_"+to_string(NUM_THREADS)+".txt";
  file.open(outfile);

  // if the video is not opened then suitable help is printed on the console
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return -1;
  }

  int count = 0; //initialize count for frames

  Mat fr;
  vector<Mat> split1;
  map<int,double> final_out;
  while(1){

    if(NUM_THREADS == 1){
      cap >> fr;                 // frame is taken from the video, count is increased
      if (fr.empty()) break; 
      count++;
      
      cvtColor(fr,gray,COLOR_BGR2GRAY);  
      crop = tdata[0].project_crop(gray); 
      tdata[0].pBackSub1->apply(crop,fgMask1,0);
      final_out[count] = tdata[0].process(fgMask1);

    }
    else{

    cap >> fr;                 // frame is taken from the video, count is increased
    if (fr.empty()) break; 
    count++;
    split1 = split(fr, (void *) &tdata[0]);

    for( i = 0; i < NUM_THREADS; i++ ) {    
    tdata[i].id = i;
    tdata[i].frame = split1[i];
    rc = pthread_create(&threads[i], &attr, execute, (void *) &tdata[i]);
    if (rc) {
      cout << "Error:unable to create thread," << rc << endl;
      exit(-1);
    }
  }

  final_out[count] = 0;
  for( i = 0; i < NUM_THREADS; i++ ) {
    rc = pthread_join(threads[i], &status);
    if (rc) {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
    final_out[count] = final_out[count] + tdata[i].out;
    }
  }
  }
  // all windows are closed.
  cap.release();
  destroyAllWindows();

  pthread_attr_destroy(&attr);

  for(auto x : final_out) {
    file<<x.first<<" "<<x.second<<endl;
  }

  file.close();

  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  cout << time_span.count() << endl;
  return 0;

}


