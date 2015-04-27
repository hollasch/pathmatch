//==================================================================================================
// pathmatch
//
//     This program returns all files and directories matching the specified pattern.
//
// _________________________________________________________________________________________________
// Copyright 2015 Steve Hollasch
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under
// the License.
//==================================================================================================

#include "pathmatcher.h"
#include "FileSystemProxyWindows.h"

#include <iostream>

using namespace std;
using namespace PathMatch;
using namespace FileSystemProxy;

    // Usage Information

static const wchar_t usage[] {
    L"\n"
    L"pathmatch v005 / 2007-12-03 / Steve Hollasch <steve@hollasch.net>\n"
    L"pathmatch: Report files and directories matching the specified pattern\n"
    L"Usage    : pathmatch [-s<slash>] [-f] <pattern> ... <pattern>\n"
    L"\n"
    L"    pathmatch finds and reports all files and directories matching wildcard\n"
    L"    patterns. These patterns may contain the special characters '?', '*',\n"
    L"    and '...'. The '?' pattern matches any single character, '*' matches\n"
    L"    multiple characters except slashes, and '...' matches multiple\n"
    L"    characters including slashes. For example, the following patterns all\n"
    L"    match the file \"abc\\def\\ghi\\jkl\": \"abc\\d?f\\??i\\jkl\", \"abc\\*\\*\\jkl\",\n"
    L"    and \"abc\\...\\jkl\".\n"
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



inline wchar_t OptionChar (const wchar_t *arg)
{
    //==========================================================================
    // OptionChar
    //     Returns the option character for a command-line argument. This is the
    //     letter following a dash character. If the first character is not a
    //     dash, this function will return 0.
    //==========================================================================

    if (arg[0] != L'-')
        return 0;
    else
        return arg[1];
}



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

    if (argc <= 1)
    {   wcout << usage;
        return 0;
    }

    // Cycle through all command-line arguments.

    for (int argi=1;  argi < argc;  ++argi)
    {
        auto arg = argv[argi];

        // The argument "/?" is a special case. While it's technically a valid file system pattern,
        // we treat it as a request for tool information by convention (if it's the first argument).

        if (0 == (wcscmp(arg, L"/?")))
        {   wcout << usage;
            return 0;
        }

        switch (OptionChar(arg))
        {
            case L'h': case L'H': case L'?':     // Help Info
            {   wcout << usage;
                break;
            }

            case L'a': case L'A':                // Absolute Path Option
            {   reportOpts.fullPath = true;
                break;
            }

            case L'f': case L'F':                // Report Files Only
            {   reportOpts.filesOnly = true;
                break;
            }

            case L's': case L'S':                // Slash Direction Option
            {   if ((arg[2] == L'/') || (arg[2] == L'\\'))
                    reportOpts.slashChar = arg[2];
                else
                {   wcerr << L"pathmatch: Unexpected -s option (\""
                          << arg+2 << L"\").\n";
                    exit (1);
                }
                break;
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
