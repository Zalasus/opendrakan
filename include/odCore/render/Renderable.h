/*
 * Renderable.h
 *
 *  Created on: 31 Oct 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDERABLE_H_
#define INCLUDE_ODCORE_RENDER_RENDERABLE_H_

#include <osg/Node>

namespace odRender
{

    class RenderManager;


    class Renderable
    {
    public:

        virtual ~Renderable() = default;

        osg::Node *getOrBuildNode(RenderManager &renderManager)
        {
            if(mCachedNode == nullptr)
            {
                mCachedNode = buildNode(renderManager);
            }

            return mCachedNode;
        }


    protected:

        virtual osg::ref_ptr<osg::Node> buildNode(RenderManager &renderManager) = 0;

        osg::ref_ptr<osg::Node> mCachedNode;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_RENDERABLE_H_ */
