# .agents/docs

Task-scoped agent guidance, routed from AGENTS.md's "Mandatory reading per task". Keep every doc
compact and agent-agnostic.

## Taxonomy

- Root — task-type docs (`cpp-guidelines.md`, `sql-guidelines.md`, `code-review.md`,
  `self-review-rules.md`, `pull-requests.md`), kebab-case. `cpp-guidelines.md` is a verbatim copy of the core's
  `.agents/docs/cpp-guidelines.md`; refresh it from there rather than editing it here.
- `<lang>-<topic>.md` — language-scoped specializations: `cpp-playerbots.md` holds the module's
  C++ conventions on top of `cpp-guidelines.md`.
- `ai-engine.md` — the Strategy-Trigger-Action engine: architecture and wiring, routed from any
  strategy, action, trigger, value, or multiplier work.
- `strategy-data.md` — the YAML strategy rows and condition language: format, function table, lessons, how to add a
  function. Routed from `data/strategies/` and `src/Bot/Data/` work.
- `systems/` — subsystem docs when a subsystem outgrows a section (RPG, travel, guild, LFG, ...),
  plain kebab-case names.

## Placing new guidance

- The most specific applicable doc wins.
- Generic C++ lesson that also holds for the core → propose it upstream in the core's
  `cpp-guidelines.md`, then refresh the copy; module-only C++ lesson → `cpp-playerbots.md`.
- Engine or wiring lesson → `ai-engine.md`.
- YAML strategy rows or condition-language lesson → `strategy-data.md`.
- Subsystem-specific lesson → `systems/<subsystem>.md`; create it if missing.
- Extend an existing doc before creating a new one.
- A new doc REQUIRES adding its routing bullet to AGENTS.md's "Mandatory reading per task" in the
  same change.
