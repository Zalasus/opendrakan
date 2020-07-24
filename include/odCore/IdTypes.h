
/**
 * @file IdTypes.h
 * @author zal
 * This file defines the various ID types for layers or objects. Defining them
 * in the Layer or LevelObject header would make forward declaring them harder
 * while still referring to the IDs. So they are instead defined in this
 * lightweight header.
 */

#ifndef IDTYPES_H_
#define IDTYPES_H_

namespace od
{
    using LevelObjectId = uint32_t;
    using LayerId = uint32_t;
}

#endif
