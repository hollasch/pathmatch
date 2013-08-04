#pragma once
/*==============================================================================
    Released to the public domain.

    Steve Hollasch <steve@stevehollasch.com>, 2005 May 27
==============================================================================*/

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