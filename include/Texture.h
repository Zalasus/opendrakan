/*
 * Texture.h
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <osg/Image>

#include "RiotAsset.h"

namespace od
{

    class Texture : public RiotAsset
    {
    public:

        Texture();

        virtual void loadFromRecord(SrscFile &srscFile, RecordId id);


    private:

        uint32_t mWidth;
        uint32_t mHeight;
        uint16_t mBitsPerPixel;
        uint16_t mAlphaBitsPerPixel;

        osg::ref_ptr<osg::Image> mImage;

    };

}




#endif /* TEXTURE_H_ */
