# PixelEd — Project Guidelines

## What This Project Is

PixelEd is a **Qt/C++ 2D game editor** — think a very primitive Unreal Editor — for the bundled **PixelEngine** game engine. The engine targets simple pixel-based 2D games (think Scorched Earth, Worms) and runs cross-platform via Qt (GNU/Linux, macOS, Windows, WebAssembly). The project is in early development: the engine library is functional but the editor UI is almost entirely a stub.

The repo contains two separate products that live side-by-side:

| Product | Target | Location |
|---------|--------|----------|
| **PixelEngine** | Shared library (`libpixelengine`) | `src/PixelEngine/` |
| **PixelEd** | Editor executable (`pixeled`) | `src/PixelEd/` |

Plus two standalone test/demo applications under `src/PixelEngine/tests/`.

---

## Repository Layout

```
configure                        # Bash configure script (see Build section)
src/
  CMakeLists.txt                 # Root CMake: orchestrates all targets
  PixelEd/                       # Editor application (Qt Widgets)
    CMakeLists.txt
    main.cpp                     # Entry point — creates MainWindow, shows maximized
    mainwindow.h / .cpp / .ui    # Sole UI class; viewport + placeholder menus
  PixelEngine/
    README.md                    # Engine purpose & motivation
    src/PixelEngine/             # Engine library source root
      CMakeLists.txt
      definitions.h              # Global constants, PE_VERSION_STRING, pe_float_t
      vector.h / .cpp            # 2D vector math (PE::Vector)
      pemath.h / .cpp            # Misc math utilities
      generic.h / .cpp           # String helpers
      engine.h / .cpp            # Singleton Engine — initialise GC, ring log
      object.h / .cpp            # Base scene object (position, parent/child, render)
      actor.h / .cpp             # Physics-enabled object (rigidbody, colliders)
      pawn.h / .cpp              # Controllable actor (placeholder)
      camera.h / .cpp            # View/projection (world→screen coordinate mapping)
      world.h / .cpp             # World container — physics loop, rendering, input
      worldgenerator.h / .cpp    # Procedural terrain generation
      terrain.h / .cpp           # Destructible bitmap terrain
      resources.h / .cpp         # Resource manager (stub)
      ringlog.h / .cpp           # Circular-buffer logger (2000 entries)
      ringlog_item.h / .cpp      # Single log entry with timestamp
      exception.h / .cpp         # Base exception class
      nullpointerexception.h/.cpp
      GC/                        # Garbage collection subsystem
        collectable.h            # Reference-counted base; global Collectables list
        collectable_smartptr.h   # Template smart pointer for all GC-managed objects
        gc.h                     # GC driver — single-threaded or multi-threaded
        gc_thread.h              # QThread background collector
      Physics/                   # Collision subsystem
        collider.h               # Abstract collider base (layer, PositionMatch, IntersectionMatch)
        boxcollider.h            # AABB box collider
        circlecollider.h         # Circle collider
        pixelcollider.h          # Single-point collider
        ellipticcollider.h       # Rotatable ellipse collider
        bitmapcollider.h         # Per-pixel bitset collider (used by Terrain)
        rigidbody.h              # Physics body (velocity, weight, bounciness, ground contact)
        collisionignorematrix.h  # 64-layer bitset filter
      Graphics/                  # Rendering subsystem
        renderer.h               # Abstract renderer interface (Clear, DrawPixel/Rect/…)
        qimagerenderer.h         # CPU renderer — QPainter on QImage
        qglrenderer.h            # GPU renderer — QOpenGLFunctions + texture blitter
        peglwidget.h             # QOpenGLWidget wrapper with world + FPS
        peglwindow.h             # QOpenGLWindow alternative
        pixmaptools.h            # Sprite-sheet loading, rotation utilities
      Objects/
        sprite.h                 # Sprite scene object (minimal/placeholder)
    tests/
      CMakeLists.txt
      single_thread/             # Demo app — CPU renderer (QImageRenderer)
        game.h / .cpp            # Game controller: startGame(), test1(), test2()
        mainwindow.h / .cpp      # MainWindow wiring for CPU render path
        console.h / .cpp         # Ring-log viewer dialog
        objects/                 # Test actors: c1.h (cube), r1.h (rect), p1.h (particle)
      open_gl/                   # Demo app — GPU renderer (PEGLWindow)
        game.h / .cpp
        mainwindow.h / .cpp
        console.h / .cpp
        objects/
```

---

## Architecture

### Object Hierarchy

```
PE::Object          — position, scale, parent/child tree, RedrawNeeded, Destroy()
  PE::Actor         — adds Rigidbody*, collider list, Update(), key/impact events
    PE::Pawn        — controllable actor (stub)
  PE::Terrain       — destructible bitmap (DestroyPixel, ShiftFloatingBitsDown)
  PE::Sprite        — sprite object (stub)
```

All engine objects must be managed through `Collectable_SmartPtr<T>`; never hold raw pointers to `Collectable`-derived objects.

