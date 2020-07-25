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
        std::shared_ptr<Widget> widget;
    };

}


#endif /* INCLUDE_ODCORE_GUI_WIDGETINTERSECTOR_H_ */
