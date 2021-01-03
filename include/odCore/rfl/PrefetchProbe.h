/*
 * PrefetchProbe.h
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_PREFETCHPROBE_H_
#define INCLUDE_RFL_PREFETCHPROBE_H_

#include <memory>

#include <odCore/rfl/FieldProbe.h>

namespace odDb
{
    class DependencyTable;
}

namespace odRfl
{

    class PrefetchProbe : public FieldProbe
    {
    public:

        PrefetchProbe(std::shared_ptr<odDb::DependencyTable> dt, bool ignoreMissing = true);

        virtual void registerField(AssetRefField &field, const char *fieldName) override;


    private:

        std::shared_ptr<odDb::DependencyTable> mDependencyTable;
        bool mIgnoreMissing;

    };

}


#endif /* INCLUDE_RFL_PREFETCHPROBE_H_ */
