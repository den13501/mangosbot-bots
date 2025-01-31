#include "botpch.h"
#include "../../playerbot.h"
#include "DropQuestAction.h"


using namespace ai;

bool DropQuestAction::Execute(Event event)
{
    string link = event.getParam();
    if (!GetMaster())
        return false;

    PlayerbotChatHandler handler(GetMaster());
    uint32 entry = handler.extractQuestId(link);

    // remove all quest entries for 'entry' from quest log
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = bot->GetQuestSlotQuestId(slot);
        Quest const* quest = sObjectMgr.GetQuestTemplate(logQuest);
        if (!quest)
            continue;

        if (logQuest == entry || link.find(quest->GetTitle()) != string::npos)
        {
            bot->SetQuestSlot(slot, 0);

            // we ignore unequippable quest items in this case, its' still be equipped
            bot->TakeOrReplaceQuestStartItems(logQuest, false, false);
            entry = logQuest;
            break;
        }
    }

    if (!entry)
        return false;

    bot->SetQuestStatus(entry, QUEST_STATUS_NONE);
    bot->getQuestStatusMap()[entry].m_rewarded = false;

    ai->TellMaster("Quest removed");
    return true;
}
