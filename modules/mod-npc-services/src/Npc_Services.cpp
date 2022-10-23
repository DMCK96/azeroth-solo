/*
** Written by MtgCore
** Rewritten by Poszer & Talamortis https://github.com/poszer/ & https://github.com/talamortis/
** AzerothCore 2019 http://www.azerothcore.org/
** Cleaned and made into a module by Micrah https://github.com/milestorme/
*/

#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

class Npc_Services : public CreatureScript
{
public:
    Npc_Services() : CreatureScript("Npc_Services") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        AddGossipItemFor(player, 10, "|TInterface\\icons\\Spell_Nature_Regenerate:40:40:-18|t Restore HP and MP", GOSSIP_SENDER_MAIN, 1);			// Restore Health and Mana
        //AddGossipItemFor(player, 10, "|TInterface\\icons\\Achievement_BG_winAB_underXminutes:40:40:-18|t Reset Instances", GOSSIP_SENDER_MAIN, 2);	// Reset Instances
        AddGossipItemFor(player, 10, "|TInterface\\icons\\SPELL_HOLY_BORROWEDTIME:40:40:-18|t Reset Cooldowns", GOSSIP_SENDER_MAIN, 3);				// Reset Cooldowns
        AddGossipItemFor(player, 10, "|TInterface\\icons\\Achievement_BG_AB_defendflags:40:40:-18|t Reset Combat", GOSSIP_SENDER_MAIN, 4);			// Leave Combat
        AddGossipItemFor(player, 10, "|TInterface\\icons\\Spell_Shadow_DeathScream:40:40:-18|t Remove Sickness", GOSSIP_SENDER_MAIN, 5);				// Remove Sickness
        AddGossipItemFor(player, 10, "|TInterface\\icons\\INV_Hammer_24:40:40:-18|t Repair Items", GOSSIP_SENDER_MAIN, 6);							// Repair Items
        AddGossipItemFor(player, 10, "|TInterface\\icons\\Achievement_WorldEvent_Lunar:40:40:-18|t Reset Talents", GOSSIP_SENDER_MAIN, 7);			// Reset Talents
        AddGossipItemFor(player, 10, "|TInterface/Icons/INV_Misc_Bag_07:40:40:-18|t Bank", GOSSIP_SENDER_MAIN, 8);                                   // Open Bank
        AddGossipItemFor(player, 10, "|TInterface/Icons/INV_Letter_11:40:40:-18|t Mail", GOSSIP_SENDER_MAIN, 9);                                     // Open Mailbox
        AddGossipItemFor(player, 10, "|TInterface/Icons/achievement_general:40:40:-18|t Learn Dual Talents", GOSSIP_SENDER_MAIN, 10);                                     // Learn Dualspec
        AddGossipItemFor(player, 10, "|TInterface\\icons\\SPELL_HOLY_POWERINFUSION:40:40:-18|t Prestige - Reset your character from level 80 to level 1 and start the leveling process again. Doing this will grant you 1 Prestige Token.", GOSSIP_SENDER_MAIN, 11);				// Reset Character

