#include "GradientOrientations.h"


using namespace std;
using namespace cv;


GradientOrientations::GradientOrientations()
{
    // Threshold to reduce noise
    mag_threshold_ = 0.001;

    // Sobel parameter
    ddepth_ = CV_32F;
}

void GradientOrientations::exec(const Mat &input, Mat &output)
{
    // Step 1: get for each image pixel the gradient
    Mat mag_resp;
    Mat d_x, d_y;

    Mat temp = input;
    // Blur
    //GaussianBlur(input, temp, Size(3,3), 0, 0, BORDER_DEFAULT);
    // Gradient X
    Sobel(temp, d_x, ddepth_, 1, 0, 3);
    // Gradient Y
    Sobel(temp, d_y, ddepth_, 0, 1, 3);

    // superposition of the absolute of both directions
    magnitude(d_x, d_y, mag_resp);

    // Step 2: for each pixel, find the largest gradient of the color channels
    Mat dominant_orientations_unit_vec = Mat::zeros(input.size(), CV_32FC2);
    for(int row_cnt = 0; row_cnt < dominant_orientations_unit_vec.rows; ++row_cnt)
    {
        for(int col_cnt = 0; col_cnt < dominant_orientations_unit_vec.cols; ++col_cnt)
        {
            // for each point in the image
            Point2i pt(col_cnt, row_cnt);

            // for magnitude > mag_threshold
            float mag_val = mag_resp.at<float>(pt);
            if(mag_resp.at<float>(pt) > mag_threshold_)
            {
                float unit_vec_x = d_x.at<float>(pt)/mag_val;
                float unit_vec_y = d_y.at<float>(pt)/mag_val;
                dominant_orientations_unit_vec.at<Vec2f>(pt) = Vec2f(unit_vec_x, unit_vec_y); // only consider the orientation (0..pi)
                //cout << sqrt(unit_vec_x*unit_vec_x + unit_vec_y*unit_vec_y);
            }
        }
    }

    //    Mat vis_x;
    //    d_x[0].convertTo(vis_x, CV_8UC1);
    //    PAUSE_AND_SHOW(vis_x)

    // abs dot product images
    const Vec2f vec0deg(1,0);
    const Vec2f vec45deg(0.707107, 0.707107);
    const Vec2f vec90deg(0,1);
    const Vec2f vec135deg(-0.707107, 0.707107);


    //Dominant orientation visualization:
    output = Mat::zeros(input.size(), CV_8UC3);
    for(int row_cnt = 0; row_cnt < dominant_orientations_unit_vec.rows; ++row_cnt)
    {
        for(int col_cnt = 0; col_cnt < dominant_orientations_unit_vec.cols; ++col_cnt)
        {
            Point2i pt(col_cnt, row_cnt);
            {
                const float i = mag_resp.at<float>(pt)/75;
                const uchar r = fabs(dominant_orientations_unit_vec.at<Vec2f>(pt).dot(vec0deg))*255;
                const uchar g = fabs(dominant_orientations_unit_vec.at<Vec2f>(pt).dot(vec90deg))*255;
                const uchar b = fabs(dominant_orientations_unit_vec.at<Vec2f>(pt).dot(vec45deg))*255;
                output.at<Vec3b>(pt) = Vec3b(b*i, g*i, r*i);
            }
        }
    }
    //imwrite("orientations.png", vis_img);
    //PAUSE_AND_SHOW(vis_img);
//    vis_img = Mat::zeros(working_data.data().size(), CV_8UC1);
//    for(int row_cnt = 0; row_cnt < dominant_orientations_unit_vec.rows; ++row_cnt)
//    {
//        for(int col_cnt = 0; col_cnt < dominant_orientations_unit_vec.cols; ++col_cnt)
//        {
//            Point2i pt(col_cnt, row_cnt);
//            if(thresholded_dominant_mag.at<float>(pt) != 0)
//            {
//                vis_img.at<uchar>(pt) = thresholded_dominant_mag.at<float>(pt);
//            }
//        }
//    }
//    PAUSE_AND_SHOW(vis_img);
//    int grad_vis_cnt = 0;
//    for(int row_cnt = 0; row_cnt < dominant_orientations_unit_vec.rows; ++row_cnt)
//    {
//        for(int col_cnt = 0; col_cnt < dominant_orientations_unit_vec.cols; ++col_cnt)
//        {
//            Point2i pt(col_cnt, row_cnt);
//            if(grad_vis_cnt++ > 10 && thresholded_dominant_mag.at<float>(pt) != 0)
//            {
//                const float l = thresholded_dominant_mag.at<float>(pt)/50;
//                Point2i pt2 = pt + Point2i(-dominant_orientations_unit_vec.at<Vec2f>(pt).val[1]*l, dominant_orientations_unit_vec.at<Vec2f>(pt).val[0]*l);
//                line(vis_img, pt, pt2, Scalar(0,0,255), 1);
//                grad_vis_cnt = 0;
//            }
//        }
//    }
//    PAUSE_AND_SHOW(vis_img);

}
