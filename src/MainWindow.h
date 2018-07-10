#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

// STD
#include <string>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <sstream>
#include <cmath>
#include <iomanip>


#include <GLFW/glfw3.h>

// Eigen
#include <Eigen/Core>

// NanoGUI
#include <nanogui/common.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/button.h>
#include <nanogui/checkbox.h>
#include <nanogui/messagedialog.h>
#include <nanogui/entypo.h>
#include <nanogui/slider.h>
#include <nanogui/textbox.h>

// Realsense
#include <librealsense2/rs.hpp>

//App components
#include "VideoWindow.h"

using namespace std;
using namespace Eigen;
using namespace nanogui;

class MainWindow : public nanogui::Screen
{
public:
    MainWindow(const Eigen::Vector2i & size, const std::string & caption);
    void onToggleStream(bool on);
    void onToggleClipping(bool on);
    void onToggleSettings(bool on);
    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    bool resizeEvent(const Eigen::Vector2i & size) override;
    void draw(NVGcontext *ctx) override;

protected:
    bool tryStartVideo();
    void stopVideo();
    bool isVideoStarted();
    bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);
    rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
    float get_depth_scale(rs2::device dev);
    void remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist);

private:
    float depth_clipping_distance;

    bool isClipping;

    //Window objects
    nanogui::Window *clippingPanel;
    nanogui::Window *_logo;
    nanogui::Window *_views;
    nanogui::Window *_edit;

    nanogui::Button *_btnLogo;
    nanogui::Button *_btnStream;
    nanogui::Button *_btnClipping;

    VideoWindow *_streamWindow;

    const float _colorRatio;
    const float _depthRatio;

    std::mutex _mutex;

    bool _isVideoStarted;

    rs2::pipeline _pipe;
    rs2::align *_align;

    float _depthScale;

    //Alignment profile
    rs2_stream align_to;
    rs2::pipeline_profile profile;

};

#endif /* MAIN_WINDOW_H_ */
