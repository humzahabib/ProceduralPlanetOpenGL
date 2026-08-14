
# ProceduralPlanetOpenGL
## Preview
<img width="1920" height="1080" alt="previewGIF" src="https://github.com/user-attachments/assets/ec7bdd95-2561-4248-bdb3-f166dbe5a4c1" />


## This project has the following phases.
- [x] Generate a Regular Icosahedron (Convex)
- [x] Subdivide it using Slerp (spherical interpolation) to approximate a triangular mesh of a sphere with fairly uniformly distributed vertices.
- [x] Apply multiple layers of Perlin noise to create landscapes and fine procedural details on the planet.
- [x] Apply shading to each vertex and calculate its colour procedurally to give planet colour.
- [x] Apply basic pixel shading.
- [] Implement atmospheric rendering.
- [x] Direct Light.
- [x] Single Scattered Light.
- [ ] Precompute single scattered light. 
- [ ] Multiple Scattered Light.
- [ ] Implement an LOD system (optional).

## Phase1: Generating a Regular Icosahedron (Convex)
The reason we are not using a normal (UV) sphere mesh is that the vertices on a UV
are not evenly distributed.
The vertices on the poles of the UV sphere are more closely packed together,
while vertices on the equator are very loosely packed. This results in geometry
pinching at the poles. This would result in uneven distribution of details
when we apply noise to generate terrain.
An obvious choice would be to then choose a primitive and then approximate a spherical mesh using that primitive.
There are several choices for the primitives I could use, but I chose to use an Icosahedron. There are two main reasons
for why I chose this primitive.

There are two reasons for why I chose icosphere. One is that Icosahedron starts from a triangular topology that makes it easy 
to manipulate the mesh from the start. The second is that the vertices on Icosahedron have almost uniform density over the whole surface.
So there won't be any uneven distribution of details later.

I generated the base 12 vertices and 20 faces using the standard golden-ratio construction (three
mutually orthogonal rectangles, vertices become the icosahedron corners.

## Phase 2: Subdivion to approximate a sphere

For each face, I:
1. Compute the 3 edge midpoints using slerp(v0, v1, 0.5), slerp(v1, v2, 0.5), slerp(v2, v0, 0.5)
2. Replace the original triangle with 4 new ones using the original 3 vertices + the 3 new midpoints
3. Cache midpoints by edge (sorted vertex-index pair as the key) so shared edges between adjacent
   triangles don't generate duplicate vertices, this also keeps the mesh indexed properly instead of
   blowing up vertex count with redundant duplicates.

I do this process recursively until I am left with an almost
spherical mesh with no visible corners.

## Phase 3: Layered Perlin Noise for Terrain

After making a sphere with uniform vertex density. I then apple Perlin noise on the planet
and displace vertices along with normals with noise applied to make terrain.

A single layer of noise looks too much not like a planet. So, after applying a layer of Perlin noise. 
I recalculate normals and then apply another layer of Perlin noise. I stack three
layers of height manipulation using Perlin noise to make my icosphere look like a planet.

Displacement height per vertex ends up being something like:
```
height = sum over octaves of: amplitude_i * perlin3D(position * frequency_i)
final_position = normalize(position) * (planet_radius + height * height_scale)
```

Next up is calculating per-vertex colour procedurally based on height.I originally planned to do it
while factoring in height, slope and location on the sphere but had to cut on it.

25th August, 2026. Mid progress checkpoint.




## Additional Stuff
### Star Mesh
To give the whole a nice space background, I create stars on a black background by randomly putting vertices on ```nStars``` locations on a large sphere and then giving them size and colour based on probability. These vertices are then rendered as points with the given size and colour and give the whole scene a nice starry background.

### Origin Based Camera
To deal with floating point inaccuracies and the artifects introduced by them, the renderer uses an
origin based camera where the eye always stays at the origin and the world revolves around it. That way, floating point inaccuracies never grow large near the camera and no artifacts appear in the camera.

## References
- Icosahedron: [Wikipedia - Icosahedron](https://en.wikipedia.org/wiki/Icosahedron)
- Geodesic subdivision / icosphere: [Wikipedia - Geodesic polyhedron](https://en.wikipedia.org/wiki/Geodesic_polyhedron)
- Slerp: [Wikipedia - Slerp](https://en.wikipedia.org/wiki/Slerp)
- Perlin noise (original): Perlin, K. (1985). *An Image Synthesizer*. SIGGRAPH '85.
- Perlin noise (improved, the version I'm using): Perlin, K. (2002). [*Improving Noise*](https://mrl.cs.nyu.edu/~perlin/paper445.pdf). ACM Transactions on Graphics 21(3), 681–682. Reference implementation: [mrl.cs.nyu.edu/~perlin/noise](https://mrl.cs.nyu.edu/~perlin/noise/)
- Fractal Brownian motion (octave stacking for terrain): [Wikipedia - Fractional Brownian motion](https://en.wikipedia.org/wiki/Fractional_Brownian_motion)
- Atmospheric scattering (for the upcoming atmosphere phases): Bruneton, E. and Neyret, F. (2008). [*Precomputed Atmospheric Scattering*](https://hal.inria.fr/inria-00288758/en). Computer Graphics Forum 27(4), 1079–1086. Updated 2017 implementation and writeup: [ebruneton.github.io/precomputed_atmospheric_scattering](https://ebruneton.github.io/precomputed_atmospheric_scattering/), code: [github.com/ebruneton/precomputed_atmospheric_scattering](https://github.com/ebruneton/precomputed_atmospheric_scattering)
