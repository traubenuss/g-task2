#include <iostream>
#include <opencv2/opencv.hpp>

#include "Utils.h"


#define KEY_ESC  27


using namespace std;
using namespace cv;


void exec(const Mat& input, Mat& output)
{
    // Put your ideas here:
    output = input;
}



int main(int argc, char* argv[])
{
    cout << "XXXXX task 2" << endl;

    CommandParams params;
    parseCmd(argc, argv, params);

    switch(params.exec_mode_)
    {
    case CommandParams::STATIC:
    {
        vector<string> filelist;
        if(!getFileList(params.str_datapath_, filelist))
            exit(-1);

        for(vector<string>::const_iterator file_it = filelist.begin(); file_it != filelist.end(); ++file_it)
        {
            cout << " File: " << *file_it << endl;
            string img_path = params.str_datapath_ + FOLDER_CHAR + *file_it;
            Mat input = imread(img_path);
            Mat output;
            exec(input, output);
            PAUSE_AND_SHOW(output);
        }
        break;
    }

    case CommandParams::VIDEO:
    {
        //int vid_w = 0, vid_h = 0;
        int vid_frame_rate = 0;
        VideoCapture video_capture(params.str_datapath_);
        if(!video_capture.isOpened())
        {
            cerr << "ERROR: could not load video!" << endl;
            exit(-1);
        }
        else
        {
            //vid_w = video_capture.get(CV_CAP_PROP_FRAME_WIDTH);
            //vid_h = video_capture.get(CV_CAP_PROP_FRAME_HEIGHT);
            vid_frame_rate = video_capture.get(CV_CAP_PROP_FPS);
            cout << "rate:" << vid_frame_rate << endl;
        }

        while(waitKey(vid_frame_rate) != KEY_ESC)
        {
            Mat input, output;
            video_capture.read(input);
            exec(input, output);
            imshow("output", output);
        }
        break;
    }

    case CommandParams::LIVE:
    {
        break;
    }
    }

    return 0;
}
