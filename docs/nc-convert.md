[TOC]

# Overview
`nc-convert` is a command line utility used for cooking assets into the .nca format, which is required for importing into NcEngine. Here's an example of converting a single file to an .nca:
```
> nc-convert -t mesh -s path/to/input.fbx -n myMesh -o outDir
```

The asset type, source file, and output asset name must be supplied with the -t,
-s, and -n flags, respectively. Optionally, and output directory may be given
with -o.

You can also use a JSON manifest file to convert many files at once:

```json
{
    "globalOptions": {
        "outputDirectory": "./",
        "workingDirectory": "./"
    },
    "mesh": [
        {
            "sourcePath": "path/to/mesh1.fbx",
            "assetNames": [
                {
                    "subResourceName" : "mesh1head",
                    "assetName" : "head"
                },
                {
                    "subResourceName" : "mesh1shoulders",
                    "assetName" : "shoulders"
                }
            ]
        },
        {
            "sourcePath": "path/to/mesh2.fbx",
            "assetName": "mesh2"
        }
    ],
    "texture": [
        {
            "sourcePath": "path/to/texture.png",
            "assetName": "myTexture"
        }
    ]
}
```

```
> nc-convert -m manifest.json
```

`nc-convert` will skip files that are already up-to-date when using a manifest.
Relative paths within `globalOptions` are interpreted relative to the manifest.

For more information, see the help text for `nc-convert`.

# Input File Requirements

## Audio Conversion
> Supported file types: .wav

Audio clips can be converted from any stereo or mono WAV file.

## Geometry Conversion
> Supported file types: .fbx, .obj

`concave-collider`, `hull-collider`, and `mesh` asset types are all generated from
geometry data, and therefore have similar requirements. For `concave-collider` and `hull-collider`, only the first
mesh in an input file will be processed, and non-geometry data in the input
file is ignored. For `mesh` asset types, if nc-convert is called in single-target mode, only the first mesh in an input
file will be processed. If the manifest is provided, either the first mesh in an input file is processed, or every mesh in an 
input file is processed if the sub-resource names are specified.

It is usually recommended for the input geometry to be centered around the origin.
This makes `Transform` operations within NcEngine less surprising. Additionally,
NcEngine will treat the origin as the object's center of mass for physics
calculations.

If the mesh is intended for animation, it needs to have the following properties:

- An armature with bones
- All bone weights for each vertex must be normalized to sum 1.0
- No more than four bone influences per vertices

These properties can be set in the modeling software. To set them in Blender, for example:

Normalizing bone weights:
1. Select the mesh
2. Change mode to Weight Paint
3. Select all vertices
4. Choose Weights -> Normalize All

Limiting bone influences to four:
1. Select the mesh
2. Change mode to Weight Paint
3. Select all vertices
4. Choose Weights -> Limit Total
5. Set the limit to 4 in the popup menu

Geometry used for `hull-collider` generation should be convex.

## Image Conversion
> Supported file types: .png, .jpg, .bmp

`cube-map` and `texture` asset types can both be converted from images containing
any number of RGBA channels. Any components not present will be filled with
'empty' values (e.g. opaque values for alpha channel).

Converting a `cube-map` requires a single input image containing all six faces.
The layout of the faces within the image must match one of the supported layouts
below. The aspect ratio of the input image is used to determine how faces should
be read.

Horizontal array layout (6:1):
```
[+X][-X][+Y][-Y][+Z][-Z]
```

Vertical array layout (1:6):
```
[+X]
[-X]
[+Y]
[-Y]
[+Z]
[-Z]
```

Horizontal cross layout (4:3):
```
    [+Y]
[-X][+Z][+X][-Z]
    [-Y]
```

Vertical cross layout (3:4):
```
    [+Y]
[-X][+Z][+X]
    [-Y]
    [-Z]
```

## Skeletal Animation Conversion
> Supported file types: .fbx

`skeletal-animation` assets can be converted from .fbx files with animation data.
When exporting the .fbx from the modeling software, the following settings must be observed. (Using Blender as an example):
- Rotate the mesh -90 degrees on the X-axis
- Set "Apply Scalings" to "FBX Units Scale"
- Check the "Apply Unit" box
- Check the "Use Space Transform" box
- Set "Forward" to "-Z Forward"
- Set "Up" to "Y Up"

![Alt text](animation_export_screenshoht.png)

There is also support for external animations such as Mixamo.

