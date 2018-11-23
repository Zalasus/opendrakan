/*
 * Renderer.h
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDERER_H_
#define INCLUDE_ODOSG_RENDERER_H_

#include <odCore/render/Renderer.h>

#include <thread>
#include <mutex>

#include <osg/Group>
#include <osgViewer/Viewer>

#include <odOsg/ShaderFactory.h>

namespace odOsg
{

    class ModelNode;

    class Renderer : public odRender::Renderer
    {
    public:

        Renderer();
        ~Renderer();

        inline ShaderFactory &getShaderFactory() { return mShaderFactory; }

        virtual void onStart() override;
        virtual void onEnd() override;

        virtual void setEnableLighting(bool b) override;
        virtual bool isLightingEnabled() const override;

        virtual odRender::ObjectNode *createObjectNode(od::LevelObject &obj) override;
        virtual odRender::ModelNode *createModelNode(odDb::Model &model) override;


    private:

        void _threadedRender();

        void _buildSingleLodModelNode(odDb::Model &model, ModelNode *node);
        void _buildMultiLodModelNode(odDb::Model &model, ModelNode *node);

        ShaderFactory mShaderFactory;
        std::thread mRenderThread;
        std::mutex mRenderMutex;

        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osg::Group> mSceneRoot;
        osg::ref_ptr<osg::Group> mObjects;
        osg::ref_ptr<osg::Group> mLayers;
    };

}

#endif /* INCLUDE_ODOSG_RENDERER_H_ */
