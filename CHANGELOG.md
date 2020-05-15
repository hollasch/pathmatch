Change Log -- `pathmatch`
================================================================================

0.2.0-alpha5  (In Progress)
--------------------------------------------------------------------------------
### Major

### Minor
  - New support for `--word` option style.
  - Stubbed in support for new options: `--debug`, `--dirSlash`, `--stream`, and
    `--ignore`.
  - Changed to MIT license

### Patch
  - Expanded usage information. Now includes future options under development.
  - Overall modernization of the C++ code.
  - Uses new C++ std::filesystem class for portable file system access. Removed
    the prior code that used a custom filesystem proxy.
  - Converted project to use CMake


0.1.1  (2017-10-08)
--------------------------------------------------------------------------------
### Minor
  - First semver version.
  - Retargetted to Win10
  - Fix crashing bug with patterns consisting only of slashes

