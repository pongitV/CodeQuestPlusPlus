#pragma once

namespace RenderingConfig {
    // Canvas / Backbuffer Resolution (Raycaster 3D internal & low-res buffer)
    constexpr int BACKBUFFER_WIDTH = 640;
    constexpr int BACKBUFFER_HEIGHT = 360;

    // Logical UI Canvas Space (High resolution coordinate space for Direct2D UI)
    constexpr float LOGICAL_WIDTH = 1920.0f;
    constexpr float LOGICAL_HEIGHT = 1080.0f;

    // Raycaster Camera & Performance Config
    constexpr float DEFAULT_FOV = 3.1415926535f / 4.0f; // 45 Degrees FOV
    constexpr float DEFAULT_MAX_DEPTH = 16.0f;

    // Texture Cache Limit
    constexpr size_t MAX_TEXTURE_CACHE_SIZE = 128;
}
