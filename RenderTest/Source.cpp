#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  const int WIDTH = 800;
  const int HEIGHT = 600;

  GLFWwindow *window;
  VkInstance instance;

  void initWindow() {
    glfwInit(); // initialize glfw library;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // set specified window hint to desired value
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // 4 - monitor, 5 - relevant only for OpenGL
  }

  void initVulkan() {
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents(); // process all pending events
    }
  }

  void cleanup() {
    glfwDestroyWindow(window);

    glfwTerminate(); // destroy all
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}