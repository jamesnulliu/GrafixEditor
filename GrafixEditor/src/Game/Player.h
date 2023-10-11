#pragma once

#include "Grafix.h"

class Player : Grafix::Entity
{
public:
    Player();
    ~Player();

    void Init();
    void Update();
    void Render();
private:
    glm::vec2 m_Position{ 420.0f, 360.0f };
    glm::vec2 m_Speed{ 1.0f, 0.0f };
};
