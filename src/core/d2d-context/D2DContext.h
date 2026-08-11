#pragma once

class D2DRenderer;
class GridEmulator2D;
class FramePipeline;
class GameWindow;

struct D2DContext {
    static D2DRenderer* renderer;
    static GridEmulator2D* grid;
    static FramePipeline* pipeline;
    static GameWindow* window;
};
