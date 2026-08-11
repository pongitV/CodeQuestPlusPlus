#pragma once

#include <chrono>
#include <mutex>

class FramePipeline {
public:
    FramePipeline();

    void iniciarQuadro();
    void finalizarQuadro(int fpsAlvo = 60);

    float obterDeltaTime() const;
    float obterTempoAbsoluto() const;
    double obterTempoAbsolutoMS() const;
    int obterFPS() const;

    // English Aliases (Thread-Safe)
    void update() { iniciarQuadro(); }
    float getDeltaTime() const { return obterDeltaTime(); }
    int getFPS() const { return obterFPS(); }

private:
    mutable std::mutex m_pipelineMutex;
    std::chrono::steady_clock::time_point m_tempoInicio;
    std::chrono::steady_clock::time_point m_ultimoTempo;
    float m_deltaTime = 0.016f;
    float m_tempoAbsoluto = 0.0f;
    int m_fps = 0;
    int m_frameCount = 0;
    float m_acumuladorFPS = 0.0f;
};
