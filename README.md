# ProceduralPlanetOpenGL
## This project has the following phases.
- [x] Generate a Regular Icosahedron (Convex)
- [x] Subdivide it using Slerp (spherical interpolation) to approximate a triangular mesh of a sphere with fairly uniformly distributed vertices.
- [x] Apply multiple layers of Perlin noise to create landscapes and fine procedural details on the planet.
- [x] Apply shading to each vertex and calculate its colour procedurally to give planet colour.
- [x] Apply basic pixel shading.
- [ ] Implement atmospheric rendering.
- [ ] Direct Light
- [ ] Single Scattered Light
- [ ] Multiple Scattered Light
- [ ] Implement an LOD system (optional)
## Phase1: Generating a Regular Icosahedron (Convex)
The reason we are not using a normal (UV) sphere mesh is that the vertices on a UV
are not evenly distributed as you can see in the following picutre.
![UV Sphere](https://www.google.com/url?sa=t&source=web&rct=j&url=https%3A%2F%2Ffreesvg.org%2Fwireframe-sphere&ved=0CBYQjRxqFwoTCNj-p6WS3JUDFQAAAAAdAAAAABBq&opi=89978449)
The vertices on the poles of the UV sphere are more closely packed together,
while vertices on the equator are very loosely packed. This results in geometry
pinching at the poles. This would result in uneven distribution of details
when we apply noise to generate terrain.
An obvious choice would be to then choose a primitive and then approximate a spherical mesh using that primitive.
There are several choices for the primitives I could use, but I chose to use an Icosahedron. There are two main reasons
for why I chose this primitive.

The first reason is that an icosahedron is made up of 20 identical equilateral triangles, and every vertex
has the same number of neighbours (5). That means when I project it onto a sphere, I start from a mesh
that's already about as close to uniform as a convex polyhedron can get. No pole, no seam, no
region of the sphere that's inherently denser than another. Any distortion I introduce later comes from
the subdivision step, not from the base shape.

The second reason is subdivision-friendliness. Each triangular face can be split into 4 smaller triangles
by bisecting its edges, and repeating this recursively gives me control over resolution: n subdivisions and I know
roughly how many vertices/triangles I'll end up with. Other primitives (cube, octahedron) can do the same
thing but converge to less uniform distributions faster, or need more subdivisions to look reasonably round.
The icosahedron gives the best uniformity-to-subdivision-count tradeoff of the platonic solids, which is why
it's the standard choice for geodesic domes and procedural planets alike.

I generated the base 12 vertices and 20 faces using the standard golden-ratio construction (three
mutually orthogonal rectangles, each with sides 1 and φ, vertices become the icosahedron corners),
then normalized every vertex so the whole thing sits on a unit sphere.

## Phase 2: Subdividing with Slerp
Straight edge-bisection gives you new vertices that lie inside the sphere, not on its surface, since
the midpoint of a chord is closer to the center than the surface is. The naive fix is to normalize the
midpoint after computing it (push it back out to the sphere along the same direction), but that's just
linear interpolation followed by a projection, and it subtly distorts triangle size/angle as you subdivide,
vertices near the original icosahedron corners stay coarser than vertices that get subdivided from
edge midpoints.

Slerp (spherical linear interpolation) fixes this properly, instead of interpolating along a straight line
and re-normalizing, I interpolate along the great-circle arc between two vertices directly. This keeps
every new vertex exactly on the sphere and preserves the angular relationship between the two
endpoints, so the resulting mesh stays far more uniform across multiple subdivision levels.

For each face, I:
1. Compute the 3 edge midpoints using slerp(v0, v1, 0.5), slerp(v1, v2, 0.5), slerp(v2, v0, 0.5)
2. Replace the original triangle with 4 new ones using the original 3 vertices + the 3 new midpoints
3. Cache midpoints by edge (sorted vertex-index pair as the key) so shared edges between adjacent
   triangles don't generate duplicate vertices, this also keeps the mesh indexed properly instead of
   blowing up vertex count with redundant duplicates.

Repeating this recursively gets me a mesh that approximates a sphere with near-uniform triangle
density and no visible pinching anywhere, poles included, because there are no poles.

## Phase 3: Layered Perlin Noise for Terrain
With a uniform base mesh in hand, I generate terrain by displacing each vertex along its own normal
(which, since this is a unit sphere, is just the vertex position itself) by a height value sampled from
3D Perlin noise, using the vertex's position directly as the sample coordinate. Sampling in 3D
instead of projecting to 2D UV space means no seams and no distortion near the poles, exactly the
same reasoning as why I picked the icosahedron in the first place.

A single octave of noise looks too smooth and repetitive to pass as terrain, so I layer multiple octaves
together (fractal Brownian motion): each successive layer uses a higher frequency and a lower amplitude
than the last, controlled by a lacunarity and persistence parameter. The low-frequency/high-amplitude
layers rough out continents and major mountain ranges, the high-frequency/low-amplitude layers add
the fine surface detail on top. Stacking maybe 4-6 octaves gets a landscape that reads as natural instead
of looking like a bumpy golf ball or plain static.

Displacement height per vertex ends up being something like:
```
height = sum over octaves of: amplitude_i * perlin3D(position * frequency_i)
final_position = normalize(position) * (planet_radius + height * height_scale)
```

Next up is calculating per-vertex colour procedurally based on height/slope/latitude so the planet
actually looks like a planet instead of a grey rock, then moving into pixel shading and the
atmospheric scattering stack.


## Additional Stuff
### Star Mesh
To give the whole a nice space background, I create stars on a black background by randomly putting vertices on ```nStars``` locations on a large sphere and then giving them size and colour based on probability. These vertices are then rendered as points with the given size and colour and give the whole scene a nice starry background.

## References
- Icosahedron: [Wikipedia - Icosahedron](https://en.wikipedia.org/wiki/Icosahedron)
- Geodesic subdivision / icosphere: [Wikipedia - Geodesic polyhedron](https://en.wikipedia.org/wiki/Geodesic_polyhedron)
- Slerp: [Wikipedia - Slerp](https://en.wikipedia.org/wiki/Slerp)
- Perlin noise (original): Perlin, K. (1985). *An Image Synthesizer*. SIGGRAPH '85.
- Perlin noise (improved, the version I'm using): Perlin, K. (2002). [*Improving Noise*](https://mrl.cs.nyu.edu/~perlin/paper445.pdf). ACM Transactions on Graphics 21(3), 681–682. Reference implementation: [mrl.cs.nyu.edu/~perlin/noise](https://mrl.cs.nyu.edu/~perlin/noise/)
- Fractal Brownian motion (octave stacking for terrain): [Wikipedia - Fractional Brownian motion](https://en.wikipedia.org/wiki/Fractional_Brownian_motion)
- Atmospheric scattering (for the upcoming atmosphere phases): Bruneton, E. and Neyret, F. (2008). [*Precomputed Atmospheric Scattering*](https://hal.inria.fr/inria-00288758/en). Computer Graphics Forum 27(4), 1079–1086. Updated 2017 implementation and writeup: [ebruneton.github.io/precomputed_atmospheric_scattering](https://ebruneton.github.io/precomputed_atmospheric_scattering/), code: [github.com/ebruneton/precomputed_atmospheric_scattering](https://github.com/ebruneton/precomputed_atmospheric_scattering)
