//==================================================================================================
// pathmatch.cpp
//
// This program returns all files and directories matching the specified pattern. See the usage
// string below for a description of its behavior.
//
// _________________________________________________________________________________________________
// Copyright 2010-2021 Steve Hollasch
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//==================================================================================================

#include "pathmatcher.h"

#include <filesystem>
#include <iostream>
#include <string>

using namespace PathMatch;
namespace fs = std::filesystem;

using std::vector;
using std::wstring;
using std::wcout;
using std::wcerr;


namespace { // File-local Variables & Parameters

const wstring versionString = L"pathmatch 0.2.2 | 2021-04-27 | https://github.com/hollasch/pathmatch";

    // Usage Information

const wstring usageString = LR"(
pathmatch: Report files and directories matching the specified pattern
usage    : pathmatch [<options>] <pattern> ... <pattern>

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

)";

const wstring usagePreview =
LR"(Preview of Planned Options:

    File Patterns
        - The pattern '**' is a synonym for '...'.
        - The pattern '{a,b,c}' matches 'a', or 'b', or 'c'. Subparts may
          themselves be patterns.

    --breadthFirst
        Print results in breadth-first fashion (shallow entries before deep
        ones). By default, results are printed depth-first (all entries under
        one directory before proceeding to the next one).

    --debug, -D
        Turn on debugging output.

    --limit <count>, -l<count>
        Limit output to the first <count> matches.

    --maxDepth <depth>, -m<depth>
        Limit the descent depth to <depth> levels. Level 0 is files in current
        directory only, level 1 is entries of each subdirectory, and so on.

    --dirSlash, -d
        Print trailing slash for directory matches.

    --stream <fileName>|( <file1> <file2> ... <fileN> )
        Apply patterns against input stream of filenames. The special filename
        '--' reads filenames from standard input, and may be specified for a
        single option only. The multiple file option requires space-separated
        '(' and ')' delimiters.

    --ignore <fileName>|( <file1> <file2> ... <fileN> )
        Suppress output of files and directories that match rules inside a file
        of patterns. The special filename '--' reads from standard input, and
        may be specified for a single option only. The multiple file option
        requires space-separated '(' and ')' delimiters
)";

struct CommandParameters
{
    // The CommandParameters structure holds the options for use by the callback routine.

    bool printHelp {false};        // Print help and exit.
    bool printPreview {false};     // Print preview help and exit.
    bool printVersion {false};     // Print version information and exit.

    bool debug {false};            // Print debug information

    bool    dirSlash {false};      // Print directories with trailing slashes
    wchar_t slashChar {L'/'};      // Forward or backward slash character to use
    bool    absolute {false};      // If true, report absolute path rather than default relative
    bool    filesOnly {false};     // If true, report only files (not directories)
    int     limit {0};             // If positive, then maximum number of matches to print, else unlimited
    size_t  maxPathLength {0};     // Maximum path length

    vector<wstring> streamSources; // Source of file paths to match
    vector<wstring> ignoreFiles;   // Files with patterns to ignore
    vector<wstring> patterns;      // Patterns to match
};


inline bool isSlash (wchar_t c) {
    // Return true if the given character is a forward or backward slash.
    return (c == L'/') || (c == L'\\');
}

inline bool equal (const wchar_t* a, const wchar_t* b) {
    // Return true if the two wchar_t strings are identical.
    return 0 == _wcsicmp(a, b);
}
};


