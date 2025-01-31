#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "ServerFacade.h"

#include "../../modules/Bots/ahbot/AhBot.h"
#include "Database/DatabaseEnv.h"
#include "PlayerbotAI.h"

#include "../../modules/Bots/ahbot/AhBotConfig.h"
#include "Movement/TargetedMovementGenerator.h"

ServerFacade::ServerFacade() {}
ServerFacade::~ServerFacade() {}

float ServerFacade::GetDistance2d(Unit *unit, WorldObject* wo)
{
    float dist =
#ifdef VMANGOS
    unit->GetDistance2d(wo);
#endif
#ifdef CMANGOS
    sqrt(unit->GetDistance2d(wo->GetPositionX(), wo->GetPositionY(), DIST_CALC_NONE));
#endif
    return round(dist * 10.0f) / 10.0f;
}

float ServerFacade::GetDistance2d(Unit *unit, float x, float y)
{
    float dist =
#ifdef VMANGOS
    unit->GetDistance2d(x, y);
#endif
#ifdef CMANGOS
    sqrt(unit->GetDistance2d(x, y, DIST_CALC_NONE));
#endif
    return round(dist * 10.0f) / 10.0f;
}

bool ServerFacade::IsDistanceLessThan(float dist1, float dist2)
{
    return dist1 - dist2 < sPlayerbotAIConfig.targetPosRecalcDistance;
}

bool ServerFacade::IsDistanceGreaterThan(float dist1, float dist2)
{
    return dist1 - dist2 > sPlayerbotAIConfig.targetPosRecalcDistance;
}

bool ServerFacade::IsDistanceGreaterOrEqualThan(float dist1, float dist2)
{
    return !IsDistanceLessThan(dist1, dist2);
}

bool ServerFacade::IsDistanceLessOrEqualThan(float dist1, float dist2)
{
    return !IsDistanceGreaterThan(dist1, dist2);
}

void ServerFacade::SetFacingTo(Player* bot, WorldObject* wo, bool force)
{
    float angle = bot->GetAngle(wo);
    MotionMaster &mm = *bot->GetMotionMaster();
    if (!force && isMoving(bot)) bot->SetFacingTo(bot->GetAngle(wo));
    else
    {
        bot->SetOrientation(angle);
        bot->SendHeartBeat();
    }
}

bool ServerFacade::IsFriendlyTo(Unit* bot, Unit* to)
{
#ifdef VMANGOS
    return bot->IsFriendlyTo(to);
#endif
#ifdef CMANGOS
    return bot->IsFriend(to);
#endif
}

bool ServerFacade::IsHostileTo(Unit* bot, Unit* to)
{
#ifdef VMANGOS
    return bot->IsHostileTo(to);
#endif
#ifdef CMANGOS
    return bot->IsEnemy(to);
#endif
}


bool ServerFacade::IsSpellReady(Player* bot, uint32 spell)
{
#ifdef VMANGOS
    return !bot->HasSpellCooldown(spell);
#endif
#ifdef CMANGOS
    return bot->IsSpellReady(spell);
#endif
}

bool ServerFacade::IsUnderwater(Unit *unit)
{
#ifdef VMANGOS
    return unit->IsUnderWater();
#endif
#ifdef CMANGOS
    return unit->IsUnderwater();
#endif
}

FactionTemplateEntry const* ServerFacade::GetFactionTemplateEntry(Unit *unit)
{
#ifdef VMANGOS
    return unit->getFactionTemplateEntry();
#endif
#ifdef CMANGOS
    return unit->GetFactionTemplateEntry();
#endif
}

Unit* ServerFacade::GetChaseTarget(Unit* target)
{
    if (target->GetTypeId() == TYPEID_PLAYER)
    {
#ifdef VMANGOS
        return static_cast<ChaseMovementGenerator<Player> const*>(target->GetMotionMaster()->GetCurrent())->GetTarget();
#endif
#ifdef CMANGOS
        return static_cast<ChaseMovementGenerator const*>(target->GetMotionMaster()->GetCurrent())->GetCurrentTarget();
#endif
    }
    else
    {
#ifdef VMANGOS
        return static_cast<ChaseMovementGenerator<Creature> const*>(target->GetMotionMaster()->GetCurrent())->GetTarget();
#endif
#ifdef CMANGOS
        return static_cast<ChaseMovementGenerator const*>(target->GetMotionMaster()->GetCurrent())->GetCurrentTarget();
#endif
        return NULL;
    }
}

bool ServerFacade::isMoving(Unit *unit)
{
/*#ifdef MANGOS
    return unit->m_movementInfo.HasMovementFlag(movementFlagsMask);
#endif*/
#ifdef VMANGOS
    return unit->IsMoving();
#endif
}
