//==================================================================================================
// These routines perform matching of strings against wild-card patterns.
//
// Copyright 2013 Steve Hollasch
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under
// the License.
//==================================================================================================

#include "wildcomp.h"
#include <ctype.h>


bool wildcomp (const wchar_t *pattern, const wchar_t *string)
{
    //==============================================================================================
    // wildcomp
    //     Compares a pattern against a string to determine if the two match. In the pattern string,
    //     the character '?' denotes any single character, and the character '*' denotes any number
    //     of characters. All other characters are interpreted literally, though they are compared
    //     without regard to case (for exampmle, 'a' matches 'A'). For case-sensitive matches, use
    //     wildcompc().
    // 
    // Parameters
    //     pattern - The pattern to compare with the string
    //     string  - The string to test for matching
    // 
    // Returns
    //     True if and only if the pattern matches the string. This function returns false if either
    //     the pattern or the string are null pointers.
    //==============================================================================================

    if (!pattern || !string) return false;

    // Scan through the single character matches.

    while (*pattern && *string)
    {
        if (*pattern == L'*')  // If we've hit an asterisk, then drop down to the section below.
            break;

        // Stop testing on mismatch.

        if ((*pattern != L'?') && (tolower(*pattern) != tolower(*string)))
            break;

        ++ pattern;    // On a successful match, increment the pattern and the string and continue.
        ++ string;
    }

    // Unless we stopped on an asterisk, we're done matching. The only valid way to match at this
    // point is if both the pattern and the string are exhausted.

    if (*pattern != L'*')
        return (*pattern == 0) && (*string == 0);

    // Advance past the asterisk. Handle pathological cases where there is more than one asterisk
    // in a row.

    while (*pattern == L'*')
        ++pattern;

    // If the asterisk is the last character of the pattern, then we match any remainder,
    // so return true.

    if (*pattern == 0)
        return true;

    // We're at an asterisk with other patterns following, so recursively eat away at the string
    // until we match or exhaust the string.

    while (true)
    {
        if (wildcomp (pattern, string))
            return true;

        if (!*string++)
            return false;
    }
}



bool wildcompc (const wchar_t *pattern, const wchar_t *string)
{
    //==============================================================================================
    // wildcompc
    //     Compares a pattern against a string to determine if the two match. In the pattern string,
    //     the character '?' denotes any single character, and the character '*' denotes any number
    //     of characters. All other characters are interpreted literally, and must match case. For
    //     case-insensitive matching, use wildcomp.
    //
    // Parameters
    //     pattern - The pattern to compare with the string
    //     string  - The string to test for matching
    //
    // Returns
    //     True if and only if the pattern matches the string. This function returns false if either
    //     the pattern or the string are null pointers.
    //==============================================================================================

    if (!pattern || !string) return false;

    // Scan through the single character matches.

    while (*pattern && *string)
    {
        if (*pattern == L'*')  // If we've hit an asterisk, then drop down to the section below.
            break;

        // Stop testing on mismatch.

        if ((*pattern != L'?') && (*pattern != *string))
            break;

        ++ pattern;    // On a successful match, increment the pattern and the string and continue.
        ++ string;
    }

    // Unless we stopped on an asterisk, we're done matching. The only valid way to match at this
    // point is if both the pattern and the string are exhausted.

    if (*pattern != L'*')
        return (*pattern == 0) && (*string == 0);

    // Advance past the asterisk. Handle pathological cases where there is more than one asterisk
    // in a row.

    while (*pattern == L'*')
        ++pattern;

    // If the asterisk is the last character of the pattern, then we match any remainder,
    // so return true.

    if (*pattern == 0)
        return true;

    // We're at an asterisk with other patterns following, so recursively eat away at the string
    // until we match or exhaust the string.

    while (true)
    {
        if (wildcompc (pattern, string))
            return true;

        if (!*string++)
            return false;
    }
}



