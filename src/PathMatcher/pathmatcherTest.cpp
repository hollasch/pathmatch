#include <pathmatcher.h>

#include <fcntl.h>
#include <io.h>
#include <iostream>

using namespace std;


void testNormalizedPattern (const wstring& pattern) {
    wcout << L"\nInput:  (" << pattern << L")\n";

    auto normalizedPattern = PathMatchTest::testGetNormalizedPattern(pattern);

    wcout << L"Ouptut: ";
    for (auto& s : normalizedPattern) {
        wcout << L"(" << s << L") ";
    }
    wcout << L"\n";

}

static const wstring testPatterns[] {
    L"",
    L"/",
    L"\\",
    L"\\\\////\\\\\\",
    L"a",
    L"a\\",
    L"a/",
    L"a////",
    L"/a",
    L"/\\///\\\\\\a",
    L"/a/",
    L"/\\///\\\\\\a\\//\\\\",
    L"\\a",
    L"\\a\\",
    L"a/b/c",
    L"a/b/c/",
    L"a/b\\c/d\\e\\f",
    L"a/b\\c/d\\e\\f\\",
    L"a//\\\\/b//c",
    L"a//\\\\/b//c/",
    L"a/b/c/../x/y/z",
    L"a/b/c/../../x/y/z",
    L"a/b/c/../../x/y/z/",
    L"../../x/y/z",
    L"a/b/../../../../x/y/z",
    L"a/./b",
    L"a/./b/./././c",
    L"a/**/b/.../c/**",
    L"a/**/b/.../c/**/",
    L"a/**......****/b",
    L"a/**......****/b/",
    L"a.../b",
    L"...b/c",
    L"a...b",
    L"a...b...c",
    L"a...b...c/",
    L"a/b*.../c",
    L"a/b?.../c",
    L"a/...b/c",
    L"a/...*b/c",
    L"a/...?b/c",
};

int main() {
    _setmode(_fileno(stdout), _O_U8TEXT);

    for (auto pattern : testPatterns) {
        testNormalizedPattern(pattern);
    } 
}
