# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

**learnuv** is an interactive workshop for learning LibUV (the async I/O library powering Node.js). Exercises are C programs in `src/` that students implement; the workshopper framework verifies their output.

## Setup

```sh
npm install
pyenv install 2.7 && pyenv local 2.7.18   # GYP requires Python 2.7
pip install six
eval "$(pyenv init -)"
```

## Commands

```sh
./learnuv               # interactive exercise menu
./learnuv make          # build all exercises with Make
./learnuv ninja         # build with Ninja
./learnuv xcode         # generate Xcode project
./learnuv verify        # verify the currently selected exercise
./learnuv clean         # clean build artifacts
```

To build a single exercise, use GYP directly or just run `./learnuv make` and execute the specific binary from `out/Release/` or `out/Debug/`.

## Exercise Workflow

1. Select an exercise via `./learnuv` (saves state to `~/.config/learnuv/current.json`)
2. Edit the corresponding `src/<exercise_name>.c`
3. Build with `./learnuv make`
4. Run the compiled binary — output is written to `~/.config/learnuv/<exercise_name>.c`
5. Verify with `./learnuv verify` (compares output against `exercises/<name>/expected.txt`)

## Architecture

**Build**: GYP-based (`learnuv.gyp`) cross-platform build. Builds all exercise targets plus `libuv_sandbox` and `epam_workshop`. The `__MAGIC_FILE__` and `__LEARNUV_CONFIG__` preprocessor defines are injected by GYP and point to `magic/file.txt` and the user's config dir.

**C exercises** (`src/*.c`): Each exercise includes `learnuv.h` which provides:
- `CHECK(r, msg)` — error macro for libuv return codes
- `log_report(fmt, ...)` — writes formatted output to `~/.config/lebuv/<exercise>.c` for verification

**Workshopper** (`exercises/<name>/`): Each exercise folder contains `problem.md` (instructions), `exercise.js` (validation logic), `expected.txt` (expected output), and `solution.c`.

**Dependencies** (in `deps/`, git-cloned by `scripts/install-dependencies.py`):
- `libuv` v1.7.5
- `log.h` logging library
- `build/gyp` build system

## Notes

- Python 2.7 is required (not 3.x) because the bundled GYP version requires it
- `.clang_complete` provides include paths for clang-based editor tooling
- The `magic/file.txt` file is read by filesystem exercises as their target file
