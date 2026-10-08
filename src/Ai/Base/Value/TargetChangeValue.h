/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_TARGETCHANGEVALUE_H
#define PLAYERBOTS_TARGETCHANGEVALUE_H

#include "ObjectGuid.h"
#include "Value.h"

class PlayerbotAI;

// Milliseconds since the `current target` guid last changed, on getMSTime. There is no event hook: the guid and a
// timestamp are kept in the value and updated each time it is read, so a change is seen at the next read and a
// target that came and went between two reads is missed. Read it every tick from a trigger to keep it exact.
//   - First read: the target is treated as new, 0.
//   - A different target than the previous read, including the same guid coming back after another target or after
//     none: 0, and the clock restarts.
//   - No target (cleared, despawned or not in the map): 0, and the next target starts a new clock.
class TimeSinceTargetChangeValue : public Uint32CalculatedValue
{
public:
    TimeSinceTargetChangeValue(PlayerbotAI* botAI, std::string const name = "time since target change")
        : Uint32CalculatedValue(botAI, name)
    {
    }

    uint32 Calculate() override;

private:
    ObjectGuid _target;
    uint32 _sinceMs = 0;
};

#endif
