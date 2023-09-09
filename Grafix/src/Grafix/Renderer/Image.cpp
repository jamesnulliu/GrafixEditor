#include "pch.h"
#include "Image.h"

#include "Grafix/Core/Application.h"

namespace Grafix
{
    static uint32_t BytesPerPixel(ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::RGBA: { return 4; }
        }
        return 0;
    }

    static VkFormat ToVulkanFormat(ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::RGBA: { return VK_FORMAT_R8G8B8A8_UNORM; }
        }
        return (VkFormat)0;
    }

    static uint32_t RGBAToUint32(const glm::vec4& color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    static uint32_t GetMemoryType(VkMemoryPropertyFlags properties, uint32_t typeBits)
    {
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(Application::GetPhysicalDevice(), &memProps);

        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((memProps.memoryTypes[i].propertyFlags & properties) == properties && typeBits & (1 << i))
            {
                return i;
            }
        }

        return 0xffffffff;
    }

    Image::Image(uint32_t width, uint32_t height, ImageFormat format, const void* data)
        : m_Width(width), m_Height(height), m_Format(format)
    {
        Allocate();

        if (data)
        {
            SetPiexels(data);
        }
    }

    Image::~Image()
    {
        Release();
    }

    void Image::SetPiexels(const void* pixels)
    {
        VkResult result;
        VkDevice device = Application::GetDevice();

        uint32_t uploadSize = m_Width * m_Height * BytesPerPixel(m_Format);

        if (!m_UploadBuffer)
        {
            // Create the Upload Buffer
            {
                VkBufferCreateInfo bufferInfo{};
                bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInfo.size = uploadSize;
                bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                result = vkCreateBuffer(device, &bufferInfo, nullptr, &m_UploadBuffer);
                GF_ASSERT(result == VK_SUCCESS, "Cannot create buffer for image!");

                VkMemoryRequirements req;
                vkGetBufferMemoryRequirements(device, m_UploadBuffer, &req);

                VkMemoryAllocateInfo alloc_info = {};
                alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc_info.allocationSize = req.size;
                alloc_info.memoryTypeIndex = GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);

                result = vkAllocateMemory(device, &alloc_info, nullptr, &m_UploadBufferMemory);
                GF_ASSERT(result == VK_SUCCESS, "Failed to allocate memory for image!");

                result = vkBindBufferMemory(device, m_UploadBuffer, m_UploadBufferMemory, 0);
                GF_ASSERT(result == VK_SUCCESS, "Failed to bind buffer memory for image!");

                m_AlignedSize = (uint64_t)req.size;
            }
        }

        // Upload to Buffer
        {
            char* map = nullptr;
            result = vkMapMemory(device, m_UploadBufferMemory, 0, m_AlignedSize, 0, (void**)(&map));
            GF_ASSERT(result == VK_SUCCESS, "Failed to map memory!");

            memcpy(map, pixels, uploadSize);
            VkMappedMemoryRange range[1] = {};
            range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[0].memory = m_UploadBufferMemory;
            range[0].size = m_AlignedSize;

            result = vkFlushMappedMemoryRanges(device, 1, range);
            GF_ASSERT(result == VK_SUCCESS, "Failed to flush mapped memory ranges!");

            vkUnmapMemory(device, m_UploadBufferMemory);
        }

        // Copy to Image
        {
            VkCommandBuffer command_buffer = Application::GetCommandBuffer();

            VkImageMemoryBarrier copy_barrier[1] = {};
            copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier[0].image = m_Image;
            copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_barrier[0].subresourceRange.levelCount = 1;
            copy_barrier[0].subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

            VkBufferImageCopy region = {};
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.layerCount = 1;
            region.imageExtent.width = m_Width;
            region.imageExtent.height = m_Height;
            region.imageExtent.depth = 1;
            vkCmdCopyBufferToImage(command_buffer, m_UploadBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            VkImageMemoryBarrier useBarrier[1] = {};
            useBarrier[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            useBarrier[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            useBarrier[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            useBarrier[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            useBarrier[1].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            useBarrier[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            useBarrier[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            useBarrier[1].image = m_Image;
            useBarrier[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            useBarrier[1].subresourceRange.levelCount = 1;
            useBarrier[1].subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, useBarrier);

            Application::FlushCommandBuffer(command_buffer);
        }
    }

    void Image::Resize(uint32_t width, uint32_t height)
    {
        if (m_Width == width && m_Height == height) { return; }

        m_Width = width, m_Height = height;

        Release();
        Allocate();
    }

    // Adapted from ImGui/backends/imgui_impl_vulkan.cpp ImGui_ImplVulkan_CreateFontsTexture()
    void Image::Allocate()
    {
        VkResult result;

        VkDevice device = Application::GetDevice();
        VkFormat vkFormat = ToVulkanFormat(m_Format);

        // Create the Image:
        {
            VkImageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = vkFormat;
            info.extent.width = m_Width;
            info.extent.height = m_Height;
            info.extent.depth = 1;
            info.mipLevels = 1;
            info.arrayLayers = 1;
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            result = vkCreateImage(device, &info, nullptr, &m_Image);
            GF_ASSERT(result == VK_SUCCESS, "Failed to create image!");

            VkMemoryRequirements req;
            vkGetImageMemoryRequirements(device, m_Image, &req);

            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = req.size;
            alloc_info.memoryTypeIndex = GetMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);

            result = vkAllocateMemory(device, &alloc_info, nullptr, &m_ImageMemory);
            GF_ASSERT(result == VK_SUCCESS, "Failed to allocate image memory!");

            result = vkBindImageMemory(device, m_Image, m_ImageMemory, 0);
            GF_ASSERT(result == VK_SUCCESS, "Failed to bind image memory!");
        }

        // Create image view
        {
            VkImageViewCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = m_Image;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = vkFormat;
            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.layerCount = 1;

            result = vkCreateImageView(device, &info, nullptr, &m_ImageView);
            GF_ASSERT(result == VK_SUCCESS, "Failed to create image view!")
        }

        // Create sampler
        {
            VkSamplerCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            info.magFilter = VK_FILTER_LINEAR;
            info.minFilter = VK_FILTER_LINEAR;
            info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.minLod = -1000;
            info.maxLod = 1000;
            info.maxAnisotropy = 1.0f;

            result = vkCreateSampler(device, &info, nullptr, &m_Sampler);
            GF_ASSERT(result == VK_SUCCESS, "Failed to create sampler!");
        }
        // Create descriptor set
        m_DescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void Image::Release()
    {
        auto fn = [imageView = m_ImageView, image = m_Image, imageMemory = m_ImageMemory,
            sampler = m_Sampler, uploadBuffer = m_UploadBuffer, uploadBufferMemory = m_UploadBufferMemory]()
            {
                VkDevice device = Application::GetDevice();

                if (imageView) { vkDestroyImageView(device, imageView, nullptr); }
                if (image) { vkDestroyImage(device, image, nullptr); }
                if (imageMemory) { vkFreeMemory(device, imageMemory, nullptr); }
                if (sampler) { vkDestroySampler(device, sampler, nullptr); }
                if (uploadBuffer) { vkDestroyBuffer(device, uploadBuffer, nullptr); }
                if (uploadBufferMemory) { vkFreeMemory(device, uploadBufferMemory, nullptr); }
            };

        Application::SubmitResourceFree(fn);
    }
}