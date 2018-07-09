#pragma once
#include <string>
#include <nanogui/window.h>
#include <librealsense2/rs.hpp>
#include <Eigen/Core>
#include "VideoView.h"

class VideoWindow : public nanogui::Window
{
public:
    VideoWindow(nanogui::Widget *parent, const std::string &title = "Untitled");
    void setVideoFrame(rs2::frame _color, rs2::frame _depth);
    void setSize(const Eigen::Vector2i &size);

private:
    VideoView *_videoview;
};
