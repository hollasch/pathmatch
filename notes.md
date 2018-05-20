Pathmatch Notes
====================================================================================================

# std::filesystem Functions

  * `fs::is_directory (std::string(d))`
  * `fs::is_symlink (fs::path s)`
  * `fs::current_path()`
    - `fs::exists (fs::path p)` (`1`)
    - `fs::path.root_name()` (`C:`)
    - `fs::path.root_path()` (`C:\`)
    - `fs::path.relative_path()` (`Windows\system.ini`)
    - `fs::path.parent_path()` (`C:\Windows`)
    - `fs::path.filename()` (`system.ini`)
    - `fs::path.stem()` (`system`)
    - `fs::path.extension()` (`.ini`)
    - `p /= "subdir"` (Append subdir to path with separator)
    - `p += "subdir"` (Concatenate subdir to path without separator)

# Components

    - The `path` object
    - `directory_entry`
    - Directory iterators
    - Misc support functions

# Directory Iteration

```C++
void DisplayDirTree(const fs::path& pathToShow, int level)
{
    if (fs::exists(pathToShow) && fs::is_directory(pathToShow))
    {
        auto lead = std::string(level * 3, ' ');

        // Note: directories '.' and '..' are skipped in the iterator.
        // See also recursive_directory_iterator.

        for (const auto& entry : fs::directory_iterator(pathToShow))
        {
            // entry is a std::path. It has .path(), .status()

            auto filename = entry.path().filename();
            if (fs::is_directory(entry.status()))
            {
                cout << lead << "[+] " << filename << "\n";
                DisplayDirTree(entry, level + 1);
                cout << "\n";
            }
            else if (fs::is_regular_file(entry.status()))
                DisplayFileInfo(entry, lead, filename);
            else
                cout << lead << " [?]" << filename << "\n";
        }
    }
}
```
