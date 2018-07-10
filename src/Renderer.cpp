#include "Renderer.h"


Renderer::Renderer() : _glslVertex{ R"(
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
    })" } {
    setupWindow();
}

void Renderer::begin(){

    //OpenGL clear op
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}

void Renderer::end(){
    //Post drawing processing(swap front and back buffers)
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::dispose() {
    glfwTerminate();
}

bool Renderer::nextFrame() {
    if (glfwWindowShouldClose(window)) return false;
    else return true;
}

double Renderer::currentTime() {
    double time = glfwGetTime();
    return time;
}

void Renderer::setupWindow() {
    // Initialize the library
    if (!glfwInit())
        return;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    width = 1100;
    height = 1000;
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(width, height, "Volume.GL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

#ifndef __APPLE__
    glewExperimental = true;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
