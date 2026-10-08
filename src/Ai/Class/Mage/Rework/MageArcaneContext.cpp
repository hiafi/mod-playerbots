/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MageArcaneContext.h"
#include "MageArcaneActions.h"
#include "MageArcaneValues.h"

namespace
{
template <typename Base, typename Derived>
Base* Make(PlayerbotAI* botAI)
{
    return new Derived(botAI);
}
}  // namespace

MageArcaneActionFactory::MageArcaneActionFactory()
{
    creators["arcane use mana gem"] = &Make<Action, MageArcaneManaGemAction>;
    creators["arcane evocation"] = &Make<Action, MageArcaneEvocationAction>;
    creators["arcane missiles barrage"] = &Make<Action, MageArcaneMissilesAction>;
    creators["arcane barrage stacks"] = &Make<Action, MageArcaneBarrageAction>;
    creators["arcane overload"] = &Make<Action, MageArcaneOverloadAction>;
    creators["arcane temporal convergence"] = &Make<Action, MageArcaneTemporalConvergenceAction>;
    creators["arcane brilliance aura"] = &Make<Action, MageArcaneBrillianceAuraAction>;
    creators["arcane amplify magic"] = &Make<Action, MageArcaneAmplifyMagicAction>;
    creators["arcane explosion cast"] = &Make<Action, MageArcaneExplosionAction>;
}

MageArcaneValueFactory::MageArcaneValueFactory()
{
    creators["arcane burn"] = &Make<UntypedValue, MageArcaneBurnValue>;
    creators["arcane mana gem usable"] = &Make<UntypedValue, MageArcaneManaGemUsableValue>;
}
