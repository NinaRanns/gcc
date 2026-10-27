# C++ Profiles framework (P3589) in GCC: research branch

This branch implements the C++ Profiles framework of **P3589R3**, *C++ Profiles: The Framework*, in the C++ front end, in order to evaluate the paper by implementing it. It is private research: the paper is not part of the standard, and nothing here is proposed for GCC yet. Authors: Nina Ranns and Iain Sandoe.

## What it is for

- **Find the holes and gaps in the paper** by implementing it, and back each one with a test.
- **Feed a review of the paper** (holes, gaps, editorial points, and proposed dispositions), towards an R4.
- **Grow into an implementation:** the framework first, then the mechanism that carries profile state to the profiles, then probe profiles, then real ones.

## Working rules

- **The paper as written is the reference.** Where it is silent or contradictory, the prototype takes a choice *to proceed*; that choice neither settles the question nor excludes others.
- **Two kinds of test, kept apart:**
  - *conformance tests* show the framework follows the paper; they live with the step that introduces the feature, and would land with it;
  - *question tests* demonstrate a point raised in the review; they live in `g++.dg/profiles/p3589r3-questions/`, in one commit at the tip, named after the review item they show. When a question is settled, its test moves to its step. Unsure which? It is a question test.
- **Trunk defects found on the way are fixed separately**, below the branch, one commit each.
- **The history is rewritten freely** while the shape settles, so steps are named, never cited by SHA.

## Status

**Nothing here is useful to an end user yet; it is for developers.** Under `-fprofiles`, in C++ and Objective-C++, the markup is parsed, and the framework records and checks all of it, but no profile acts on it.

**Open design questions:**

- the interface: a query the framework answers, or profiles reading the recorded markup;
- plugin, core or a hybrid;
- where profiles act, including how GCC reaches "after phase 7";
- whether profile rules apply in constant evaluation and in unevaluated operands;
- what a suppression means: the diagnostic muted, or the profile informed;
- the policy for an unknown profile;
- where `enforce` and `require` may appear around module and import declarations;
- the name of `PROFILED_STMT`.

## The design

**Built:** the framework, assembled by concern.

| Concern | What the compiler does |
|---|---|
| Parse | `-fprofiles`; `profiles::enforce`, `require`, `suppress` and `exempt` parsed inside the attribute-specifier-seq, so any spelling of the namespace works and they can share an attribute-list; the paper's grammar for names, designators and arguments; an unknown `profiles::` name is ignored with a warning |
| Record | the per-TU record of `enforce` designators, compared by token spelling, with the repetition rule |
| Placement | where each markup may appear: the `enforce` region, `require` only on imports, `exempt` in an attribute-declaration or on an otherwise empty statement |
| Exemptions | each exemption matched against the `#include` directives after it, as written; one after a matching `#include` is diagnosed |
| Suppressions | a `suppress` kept with what it appertains to: on a statement, in a `PROFILED_STMT` wrapping what the statement added (a placeholder, with a provisional name: genericization drops it); on a declaration or label, in `DECL_ATTRIBUTES` (the `profiles` namespace is registered under `-fprofiles`) |
| Modules | exported enforcements carried in the CMI; `require` checked at import, header units included |
| Plugin | a profile registry and hooks (`PLUGIN_PROFILES`), kept apart because plugin versus core is not settled |

**Under consideration:** the mechanism that tells a profile which code it governs. These are options from a draft design, not decisions; where a profile can act also depends on what GCC exposes to it, which for a plugin is only its plugin events.

- **enforced sets** per unit of origin: this TU's record, and each import's set from its CMI;
- **an include-origin test** for `exempt`: whether code came from an exempted `#include`, from the location's include chain;
- **one query**, roughly `profile_applies_p (profile, context, rule)`, so no profile walks the representation;
- for function bodies, profile checks **before genericization**, and runtime effects lowered to **internal-function markers**, materialised by gimplification at the latest. How to implement "after phase 7" for the rest of a TU (file-scope initializers, constant expressions, unevaluated operands), and what a plugin can reach of it, is an open question for GCC.

## The sequence of changes

| Step | What it does |
|---|---|
| trunk fix | c++: process attributes on non-type template parameters (they were dropped silently) |
| trunk fix | c++: diagnose attributes on module and import declarations (they were dropped silently) |
| 1 parse | `-fprofiles` and the grammar of the four markups; unknown `profiles::` names warned about; the `profiles.exp` driver, which each later step extends for its own tests |
| 2a record | the per-TU record of `enforce` designators and the repetition rule |
| 2b placement | where `enforce` and `require` may appear; a misplaced attribute diagnosed once |
| 2c exempt placement | where `exempt` may appear (the paper does not say) |
| 2d exempt record | exemptions recorded and matched against the `#include` directives as written |
| 2e modules | enforcements exported in the CMI, `require` checked at import |
| 2f suppress | a `suppress` kept with what it appertains to: a statement in a `PROFILED_STMT`, a declaration in `DECL_ATTRIBUTES` |
| 3 plugin | the profile registry and plugin interface |
| questions | the question tests, one per review item demonstrated |
| README | this file |

## Testing

```
make check-c++ RUNTESTFLAGS="profiles.exp"
```

`profiles.exp` runs the conformance tests, the module groups, the plugin test and, in a section of its own, the question tests. `obj-c++.dg/attributes/profiles-1.mm` (`make check-obj-c++ RUNTESTFLAGS="attributes.exp=profiles-1.mm"`) checks that the framework is also in the Objective-C++ front end. `-fdump-lang-profiles` shows what the framework recorded: each exemption and the `#include` directives it covers, each statement wrapped for a `suppress`, and each entity that keeps one.
