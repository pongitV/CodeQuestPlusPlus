#pragma once

#include "interfaces/IRendererPopup.h"
#include "interfaces/IRendererScreen.h"

class UIRenderer : public IRenderizadorPopup, public IRenderizadorTela {
public:
    ~UIRenderer() override = default;
};
