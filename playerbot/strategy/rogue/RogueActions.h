#pragma once

#include "../actions/GenericActions.h"
#include "RogueComboActions.h"
#include "RogueOpeningActions.h"
#include "RogueFinishingActions.h"

namespace ai
{
    BUFF_ACTION(CastColdBloodAction, "cold blood");

	class CastEvasionAction : public CastBuffSpellAction
	{
	public:
		CastEvasionAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "evasion") {}
	};

	class CastSprintAction : public CastBuffSpellAction
	{
	public:
		CastSprintAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "sprint") {}
        virtual string GetTargetName() { return "self target"; }
	};

    class CastStealthAction : public CastBuffSpellAction
    {
    public:
        CastStealthAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "stealth") {}
        virtual string GetTargetName() { return "self target"; }
        virtual bool isUseful()
        {
            bool hasStealth = ai->HasAura("stealth", bot);
            if (hasStealth)
                return false;

            // do not use with WSG flag
            return !ai->HasAura(23333, bot) && !ai->HasAura(23335, bot) && !ai->HasAura(34976, bot);
        }
        virtual bool Execute(Event event)
        {
            if (ai->CastSpell("stealth", bot))
            {
                ai->ChangeStrategy("+stealthed", BOT_STATE_COMBAT);
                bot->InterruptSpell(CURRENT_MELEE_SPELL);
            }
            return true;
        }
    };

    class UnstealthAction : public Action {
    public:
        UnstealthAction(PlayerbotAI* ai) : Action(ai, "unstealth") {}
        virtual bool Execute(Event event) {
            ai->RemoveAura("stealth");
            ai->ResetStrategies();
            return true;
        }
    };

    class CheckStealthAction : public Action {
    public:
        CheckStealthAction(PlayerbotAI* ai) : Action(ai, "check stealth") {}
        virtual bool isPossible() { return true; }
        virtual bool Execute(Event event) {
            bool hasStealth = ai->HasAura("stealth", bot);
            if (hasStealth)
            {
                if (!ai->HasStrategy("stealthed", BOT_STATE_COMBAT))
                    ai->ChangeStrategy("+stealthed", BOT_STATE_COMBAT);
            }
            else if (!hasStealth)
            {
                ai->ResetStrategies();
                //ai->ChangeStrategy("+dps,-stealthed", BOT_STATE_COMBAT);
            }
            return true;
        }
    };

	class CastKickAction : public CastSpellAction
	{
	public:
		CastKickAction(PlayerbotAI* ai) : CastSpellAction(ai, "kick") {}
	};

	class CastFeintAction : public CastBuffSpellAction
	{
	public:
		CastFeintAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "feint") {}
	};

	class CastDismantleAction : public CastSpellAction
	{
	public:
		CastDismantleAction(PlayerbotAI* ai) : CastSpellAction(ai, "dismantle") {}
	};

	class CastDistractAction : public CastSpellAction
	{
	public:
		CastDistractAction(PlayerbotAI* ai) : CastSpellAction(ai, "distract") {}
	};

	class CastVanishAction : public CastBuffSpellAction
	{
	public:
		CastVanishAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "vanish") {}
        virtual bool isUseful()
        {
            // do not use with WSG flag or EYE flag
            return !ai->HasAura(23333, bot) && !ai->HasAura(23335, bot) && !ai->HasAura(34976, bot);
        }
	};

	class CastBlindAction : public CastDebuffSpellAction
	{
	public:
		CastBlindAction(PlayerbotAI* ai) : CastDebuffSpellAction(ai, "blind") {}
	};


	class CastBladeFlurryAction : public CastBuffSpellAction
	{
	public:
		CastBladeFlurryAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "blade flurry") {}
	};

	class CastAdrenalineRushAction : public CastBuffSpellAction
	{
	public:
		CastAdrenalineRushAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "adrenaline rush") {}
	};

	class CastKillingSpreeAction : public CastBuffSpellAction
	{
	public:
		CastKillingSpreeAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "killing spree") {}
	};

    class CastKickOnEnemyHealerAction : public CastSpellOnEnemyHealerAction
    {
    public:
        CastKickOnEnemyHealerAction(PlayerbotAI* ai) : CastSpellOnEnemyHealerAction(ai, "kick") {}
    };

    class CastTricksOfTheTradeOnPartyAction : public BuffOnPartyAction {
    public:
        CastTricksOfTheTradeOnPartyAction(PlayerbotAI* ai) : BuffOnPartyAction(ai, "tricks of the trade") {}
    };

    class CastCloakOfShadowsAction : public CastCureSpellAction
    {
    public:
        CastCloakOfShadowsAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "cloak of shadows") {}
    };
}
