/*
 * CursorListener.cpp
 *
 *  Created on: Jan 27, 2019
 *      Author: zal
 */

#include <odCore/input/CursorListener.h>

namespace odInput
{

    void CursorListener::triggerCallback(const glm::vec2 &ndc)
    {
        mCallback(ndc);
    }

}


