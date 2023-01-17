![Irebot](imgs/icon.png)

<a href="https://discord.gg/RQttgV6Mmg"><img src="https://img.shields.io/discord/942146433652506714"></a>


## Building

  ### Prerequisites
   - MINGW
   - GCC
   - make


  ```
  git clone https://github.com/equitatyo/irebot.git
  cd irebot
  make
  ```
  
  You will need to move glfw3.dll into the build folder and arial_rounded.tff to build/assets/
  
## Usage
  Your clicks are placed in /build/player1
  
  Clicks structure should look like the following
  
  ```
  player1
  │
  └───clicks
  │   │   1.wav
  │   │   2.wav
  │   │   ...
  │   
  └───clicks
  │   │   1.wav
  │   │   2.wav
  │   │   ...
  │   
  └───releases
  │   │   1.wav
  │   │   2.wav
  │   │   ...
  │   
  └───softClicks
  │   │   1.wav
  │   │   2.wav
  │   │   ...
  │   
  └───softReleases
      │   1.wav
      │   2.wav
      │   ...
  
  ```
  
## Screenshots

![GUI](imgs/ex1.png)
  
## Contributors
  - equitatyo
