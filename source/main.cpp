#include <iostream>
#include <opencv2/opencv.hpp>

#include "Utils.h"

#include "GradientOrientations.h"

#define KEY_ESC  27


using namespace std;
using namespace cv;


void getGradients(const Mat& input, Mat& mag, Mat& phase)
{
    Mat d_x, d_y;

    // Blur
    //GaussianBlur(input, temp, Size(3,3), 0, 0, BORDER_DEFAULT);
    // Gradient X
    Sobel(input, d_x, CV_32F, 1, 0, 3);
    // Gradient Y
    Sobel(input, d_y, CV_32F, 0, 1, 3);

    // superposition of the absolute of both directions
    magnitude(d_x, d_y, mag);

    // Step 2: for each pixel, find the largest gradient of the color channels
    phase = Mat::zeros(input.size(), CV_32FC2);
    for(int row_cnt = 0; row_cnt < phase.rows; ++row_cnt)
    {
        for(int col_cnt = 0; col_cnt < phase.cols; ++col_cnt)
        {
            // for each point in the image
            Point2i pt(col_cnt, row_cnt);

            // for magnitude > mag_threshold
            float mag_val = mag.at<float>(pt);
            if(mag_val > 0.0)
            {
                float unit_vec_x = d_x.at<float>(pt)/mag_val;
                float unit_vec_y = d_y.at<float>(pt)/mag_val;
                phase.at<Vec2f>(pt) = Vec2f(unit_vec_x, unit_vec_y); // only consider the orientation (0..pi)
                //cout << sqrt(unit_vec_x*unit_vec_x + unit_vec_y*unit_vec_y);
            }
        }
    }
}

// temporal filter
Mat temp_mat;
bool first_img = true;
void temp_filter(const Mat& input, Mat& output)
{
    if(first_img)
    {
        temp_mat = Mat::zeros(input.size(), CV_8UC1);
        first_img = false;
    }
    Mat input_gray;
    cvtColor(input, input_gray, CV_RGB2GRAY);
    const float mixture = 0.8;
    output = (input_gray*(1-mixture)) + temp_mat*mixture;
    temp_mat = output;

//    Mat output_float;
//    output.convertTo(output_float, CV_32FC1, 1/255.0);
//    Mat output_dx, output_dy;
//    Sobel(output_float, output_dx, CV_32F, 1, 0, 3);
//    Sobel(output_float, output_dy, CV_32F, 0, 1, 3);
//    output = output_dx*0.5+output_dy*0.5;
}

