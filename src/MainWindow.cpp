#include "MainWindow.h"

MainWindow::MainWindow(const Vector2i & size, const string & caption)
        : Screen(size, caption)
        , _isVideoStarted{ false }
        , _colorRatio{ 16.0f / 9.0f }
        , _depthRatio{ 16.0f / 9.0f }
        ,isClipping{ false }
        ,depth_clipping_distance{ 1.0f }{

    std::cout.setf(std::ios::fixed);

    _logo = new Window(this, "");
    _logo->setPosition(Vector2i(15, 15));
    _logo->setLayout(new GroupLayout());

    _btnLogo = _logo->add<Button>("");
    _btnLogo->setIcon(ENTYPO_ICON_VIMEO);
    _btnLogo->setTextColor(Color(26, 183, 234, 255));
    _btnLogo->setChangeCallback([&](bool state) {
      system("open https://vimeo.com");
    });

    // Create a new window to hold open/close stream buttons
    _views = new Window(this, "");
    _views->setPosition(Vector2i(90, 15));
    _views->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 15, 15));

    // Stream button
    _btnStream = _views->add<Button>("");
    _btnStream->setIcon(ENTYPO_ICON_CONTROLLER_PLAY);
    _btnStream->setFlags(Button::ToggleButton);
    _btnStream->setTooltip("Start streaming RGB and depth from a connected RealSense sensor 🎥");
    _btnStream->setChangeCallback([&](bool state) { onToggleStream(state); });

    //Clipping button
    _btnClipping = _views->add<Button>("");
    _btnClipping->setIcon(ENTYPO_ICON_SCISSORS);
    _btnClipping->setFlags(Button::ToggleButton);
    _btnClipping->setTooltip("Open depth clipping widget");
    _btnClipping->setChangeCallback([&](bool state) { onToggleClipping(state); });

    //Placeholders for the window elements
    _streamWindow = nullptr;

    performLayout();

    rs2::context ctx;
    if(ctx.query_devices().size() > 0){
      std::cout << "[Vimeo - Depth Viewer] Found " << ctx.query_devices().size() << " RealSense sensors connected." << std::endl;
    }

}

void MainWindow::onToggleClipping(bool on)
{
  if(on){
    clippingPanel = new Window(this, "Clipping");
    clippingPanel->setPosition(Vector2i(350, 90));
    clippingPanel->setLayout(new GroupLayout());

    // Disable/enable depth clipping
    CheckBox *cb = new CheckBox(clippingPanel, "Depth Clipping",
            [this](bool state) { isClipping = state; }
    );


    Widget *alignmentWidget = new Widget(clippingPanel);
    alignmentWidget->setLayout(new BoxLayout(Orientation::Horizontal,
                                           Alignment::Middle, 15, 15));
    // Clipping slider
    Slider *slider = new Slider(alignmentWidget);
    slider->setValue(0.5f);
    slider->setFixedWidth(150);

    // Clipping value text label
    TextBox *textBox = new TextBox(alignmentWidget);
    textBox->setFixedSize(Vector2i(60, 25));
    textBox->setValue("0.5");
    slider->setCallback([this, textBox](float value) {
        depth_clipping_distance = value * 15.0f;
        textBox->setValue(std::to_string((int)depth_clipping_distance));
    });
    textBox->setFixedSize(Vector2i(60,25));
    textBox->setFontSize(20);
    textBox->setAlignment(TextBox::Alignment::Right);

  } else {
    clippingPanel->dispose();
  }

  //Perform new GUI layout
  performLayout();
}

void MainWindow::onToggleStream(bool on)
{
    if (on && !tryStartVideo())
    {
        _btnStream->setPushed(false);
        _btnStream->setIcon(ENTYPO_ICON_CONTROLLER_PLAY);
        return;
    }

    if (on && _streamWindow == nullptr)
    {
        std::cout << "[Vimeo - Depth Viewer] Opening stream 🎥" << std::endl;
        _streamWindow = new VideoWindow(this,"Stream");
        _streamWindow->setPosition(Vector2i(15, 90));
        _btnStream->setIcon(ENTYPO_ICON_CONTROLLER_STOP);
        _streamWindow->setSize(Vector2i(640, 960));
        performLayout();
        resizeEvent(this->size());
    }
    else if (!on && _streamWindow != nullptr)
    {
        std::cout << "[Vimeo - Depth Viewer] Closing stream 🎥" << std::endl;
        _streamWindow->dispose();
        _streamWindow = nullptr;
        stopVideo();
        _btnStream->setIcon(ENTYPO_ICON_CONTROLLER_PLAY);
    }
}

bool MainWindow::keyboardEvent(int key, int scancode, int action, int modifiers)
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

