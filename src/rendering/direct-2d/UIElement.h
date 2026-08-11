#pragma once

#include "D2DRenderer.h"
#include <string>
#include <vector>
#include <memory>

class UIElement {
public:
    virtual ~UIElement() = default;
    virtual void render(D2DRenderer* d2d) = 0;
    virtual void update(float deltaTime) { (void)deltaTime; }
    virtual bool isVisible() const { return m_visible; }
    virtual void setVisible(bool visible) { m_visible = visible; }

protected:
    bool m_visible = true;
};

class UITextElement : public UIElement {
public:
    UITextElement(const std::wstring& text, float x, float y, D2D1_COLOR_F color, float fontSize = 14.0f, bool centered = false)
        : m_text(text), m_x(x), m_y(y), m_color(color), m_fontSize(fontSize), m_centered(centered) {}

    void render(D2DRenderer* d2d) override {
        if (!m_visible || !d2d) return;
        if (m_centered) {
            d2d->desenharTextoCentralizado(m_text, m_x, m_y, 400.0f, 40.0f, m_color, m_fontSize);
        } else {
            d2d->desenharTexto(m_text, m_x, m_y, m_color, m_fontSize);
        }
    }

    void setText(const std::wstring& newText) { m_text = newText; }
    const std::wstring& getText() const { return m_text; }

private:
    std::wstring m_text;
    float m_x, m_y;
    D2D1_COLOR_F m_color;
    float m_fontSize;
    bool m_centered;
};

class UIRectElement : public UIElement {
public:
    UIRectElement(float x, float y, float w, float h, D2D1_COLOR_F color, bool fill = true)
        : m_x(x), m_y(y), m_w(w), m_h(h), m_color(color), m_fill(fill) {}

    void render(D2DRenderer* d2d) override {
        if (!m_visible || !d2d) return;
        if (m_fill) {
            d2d->preencherRetangulo(m_x, m_y, m_w, m_h, m_color);
        } else {
            d2d->desenharRetangulo(m_x, m_y, m_w, m_h, m_color);
        }
    }

private:
    float m_x, m_y, m_w, m_h;
    D2D1_COLOR_F m_color;
    bool m_fill;
};