        SendGossipMenuFor(player, 1, creature->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
        case 1: // Restore HP and MP
            CloseGossipMenuFor(player);
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                player->GetSession()->SendNotification("You are in combat!");
                return false;
            }
            else if (player->getPowerType() == POWER_MANA)
                player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));

            player->SetHealth(player->GetMaxHealth());
            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFHP & MP succesfully restored!");
            player->CastSpell(player, 31726);
            break;

        case 2: // Reset Instances
            CloseGossipMenuFor(player);
            for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
            {
                BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUID(), Difficulty(i));
                for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end();)
                {
                    InstanceSave* save = itr->second.save;
                    if (itr->first != player->GetMapId())
                    {
                        uint32 resetTime = itr->second.extended ? save->GetExtendedResetTime() : save->GetResetTime();
                        uint32 ttr = (resetTime >= time(nullptr) ? resetTime - time(nullptr) : 0);
                        sInstanceSaveMgr->PlayerUnbindInstance(player->GetGUID(), itr->first, Difficulty(i), true, player);
                        itr = m_boundInstances.begin();
                    }
                    else
                        ++itr;
                }
            }

            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFInstances succesfully reseted!");
            player->CastSpell(player, 59908);
            return true;
            break;

        case 3: // Reset Cooldowns
            CloseGossipMenuFor(player);
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                player->GetSession()->SendNotification("You are in combat!");
                return false;
            }

            player->RemoveAllSpellCooldown();
            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFCooldowns succesfully reseted!");
            player->CastSpell(player, 31726);
            break;

        case 4: // Leave Combat
            CloseGossipMenuFor(player);
            player->CombatStop();
            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFCombat succesfully removed!");
            player->CastSpell(player, 31726);
            break;

        case 5: // Remove Sickness
            CloseGossipMenuFor(player);
            if (player->HasAura(15007))
                player->RemoveAura(15007);
            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFSickness succesfully removed!");
            player->CastSpell(player, 31726);
            break;

        case 6: // Repair Items
            CloseGossipMenuFor(player);
            player->DurabilityRepairAll(false, 0, false);
            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFItems repaired succesfully!");
            player->CastSpell(player, 31726);
            break;

        case 7: // Reset Talents
            CloseGossipMenuFor(player);
            player->resetTalents(true);
            player->SendTalentsInfoData(false);
            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFTalents reseted succesfully!");
            player->CastSpell(player, 31726);
            break;

        case 8:	// BANK
            CloseGossipMenuFor(player);
            player->GetSession()->SendShowBank(player->GetGUID());
            break;

        case 9: // MAIL
            CloseGossipMenuFor(player);
            player->GetSession()->SendShowMailBox(player->GetGUID());
            break;

        case 10: // Learn Dual Talent Specialization
            CloseGossipMenuFor(player);
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                player->GetSession()->SendNotification("You are in combat!");
                return false;
            }

            player->learnSpell(63644);
            player->CastSpell(player, 31726);
            player->CastSpell(player, 63624);
            player->learnSpell(63645);
            player->UpdateSpecCount(2);
            player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFDual Talents Learned Succesfully!");
            return true;
            break;
        case 11: // Reset to level 1
            if (player->getLevel() == 80)
            {
                CloseGossipMenuFor(player);

                // Reset Character
                player->SetLevel(1);
                player->resetTalents(true);
                player->resetSpells();
                player->SendTalentsInfoData(false);
                // How to unequip character?

                // base stats and related field values
                player->InitStatsForLevel();
                player->InitGlyphsForLevel();
                player->InitTalentForLevel();

                // apply original stats mods before spell loading or item equipment that call before equip _RemoveStatsMods()
                player->UpdateMaxHealth();                                      // Update max Health (for add bonus from stamina)
                player->SetFullHealth();
                if (player->getPowerType() == POWER_MANA)
                {
                    player->UpdateMaxPower(POWER_MANA);                         // Update max Mana (for add bonus from intellect)
                    player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
                }

                if (player->getPowerType() == POWER_RUNIC_POWER)
                {
                    player->SetPower(POWER_RUNE, 8);
                    player->SetMaxPower(POWER_RUNE, 8);
                    player->SetPower(POWER_RUNIC_POWER, 0);
                    player->SetMaxPower(POWER_RUNIC_POWER, 1000);
                }

                // original spells
                player->LearnDefaultSkills();
                player->LearnCustomSpells();

                // Add prestige token
                player->AddItem(701004, 1);

                player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFCharacter level reset succesfully!");
                player->CastSpell(player, 31726);
                break;
            } else
            {
                player->GetSession()->SendNotification("|cffFFFF00NPC SERVICES \n |cffFFFFFFYou mmust be level 80 before you can reset!");
                player->CastSpell(player, 31726);
                break;
            }
            
        }
        return true;
    }
};

void AddSC_Npc_Services()
{
    new Npc_Services();
}
