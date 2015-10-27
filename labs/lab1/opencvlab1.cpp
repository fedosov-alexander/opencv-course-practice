#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;
void filter(Mat imgChannel, Mat integralChannel, Mat distanceMap);
unsigned char getFilteredPixelValue(Mat imgChannel, Mat integralChannel, int row, int col, int neighbourhoodSize);
int getNeighbourhoodSize(int distance);
int main( int argc, char** argv )
{
    if( argc != 2)
    {
        cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }

    Mat image;
    Mat edged;
    Mat binary;
    Mat distanceMap;
    Mat integralImg;
    Mat blurred;
image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

if(! image.data )                              // Check for invalid input
{
    cout <<  "Could not open or find the image" << std::endl ;
    return -1;
}


imshow( "Original image", image );                   // Show our image inside it.
cvtColor(image, edged, CV_BGR2GRAY);
Canny(edged,edged,100,250);
threshold(edged,binary,100,255,THRESH_BINARY_INV);
distanceTransform(binary,distanceMap,CV_DIST_L2,CV_DIST_MASK_PRECISE);
vector<Mat> imgChannels(3);
vector<Mat> integralChannels(3);
split(image,imgChannels);
for(int i =0; i < 3; i++){
    integral(imgChannels[i],integralChannels[i],CV_32S);
    filter(imgChannels[i],integralChannels[i],distanceMap);
}
merge(imgChannels,image);
imshow( "Filtered image",image);  
waitKey(0);                                          // Wait for a keystroke in the window
return 0;
}

void filter(Mat imgChannel, Mat integralChannel, Mat distanceMap){
    int k;
    Mat imgCopy = imgChannel.clone();
    for (int i = 0; i < imgChannel.rows; i++)
    {
        for (int j = 0; j < imgChannel.cols; j++)
        {
            k  = getNeighbourhoodSize((int)distanceMap.at<float>(i,j));
            imgChannel.at<unsigned char>(i,j) = getFilteredPixelValue(imgCopy,integralChannel,i,j,k);
        }
    }

}

int getNeighbourhoodSize(int distance){
    if(distance == 0){
        return 0;
    }
    if(distance == 1){
        return 1;
    }
    if (distance<= 3)
    {
        return 3;
    }
    if (distance<= 5)
    {
        return 5;
    }
    if (distance<= 7)
    {
        return 7;
    }
    else
    {
        return 9;
    }
}

unsigned char getFilteredPixelValue(Mat imgChannel, Mat integralChannel, int row, int col, int neighbourhoodSize){
    Point2i leftTop = Point2i(0,0);
    Point2i rightTop = Point2i(0,0); 
    Point2i leftBottom = Point2i(0,0);
    Point2i rightBottom = Point2i(0,0);
    int k = 1;
    int pixelValue = 0;
    if(neighbourhoodSize > 1){
        for (int i = 0; i <= (neighbourhoodSize/2); i++){
            if(row - i > 0) {
                for (int j = 0; j < (neighbourhoodSize/2); j++){
                    
                    if(col - j > 0){
                        leftTop.x = col-j;
                        leftTop.y = row-i;
                    } 
                    if(col + j < imgChannel.cols){
                        rightTop.x = col + j;
                        rightTop.y = row - i;
                    } 
                }
            }

            if(row + i < imgChannel.rows){
                for (int j = 0; j < (neighbourhoodSize/2); j++){
                    
                    if(col - j > 0) {
                        leftBottom.x = col-j;
                        leftBottom.y = row+i;
                    } 

                    if(col + j < imgChannel.cols){
                        rightBottom.x = col + j;
                        rightBottom.y = row + i;
                    } 
                }

            }

        }
        for(int i =leftTop.y;i<=leftBottom.y;i++){
            for (int j = leftTop.x; j <= rightTop.x; j++)
            {
                pixelValue+=imgChannel.at<unsigned char>(i,j);
            }
        }
        
        k = (rightBottom.x-leftBottom.x+1)*(leftBottom.y-leftTop.y+1);
        pixelValue = integralChannel.at<int>(leftTop.y,leftTop.x)+
        integralChannel.at<int>(rightBottom.y+1,rightBottom.x+1)-
        integralChannel.at<int>(rightTop.y,rightTop.x+1)-
        integralChannel.at<int>(leftBottom.y+1,leftBottom.x);
        return ((unsigned char)((pixelValue)/k));
    } 
    else {
        return imgChannel.at<unsigned char>(row,col);
    }
}