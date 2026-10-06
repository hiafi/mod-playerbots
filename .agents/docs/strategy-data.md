# Strategy data (YAML rows)

Class strategies are a small C++ shell plus YAML rows. Read this before touching `data/strategies/`, `src/Bot/Data/`,
or a YAML-backed class strategy (Ret, Fire, Arcane, Frost today). The engine itself is in `ai-engine.md`.

## Contents

1. How the hybrid works
2. The condition language
3. When to write C++ instead
4. Lessons
5. Workflow
6. How to add a function

## 1. How the hybrid works

**The shell.** A strategy is still a C++ class. It keeps its name registration, parent chain, multipliers, node
factory and default actions. Its `InitTriggers` chains the parent and appends the rows:

```cpp
void MageReworkFrostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MageReworkGenericStrategy::InitTriggers(triggers);
    ai::data::AppendRows("mage/frost", triggers);
}
```

A new spec is a ~10-line shell plus a YAML file, plus the usual strategy-context registration (the wiring checklist in
`ai-engine.md`). Actions, values, triggers and multipliers stay C++; YAML only names them.

**The file.** `data/strategies/<class>/<spec>.yaml`. The loader reads every `*.yaml` under the data path
(`AiPlayerbot.StrategyDataPath`, empty = the module's `data/strategies`), in sorted path order. A file may hold several
YAML documents (`---`), one strategy key each.

| Key | Meaning |
|---|---|
| `strategy:` | The key the shell passes to `AppendRows`, `<class>/<spec>`. Unique across all files. |
| `class:` | Lowercase class name (`dk` for death knight). Selects the creator tables that `do:`, `trigger:` and `value()` names are checked against at load; `.botstrat check <bot>` binds only the bot's own class. |
| `conditions:` | Named conditions, reusable inside the same YAML document (each `---` document starts fresh; section 2). |
| `rows:` | A sequence. Order is kept, but execution order is by relevance. |

**A row** has `do:`, exactly one of `when:` / `trigger:`, and `relevance:`.

| Field | Meaning |
|---|---|
| `do:` | An action name, or a list of them (several `NextAction`s on one row). Must exist in a creator table. |
| `when:` | A condition string. The row becomes a `ConditionTrigger`. |
| `trigger:` | An existing C++ trigger by name, optionally `name::qualifier`. |
| `relevance:` | A number, or a band plus an offset: `normal + 8`, `emergency - 1`. |

Bands, as the loader spells them: `idle`, `bg`, `default`, `normal`, `high`, `move`, `interrupt`, `dispel`, `raid`,
`light heal`, `medium heal`, `critical heal`, `emergency` (the `ACTION_*` values of `ai-engine.md`). The loader accepts
any finite number, so the ceiling is a rule you follow, not a check: stay below the pull sequence (`ai-engine.md`).
Use a band plus an offset, never a bare number chosen by racing existing rows.

**Loading.** The snapshot is built at startup and by `.botstrat reload`. It is all-or-nothing per reload: any error
keeps the old snapshot. At startup a file with errors contributes nothing and its strategy runs without rows. A
`when:` row gets the trigger name `data::<key>#<row index>` (a qualified `data` trigger). Each bot binds the
condition once, then re-inits its engines when the snapshot generation changes.

## 2. The condition language

A condition is a string compiled once at load into a tree, then bound per bot: every value it reads resolves to a typed
`Value<T>*` once. Evaluating only walks the tree. A failure while evaluating reads as false.

**Operators.** `and`, `or`, `not`, parentheses; `< <= > >= == !=` on numbers. A bool stands alone. Both sides of a
comparison may be values. No arithmetic, variables or loops.

**Units.** `self`, `target` (the current target), or a quoted Unit* value name (`"effective tank"`). A function on a
missing or dead unit never reports a default (health 100, distance 0); see the table.

**Named conditions.** A bare identifier refers to a `conditions:` entry of the same file. Names may not be function
names or keywords (`and or not self target own`). Cycles are rejected.

**`value("name::qualifier")`.** Reads any registered value by its registered name. A `bool` value is a condition; a
`uint8`, `uint32` or `float` value is a number. Using one the wrong way fails at bind time. Unit* values go in unit
arguments, not here.

### The function table

"Missing" is a unit that is absent or dead.

| Function | Result | Missing / unknown | Backed by |
|---|---|---|---|
| `value(name)` | bool or number | no unit gate | any registered uint8/uint32/float/bool value |
| `health(u)` | number, % truncated | 0 | value `health::<u>` |
| `health_pct(u)` | number, % | 0 | `Unit::GetHealthPct` |
| `mana(u)` | number, % truncated | 0 | value `mana::<u>` |
| `enemies_within(yd)` | number | | value `enemies within::<yd>` |
| `enemies_near_target(yd)` | number | | value `enemies near target::<yd>` |
| `enemies_in_cone(yd, deg)` | number | | value `enemies in cone::<yd>,<deg>` |
| `last_crit(ids)` | number | UINT32_MAX when the last result was not a crit | value `last own spell crit::<ids>` |
| `cooldown(spell)` | ms | 0 when ready, unknown or unresolved | `ai::spell::CooldownRemainingMs` |
| `ms_since_cast(spell)` | ms | infinity when never cast, evicted, or the name is unresolved | `ai::spell::SpellCastStamps` |
| `lifetime(u)` | ms | 0 | value `target lifetime` / `estimated lifetime::<u>` |
| `time_since_target_change()` | ms | | value `time since target change` |
| `exists(u)` | bool | false | the unit value yields a unit (dead or alive) |
| `alive(u)` | bool | false | exists and alive |
| `aura(u, ids[, own])` | bool | false | `ai::aura::HasAnyAura` |
| `stacks(u, ids[, own])` | number | 0 | `ai::aura::AuraStacks` |
| `remaining(u, ids[, own])` | ms | 0; infinity for a permanent aura | `ai::aura::AuraRemainingMs` |
| `charges(u, ids[, own])` | number | 0 | `ai::aura::AuraCharges` |
| `known(spell)` | bool | | id: `Player::HasSpell`; name: `spell id::<name>` is not 0 |
| `boss(u)` / `elite(u)` / `controlled(u)` | bool | false | `ai::target::IsBoss` / `IsElite` / `IsControlled` |
| `is_self(u)` | bool | false | the unit is the bot |
| `in_arc(u, deg)` | bool | false | `Player::HasInArc`; `deg` above 0 and below 360 |
| `in_range(u, yd)` | bool | false | `Player::GetDistance` (3D, reach-adjusted) |
| `dynobj(spellId)` | bool | | the bot owns a dynamic object of that spell |
| `moving(u)` | bool | false | value `moving::<u>` |
| `trigger(name)` | bool | | `Trigger::IsActive` of a C++ trigger |

Argument forms: `ids` is a spell id or a quoted comma list (any-of); `spell` is an id or a quoted name resolved per bot
through the value `spell id::<name>`; `own` means cast by the bot (default: any caster).

`trigger()` calls `IsActive()` directly: the nested trigger's check interval and per-tick reset don't apply. Use it only
for interval-1 triggers without per-tick state. A `data` trigger can't be nested.

**Conventions.**

- A missing aura reads 0 (and `aura()` false). `remaining()` of a permanent aura compares as infinite.
- `cooldown()` ignores the global cooldown and returns 0 for an unknown spell. Gate on `known()` for a spell a level
  may lack: `known(642) and cooldown(642) == 0`.
- Compare ms functions in ms. `ms_since_cast(x) <= 2000` is false for a spell never cast.
- `.botstrat dump <key>` prints each condition re-printed from its tree; use it to check what the compiler read.

## 3. When to write C++ instead

YAML covers a condition made of the functions above. Write C++ for:

- **Logic values:** seal choice, aura swap, meteor target, clusters, anything computed. Register a value and read it
  with `value()`.
- **Stateful, item-scanning or pet triggers:** the bags (Mana Gem), a pet's cooldown (Freeze), anything with per-tick
  state. Reference them with `trigger:` or `trigger("name")`.
- **Actions.** All of them. YAML only names them.

**Actions that re-check their row.** A queued action can run well after the tick that queued it (section 4), so a row
whose condition can go stale gets its own action class. `isUseful()` re-checks the same condition. YAML and C++ must
agree, so the check lives in a shared helper in the spec's namespace (`ai::mage_fire`, `ai::mage_frost`,
`ai::mage_arcane`), and the action calls it:

