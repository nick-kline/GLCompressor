#include <iostream>
#include <fstream>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <GL/glew.h>

int main(int argc, char** argv)
{
    sf::Context context;
    context.setActive(true);

    GLenum status = glewInit();
    if(status != GLEW_OK)
    {
        std::cerr << "glew failed to initialize" << std::endl;
    }

    unsigned int texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    for(int i = 1; i < argc; i+=2)
    {
        if(i+1 < argc)
        {
            std::cout << argv[i] << " being formatted into " << argv[i+1] << std::endl;

            const std::string loadfile = argv[i];
            const std::string savefile = argv[i+1];

            sf::Image image;
            if(image.loadFromFile(loadfile))
            {
                image.flipVertically();
                sf::Vector2u imagesize = image.getSize();
                glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_BPTC_UNORM, imagesize.x, imagesize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

                unsigned int miplevels = 1;
                float pot = std::log2(float(imagesize.x));
                if(imagesize.x == imagesize.y && std::ceil(pot) == std::floor(pot))
                {
                    miplevels += pot;
                    glGenerateMipmap(GL_TEXTURE_2D);
                }

                std::ofstream ofile;
                ofile.open(savefile, std::ios::binary | std::ios::out | std::ios::trunc);
                ofile.write((char*)&miplevels, sizeof(unsigned int)); // how many mipmap levels are stored

                std::vector<int> levelsizes;
                unsigned int accumulative = 0;
                for(unsigned int i = 0; i < miplevels; ++i)
                {
                    int size = 0;
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size); // get size of current level of texture
                    levelsizes.push_back(size);
                    accumulative += size;
                }

                ofile.write((char*)&accumulative, sizeof(unsigned int)); // write the total size of the entire texture including all of its levels
                for(unsigned int i = 0; i < levelsizes.size(); ++i) // loop through each mipmap level
                {
                    auto bytes = std::make_unique<unsigned char[]>(levelsizes[i]);
                    glGetCompressedTexImage(GL_TEXTURE_2D, i, bytes.get()); // download current texture level and store it in file
                    ofile.write((char*)&levelsizes[i], sizeof(int)); // write size of this particular level
                    ofile.write((char*)&(imagesize.x), sizeof(unsigned int)); // write dimensions width and length of this level
                    ofile.write((char*)&(imagesize.y), sizeof(unsigned int));
                    ofile.write((char*)bytes.get(), levelsizes[i]); // the actual image of the current level that was downloaded
                    imagesize.x /= 2; // change the image dimensions to the next mip level
                    imagesize.y /= 2;
                }
                ofile.close();
            }
            else
            {
                std::cout << "Could not convert " << loadfile << std::endl;
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);

    return 0;
}
