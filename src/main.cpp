// Libraries
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <opencv2/rgbd.hpp>     // OpenCV RGBD Contrib package

// STD
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

using namespace cv;

float get_depth_scale(rs2::device dev);
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);
void remove_background(rs2::video_frame& other, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist);

/*
* Class for enqueuing and dequeuing cv::Mats efficiently
* Thanks to this awesome post by PKLab
* http://pklab.net/index.php?id=394&lang=EN
*/
class QueuedMat{
public:

    Mat img; // Standard cv::Mat

    QueuedMat(){}; // Default constructor

    // Destructor (called by queue::pop)
    ~QueuedMat(){
        img.release();
    };

    // Copy constructor (called by queue::push)
    QueuedMat(const QueuedMat& src){
        src.img.copyTo(img);
    };
};

/*
* Awesome method for visualizing the 16bit unsigned depth data using a histogram, slighly modified (:
* Thanks to @Catree from https://stackoverflow.com/questions/42356562/realsense-opencv-depth-image-too-dark
*/
void make_depth_histogram(const Mat &depth, Mat &normalized_depth, int coloringMethod) {
  normalized_depth = Mat(depth.size(), CV_8U);
  int width = depth.cols, height = depth.rows;

  static uint32_t histogram[0x10000];
  memset(histogram, 0, sizeof(histogram));

  for(int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      ++histogram[depth.at<ushort>(i,j)];
    }
  }

  for(int i = 2; i < 0x10000; ++i) histogram[i] += histogram[i-1]; // Build a cumulative histogram for the indices in [1,0xFFFF]

  for(int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if (uint16_t d = depth.at<ushort>(i,j)) {
        int f = histogram[d] * 255 / histogram[0xFFFF]; // 0-255 based on histogram location
        normalized_depth.at<uchar>(i,j) = static_cast<uchar>(f);
      } else {
        normalized_depth.at<uchar>(i,j) = 0;
      }
    }
  }

  // Apply the colormap:
  applyColorMap(normalized_depth, normalized_depth, coloringMethod);
}

int main(int argc, char * argv[]) try {

    // Create a pipeline to easily configure and start the camera
    rs2::pipeline pipe;
    //Calling pipeline's start() without any additional parameters will start the first device
    // with its default streams.
    //The start function returns the pipeline profile which the pipeline used to start the device
    rs2::pipeline_profile profile = pipe.start();

    // Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    float depth_scale = get_depth_scale(profile.get_device());

    //Pipeline could choose a device that does not have a color stream
    //If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(profile.get_streams());

    // Create a rs2::align object.
    // rs2::align allows us to perform alignment of depth frames to others frames
    //The "align_to" is the stream type to which we plan to align depth frames.
    rs2::align align(align_to);

    // openCV window
    const auto window_name = "Vimeo - Depth Viewer";
    namedWindow(window_name, WINDOW_AUTOSIZE);


    // Atomic boolean to allow thread safe way to stop the thread
    std::atomic_bool stopped(true);

    // The threaded processing thread function
    std::thread processing_thread([&]() {
        while (!stopped){

        }
    });

    Mat depthDequeuedMat(Size(640, 480), CV_8UC3);

    //Main thread function
    while (waitKey(1) < 0 && cvGetWindowHandle(window_name)){

        // Using the align object, we block the application until a frameset is available
        rs2::frameset frameset = pipe.wait_for_frames();

        // rs2::pipeline::wait_for_frames() can replace the device it uses in case of device error or disconnection.
        // Since rs2::align is aligning depth to some other stream, we need to make sure that the stream was not changed
        //  after the call to wait_for_frames();
        if (profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams()))
        {
            //If the profile was changed, update the align object, and also get the new device's depth scale
            profile = pipe.get_active_profile();
            align_to = find_stream_to_align(profile.get_streams());
            align = rs2::align(align_to);
            depth_scale = get_depth_scale(profile.get_device());
        }

        //Get processed aligned frame
        auto processed = align.process(frameset);

        // Trying to get both other and aligned depth frames
        rs2::video_frame other_frame = processed.first(align_to);
        rs2::depth_frame aligned_depth_frame = processed.get_depth_frame();

        //If one of them is unavailable, continue iteration
        if (!aligned_depth_frame || !other_frame)
        {
            continue;
        }

        // Define a variable for controlling the distance to clip
        float depth_clipping_distance = 0.5f;

        remove_background(other_frame, aligned_depth_frame, depth_scale, depth_clipping_distance);

        // Query frame size (width and height)
        const int w = aligned_depth_frame.as<rs2::depth_frame>().get_width();
        const int h = aligned_depth_frame.as<rs2::depth_frame>().get_height();

        Mat rawColorMat(Size(w,h), CV_8UC3, (void*)other_frame.get_data());

        // Create an openCV matrix from the raw depth (CV_16U holds a matrix of 16bit unsigned ints)
        Mat rawDepthMat(Size(w, h), CV_16U, (void*)aligned_depth_frame.get_data());

        Mat coloredDepth;

        make_depth_histogram(rawDepthMat, coloredDepth, COLORMAP_JET);

        Mat res;
        cvtColor(rawColorMat,rawColorMat,COLOR_BGR2RGB);
        vconcat(rawColorMat, coloredDepth, res);
        imshow(window_name, res);
    }

    // Signal the processing thread to stop, and join
    stopped = true;
    processing_thread.join();

    return EXIT_SUCCESS;
}
catch (const rs2::error & e){
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e){
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

void remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist)
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threashold
            if (pixels_distance <= 0.f || pixels_distance > clipping_dist)
            {
                // Calculate the offset in other frame's buffer to current pixel
                auto offset = depth_pixel_index * other_bpp;

                // Set pixel to "background" color (0x999999)
                std::memset(&p_other_frame[offset], 0x99, other_bpp);
            }
        }
    }
}

float get_depth_scale(rs2::device dev)
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor if a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams)
{
    //Given a vector of streams, we try to find a depth stream and another stream to align depth with.
    //We prioritize color streams to make the view look better.
    //If color is not available, we take another stream that (other than depth)
    rs2_stream align_to = RS2_STREAM_ANY;
    bool depth_stream_found = false;
    bool color_stream_found = false;
    for (rs2::stream_profile sp : streams)
    {
        rs2_stream profile_stream = sp.stream_type();
        if (profile_stream != RS2_STREAM_DEPTH)
        {
            if (!color_stream_found)         //Prefer color
                align_to = profile_stream;

            if (profile_stream == RS2_STREAM_COLOR)
            {
                color_stream_found = true;
            }
        }
        else
        {
            depth_stream_found = true;
        }
    }

    if(!depth_stream_found)
        throw std::runtime_error("No Depth stream available");

    if (align_to == RS2_STREAM_ANY)
        throw std::runtime_error("No stream found to align with Depth");

    return align_to;
}

bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev)
{
    for (auto&& sp : prev)
    {
        //If previous profile is in current (maybe just added another)
        auto itr = std::find_if(std::begin(current), std::end(current), [&sp](const rs2::stream_profile& current_sp) { return sp.unique_id() == current_sp.unique_id(); });
        if (itr == std::end(current)) //If it previous stream wasn't found in current
        {
            return true;
        }
    }
    return false;
}
