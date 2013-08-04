
    pathmatch: A directory/filename pattern match utility for the Windows command line


    DESCRIPTION

    'pathmatch' is a tool to match files and directories against a path pattern
    that supports '?', '*', and '...' wildcards. The '...' specifier matches any
    character, including slashes. For example, "foo...bar" will match
    "fooz/blaz/rebar". To find all files with a .obj extension, you'd give the
    path '....obj'. Very handy for locating files and directories, and for using
    as a command-line argument to the 'for' command.


    BUILDING

    This project requires Visual Studio to build, and generates a 64-bit console
    application.


--
Steve Hollasch <steve@hollasch.net>  /  2013 August 3
