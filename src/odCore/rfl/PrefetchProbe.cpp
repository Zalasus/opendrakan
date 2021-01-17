/*
 * PrefetchProbe.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/Panic.h>

#include <odCore/rfl/AssetRefField.h>

namespace odRfl
{


    PrefetchProbe::PrefetchProbe(std::shared_ptr<odDb::DependencyTable> dt, bool ignoreMissing)
    : mDependencyTable(dt)
    , mIgnoreMissing(ignoreMissing)
    {
    }

    void PrefetchProbe::registerField(AssetRefField &field, const char *fieldName)
    {
        bool success = field.fetchAssets(*mDependencyTable);

        if(!success)
        {
            if(!mIgnoreMissing)
            {
                OD_PANIC() << "Field '" << fieldName << "' contains invalid asset reference";
            }
        }
    }

}
