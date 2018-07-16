#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <string>
#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <librealsense2/rs.hpp>

//GLFW
#include <GLFW/glfw3.h>

#include <Eigen/Core>

//STD
#include <iostream>
#include <string>
#include <vector>

using namespace nanogui;
using namespace rs2;
using namespace Eigen;

////////////////////////////////
///////////Renderer/////////////
/////////////////////////c///////
class Renderer {
public:

	//GLFW window
	GLFWwindow* window;

	int width, height;

	Renderer();

    void begin();

		void draw();

    void end();

	void dispose();

	bool nextFrame();

	double currentTime();
	
	void setFrame(rs2::frame _color, rs2::frame _depth);

private:
	GLuint textures[2];
	const std::string _glslVertex;
	const std::string _glslFragment;
	nanogui::GLShader _shader;
	rs2::frame_queue _colorQueue;
	rs2::frame_queue _depthQueue;

	void setupWindow();
};

#endif
