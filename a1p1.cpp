#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct params{
    Mat im;
    vector<Point2f> p;
};

void mouseHandle(int event, int x, int y,int flags, void* data_ptr){
    if(event==EVENT_LBUTTONDOWN){
        params *data = ((params *) data_ptr);
        circle(data->im, Point(x,y),2,Scalar(0,0,0), 5, cv::LINE_AA);
        imshow("Original-Image", data->im);    

        if(data->p.size()<3) data->p.push_back(Point2f(x,y));
        else{
            data->p.push_back(Point2f(x,y));
            destroyWindow("Original-Image");
        }

    }
}

int main(int argc, char const *argv[]){

    Mat im_src = imread("traffic.jpg", IMREAD_GRAYSCALE);
    Mat im_pro;
    Mat im_dst;

    Size pro_size(1280,875);
    Size dst_size(400,1000);

    im_dst = Mat::zeros(dst_size,CV_8UC3);

    vector<Point2f> pts_pro;
    vector<Point2f> pts_dst;

    pts_pro.push_back(Point2f(472,52));
    pts_pro.push_back(Point2f(800,52));
    pts_pro.push_back(Point2f(800,830));
    pts_pro.push_back(Point2f(472,830));

    pts_dst.push_back(Point2f(0,0));
    pts_dst.push_back(Point2f(dst_size.width-1, 0));
    pts_dst.push_back(Point2f(dst_size.width-1, dst_size.height-1));
    pts_dst.push_back(Point2f(0, dst_size.height-1));

    Mat im_tmp = im_src.clone();
    params data;
    data.im = im_tmp;

    imshow("Original-Image", im_tmp);
    setMouseCallback("Original-Image", mouseHandle, &data);
    waitKey(0);

    Mat h1 = findHomography(data.p, pts_pro);
    Mat h2 = findHomography(data.p, pts_dst);

    warpPerspective(im_src, im_pro, h1, pro_size);
    warpPerspective(im_src, im_dst, h2, dst_size);

    imshow("Projected-Image", im_pro);
    imwrite("projected.jpg", im_pro);
    waitKey(0);

    destroyWindow("Projected-Image");

    imshow("Cropped-Image", im_dst);
    imwrite("cropped.jpg", im_dst);
    waitKey(0);

    return 0;
}


