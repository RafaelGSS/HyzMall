#  HyzMall


## What is Hyz?

Hyz is malware that runs in the background, executing tasks requested by a server administrator.
These requests are based on an API hosted by Heroku. (Because the protocol is HTTP and the port is 80, it's very simple!)

## How does Hyz work?

Hyz is based on tasks. The server (API) request any task. The Hyz client captures this request and executes a task.

## What does Hyz do?

### Hyz has the following features:
- Desktop Capture ( Capture a ScreenShot )
- Webcam Capture
- Drop connection by X seconds (X is param)
- Show any image hosted in internet
- Open default browser in any url
- Keylogger ( capture keys in X seconds )


#### PS: I wrote this application only for fun.
#### info_functions.h have all methods and your arguments

## Dependencies needed to build Hyz:

- OpenCV 2.4.13 ( only 3 dll )
- Boost 1.66
- zlib.dll
- libcurl.dll

## Important!

**HyzMall has other repositories, but these repositories are private. If you want see these repositories, send me a email**

Have:
- HyzMallAdmin ( View for administrator, capture the data from HyzMallApi and create tasks )
- HyzMallApi ( Server HTTP with routes and database )
- HyzMallInstalator ( Instalator of HyzMall that create a task on windows to execute the trojan all the day)


##### Todo
- Remove keylogger for prevent anti-virus.
- Use AES-Cripter forked.
