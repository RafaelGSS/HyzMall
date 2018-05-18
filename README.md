#  HyzMall


## What's Hyz ?

Hyz it's only an mallware that run in background mode executing tasks requested by server administrator.
This requests is based in an API hosted in Heroku. (Because protocol HTTP and port 80 it's very sample!)

## How Hyz working ?

Hyz worked based in tasks, at server side (API) requested any task, the Hyz capture this request and execute task.

## What does Hyz do ?

### Hyz have the features:
- Desktop Capture ( Capture an ScreenShot )
- Webcam Capture
- Drop connection by X seconds (X is param)
- Show any image hosted in internet
- Open default browser in any url
- Keylogger ( capture keys in X seconds )


#### PS: I writen this application only for funny.
#### info_functions.h have all methods and your arguments

## Dependencies  Hyz to build: 

- OpenCV 2.4.13 ( only 3 dll )
- Boost 1.66
- zlib.dll
- libcurl.dll

## Important!

**HyzMall have another repositories, but these repositories are private. If you want see these repositories, send me a email**

Have:
- HyzMallAdmin ( View for administrator, capture the data from HyzMallApi and create tasks )
- HyzMallApi ( Server HTTP with routes and database )
- HyzMallInstalator ( Instalator of HyzMall that create a task on windows to execute the trojan all the day)


##### Todo
- Remove keylogger for prevent anti-virus.
- Use AES-Cripter forked.