# Cooked File Formats
- [.nca File Format](#nca-file-format)
- [Blob Formats](#blob-formats)
    - [AudioClip](#audioclip-blob-format)
    - [ConcaveCollider](#concavecollider-blob-format)
    - [Cubemap](#cubemap-blob-format)
    - [HullCollider](#hullcollider-blob-format)
    - [Mesh](#mesh-blob-format)
    - [Shader](#shader-blob-format)
    - [SkeletalAnimation](#skeletalanimation-blob-format)
    - [Texture](#texture-blob-format)

## .nca File Format
Each nca consists of a header that is common between all asset types and
an asset blob, the layout of which is determined by the asset type.
| Name         | Type    | Size         | Note |
|--------------|---------|--------------|------
| magic number | string  | 4            | non-null-terminated string identifying the asset type
| compression  | string  | 4            | NONE until supported
| version      | u64     | 8            | major version of nc-convert used to create the asset
| blob size    | u64     | 8            | size of the asset blob
| asset blob   | -       | blob size    | unique layout for each asset type

## Blob Formats

### AudioClip Blob Format
> Magic Number: 'CLIP'

| Name                | Type     | Size                    |
|---------------------|----------|-------------------------|
| samples per channel | u64      | 8                       |
| left channel        | double[] | 8 * samples per channel |
| right channel       | double[] | 8 * samples per channel |

### ConcaveCollider Blob Format
> Magic Number: 'CONC'

| Name           | Type       | Size                |
|----------------|------------|---------------------|
| extents        | Vector3    | 12                  |
| max extent     | float      | 4                   |
| triangle count | u64        | 8                   |
| triangles      | Triangle[] | triangle count * 36 |

### Cubemap Blob Format
> Magic Number: 'CUBE'

| Name             | Type            | Size                         | Note 
|------------------|-----------------|------------------------------|------
| face side length | u32             | 4                            |
| pixel data       | unsigned char[] | face side length ^ 2 * 4 * 6 | 6 faces with 4 8-bit channels packed together

CubeMap faces in pixel data array are ordered: front, back, up, down, right, left.

### HullCollider Blob Format
> Magic number: 'HULL'

| Name         | Type      | Size              |
|--------------|-----------|-------------------|
| extents      | Vector3   | 12                |
| max extent   | float     | 4                 |
| vertex count | u64       | 8                 |
| vertex list  | Vector3[] | vertex count * 12 |

### Mesh Blob Format
> Magic Number: 'MESH'

| Name                 | Type                                 | Size              | Note
|----------------------|--------------------------------------|-------------------|-------------
| extents              | Vector3                              | 12                |
| max extent           | float                                | 4                 |
| vertex count         | u64                                  | 8                 |
| index count          | u64                                  | 8                 |
| vertex list          | MeshVertex[]                         | vertex count * 88 |
| indices              | int[]                                | index count * 4   |
| bones data has value | bool                                 | 1                 |
| BonesData            | BonesData                            |                   | [BonesData](#bones-data-blob-format)

### Bones Data Blob Format

| Name                         | Type                                                | Size                                                    | Note
|------------------------------|-----------------------------------------------------|---------------------------------------------------------|-------------
| vertexSpaceToBoneSpace count | u64                                                 | 8                                                       |
| boneSpaceToParentSpace count | u64                                                 | 8                                                       |
| boneMapping                  | (u64 + string + u32) * vertexSpaceToBoneSpace count | (12 + sizeof(boneName)) * vertexSpaceToBoneSpace count  |
| vertexSpaceToBoneSpace       | VertexSpaceToBoneSpace[]                            | (72 + sizeof(boneName)) * vertexSpaceToBoneSpace count  |
| boneSpaceToParentSpace       | BoneSpaceToParentSpace[]                            | (136 + sizeof(boneName)) * vertexSpaceToBoneSpace count |

### Shader Blob Format
> Magic Number: 'SHAD'

TODO

### SkeletalAnimation Blob Format
> Magic Number: 'SKEL'

| Name                       | Type             | Size                      | Note 
|----------------------------|------------------|---------------------------|------
| name size                  | u64              | 8                         |
| name                       | string           | name.size()               |
| durationInTicks            | u32              | 4                         |
| ticksPerSecond             | float            | 4                         |
| framesPerBone count        | u64              | 8                         |
| framesPerBone list         | FramesPerBone[]  |                           | [FramesPerBone](#FramesPerBone-blob-format)

### FramesPerBone Blob Format

| Name                 | Type             | Size                      | Note 
|----------------------|------------------|---------------------------|------
| name size            | u64              | 8                         |
| name                 | string           | name.size()               |
| position frames size | u64              | 8                         |
| position frames      | PositionFrames[] | position frames size * 20 |
| rotation frames size | u64              | 8                         |
| rotation frames      | RotationFrames[] | rotation frames size * 24 |
| scale frames size    | u64              | 8                         |
| scale frames         | ScaleFrames[]    | scale frames size * 20    |

### Texture Blob Format
> Magic Number: 'TEXT'

| Name      | Type            | Size               | Note 
|-----------|-----------------|--------------------|------
| width     | u32             | 4                  |
| height    | u32             | 4                  |
| pixelData | unsigned char[] | width * height * 4 | Always forced to 4 8-bit channels
