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
#include <map>

#include <osg/Group>
#include <osg/Uniform>
#include <osgViewer/Viewer>

#include <odCore/RefCounted.h>

#include <odOsg/ShaderFactory.h>

namespace odOsg
{

    class ModelNode;
    class Texture;

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
        virtual odRender::ModelNode *getNodeForModel(odDb::Model *model) override;

        Texture *getTexture(odDb::Texture *texture);


    private:

        void _threadedRender();

        ShaderFactory mShaderFactory;
        std::thread mRenderThread;
        std::mutex mRenderMutex;

        std::map<odDb::Model*, od::RefPtr<ModelNode>> mModelNodeMap;
        std::map<odDb::Texture*, od::RefPtr<Texture>> mTextureMap;

        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osg::Group> mSceneRoot;
        osg::ref_ptr<osg::Group> mObjects;
        osg::ref_ptr<osg::Group> mLayers;

        osg::ref_ptr<osg::Uniform> mGlobalAmbient;
    };

}

#endif /* INCLUDE_ODOSG_RENDERER_H_ */
