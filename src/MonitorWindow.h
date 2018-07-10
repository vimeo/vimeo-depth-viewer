#ifndef _MONITOR_WINDOW_H_
#define _MONITOR_WINDOW_H_


// STD
#include <string>
#include <iostream>

// GLFW
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

using namespace Eigen;
using namespace std;
using namespace nanogui;

class MonitorWindow : public nanogui::Screen
{
public:
    MonitorWindow(const Eigen::Vector2i & size, const std::string & caption);
    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    bool resizeEvent(const Eigen::Vector2i & size) override;
    void draw(NVGcontext *ctx) override;

};


#endif //_MONITOR_WINDOW_H_
