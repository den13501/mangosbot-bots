#include "botpch.h"
#include "../../playerbot.h"
#include "QueryQuestAction.h"
#include "../../TravelMgr.h"


using namespace ai;

void QueryQuestAction::TellObjective(string name, int available, int required)
{
    ai->TellMaster(chat->formatQuestObjective(name, available, required), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
}

bool QueryQuestAction::Execute(Event& event)
{
    Player *bot = ai->GetBot();
    WorldPosition pos = WorldPosition(bot);
    WorldPosition* botPos = &pos;
    std::string text = event.getParam();
    bool travel = false;

    if (text.find("travel") != std::string::npos)
    {
        travel = true;
        chat->eraseAllSubStr(text, " travel");
    }

    PlayerbotChatHandler ch(bot);
    uint32 questId = ch.extractQuestId(text);
    if (!questId)
        return false;

    for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        if(questId != bot->GetQuestSlotQuestId(slot))
            continue;

        ostringstream out;
        out << "--- " << chat->formatQuest(sObjectMgr.GetQuestTemplate(questId)) << " ";
        if (bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
        {
            out << "|c0000FF00completed|r ---";
            ai->TellMaster(out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
        }
        else
        {
            out << "|c00FF0000not completed|r ---";
            ai->TellMaster(out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
            TellObjectives(questId);
        }

        if (travel)
        {
            uint32 limit = 0;
            vector<TravelDestination*> allDestinations = sTravelMgr.getQuestTravelDestinations(bot, questId, true, true, -1);

            std::sort(allDestinations.begin(), allDestinations.end(), [botPos](TravelDestination* i, TravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });

            for (auto dest : allDestinations) {
                if (limit > 50)
                    continue;

                ostringstream out;

                uint32 tpoints = dest->getPoints(true).size();
                uint32 apoints = dest->getPoints().size();


                out << round(dest->distanceTo(botPos));

                out << " to " << dest->getTitle();

                out << " " << apoints;
                if (apoints < tpoints)
                    out << "/" << tpoints;
                out << " points.";

                if (!dest->isActive(bot))
                    out << " not active";
                if (dest->isFull(bot))
                    out << " crowded";
                if (dest->isFull(bot))
                    out << " crowded";

                ai->TellMaster(out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

                limit++;
            }
        }

        return true;
    }

    return false;
}

void QueryQuestAction::TellObjectives(uint32 questId)
{
    Quest const* questTemplate = sObjectMgr.GetQuestTemplate(questId);
    QuestStatusData questStatus = bot->getQuestStatusMap()[questId];

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
    {
        if (!questTemplate->ObjectiveText[i].empty())
            ai->TellMaster(questTemplate->ObjectiveText[i], PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

        if (questTemplate->ReqItemId[i])
        {
            int required = questTemplate->ReqItemCount[i];
            int available = questStatus.m_itemcount[i];
            ItemPrototype const* proto = sObjectMgr.GetItemPrototype(questTemplate->ReqItemId[i]);
            TellObjective(chat->formatItem(proto), available, required);
        }

        if (questTemplate->ReqCreatureOrGOId[i])
        {
            int required = questTemplate->ReqCreatureOrGOCount[i];
            int available = questStatus.m_creatureOrGOcount[i];

            if (questTemplate->ReqCreatureOrGOId[i] < 0)
            {
                GameObjectInfo const* info = sObjectMgr.GetGameObjectInfo(-questTemplate->ReqCreatureOrGOId[i]);
                if (info)
                    TellObjective(info->name, available, required);
            }
            else
            {

                CreatureInfo const* info = sObjectMgr.GetCreatureTemplate(questTemplate->ReqCreatureOrGOId[i]);
                if (info)
                    TellObjective(info->Name, available, required);
            }
        }
    }
}