//--------------------------------------------------------------------------------------------------
bool parseArguments (CommandParameters& commandParams, int argc, wchar_t *argv[])
{
    // Parse arguments from the command line and set the given CommandParameters structure with the
    // resulting values. This function returns true if no errors were encountered, otherwise false.

    if (argc <= 1) {
        commandParams.printHelp = true;
        return true;
    }

    // Cycle through all command-line arguments.

    for (size_t argi=1;  argi < argc;  ++argi) {

        auto arg = argv[argi];

        // The argument "/?" is a special case. While it's technically a valid file system pattern,
        // we treat it as a request for tool information by convention (if it's the first argument).

        if (equal(arg, L"/?")) {
            commandParams.printHelp = true;
            return true;
        }

        if (arg[0] != L'-') {

            commandParams.patterns.push_back (argv[argi]);

            // Patterns

        } else {

            if (arg[1] != L'-') {

                // Single letter command options

                switch (arg[1]) {
                    case L'h': case L'H': case L'?': {
                        commandParams.printHelp = true;
                        return true;
                    }

                    case L'a': { commandParams.absolute = true;     break; }
                    case L'd': { commandParams.dirSlash = true;     break; }
                    case L'D': { commandParams.debug = true;        break; }
                    case L'f': { commandParams.filesOnly = true;    break; }
                    case L'v': { commandParams.printVersion = true; return true; }

                    case L'l': { // Limit option
                        auto limitString = arg+2;
                        if (!*limitString) {
                            if (++argi >= argc) {
                                wcerr << L"pathmatch: Expected numeric argument to '-l' option.\n";
                                return false;
                            }
                            limitString = argv[argi];
                        }
                        commandParams.limit = std::max(0, _wtoi(limitString));
                        break;
                    }

                    case L's': { // Slash type option
                        auto slashString = arg+2;
                        if (!slashString[0]) {
                            if (++argi >= argc) {
                                wcerr << L"pathmatch: Expected slash character after '-s' option.\n";
                                return false;
                            }
                            slashString = argv[argi];
                        }

                        if (slashString[1] != 0) {
                            wcerr << L"pathmatch: Expected single character for -s option, got '"
                                  << slashString << L"'.\n";
                            return false;
                        }

                        auto slashChar = slashString[0];
                        if ((slashChar != L'/') && (slashChar != L'\\')) {
                            wcerr << L"pathmatch: Invalid '-s' option (\"" << slashChar << L"\").\n";
                            return false;
                        }

                        commandParams.slashChar = slashChar;
                        break;
                    }
                }

            } else {

                auto optionWord = argv[argi] + 2;

                // Double dash word options
                if (equal(optionWord, L"absolute")) {
                    commandParams.absolute = true;

                } else if (equal(optionWord, L"debug")) {
                    commandParams.debug = true;

                } else if (equal(optionWord, L"dirSlash")) {
                    commandParams.dirSlash = true;

                } else if (equal(optionWord, L"files")) {
                    commandParams.filesOnly = true;

                } else if (equal(optionWord, L"stream")) {
                    if (++argi >= argc) {
                        wcerr << L"pathmatch: missing argument for '--stream' option.\n";
                        return false;
                    }
                    if (!equal(argv[argi], L"(")) {
                        commandParams.streamSources.push_back(argv[argi]);
                    } else {
                        for (++argi;  argi < argc && !equal(argv[argi],L")");  ++argi) {
                            commandParams.streamSources.push_back(argv[argi]);
                        }
                    }

                } else if (equal(optionWord, L"limit")) {
                    if (++argi >= argc) {
                        wcerr << L"pathmatch: missing argument for '--limit' option.\n";
                        return false;
                    }
                    commandParams.limit = std::max(0, _wtoi(argv[argi]));

                } else if (equal(optionWord, L"ignore")) {
                    if (++argi >= argc) {
                        wcerr << L"pathmatch: missing argument for '--ignore' option.\n";
                        return false;
                    }
                    if (!equal(argv[argi], L"(")) {
                        commandParams.ignoreFiles.push_back(argv[argi]);
                    } else {
                        for (++argi;  argi < argc && !equal(argv[argi],L")");  ++argi) {
                            commandParams.ignoreFiles.push_back(argv[argi]);
                        }
                    }

                } else if (equal(optionWord, L"help")) {
                    commandParams.printHelp = true;
                    return true;

                } else if (equal(optionWord, L"preview")) {
                    commandParams.printPreview = true;
                    return true;

                } else if (equal(optionWord, L"slash")) {
                    if (++argi >= argc) {
                        wcerr << L"pathmatch: missing argument for '--slash' option.\n";
                        return false;
                    }
                    if (wcslen(argv[argi]) > 1) {
                        wcerr << L"pathmatch: Expected single character for slash option, got '"
                              << argv[argi] << L"'.\n";
                        return false;
                    }
                    auto slashChar = argv[argi][0];
                    if (slashChar != L'\\' && slashChar != '/') {
                        wcerr << L"pathmatch: Expected slash character after '--slashChar' option, got '"
                              << slashChar << L".\n";
                        return false;
                    }
                    commandParams.slashChar = argv[argi][0];

                } else if (equal(optionWord, L"version")) {
                    commandParams.printVersion = true;
                    return true;

                } else {
                    wcerr << L"pathmatch: Unrecognized option (--" << optionWord << L").\n";
                    return false;
                }
            }
        }
    }

    return true;
}


//--------------------------------------------------------------------------------------------------
void printWordList (const vector<wstring> wordList)
{
    if (wordList.empty()) {
        wcout << L"<empty>";
        return;
    }

    bool firstItem = true;

    for (auto v: wordList) {
        if (firstItem) {
            firstItem = false;
        } else {
            wcout << L", ";
        }
        wcout << v;
    }
}


