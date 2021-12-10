# Camera Angle Correction and Frame Cropping

## About:
OpenCV algorithms work best on rectangular frames. But when a camera is deployed on a real road, it is very hard to position the camera so that the road to be monitored is a perfect rectangle, and the camera gets a top view of that rectangle. Typically the road to be monitored is at an arbitrary angle to the camera, and many additional objects are part of the frame. For useful tasks like traffic density estimation on a particular road stretch, it is therefore important to first correct the camera angle in software, and crop the image to remove the additional objects.

This repository contains the code for camera angle correction and frame cropping. There are two sample images present to test the code - ```traffic.jpg``` and ```empty.jpg```.

## How to Run the Code:
1. Put the code file and ```Makefile``` along with the sample images in the same folder.
2. Run 
    ```
    make
    ```
3. Run 
    ```
    ./output image
    ``` 
    where image = ```empty``` or ```traffic``` or any other suitable image. 
4. Select four images on the displayed grayscale image in ```clockwise direction``` i.e. select
    ``` 
    TOP-LEFT -> TOP-RIGHT -> BOTTOM-RIGHT -> BOTTOM-LEFT
    ``` 
    The selected point will be shown by a small ```black``` dot.
5. The projected image will be displayed. To see the cropped image ```press any key``` on keyboard.
6. To exit the program, ```press any key```. The corresponding images will be stored in the same folder.

### Brief Description of the code:
The code takes the source image as input and the user is asked to select four points on the shown image. After selecting four points, the projected image is displayed by performing homography using the user's selected set of points and a seperate set of points already present in the code. Another homography is performed between user's selected points and the four corner of the final cropped image. Corresponding windows are shown for the processed images.
## References-
1. https://docs.opencv.org/master/d9/dab/tutorial_homography.html
2. https://learnopencv.com/homography-examples-using-opencv-python-c/




