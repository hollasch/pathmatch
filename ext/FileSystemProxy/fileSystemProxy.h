//==================================================================================================
// FileSystemProxy
//
// Declarations for the file system proxy classes. These objects proxy the file system of the
// underlying operating system, or test harness.
//
// _________________________________________________________________________________________________
// Copyright 2015 Steve Hollasch
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under
// the License.
//==================================================================================================

#ifndef _fileSystemProxy_h
#define _fileSystemProxy_h

    // Includes

#include <string>


namespace FSProxy {



class DirectoryIterator {
    
    // This abstract base class provides a way to iterate through file & directory entries in a
    // directory.

  public:
    DirectoryIterator() {}
    virtual ~DirectoryIterator() {}

    // Advance to first/next entry.
    virtual bool next() = 0;

    // True => current entry is a directory.
    virtual bool isDirectory() const = 0;

    // Return name of the current entry.
    virtual const wchar_t* name() const = 0;
};



class FileSysProxy {

    // This abstract base class provides a general file system interface across different file
    // systems, including test harnesses.

  public:
    virtual ~FileSysProxy() {}

    virtual size_t maxPathLength() const = 0;

    // Return a directory iterator object. NOTE: User must delete this object! It is recommended
    // that you hold the return value in a unique_ptr<>.
    virtual DirectoryIterator* newDirectoryIterator (const std::wstring path) const = 0;

    // Set the current working directory. Returns false if the directory does not exist.
    virtual bool setCurrentDirectory (const std::wstring path) = 0;
};



};  // namespace FileSystemProxy


#endif   // ifndef _fileSystemProxy_h