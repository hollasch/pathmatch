#ifndef _INCLUDED_PATHMATCHER_H
//==================================================================================================
// pathmatcher.h
//
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
#define _INCLUDED_PATHMATCHER_H


#include <filesystem>
#include <string>


namespace PathMatch
{

// Standalone Function Declarations

// Wildcard comparison test, case sensitive.
bool wildComp (const std::wstring& pattern, const std::wstring& str);
bool wildComp (std::wstring::const_iterator patternStart, std::wstring::const_iterator patternEnd,
               std::wstring::const_iterator strStart,     std::wstring::const_iterator strEnd);

// Path matching test, with ellipses or double asterisk (directory-spanning path portion), asterisk
// (substring of directory or file name), and question mark (matches any single character).
bool pathMatch (const wchar_t *pattern, const wchar_t *path);



class PathMatcher
{
    //---------------------------------------------------------------------------------------------
    // The PathMatcher class traverses the file system and reports all entries in a directory tree
    // tree that match a specified pattern. This pattern may contain the special match operators
    // '?', '*', '**', and '...'.
    //---------------------------------------------------------------------------------------------

  public:

    PathMatcher();
    ~PathMatcher();

    // The callback function signature that PathMatcher uses to report back all matching entries.
    using MatchCallback = bool (
        const std::filesystem::path& path,
        const std::filesystem::directory_entry& dirEntry,
        void* userData);

    // The main match procedure.
    bool match (const std::wstring pattern, MatchCallback* callback, void* userData);

    // Temporarily define a maximum path length. This is the Windows max path length, but it appears
    // that std::filesystem has no maximum path length (or it's not exposed).
    static const auto mc_MaxPathLength { 260 };

  private:   // Private Member Variables

    MatchCallback* m_callback { nullptr };     // Match Callback Function
    void*          m_callbackData { nullptr }; // Callback Function Data

    wchar_t* m_path;                           // Current path
    wchar_t* m_pattern { nullptr };            // Wildcarded portion of the given pattern
    size_t   m_patternBufferSize { 0 };        // Size of the pattern buffer.
    bool     m_dirsOnly { false };             // If true, report directories only

    const wchar_t* m_ellipsisPattern { nullptr };  // Ellipsis Pattern
    wchar_t*       m_ellipsisPath { nullptr };     // Path part to match against ellipsis pattern


  private:   // Private Methods

    bool allocPatternBuff (size_t requestedSize);

    bool groomPattern (const std::wstring pattern);

    void handleEllipsisSubpath (wchar_t *pathEnd, const wchar_t *pattern, int iPattern);

    void matchDir (wchar_t* pathend, const wchar_t* pattern);
    void fetchAll (wchar_t* pathend, const wchar_t* ellipsisPrefix);

    wchar_t* appendPath (wchar_t *pathEnd, const wchar_t *str);

    size_t pathSpaceLeft (const wchar_t *pathEnd) const;
};

}; // Namespace PathMatch


#endif  // _INCLUDED_PATHMATCHER_H
