/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ROWCHECKEDACTION_H
#define PLAYERBOTS_ROWCHECKEDACTION_H

#include "ConditionTrigger.h"
#include "Event.h"

// An action that drops itself when the YAML row that queued it no longer holds. Queued baskets outlive their tick (up
// to 5 s, and the engine doesn't tick during a cast), so a row whose condition can go stale (a heal threshold, a proc
// window) wraps its action in this instead of copying the condition into C++: the action evaluates the same condition
// again, from the same row. An action whose Execute fails is logged FAILED and the engine moves on to the next basket.
//
// Rows that queue the same action name (qualifier included) merge into one basket that keeps the first pusher's
// event, so only that row is re-checked: give each row its own action name or qualifier.
//
// `Base` is any action whose Execute takes an Event: `RowCheckedAction<CastSpellAction>`. Events from anything but a
// YAML row (a C++ trigger, a default action, a chat command) are not re-checked.
template <class Base>
class RowCheckedAction : public Base
{
public:
    using Base::Base;

    bool Execute(Event event) override { return ai::data::RowStillHolds(this->botAI, event) && Base::Execute(event); }
};

#endif
