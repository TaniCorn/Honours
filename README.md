# Honours - To Update
Non-polygonal rendering of voxels. Using raycasting and raymarching and comparing these two techniques with two acceleration structures. Using DirectX11, C++, and ImGui.

![image](https://github.com/TaniCorn/Honours/assets/63819551/c52cab8b-8cf1-44cf-93c2-1a5d265e6063)
[Source: John Lins Voxel Water Physics](https://youtu.be/1R5WFZk86kE?si=qBwCZG7cqMKTXDgh)

# Summary
Below is a quick high level overview of my research goal, method, and technologies. Below the summary you'll find more detailed descriptions.

## Goal
My research goal is to determine which non-polygonal rendering technique works best with which of the two acceleration techniques?

## Method
My method for doing this will be comparing each technique in 3 different scenarios:
- A static Scene - Only the camera can move
- Dynamic Scene - Voxels can move(animation)
- Data dynamic scene - Voxels will be added and deleted to the original data set on runtime.

This will result in 2x2x3 scenarios; 12 different scenarios in total.
In each scene I will be comparing the following:
- Average Frames Per Second
- Lowest Frames
- Highest Frames
- 99% FPS
- Frame Completion time(in ms)
- Physical Ram Usage/Virtual Ram Usage
- Graphics Card Memory Usage
- GPU Load(Usage percentage)
- CPU Load(Usage percentage)
- Acceleration structure size

This will be tested on differnt machines with different specs:
- RTX 3070, Ryzen 7 2700x, 16GB 1600MHz Ram
- GTX 1060, Ryzen 7 2700x, 16GB 1600MHz Ram
- TBC
## Technologies
The rendering techniques I'll be using are Raytracing and Raymarching. Using both Octree and BVH accelleration structures to compare the two of them.

I'll be using DirectX11 with my universities framework as a base. Everything will be coded in C++, and HLSL.


# Origin
Coming around to my 4th year I had 4 main passions: Networking architecture, Procedural Generation, VR, and Voxels. I was teatering on Procedural Generation, wanting to do something like no-mans sky and knew a little bit about marching cubes. However I got reminded of an old game I used to play on the Xbox 360, from the Indie store. 
Total Miner: Forge
![image](https://github.com/TaniCorn/Honours/assets/63819551/c8646a1d-04c7-4e27-91e4-550f1b482f16) <- [Total Miner: Forge Demo Gameplay from Kalzec.](https://youtu.be/yllvLnK3VNk?si=UEMnqUFnmpzY-_eb)

I loved that voxel look, especially when it came to using the guns, the way that tiny little voxels would split from the main block was very endearing to my young eyes.
That blast from the past pushed me to want to do Voxels, and as I researched more, I found that it was the graphical technique of voxels I wanted to research.

Having a look at a bunch of inspirations such as [John Lins Crystal Islands](https://youtu.be/8ptH79R53c0?si=uuXEO0qqirxyITkA) , [Tooley1998 lovely voxel world](https://youtu.be/y1u9QCRDO5g?si=Awye-SkP5BNZGRWC) and [voxelbee's fantastic rendering capabilities](https://youtu.be/i7vq-HY10hI?si=UHoJ406ZrGhRe4Gt), I was fully inspired to take that leap.

## Polygons vs no polygons
So why am I specifically targeting non-polygonal research? Why not do it the way minecraft does it? Or No Mans Sky? Well, there are limitations. Suppose I want to achieve the small voxel look, imagine that for every tiny voxel it was drawn via a cube. That's 8 vertices per voxel. What happens if I have 10 million voxels in my viewport? That's 80 million vertices, drawn with triangles is 120 million triangles. That's a lot! So what do we have to consider here? Well that's the question.
![image](https://github.com/TaniCorn/Honours/assets/63819551/d0e55adc-aac7-4cab-8ad5-75c886862876) [Voxel Ray Tracing](https://youtu.be/gXSHtBZFxEI?si=btAyvhFp0UE2vFiE)

Of course there's a bunch of ways to reduce the load on polygons such as Greedy Mesh.


## How do we scale the voxels down, and increase their amount, without having to process so much more?
I'll tell you how, non-polygonal rendering. With it, it doesn't matter how many voxels are on our screen, we can't render more than our resolution dictates. Got a 1920x1080 pixel monitor? Raycasting will only compute the 2 million ish pixels. So all it leaves it how we can trim out the unnecessary voxels from our data. That's where accelleration structures come in.

We have a boatload to choose from:
- Octrees
- Bounding Volume Hierarchies
- Splat
- k-d trees
- Sparse Voxel Octrees
- *Efficient* Sparse Voxel Octrees
- Sparse Voxel DAGS
- Cascading Voxel Grids
- SparseLEAP
- GIGAVOXELS?

<sup> Ranting ahead </sup>

That's not even mentioning the confusing subject of the efficiency of other things and how they interact. I spent a long time being confused with Raymarching, Raycasting, Raytracing, Signed Distance Functions/Fields and how they all mesh together. Then of course you have research on the other end of the spectrum for triangles such as Dual Contouring, Instancing, Geometry Shader(which is still relatively new). And then there's the consideration for what graphics api to use, apparently some algorithms run better on RTX cards, some on Vulkan, some on DX12 vs DX11.

**There's a boatload of information.** Not all of them are correct, not all of them are needed. I found it was very hard to focus on what area, when I think "Well what if someone already has an answer? I don't want to research x and be told actually if I did some more research I would have found article y says that z is much better than x, why didn't I research more and do this instead?". That could just be me overthinking but I don't want to implement a sub-optimal algorithm and be told, by future self or otherwise, that I could have chosen a better comparison of algorithms. Even now I might have chosen wrong. However as I'm writing this my feasibility deadline is coming in and I have to choose. So with all this drival out of the way, what am I doing?

## Bounding Volume Heirarchies vs Octrees!
When it comes down to it, I don't have 2-3 years to do this. I have less than 9 months to research, implement, test, and evaluate. I want to keep researching about what's the best thing to implement, learn a whole new graphics API, and slowly build it from the ground up. However with my current skill level in programming, and with all the other stuff I'm doing(Shameless plug ahead) <sup> I'm starting a video games company called Afterfire Studios, and running it part-time, check us out!</sup>. 
That's just not possible, so I've chosen the two most promising structures that have the potential to be improved upon.

Check out the technical deets below.

# The technical details
Below are the technical details of the technologies I've chosen in a way that makes sense to me. Graphics is like a web that never untangles, so I've listed it out in a way that will hopefully clearly draw a line between what things are and what they are not.
I wish I could encompass every detail of what I've researched but I can only give a quick summary overview of what each thing can do.

## Rendering Technique and Voxel Intersection technique
I'm researching two techniques, Raymarching and Raytracing.

With the Raymarcher, I plan on using Signed Distance Functions to traverse space and create shapes in respect to the voxel points. I'll probably have to look at encoding the coordinates using Mortan Codes.
With the Raytracer I'll be using intersection functions for now.

If I find any better ways to render the shapes I will update this.
Something to clear up is that Raytracing, Raymarching, Raycasting are all different. 
- Raycasting is the process of shooting a ray out into space.
- Raytracing is where you use raycasting, but continue the ray via bouncing, or refraction.
- Raymarching is where you continue in increments in space finding out the distance between your point and other points. This is better done with Sphere tracing,, which eliminates the constant step.

## Voxel Storage / Acceleration Techniques

This is where I get a bit confused, I'll be coming back to tidy this up in the future when I understand it a bit more.
The most basic way to store voxels is in an array, a 3D array to specify whether a voxel is filled or not. This is ok for storing the world, but innefficient when it comes to sending to the GPU to render.
I was under the impression that Octrees and BVH's are not storing them but sorting them? However the more I try to understand it the more I think that they are storing them. So don't quote me around this area because I may be wrong.
There are others I've considered but ruled out such as Signed Distance Fields, and 3D Textures.

**3D Array**

Initially I will use a simple 3D array to store voxels and say if they are on or off.

**Octrees -> Sparse Voxel Octrees -> Efficient Sparse Voxel Octrees -> Sparse Voxel DAGS**

Octrees seem to have a never ending way to keep going deeper and deeper... yes that was a bad joke.
But this progression seems to constantly improve upon eachother.

**Bounding Volume Heirarchies**

After I ruled out k-d trees and Gigavoxels, I decided on the plain old BVH. It's known to work well and is known to work better for dynamic objects. If it can handle rendering as well, it will give Octrees a run for it's money considering they apparently do not do well with dynamic scenes.

**I've chosen DirectX11 for my familiarity with it.**

I would have liked to use Vulkan or DirectX12. 
Vulkan would be great to test out John Lins claim that BVH's run much better than SVO's on RTX and Vulkan cards due to their hardware support.
![image](https://github.com/TaniCorn/Honours/assets/63819551/c4cbb303-9e7e-4a68-b007-842a360b4b14)
DirectX12 would be great because of it's flexibility and Raytracing pipeline.

# Research and Inspirations
Below I've listed some key inspirations and research that I've done to collect my information.

TBD

