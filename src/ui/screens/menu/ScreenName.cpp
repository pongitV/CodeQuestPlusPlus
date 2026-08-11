#include "ScreenName.h"
#include "../utils/ScreenRegistry.h"

TelaNome::Resultado TelaNome::display() {
    return RegistroTelas::telaNome();
}