```cpp
bool MageFrostGlacialSpikeAction::isUseful()
{
    return (!_shattered || ai::mage_frost::ShatteringColdReady(botAI, GetTarget())) && CastSpellAction::isUseful();
}
```

When a helper mirrors a YAML clause, put a comment on both sides. If the YAML clause can be written with a function
(`ms_since_cast(200004) <= 2000`), the C++ helper reads the same source (`SpellCastStamps`), so they can't drift.

## 4. Lessons

Each lesson names the case that taught it.

- **Queued baskets outlive their tick.** An action stays queued up to 5 s, and the engine doesn't tick while the bot
  casts, so a basket can survive a whole cast. Fire's Flashpoint and Evocation, and Frost's Ice Lance after Glacial
  Spike. Re-check in `isUseful()`.
- **Rows with the same `do:` name merge into one basket** at the highest relevance. Rows that need different re-checks
  need different action names: Frost's three Ice Lance actions.
- **Cached values lag a cast.** Arcane's `arcane burn` caches 1 s. A re-check calls the uncached helper, not the value.
- **Server spell data can forbid what a guide asks for.** Shared cooldown categories, excluded caster auras, and
  cooldown-on-event spells that read an endless cooldown: Arcane Power and Presence of Mind (shared category 1151,
  each excludes the other's buff, and PoM's cooldown starts only when its buff ends: until then the whole category reads
  an endless cooldown). Check `Spell.dbc` and the spell scripts, not the tooltip.
