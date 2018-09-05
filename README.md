# WaterRenderingApp

This is my implementation of three different water simulation algorithms. The application I've been working on for my master's thesis.

1) The first Algorithm is based on Distortion map for UV distortion of the coords used to sample refracted and reflected texture of the scene. Got some hints from [this](url=https://www.youtube.com/watch?v=HusvGeEDU_U) guy! 

![](https://media.giphy.com/media/3ojpoNvEfn0y8bj0Oh/giphy.gif)
![](https://media.giphy.com/media/fiqd9kNSDsW5ILi8rT/giphy.gif)
![](https://github.com/IonUreche/WaterRenderingApp/master/CUDA_Water_Simulation_Project/src/Media/Water1Gif.gif)

2) The second algorithm is using Gerstner Waves. The final wave is a sum of 4 Gerstner Waves.
Based on this paper from GPU Gems: https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch01.html

The texturing of the water in the gif below is for demo purpose only, and needs proper implementation.

![](https://media.giphy.com/media/vw8UdCREdLOIiJDRbf/giphy.gif)

3) The third algorithm is based on Inverse Fast Fourier Transform, and is based mainly on the explanation from this really well written paper: [LINK](https://tubdok.tub.tuhh.de/handle/11420/1439?locale=en)
The author also have some  youtube [videos](https://www.youtube.com/watch?v=B3YOLg0sA2g) with explanation, but I suggest reading the paper as it goes in more detail about each step of the algorithm.

![](https://media.giphy.com/media/5BWuFKYtU0r6fLyZDO/giphy.gif)

TO DO's:
1. Proper shading for water on algorithm 2 and 3
2. Better logic for choosing tesselation factor
3. Generater Texture space normal maps for Gerstner Waves implementation
