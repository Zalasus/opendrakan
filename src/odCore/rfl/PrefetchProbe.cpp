/*
 * PrefetchProbe.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/rfl/AssetRefField.h>

namespace odRfl
{


    PrefetchProbe::PrefetchProbe(odDb::AssetProvider &ap, bool ignoreMissing)
    : mAssetProvider(ap)
    , mIgnoreMissing(ignoreMissing)
    {
    }

    void PrefetchProbe::registerField(AssetRefField &field, const char *fieldName)
    {
        try
        {
            field.fetchAssets(mAssetProvider, false);

        }catch(od::NotFoundException &e)
        {
            Logger::warn() << "Field '" << fieldName << "' contains invalid asset reference";

            if(!mIgnoreMissing)
            {
                throw;
            }
        }
    }

}


