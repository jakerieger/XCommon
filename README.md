# xCommon

xCommon is a collection of utility source files for C++20. It includes utilities for date & time,
filesystem operations, timers, strings, and convenience files such as simplified typedefs and macros.

This was developed primarily for my own use, but I decided to share it incase anyone else finds it useful.

No compilation is required, this is not a library. Just include the headers in your project and that should be it.
The only file that needs to be compiled is `Filesystem.cpp`, which can simply be added to your CMake sources
(or whatever build system you're using).

## Examples

### Reading a file from disk
```cpp
#include <Typedefs.hpp>
#include <Filesystem.hpp>

void ReadSomeFile() {
    using namespace x;
    
    Path myFile("my_file.txt");
    
    // Read as a string
    str fileContents = FileReader::ReadText(myFile);
    
    // Read as bytes
    vector<u8> fileBytes = FileReader::ReadBytes(myFile);
}
```

### Writing a file to disk
```cpp
#include <Typedefs.hpp>
#include <Filesystem.hpp>

// TEXT
void WriteTextToFile() {
    using namespace x;
    
    Path myFile("my_file.txt");
    str fileContents = "This is the contents of my file";
    
    // Will return false if it failed to write
    bool result = FileWriter::WriteText(myFile, fileContents);
}

// BYTES
void WriteBytesToFile() {
    using namespace x;
    
    Path myFile("my_file.txt");
    vector<u8> fileContents {0x0, 0x56, 0x44, 0x77, 0x88};
    
    bool result = FileWriter::WriteBytes(myFile, fileContents);
}
```

### Using the Timer class
```cpp
#include <Typedefs.hpp>
#include <Timer.hpp>

void TimedFunction() {
    using namespace x;
    
    // Timer starts at construction unless .Reset() is called
    Timer timer;
    
    // Do some work that takes time
    DoWork();
    
    // Get elapsed time in seconds
    f32 elapsedTime = timer.Elapsed();
    
    // Get elapsed time in milliseconds
    f32 elapsedTimeMillis = timer.ElapsedMillis();
}

void ScopedTimerFunction() {
    using namespace x;
    
    // ScopedTimer takes a name at construction
    ScopedTimer scopedTimer("ScopedTimerFunction()");
    
    // Do some work that takes time
    DoWork();
    
    // ScopedTimer will automatically deconstruct itself when out of scope
    // It will print the provided name and elapsed time in milliseconds to stdout
}
```

### Using DateTime class
```cpp
#include <Typedefs.hpp>
#include <DateTime.hpp>

void DateTimeFunction() {
    using namespace x;
    
    // Get and store the current date and time
    DateTime dateTime = DateTime::Now();
    
    // Returns a string with the format: YYYY-MM-DD HH:MM:SS AM/PM
    str utcString = dateTime.UTCString();
    
    // Returns a string in the same format as .UTCString(), but local to current time zone
    str localString = dateTime.LocalString();
    
    // Returns a string in the format: YYYY-MM-DD
    str dateString = dateTime.DateString();
    
    // Returns a string in the format: HH:MM:SS AM/PM
    str timeString = dateTime.TimeString();
}
```

There is a ton more available among the different utility headers, but that is a quick introduction to get
you started.

I'll be updating and adding more comments and documentation to this in the near future.