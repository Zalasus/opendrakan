/*
 * NodeMasks.h
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_NODEMASKS_H_
#define INCLUDE_NODEMASKS_H_

#include <osg/Node>

namespace odRender
{

    class NodeMasks
    {
    public:

        static const osg::Node::NodeMask Hidden = 0x0000;
        static const osg::Node::NodeMask Object = 0x0002;
        static const osg::Node::NodeMask Layer  = 0x0004;
        static const osg::Node::NodeMask Gui    = 0x0008;
        static const osg::Node::NodeMask All    = 0xffff;
    };

}

#endif /* INCLUDE_NODEMASKS_H_ */
