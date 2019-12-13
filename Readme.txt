Keegan Millard
CS410
12/13/2019

Yup, the scene, textures and block types are hardcoded.
The block data was parsed into a text format with a Java program.
Only the top layer of blocks are loaded.
Adding a second subcontainer for the blocks would speed things up a lot.

usage: $raytracer sceneNum heightRes threadCount(default 8)
