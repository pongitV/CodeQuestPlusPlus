#include "FramePipeline.h"
#include <thread>

FramePipeline::FramePipeline() {
    m_tempoInicio = std::chrono::steady_clock::now();
    m_ultimoTempo = m_tempoInicio;
}

void FramePipeline::iniciarQuadro() {
    auto agora = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = agora - m_ultimoTempo;
    float dt = elapsed.count();
    m_ultimoTempo = agora;

    if (dt > 0.1f) dt = 0.1f;

    std::chrono::duration<float> totalElapsed = agora - m_tempoInicio;
    float tempAbs = totalElapsed.count();

    std::lock_guard<std::mutex> lock(m_pipelineMutex);
    m_deltaTime = dt;
    m_tempoAbsoluto = tempAbs;

    m_frameCount++;
    m_acumuladorFPS += m_deltaTime;
    if (m_acumuladorFPS >= 1.0f) {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_acumuladorFPS -= 1.0f;
    }
}

void FramePipeline::finalizarQuadro(int fpsAlvo) {
    auto frameEnd = std::chrono::steady_clock::now();
    auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - m_ultimoTempo).count();
    int targetMs = 1000 / fpsAlvo;
    int sleepTime = targetMs - static_cast<int>(frameDuration);
    if (sleepTime > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}

float FramePipeline::obterDeltaTime() const {
    std::lock_guard<std::mutex> lock(m_pipelineMutex);
    return m_deltaTime;
}

float FramePipeline::obterTempoAbsoluto() const {
    std::lock_guard<std::mutex> lock(m_pipelineMutex);
    return m_tempoAbsoluto;
}

double FramePipeline::obterTempoAbsolutoMS() const {
    std::lock_guard<std::mutex> lock(m_pipelineMutex);
    return m_tempoAbsoluto * 1000.0;
}

int FramePipeline::obterFPS() const {
    std::lock_guard<std::mutex> lock(m_pipelineMutex);
    return m_fps;
}
