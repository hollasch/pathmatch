//==================================================================================================
// Declarations and definitions for the PathMatcher object. This object uses path match patterns
// (including the special operators '?', '*', and '...' to locate and report matching directory
// entries in a subdirectory tree.
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

#ifndef _pathmatcher_h
#define _pathmatcher_h

    // Includes

#include <io.h>
#include <stdlib.h>
#include <windows.h>

#include <filesystem>
#include <string>

#include <FileSystemProxy.h>



namespace PathMatch {

    // Standalone Function Declarations

// Wildcard comparison test, case sensitive.
bool wildComp (const std::wstring& pattern, const std::wstring& str);
bool wildComp (std::wstring::const_iterator patternIt, std::wstring::const_iterator patternEnd,
               std::wstring::const_iterator strIt,     std::wstring::const_iterator strEnd);

// Path matching test, with ellipses (directory-spanning path portion), asterisk (substring of
// directory or file name), and question mark (matches any single character).
bool pathMatch (const wchar_t *pattern, const wchar_t *path);

// The callback function signature that PathMatcher uses to report back all matching entries.
using MatchTreeCallback = bool (const wchar_t* entry, const std::filesystem::path& path, void* userData);


class PathMatcher
{
    //--------------------------------------------------------------------------
    // The PathMatcher object locates and reports all entries in a directory
    // tree that match a given pattern, which may contain the special operators
    // '?', '*', and '...'.
    //--------------------------------------------------------------------------

  public:

    PathMatcher (FileSystemProxy::FSProxy &fsProxy);
    ~PathMatcher();

    // The main match procedure.

    bool Match (const wchar_t *pattern, MatchTreeCallback* callback, void* userData);

  private:   // Private Member Variables

    FileSystemProxy::FSProxy& m_fsProxy;                  // File System Proxy
    MatchTreeCallback*        m_callback { nullptr };     // Match Callback Function
    void*                     m_callbackData { nullptr }; // Callback Function Data

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
