# solar-system-opengl
# 3D Solar System Simulation using OpenGL

This project is a 3D simulation of the solar system built with C++ using OpenGL and SOIL for texture mapping. Each planet orbits the sun with realistic color and movement, and textures are applied to enhance visual quality.

## Features
- 3D camera movement with orbit control
- Rotating planets with individual orbit speeds
- Starfield background
- Realistic orbits using `GL_LINE_LOOP`
- Texture loading with SOIL

## Technologies
- OpenGL (GLUT)
- SOIL (Simple OpenGL Image Library)
- C++

## Controls
- Arrow keys to rotate the camera
- +/- to zoom
- Space to pause/resume planet motion

## Installation
1. Make sure you have OpenGL and SOIL installed.
2. Compile with:
   ```bash
   g++ main.cpp -lGL -lGLU -lglut -lSOIL -o SolarSystem

