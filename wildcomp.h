#pragma once
//==================================================================================================
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

#if !defined _WILDCOMP_H_
#define _WILDCOMP_H_


    // Function Declarations

inline bool IsEllipsis  (const wchar_t *str);
inline bool IsMultiWild (const wchar_t *str);

bool wildcomp  (const wchar_t *pattern, const wchar_t *string);
bool wildcompc (const wchar_t *pattern, const wchar_t *string);
bool pathmatch (const wchar_t *pattern, const wchar_t *path);

    // Function Definitions

// Return true if and only if the string begins with "...".

inline bool IsEllipsis (const wchar_t *str)
{
    return (str[0] == L'.') && (str[1] == L'.') && (str[2] == L'.');
}

// Return true if and only if the string begins with a wildcard that matches
// multiple characters ("*" or "...").

inline bool IsMultiWildStr (const wchar_t * str)
{
    return (*str == L'*') || IsEllipsis(str);
}

// Return true if and only if the string begins with a wildcard.

inline bool IsWildStr (const wchar_t * str)
{
    return (*str == L'?') || IsMultiWildStr(str);
}

// Return true if and only if the character is a forward or backward slash.

inline bool IsSlash (const wchar_t c)
{
    return ((c == L'/') || (c == L'\\'));
}

#endif