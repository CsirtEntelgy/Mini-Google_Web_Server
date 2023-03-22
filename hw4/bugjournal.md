# Bug 1

## A) How is your program acting differently than you expect it to?
- Compiler error when modifying the private member variable headers_ outside the class definition

## B) Brainstorm a few possible causes of the bug
- If we want to access private member variables, we often use self-designed getter method for that var
- If we want to modify private member variables, we often use self-designed setter method for that var

## C) How you fixed the bug and why the fix was necessary
- Here we are already provided with a setter method for the headers_ private member variable, so we can just use that
- It is always better to make member variables as private and create certain setter/getter as the entry point for the client to interact with them, which is safer and clearer


# Bug 2

## A) How is your program acting differently than you expect it to?
- Get memory leak for the listen file descriptor in ServerSocket.cc

## B) Brainstorm a few possible causes of the bug
- The caller forgets to close the listen file descriptor when normally done with it
- The caller forgets to close the listen file descriptor when error happens
- The close() function is not functioning correctly

## C) How you fixed the bug and why the fix was necessary
- It turns out that I try to transfer all the cerr to the Verify333 based on the specs saying "You don't need to worry about propagating errors back to callers in all situations. You will use Verify333()'s to spot errors and cause your program to crash out if they occur"
- With that transfer to Verify333(), there's no chance for developers to close relevant resources when error happens. Therefore, in some cases like here (line 120), we shall still use cerr. 


# Bug 3

## A) How is your program acting differently than you expect it to?
- Didn't pass the unit test of HttpUtils.cc, to be specific, the EscapeHtml()

## B) Brainstorm a few possible causes of the bug
- not replacing the original symbols with right symbols
- not replacing the right original symbols
- not replacing them in correct order

## C) How you fixed the bug and why the fix was necessary
- It is important that "&" should always come first to be replaced, because other symbols like "\'", "<" will all be replaced by something that contain "&". We don't want to replace the "&" twice.
