# random islands with raylib

Psuedo-random generation of landmass using perlin noise in [raylib](https://github.com/raysan5/raylib)

## Description

![screenshot.png](https://github.com/jmcrafty/random-islands-with-raylib/blob/main/screenshot.png?raw=true)

Code based on Playing with Perlin Noise: Generating Realistic Archipelagos by Yvan Scher on [Medium.com](https://medium.com/@yvanscher/playing-with-perlin-noise-generating-realistic-archipelagos-b59f004d8401)
[Direct link](https://yvanscher.com/2017-11-07_Playing-with-Perlin-Noise--Generating-Realistic-Archipelagos-b59f004d8401.html)

There are several settings to change the generated output
* land_threshold: Increase this value to have less total land above sea level
* land_radius: Increase this value to have a wider island area
* perlin_offset: Change this value to use a different area of the perlin image for rendering
* perlin_scale: Increase this value to create more island segments

For more information about the generation process check out the original blog post.

### Installing

* Compiles in VSCode
* If using raygui must be manually added your C:/raylib/raylib/src directory

## Version History

* 0.1
    * Initial Release
