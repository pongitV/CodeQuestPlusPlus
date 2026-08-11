#pragma once

namespace MenuRaycasterUtils {

    inline float s_cicloCelestial = 0.0f;
    inline float s_velocidadeCelestial = 0.005f;
    inline int s_estrelasX = 0;
    inline int s_guerreiroPassos = 0;

    inline int s_estadoCena = 0;
    inline int s_combatTick = 0;
    inline int s_guerreiroX = -10;
    inline int s_magoX = -25;
    inline int s_arqueiroX = -40;
    inline int s_goblin1X = 0;
    inline int s_goblin2X = 0;
    inline int s_orkX = 0;
    inline bool s_goblin1Vivo = false;
    inline bool s_goblin2Vivo = false;
    inline bool s_orkVivo = false;
    inline int s_goblin1HitTimer = 0;
    inline int s_goblin2HitTimer = 0;
    inline int s_orkHitTimer = 0;
    inline int s_warriorOffset = 0;
    inline int s_archerProjX = 0;
    inline int s_laserTimer = 0;
    inline int s_healTimer = 0;

    inline void incrementarCicloDia() {
        s_cicloCelestial += s_velocidadeCelestial;
        if (s_cicloCelestial >= 4.6f) s_cicloCelestial -= 4.6f;
        s_estrelasX = (s_estrelasX + 1) % 2000;
        
        int largura = 120;
        if (largura <= 0) largura = 120;
        int centro = largura / 2;
        
        if (s_estadoCena == 0) {
            if (s_combatTick == 0) {
                s_guerreiroX = -40; s_magoX = -55; s_arqueiroX = -70;
                s_goblin1X = largura + 40; s_goblin2X = largura + 55; s_orkX = largura + 70;
                s_goblin1Vivo = true; s_goblin2Vivo = true; s_orkVivo = true;
                s_warriorOffset = 0; s_archerProjX = 0; s_laserTimer = 0; s_healTimer = 0;
            }
            s_combatTick++;
            s_guerreiroX += 2; s_magoX += 2; s_arqueiroX += 2;
            s_goblin1X -= 2; s_goblin2X -= 2; s_orkX -= 2;
            
            if (s_guerreiroX >= centro - 5) {
                s_guerreiroX = centro - 5; s_magoX = centro - 20; s_arqueiroX = centro - 35;
                s_goblin1X = centro + 5; s_goblin2X = centro + 20; s_orkX = centro + 35;
                s_estadoCena = 1; 
                s_combatTick = 0;
            }
        } 
        else if (s_estadoCena == 1) {
            s_combatTick++;
            int t = s_combatTick;
            
            if (t < 30) { 
                if (t == 5) { s_warriorOffset = s_goblin1X - s_guerreiroX - 10; s_goblin1HitTimer = 5; } 
                if (t == 10) { s_warriorOffset = 0; }
                if (t == 20) { s_warriorOffset = s_goblin1X - s_guerreiroX - 10; s_goblin1HitTimer = 5; } 
                if (t == 25) { s_warriorOffset = 0; s_goblin1Vivo = false; } 
            }
            else if (t < 60) { 
                if (t == 35) { s_archerProjX = s_arqueiroX + 10; } 
                if (t > 35 && t < 40) s_archerProjX = s_arqueiroX + 10 + (s_goblin2X - s_arqueiroX - 10) * (t - 35) / 5;
                if (t == 40) { s_archerProjX = 0; s_goblin2HitTimer = 5; } 
                
                if (t == 50) { s_archerProjX = s_arqueiroX + 10; } 
                if (t > 50 && t < 55) s_archerProjX = s_arqueiroX + 10 + (s_goblin2X - s_arqueiroX - 10) * (t - 50) / 5;
                if (t == 55) { s_archerProjX = 0; s_goblin2HitTimer = 5; s_goblin2Vivo = false; } 
            }
            else if (t < 90) { 
                if (t == 65) s_healTimer = 20;
            }
            else if (t < 120) { 
                if (t == 95) { s_warriorOffset = s_orkX - s_guerreiroX - 10; s_orkHitTimer = 5; } 
                if (t == 100) { s_warriorOffset = 0; }
                if (t == 105) { s_archerProjX = s_arqueiroX + 10; }
                if (t > 105 && t < 110) s_archerProjX = s_arqueiroX + 10 + (s_orkX - s_arqueiroX - 10) * (t - 105) / 5;
                if (t == 110) { s_archerProjX = 0; s_orkHitTimer = 5; } 
            }
            else if (t < 150) { 
                if (t == 125) { s_laserTimer = 15; s_orkHitTimer = 15; } 
                if (t == 140) { s_orkVivo = false; } 
            }
            else {
                s_estadoCena = 2; 
            }
        }
        else if (s_estadoCena == 2) {
            s_guerreiroX += 2; s_magoX += 2; s_arqueiroX += 2;
            if (s_arqueiroX > largura + 10) {
                s_estadoCena = 0; 
                s_combatTick = 0;
            }
        }

        if (s_goblin1HitTimer > 0) s_goblin1HitTimer--;
        if (s_goblin2HitTimer > 0) s_goblin2HitTimer--;
        if (s_orkHitTimer > 0) s_orkHitTimer--;
        if (s_laserTimer > 0) s_laserTimer--;
        if (s_healTimer > 0) s_healTimer--;

        s_guerreiroPassos++;
    }

}
