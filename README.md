# Mini-Google_Web_Server
A mini-Google web server available for multiple key words searching in file system via web page

## What it looks like at client side

<img width="1826" alt="截屏2023-03-21 下午5 24 29" src="https://user-images.githubusercontent.com/58315637/226771084-33ef64a5-de8b-4e7c-86be-7ae0b37c8be6.png">

From the web page above, we have a simple front end page with similar style to Google. The textbox allows user to input multiple words they are interested in. After clicking on the "search" bar, the user sends HTTP request to the web server. The web server will parse the request, grab the data from local disk based on the request, generate response accordingly and send it back to the client side. The results are shown on the left side of the page.
The results include:
(1) The number of files that contain those words
(2) The word query the user inputs
(3) The absolute path of the file (clickable for scanning the whole file)
(4) The number in [] indicates the number of times each query word appears in the file. The higher the number is, the more front the file shows up.