bool pathmatch (const wchar_t *pattern, const wchar_t *path)
{
    //==============================================================================================
    // pathmatch
    //     Compares a path against a VMS-style wildcard specification. In the pattern string, the
    //     character '?' denotes any single character except '/', the character '*' denotes any
    //     number of characters except '/', and the sequence '...' denotes any number of characters
    //     including '/'. All other characters in the pattern are interpreted literally, though
    //     without regard to case. For example, 'a' matches 'A'.
    // 
    //     Note that this routine also interprets a backslash ('\') as a euphemism for a forward
    //     slash.
    // 
    //     Also note:
    // 
    //         - Multiple slashes compare as a single slash. That is, "/////" compares the same
    //           as "/".
    // 
    //         - ".../" and "*\" (without regard to slash type) both match the empty string, so
    //           ".../foo" and "*\foo" both match "foo". To exclude this rule, use a question mark
    //           like so: "...?/foo".
    // 
    // Parameters
    //     pattern - The pattern to compare with the path.
    //     path    - The path to test for matching
    // 
    // Returns
    //     True if and only if the pattern matches the given path. This function returns false if
    //     the pattern is null or empty, or if the path is null.
    //==============================================================================================

    if (!pattern || !path) return false;

    // Scan through the pattern and path until we hit an asterisk or an ellipsis. Handle the special
    // cases of "/.../" and "/*/", tested against null subdirectories (where both are also
    // equivalent to "/").

    while (*pattern && *path)
    {
        if (IsSlash(path[0]))             // Consume repeated slashes on path.
        {   while (IsSlash(path[1]))
                ++ path;
        }

        if (IsSlash(pattern[0]))
        {
            if (!IsSlash(path[0]))
                return false;

            while (IsSlash(pattern[1]))  // Consume repeated slashes on pattern.
                ++ pattern;
        }
        else if (IsMultiWildStr (pattern))
        {   // If we've hit a multi-character wildcard character, then drop to section below.
            break;
        }

        // Test for a single character match. In order to support case-sensitive path matching,
        // you'd only need to change the tolower comparison below.

        if (*pattern != L'?')
        {   if (tolower(*pattern) != tolower(*path))
                return false;
        }
        else if (IsSlash(*path))         // '?' matches all but slash.
        {   return false;
        }

        ++ pattern;    // On a successful match, increment the pattern and the path and continue.
        ++ path;
    }

    // Unless we stopped on a multi-character wildcard, we're done matching. The only valid way to
    // match at this point is if both the pattern and the path are exhausted.

    if (!IsMultiWildStr(pattern))
        return (*pattern == 0) && (*path == 0);

    // Advance past the multi-character wildcard(s). A sequence of asterisks is equivalent to a
    // single asterisk, and a sequence of ellipses and asterisks is equivalent to a single
    // ellipsis. We handle this here because many asterisks and ellipses in a row would yield
    // exponential (and pathological) runtimes.

    bool fEllipsis = false;

    while (IsMultiWildStr (pattern))
    {
        if (pattern[0] == L'*')
            pattern += 1;
        else
        {   pattern += 3;
            fEllipsis = true;
        }
    }

    // If the pattern ends in an ellipsis, then we trivially match any remainder of the path, so
    // return true, otherwise perform match testing.

    if (fEllipsis && (*pattern == 0))
        return true;

    // A multi-wild pattern (* or ...) followed by any number of slashes can match the empty string,
    // so we test for that here. Thus, ".../foo" will match against "foo".

    if (IsSlash(*pattern))
    {
        // Search forward past any number of trailing slashes.

        const wchar_t *ptr = pattern + 1;

        while (IsSlash(*ptr)) ++ptr;

        // Match the remainder of the pattern against the remainder of the path.

        if (pathmatch(ptr,path))
            return true;
    }

    if (fEllipsis)
    {
        // If we have an ellipsis, then recursively nibble away at the path to see if we can yield
        // a match, until we either match or exhaust the path.

        for (;; ++path)
        {
            if (pathmatch (pattern, path)) return true;
            if (*path == 0) return false;
        }
    }
    else
    {
        // If we have an asterisk, then recursively nibble away at the path until we encounter a
        // slash or exhaust the path.

        for (; *path && !IsSlash(*path);  ++path)
        {
            if (pathmatch (pattern, path)) return true;
        }

        // Test the remainder of the pattern and path.

        return pathmatch (pattern, path);
    }
}