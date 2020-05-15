pathmatch
================================================================================

A directory/filename pattern match utility for the Windows command line.


Description
------------
`pathmatch` is a tool to match files and directories against a path pattern that
supports `?`, `*`, and `...` wildcards. These wildcards match the following
patterns:

| Token | Description
|:-----:|:---------------------------------------------------------
| `?`   | Matches any single character
| `*`   | Matches zero or more of any character, except '/'.
| `**`  | Matches zero or more of any character, including '/'.
| `...` | Matches zero or more of any character, including '/'.


Examples
---------

#### `pathmatch a?c`
  Matches `abc`, `axc`, and `aac`. Does not match `ac`, `abbc`, or `a/c`.

#### `a*c`
  Matches "ac", "abc" and "azzzzzzzc". Does not match "a/c".

#### `foo...bar`
  Matches "foobar", "fooxbar", and "fooz/blaz/rebar". Does not match "fo/obar",
  "fobar" or "food/bark".

#### `pathmatch ....obj`
  Matches all files anywhere in the current hierarchy that end in ".obj". Note
  that the first three periods are interpreted as "...", and the fourth one is
  interpreted as a literal "." character.


Help Output
------------
```
pathmatch  v0.2.1  https://github.com/hollasch/pathmatch/
pathmatch: Report files and directories matching the specified pattern
Usage: pathmatch [<options>] <pattern> ... <pattern>

    `pathmatch` finds and reports all files and directories matching wildcard
    patterns. These patterns may contain the special characters '?', '*', and
    '...'. The '?' pattern matches any single character, '*' matches multiple
    characters except slashes, and '...' matches multiple characters including
    slashes. For example, the following patterns all match the file
    "abc\def\ghi\jkl": "abc\d?f\??i\jkl", "abc\*\*\jkl", "abc\...\jkl", and
    "ab...kl".

    The following command options are supported:

Command Options:
    --absolute, -a
        Report absolute paths. By default, reported paths are relative to the
        current working directory.

    --files, -f
        Report files only (no directories). To report directories only, append
        a slash to the pattern.

    --help, /?, -?, -h
        Print help information.

    --slash [/|\], -s[/|\]
        Specifies the slash direction to be reported. By default, slashes will
        be back slashes. Use "-s/" to report paths with forward slashes, "-s\"
        to report backslashes. A space is allowed before the slash.

    --version, -v
        Print version information.

    --preview
        Print preview of planned command options.
```


Building
----------
This project uses the CMake build tool. CMake is a meta-build system that
locates and uses your local development tools to build the project if possible.

To build, first install [CMake][https://cmake.org/]. Then go to the project root
directory and run the following command:

    cmake -B build

This will locate your installed development tools and configure your project
build in the `build/` directory. After that, whenever you want a new build, run
this command:

    cmake --build build

This will build a debug version of the project, located in `build/Debug/`. To
build a release version, run

    cmake --build build --config release

You can find the built release executable in `build/Release/`.


----
Steve Hollasch <steve@hollasch.net>
https://github.com/hollasch/pathmatch
