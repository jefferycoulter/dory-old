#ifndef DORY_FRAME_INFO_INCL
#define DORY_FRAME_INFO_INCL

#include "renderer/camera.h"
#include <vulkan/vulkan.h>

namespace DORY
{
    /**
     * @brief struct containing information related to a frame passed to the renderer.
     */
    struct FrameInfo
    {
        int frame_index;
        float frame_time;
        VkCommandBuffer command_buffer;
        Camera &camera;
        VkDescriptorSet descriptor_set;
    }; // struct FrameInfo
} // namespace DORY

#endif // DORY_FRAME_INFO_INCL