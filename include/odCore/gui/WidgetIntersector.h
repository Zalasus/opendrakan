/*
 * WidgetIntersector.h
 *
 *  Created on: Dec 10, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_GUI_WIDGETINTERSECTOR_H_
#define INCLUDE_ODCORE_GUI_WIDGETINTERSECTOR_H_

#include <memory>

namespace odGui
{

    class Widget;

    struct HitWidgetInfo
    {
        glm::vec2 hitPointInWidget;
        Widget *widget; // FIXME: bare pointer, but the way widgets store themselves here makes a shared_ptr difficult
    };

}


#endif /* INCLUDE_ODCORE_GUI_WIDGETINTERSECTOR_H_ */
