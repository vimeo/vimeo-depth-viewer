#include <string>
#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
// #include <glad/glad.h>
#include <Eigen/Core>
#include "VideoView.h"

using std::string;
using nanogui::GLCanvas;
using nanogui::GLShader;
using Eigen::MatrixXf;
using Eigen::Vector2f;
using MatrixXu = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic>;

VideoView::VideoView(Widget * parent)
    : GLCanvas(parent)
    , _glslVertex{ R"(
        #version 330 core
        in vec2 position;
        in vec2 coord;
        out vec2 texCoord;
        void main()
        {
            gl_Position = vec4(position.x, position.y, 0.0, 1.0);
            texCoord = coord;
        })" }
    , _glslFragment{ R"(
        #version 330 core
        out vec4 fragColor;
        in vec2 texCoord;
        uniform sampler2D colorFrame;
        uniform sampler2D depthFrame;
        void main()
        {
            //Copy the UV
            vec2 topUV = texCoord;

            //The UV is based on one texture so squeeze it in the Y axis
            topUV.y *= 2.0;

            //Read the color texture
            vec4 top = texture(colorFrame, topUV);

            vec2 bottomUV = texCoord;

            //Squeeze the bottom part
            bottomUV.y *= 2.0;
            bottomUV.y -= 1.0;
            vec4 bottom =  texture(depthFrame, bottomUV);

            vec2 st = gl_FragCoord.xy / texCoord;
            vec3 topBottom;
            topBottom += top.rgb;
            topBottom += bottom.rgb;
            topBottom.b -= 1.0;
            fragColor = vec4(topBottom, 1.0);
        })" }
    , _colorQueue(1)
    , _depthQueue(1)
{
    _shader.init("VideoViewShader", _glslVertex, _glslFragment);

    // positions
    MatrixXf positions(2, 4);
    positions.col(0) << -1,  1; // top left
    positions.col(1) <<  1,  1; // top right
    positions.col(2) << -1, -1; // bottom left
    positions.col(3) <<  1, -1; // bottom right

    // texture coords
    MatrixXf coords(2, 4);
    coords.col(0) << 0, 0; // top left
    coords.col(1) << 1, 0; // top right
    coords.col(2) << 0, 1; // bottom left
    coords.col(3) << 1, 1; // bottom right

    // indices
    MatrixXu indices(3, 2);
    indices.col(0) << 0, 1, 2;  // first triangle
    indices.col(1) << 2, 3, 1;  // first triangle

    // claim and bind to buffers
    _shader.bind();
    _shader.uploadAttrib("position", positions);
    _shader.uploadAttrib("coord", coords);
    _shader.uploadIndices(indices);

    // Create the color texture

    glGenTextures(2, textures);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _shader.setUniform("colorFrame", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _shader.setUniform("depthFrame", 1);
}

VideoView::~VideoView()
{
    _shader.free();
}

void VideoView::setFrame(rs2::frame _color, rs2::frame _depth)
{
    _colorQueue.enqueue(std::move(_color));
    _depthQueue.enqueue(std::move(_depth));
}

void VideoView::drawGL()
{
    // dequeue a frame from queue
    rs2::video_frame color_frame = _colorQueue.wait_for_frame().as<rs2::video_frame>();
    rs2::video_frame depth_frame = _depthQueue.wait_for_frame().as<rs2::video_frame>();
    int frameWidth = color_frame.get_width();
    int frameHeight = color_frame.get_height();
    int depthFrameWidth = depth_frame.get_width();
    int depthFrameHeight = depth_frame.get_height();

    // std::cout << "Color frame height is: " << frameHeight << " | " << "Color frame width is: " << frameWidth << std::endl;
    // std::cout << "Depth frame height is: " << depthFrameHeight << " | " << "Depth frame width is: " << depthFrameWidth << std::endl;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, color_frame.get_data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, depth_frame.get_data());
    glGenerateMipmap(GL_TEXTURE_2D);

    // calculate scale factor
    float viewer_ratio = (float)this->width() / (float)this->height();

    // video frames always have W/H ratio > 1.0
    float frame_ratio = (float)frameWidth * 2 / (float)frameHeight * 2;
    float ratio_diff = frame_ratio - viewer_ratio;



    _shader.bind();

    glEnable(GL_DEPTH_TEST);
    // Draw 2 triangles starting at index 0
    _shader.drawIndexed(GL_TRIANGLES, 0, 2);
    glDisable(GL_DEPTH_TEST);
}
