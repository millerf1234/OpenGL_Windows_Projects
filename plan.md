# Plan: Fix C++ Build Tools Upgrade Warnings

Solution: C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project.sln

Tasks

TASK-001: Fix format specifiers and unreferenced label in AssetLoadingDemo.cpp
- Files: C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\AssetLoadingDemo.cpp
- Actions:
  1. Replace '%u' with '%llu' and cast `loadedModlCount` to `unsigned long long` when calling `fprintf`.
  2. Remove or comment out the unreferenced label `SKIP_QUADTEXTURE_TEST_SHADER`.
  3. Clean ASCII art comments to avoid C5333 by removing trailing spaces after backslashes.

TASK-002: Fix TGASDK inconsistent dll linkage warnings
- Files:
  - C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\TGASDK\include\TGA.h
  - C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\TGASDK\sources\TGA.cpp
  - C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\TGASDK\include\TGAHeader.h
  - C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\TGASDK\sources\TGAHeader.cpp
- Actions:
  1. Ensure headers declare export/import macros like `TGASDK_API` and use `__declspec(dllexport)` when building the library and `__declspec(dllimport)` when consuming. Add macro or include appropriate header defining it.
  2. Ensure source files define the appropriate symbol (e.g., when building the static exe, define TGASDK_STATIC to avoid dllexport/import).
  3. Rebuild and verify warnings are resolved.

TASK-003: Fix implicitly-deleted destructors in RawImageData.h
- File: C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\RawImageData.h
- Actions:
  1. Inspect the union/struct members that cause non-trivial destruction (e.g., objects with non-trivial destructors). Make members pointers or add explicit destructors/defaulted as appropriate.
  2. Define defaulted destructors where needed (e.g., `~RawImageData() = default;`) if safe.

TASK-004: Fix TGAVariable.cpp narrowing warning
- File: C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\TGASDK\sources\TGAVariable.cpp
- Actions:
  1. Ensure variables passed to `read` match expected types (use `size_t` or cast explicitly with bounds checks).

TASK-005: Investigate missing XDC files and XML doc warnings
- Files:
  - C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\Debug\debug_glad.xdc
  - C:\Users\4estm\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\Debug\glad.xdc
  - glm.hpp, alc.h
- Actions:
  1. Confirm whether the XDC files are required; if missing, either update project references or remove references.
  2. Suppress C4635 for third-party headers or patch locally if acceptable.

Validation
- After each task, run `cppupgrade_rebuild_and_get_issues` to validate changes.
- Ensure no new warnings/errors are introduced compared to the out-of-scope list.

Approval
- Confirm you want me to proceed with applying the in-scope fixes. I will create a new branch and apply changes, then iterate rebuild/validate until resolved.
