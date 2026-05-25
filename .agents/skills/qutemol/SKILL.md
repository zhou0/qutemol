```markdown
# qutemol Development Patterns

> Auto-generated skill from repository analysis

## Overview
This skill teaches you the key development patterns, coding conventions, and workflows used in the `qutemol` TypeScript codebase. You'll learn how to structure files, write imports/exports, follow commit practices, and maintain the build system and CI configuration. This guide also covers how to write and locate tests, and provides handy commands for common workflows.

## Coding Conventions

### File Naming
- **PascalCase** is used for file names.
  - Example: `MoleculeRenderer.ts`, `AtomModel.ts`

### Import Style
- **Relative imports** are preferred.
  - Example:
    ```typescript
    import { Atom } from './Atom';
    import { MoleculeRenderer } from '../render/MoleculeRenderer';
    ```

### Export Style
- **Named exports** are used instead of default exports.
  - Example:
    ```typescript
    // Atom.ts
    export interface Atom {
      element: string;
      position: [number, number, number];
    }
    ```

### Commit Patterns
- **Freeform commit messages** (no strict prefixes)
- **Average length:** ~55 characters

  Example:
  ```
  Fix rendering bug when loading large molecules
  ```

## Workflows

### Update Build System and CI
**Trigger:** When updating build dependencies, build scripts, or fixing CI issues related to building the project.  
**Command:** `/update-build-system-ci`

1. **Modify `CMakeLists.txt`**  
   Update build instructions or dependencies as needed.
   ```cmake
   # Example: Add a new dependency
   find_package(OpenGL REQUIRED)
   ```
2. **Update `.github/workflows/cmake-multi-platform.yml`**  
   Reflect any changes in build requirements or CI steps.
   ```yaml
   - name: Install dependencies
     run: sudo apt-get install libopengl-dev
   ```
3. **Test CI**  
   Push your changes and verify that builds succeed on all platforms via GitHub Actions.

## Testing Patterns

- **Test files** follow the pattern: `*.test.*`
  - Example: `MoleculeRenderer.test.ts`
- **Testing framework:** Not explicitly detected; check the test files for framework usage.
- **Typical test structure:**
  ```typescript
  import { renderMolecule } from './MoleculeRenderer';

  test('renders molecule correctly', () => {
    // Test implementation
  });
  ```

## Commands
| Command                | Purpose                                                        |
|------------------------|----------------------------------------------------------------|
| /update-build-system-ci| Sync build scripts and CI configuration after relevant changes |
```
