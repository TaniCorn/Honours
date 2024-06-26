# Honours - To Update
Non-polygonal rendering of voxels. Using raycasting and Sparse Voxel Octree's. 
Using DirectX11, C++, and ImGui.
![HonoursViews](https://github.com/TaniCorn/Honours/assets/63819551/8aa5b016-54a7-4f17-8bcf-463428141d04)
[TanapatSomridDissWithComments.pdf](https://github.com/user-attachments/files/15984956/TanapatSomridDissWithComments.pdf)

## Abstract
Voxel rendering has been used in games for a voxel art style look, commonly
accomplished by polygonal rendering and the utilisation of techniques like Marching
Cubes and Greedy Meshing. However, there eventually exists a limit to the efficiency
of these techniques when trying to scale the voxel sizes down, and alternate
rendering methods have been used more recently to explore different solutions for
the same problem. Now, techniques for non-polygonal rendering have been used in
many recent raytraced voxel games and this research explores one of the
techniques in this field.


This project aimed to evaluate how well an octree can handle construction and
rendering for a real-time rendering application and identify the key weaknesses of
the structure and discuss possible improvements.


The project artefact is a computer application that constructs a sparse voxel octree
on the GPU and CPU and runs a traversal algorithm on a compute shader.
Quantitative data is collected for memory consumption, rendering latency, and visual
fidelity to gauge the viability of octrees and evaluate key areas that need
improvement from a basic implementation.


The data shows that construction times and traversal fidelity are the biggest issues
with a basic implementation. Where 100% complexity scenes increase rendering
times by O(N) amounts and construction time on the GPU increases to well above
real-time rates.


Although large improvements can be made from following later implementations like
ESVO, it can be considerably harder to understand. The improvements suggested
focus specifically on the issues that arose and are not too far ahead in terms of
complexity. Those being parallelisation of the construction on the GPU, and
removing ray/box collision tests from the traversal and replacing it with a
implementation that follows a bit masking approach to determine ray position in
octrees, to achieve better construction times and facilitate cheaper traversal to
improve fidelity.

# Reflection
As I've finished this project a while ago, I've had a good amount of time to reflect on my work completed. Overall I'm really happy that I managed to make a working prototype of the technologies.




# Origin
Below is a description of the project I wrote early on to ascertain why I wanted to work on the project and my knowledge of it in the first 3 months. I would highly recommend having a read of my disseration for a more formal reading experience.


## Reason for work
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

# Research and Inspirations
Below I've listed some key inspirations and research that I've done to collect my information.

There are so many voxel developers that I have been following, and watching them all has helped me understand the landscape of voxel development. One note is that they do not go into implementation detail much and you'd be better off trying to find appropriate literature to implement the more complex structures. Furthermore, some will not talk about the exact structures they are using at all. This is unfortunate, but we can only read between the lines for some.
- [Gabe Rundlett](https://www.youtube.com/@GabeRundlett)
- [Xima](https://www.youtube.com/@xima1)
- [LetMeDevPlease](https://www.youtube.com/@letmedevplease)
- [Ein Baum](https://www.youtube.com/@Ein_Baum)
- [frozein](https://www.youtube.com/@frozein)
- [Polaron](https://www.youtube.com/@PolaronWorldEngine/videos)
- [DouglasDwyer](https://www.youtube.com/@DouglasDwyer)
- [Oberdiah](https://www.youtube.com/@oberdiah9064)
- [Bohdi Donselaar](https://www.youtube.com/@BodhiDon)
- [voxelbee](https://www.youtube.com/@voxelbee)
- [John Lin](https://www.youtube.com/@johnlin9665)
- [Isotopia](https://www.youtube.com/@IsotopiaGame)
- [Tooley1998](https://www.youtube.com/@Tooley1998)
- [Dennis Gustafsson](https://youtu.be/0VzE8ROwC58?si=N3Az_BsSo5rUTxXd)

Here's a full playlist of videos that I have saved. This doesn't contain all the videos I've watched from start to finish, but hopefully it'll be helpful: [Playlist](https://youtube.com/playlist?list=PLUVhEuL9O3wteCtiT1nh27n9XDElTOBns&si=1PKJ-cayGCdLgq_H)


