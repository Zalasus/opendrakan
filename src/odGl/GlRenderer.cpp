/**
 * @file GlRenderer.cpp
 * @date Jan 26, 2020
 * @author zal
 */

#include <odGl/GlRenderer.h>

#include <odCore/Logger.h>

namespace odGl
{

    void _glfwErrorCallback(int error, const char* description)
    {
        Logger::error() << "GLFW error: " << description;
    }


    GlRenderer::GlRenderer()
    : mWindow(nullptr)
    {
        if(!glfwInit())
        {
            throw od::Exception("Failed to initialize GLFW");
        }

        glfwSetErrorCallback(&_glfwErrorCallback);

        // we want OpenGL 3.0 (shaders are required)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        mWindow = glfwCreateWindow(640, 480, "OpenDrakan (GL)", NULL, NULL);
        if(mWindow == nullptr)
        {
            throw od::Exception("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(mWindow);

        gladLoadGLLoader(&glfwGetProcAddress);
    }

    GlRenderer::~GlRenderer()
    {
        if(mWindow != nullptr)
        {
            glfwDestroyWindow(mWindow);
        }

        glfwTerminate();
    }


}
