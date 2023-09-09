#pragma once

#include <vulkan/vulkan.h>

namespace Grafix
{
    enum class ImageFormat : uint8_t
    {
        None = 0,
        RGBA,
    };

    class Image
    {
    public:
        Image(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
        ~Image();

        void SetPiexels(const void* pixels);
        void Resize(uint32_t width, uint32_t height);

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
    private:
        void Allocate();
        void Release();

        uint32_t BytesPerPixel(ImageFormat format) const;
        VkFormat ToVulkanFormat(ImageFormat format) const;
        uint32_t GetMemoryType(VkMemoryPropertyFlags properties, uint32_t typeBits) const;
    private:
        uint32_t m_Width, m_Height;
        ImageFormat m_Format = ImageFormat::None;

        // Vulkan
        VkImage m_Image = nullptr;
        VkImageView m_ImageView = nullptr;
        VkDeviceMemory m_ImageMemory = nullptr;

        VkSampler m_Sampler = nullptr;

        uint64_t m_AlignedSize = 0;

        VkBuffer m_UploadBuffer = nullptr;
        VkDeviceMemory m_UploadBufferMemory = nullptr;

        VkDescriptorSet m_DescriptorSet = nullptr;
    };
}
