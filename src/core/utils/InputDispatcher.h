#pragma once

#include <windows.h>
#include <functional>
#include <unordered_map>
#include <vector>

class InputDispatcher {
public:
    using Action = std::function<void()>;
    using Acao = Action;

    void registerAction(int key, Action action) {
        if (action) actions[key] = std::move(action);
    }
    void registrar(int tecla, Acao acao) { registerAction(tecla, std::move(acao)); }

    bool execute(int key) const {
        auto it = actions.find(key);
        if (it != actions.end()) {
            it->second();
            return true;
        }
        return false;
    }
    bool executar(int tecla) const { return execute(tecla); }

    // Consulta todas as teclas registradas via GetAsyncKeyState; executa a primeira correspondência encontrada.
    using ActionWithReturn = std::function<char()>;
    using AcaoComRetorno = ActionWithReturn;
    struct PollEntry {
        int key;
        ActionWithReturn action; // retorna '\0' para continuar, qualquer outro valor para retornar
    };

    void registerPoll(int key, ActionWithReturn action) {
        pollActions.push_back({key, std::move(action)});
    }
    void registrarPoll(int tecla, AcaoComRetorno acao) { registerPoll(tecla, std::move(acao)); }

    char poll() const {
        for (const auto& entry : pollActions) {
            if (GetAsyncKeyState(entry.key) & 0x8000) {
                char r = entry.action();
                if (r != '\0') return r;
            }
        }
        return '\0';
    }

    void clear() {
        actions.clear();
        pollActions.clear();
    }
    void limpar() { clear(); }

private:
    std::unordered_map<int, Action> actions;
    std::vector<PollEntry> pollActions;
};
