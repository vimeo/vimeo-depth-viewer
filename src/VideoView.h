#pragma once
#include <string>
#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <librealsense2/rs.hpp>

class VideoView : public nanogui::GLCanvas
{
public:
    VideoView(nanogui::Widget *parent);
    ~VideoView();
    void setFrame(rs2::frame _color, rs2::frame _depth);
    void drawGL() override;

private:
    GLuint textures[2];
    nanogui::GLShader _shader;
    const std::string _glslVertex;
    const std::string _glslFragment;
    uint32_t _colortextureid;
    uint32_t _depthtextureid;
    rs2::frame_queue _colorQueue;
    rs2::frame_queue _depthQueue;
};
