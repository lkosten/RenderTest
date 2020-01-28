#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <map>
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

  const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

  #ifdef NDEBUG
  const bool enableValidationLayers = false;
  #else
  const bool enableValidationLayers = true;
  #endif

  GLFWwindow *window;
  VkInstance instance;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  void initWindow() {
    glfwInit(); // initialize glfw library;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // set specified window hint to desired value
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // 4 - monitor, 5 - relevant only for OpenGL
  }

  void initVulkan() {
    createInstance();
    pickPhysicalDevice();
  }

  void pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
      throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices) {
      candidates.emplace(rateDeviceSuitability(device), device);
    }

    if (candidates.rbegin()->first > 0) {
      physicalDevice = candidates.rbegin()->second;
    }
    else {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  int rateDeviceSuitability(const VkPhysicalDevice &device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
      return 0;
    }

    return score;
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents(); // process all pending events
    }
  }

  void cleanup() {
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate(); // destroy all
  }

  void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error("validation layers requested, but not available!");
    }
    checkExtensoins();


    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;
    if (enableValidationLayers) {
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
      createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
  }

  void checkExtensoins() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions:" << std::endl;
    for (const auto& extension : extensions) {
      std::cout << "\t" << extension.extensionName << std::endl;
    }
  }

  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
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