bool MainWindow::resizeEvent(const Eigen::Vector2i & size)
{
    return true;
}

void MainWindow::draw(NVGcontext * ctx)
{
    if (isVideoStarted())
    {
        rs2::frameset frames = _pipe.wait_for_frames();

        if (profile_changed(_pipe.get_active_profile().get_streams(), profile.get_streams()))
        {
            //If the profile was changed, update the align object, and also get the new device's depth scale
            profile = _pipe.get_active_profile();
            align_to = find_stream_to_align(profile.get_streams());
            _align = new rs2::align(align_to);
            _depthScale = get_depth_scale(profile.get_device());
        }

        frames = _align->process(frames);
        rs2::video_frame colorFrame = frames.first(align_to);
        rs2::depth_frame depthFrame = frames.get_depth_frame();

        if(isClipping){
          remove_background(colorFrame, depthFrame, _depthScale, depth_clipping_distance);
        }

        if (_streamWindow != nullptr){

          //Create a colorizer which maps depth to 8bit color space
          rs2::colorizer colormap;

          //Set the current frame to the frame buffer
          _streamWindow->setVideoFrame(colorFrame, colormap(depthFrame));

        }


    }

    Screen::draw(ctx);
}

bool MainWindow::tryStartVideo()
{
    lock_guard<mutex> guard{ _mutex };

    if (_isVideoStarted)
        return true;

    try
    {
        // check if realsense device connected
        rs2::context ctx;
        if (ctx.query_devices().size() == 0)
        {
            std::cout << "[Vimeo - Depth Viewer] Could not find any RealSense camera connected" << std::endl;
            new MessageDialog(this, MessageDialog::Type::Warning, "Warning", "No RealSense device is found, please connect the device and try again.");
            return false;
        }

        profile = _pipe.start();
        _depthScale =  get_depth_scale(profile.get_device());

        align_to = find_stream_to_align(profile.get_streams());
        _align = new rs2::align(align_to);

        _isVideoStarted = true;
        return true;
    }
    catch (const rs2::error & e)
    {
        std::cout << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() <<std::endl;
        return false;
    }
    catch (const std::exception & e)
    {
        std::cout << std::string(e.what()) << std::endl;
        return false;
    }
}
//
void MainWindow::stopVideo()
{
    lock_guard<mutex> guard{ _mutex };

    if (!_isVideoStarted)
        return;

    try
    {
        _isVideoStarted = false;
        _pipe.stop();
    }
    catch (const rs2::error & e)
    {
        std::cout << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() <<std::endl;
        return;
    }
    catch (const std::exception & e)
    {
        std::cout << std::string(e.what()) << std::endl;
        return;
    }
}

bool MainWindow::isVideoStarted()
{
    lock_guard<mutex> guard{ _mutex };
    return _isVideoStarted;
}

bool MainWindow::profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev)
{
    for (auto&& sp : prev)
    {
        //If previous profile is in current (maybe just added another)
        auto itr = std::find_if(std::begin(current), std::end(current), [&sp](const rs2::stream_profile& current_sp) { return sp.unique_id() == current_sp.unique_id(); });
        if (itr == std::end(current)) //If it previous stream wasn't found in current
        {
            return true;
        }
    }
    return false;
}

rs2_stream MainWindow::find_stream_to_align(const std::vector<rs2::stream_profile>& streams)
{
    //Given a vector of streams, we try to find a depth stream and another stream to align depth with.
    //We prioritize color streams to make the view look better.
    //If color is not available, we take another stream that (other than depth)
    rs2_stream align_to = RS2_STREAM_ANY;
    bool depth_stream_found = false;
    bool color_stream_found = false;
    for (rs2::stream_profile sp : streams)
    {
        rs2_stream profile_stream = sp.stream_type();
        if (profile_stream != RS2_STREAM_DEPTH)
        {
            if (!color_stream_found)         //Prefer color
                align_to = profile_stream;

            if (profile_stream == RS2_STREAM_COLOR)
            {
                color_stream_found = true;
            }
        }
        else
        {
            depth_stream_found = true;
        }
    }

    if(!depth_stream_found)
        throw std::runtime_error("No Depth stream available");

    if (align_to == RS2_STREAM_ANY)
        throw std::runtime_error("No stream found to align with Depth");

    return align_to;
}

float MainWindow::get_depth_scale(rs2::device dev)
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor if a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

void MainWindow::remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist)
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threashold
            if (pixels_distance <= 0.f || pixels_distance > clipping_dist)
            {
                // Calculate the offset in other frame's buffer to current pixel
                auto offset = depth_pixel_index * other_bpp;

                // Set pixel to "background" color (0x999999)
                std::memset(&p_other_frame[offset], 0x00, other_bpp);
            }
        }
    }
}
