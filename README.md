# Game of Evolution
This is a for from the great repository of [biosim4](https://github.com/davidrmiller/biosim4)

At the moment the backend logic is identical to the original biosim4 repository.

Initial look of the UI
![doc1](https://github.com/artofimagination/game-of-evolution/resources/DocImg1.png)

# Intro
I found the very idea of mixing neural networks and genetic algorithms fascinating and was dabbling around with my own engine in the past. But biosim4 is basically doing everything I needed. I think there are numerous applications of this system and the only thing to do is to add the appropriate sensor/action neurons and matching challenges.

# Purpose of the fork
 - Adding a user friendly UI
 - Play around with existing challenges to understand the existing system
 - I'd like to reorganize the code for better scalability
 
 - implementing more complex challenges, like image processing/cooperative systems/conventional cell survival (cells eating each other), anything else that comes up in the mean time.

 # Setup
 - I am using Visual Studio code for development, so I include the complete setup for it. Everything shall happen in a Docker container. The build is using CMake and Qt 5.12.8 for the UI implementation. I created a custom ubuntu qt image (artofimagination/qt-linux-compiler:v1.0.2_qt5.12.8) to supply Qt for the container. Hoping to upgrade it to more recent version.
 
 VS Code steps:
 - Install extensions Docker (Microsoft), Remote-containers (Microsoft), C/C++ (Microsoft), CMake Tools (Microsoft), QML (Baptist Benoist)
 - Click in the green area in the bottom left corner (Open a Remote Window)
 - Reopen in container (Select from Dockerfile)
 - Run ```xhost local:docker``` in the host command line
 - If all goes well, you just need press F5 and it should start build and then the application with Debug.

# Troubleshooting
## Missing ```cppdbg```
 If your build does not start at all and VS Code is looking for ```cppdbg```, most likely you need to install C/C++ extension of VS Code within the container
## GUI is not starting
 Most likely you get this message if the container cannot start a gui
 ```
 qt.qpa.xcb: could not connect to display 
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: eglfs, linuxfb, minimal, minimalegl, offscreen, vnc, wayland-egl, wayland, wayland-xcomposite-egl, wayland-xcomposite-glx, xcb.
```

The caontainer shall be configured by the ```devcontainer.json``` to enable X11 forwarding, so most likley you forgot to run ```xhost local:docker``` in the host system command line.