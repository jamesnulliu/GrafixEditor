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
        SetPixel((int)seedPoint.x, (int)seedPoint.y, fillColor, 1);

        // �����ĸ������ƫ����
        int dx[] = { 1, 0, -1, 0 };
        int dy[] = { 0, 1, 0, -1 };
        while (!pixelQueue.empty())
        {
            glm::vec2 current = pixelQueue.front();
            pixelQueue.pop();

            for (int i = 0; i < 4; i++)
            {
                int newX = current.x + dx[i];
                int newY = current.y + dy[i];

                if (newX < 0 || newX >= GetWidth() || newY < 0 || newY >= GetHeight())
                    continue;

                if (GetPixelValue(newX, newY) == oldColor)
                {
                    // �����ڵı߽�������Ӳ��������
                    pixelQueue.push(glm::vec2(newX, newY));
                    SetPixel(newX, newY, fillColor, 1);
                }
            }
        }
    }
}