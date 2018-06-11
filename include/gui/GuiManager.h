/*
 * GuiManager.h
 *
 *  Created on: 8 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_GUIMANAGER_H_
#define INCLUDE_GUI_GUIMANAGER_H_

#include <string>

#include "FilePath.h"
#include "SrscFile.h"
#include "db/TextureFactory.h"
#include "db/AssetProvider.h"
#include "db/Database.h"

namespace od
{

    class Engine;

    /**
     * Class for managing the game's GUI, as well as an interface for accessing the
     * RRC file which contains GUI textures and localized GUI text.
     */
    class GuiManager : public AssetProvider
    {
    public:

        GuiManager(Engine &engine, const FilePath &rrcPath);

        /**
         * @brief Localizes string with localization tag.
         *
         * Will check whether passed string begins with a localization tag "<0x...>". If not, the string is returned as-is.
         * If it does, the string ID in the tag will be looked up and the localized version of that string returned.
         * If the string ID can not be found in the RRC, the tag will be stripped and the unlocalized string returned.
         */
        std::string localizeString(const std::string &s);

        void dumpStrings();
        void dumpTextures();

        // implement AssetProvider
        virtual Texture *getTextureByRef(const AssetRef &ref) override;
        virtual Texture *getTexture(RecordId recordId) override;


    private:

        // make all methods for non-textures inaccessible. can't do the opposite because the AssetProvider base must stay accessible
        using AssetProvider::getClassByRef;
        using AssetProvider::getModelByRef;
        using AssetProvider::getSequenceByRef;
        using AssetProvider::getAnimationByRef;
        using AssetProvider::getSoundByRef;
        using AssetProvider::getClass;
        using AssetProvider::getModel;
        using AssetProvider::getSequence;
        using AssetProvider::getAnimation;
        using AssetProvider::getSound;

        inline void _decryptString(char * const str, const size_t len);

        Engine &mEngine;
        SrscFile mRrcFile;
        TextureFactory mTextureFactory;
        Database &mInterfaceDb;

    };

}

#endif /* INCLUDE_GUI_GUIMANAGER_H_ */
