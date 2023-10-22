# TabCounter

I wanted to display my browser tab count on my discord status because I thought it would be funny.

## How it works
### TabCounterExtension
This is a firefox extension that counts the number of tabs, encodes it into a url request, and then sends to port 4455. I use a web request because basic JavaScript doesn't easily support direct byte transmission through sockets.

### PythonRedirectServer
I didn't want to deal with compiling a web server because all I needed to do was extract part of the URL from the request and then forward the payload (just an integer encoded in a 4-byte message) to port 5566.

### DiscordUpdater
This is the the discord script that acts as a game to hijack the game activity area on the discord profile. By listening for UDP messages on port 5566, it can keep track of the current count of open tabs and forward that information to the Discord client running on the local computer.


## How to run
### TabCounterExtension
I simply added this extension to firefox as a debug/testing extension.

### PythonRedirectServer
The only dependency is Flask. After installing it, simply execute the script.

### DiscordUpdater
I used Visual Studio to compile the project and followed the setup instructions provided by the Discord SDK documentation to run the project.