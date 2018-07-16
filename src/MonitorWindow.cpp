#include "MonitorWindow.h"

MonitorWindow::MonitorWindow(const Eigen::Vector2i & size, const std::string & caption)
        : Screen(size, caption)
{
  performLayout();
}

bool MonitorWindow::keyboardEvent(int key, int scancode, int action, int modifiers)
{
  if (Screen::keyboardEvent(key, scancode, action, modifiers))
      return true;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
      setVisible(false);
      return true;
  }
  return false;
}

bool MonitorWindow::resizeEvent(const Eigen::Vector2i & size)
{
  return true;
}

void MonitorWindow::draw(NVGcontext *ctx)
{
  Screen::draw(ctx);
}
