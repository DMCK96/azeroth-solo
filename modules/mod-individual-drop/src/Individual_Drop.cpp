#include "ScriptMgr.h"
#include "Configuration/Config.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "Player.h"
#include "Object.h"
#include "DataMap.h"

#if AC_COMPILER == AC_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using namespace Acore::ChatCommands;

/*
Coded by Talamortis - For Azerothcore
Thanks to Rochet for the assistance
*/

bool IndividualDropEnabled;
bool IndividualDropAnnounceModule;
uint32 MaxDropRate;
uint32 DefaultDropRate;

class Individual_Drop_conf : public WorldScript
{
public:
    Individual_Drop_conf() : WorldScript("Individual_drop_conf_conf") { }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        IndividualDropAnnounceModule = sConfigMgr->GetOption<bool>("IndividualDrop.Announce", 1);
        IndividualDropEnabled = sConfigMgr->GetOption<bool>("IndividualDrop.Enabled", 1);
        MaxDropRate = sConfigMgr->GetOption<int32>("MaxDropRate", 10);
        DefaultDropRate = sConfigMgr->GetOption<int32>("DefaultDropRate", 1);
    }
};

class Individual_Drop_Announce : public PlayerScript
{
public:

    Individual_Drop_Announce() : PlayerScript("Individual_Drop_Announce") {}

    void OnLogin(Player* player)
    {
        // Announce Module
        if (IndividualDropEnabled && IndividualDropAnnounceModule)
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00IndividualDropRate |rmodule");
        }
    }
};

class PlayerDropRate : public DataMap::Base
{
public:
    PlayerDropRate() {}
    PlayerDropRate(uint32 DropRate) : DropRate(DropRate) {}
    uint32 DropRate = 1;
};

class Individual_Drop : public PlayerScript
{
public:
    Individual_Drop() : PlayerScript("Individual_Drop") { }

    void OnLogin(Player* p) override
    {
        QueryResult result = CharacterDatabase.Query("SELECT `dropRate` FROM `individualdrop` WHERE `CharacterGUID` = '{}'", p->GetGUID().GetCounter());
        if (!result)
        {
            p->CustomData.GetDefault<PlayerDropRate>("Individual_Drop")->DropRate = DefaultDropRate;
        }
        else
        {
            Field* fields = result->Fetch();
            p->CustomData.Set("Individual_Drop", new PlayerDropRate(fields[0].Get<uint32>()));
        }
    }

    void OnLogout(Player* p) override
    {
        if (PlayerDropRate* data = p->CustomData.Get<PlayerDropRate>("Individual_Drop"))
        {
            uint32 rate = data->DropRate;
            CharacterDatabase.DirectExecute("REPLACE INTO `individualdrop` (`CharacterGUID`, `dropRate`) VALUES ('{}', '{}');", p->GetGUID().GetCounter(), rate);
        }
    }
};

class Individual_Drop_command : public CommandScript
{
public:
    Individual_Drop_command() : CommandScript("Individual_Drop_command") { }
    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> IndividualDropCommandTable =
        {
            { "view",       SEC_PLAYER, false, &HandleViewCommand,      "" },
            { "set",        SEC_PLAYER, false, &HandleSetCommand,       "" },
            { "default",    SEC_PLAYER, false, &HandleDefaultCommand,   "" },
            { "",           SEC_PLAYER, false, &HandleDropCommand,        "" }
        };

        static std::vector<ChatCommand> IndividualDropBaseTable =
        {
            { "drop", SEC_PLAYER, false, nullptr, "", IndividualDropCommandTable }
        };

        return IndividualDropBaseTable;
    }

    static bool HandleDropCommand(ChatHandler* handler, char const* args)
    {
        if (!IndividualDropEnabled)
        {
            handler->PSendSysMessage("[Drop] The Individual drop rate module is deactivated.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!*args)
            return false;

        return true;
    }

    static bool HandleViewCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!IndividualDropEnabled)
        {
            handler->PSendSysMessage("[Drop] The Individual drop rate module is deactivated.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        me->GetSession()->SendAreaTriggerMessage("Your current drop rate is %u", me->CustomData.GetDefault<PlayerDropRate>("Individual_Drop")->DropRate);

        return true;
    }

    // Set Command
    static bool HandleSetCommand(ChatHandler* handler, char const* args)
    {
        if (!IndividualDropEnabled)
        {
            handler->PSendSysMessage("[Drop] The Individual drop module is deactivated.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!*args)
            return false;

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        uint32 rate = (uint32)atol(args);
        if (rate > MaxDropRate)
        {
            handler->PSendSysMessage("[Drop] The maximum rate limit is %u.", MaxDropRate);
            handler->SetSentErrorMessage(true);
            return false;
        }
        else if (rate == 0)
        {
            handler->PSendSysMessage("[Drop] The minimum rate limit is 1.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        me->CustomData.GetDefault<PlayerDropRate>("Individual_Drop")->DropRate = rate;
        me->GetSession()->SendAreaTriggerMessage("You have updated your drop rate to %u", rate);
        return true;
    }

    // Default Command
    static bool HandleDefaultCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!IndividualDropEnabled)
        {
            handler->PSendSysMessage("[Drop] The Individual drop module is deactivated.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        me->CustomData.GetDefault<PlayerDropRate>("Individual_Drop")->DropRate = DefaultDropRate;
        me->GetSession()->SendAreaTriggerMessage("You have restored your drop rate to the default value of %u", DefaultDropRate);
        return true;
    }
};

void AddIndividual_DropScripts()
{
    new Individual_Drop_conf();
    new Individual_Drop_Announce();
    new Individual_Drop();
    new Individual_Drop_command();
}
