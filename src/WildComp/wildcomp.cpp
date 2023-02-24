//==================================================================================================
// wildcomp.cpp
//
// Definition of the `wildComp` (fileglob) function. This function performs string matching against
// path (fileglob) patterns containing `?`, `*`, `**`/`...` tokens.
//
//                                                                Copyright 2010-2023 Steve Hollasch
//==================================================================================================
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

#include "wildcomp.h"

using std::wstring;


namespace { // File-local definitions

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

}; // End anonymous namespace


bool wildComp (const wstring& pattern, const wstring& str) {
    // Performs a case-sensitive comparison of the string pattern against the string str.
    // See below for further details.

    return wildComp (pattern.cbegin(), pattern.cend(), str.cbegin(), str.cend());
}
