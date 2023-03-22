# Mini-Google_Web_Server
A mini-Google web server available for multiple key words searching in file system via web page  
Combination of C++ and C and POSIX
<br><br>

## Usage Instruction
**(1) First initiate the web server.**  
You can launch the server on either your own local machine/virtual machine, or a remote machine. If you want to launch it on a remote machine, just simply clone this repository to that machine.  
Enter into the directory of hw4 with console, put in the following command:  
"  
**./http333d 5555 ../projdocs unit_test_indices/***  
"  
http333d means the binary for the web server  
5555 means the port number (you can switch to another if you like)  
projdocs is the directory storing all the local files  
unit_test_indices is for testing  

If successfully launching the server, you will see something like this:  
<img width="671" alt="截屏2023-03-21 下午6 02 28" src="https://user-images.githubusercontent.com/58315637/226775191-bbcd36dd-4c8b-4855-a62f-266f7d8fff28.png">  

**(2) Next initiate the client side via browser**  
If you are running the code on a **local computer or the VM**, launch a browser on that machine and open http://localhost:5555/ and http://localhost:5555/static/bikeapalooza_2011/Bikeapalooza.html in different tabs, changing the 5555 to the port you specified when launching http333d  
If you are running the code on **remote machine (eg. attu)**, note which specific machine you are running the web server on (e.g., attu4) and open http://attu4.cs.washington.edu:5555/ and http://attu4.cs.washington.edu:5555/static/bikeapalooza_2011/Bikeapalooza.html in different tabs, changing the attu number and port number as needed.  

### Congratulations! Now you've done for all the set up parts!

<br><br>

## What it looks like at client side

<img width="1826" alt="截屏2023-03-21 下午5 24 29" src="https://user-images.githubusercontent.com/58315637/226771084-33ef64a5-de8b-4e7c-86be-7ae0b37c8be6.png">

From the web page above, we have a simple front end page with similar style to Google. The textbox allows user to input multiple words they are interested in. After clicking on the "search" bar, the user sends HTTP request to the web server. The web server will parse the request, grab the data from local disk based on the request, generate response accordingly and send it back to the client side. The results are shown on the left side of the page.  
**The results include:**  
(1) The number of files that contain those words  
(2) The word query the user inputs  
(3) The absolute path of the file (clickable for scanning the whole file)  
(4) The number in [] indicates the number of times each query word appears in the file. The higher the number is, the more front the file shows up.
<br><br>

## What happens on server side
The general idea is that we build almost every modules from scratch, like **data structures, file system crawler and indexer, search engine, on-disk index, network socket, threadpool, HTTP parsing and generating, security issues**. These modules are developed in a reasonable order, from HW1 to HW4. Each HW is based on the previous HW implementations. HW4 will be the final version, i.e. the Mini-Google Web Server.  

We also pay huge attention to **architecture design, code style, code readability, error handling and memory management.**

(1) HW1 : Build complete LinkedList and HashTable modules from scratch【 C 】  
(2) HW2 : Build file system crawler and indexer (inverted index) as well as the search engine【 C / POSIX 】  
(3) HW3 : Move the inverted index to an on-disk index in an architecture-neutral format (able to store from memory to disk and restore from disk to memory) 【 C++ / POSIX 】  
(4) HW4 : Complete a multithreaded web server (network socket, threadpool, HTTP parsing and generating) and relevant front end for the user to interact with server. Fix some security problems ("cross-site scripting" flaw, directory traversal attack). 【 C++ / POSIX 】  

