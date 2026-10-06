/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_AURAIDTRIGGERS_H
#define PLAYERBOTS_AURAIDTRIGGERS_H

#include "Trigger.h"
#include <string>
#include <utility>
#include <vector>

class PlayerbotAI;

// Base classes only: class contexts instantiate them with their own ids and trigger names.
class AuraIdTriggerBase : public Trigger
{
public:
    AuraIdTriggerBase(PlayerbotAI* botAI, std::string const name, std::vector<uint32> ids, bool ownedByBot,
                      int checkInterval)
        : Trigger(botAI, name, checkInterval), Ids(std::move(ids)), OwnedByBot(ownedByBot)
    {
    }

    std::string const GetTargetName() override { return "self target"; }

protected:
    ObjectGuid GetCaster() const;

    std::vector<uint32> Ids;
    bool OwnedByBot;
};

class HasAuraIdTrigger : public AuraIdTriggerBase
{
public:
    HasAuraIdTrigger(PlayerbotAI* botAI, std::string const name, std::vector<uint32> ids, bool ownedByBot = true,
                     int checkInterval = 1)
        : AuraIdTriggerBase(botAI, name, std::move(ids), ownedByBot, checkInterval)
    {
    }

    bool IsActive() override;
};

class NoAuraIdTrigger : public AuraIdTriggerBase
{
public:
    NoAuraIdTrigger(PlayerbotAI* botAI, std::string const name, std::vector<uint32> ids, bool ownedByBot = true,
                    int checkInterval = 1)
        : AuraIdTriggerBase(botAI, name, std::move(ids), ownedByBot, checkInterval)
    {
    }

    bool IsActive() override;
};

class AuraIdStacksTrigger : public AuraIdTriggerBase
{
public:
    AuraIdStacksTrigger(PlayerbotAI* botAI, std::string const name, std::vector<uint32> ids, uint8 minStacks,
                        bool ownedByBot = true, int checkInterval = 1)
        : AuraIdTriggerBase(botAI, name, std::move(ids), ownedByBot, checkInterval), _minStacks(minStacks)
    {
    }

    bool IsActive() override;

private:
    uint8 _minStacks;
};

class AuraIdExpiringTrigger : public AuraIdTriggerBase
{
public:
    AuraIdExpiringTrigger(PlayerbotAI* botAI, std::string const name, std::vector<uint32> ids, int32 belowMs,
                          bool ownedByBot = true, int checkInterval = 1)
        : AuraIdTriggerBase(botAI, name, std::move(ids), ownedByBot, checkInterval), _belowMs(belowMs)
    {
    }

    bool IsActive() override;

private:
    int32 _belowMs;
};

// The inverse of AuraIdExpiringTrigger: one of the ids is present and is permanent or has at least `atLeastMs` left,
// e.g. "the buff still has 1.5 s to spend".
class AuraIdRemainingAboveTrigger : public AuraIdTriggerBase
{
public:
    AuraIdRemainingAboveTrigger(PlayerbotAI* botAI, std::string const name, std::vector<uint32> ids, int32 atLeastMs,
                                bool ownedByBot = true, int checkInterval = 1)
        : AuraIdTriggerBase(botAI, name, std::move(ids), ownedByBot, checkInterval), _atLeastMs(atLeastMs)
    {
    }

    bool IsActive() override;

private:
    int32 _atLeastMs;
};

// Current-target versions of the triggers above, for debuffs and DoTs. Same base-class rules apply.
class TargetHasAuraIdTrigger : public HasAuraIdTrigger
{
public:
    using HasAuraIdTrigger::HasAuraIdTrigger;

    std::string const GetTargetName() override { return "current target"; }
};

class TargetNoAuraIdTrigger : public NoAuraIdTrigger
{
public:
    using NoAuraIdTrigger::NoAuraIdTrigger;

    std::string const GetTargetName() override { return "current target"; }
};

class TargetAuraIdStacksTrigger : public AuraIdStacksTrigger
{
public:
    using AuraIdStacksTrigger::AuraIdStacksTrigger;

    std::string const GetTargetName() override { return "current target"; }
};

class TargetAuraIdExpiringTrigger : public AuraIdExpiringTrigger
{
public:
    using AuraIdExpiringTrigger::AuraIdExpiringTrigger;

    std::string const GetTargetName() override { return "current target"; }
};

class TargetAuraIdRemainingAboveTrigger : public AuraIdRemainingAboveTrigger
{
public:
    using AuraIdRemainingAboveTrigger::AuraIdRemainingAboveTrigger;

    std::string const GetTargetName() override { return "current target"; }
};

// Active when AI_VALUE2(uint8, valueName, qualifier) >= minCount, e.g. "3+ enemies within 8 yd".
class CountAtLeastTrigger : public Trigger
{
public:
    CountAtLeastTrigger(PlayerbotAI* botAI, std::string const name, std::string const valueName,
                        std::string const qualifier, uint8 minCount)
        : Trigger(botAI, name), _valueName(valueName), _qualifier(qualifier), _minCount(minCount)
    {
    }

    bool IsActive() override;

private:
    std::string _valueName;
    std::string _qualifier;
    uint8 _minCount;
};

#endif
