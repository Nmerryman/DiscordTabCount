# TabCounter

I wanted display my browser tab count on my discord status because I thought it would be funny.

## How it works
### TabCounterExtension
This is a firefox extension that counts the number of tabs and then encodes it into a url request then sends to port 4455. We send a web request because basic JavaScript doesn't easily support directly sending bytes through sockets.

### PythonRedirectServer
Because I didn't want to deal with compiling a web server and all I need to do is extract part of the url from the request and then forward the payload (Just an int encoded in a 4 byte message) to port 5566.

### DiscordUpdater
This is the the discord script that acts as a game to hijack the game activity area on the discord profile. By listening for UDP messages on port 5566, it can keep track of the currunt count of tabs open and forward that information to the open discord client on the local computer.


## How to run
### TabCounterExtension
I simply added this extension to firefox as a debug/testing extension.

### PythonRedirectServer
The only dependency is Flask. After that simply execute the script.

### DiscordUpdater
I used Visual Studio to compile the project file to run the project. I followed the setup instructions given by the discord sdk docs.