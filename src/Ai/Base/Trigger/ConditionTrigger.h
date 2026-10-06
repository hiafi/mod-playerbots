/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CONDITIONTRIGGER_H
#define PLAYERBOTS_CONDITIONTRIGGER_H

#include "NamedObjectContext.h"
#include "StrategyBinding.h"
#include "Trigger.h"
#include <memory>
#include <string>

class PlayerbotAI;

// The trigger behind an inline "when:" condition of a YAML strategy row (src/Bot/Data). Registered once as "data";
// the row's TriggerNode is named "data::<strategy key>#<row index>" and that qualifier picks the condition out of the
// current snapshot. The condition is bound to this bot on first use and again whenever the snapshot changes. A row
// that a reload removed, or a condition whose values this bot can't bind, is inactive.
class ConditionTrigger : public Trigger, public Qualified
{
public:
    explicit ConditionTrigger(PlayerbotAI* botAI) : Trigger(botAI, "data") {}

    bool IsActive() override;
    // "data::<key>#<row>", so action logs and the perf monitor tell rows apart
    std::string const getName() override { return _fullName.empty() ? Trigger::getName() : _fullName; }
    using Qualified::Qualify;
    void Qualify(std::string const qual) override
    {
        Qualified::Qualify(qual);
        _fullName = "data::" + qual;
    }

private:
    void Rebind(uint32 generation);

    std::string _fullName;
    bool _bound = false;
    uint32 _generation = 0;
    std::unique_ptr<ai::data::BoundCondition> _condition;
};

#endif
