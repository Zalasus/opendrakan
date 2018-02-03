/*
 * Texture.h
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <osg/Image>

#include "SrscFile.h"

namespace od
{

	class TextureFactory;

    class Texture : public osg::Image
    {
    public:

        Texture(RecordId id);

        void loadFromRecord(TextureFactory &factory, SrscFile &srscFile, SrscFile::DirIterator record);

        void exportToPng(const FilePath &path);

    private:

        RecordId mId;

        uint32_t mWidth;
        uint32_t mHeight;
        uint16_t mBitsPerPixel;
        uint16_t mAlphaBitsPerPixel;
    };

    typedef osg::ref_ptr<Texture> TexturePtr;
}




#endif /* TEXTURE_H_ */
