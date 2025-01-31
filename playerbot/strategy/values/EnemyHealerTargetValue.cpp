#include "botpch.h"
#include "../../playerbot.h"
#include "EnemyHealerTargetValue.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

Unit* EnemyHealerTargetValue::Calculate()
{
    string spell = qualifier;

    list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("attackers")->Get();
    Unit* target = ai->GetAiObjectContext()->GetValue<Unit*>("current target")->Get();
    for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit || unit == target)
            continue;

        if (bot->GetDistance(unit) > ai->GetRange("spell"))
            continue;

        if (!ai->IsInterruptableSpellCasting(unit, spell, true))
            continue;

        Spell* spell = unit->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (spell && Spells::IsPositiveSpell(spell->m_spellInfo->Id))
            return unit;

        spell = unit->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (spell && Spells::IsPositiveSpell(spell->m_spellInfo->Id))
            return unit;
    }

    return NULL;
}
