# GLCompressor
Takes an image and compresses it through OpenGL as BPTC. Dumps the compressed image and the compressed mipmaps to a file. Only supports power-of-two textures currently.\
This is useful for uploading compressed images straight into texture memory, no decompression required. This also means textures will take less gpu memory, so more textures can be used.
## Required
OpenGL 4.2+\
SFML 2.5.1
