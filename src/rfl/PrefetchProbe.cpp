/*
 * PrefetchProbe.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#include "rfl/PrefetchProbe.h"

#include "rfl/RflField.h"

namespace odRfl
{


    PrefetchProbe::PrefetchProbe(od::AssetProvider &ap)
    : mAssetProvider(ap)
    {
    }

    void PrefetchProbe::registerField(RflAssetRef &field, const char *fieldName)
    {
        try
        {
            field.fetchAssets(mAssetProvider);

        }catch(od::NotFoundException &e)
        {
            Logger::warn() << "Field '" << fieldName << "' contains invalid asset reference";
        }
    }

}


