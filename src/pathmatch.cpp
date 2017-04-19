//==================================================================================================
// pathmatch
//
//     This program returns all files and directories matching the specified pattern.
//
//==================================================================================================

#include "pathmatcher.h"
#include "FileSystemProxyWindows.h"

#include <iostream>

using namespace std;
using namespace PMatcher;
using namespace FSProxy;

static const wstring version = L"0.1.1";

    // Usage Information

static const wstring usage_header {
    L"pathmatch  v" + version + L"  https://github.com/hollasch/pathmatch/"
};

static const wstring usage {
//    ----+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
    L"pathmatch: Report files and directories matching the specified pattern\n"
    L"Usage    : pathmatch [-s<slash>] [-f] [-v] <pattern> ... <pattern>\n"
    L"\n"
    L"    pathmatch finds and reports all files and directories matching wildcard\n"
    L"    patterns. These patterns may contain the special characters '?', '*', and\n"
    L"    '...'. The '?' pattern matches any single character, '*' matches multiple\n"
    L"    characters except slashes, and '...' matches multiple characters including\n"
    L"    slashes. For example, the following patterns all match the file\n"
    L"    \"abc\\def\\ghi\\jkl\": \"abc\\d?f\\??i\\jkl\", \"abc\\*\\*\\jkl\", \"abc\\...\\jkl\", and\n"
    L"    \"ab...kl\".\n"
    L"\n"
    L"    The following command options are supported:\n"
    L"\n"
    L"    -s<slash>  Specifies the slash direction to be reported. By default,\n"
    L"               slashes will be back slashes. Use \"-s/\" to report paths\n"
    L"               with forward slashes.\n"
    L"\n"
    L"    -a         Report absolute paths. By default, reported paths are\n"
    L"               relative to the current working directory.\n"
    L"\n"
    L"    -f         Report files only (no directories). To report directories\n"
    L"               only, append a slash to the pattern.\n"
    L"\n"
    L"    -v         Print version information.\n"
    L"\n"
};


MatchTreeCallback mtCallback;    // Matching Entry Callback Routine

    // The ReportOpts structure holds the entry options for use by the callback
    // routine.

struct ReportOpts
{
    wchar_t slashChar;      // Forward or backward slash character to use
    bool    fullPath;       // If true, report full path rather than default relative
    bool    filesOnly;      // If true, report only files (not directories)
    size_t  maxPathLength;  // Maximum path length
};



int wmain (int argc, wchar_t *argv[])
{
    //==========================================================================
    // Main
    //==========================================================================

    FileSysProxyWindows fsProxy;    // File System Proxy Object
    PathMatcher matcher {fsProxy};  // PathMatcher Object

    ReportOpts reportOpts {      // Options for callback routine
        L'\\',                   // slashChar:     Default slashes are backward.
        false,                   // fullPath:      Default to relative paths.
        false,                   // filesOnly:     Default to report files and directories
        fsProxy.maxPathLength()  // maxPathLength: Use file system proxy value.
    };

    // Usage-printing helper function.
    auto exitWithUsage = [] () {
        wcout << usage_header << endl << usage;
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

    for (int argi=1;  argi < argc;  ++argi)
    {
        auto arg = argv[argi];

        // The argument "/?" is a special case. While it's technically a valid file system pattern,
        // we treat it as a request for tool information by convention (if it's the first argument).

        if (0 == (wcscmp(arg, L"/?")))
            exitWithUsage();

        switch (optionChar(arg))
        {
            case L'h': case L'H': case L'?':     // Help Info
                exitWithUsage();

            case L'a': case L'A':                // Absolute Path Option
            {   reportOpts.fullPath = true;
                break;
            }

            case L'f': case L'F':                // Report Files Only
            {   reportOpts.filesOnly = true;
                break;
            }

            case L's': case L'S':                // Slash Direction Option
            {
                auto slashChar = arg[2];

                if (slashChar == 0) {
                    ++argi;
                    if (argi >= argc)
                    {   wcerr << L"pathmatch: Expected slash type after '-s' option.\n";
                        exit (1);
                    }
                    slashChar = *argv[argi];
                }

                if ((slashChar != L'/') && (slashChar != L'\\'))
                {   wcerr << L"pathmatch: Invalid '-s' option (\"" << slashChar << L"\").\n";
                    exit (1);
                }

                reportOpts.slashChar = slashChar;
                break;
            }

            case L'v': case L'V':                // Version Query
            {   wcout << version << endl;
                exit(0);
            }

            default:
            {
                matcher.Match (arg, &mtCallback, &reportOpts);
                break;
            }
        }
    }

    return 0;
}



static inline bool isSlash (wchar_t c)
{
    // Return true if the given character is a fore or back slash.
    return (c == L'/') || (c == L'\\');
}



bool mtCallback (
    const wchar_t*           entry,
    const DirectoryIterator& filedata,
    void*                    cbdata)
{
    //==========================================================================
    // mtcallback
    //     This is the callback function for the PathMatcher object.
    //
    // Parameters
    //     entry ....... The relative matching file or directory
    //     attribs ..... The file or directory's attributes
    //     cbdata ...... Pointer to the report options
    //
    // Returns
    //     True to continue fetching matching entries, unless the routine
    //     encountered and error attempting to convert a path to a full path.
    //==========================================================================

    // Get the properly typed report options structure from the callback data.

    auto reportOpts = static_cast<const ReportOpts*>(cbdata);

    auto fullPath = new wchar_t [reportOpts->maxPathLength + 1];  // Optional Full Path
    auto item = entry;       // Pointer to Matching Entry

    // If we are to report only files and this entry is a directory, then return without reporting.

    if (reportOpts->filesOnly && (filedata.isDirectory()))
        return true;

    if (!reportOpts->fullPath)
        item = entry;
    else
    {
        // If we are to convert the default relative path to a full path, use the stdlib _fullpath
        // function to do so. If this is not possible, then emit an error message and halt matching
        // entry enumeration.

        if (!_wfullpath(fullPath, entry, reportOpts->maxPathLength))
        {
            wcerr << L"pathmatch: Unable to convert \""
                  << entry
                  << L"\" to absolute path."
                  << endl;
            return false;
        }

        item = fullPath;
    }

    // Print out the matching item, converted to the requested slash type.

    for (const wchar_t *ptr = item;  *ptr;  ++ptr)
        wcout << (isSlash(*ptr) ? reportOpts->slashChar : *ptr);

    wcout << endl;

    return true;   // Continue enumeration.
}
