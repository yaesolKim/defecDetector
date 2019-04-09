#include <windows.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using std::cout;
using std::cerr;
using std::endl;

Mat frame, frame_1;
const char* src_window = "Select ROI";

int drag = 0, select_flag = 0;
bool setmonitor = false;

Point point1, point2, p11, p12, p21, p22, p31, p32, p41, p42;
int err1, err2, err3, err4;
bool err1_st, err2_st, err3_st, err4_st;
int thr = 100;

int n1, n2, n3, n4;
double diff1, diff2, diff3, diff4;


void mouseHandler(int event, int x, int y, int flags, void* param);

int main(int, char**)
{
	err1 = 0;
	err2 = 0;
	err3 = 0;
	err4 = 0;

	VideoCapture capture(0); // open the camera
	if (!capture.isOpened())
	{
		cerr << "ERROR: Can't initialize camera capture" << endl;
		return 1;
	}

	cout << "Frame width: " << capture.get(CAP_PROP_FRAME_WIDTH) << endl;
	cout << "     height: " << capture.get(CAP_PROP_FRAME_HEIGHT) << endl;
	cout << "Capturing FPS: " << capture.get(CAP_PROP_FPS) << endl;

	cout << endl << "Press 'ESC' to quit, 'space' to toggle frame processing" << endl;
	cout << endl << "Start grabbing..." << endl;

	size_t nFrames = 0;
	bool enableProcessing = false;
	int64 t0 = cv::getTickCount();
	int64 processingTime = 0;

	capture >> frame;

	int height = frame.rows;
	int width = frame.cols;


	namedWindow(src_window, WINDOW_AUTOSIZE);
	imshow(src_window, frame);
	setMouseCallback(src_window, mouseHandler, 0);

	for (;;)
	{
		capture >> frame; // read the next frame from camera

		diff1 = 0.0000;
		diff2 = 0.0000;
		diff3 = 0.0000;
		diff4 = 0.0000;

		if (frame.empty())
		{
			cerr << "ERROR: Can't grab camera frame." << endl;
			break;
		}
		nFrames++;
		if (nFrames % 10 == 0)
		{
			const int N = 10;
			int64 t1 = cv::getTickCount();
			t0 = t1;
			processingTime = 0;
		}
		if (!enableProcessing)
		{
			if (!setmonitor) // set locations of four monitors
			{
				if (select_flag == 1 && !drag)
				{
					p11 = point1;	p12 = point2;
					p21 = point1;	p22 = point2;
					p31 = point1;	p32 = point2;
					p41 = point1;	p42 = point2;
					n1 = abs(p11.x - p12.x) * abs(p11.y - p12.y);
				}

				else if (select_flag == 2 && !drag)
				{
					p21 = point1;	p22 = point2;
					n2 = abs(p21.x - p22.x) * abs(p21.y - p22.y);
				}

				else if (select_flag == 3 && !drag)
				{
					p31 = point1;	p32 = point2;
					n3 = abs(p31.x - p32.x) * abs(p31.y - p32.y);
				}

				else if (select_flag == 4 && !drag)
				{
					p41 = point1;	p42 = point2;
					n4 = abs(p41.x - p42.x) * abs(p41.y - p42.y);

					frame_1 = frame.clone();
					setmonitor = true;
				}

				if (frame.empty())
					break;
			}

			else
			{

				for (int y = p11.y; y < p12.y; y++)
				{
					for (int x = p11.x; x < p12.x; x++)
					{
						diff1 += abs(int(frame.at<Vec3b>(y, x)[0]) - int(frame_1.at<Vec3b>(y, x)[0]));
						diff1 += abs(int(frame.at<Vec3b>(y, x)[1]) - int(frame_1.at<Vec3b>(y, x)[1]));
						diff1 += abs(int(frame.at<Vec3b>(y, x)[2]) - int(frame_1.at<Vec3b>(y, x)[2]));
					}
				}
				diff1 = diff1 / n1;


				for (int y = p21.y; y < p22.y; y++)
				{
					for (int x = p21.x; x < p22.x; x++)
					{
						diff2 += abs(int(frame.at<Vec3b>(y, x)[0]) - int(frame_1.at<Vec3b>(y, x)[0]));
						diff2 += abs(int(frame.at<Vec3b>(y, x)[1]) - int(frame_1.at<Vec3b>(y, x)[1]));
						diff2 += abs(int(frame.at<Vec3b>(y, x)[2]) - int(frame_1.at<Vec3b>(y, x)[2]));
					}
				}
				diff2 = diff2 / n2;

				for (int y = p31.y; y < p32.y; y++)
				{
					for (int x = p31.x; x < p32.x; x++)
					{
						diff3 += abs(int(frame.at<Vec3b>(y, x)[0]) - int(frame_1.at<Vec3b>(y, x)[0]));
						diff3 += abs(int(frame.at<Vec3b>(y, x)[1]) - int(frame_1.at<Vec3b>(y, x)[1]));
						diff3 += abs(int(frame.at<Vec3b>(y, x)[2]) - int(frame_1.at<Vec3b>(y, x)[2]));
					}

				}
				diff3 = diff3 / n3;

				for (int y = p41.y; y < p42.y; y++)
				{
					for (int x = p41.x; x < p42.x; x++)
					{
						diff4 += abs(int(frame.at<Vec3b>(y, x)[0]) - int(frame_1.at<Vec3b>(y, x)[0]));
						diff4 += abs(int(frame.at<Vec3b>(y, x)[1]) - int(frame_1.at<Vec3b>(y, x)[1]));
						diff4 += abs(int(frame.at<Vec3b>(y, x)[2]) - int(frame_1.at<Vec3b>(y, x)[2]));
					}
				}
				diff4 = diff4 / n4;
				 
				if (diff1 > thr)
				{
					if (err1_st == false)
					{
						err1++;
						cout << "monitor 1 makes error: " << err1 << " times" << endl;
					}
					err1_st = true;
				}
				else
				{
					err1_st = false;
				}

				if (diff2 > thr)
				{
					if (err2_st == false)
					{
						err2++;
						cout << "monitor 2 makes error: " << err2 << " times" << endl;
					}
					err2_st = true;
				}
				else
				{
					err2_st = false;
				}

				if (diff3 > thr)
				{
					if (err3_st == false)
					{
						err3++;
						cout << "monitor 3 makes error: " << err3 << " times" << endl;
					}
					err3_st = true;
				}
				else
				{
					err3_st = false;
				}

				if (diff4 > thr)
				{
					if (err4_st == false)
					{
						err4++;
						cout << "monitor 4 makes error: " << err4 << " times" << endl;
					}						
					err4_st = true;
				}
				else
				{
					err4_st = false;
				}
			}

			cv::rectangle(frame, p11, p12, CV_RGB(255, 0, 0), 3, 8, 0);
			cv::rectangle(frame, p21, p22, CV_RGB(255, 0, 0), 3, 8, 0);
			cv::rectangle(frame, p31, p32, CV_RGB(255, 0, 0), 3, 8, 0);
			cv::rectangle(frame, p41, p42, CV_RGB(255, 0, 0), 3, 8, 0);

			cv::imshow(src_window, frame);

			cv::waitKey(5);
		}
		else
		{
			int64 tp0 = cv::getTickCount();
			Mat processed;
			cv::Canny(frame, processed, 400, 1000, 5);
			processingTime += cv::getTickCount() - tp0;
			imshow("Frame", processed);
		}
		int key = waitKey(1);
		if (key == 27)//ESC
			break;
		if (key == 32)//SPACE
		{
			enableProcessing = !enableProcessing;
			cout << "Enable frame processing ('space' key): " << enableProcessing << endl;
		}
	}
	std::cout << "Number of captured frames: " << nFrames << endl;

	return nFrames > 0 ? 0 : 1;
}



void mouseHandler(int event, int x, int y, int flags, void* param)
{
	if (event == EVENT_LBUTTONDOWN && !drag && (select_flag != 4))
	{
		/* left button clicked. ROI selection begins */
		point1 = cv::Point(x, y);
		drag = 1;
	}

	if (event == EVENT_MOUSEMOVE && drag && (select_flag != 4))
	{
		/* mouse dragged. ROI being selected */
		cv::Mat img1 = frame.clone();
		point2 = cv::Point(x, y);
		cv::rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 3, 8, 0);
		cv::imshow(src_window, img1);
	}

	if (event == EVENT_LBUTTONUP && drag && (select_flag != 4))
	{
		cv::Mat img2 = frame.clone();
		point2 = cv::Point(x, y);
		drag = 0;
		select_flag += 1;
		cv::imshow(src_window, img2);
	}
}

 