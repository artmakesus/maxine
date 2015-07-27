.MXN File Format
================

Overview
--------

Name | Optional | Size | Purpose
-----|----------|------|--------
Maxine magic keyword ("maxine") | No | 6 bytes | To identify the file as a .mxn file
Viewport resolution | No | 8 bytes | To know the resolution of the viewport in which the shapes will be displayed
Shape count | No | 4 bytes | To know the number of projection shapes in advance
Shapes | No | Shape size * shape count | To know the shape data such as vertices, texture coordinates, etc..


Shape 
-----

Name | Optional | Size | Purpose
-----|----------|------|--------
Has texture? | No | 1 byte | To know if the shape has a texture
Texture filepath | Yes | N bytes | To know where to load the texture
X and Y position | No | 8 bytes | To know where to place the shape
Vertex count | No | 4 bytes | To know the number of vertices in advance
Vertices (consists two floats, X and Y) | No | 8 bytes * vertex count | To know how to form the shape
Tex. coord count | No | 4 bytes | To know the number of tex. coords in advance
Tex. coords | No | 8 bytes * tex. coord count | To know how to place the texture on the shape
