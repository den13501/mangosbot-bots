#include "botpch.h"
#include "../../playerbot.h"
#include "TrainerAction.h"
#include "../../ServerFacade.h"

using namespace ai;

void TrainerAction::Learn(uint32 cost, TrainerSpell const* tSpell, ostringstream& msg)
{
    if (bot->GetMoney() < cost)
    {
        msg << " - too expensive";
        return;
    }

    bot->ModifyMoney(-int32(cost));

    SpellEntry const* proto = sServerFacade.LookupSpellInfo(tSpell->spell);
    if (!proto)
        return;

#ifdef CMANGOS
    Spell* spell = new Spell(bot, proto, false);
    SpellCastTargets targets;
    targets.setUnitTarget(bot);
    spell->SpellStart(&targets);
#endif

#ifdef VMANGOS
    bool learned = false;
    for (int j = 0; j < 3; ++j)
    {
        if (proto->Effect[j] == SPELL_EFFECT_LEARN_SPELL)
        {
            uint32 learnedSpell = proto->EffectTriggerSpell[j];
            bot->LearnSpell(learnedSpell, false);
            learned = true;
        }
    }
    if (!learned) bot->LearnSpell(tSpell->spell, false);
#endif

    msg << " - learned";
}

void TrainerAction::Iterate(Creature* creature, TrainerSpellAction action, SpellIds& spells)
{
    TellHeader(creature);

    TrainerSpellData const* cSpells = creature->GetTrainerSpells();
    TrainerSpellData const* tSpells = creature->GetTrainerTemplateSpells();
    float fDiscountMod =  bot->GetReputationPriceDiscount(creature);
    uint32 totalCost = 0;

    TrainerSpellData const* trainer_spells = cSpells;
    if (!trainer_spells)
        trainer_spells = tSpells;

    for (TrainerSpellMap::const_iterator itr =  trainer_spells->spellList.begin(); itr !=  trainer_spells->spellList.end(); ++itr)
    {
        TrainerSpell const* tSpell = &itr->second;

        if (!tSpell)
            continue;

        uint32 reqLevel = 0;

        //reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);
        TrainerSpellState state = bot->GetTrainerSpellState(tSpell/*, reqLevel*/);
        if (state != TRAINER_SPELL_GREEN)
            continue;

        uint32 spellId = tSpell->spell;
        const SpellEntry *const pSpellInfo =  sServerFacade.LookupSpellInfo(spellId);
        if (!pSpellInfo)
            continue;

        if (!spells.empty() && spells.find(tSpell->spell) == spells.end())
            continue;

        uint32 cost = uint32(floor(tSpell->spellCost *  fDiscountMod));
        totalCost += cost;

        ostringstream out;
        out << chat->formatSpell(pSpellInfo) << chat->formatMoney(cost);

        if (action)
            (this->*action)(cost, tSpell, out);

        ai->TellMaster(out);
    }

    TellFooter(totalCost);
}


bool TrainerAction::Execute(Event event)
{
    string text = event.getParam();

    Player* master = GetMaster();
    if (!master)
        return false;

    Creature *creature = ai->GetCreature(master->GetSelectionGuid());
    if (!creature || !creature->IsTrainer())
        return false;

    if (!creature->IsTrainerOf(bot, false))
    {
        ai->TellError("This trainer cannot teach me");
        return false;
    }

    // check present spell in trainer spell list
    TrainerSpellData const* cSpells = creature->GetTrainerSpells();
    TrainerSpellData const* tSpells = creature->GetTrainerTemplateSpells();
    if (!cSpells && !tSpells)
    {
        ai->TellError("No spells can be learned from this trainer");
        return false;
    }

    uint32 spell = chat->parseSpell(text);
    SpellIds spells;
    if (spell)
        spells.insert(spell);

    if (text.find("learn") != string::npos)
        Iterate(creature, &TrainerAction::Learn, spells);
    else
        Iterate(creature, NULL, spells);

    return true;
}

void TrainerAction::TellHeader(Creature* creature)
{
    ostringstream out; out << "--- Can learn from " << creature->GetName() << " ---";
    ai->TellMaster(out);
}

void TrainerAction::TellFooter(uint32 totalCost)
{
    if (totalCost)
    {
        ostringstream out; out << "Total cost: " << chat->formatMoney(totalCost);
        ai->TellMaster(out);
    }
}
