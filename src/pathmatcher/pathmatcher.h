//==================================================================================================
// Declarations and definitions for the PathMatcher object. This object uses path match patterns
// (including the special operators '?', '*', and '...' to locate and report matching directory
// entries in a subdirectory tree.
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

#ifndef _pathmatcher_h
#define _pathmatcher_h

    // Includes

#include <io.h>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <FileSystemProxy.h>

using namespace std;
using FileSystemProxy::DirectoryIterator;
using FileSystemProxy::FileSysProxy;


namespace PathMatch {


    // Standalone Function Declarations

// Wildcard comparison test. Case insensitive.
bool wildComp (const wchar_t *pattern, const wchar_t *string);

// Wildcard comparison test. Case sensitive.
bool wildCompCaseSensitive (const wchar_t *pattern, const wchar_t *string);

// Path matching test, with ellipses (directory-spanning path portion), asterisk (substring of
// directory or file name), and question mark (matches any single character).
bool pathMatch (const wchar_t *pattern, const wchar_t *path);



// The callback function signature that PathMatcher uses to report back all matching entries.
typedef bool (MatchTreeCallback) (
    const wchar_t* entry,
    const DirectoryIterator& fileData,
    void* userData);

class PathMatcher
{
    //--------------------------------------------------------------------------
    // The PathMatcher object locates and reports all entries in a directory
    // tree that match a given pattern, which may contain the special operators
    // '?', '*', and '...'.
    //--------------------------------------------------------------------------

  public:

    PathMatcher (FileSysProxy &fsProxy);
    ~PathMatcher();

    // The main match procedure.

    bool Match (const wchar_t *pattern, MatchTreeCallback* callback, void* userData);


  private:   // Private Member Variables

    FileSysProxy&      m_fsProxy;                  // File System Proxy
    MatchTreeCallback* m_callback { nullptr };     // Match Callback Function
    void*              m_callbackData { nullptr }; // Callback Function Data

    wchar_t* m_path;                               // Current path
    wchar_t* m_pattern { nullptr };                // Wildcarded portion of the given pattern
    size_t   m_patternBufferSize { 0 };            // Size of the pattern buffer.
    bool     m_dirsOnly { false };                 // If true, report directories only

    const wchar_t* m_ellipsisPattern { nullptr };  // Ellipsis Pattern
    wchar_t*       m_ellipsisPath { nullptr };     // Path part to match against ellipsis pattern


  private:   // Private Methods

    bool AllocPatternBuff (size_t requestedSize);

    bool CopyGroomedPattern (const wchar_t *pattern);

    void HandleEllipsisSubpath (wchar_t *pathEnd, const wchar_t *pattern, int iPattern);

    void MatchDir (wchar_t* pathend, const wchar_t* pattern);
    void FetchAll (wchar_t* pathend, const wchar_t* ellipsisPrefix);

    wchar_t* AppendPath (wchar_t *pathEnd, const wchar_t *str);

    size_t PathSpaceLeft (const wchar_t *pathEnd) const;
};

}; // Namespace PathMatch


#endif  // ifndef _pathmatcher_h