# Bug 1

## A) How is your program acting differently than you expect it to?
- When running make, often time there are error happening

## B) Brainstorm a few possible causes of the bug
- makefile goes wrong (least possible)
- my implementation on the source file goes wrong
- there are some previous cache/output make files that affect the present make process

## C) How you fixed the bug and why the fix was necessary
- Even if my implementation goes wrong, it shouldn't cause the make process fail. Make shall run well and show the errors to me.
- So the third reason in B) is very likely. I tried 'make clean' before each 'make', and that works.


# Bug 2

## A) How is your program acting differently than you expect it to?
- When running test_suite on LinkedList part, a case fails. I looked up in the test_linkedlist.cc and found the assertion at line 212 causes the failure

## B) Brainstorm a few possible causes of the bug
The assertion said value should be 0x02, but I have 0x01
- The value is updated, but not updated correctly
- The value is not updated (forget to update)
- Some other value is mistakenly updated

## C) How you fixed the bug and why the fix was necessary
- I checked my code and found I have the implementation for update. Then I found that cuz Slice func is similar to some previous func, I just copy and paste the code and make some modifications, during which time I forget to modify "head" to "tail".
- I have to be more careful next time 


# Bug 3

## A) How is your program acting differently than you expect it to?
- There are some memory leak when running valgrind, in HashTable_Find() func

## B) Brainstorm a few possible causes of the bug
- The only allocating behavior I have in this HashTable_Find() func is calling LLIterator_Allocate() func, so I think the cause is that I forget to free iterator

## C) How you fixed the bug and why the fix was necessary
- I call LLIterator_Free() to free the iterator when it is no longer used. That fixes the memory leak.


# Bug 4

## A) How is your program acting differently than you expect it to?
- When trying to run the example program and test suite for hash table, it turns out they both break the assertion of verify333(iter->node != NULL) in LLIterator_Get. Simply commenting out the assertion won't eliminate the bugs, but further cause seg fault.

## B) Brainstorm a few possible causes of the bug
- I first try to locate what related modules/functions are involved. Seeing that the InsertFindRemove test module in the test suite fails. Then I try to locate exactly which one of Insert/Find/Remove func is causing the bug of the assertion of LLIterator_Get().
- I add some printf and breakpoints in example program, and figure out Find func has bugs
- Then I try to see what other funcs inside Find also call LLIterator_Get(). Through trials, find that when HashTable_Find() calls the helper func IsKeyFound() which further calls LLIterator_Get(), sometimes verify333(iter->node != NULL) breaks.
- I think there shall be some bugs within my IsKeyFound() func

## C) How you fixed the bug and why the fix was necessary
- I turned to TA for help on Ed, and realized that I'm using LLIterator_Next() as a check func used for checking whether iter->node has a next node, but neglecting its other functionality of physically moving iter->node to the next node. So when you are at the penultimate node, you check there is a last node existing, and then I repeatedly call the LLIterator_Next() func which brings the iter->node to NULL
- I think next time I should pay attention to the func that has "double identities", like here LLIterator_Next not only tells us whether the next node exists but also physically move the iter->node points to the next node(can be NULL). Maybe it's a better idea to have two APIs that separate these two functionalities.
- Also during the process of debug, I shall pay more attention to the values of different variables, which will help me troubleshoot


# Bug 5

## A) How is your program acting differently than you expect it to?
- When running the test_suite for hash table, the Test_HashTable.Iterator tells me the expected ret val of HTIterator_IsValid shall be true, but mine is false

## B) Brainstorm a few possible causes of the bug
- The intuition is that my checking condition is wrong.
- My checking conditions seem to be too strict, which makes the expected valid to be invalid

## C) How you fixed the bug and why the fix was necessary
- I modify the condition to be just one rule based on the bucket_idx == INVALID_IDX
- Wherever the iter is invalid, I will set the bucket_idx to be INVALID_IDX
- Always try to use as few vars as possible for checking, which reduce potential bugs
