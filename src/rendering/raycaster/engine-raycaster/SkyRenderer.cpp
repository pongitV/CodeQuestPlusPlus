#include "SkyRenderer.h"
#include "RaycasterWorld.h"
#include <cmath>
#include <algorithm>

Pixel3D SkyRenderer::calcularPixelCeu(
    float anguloVisao, 
    float raioAngulo, 
    int y, 
    int horizonte, 
    float tempoAnimacao, 
    bool isMenu
) {
    float ratio = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    struct SkyColor { float topR, topG, topB, botR, botG, botB; };
    auto mixSkyColor = [](SkyColor a, SkyColor b, float t) -> SkyColor {
        return { a.topR + (b.topR - a.topR) * t, a.topG + (b.topG - a.topG) * t, a.topB + (b.topB - a.topB) * t,
                 a.botR + (b.botR - a.botR) * t, a.botG + (b.botG - a.botG) * t, a.botB + (b.botB - a.botB) * t };
    };

    SkyColor nascer = { 20, 20, 60,   255, 120, 50 };
    SkyColor dia =    { 10, 60, 150,  70, 150, 230 };
    SkyColor porSol = { 40, 10, 60,   255, 80, 20 };
    SkyColor noite =  { 2, 2, 5,      5, 25, 45 };

    float t = std::fmod(tempoAnimacao, 120.0f) / 120.0f;
    SkyColor corAtual;
    if (t < 0.05f)      corAtual = mixSkyColor(nascer, dia, t / 0.05f);
    else if (t < 0.45f) corAtual = dia;
    else if (t < 0.5f)  corAtual = mixSkyColor(dia, porSol, (t - 0.45f) / 0.05f);
    else if (t < 0.55f) corAtual = mixSkyColor(porSol, noite, (t - 0.5f) / 0.05f);
    else if (t < 0.95f) corAtual = noite;
    else                corAtual = mixSkyColor(noite, nascer, (t - 0.95f) / 0.05f);

    int r = (int)(corAtual.topR * (1.0f - ratio) + corAtual.botR * ratio);
    int g = (int)(corAtual.topG * (1.0f - ratio) + corAtual.botG * ratio);
    int b = (int)(corAtual.topB * (1.0f - ratio) + corAtual.botB * ratio);

    float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
    float ratioY = (float)y / divHorizonte;
    float globalRotation = tempoAnimacao * 0.05f;

    float diffAnguloSol;
    float distYSol;
    
    if (RaycasterWorld::s_overrideCelestials) {
        diffAnguloSol = raioAngulo - RaycasterWorld::s_overrideSunAng;
        distYSol = ratioY - RaycasterWorld::s_overrideSunRatioY;
        if (diffAnguloSol < -3.14159f) diffAnguloSol += 2.0f * 3.14159f;
        if (diffAnguloSol > 3.14159f) diffAnguloSol -= 2.0f * 3.14159f;
    } else {
        if (isMenu) {
            float colAng = raioAngulo;
            diffAnguloSol = colAng - (t - 0.25f) * 3.2f;
        } else {
            diffAnguloSol = std::fmod(raioAngulo - globalRotation, 2.0f * 3.14159f);
            if (diffAnguloSol < -3.14159f) diffAnguloSol += 2.0f * 3.14159f;
            if (diffAnguloSol > 3.14159f) diffAnguloSol -= 2.0f * 3.14159f;
        }
        float sunPhase = (t - 0.25f) * 2.0f * 3.14159f;
        float sunElevation = std::cos(sunPhase); 
        distYSol = isMenu ? (ratioY - (0.5f - 0.2f * sunElevation)) : (ratioY - (0.9f - 1.1f * sunElevation));
    }
    
    float distSol = std::sqrt(diffAnguloSol * diffAnguloSol * 6.0f + distYSol * distYSol);
    
    float diffAnguloLua;
    float distYLua;

    if (RaycasterWorld::s_overrideCelestials) {
        diffAnguloLua = raioAngulo - RaycasterWorld::s_overrideMoonAng;
        distYLua = ratioY - RaycasterWorld::s_overrideMoonRatioY;
        if (diffAnguloLua < -3.14159f) diffAnguloLua += 2.0f * 3.14159f;
        if (diffAnguloLua > 3.14159f) diffAnguloLua -= 2.0f * 3.14159f;
    } else {
        if (isMenu) {
            float colAng = raioAngulo;
            diffAnguloLua = colAng - (t - 0.75f) * 3.2f;
        } else {
            diffAnguloLua = std::fmod(raioAngulo - globalRotation + 3.14159f, 2.0f * 3.14159f);
            if (diffAnguloLua < -3.14159f) diffAnguloLua += 2.0f * 3.14159f;
            if (diffAnguloLua > 3.14159f) diffAnguloLua -= 2.0f * 3.14159f;
        }
        float moonPhase = (t - 0.75f) * 2.0f * 3.14159f;
        float moonElevation = std::cos(moonPhase);
        distYLua = isMenu ? (ratioY - (0.5f - 0.2f * moonElevation)) : (ratioY - (0.9f - 1.1f * moonElevation));
    }
    float distLua = std::sqrt(diffAnguloLua * diffAnguloLua * 6.0f + distYLua * distYLua);

    Pixel3D px;
    px.ch = ' '; px.hasFg = false; px.isFundo = false;

    // Draw Moon
    float moonAlpha = 1.0f;
    float moonGlowRadius = 0.11f;

    if (distLua < 0.10f) {
        float shadowOffset = 0.04f + std::sin(tempoAnimacao * 0.5f) * 0.01f; 
        float shadowDist = std::sqrt((diffAnguloLua - shadowOffset) * (diffAnguloLua - shadowOffset) * 6.0f + distYLua * distYLua);
        if (shadowDist < 0.10f) {
            r = 25; g = 30; b = 40;
        } else {
            float lunarX = diffAnguloLua * 20.0f;
            float lunarY = distYLua * 20.0f;
            float rotation = tempoAnimacao * 0.15f; 
            float mariaNoise = std::sin(lunarX * 3.0f + lunarY + rotation) * std::cos(lunarY * 4.0f - lunarX - rotation) 
                             + std::sin(lunarX * 7.0f - rotation);
            if (mariaNoise > 0.4f) {
                r = 140; g = 150; b = 180;
            } else {
                r = 240; g = 245; b = 255;
            }
        }
    } else if (distLua < moonGlowRadius) {
        float coronaLerp = (distLua - 0.10f) / (moonGlowRadius - 0.10f);
        r = 240 - (int)(40 * coronaLerp); 
        g = 245 - (int)(35 * coronaLerp); 
        b = 255 - (int)(15 * coronaLerp); 
    } else if (distLua < 0.45f) {
        float glowPulse = std::sin(tempoAnimacao * 1.5f) * 0.02f;
        float glowDist = (distLua - moonGlowRadius) / (0.45f - moonGlowRadius + glowPulse);
        float glow = std::max(0.0f, 1.0f - glowDist);
        glow = glow * glow; 
        
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(120 * glow * moonAlpha));
            g = std::min(255, g + (int)(150 * glow * moonAlpha));
            b = std::min(255, b + (int)(220 * glow * moonAlpha));
        }
    }

    // Draw Sun
    float sunAlpha = 1.0f;
    float angleSol = std::atan2(distYSol, diffAnguloSol * 2.449f);
    float rays = std::sin(angleSol * 12.0f + tempoAnimacao * 1.5f) * 0.5f 
               + std::sin(angleSol * 7.0f - tempoAnimacao * 0.8f) * 0.5f;
    float glowRadius = 0.12f + rays * 0.025f;
    
    if (distSol < 0.08f) {
        r = 255; g = 255; b = 255;
    } else if (distSol < glowRadius) {
        float coronaLerp = (distSol - 0.08f) / (glowRadius - 0.08f);
        r = 255; 
        g = 255 - (int)(35 * coronaLerp); 
        b = 255 - (int)(205 * coronaLerp); 
    } else if (distSol < 0.45f) {
        float glowPulse = std::sin(tempoAnimacao * 2.0f) * 0.03f;
        float glowDist = (distSol - glowRadius) / (0.45f - glowRadius + glowPulse);
        float glow = std::max(0.0f, 1.0f - glowDist);
        glow = glow * glow; 
        
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(180 * glow * sunAlpha));
            g = std::min(255, g + (int)(110 * glow * sunAlpha));
            b = std::min(255, b + (int)(20 * glow * sunAlpha));
        }
    }

    // True Value Noise for Clouds
    auto hash2D = [](int x, int y) {
        unsigned int h = x * 374761393U + y * 668265263U;
        h = (h ^ (h >> 13)) * 1274126177U;
        return (float)(h % 1000) / 1000.0f;
    };
    auto noise2D = [&](float x, float y) {
        int ix = (int)std::floor(x);
        int iy = (int)std::floor(y);
        float fx = x - ix;
        float fy = y - iy;
        float u = fx * fx * (3.0f - 2.0f * fx);
        float v = fy * fy * (3.0f - 2.0f * fy);
        float a = hash2D(ix, iy);
        float b = hash2D(ix + 1, iy);
        float c = hash2D(ix, iy + 1);
        float d = hash2D(ix + 1, iy + 1);
        return a + u*(b-a) + v*(c-a) + u*v*(a-b-c+d);
    };

    // Clouds
    float wind = tempoAnimacao * 0.08f; 
    float cx = raioAngulo * 3.0f + wind;
    float cy = (float)y * 0.04f;
    
    // 3-octave fBM
    float cloudNoise = noise2D(cx, cy) 
                     + 0.5f * noise2D(cx * 2.0f - wind * 0.5f, cy * 2.0f)
                     + 0.25f * noise2D(cx * 4.0f + wind, cy * 4.0f);
    cloudNoise = cloudNoise / 1.75f; 
    
    if (cloudNoise > 0.48f) {
        float cloudIntensity = (cloudNoise - 0.48f) * 3.0f;
        cloudIntensity = std::min(1.0f, cloudIntensity);
        
        int cr = 255, cg = 255, cb = 255;
        if (t > 0.4f && t < 0.6f) { // Sunset color
            cr = 255; cg = 180; cb = 140;
        } else if (t > 0.9f || t < 0.1f) { // Sunrise color
            cr = 255; cg = 210; cb = 180;
        }
        
        cloudIntensity *= (0.2f + (ratio * 0.8f));
        r = r + (int)((cr - r) * cloudIntensity);
        g = g + (int)((cg - g) * cloudIntensity);
        b = b + (int)((cb - b) * cloudIntensity);
    }

    // Stars (only at night)
    if (t > 0.5f && t < 0.95f) {
        float starAlpha = (t > 0.6f && t < 0.85f) ? 1.0f : 0.5f;
        
        int gridSize = 6;
        int starX = (int)(raioAngulo * 200.0f);
        int gX = starX / gridSize;
        int gY = y / gridSize;
        
        unsigned int hash = gX * 374761393U + gY * 668265263U;
        hash = (hash ^ (hash >> 13)) * 1274126177U;
        int noise = hash % 150; 
        
        if (noise < 4 && y <= horizonte) { 
            int cx = gX * gridSize + (hash % gridSize);
            int cy = gY * gridSize + ((hash >> 3) % gridSize);
            
            int dx = std::abs(starX - cx);
            int dy = std::abs(y - cy);
            
            bool isCenter = (dx == 0 && dy == 0);
            bool isCross = (dx + dy == 1);
            
            if (isCenter || isCross) {
                float intensity = isCenter ? 1.0f : 0.4f;
                int sr = 255, sg = 255, sb = 255;
                if (noise == 1) { sr = 200; sg = 220; sb = 255; }
                if (noise == 2) { sr = 255; sg = 230; sb = 200; }
                
                px.fgR = (uint8_t)(sr * starAlpha * intensity);
                px.fgG = (uint8_t)(sg * starAlpha * intensity);
                px.fgB = (uint8_t)(sb * starAlpha * intensity);
                
                px.r = (uint8_t)std::min(255, r + px.fgR); 
                px.g = (uint8_t)std::min(255, g + px.fgG); 
                px.b = (uint8_t)std::min(255, b + px.fgB);
                return px;
            }
        }
    }

    px.r = r; px.g = g; px.b = b;
    return px;
}