//--------------------------------------------------------------------------------------------------
void printParameters (const CommandParameters& params)
{
    auto boolValue = [](bool b) { return b ? L"true\n" : L"false\n"; };

    wcout << L"Command Parameters:\n";
    wcout << L"       printHelp: " << boolValue(params.printHelp);
    wcout << L"    printPreview: " << boolValue(params.printPreview);
    wcout << L"    printVersion: " << boolValue(params.printVersion);
    wcout << L"           debug: " << boolValue(params.debug);
    wcout << L"        dirSlash: " << boolValue(params.dirSlash);
    wcout << L"        absolute: " << boolValue(params.absolute);
    wcout << L"       filesOnly: " << boolValue(params.filesOnly);
    wcout << L"       slashChar: " << params.slashChar << L'\n';
    wcout << L"           limit: " << params.limit << L'\n';
    wcout << L"   maxPathLength: " << params.maxPathLength << L'\n';
    wcout << L"     ignoreFiles: "; printWordList(params.ignoreFiles); wcout << L'\n';
    wcout << L"   streamSources: "; printWordList(params.streamSources); wcout << L'\n';
    wcout << L"        patterns: "; printWordList(params.patterns); wcout << L'\n';
    wcout << L'\n';
}


//--------------------------------------------------------------------------------------------------
bool mtCallback (
    const fs::path& path,
    const fs::directory_entry& dirEntry,
    void* cbdata)
{
    // This is the callback function for the PathMatcher object.
    //
    // Parameters
    //     path ........ The full path of the matching file or directory
    //     dirEntry .... The directory entry for the matching file or directory
    //     cbdata ...... Pointer to the report options
    //
    // Returns
    //     True to continue fetching matching entries, unless the routine
    //     encountered and error attempting to convert a path to a full path.
    //--------

    // Get the properly typed report options structure from the callback data.

    auto commandParams = static_cast<const CommandParameters*>(cbdata);

    auto absolute = new wchar_t [commandParams->maxPathLength + 1];  // Optional Full Path
    auto item = path;       // Pointer to Matching Entry

    // If we are to report only files and this entry is a directory, then return without reporting.

    auto isDirectory = fs::is_directory(path);
    if (commandParams->filesOnly && isDirectory)
        return true;

    // TODO: Handle absolute and relative paths (reportOpts->absolute).
    // TODO: Handle desired slash character (reportOpts->slashChar).

    wcout << path.wstring() << L'\n';

    #if 0
    if (!commandParams->absolute)
        item = path;
    else
    { // If we are to convert the default relative path to a full path, use the stdlib _fullpath
        // function to do so. If this is not possible, then emit an error message and halt matching
        // entry enumeration.

        if (!_wfullpath(absolute, entry, commandParams->maxPathLength))
        {
            wcerr << L"pathmatch: Unable to convert \""
                  << entry
                  << L"\" to absolute path.\n";
            return false;
        }

        item = fs::path(absolute);
    }

    // Print out the matching item, converted to the requested slash type.

    for (const wchar_t *ptr = item;  *ptr;  ++ptr)
    for (auto wchar_t : item
        wcout << (isSlash(*ptr) ? commandParams->slashChar : *ptr);

    wcout << L'\n';
    #endif

    return true;   // Continue enumeration.
}


//--------------------------------------------------------------------------------------------------
#ifndef MS_STDLIB_BUGS
    #if ( _MSC_VER || __MINGW32__ || __MSVCRT__ )
        #define MS_STDLIB_BUGS 1
    #else
        #define MS_STDLIB_BUGS 0
    #endif
#endif

#if MS_STDLIB_BUGS
    #include <io.h>
    #include <fcntl.h>
#endif

void initLocale()
{
    // Set up Unicode IO according to environment.

    #if MS_STDLIB_BUGS
        constexpr char cp_utf16le[] = ".1200";
        setlocale( LC_ALL, cp_utf16le );
        _setmode( _fileno(stdout), _O_WTEXT );
    #else
        // The correct locale name may vary by OS, e.g., "en_US.utf8".
        constexpr char locale_name[] = "";
        setlocale( LC_ALL, locale_name );
        std::locale::global(std::locale(locale_name));
        std::wcin.imbue(std::locale());
        std::wcout.imbue(std::locale());
    #endif
}

//--------------------------------------------------------------------------------------------------
int wmain (int argc, wchar_t *argv[])
{
    initLocale();

    PathMatcher matcher;  // PathMatcher Object

    CommandParameters commandParams;

    if (!parseArguments(commandParams, argc, argv))
        exit(1);

    if (commandParams.debug) {
        printParameters(commandParams);
    }

    if (commandParams.printHelp) {
        wcout << usageString << versionString << L'\n';
        exit(0);
    }

    if (commandParams.printPreview) {
        wcout << usagePreview;
        exit(0);
    }

    if (commandParams.printVersion) {
        wcout << versionString << L'\n';
        exit(0);
    }

    for (auto pattern: commandParams.patterns) {
        matcher.match (pattern, &mtCallback, &commandParams);
    }

    exit (0);
}
