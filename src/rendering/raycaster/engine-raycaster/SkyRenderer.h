#pragma once
#include "RaycasterWorld.h"

class SkyRenderer {
public:
    static Pixel3D calcularPixelCeu(
        float anguloVisao, 
        float raioAngulo, 
        int y, 
        int horizonte, 
        float tempoAnimacao, 
        bool isMenu = false
    );
};
