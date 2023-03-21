# Bug 1

## A) How is your program acting differently than you expect it to?
- Dead loop in InsertContent() function

## B) Brainstorm a few possible causes of the bug
- forget to set the break condition
- break condition is not set correctly
- cur_ptr is not updated correctly

## C) How you fixed the bug and why the fix was necessary
- I found that the cur_ptr is not updated during the loop, which is the cause
- after writing the complex if else logics, it is easy for us to forget updating the cur_ptr


# Bug 2

## A) How is your program acting differently than you expect it to?
- Assertion of Verify333(ret==1) isn't satisfied where ret is the return value of HashTable_Insert() function

## B) Brainstorm a few possible causes of the bug
- HashTable_Insert didn't insert kv pair successfully
- I misunderstood the meaning of the return value

## C) How you fixed the bug and why the fix was necessary
- It turns out that I misunderstood the true ret value to be inserting successfully. The actual meaning of true ret val is that there already exists the same key inside the hashtable. So for here, we shall use Verify333(ret==false) instead


# Bug 3

## A) How is your program acting differently than you expect it to?
- For the searchshell, when passing in space OR multiple spaces OR \n, the program crashes

## B) Brainstorm a few possible causes of the bug
- didn't consider such cases

## C) How you fixed the bug and why the fix was necessary
- when detecting such input, continue for the loop, which behaves the same as the sample solution


# Bug 4

## A) How is your program acting differently than you expect it to?
- Segmentation fault when running Test_CrawlFileTree.*

## B) Brainstorm a few possible causes of the bug
- Have uninitialized pointers and attempte to access them.
- Make a pointers equal to NULL and try to dereference them.

## C) How you fixed the bug and why the fix was necessary
- I found `i` is not only used as index in the first loop, but also indicates the number of entries using the line `num_entries = i`.
- I added line `i--` before each `continue` and `break`, to indicate there is no entry added in these lines. And it worked.