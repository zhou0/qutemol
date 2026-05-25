---
name: update-build-system-and-ci
description: Workflow command scaffold for update-build-system-and-ci in qutemol.
allowed_tools: ["Bash", "Read", "Write", "Grep", "Glob"]
---

# /update-build-system-and-ci

Use this workflow when working on **update-build-system-and-ci** in `qutemol`.

## Goal

Keeps the build system and CI configuration in sync, ensuring that changes to build scripts are reflected in CI workflows and vice versa.

## Common Files

- `CMakeLists.txt`
- `.github/workflows/cmake-multi-platform.yml`

## Suggested Sequence

1. Understand the current state and failure mode before editing.
2. Make the smallest coherent change that satisfies the workflow goal.
3. Run the most relevant verification for touched files.
4. Summarize what changed and what still needs review.

## Typical Commit Signals

- Modify CMakeLists.txt to update build instructions or dependencies.
- Update .github/workflows/cmake-multi-platform.yml to reflect changes in build requirements or CI steps.
- Test CI to ensure builds succeed on all platforms.

## Notes

- Treat this as a scaffold, not a hard-coded script.
- Update the command if the workflow evolves materially.