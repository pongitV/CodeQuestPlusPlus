#pragma once

#include <memory>
#include <vector>
#include "ClassBase.h"

class ClassFactory {
public:
    static std::unique_ptr<ClassBase> createClass(ClassType tipo);
    static std::vector<ClassType> obterClassesJogaveis();

    // English Alias
    static std::vector<ClassType> getPlayableClasses() { return obterClassesJogaveis(); }
};
