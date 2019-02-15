//==================================================================================================
// pathmatcher.cpp
//
// Routines for matching wildcard path specifications against a given directory tree.
//
// _________________________________________________________________________________________________
// Copyright 2010-2019 Steve Hollasch
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

#include <assert.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <filesystem>
#include <locale>
#include <memory>
#include <string>
#include <vector>


namespace fs = std::filesystem;

using std::wstring;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::vector;


// =================================================================================================
// Local Helper Functions
// =================================================================================================

namespace {

    bool isSlash (const wchar_t c) {
        // Return true if and only if the character is a forward or backward slash.
        return (c == L'/') || (c == L'\\');
    }

    bool isDoubleAsterisk (wstring::const_iterator strIt, wstring::const_iterator end) {
        // Return true if the string iterator points to a sequence of two asterisk characters.
        return ((end - strIt) >= 2) && (strIt[0] == L'*') && (strIt[1] == L'*');
    }

    bool isDoubleAsterisk (const wchar_t* str) {
        // Return true if the string begins with a sequence of two asterisk characters.
        return (str[0] == L'*') && (str[1] == L'*');
    }

    bool isEllipsis (wstring::const_iterator strIt, wstring::const_iterator end) {
        // Return true iff string iterator begins with "...".
        return ((end - strIt) >= 3) && (strIt[0] == L'.') && (strIt[1] == L'.') && (strIt[2] == L'.');
    }

    bool isEllipsis (const wchar_t* str) {
        // Return true iff string begins with "...".
        return (str[0] == L'.') && (str[1] == L'.') && (str[2] == L'.');
    }

    bool isMultiWildStr (wstring::const_iterator strIt, wstring::const_iterator end) {
        // Return true if and only if the string begins with a wildcard that matches
        // multiple characters ("*" or "..." or "**").
        return (strIt != end) && ((*strIt == L'*') || isEllipsis(strIt, end));
    }

    bool isMultiWildStr (const wchar_t* str) {
        // Return true if and only if the string begins with a wildcard that matches
        // multiple characters ("*" or "..." or "**").
        return (str[0] == L'*') || isEllipsis(str);
    }

    const wchar_t c_slash = L'\\';
    const wchar_t c_multiWild = L'\u001d';    // U+001D - GROUP SEPARATOR

