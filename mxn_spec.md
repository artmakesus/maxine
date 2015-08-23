.MXN File Format
================

Overview
--------

Name | Optional | Size | Purpose
-----|----------|------|--------
Maxine magic keyword ("maxine") | No | 7 bytes | To identify the file as a .mxn file
Viewport resolution | No | 4 * 8 bytes | To know the resolution of the viewport in which the shapes will be displayed
Shape count | No | 8 bytes | To know the number of projection shapes in advance
Shapes | No | Shape size * shape count | To know the shape data such as vertices, texture coordinates, etc..


Shape 
-----

Name | Optional | Size | Purpose
-----|----------|------|--------
Has texture? | No | 1 byte | To know if the shape has a texture
Texture filepath | Yes | null-terminated | To know where to load the texture
X and Y position | No | 2 * 8 bytes | To know where to place the shape
Vertex count | No | 8 bytes | To know the number of vertices in advance
Vertices | No | vertex count * 2 * 8 bytes | To know how to form the shape
Tex. coords count | No | 8 bytes | To know the number of tex. coords in advance
Tex. coords | No | tex. coords count * 2 * 8 bytes | To know how to place the texture on the shape
