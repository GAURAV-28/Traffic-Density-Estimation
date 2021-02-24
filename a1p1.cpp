#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct params{          // defined a datatype params for easier handling in the rest of the code
    Mat im;             // im stores the actual image data
    vector<Point2f> p;  // p stores the 4 points used for homography calculations
};

inline bool file_notexist (const std::string& name) {  // a function used to check a file with a given name exists in the working directory or not
    if (FILE *file = fopen(name.c_str(), "r")) {        // returns true if file doesn't exist
        fclose(file);                                   // returns false if a file with the given name exists
        return false;
    } else {
        return true;
    }   
}

void mouseHandle(int event, int x, int y,int flags, void* data_ptr){    // function used to handle mouse input
    if(event==EVENT_LBUTTONDOWN){                                       // only accepts left click of mouse as a trigger to perform the operation
        params *data = ((params *) data_ptr);                           // data_ptr is a pointer of a params structure which is holding the data of the image that is being operated on
        circle(data->im, Point(x,y),2,Scalar(0,0,0), 5, cv::LINE_AA);   // x,y is the coordinate where the mouse was clicked(passed by the setmousecallback function). A small dot is drawn at this positon using circle(...)
        imshow("Original-Image", data->im);                             // the image being displayed is updated with the new drawn dot.

        if(data->p.size()<3) data->p.push_back(Point2f(x,y));           // if no. of input by the user earlier < 3, add the new coordinate to data.p(vector),
        else{                                                           // else, push the coordinate into p and close the window(stopping mouse input)
            data->p.push_back(Point2f(x,y));
            destroyWindow("Original-Image");
        }

    }
}

int main(int argc, char const *argv[]){         //filename parameter is passed through while running the program

    if (argc!=2){       // unexpected no. of parameters
        cout << "Please run the program using this command format \"./output filename\"\n";
        return 1;
    }
    string f = argv[1];  //file = the image's name
    string file = f+".jpg";
    if (file_notexist(file)){   // checks if file doesn't exist, prints error message if true
        cout << "Please enter a file that exists\n";
        cout << "Please run the program using this command format \"./output filename\"\n";
        return 1;
    }

    Mat im_src;
    Mat im_pro;
    Mat im_dst;

  
    im_src = imread(file, IMREAD_GRAYSCALE);    // image is read in grayscale using opencv function
    Size pro_size(1280,875);                    // output size of the projected and cropped image is harcoded
    Size dst_size(400,1000);                    

    vector<Point2f> pts_pro;
    vector<Point2f> pts_dst;

    pts_pro.push_back(Point2f(472,52));         // hardcodes the homography hpoints of the perspective image using riju ma'am's points
    pts_pro.push_back(Point2f(800,52));
    pts_pro.push_back(Point2f(800,830));
    pts_pro.push_back(Point2f(472,830));

    pts_dst.push_back(Point2f(0,0));            // homography points of the cropped image is the corner points of the image
    pts_dst.push_back(Point2f(dst_size.width-1, 0));
    pts_dst.push_back(Point2f(dst_size.width-1, dst_size.height-1));
    pts_dst.push_back(Point2f(0, dst_size.height-1));

    Mat im_tmp = im_src.clone();        //copy of orginal image to draw dot while taking input
    params data;
    data.im = im_tmp;

    try{imshow("Original-Image", im_tmp);}
    catch(...){cout << "Please provide a valid image file\n"; return 1;}    // error message if the file isn't a valid image file
    setMouseCallback("Original-Image", mouseHandle, &data);                 // opencv function to handle mouse input to which we pass the earlier defined function mousehandle
    waitKey(0);                                                             // waits until a keyboard key is pressed before proceeding to the next line. Needed to take input using mouse

    Mat h1 = findHomography(data.p, pts_pro);                           // calculating homography using opencv function(original image, points chosen of that image)
    Mat h2 = findHomography(data.p, pts_dst);

    warpPerspective(im_src, im_pro, h1, pro_size);                  // outputs the im_src after applying homography using h1 and pro_size to im_pro
    warpPerspective(im_src, im_dst, h2, dst_size);                  

    imshow("Projected-Image", im_pro);                              // displays im_pro
    imwrite(f+"_projected.jpg", im_pro);                               // writes im_pro to a file
    waitKey(0);                                                     // waits until key is pressed

    destroyWindow("Projected-Image");                               // window is closed

    imshow("Cropped-Image", im_dst);                                // displays cropped image im_dst
    imwrite(f+"_cropped.jpg", im_dst);                                 // writes the same image to a file
    waitKey(0);                                                     // displays the file until key is pressed

    return 0;
}