    bool entryIsADir (const WIN32_FIND_DATA &finddata) {
        // Returns true if the current directory entry is a directory.
        return 0 != (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool isDotsDir (const wchar_t *str) {
        // Return true if the string is either "." or ".."
        return (str[0] == L'.') && (!str[1] || ((str[1] == L'.') && !str[2]));
    }

    bool isUpDir (const wstring::const_iterator strIt) {
        // Return true if string begins with parent ("..") subpath.
        return (strIt[0] == L'.' && strIt[1] == L'.' && (!strIt[2] || isSlash(strIt[2])));
    }


    //----------------------------------------------------------------------------------------------
    vector<wstring> getGroomedPattern (const wstring pattern, bool& dirsOnly)
    {
        // FUTURE SPEC:
        // This function doesn't yet accomplish the goals described below. To perform these, it
        // should first split the string into components separated by slashes, then perform the
        // transformations below, then reassemble them (or perhaps not?).
        //
        // This function returns a groomed path pattern from the supplied pattern. It performs the
        // following transformations:
        //
        //   1. /a/../ -> /           (a)
        //   2. // -> /
        //   3. /a/.../../ -> /.../   (a)
        //   4. /.../.../ -> /.../
        //   5. /./ -> /
        //
        //   (a) Determine whether to allow (and how to handle) `..` paths in patterns.
        //       --> No `..` paths allowed after wildcard patterns
        //
        // It also replaces multi-wild sequences (`**`, `...`) with the c_multiWild character.
        //
        // In addition, it sets the `dirsOnly` out parameter according to whether the pattern had
        // a trailing slash to indicate matches with directories only.
        // --------

        wprintf (L"Starting getGroomedPattern\n");

        vector<wstring> patternVec;
        patternVec.push_back(L"one");
        patternVec.push_back(L"two");
        patternVec.push_back(L"three");

        return patternVec;

        #if 0
        // Allocate the buffer needed to store the pattern.

        auto desiredBufferSize = pattern.length() + 1;
        size_t   patternBufferSize = 0;
        unique_ptr<wchar_t[]> patternBuff;

        if (desiredBufferSize > patternBufferSize) {
            patternBuff = std::make_unique<wchar_t[]>(desiredBufferSize);

            if (!patternBuff) {
                patternBufferSize = 0;
                return wstring(L"");
            }

            patternBufferSize = desiredBufferSize;
        }

        auto src  = pattern.cbegin();
        auto dest = patternBuff.get();
        auto pastAnyLeadingSlashes = false;

        // Preserve leading multiple slashes at the beginning of the pattern.

        while (isSlash(*src))
            *dest++ = *src++;

        // Now copy the remainder of the path. Eliminate "." subpaths, reduce repeating slashes to
        // single slashes, and resolve ".." portions.

        while (src != pattern.cend()) {

            pastAnyLeadingSlashes = true;

            if ((src[0] == L'.') && ((src[1] == 0) || isSlash(src[1]))) {
                // The current subpath is a '.' directory.

                auto atStart = (dest == patternBuff.get());

                // Skip past any trailing slashes

                do { ++ src; } while (isSlash(*src));

                if (atStart) {
                    // If the pattern is just "." or "./" (for any number of tailing slashes), then
                    // just use "." as the pattern. If it is just prefixed with "./", then skip that
                    // and continue.

                    if (*src == 0) {
                        *dest++ = L'.';
                        dirsOnly = true;
                    }
                } else if (*src == 0) {
                    // If the pattern ends in "." or "./" (for any number of trailing slashes), then
                    // flag the search as directories-only and zap the prior slash.

                    --dest;
                    dirsOnly = true;
                } else {
                    // We've encountered a "./" in the middle of a path. In this case, just skip
                    // the copy.
                }

            } else if (isSlash(*src)) {

                while (isSlash(src[1]))   // Scan to the last slash in a series of slashes.
                    ++src;

                if (src[1] == 0) {
                    // If the pattern ends in a slash, then record that the pattern
                    // is matching directories only.
                    dirsOnly = true;
                    ++src;
                } else {
                    // Copy one slash only.
                    *dest++ = c_slash;
                    do { ++src; } while (isSlash(*src));
                }

            } else if (isUpDir(src)) {

                // If we encounter a "../" in the middle of a pattern, then erase the prior parent
                // directory if possible, otherwise append the "../" substring.

                // Skip forward in the source string past all trailing slashes.

                for (src+=3;  isSlash(*src);  ++src)
                    continue;

                auto destlen = dest - patternBuff.get();   // Current pattern length
                wchar_t* parent { nullptr };       // Candidate parent portion

                if ((destlen >= 2) && isSlash(dest[-1]) && !isSlash(dest[-2])) {
                    parent = dest - 2;

                    // Scan backwards to the beginning of the parent directory.

                    while ((parent > patternBuff.get()) && !isSlash(*parent))
                        --parent;

                    // Move past the prior leading slash if necessary (if the parent directory isn't
                    // the first subdirectory in the path).

                    if (isSlash(*parent)) ++parent;

                    // If the parent directory is already a "../", then just append the current up
                    // directory to the last one.

                    auto parentStr = wstring(parent);

                    if (isUpDir(parentStr.cbegin()))
                        parent = nullptr;
                }

                if (parent)
                    dest = parent;
                else {
                    *dest++ = L'.';
                    *dest++ = L'.';
                    *dest++ = c_slash;
                }

            } else {

                // If no special cases, then just copy up till the next slash or end of pattern.

                while (src != pattern.cend() && !isSlash(*src))
                    *dest++ = *src++;
            }
        }

        if (pastAnyLeadingSlashes)
            assert (!isSlash(dest[-1]));

        *dest = 0;

        return wstring(patternBuff.get());
        #endif
    }
}


// =================================================================================================
// PathMatch Namespace
// =================================================================================================

namespace PathMatch {


// =================================================================================================
// Standalone PathMatch Functions
// =================================================================================================

bool wildComp (const wstring& pattern, const wstring& str) {
    // Performs a case-sensitive comparison of the string pattern against the string str.
    // See below for further details.

    return wildComp (pattern.cbegin(), pattern.cend(), str.cbegin(), str.cend());
}


//--------------------------------------------------------------------------------------------------
bool wildComp (
    std::wstring::const_iterator patternIt,
    std::wstring::const_iterator patternEnd,
    std::wstring::const_iterator strIt,
    std::wstring::const_iterator strEnd)
{
    // Compares a pattern against a string (case sensitive) to determine if the two match. In the
    // pattern string, the character '?' denotes any single character, and the character '*' denotes
    // any number of characters. All other characters are interpreted literally, though they are
    // compared without regard to case (for exampmle, 'a' matches 'A'). For case-insensitive
    // matches, ensure that the pattern and string are both lowercase first.
    //
    // Parameters
    //     patternIt  - Iterator over the const pattern to compare with the string
    //     patternEnd - The end of the const pattern string
    //     strIt      - Iterator over the const string to test for matching
    //     strIt      - The end of the const string to test for matching
    //
    // Returns
    //     True if and only if the pattern matches the string. This function returns false if either
    //     the pattern or the string are null pointers.
    //--------

    // Scan through the single character matches.

    while ((patternIt != patternEnd) && (strIt != strEnd)) {
        if (*patternIt == L'*')  // If we've hit an asterisk, then drop down to the section below.
            break;

        // Stop testing on mismatch.

        if ((*patternIt != L'?') && *patternIt != *strIt)
            break;

        ++ patternIt;   // On a successful match, increment the pattern and the string and continue.
        ++ strIt;
    }

    // Unless we stopped on an asterisk, we're done matching. The only valid way to match at this
    // point is if both the pattern and the string are exhausted.

    if (*patternIt != L'*')
        return (patternIt == patternEnd) && (strIt == strEnd);

    // Advance past the asterisk. Handle pathological cases where there is more than one asterisk
    // in a row.

    while (*patternIt == L'*')
        ++patternIt;

    // If the asterisk is the last character of the pattern, then we match any remainder,
    // so return true.

    if (patternIt == patternEnd)
        return true;

    // We're at an asterisk with other patterns following, so recursively eat away at the string
    // until we match or exhaust the string.

    while (true) {
        if (wildComp (patternIt, patternEnd, strIt, strEnd))
            return true;

        if (strIt == strEnd)
            return false;

        ++ strIt;
    }
}


//--------------------------------------------------------------------------------------------------
bool pathMatch (const wchar_t *pattern, const wchar_t *path)
{
    // Compares a single path against a VMS-style wildcard specification. In the pattern string, the
    // character '?' denotes any single character except '/', the character '*' denotes any number
    // of characters except '/', and the sequence '...' or '**' denotes any number of characters
    // including '/'. All other characters in the pattern are interpreted literally, though without
    // regard to case. For example, 'a' matches 'A'.
    //
    // Note that this routine also interprets a backslash ('\') as a euphemism for a forward slash.
    //
    // Also note:
    //
    //     - Multiple slashes compare as a single slash. That is, "/////" compares the same as "/".
    //
    //     - ".../" and "*\" (without regard to slash type) both match the empty string, so
    //       ".../foo" and "*\foo" both match "foo". To exclude this rule, use a question mark like
    //       so: "...?/foo".
    //
    // Parameters
    //     pattern - The pattern to compare with the path.
    //     path    - The path to test for matching
    //
    // Returns
    //     True if and only if the pattern matches the given path. This function returns false if
    //     the pattern is null or empty, or if the path is null.
    //--------

    if (!pattern || !path) return false;

    // Scan through the pattern and path until we hit an asterisk or an ellipsis. Handle the special
    // cases of "/.../" and "/*/", tested against null subdirectories (where both are also
    // equivalent to "/").

    while (*pattern && *path) {

        if (isSlash(path[0])) {           // Consume repeated slashes on path.
            while (isSlash(path[1]))
                ++ path;
        }

        if (isSlash(pattern[0])) {
            if (!isSlash(path[0]))
                return false;

            while (isSlash(pattern[1]))  // Consume repeated slashes on pattern.
                ++ pattern;
        } else if (isMultiWildStr (pattern)) {
            // If we've hit a multi-character wildcard character, then drop to section below.
            break;
        }

        // Test for a single character match. In order to support case-sensitive path matching,
        // you'd only need to change the tolower comparison below.

        if (*pattern != L'?') {
            if (tolower(*pattern) != tolower(*path))
                return false;
        } else if (isSlash(*path)) {       // '?' matches all but slash.
            return false;
        }

        ++ pattern;    // On a successful match, increment the pattern and the path and continue.
        ++ path;
    }

    // Unless we stopped on a multi-character wildcard, we're done matching. The only valid way to
    // match at this point is if both the pattern and the path are exhausted.

    if (!isMultiWildStr(pattern))
        return (*pattern == 0) && (*path == 0);

    // Advance past the multi-character wildcard(s). A sequence of asterisks is equivalent to a
    // single asterisk, and a sequence of ellipses and asterisks is equivalent to a single
    // ellipsis. We handle this here because many asterisks and ellipses in a row would yield
    // exponential (and pathological) runtimes.

    bool fEllipsis { false };

    while (isMultiWildStr (pattern)) {
        if (isEllipsis(pattern)) {
            pattern += 3;
            fEllipsis = true;
        }
        else if (isDoubleAsterisk(pattern)) {
            pattern += 2;
            fEllipsis = true;
        } else {
            pattern += 1;
        }
    }

    // If the pattern ends in an ellipsis, then we trivially match any remainder of the path, so
    // return true, otherwise perform match testing.

    if (fEllipsis && (*pattern == 0))
        return true;

    // A multi-wild pattern (* or ...) followed by any number of slashes can match the empty string,
    // so we test for that here. Thus, ".../foo" will match against "foo".

    if (isSlash(*pattern)) {
        // Search forward past any number of trailing slashes.

        auto ptr = pattern + 1;

        while (isSlash(*ptr)) ++ptr;

        // Match the remainder of the pattern against the remainder of the path.

        if (pathMatch(ptr,path))
            return true;
    }

    if (fEllipsis) {
        // If we have an ellipsis, then recursively nibble away at the path to see if we can yield
        // a match, until we either match or exhaust the path.

        for (;; ++path) {
            if (pathMatch(pattern, path)) return true;
            if (*path == 0) return false;
        }
    } else {
        // If we have an asterisk, then recursively nibble away at the path until we encounter a
        // slash or exhaust the path.

        for (; *path && !isSlash(*path);  ++path) {
            if (pathMatch(pattern, path)) return true;
        }

        // Test the remainder of the pattern and path.

        return pathMatch(pattern, path);
    }
}



//==================================================================================================
// PathMatcher Class Implementation
//==================================================================================================

size_t PathMatcher::pathSpaceLeft (const wchar_t *pathend) const
{
    // Returns the number of characters that can be appended to the m_path
    // string, while allowing room for a terminating character.

    return (mc_MaxPathLength + 1) - (pathend - m_path) - 1;
}


//--------------------------------------------------------------------------------------------------
PathMatcher::PathMatcher()
{
    // PathMatcher Default Constructor
    size_t pathSize = mc_MaxPathLength + 1;    // Temporary Windows max path length.
    m_path = new wchar_t [pathSize];
    m_path[0] = 0;
}

PathMatcher::~PathMatcher ()
{
    // PathMatcher Destructor
    delete[] m_path;
    delete[] m_patternBuff;
}


//--------------------------------------------------------------------------------------------------
wchar_t* PathMatcher::appendPath (wchar_t *pathend, const wchar_t *str)
{
    // This procedure appends the current path with the specified string.
    //
    // Parameter 'pathend' is the end of the current path (one past the last character). Parameter
    // 'str' is the string to append.
    //
    // This function  returns the new path end pointer, or null if the path buffer is not large
    // enough to append the new entry name.
    //--------

    auto strlength = wcslen (str);

    // Return null if there's not enough space to append the string.

    if (pathSpaceLeft(pathend) < (strlength + 1))
        return nullptr;

    while (*str)
        *pathend++ = *str++;

    *pathend = 0;

    return pathend;
}


//--------------------------------------------------------------------------------------------------
bool PathMatcher::match (
    const wstring  path_pattern,
    MatchCallback* callback_func,
    void*          userdata)
{
    // This function walks a directory tree according to the given wildcard pattern, and calls the
    // specified callback function for each matching entry.
    //
    // 'path_pattern' is the pattern to match against tree entries.
    // 'callback_func' is the callback function for each matching entry.
    // 'userdata' is the user data to be passed along to callback function.
    //
    // This function returns true if the function successfully completes the search, otherwise
    // false.
    //--------

    if (!callback_func)      // Bail out if the user didn't provide a
        return false;        // callback function.

    m_callback = callback_func;
    m_callbackData = userdata;

    // Copy the groomed pattern (see comments for CopyGroomedPattern) into the appropriate member
    // fields.

    // m_pattern = getGroomedPattern(path_pattern, m_dirsOnly);
    auto patternVec = getGroomedPattern(path_pattern, m_dirsOnly);

    wprintf (L"Pattern vector:\n");
    for (auto component : patternVec) {
        wprintf (L"    '%s'\n", component.c_str());
    }
    return false;

    if (m_pattern.empty())
        return false;

    m_patternBuff = new wchar_t[m_pattern.length() + 1];
    wcscpy_s (m_patternBuff, m_pattern.length() + 1, m_pattern.c_str());

    // We will divide the path_pattern up into two parts: the root path, and the remaining pattern.
    // For example, "C:/foo/.../bar*" would be divided up into a root of "C:/foo" and a pattern of
    // ".../bar*".

    auto rootend   = m_patternBuff;
    auto wildstart = m_patternBuff;
    auto ptr       = m_patternBuff;

    // Locate the end of the root portion of the file pattern, and the start of the wildcard
    // pattern.

    for (; *ptr; ++ptr) {
        if (isSlash(*ptr) || (*ptr == L':')) {
            rootend   = ptr;
            wildstart = ptr + 1;
        } else if ((*ptr == L'?') || isMultiWildStr(ptr)) {
            break;
        }
    }

    // If the supplied pattern has no specific root directory, then just set the root directory to
    // the current directory.

    size_t rootlen;    // Length of the root path string.

    if (rootend == m_patternBuff) {
        m_path[0] = 0;
        rootlen = 0;
    } else {
        ++rootend;                // Include the '/' or ':' character.

        rootlen = rootend - m_patternBuff;

        if (FAILED(wcsncpy_s (m_path, (mc_MaxPathLength + 1), m_patternBuff, rootlen))) {
            delete[] m_patternBuff;
            m_patternBuff = nullptr;
            return false;
        }
    }

    matchDir (m_path + rootlen, wildstart);

    delete[] m_patternBuff;
    m_patternBuff = nullptr;

    return true;
}


//--------------------------------------------------------------------------------------------------
void PathMatcher::matchDir (
    wchar_t*       pathend,
    const wchar_t* pattern)
{
    // This procedure matches a substring pattern against a given root directory. Each matching
    // entry in the tree will yield a call back to the specified function, along with given user
    // data. Note that the path string buffer will be used to pass back matching entries to the
    // callback function.
    //
    // 'pathend' is the end of the current path (one past the last character)
    // 'pattern is the pattern against which to match directory entries.
    //--------

    // If the pattern is null, then just return.

    if (!pattern || !*pattern) return;

    // Characterize the type of pattern matching we'll be doing in the current directory. Scan
    // forward to find the first of the end of the pattern, a slash, an ellipsis, or a double asterisk.

    int  ipatt { 0 };
    auto fliteral = true;

    while (pattern[ipatt] && !isSlash(pattern[ipatt])
       && !(isEllipsis(pattern + ipatt) || isDoubleAsterisk(pattern + ipatt)))
    {
        if ((pattern[ipatt] == L'?') || (pattern[ipatt] == L'*'))
            fliteral = false;

        ++ ipatt;
    }

    // If the current pattern subdirectory contains an ellipsis, then handle the remainder of the
    // pattern and return.

    if (isEllipsis(pattern + ipatt) || isDoubleAsterisk(pattern + ipatt)) {
        handleEllipsisSubpath (pathend, pattern, ipatt);
        return;
    }

    assert (!pattern[ipatt] || isSlash(pattern[ipatt]));

    auto fdirmatch = isSlash(pattern[ipatt]);
    auto fdescend  = fdirmatch && (pattern[ipatt+1] != 0);

    // (simple) (end)

    auto subPattern = new wchar_t [ipatt+1];

    if (!subPattern) return;   // Bail out if out of memory.

    if (FAILED(wcsncpy_s (subPattern, ipatt+1, pattern, ipatt))) {
        delete[] subPattern;
        return;
    }

    // TODO: lowercase subPattern here.

    // If we have a literal subdirectory name (or filename), then just provide that name to the
    // find-file functions.

    errno_t retval { S_OK };    // General Return Value

    if (fliteral)
        retval = wcsncpy_s (pathend, pathSpaceLeft(pathend), pattern, ipatt);

    // If there's a wildcard subdirectory or file name, then enumerate all directory entries and
    // filter the results.

    if (!fliteral || FAILED(retval)) {
        pathend[0] = L'*';
        pathend[1] = 0;
    }

    auto fsPath = fs::path(m_path);

    for (const auto& dirEntry : fs::directory_iterator(fsPath)) {
        // Ignore "." and ".." entries.

        auto entryName = dirEntry.path().filename();

        if (isDotsDir(entryName.c_str())) continue;

        // TODO: Create lowercase of entryName here, use for wildcomp call.

        if (!fliteral) {
            auto subPatternString = make_unique<wstring> (subPattern);
            if (!wildComp (*subPatternString, entryName)) continue;
        }

        // Skip files if the pattern ended in a slash or if the original pattern specified
        // directories only.

        auto isDirectory = fs::is_directory(dirEntry.status());
        if ((m_dirsOnly || fdirmatch) && !isDirectory) {
            // Do nothing.
        } else if (fdescend) {
            auto pathend_new = appendPath (pathend, entryName.c_str());

            if (!pathend_new) continue;

            *pathend_new++ = c_slash;
            *pathend_new   = 0;

            matchDir (pathend_new, pattern + ipatt + 1);
        } else {
            // Construct full relative entry path.

            if (appendPath(pathend, entryName.c_str())) {
                if (!m_callback (fsPath / entryName, dirEntry, m_callbackData))
                    break;
            }
        }
    }

    delete[] subPattern;
    return;
}


//--------------------------------------------------------------------------------------------------
void PathMatcher::handleEllipsisSubpath (
    wchar_t       *pathend,    // One past the last character
    const wchar_t *pattern,    // Pointer to the beginning of the current subdir of the full pattern
    int            ipatt)      // Offset from the pattern to the beginning of the ellipsis
{
    // This function handles subdirectories that contain ellipses (or double asterisks).

    wchar_t* ellipsis_prefix { nullptr };    // Pattern Filter for Prefixed Ellipses

    auto ellipsisEnd = isEllipsis(pattern + ipatt) ? (ipatt + 3) : (ipatt + 2);

    if ((ipatt == 0) && !pattern[ellipsisEnd]) {
        // ...<end> - Just do a simple recursive fetch of the tree.

        m_ellipsisPattern = nullptr;

    } else {

        m_ellipsisPattern = pattern;
        m_ellipsisPath    = pathend;

        // If the ellipsis is prefixed with a pattern, then we want to save the pattern for
        // filtering of candidate directory entries by the FetchAll routine.

        if (ipatt <= 0) {
            ellipsis_prefix = nullptr;
        } else {
            ellipsis_prefix = new wchar_t [ipatt+2];

            if (!ellipsis_prefix || FAILED(wcsncpy_s (ellipsis_prefix, ipatt+2, pattern, ipatt))) {
                return;
            }

            ellipsis_prefix[ipatt]   = L'*';
            ellipsis_prefix[ipatt+1] = 0;
        }
    }

    // TODO: lowercase ellipsis_prefix here.

    fetchAll (pathend, ellipsis_prefix);
    return;
}


//--------------------------------------------------------------------------------------------------
void PathMatcher::fetchAll (wchar_t* pathend, const wchar_t* ellipsis_prefix)
{
    // This procedure is called when an ellipsis is encountered, and recursively fetches all tree
    // entries and optionally matches against a pattern.
    //
    // 'pathend' is the end of the current path (one past last character)
    //
    // 'ellipsis_prefix' is the pattern that prefixes the ellipsis, followed by an asterisk. It will
    // be used to filter directory entries for subsequent ellipsis pattern matching.
    //
    // This function silently returns on error.
    //--------

    static const wchar_t c_slashstr[] { c_slash, 0 };

    // Append slash if needed.

    if ((pathend > m_path) && !isSlash(pathend[-1])) {
        pathend = appendPath (pathend, c_slashstr);
        if (!pathend) return;          // Bail out if the append failed.
    }

    // Bail out if we've run out of path length.

    if (pathSpaceLeft(pathend) < 1) return;

    auto fsPath = fs::path(m_path);

    for (const auto& dirEntry : fs::directory_iterator(fsPath)) {

        auto entryName = dirEntry.path().filename();

        // Skip file entries if we're only looking for directories.

        auto isDirectory = fs::is_directory(dirEntry.status());
        if (m_dirsOnly && !isDirectory)
            continue;

        // If there's an ellipsis prefix, then ensure first that we match against it before
        // descending further.

        // TODO: Create lowercase of entryName here, use in wildcomp call.

        if (ellipsis_prefix && !wildComp (ellipsis_prefix, entryName))
            continue;

        auto pathEndNew = appendPath (pathend, entryName.c_str());

        if (!pathEndNew) break;

        if (!m_ellipsisPattern || pathMatch(m_ellipsisPattern, m_ellipsisPath)) {
            if (!m_callback (fsPath / entryName, dirEntry, m_callbackData))
                return;
        }

        if (isDirectory)
            fetchAll (pathEndNew, nullptr);
    }
}


}; // Namespace PathMatch
