/*
 * PrefetchProbe.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/rfl/RflField.h>

namespace od
{


    PrefetchProbe::PrefetchProbe(od::AssetProvider &ap, bool ignoreMissing)
    : mAssetProvider(ap)
    , mIgnoreMissing(ignoreMissing)
    {
    }

    void PrefetchProbe::registerField(RflAssetRef &field, const char *fieldName)
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


