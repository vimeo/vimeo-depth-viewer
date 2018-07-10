#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <nanogui/nanogui.h>
#include <nanogui/glutil.h>

//GLFW
#include <GLFW/glfw3.h>

//STD
#include <iostream>
#include <string>
#include <vector>

using namespace nanogui;

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

    void end();

	void dispose();

	bool nextFrame();

	double currentTime();

private:

	const std::string _glslVertex;
	const std::string _glslFragment;
	nanogui::GLShader _shader;

	void setupWindow();
};

#endif
