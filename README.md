pathmatch
================================================================================

A directory/filename pattern match utility for the Windows command line.


Description
---------------

`pathmatch` is a tool to match files and directories against a path pattern that
supports `?`, `*`, and `...` wildcards. These wildcards match the following
patterns:

| Token | Description
|:-----:|:---------------------------------------------------------
| `?`   | Matches any single character
| `*`   | Matches zero or more of any character, except '/'.
| `...` | Matches zero or more of any character, including '/'.


Examples
-----------

    |  Sequence   | Result                                                        |
    |-------------|---------------------------------------------------------------|
    | `a?c`       | Matches `abc`, `axc`, and `aac`. Does not match `ac`, `abbc`, | 
    |             | or `a/c`.                                                     |
    |-------------|---------------------------------------------------------------|
    | `a*c`       | Matches "ac", "abc" and "azzzzzzzc". Does not match "a/c".    |
    |-------------|---------------------------------------------------------------|
    | `foo...bar` | Matches "foobar", "fooxbar", and "fooz/blaz/rebar". Does not  |
    |             | match "fo/obar", "fobar" or "food/bark".                      |
    |-------------|---------------------------------------------------------------|
    | `....obj`   | Matches all files anywhere in the current hierarchy that end  |
    |             | in ".obj". Note that the first three periods are interpreted  |
    |             | as "...", and the fourth one is interpreted as a literal "."  |
    |             | character.                                                    |
    |-------------|---------------------------------------------------------------|


pathmatcher.lib
-------------------

In addition to the `pathmatch.exe` tool, this project creates the
`PathMatch.lib` library. This library includes C++ header files `pathmatcher.h`
and `wildcomp.h`. `Pathmatcher` and `wildcomp` are classes the provide wildcard
matching capabilities.


Building
-----------

This project requires Visual Studio to build, and generates the 64-bit console
application and 64-bit static library.

For the time being, this project includes a clone of the FileSystemProxy project
as a sub-directory. To set this up before building, run the following command:

```
git clone https://github.com/hollasch/FileSystemProxy.git ext/FileSystemProxy
```


----
Steve Hollasch <steve@hollasch.net>
