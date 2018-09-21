/*
 * PrefetchProbe.h
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_PREFETCHPROBE_H_
#define INCLUDE_RFL_PREFETCHPROBE_H_

#include <odCore/rfl/FieldProbe.h>

namespace od
{

    class PrefetchProbe : public FieldProbe
    {
    public:

        PrefetchProbe(od::AssetProvider &ap);

        virtual void registerField(RflAssetRef &field, const char *fieldName) override;


    private:

        od::AssetProvider &mAssetProvider;

    };

}


#endif /* INCLUDE_RFL_PREFETCHPROBE_H_ */
