#pragma once

#include <algorithm>

class ScreenTransition {
public:
    enum class Type { Fade, Slide, Wipe };

private:
    Type type = Type::Fade;
    float progress = 0.0f;
    float speed = 2.0f; // 0.5s transition

public:
    ScreenTransition(Type t = Type::Fade, float s = 2.0f) : type(t), speed(s), progress(0.0f) {}
    ~ScreenTransition() = default;

    void update(float deltaTime) {
        progress += deltaTime * speed;
        if (progress >= 1.0f) progress = 1.0f;
    }

    void reset() {
        progress = 0.0f;
    }

    bool isComplete() const {
        return progress >= 1.0f;
    }

    float getProgress() const {
        return progress;
    }

    float getOpacity() const {
        return progress;
    }

    Type getType() const {
        return type;
    }
};
