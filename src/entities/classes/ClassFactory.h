#pragma once

#include <memory>
#include <vector>
#include "ClassBase.h"

class ClassFactory {
public:
    static std::unique_ptr<ClassBase> createClass(ClassType type);
    static std::vector<ClassType> getPlayableClasses();

    // Compatibilidade legada
    static std::vector<ClassType> obterClassesJogaveis() { return getPlayableClasses(); }
};

using FabricaClasses = ClassFactory;
