# Traffic Density Estimation

## About:
After the process of camera angle correction and cropping, queue and dynamic traffic density is calculated using the ```background subtraction``` function of OpenCV. The background has been extracted and present as ```bg.jpg``` in the repository.

The repository contains one C++ code file and one make file. The output generated when we ran is stored as ```out.txt``` and the chart based on this data as ```td.png```.


## How to Run the Code:
1. Put the code file and Makefile along with the background image in the same folder.
2. Run 
    ```
    make
    ```
3. Run 
    ```
    ./output video
    ``` 
    where video = ```trafficvideo```. Click [here](https://drive.google.com/file/d/13iUkmyZhoXvO_P2lAk-7QpZUmcrRF2tS/view?usp=sharing) to download the video.

4. Three values will be printed on the console- 
    ```
    Time(in seconds) Queue Density Dynamic Density
    ```
5. For user's convienence, the above printed values is stored in a csv file named as- ```output.csv```.

## References-
1. https://docs.opencv.org/master/d9/dab/tutorial_homography.html
2. https://learnopencv.com/homography-examples-using-opencv-python-c/




