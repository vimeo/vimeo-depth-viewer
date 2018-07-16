// Libraries
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <nanogui/common.h>     // NanoGUI
#include <nanogui/object.h>     // NanoGUI
#include <Eigen/Core>

//App components
#include "MainWindow.h"
#include "MonitorWindow.h"

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

std::atomic_bool stopped(false);

int main(int argc, char * argv[]) try {

      //Initialize performance thread
      std::thread processing_thread([&]() {
        while (!stopped) //While application is running
        {

        }
      });
      // initialize GUI
      nanogui::init();
      {
          //GUI view
          nanogui::ref<MainWindow> guiMain = new MainWindow(Eigen::Vector2i(700, 640), "Vimeo - Depth Viewer");
          nanogui::ref<MonitorWindow> monitor = new MonitorWindow(Eigen::Vector2i(640, 960), "Vimeo - Monitor");

          guiMain->drawAll();
          guiMain->setVisible(true);

          monitor->drawContents();
          // monitor->setVisible(true);

          /*
          * Print instructions into the console
          */
          std::cout << "[Vimeo - Depth Viewer] GUI Initialized 💪🏻" << std::endl;
          std::cout << "  - Use escape key to quit" << std::endl;
          nanogui::mainloop(30);
      }

      nanogui::shutdown();
      stopped = true;
      processing_thread.join();
      std::cout << "[Vimeo - Depth Viewer] Goodbye 👋🏻" << std::endl;

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
