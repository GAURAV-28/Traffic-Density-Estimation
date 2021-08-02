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

} thread_data;

void *execute(void *t){

  thread_data *tdata = (thread_data *)t;
  int id = tdata->id;

  Ptr<BackgroundSubtractor> pBackSub1 = tdata->pBackSub1;
  Mat frame = tdata->frame;

  Mat fgMask1, gray, crop;
  cvtColor(frame,gray,COLOR_BGR2GRAY);  
  crop = tdata->project_crop(gray);           

  pBackSub1->apply(crop, fgMask1, 0); 
    
  tdata->out = tdata->process(fgMask1);   
  pthread_exit(NULL);
}

//------------------------------------------------------------------------------------------------------------------------------//


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

  high_resolution_clock::time_point t1 = high_resolution_clock::now();  // for running time estimation

  if (argc!=3){         // unexpected no. of parameters
    cout << "Please run the program using this command format \"./output videofilename NUM_THREADS\"\n";
    return 1;
  }
  string f = argv[1];  //file = the video's name
  string video = f+".mp4";

  int num_thread = stoi(argv[2]);
  NUM_THREADS = num_thread;

  if (file_notexist(video)){   // checks if file doesn't exist, prints error message if true
    cout << "Please enter a videofile that exists\n";
    cout << "Please run the program using this command format \"./output videofilename NUM_THREADS\"\n";
    return 1;
  }

  VideoCapture cap(video);                     //capture the traffic video
  
  // Parameter Initialization for pthreads
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


  if (file_notexist("bg.jpg")){   // checks if file doesn't exist, prints error message if true
    cout << "Please put the background file in the directory"<<endl;
    return 1;
  }

  frame = imread("bg.jpg");             // empty background is taken as frame matrix, bg.jpg is extracted from the traffic video
  
  // background subtraction function is initialized using the background frame for all threads
  for( i = 0; i < NUM_THREADS; i++ ) {  
      tdata[i].frame = frame;     
      tdata[i].pBackSub1 = createBackgroundSubtractorMOG2(500,32,false);     
      tdata[i].id = i;   
      rc = pthread_create(&threads[i], &attr, execute, (void *) &tdata[i]);
      if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
      }
  }
  // all threads are joined
  for( i = 0; i < NUM_THREADS; i++ ) {
    rc = pthread_join(threads[i], &status);
    if (rc) {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
    }

  ofstream file;                        // the output is saved in a text file used for further analysis
  string outfile = "M4output/m4_num_"+to_string(NUM_THREADS)+".txt";
  file.open(outfile);

  // if the video is not opened then suitable help is printed on the console
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    cout << "Please run the program using this command format \"./output videofilename\"\n";
    return -1;
  } 

  int count = 0; //initialize count for frames

  Mat fr,gray,crop,fgMask1; // for taking the frame from the video
  map<int,double> final_out;
  bool done = false;
  int j = NUM_THREADS;
  // This while is used to capture the frames from the video and passed to different threads for processing

  if(NUM_THREADS == 1){
  while(1){   
      cap >> fr;                     // frame is taken from the video, count is increased
      if (fr.empty()) break; 
      count++;

      cvtColor(fr,gray,COLOR_BGR2GRAY);           // converted to gray scale
      crop = tdata[0].project_crop(gray);            // apply projection and cropping this gray frame matrix (using subtask1 code)

      tdata[0].pBackSub1->apply(crop, fgMask1, 0);   // apply background substraction on this frame to get fgMask1 keeping third param as 0 to fix the background
      final_out[count] = tdata[0].process(fgMask1);  // calculated density is stored in final_out map
    }
  }
  else{
  while(1){

    for( i = 0; i < NUM_THREADS; i++ ) {    
        cap >> fr;                     // frame is taken from the video, count is increased
        if (fr.empty()){
            j = i;
            done = true;
            break; 
        } 
        count++;
        tdata[i].frame = fr;          // this frame is passed to thread i for processing
        rc = pthread_create(&threads[i], &attr, execute, (void *) &tdata[i]);
        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }
    

    for( i = 0; i < j; i++ ) {        // threads are joined
        rc = pthread_join(threads[i], &status);
        if (rc) {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }
        final_out[count-(j-1)+i] = tdata[i].out; // calculated density is stored in final_out map
    }
    if (done) break;
  }
  }

  // all windows are closed.
  cap.release();
  destroyAllWindows();
  // thread attributes are destroyed
  pthread_attr_destroy(&attr);

  for(auto x : final_out) {
    file<<x.first<<" "<<x.second<<endl;   // data is stored in the file
  }

  file.close();

  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  cout << time_span.count() << endl;    // run time is printed
  return 0;
}

