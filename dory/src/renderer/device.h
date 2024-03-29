#ifndef DORY_DEVICE_INCL
#define DORY_DEVICE_INCL

#include "platform/window.h"
#include "utils/nocopy.h"

#include <vector>
#include <string>

namespace DORY
{
    /**
     * @brief struct containing info about swap chain supported capabilities, formats, and present modes
     */
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR _capabilities;
        std::vector<VkSurfaceFormatKHR> _formats;
        std::vector<VkPresentModeKHR> _present_modes;
    };

    /**
     * @brief a struct for bundling together queue families.  _graphics_family and _present_family are hold
     * the indices of the queue families that support graphics and presentation respectively which have been
     * selected by ::Device::FindQueueFamilies()
     */
    struct QueueFamilyIndices
    {
        uint32_t _graphics_family;
        uint32_t _present_family;
        bool _graphics_family_has_value = false;
        bool _present_family_has_value = false;
        bool IsComplete() { return _graphics_family_has_value && _present_family_has_value; }
    };

    /**
     * @brief this class represents the physical device and its associated queues.  it is responsible for
     * determing which physical device to use and setting up the logical device.  it also creates the command 
     * pool and debug messenger
     */
    class Device : public NoCopy
    {
        public:
            #ifdef NDEBUG
                const bool EnableValidationLayers = false;
            #else
                const bool EnableValidationLayers = true;
            #endif
            
            /**
             * @brief construct a new Device object. this includes: initializing Vulkan, setting up 
             * validation layers, finding suitable graphics card (device), creating command pool...
             * @param window reference to the window that the device will run in
             */
            Device(Window &window);

            /**
             * @brief destroy the Device object
             */
            ~Device();

            /**
             * @brief prevent moving of the device object
             */
            Device(Device &&) = delete;
            Device &operator=(Device &&) = delete;

            /**
             * @brief get the device's command pool. this is an object from which the command buffer
             * will take memory, that way memory allocation only needs to be done once.
             * function to return private members
             * @return VkCommandPool 
             */
            VkCommandPool GetCommandPool() { return m_command_pool; }

            /**
             * @brief get the handle for this device. function to return private members
             * @return VkDevice 
             */
            VkDevice GetDevice() { return m_device; }

            /**
             * @brief get the vulkan surface created by this device. function to return private members
             * @return VkSurfaceKHR 
             */
            VkSurfaceKHR GetSurface() { return m_surface; }

            /**
             * @brief get the graphics queue, which is created in ::Device::CreateLogicalDevice().
             * function to return private members
             * @return VkQueue 
             */
            VkQueue GetGraphicsQueue() { return m_graphics_queue; }

            /**
             * @brief get the present queue, which is created in ::Device::CreateLogicalDevice().
             * function to return private members
             * @return VkQueue 
             */
            VkQueue GetPresentQueue() { return m_present_queue; }

            /**
             * @brief get supported swapchain functionality.
             * @return SwapChainSupportDetails 
             */
            SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_physical_device); }
            uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            
            /**
             * @brief this calls the private member function FindQueueFamilies() to return the indices of
             * the queue families to an external caller.
             * @return QueueFamilyIndices 
             */
            QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_physical_device); }
            
            /**
             * @brief find supported formats for swap chain images
             * @param candidates candidate formats to check
             * @param tiling tiling mode of the image
             * @param features features of the image
             * @return VkFormat 
             */
            VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

            /**
             * @brief create a buffer object
             * @param size size of the buffer to be created
             * @param usage usage of the buffer to be created (e.g. vertex buffer, index buffer, uniform buffer)
             * @param properties properties of the buffer to be created (e.g. host visible, host coherent, device local)
             * @param buffer pointer to where the created buffer will be stored
             * @param buffer_memory pointer to the created buffer's memory size
             */
            void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &buffer_memory);
            VkCommandBuffer BeginSingleTimeCommands();
            void EndSingleTimeCommands(VkCommandBuffer command_buffer);

            /**
             * @brief copy data from one buffer to another
             * @param src_buffer source buffer
             * @param dst_buffer destination buffer
             * @param size size of buffer to copy
             */
            void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

            /**
             * @brief copy buffer to image
             * @param buffer buffer to copy
             * @param image image to copy to
             * @param width width of the image
             * @param height height of the image
             * @param layer_count number of layers in the image
             */
            void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);

            /**
             * @brief copy image to buffer
             * @param image_info image info
             * @param properties properties of the buffer
             * @param image image to copy
             * @param image_memory image memory object
             */
            void CreateImageWithInfo(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &image_memory);

            VkPhysicalDeviceProperties m_properties; // physical device properties in device

        private: // methods
            /**
             * @brief initialize the Vulkan library to connect with the application
             */
            void CreateInstance();

            /**
             * @brief set up debugging messages since Vulkan doesn't really perform error checking
             */
            void SetupDebugMessenger();

            /**
             * @brief create a surface.  connects glfw with Vulkan rendering.  see ::Window::CreateWindowSurface()
             */
            void CreateSurface();

            /**
             * @brief choose the physical/graphical device (e.g. GPU) that the application will
             * use to run Vulkan.  does not allow for multiple devices to be selected.
             * if multiple devices are available, it picks the first "suitable" device
             */
            void PickPhysicalDevice();

            /**
             * @brief create the logical device corresponding to the previously selected physical
             * device. determine what features of the physical device selected above should be used
             * for running the application, e.g. which queue families should be created/used, etc
             */
            void CreateLogicalDevice();

            /**
             * @brief create a command pool for command buffer allocation
             */
            void CreateCommandPool();

            /**
             * @brief check whether the device meets certain requirements, e.g. extensions, etc.
             * @param device the device to check
             * @return true 
             * @return false 
             */
            bool IsDeviceSuitable(VkPhysicalDevice device);

            /**
             * @brief get required extensions specified in ::Device::_device_extensions vector
             * 
             * @return std::vector<const char *> 
             */
            std::vector<const char *> GetRequiredExtensions();

            /**
             * @brief check to see if all of the layers specified in the device's _validation_layers 
             * member vector are available for use
             * @return true 
             * @return false 
             */
            bool CheckValidationLayerSupport();

            /**
             * @brief find the queue families that are supported by the chosen device if they are available.
             * @param device 
             * @return QueueFamilyIndices 
             * 
             * @link https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
             */
            QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

            /**
             * @brief fill in the details for the debug messenger struct
             * @param create_info pointer to the struct to be filled in
             */
            void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info);

            /**
             * @brief get the required extensions for glfw and the available extensions on the current device.
             * if the required glfw extensions aren't available, a runtime error is thrown
             */
            void HasGflwRequiredInstanceExtensions();

            /**
             * @brief check to see if all of the required extensions are available on the current device.
             * @param device device to check
             * @return true 
             * @return false 
             */
            bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

            /**
             * @brief find the supported swap chain properties for the current device.
             * @param device 
             * @return SwapChainSupportDetails 
             */
            SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        private: // members
            VkInstance m_instance; // vulkan instance in device
            VkDebugUtilsMessengerEXT m_debug_messenger; // debug messenger for vulkan in device
            VkPhysicalDevice m_physical_device = VK_NULL_HANDLE; // physical device in device
            Window &m_window; // window in device
            VkCommandPool m_command_pool; // command pool in device

            VkDevice m_device; // logical device in device
            VkSurfaceKHR m_surface; // surface in device
            VkQueue m_graphics_queue; // graphics queue in device
            VkQueue m_present_queue; // present queue in device

            /**
             * @brief vector enabling the "useful standard validation"
             * @link https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
             */
            const std::vector<const char *> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};

            /**
             * @brief specify required device extensions.
             * VK_KHR_SWAPCHAIN_EXTENSION_NAME is required to ensure swapchain use is supported.
             * VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME is required to prevent warnings/errors on mac M1.
             */
            #if defined(__APPLE__)
                const std::vector<const char *> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};
            #elif defined (_WIN64)
                const std::vector<const char*> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
            #endif  
    }; // class Device
} // namepsace DORY

#endif // DORY_DEVICE_INCL