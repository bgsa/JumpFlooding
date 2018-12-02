
How to Install

Windows Environment:

- Open the project on Visual Studio 2017
- Play!

Linux Environment:

# Install the libraries dependencies
sudo apt-get install cmake
sudo apt-get install xorg-dev
sudo apt-get install libglfw3
sudo apt-get install libglfw3-dev
sudo apt-get install libglew-dev

# Download the source code
git clone https://github.com/bgsa/JumpFlooding.git

# Build the project
./build.sh

# Run the project
./build/PixelEffectLinuxApp


How to Use

Press "v" voronoi diagram:
   - click on the screen in order to draw voronoi vertexes
   - click on "Enter" to draw voronoi diagram OR click on "M" to draw voronoi diagram with medial axis
   
Press "p" paint a shape:
   - click "Enter" in order to draw medial axis from shape
