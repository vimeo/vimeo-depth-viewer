#ifndef _CV_UTILS_
#define _CV_UTILS_

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <exception>

using namespace cv;
using namespace rs2;

class CVUtils{
public:
  static Mat frame_to_mat(const frame& f);
  static Mat depth_frame_to_meters(const pipeline& pipe, const depth_frame& f);
  static void make_depth_histogram(const Mat &depth, Mat &normalized_depth, int coloringMethod);
};

#endif //_CV_UTILS_