### Memory / GC

- Every class that needs automatic lifetime management inherits `PE::Collectable`.
- `Collectable_SmartPtr<T>` is the project's smart pointer — prefer it over `std::shared_ptr` for GC-tracked objects.
- `Engine::Initialize(bool multithreaded_gc)` must be called before any world is created.
- In WebAssembly (`__EMSCRIPTEN__`) the GC must be polled manually from the game loop; it cannot run on a background thread.

### Rendering Backends

Selected at **compile time** via the `PIXELED_GL` preprocessor define:

| Define absent | `PIXELED_GL` defined |
|---------------|----------------------|
| `PE::QImageRenderer` (CPU, QPainter) | `PE::QGLRenderer` (GPU, OpenGL) |
| Uses `QImage` pixel buffer | Uses `QOpenGLTextureBlitter` |

Both backends implement the same `PE::Renderer` abstract interface, so engine objects call `Renderer::DrawPixel/Rect/Line/Bitmap/Text` without knowing which backend is active.

### Physics / Collision

- `World::Update()` runs one physics tick: `updateGravity()` + `updateMovement()`.
- Collision is resolved between registered `Collider` objects; ignored pairs are filtered through `CollisionIgnoreMatrix` (64 layers, bitset indexed).
- `Terrain` uses `BitmapCollider` — a per-row `std::bitset<PE_BITSET_MAX>` — for pixel-accurate destruction.

---

## Build System

### Quick Start

```sh
# Debug build with Qt6
./configure --debug

# With unit-test apps
./configure --debug --unit-test

# Select Qt version explicitly
./configure --qt5
./configure --qt6
```

The `configure` script detects the platform, runs `cmake` into `src/build/<profile>/`, and produces:
- `src/build/.../bin/pixeled` — editor executable
- `src/build/.../libpixelengine` — engine shared library

### CMake Targets

| Target | Type | Source |
|--------|------|--------|
| `PixelEngine` | Shared library | `src/PixelEngine/src/PixelEngine/` |
| `PixelEd` | Executable | `src/PixelEd/` |
| `single_thread` (optional) | Executable | `src/PixelEngine/tests/single_thread/` |
| `open_gl` (optional) | Executable | `src/PixelEngine/tests/open_gl/` |

C++ standard is **C++11**. Qt5 is the default; Qt6 is selected with `--qt6` or by setting `PIXELED_QT6=ON`.

---

## Naming & Code Conventions

| Category | Pattern | Example |
|----------|---------|---------|
| Namespace | `PE::` for all engine code | `PE::World`, `PE::Actor` |
| Classes | `PascalCase` | `BoxCollider`, `QImageRenderer` |
| Enums | `PE_EnumName_Value` | `PE_ObjectType_Actor`, `PE_ColliderType_Box` |
| Constants / macros | `UPPER_SNAKE_CASE` with `PE_` prefix | `PE_MAX_CM_SIZE`, `PE_RING_LOG` |
| Private members | `_camelCase` | `_refCount` |
| Public members | `PascalCase` | `Position`, `Velocity` |
| Logging | `PE_WRITE_RING(text)` macro | writes to engine ring log |
| Header comments | `//!` Doxygen-style | present on all public members |

All source files carry **LGPLv3** license headers.

---

## Current State (What's Done vs. Stub)

### Engine library — functional
- Physics loop (gravity, velocity, collision dispatch)
- Dual-renderer architecture (CPU + OpenGL)
- Destructible terrain with bitmap collider
- Reference-counted GC (single- and multi-threaded)
- Input event routing (keyboard → World → Actors)
- Ring-log debug console
- Procedural terrain generation (`WorldGenerator`)

### Editor (PixelEd) — mostly stub
- `MainWindow` renders the engine viewport (world can be set programmatically)
- Menu bar has File/Edit items (New, Open, Save, Preferences, Exit) **with no handlers connected**
- No property panels, no asset browser, no scene tree, no level editor
- No save/load system

### Planned editor features (not yet started)
- GUI widgets for placing/editing world objects and actors
- Asset/resource manager UI
- Level serialisation (save/load)
- Property inspector panel

---

## Key Files to Know

| File | Why it matters |
|------|----------------|
| `src/PixelEngine/src/PixelEngine/world.h` | Central physics + render loop; most engine work touches this |
| `src/PixelEngine/src/PixelEngine/object.h` | Base class for everything renderable |
| `src/PixelEngine/src/PixelEngine/GC/collectable_smartptr.h` | Smart pointer used everywhere |
| `src/PixelEngine/src/PixelEngine/Graphics/renderer.h` | Renderer interface — target for all draw calls |
| `src/PixelEd/mainwindow.h` | Only editor UI class; start here when adding editor features |
| `src/PixelEngine/tests/single_thread/game.cpp` | Best working example of the engine in use |
| `configure` | Entry point for building; controls Qt version and test targets |
| `src/CMakeLists.txt` | Root CMake; defines all targets and dependencies |
