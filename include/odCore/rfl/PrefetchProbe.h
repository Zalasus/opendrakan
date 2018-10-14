/*
 * PrefetchProbe.h
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_PREFETCHPROBE_H_
#define INCLUDE_RFL_PREFETCHPROBE_H_

#include <odCore/rfl/FieldProbe.h>

namespace odDb
{
    class AssetProvider;
}

namespace odRfl
{

    class PrefetchProbe : public FieldProbe
    {
    public:

        PrefetchProbe(odDb::AssetProvider &ap, bool ignoreMissing = true);

        virtual void registerField(AssetRefField &field, const char *fieldName) override;


    private:

        odDb::AssetProvider &mAssetProvider;
        bool mIgnoreMissing;

    };

}


#endif /* INCLUDE_RFL_PREFETCHPROBE_H_ */
