#include "pch.h"
#include "SeedFillAlgorithm.h"

namespace Grafix
{
 void SeedFillAlgorithm::Fill(const glm::vec2& seedPoint, uint32_t fillColor, uint32_t oldColor)
{
     if (fillColor == oldColor)
         return;

    // 创建一个队列用于种子填充
    std::queue<glm::vec2> pixelQueue;
    // 将种子点入队并填充它
    pixelQueue.push(seedPoint);
    SetPixel((int)seedPoint.x, (int)seedPoint.y, fillColor);

    // 定义四个方向的偏移量
    int dx[] = { 1, 0, -1, 0 };
    int dy[] = { 0, 1, 0, -1 };
    while (!pixelQueue.empty()) {
        glm::vec2 current = pixelQueue.front();
        pixelQueue.pop();

        for (int i = 0; i < 4; i++) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            if (newX < 0 || newX >= GetWidth() || newY < 0 || newY >= GetHeight())
                continue;

            if (GetPixelValue(newX, newY) == oldColor) 
            {
                /*GF_INFO("Fill ({0}, {1})", newX, newY);*/
                // 将相邻的边界像素入队并进行填充
                pixelQueue.push(glm::vec2(newX, newY));
                SetPixel(newX, newY, fillColor);
            }
        }
    }
}
    
    //
}
/*void SeedFillAlgorithm::Fill(const glm::vec2& seedPoint, const glm::vec3& fillColor, const glm::vec3& oldColor, const uint32_t* seedPixels, const uint32_t width)
 {
     // 获取种子点的颜色,同时检查是否在边界内
     glm::vec3 currentColor = GetPixelColor(seedPoint.x, seedPoint.y, seedPixels,width);
     if (currentColor == (1.0f, 1.0f, 1.0f))return;


     // 如果种子点颜色不是填充颜色并且不是旧颜色，则进行填充
     if (currentColor != oldColor && currentColor != fillColor)
     {
         // 设置当前像素的颜色为填充颜色
         SetPixel(seedPoint.x, seedPoint.y, fillColor);

         // 递归填充相邻的像素
         Fill(glm::vec2(seedPoint.x + 1, seedPoint.y), fillColor, oldColor, seedPixels);
         Fill(glm::vec2(seedPoint.x - 1, seedPoint.y), fillColor, oldColor, seedPixels);
         Fill(glm::vec2(seedPoint.x, seedPoint.y + 1), fillColor, oldColor, seedPixels);
         Fill(glm::vec2(seedPoint.x, seedPoint.y - 1), fillColor, oldColor, seedPixels);
     }
 }*/