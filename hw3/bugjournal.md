# Bug 1

## A) How is your program acting differently than you expect it to?
- Fails to construct the FileIndexReader object

## B) Brainstorm a few possible causes of the bug
- The magic number isn't correct
- The re-calculated checksum doesn't match the header fields
- The length of the index file doesn't line up with the header fields

## C) How you fixed the bug and why the fix was necessary
- Leveraging the Verify333 assertion, we find the magic number is wrong at the very beginning
- There's no additional operations on data once they are stored as file form on disk. We also store the data properly with the ToDiskFormat() function
- The only reason is that we forget to recover the data from disk format to host format using ToHostFormat() after fread the file data (added at Line 43)


# Bug 2

## A) How is your program acting differently than you expect it to?
- Discover memory leak when running valgrind

## B) Brainstorm a few possible causes of the bug
- fail to manually free the heap-allocated data with hw1 C data structrues (like LinkedList, HashTable)
- fail to manually free the file relevant resources (like file descriptor)
- fail to manully free the self-designed c++ objects (we are not using smart ptrs here so objs have to be managed by callers)

## C) How you fixed the bug and why the fix was necessary
- In usual cases we will not forget to free those heap resources, but in some special cases like variable a == nullptr, we may forget to free those resources before breaking the loop or returning false
- Line 378 in WriteIndex.cc, we forget to free the LLIterator
- Line 101 in QueryProcessor.cc, we forget to free the heap-allocated obj that didtr is pointing to (If you check LookUpWord() function definition in IndexTableReader.cc, you will find the return value is actually a heap-allocated obj, which has to be manually freed by caller)


# Bug 3

## A) How is your program acting differently than you expect it to?
- Get warning message when trying to convert the type of payload in Line 394 in WriteIndex.cc

## B) Brainstorm a few possible causes of the bug
- We are using the C-style conversion style rather than C++ style
- We are using C++ style in a wrong way

## C) How you fixed the bug and why the fix was necessary
- After carefully walking through related documentations, we find we are using C++ style in a wrong way. We shall use reinterpret_cast to convert at pointer level and then use dereference
