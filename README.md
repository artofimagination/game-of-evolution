# Content
* [Intro](#intro)<br>
* [Purpose of the fork](#purpose-of-the-fork)<br>
* [Setup](#setup)<br>
* [Troubleshooting](troubleshooting)<br>
* [Quick Guide](quick-guide)<br>

# Game of Evolution
This is a fork from the great repository of [biosim4](https://github.com/davidrmiller/biosim4)<br>
Main branch of this repo is _game_of_evolution_. _main_ is the maintenance branch to grab upstream changes from the original biosim4 repo.<br><br>

This repo has a major overhaul of the code into a new structure. The core logic has not been changed and will be kept in original-biosim4-core branch, for the keen users, who wants to have the same behavior as the original biosim4 repo, but with a UI interface.<br>
TODO:
  - Video generation feature is not yet migrated and is not working at the moment
  - There is no logging migrated yet, so it is not functional either

Current look of the UI
![doc1](https://github.com/artofimagination/game-of-evolution/blob/game_of_evolution/resources/DocImg1.png)

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

The container shall be configured by the ```devcontainer.json``` to enable X11 forwarding, so most likley you forgot to run ```xhost local:docker``` in the host system command line.

# Quick Guide
## Main page
There is only a single main page at the moment. If you are interested in the backend of the system, checkout [biosim4](https://github.com/davidrmiller/biosim4)<br>
![doc1](https://github.com/artofimagination/game-of-evolution/blob/game_of_evolution/resources/DocImg1.png)

* **World Canvas** - canvas to visualize the peeps (entities of this world), challenge layer and barriers
* **Tabs** - There are three tabs available for now. _Details_, _Analytics_, _Neural Network_

## Detail Tab
* **Details** - gives details about the simulation (there will be more over time)
     - _Generation_ - current generation count
     - _Sim Step_ - current simulation step
     - _Max Population_ - the maximum allowed population

* **Settings** - allows to configure the simulation
     - _Sensors_ - the user can select one or more sensors. Only the selected ones will be used during the genome generation
     - _Actions_ - the user can select one or more actions. Only the selected ones will be used during the genome generation
     - _Select All_ - allows the user to select every sensor and action at once
     - _Update_ - Sensor action changes will apply ony if Update is pressed. This also will reset and restart the simulation
* **Challenges** - challenges selector. When a new challenge is selected the simulation and analytics will restart.

![doc1](https://github.com/artofimagination/game-of-evolution/blob/game_of_evolution/resources/DocImg2.png)

## Analytics Tab
Allows the user to check some statistics. At the moment only a few analytics are available and are displayed in a single line chart (single analytics on the same chart).

![doc1](https://github.com/artofimagination/game-of-evolution/blob/game_of_evolution/resources/DocImg3.png)

## Neural Network Tab
Displays the neural network. **TBD**
