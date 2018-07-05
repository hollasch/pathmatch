Change Log -- `pathmatch`
================================================================================

0.2.0-alpha5  (In Progress)
--------------------------------------------------------------------------------
### New
  - Expanded usage information. Now includes future options under development.
  - New support for `--word` option style.
  - Stubbed in support for new options: `--debug`, `--dirSlash`, `--stream`, and
    `--ignore`.

### Changed
  - Changed to MIT license
  - Uses new C++ std::filesystem class for portable file system access. Removed
    the prior code that used a custom filesystem proxy.


0.1.1 (2017-10-08)
--------------------------------------------------------------------------------
### Added
  - First semver version.
  - Retargetted to Win10
  - Fix crashing bug with patterns consisting only of slashes