void exec(const Mat& input, Mat& output)
{
    // Put your ideas here:
    Mat input_gray;
    //PAUSE_AND_SHOW(input)
    //cvtColor(input, input_gray, COLOR_RGB2GRAY);

    temp_filter(input, input_gray);

    Mat input_gray_float;
    input_gray.convertTo(input_gray_float, CV_32FC1, 1/255.0);

    // find overall smooth gradient
    Mat strong_blur;
    GaussianBlur(input_gray_float, strong_blur, Size(51,51), 18.0);
//    Mat strong_dx, strong_dy;
//    Sobel(strong_blur, strong_dx, CV_32F, 1, 0, 3);
//    Sobel(strong_blur, strong_dy, CV_32F, 0, 1, 3);
    //PAUSE_AND_SHOW(strong_blur);
    //Mat strong_mag, strong_phase;
    //getGradients(strong_blur, strong_mag, strong_phase);

    // extract more local gradient
    Mat local_blur;
    //medianBlur(input_gray_float, local_blur, 5);
    GaussianBlur(input_gray_float, local_blur, Size(5,5), 20.0); // mean filter
    //medianBlur(local_blur, local_blur, 5);
    //local_blur = input_gray_float;
//    Mat local_dx, local_dy;
//    Sobel(local_blur, local_dx, CV_32F, 1, 0, 3);
//    Sobel(local_blur, local_dy, CV_32F, 0, 1, 3);
    //PAUSE_AND_SHOW(local_blur);

    Mat color_out;
    Mat diff = strong_blur - local_blur;
    cv::normalize(diff, diff, 1, 0, NORM_MINMAX);
//    double min,max;
//    minMaxLoc(diff, &min, &max);
//    cout << "min:" << min << " max:" << max << endl;


    //GaussianBlur(diff, diff, Size(9,9), 50);
    //threshold(diff, diff, 0.001, 1.0, THRESH_BINARY);
    const float contrast_zoom = 3;
    diff = (diff*contrast_zoom)-(contrast_zoom-1)/2.0;
//    Mat diff_8C;
//    diff.convertTo(diff_8C, CV_8UC1, 255.0);
//    medianBlur(diff_8C, diff_8C, 9);

    //imwrite("diff.bmp", diff*255);
//    GaussianBlur(diff, diff, Size(21,21), 75);
//    PAUSE_AND_SHOW(diff);

//    Mat diff_dx, diff_dy;
//    Sobel(diff, diff_dx, CV_32F, 1, 0, 3);
//    Sobel(diff, diff_dy, CV_32F, 0, 1, 3);
//    diff = diff_dx*0.2+diff_dy*0.2;

    output = diff;

//    cv::normalize(strong_blur, strong_blur, 1, 0, NORM_MINMAX);
//    cv::normalize(local_blur, local_blur, 1, 0, NORM_MINMAX);
//    vector<Mat> bgr;
//    bgr.push_back(diff);
//    bgr.push_back(strong_blur);
//    bgr.push_back(local_blur);
//    merge(bgr, output);

    //PAUSE_AND_SHOW(blur_diff);
    //Mat local_mag, local_phase;
    //getGradients(local_blur, local_mag, local_phase);

    // investigate gradients for direction change
//    output = Mat::zeros(input.size(), CV_8UC1);
//    for(int row_cnt = 0; row_cnt < output.rows; ++row_cnt)
//    {
//        for(int col_cnt = 0; col_cnt < output.cols; ++col_cnt)
//        {
//            const Point2i pt(col_cnt, row_cnt);
//            const Vec2f local_vec(local_dx.at<float>(pt), local_dy.at<float>(pt));
//            const Vec2f strong_vec(strong_dx.at<float>(pt), strong_dy.at<float>(pt));
//            const Vec2f diff = local_vec - strong_vec;

//            float unsimilarity_score = sqrt(diff[0]*diff[0] + diff[1]*diff[1]);

//            output.at<uchar>(pt) = static_cast<uchar>(unsimilarity_score*255*2);

//        }
//    }
}

const int ringbuffer_length = 2;
Mat img_ringbuffer[ringbuffer_length];
int oldest_img_idx = 0;
int newest_img_idx = 0;
void exec2(const Mat& input, Mat& output)
{
    // convert input img to gray-valued float
    Mat input_gray;
    cvtColor(input, input_gray, CV_RGB2GRAY);
    Mat input_float;
    input_gray.convertTo(input_float, CV_32FC1, 1/255.0);

    // ringbuffer: increment newest index with wrap around
    newest_img_idx = (newest_img_idx+1) % ringbuffer_length;

    // if newest index meets with oldest index (buffer full), 'delete' the oldest sample
    if(newest_img_idx == oldest_img_idx)
        oldest_img_idx = (oldest_img_idx+1) % ringbuffer_length;

    //cout << "newest idx: " << newest_img_idx << " oldest idx: " << oldest_img_idx << endl;

    img_ringbuffer[newest_img_idx] = input_float;


    Mat temp_filtered_img = Mat::zeros(input_float.size(), CV_32FC1);

    // temporal mean filter
    for(int i = 0; i < ringbuffer_length; ++i)
    {
        if(!img_ringbuffer[i].empty())
        {
            temp_filtered_img = temp_filtered_img + img_ringbuffer[i];
        }
    }
    temp_filtered_img = temp_filtered_img/ringbuffer_length;


//    // temporal median filter of all ringbuffer images for all pixels
//    for(int row_cnt = 0; row_cnt < input_float.rows; ++row_cnt)
//    {
//        for(int col_cnt = 0; col_cnt < input_float.cols; ++col_cnt)
//        {
//            float* const temp_values = Malloc(float, ringbuffer_length);
//            for(int i = 0; i < ringbuffer_length; ++i)
//            {
//                if(!img_ringbuffer[i].empty())
//                {
//                    temp_values[i] = img_ringbuffer[i].at<float>(row_cnt, col_cnt);
//                }
//            }
//            quicksort_iterative<float>(temp_values, ringbuffer_length);
//            temp_filtered_img.at<float>(row_cnt, col_cnt) = temp_values[ringbuffer_length/2];
//            //cout << "med-val:" << temp_values[ringbuffer_length/2] << endl;
//            free(temp_values);
//        }
//    }

    //cout << "hier" << endl;



    temp_filtered_img.convertTo(output, CV_8UC1, 255);
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
