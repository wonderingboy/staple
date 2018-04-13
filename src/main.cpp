#include "staple_tracker.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
bool init=false;
Rect location;
int cols;
int rows;
Rect selectl;
bool mouse_down=false;
Mat image;
void onMouse(int event,int x,int y,int flags,void*)
{
    if(event==CV_EVENT_LBUTTONDOWN&&false==init)
    {
        selectl.x=x;
        selectl.y=y;
        cout<<x<<endl;
        cout<<y<<endl;
		mouse_down=true;
    }
    else if(event==CV_EVENT_LBUTTONUP&&false==init)
    {
        selectl.width=x-selectl.x;//以下2行计算出来的值要么都大于0，要么都小于0
        selectl.height=y-selectl.y;
		if(x<cols&&y<rows&&x>0&&y>0&&selectl.width<cols&&selectl.height<rows&&selectl.width>0&&selectl.height>0) {
			location=selectl;
			init=true;
		}else{
			cout<<"fail to  initialize location"<<endl;
		}
		mouse_down=false;
    } else if(event==CV_EVENT_MOUSEMOVE&&true==mouse_down&&false==init){
		int lx=selectl.x<x?selectl.x:x;
		int rx=selectl.x>x?selectl.x:x;
		int uy=selectl.y<y?selectl.y:y;
		int dy=selectl.y>y?selectl.y:y;
		Mat tempo;
		image.copyTo(tempo);
		cv::rectangle(tempo, Rect(lx,uy,rx-lx,dy-uy), cv::Scalar(0, 0, 255), 2);
		imshow("STAPLE",tempo);
		waitKey(1);		
	} 
}
int main(int argc, char * argv[])
{
    if(argc!=2){
		cout<<"please load a video"<<endl;
		return -1;
	 }
	cout<<argv[1]<<endl; 
	STAPLE_TRACKER staple;
    bool show_visualization = true;
	namedWindow("STAPLE");
	setMouseCallback("STAPLE",onMouse,0);
	VideoCapture video;
	video.open(argv[1]);
	if (!video.isOpened()){
		cout<<"fail to load video"<<endl;
		return -1;
	}
	long num_frames=video.get(CV_CAP_PROP_FRAME_COUNT);
	for(int i=0;i<num_frames;i++) {
		video.read(image);
		rows=image.rows;
		cols=image.cols;
		if(false==init){
			cout<<"please choose rect to initialize"<<endl;
			cv::imshow("STAPLE", image);
			while(false==init) { 
	            cv::waitKey(1);
            }
            cv::rectangle(image, location, cv::Scalar(0, 0, 255), 2);
			cv::imshow("STAPLE", image);
			staple.tracker_staple_initialize(image, location);
            staple.tracker_staple_train(image, true);
		} else {
			location = staple.tracker_staple_update(image);
            staple.tracker_staple_train(image, false);		
		}
        if (show_visualization) {
            cv::rectangle(image, location, cv::Scalar(0, 128, 255), 2);
            cv::imshow("STAPLE", image);
            char key = cv::waitKey(10);
            if (key == 27 || key == 'q' || key == 'Q')
                return 0;
			if (key=='r'||key=='R') {
				init=false;
				mouse_down=false;
			}
        }
	}
    return 0;
}


