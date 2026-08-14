#pragma once

#include "../../character/Character.h"
#include <string>
#include <vector>
#include "../NPCInteraction.h"
#include "../../../core/utils/Color.h"

class NPCAlchemist : public NPCInteraction
{
public:
    void interact(Character* player);
    void interagir(Character* player) { interact(player); }

protected:
    std::string getPlaceName() const override;
    Color getHeaderColor() const override;
    Color getArtColor() const override;
    const std::vector<std::string>& getASCIIArt() const override;

    std::vector<std::string> getDialogue(Character* player) override;
    std::vector<std::string> getMenuOptions(Character* player, int terminalWidth) override;
    void processOption(Character* player, const std::string& option, int terminalWidth) override;
};

using NPCAlquimista = NPCAlchemist;
