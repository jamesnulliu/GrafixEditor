#include "pch.h"
#include "SeedFillAlgorithm.h"

namespace Grafix
{
 void SeedFillAlgorithm::Fill(const glm::vec2& seedPoint, uint32_t fillColor, uint32_t oldColor)
{
     if (fillColor == oldColor)
         return;

    // ����һ�����������������
    std::queue<glm::vec2> pixelQueue;
    // �����ӵ���Ӳ������
    pixelQueue.push(seedPoint);
    SetPixel((int)seedPoint.x, (int)seedPoint.y, fillColor);

    // �����ĸ������ƫ����
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
                // �����ڵı߽�������Ӳ��������
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
     // ��ȡ���ӵ����ɫ,ͬʱ����Ƿ��ڱ߽���
     glm::vec3 currentColor = GetPixelColor(seedPoint.x, seedPoint.y, seedPixels,width);
     if (currentColor == (1.0f, 1.0f, 1.0f))return;


     // ������ӵ���ɫ���������ɫ���Ҳ��Ǿ���ɫ����������
     if (currentColor != oldColor && currentColor != fillColor)
     {
         // ���õ�ǰ���ص���ɫΪ�����ɫ
         SetPixel(seedPoint.x, seedPoint.y, fillColor);

         // �ݹ�������ڵ�����
         Fill(glm::vec2(seedPoint.x + 1, seedPoint.y), fillColor, oldColor, seedPixels);
         Fill(glm::vec2(seedPoint.x - 1, seedPoint.y), fillColor, oldColor, seedPixels);
         Fill(glm::vec2(seedPoint.x, seedPoint.y + 1), fillColor, oldColor, seedPixels);
         Fill(glm::vec2(seedPoint.x, seedPoint.y - 1), fillColor, oldColor, seedPixels);
     }
 }*/