- **Name rework actions and values with a spec prefix** (`frost flurry`, `arcane burn`), never a stock name; the
  stock one stays registered. Grep the creator tables for collisions.
- **Check aura ids, not names.** Several auras share a name: Fingers of Frost's 74396 carries the count as stacks,
  44544 is only the aura state.
- **Wall clocks lie in the sim.** mod-dpssim drives `getMSTime()`; `time(nullptr)` and `GameTime` run on the real
  clock. History that conditions read must use `getMSTime()`: that is why `ms_since_cast` exists, replacing a
  whole-second `last spell cast` check.

## 5. Workflow

1. Edit the YAML.
2. `.botstrat check <bot>`: validates every file without loading it, and binds the bot's class conditions (an unknown
   value, a wrong type). Then `.botstrat reload`, and `.botstrat dump <key>` to read the compiled rows.
3. **Tune with the sim, no rebuild.** `modules/mod-dpssim/tools/run-sim.sh` runs the built image with the live
   `modules/` tree mounted, so a YAML edit shows in the next sim. Arcane's Barrage band change moved the sim from 6564
   to 7720 DPS this way. The guide-talent profiles are `modules/mod-dpssim/reports/*GuideSim.conf` (gitignored
   copies); pass them as a path, since a bare name only resolves under `conf/profiles/`.
4. Rebuild only for C++ changes (a new action, value, trigger or function).

A broken file is rejected and the previous data stays live; the log or the GM reply names the file, the node path
(`mage/frost.yaml: rows[3].when`) and the column.

## 6. How to add a function

Keep it class-agnostic and cheap: no class names or spell ids in `src/Bot/Data/` or `src/Ai/Base/`, and nothing per
tick beyond the bound tree walk. The worked example is `ms_since_cast(spell)`.

1. **Backing data.** If the function needs history, add a small per-bot structure fed outside the tick, bounded and
   allocation-free. `ms_since_cast` uses `ai::spell::SpellCastStamps` (`src/Ai/Base/Util/`): 16 `(spell id, getMSTime)`
   slots owned by `PlayerbotAI`, stamped in `PlayerbotAI::CastSpell` next to `last spell cast`. A repeat cast
   overwrites its slot, a new spell replaces the oldest. Use `getMSTime()`, never `time(nullptr)`.
   **Scope:** only the bot's own casts through `PlayerbotAI::CastSpell`. The unit overload stamps after the cast was
   accepted; the location overload (ground spells: Blizzard, Rain of Fire) stamps even when the cast is refused, as
   `last spell cast` does. Pet spells (`CastPetSpellAction`), vehicle spells, item use and direct `bot->CastSpell`
   calls are never stamped. After 16 other distinct spells a stamp is evicted and reads as never cast, so
   `ms_since_cast(x) > N` is true then: fine for short windows, unreliable for long "not cast in N s" checks.
2. **`ExprFn` enum** (`StrategyExpression.h`): add the entry.
3. **Table row** (`FUNCTIONS` in `StrategyExpression.cpp`): name, `ExprFn`, result type, min/max arguments, argument
   kinds (`Unit`, `Number`, `Ids`, `Spell`, `SpellId`, `Text`, `Own`). `{"ms_since_cast", ..., ExprType::Number, 1, 1,
   {ArgKind::Spell}}`.
4. **Parse and compile.** Argument parsing is driven by the kinds. Only a function that needs a derived name or a
   range check adds a `case` in the compile switch: `ms_since_cast` shares `known()`'s, which sets `valueName` to
   `spell id::<name>` for a name argument.
5. **Bind** (`StrategyBinding.cpp`, `BindCall`): resolve everything once. A typed value goes into a `Node` member via
   `Cast<T>`; `ms_since_cast` binds the `spell id` value for a name argument and nothing for an id.
6. **Evaluate** (`EvalCall`): no string building, no map lookup, a fixed-size scan. Return a double; a result that
   means "never" is infinity (as `remaining()` of a permanent aura), never a sentinel that could pass a comparison.
7. **Canonical form.** `.botstrat dump` prints through `PrintCall`, which walks the argument kinds; a new kind of
   argument needs a case there.
8. **Docs.** Add the function to the comment block above `FUNCTIONS` and to the table in section 2.
9. If a C++ helper needs the same fact, expose one accessor and have both read it (`FlurryJustCast` reads
   `GetSpellCastStamps().MsSince`).
