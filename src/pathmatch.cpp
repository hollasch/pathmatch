//==================================================================================================
// pathmatch.cpp
//
// This program returns all files and directories matching the specified pattern. See the usage
// string below for a description of its behavior.
//
// _________________________________________________________________________________________________
// Copyright 2010 Steve Hollasch
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

using std::wstring;
using std::wcout;
using std::wcerr;

static const wstring version = L"0.2.4-beta";

    // Usage Information

static const wstring usage_header {
    L"pathmatch  v" + version + L"  https://github.com/hollasch/pathmatch/"
};

static const wstring usage =
LR"(pathmatch: Report files and directories matching the specified pattern
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
    /?, -h, -?
        Print help information.

    -a
        Report absolute paths. By default, reported paths are relative to the
        current working directory.

    -f
        Report files only (no directories). To report directories only, append
        a slash to the pattern.

    -s<slash>
        Specifies the slash direction to be reported. By default, slashes will
        be back slashes. Use "-s/" to report paths with forward slashes.

    -v
        Print version information.

Future Options:
    --absolute
        Equivalent to the -a option.

    --debug, -d
        Turn on debugging output.

    --dirSlash
        Print matching directories with a trailing slash.
    
    --files
        Equivalent to the -f option.

    --stream <fileName>|--
        Apply patterns against input stream of filenames. The special filename
        '--' reads filenames from standard input. '--' may be specified for a
        single option only.
    
    --ignore <fileName>|--
        Suppress output of files and directories that match rules inside a file
        of patterns. '--' may be specified for a single option only.

    --help
        Equivalent to the -h option.

    --slash<slash>
        Equivalent to the -s option.
    
    --version
        Equivalent to the -v option.
)";

enum class OptionType {
    AbsolutePath,
    Debug,
    DirectorySlash,
    FilesOnly,
    Help,
    Ignore,
    SlashType,
    Stream,
    PrintVersion
};


struct ReportOpts
{
    // The ReportOpts structure holds the entry options for use by the callback routine.

    wchar_t slashChar;      // Forward or backward slash character to use
    bool    fullPath;       // If true, report full path rather than default relative
    bool    filesOnly;      // If true, report only files (not directories)
    size_t  maxPathLength;  // Maximum path length
};



static inline bool isSlash (wchar_t c) {
    // Return true if the given character is a forward or backward slash.
    return (c == L'/') || (c == L'\\');
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

    auto reportOpts = static_cast<const ReportOpts*>(cbdata);

    auto fullPath = new wchar_t [reportOpts->maxPathLength + 1];  // Optional Full Path
    auto item = path;       // Pointer to Matching Entry

    // If we are to report only files and this entry is a directory, then return without reporting.

    auto isDirectory = fs::is_directory(path);
    if (reportOpts->filesOnly && isDirectory)
        return true;

    // TODO: Handle absolute and relative paths (reportOpts->fullPath).
    // TODO: Handle desired slash character (reportOpts->slashChar).

    wcout << path.wstring() << '\n';

    #if 0
    if (!reportOpts->fullPath)
        item = path;
    else
    {
        // If we are to convert the default relative path to a full path, use the stdlib _fullpath
        // function to do so. If this is not possible, then emit an error message and halt matching
        // entry enumeration.

        if (!_wfullpath(fullPath, entry, reportOpts->maxPathLength))
        {
            wcerr << L"pathmatch: Unable to convert \""
                  << entry
                  << L"\" to absolute path.\n";
            return false;
        }

        item = fs::path(fullPath);
    }

    // Print out the matching item, converted to the requested slash type.

    for (const wchar_t *ptr = item;  *ptr;  ++ptr)
    for (auto wchar_t : item
        wcout << (isSlash(*ptr) ? reportOpts->slashChar : *ptr);

    wcout << L'\n';
    #endif

    return true;   // Continue enumeration.
}


//--------------------------------------------------------------------------------------------------
int wmain (int argc, wchar_t *argv[])
{
    PathMatcher matcher;  // PathMatcher Object

    ReportOpts reportOpts {      // Options for callback routine
        L'\\',                   // slashChar:     Default slashes are backward.
        false,                   // fullPath:      Default to relative paths.
        false,                   // filesOnly:     Default to report files and directories
        PathMatcher::mc_MaxPathLength  // maxPathLength: Use PathMatcher temp value.
    };

    // Usage-printing helper function.
    auto exitWithUsage = [] () {
        wcout << usage_header << L'\n' << usage;
        exit(0);
    };

    // Option character helper function. If the string is a command-line option, then it returns
    // the option character, else 0.
    auto optionChar = [] (const wchar_t* arg) {
        return (arg[0] == L'-') ? arg[1] : 0;
    };

    if (argc <= 1)
        exitWithUsage();

    // Cycle through all command-line arguments.

    for (int argi=1;  argi < argc;  ++argi) {

        auto arg = argv[argi];

        // The argument "/?" is a special case. While it's technically a valid file system pattern,
        // we treat it as a request for tool information by convention (if it's the first argument).

        if (0 == (wcscmp(arg, L"/?")))
            exitWithUsage();

        switch (optionChar(arg)) {

            case L'h': case L'H': case L'?':     // Help Info
                exitWithUsage();

            case L'a': case L'A': {              // Absolute Path Option
                reportOpts.fullPath = true;
                break;
            }

            case L'f': case L'F': {              // Report Files Only
                reportOpts.filesOnly = true;
                break;
            }

            case L's': case L'S': {              // Slash Direction Option
             
                auto slashChar = arg[2];

                if (slashChar == 0) {
                    ++argi;
                    if (argi >= argc) {
                        wcerr << L"pathmatch: Expected slash type after '-s' option.\n";
                        exit (1);
                    }
                    slashChar = *argv[argi];
                }

                if ((slashChar != L'/') && (slashChar != L'\\')) {
                    wcerr << L"pathmatch: Invalid '-s' option (\"" << slashChar << L"\").\n";
                    exit (1);
                }

                reportOpts.slashChar = slashChar;
                break;
            }

            case L'v': case L'V': {              // Version Query
                wcout << version << L'\n';
                exit(0);
            }

            default: {
                matcher.Match (wstring(arg), &mtCallback, &reportOpts);
                break;
            }
        }
    }

    return 0;
}
