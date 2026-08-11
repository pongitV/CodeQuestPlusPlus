#include "Screen3DScene.h"
void TelaCena3D::executar() {
    auto obterFundoOpacity = [this](float opacity) {
        return this->comporFundoComElementos(opacity);
    };


}

void TelaCena3D::sobreporPainel(std::vector<std::string>& fundo, const std::vector<std::string>& arte, int startY, int startX) const {
    for (size_t i = 0; i < arte.size(); ++i) {
        int y = startY + i;
        if (y >= 0 && y < static_cast<int>(fundo.size())) {
            fundo[y] = "";
        }
    }
}
