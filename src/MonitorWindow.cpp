#include "MonitorWindow.h"

MonitorWindow::MonitorWindow(const Eigen::Vector2i & size, const std::string & caption)
        : Screen(size, caption)
{

  _videoview = new VideoView(this);
  _videoview->setBackgroundColor(theme()->mWindowFillFocused);
  _videoview->setSize(this->size());
  performLayout();
}

bool MonitorWindow::keyboardEvent(int key, int scancode, int action, int modifiers)
{
  return false;
}

bool MonitorWindow::resizeEvent(const Eigen::Vector2i & size)
{
  return true;
}

void MonitorWindow::setVideoFrame(rs2::frame _color, rs2::frame _depth){
  _videoview->setFrame(_color, _depth);
}

void MonitorWindow::draw(NVGcontext *ctx)
{
  Screen::draw(ctx);
}
