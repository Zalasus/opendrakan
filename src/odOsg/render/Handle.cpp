/*
 * Handle.cpp
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#include <odOsg/render/Handle.h>

#include <odCore/Downcast.h>

#include <odOsg/render/Model.h>
#include <odOsg/GlmAdapter.h>
#include <odOsg/Constants.h>

namespace odOsg
{

    Handle::Handle(Renderer *renderer, osg::Group *parentGroup)
    : mParentGroup(parentGroup)
    , mModel(nullptr)
    , mTransform(new osg::PositionAttitudeTransform)
    , mLightStateAttribute(new LightStateAttribute(renderer, Constants::MAX_LIGHTS))
    {
        mTransform->getOrCreateStateSet()->setAttribute(mLightStateAttribute, osg::StateAttribute::ON);

        mParentGroup->addChild(mTransform);
    }

    Handle::~Handle()
    {
        mParentGroup->removeChild(mTransform);
    }

    std::mutex &Handle::getMutex()
    {
        return mMutex;
    }

    glm::vec3 Handle::getPosition()
    {
        return GlmAdapter::toGlm(mTransform->getPosition());
    }

    glm::quat Handle::getOrientation()
    {
        return GlmAdapter::toGlm(mTransform->getAttitude());
    }

    glm::vec3 Handle::getScale()
    {
        return GlmAdapter::toGlm(mTransform->getScale());
    }

    void Handle::setPosition(const glm::vec3 &pos)
    {
        mTransform->setPosition(GlmAdapter::toOsg(pos));
    }

    void Handle::setOrientation(const glm::quat &orientation)
    {
        mTransform->setAttitude(GlmAdapter::toOsg(orientation));
    }

    void Handle::setScale(const glm::vec3 &scale)
    {
        mTransform->setScale(GlmAdapter::toOsg(scale));
    }

    odRender::Model *Handle::getModel()
    {
        return mModel;
    }

    void Handle::setModel(odRender::Model *model)
    {
        auto *osgModel = od::confident_downcast<Model>(model);

        if(mModel != nullptr)
        {
            mTransform->removeChild(mModel->getGeode());
        }

        mModel = osgModel;

        if(mModel != nullptr)
        {
            mTransform->addChild(mModel->getGeode());
        }
    }

    void Handle::setVisible(bool visible)
    {
        int mask = visible ? -1 : 0;
        mTransform->setNodeMask(mask);
    }

    void Handle::setModelPartVisible(size_t partIndex, bool visible)
    {
    }

    void Handle::setRenderMode(odRender::RenderMode rm)
    {
        osg::StateSet *ss = mTransform->getOrCreateStateSet();

        switch(rm)
        {
        case odRender::RenderMode::Normal:
            if(mDepth != nullptr)
            {
                ss->removeAttribute(mDepth);
                mDepth = nullptr;
            }
            ss->setRenderBinDetails(0, "RenderBin");
            break;

        case odRender::RenderMode::Sky:
            {
                if(mDepth == nullptr)
                {
                    mDepth = new osg::Depth;
                    mDepth->setWriteMask(false);
                }
                ss->setAttribute(mDepth, osg::StateAttribute::ON);
                ss->setRenderBinDetails(-1, "RenderBin");
            }
        }
    }

    void Handle::addFrameListener(odRender::FrameListener *listener)
    {
    }

    void Handle::removeFrameListener(odRender::FrameListener *listener)
    {
    }

    void Handle::setEnableColorModifier(bool enable)
    {
        if(enable && mColorModifierUniform == nullptr)
        {
            mColorModifierUniform = new osg::Uniform("colorModifier", osg::Vec4(1.0, 1.0, 1.0, 1.0));

            osg::StateSet *ss = mTransform->getOrCreateStateSet();
            ss->setDefine("COLOR_MODIFIER");
            ss->addUniform(mColorModifierUniform);

        }else if(!enable && mColorModifierUniform != nullptr)
        {
            osg::StateSet *ss = mTransform->getOrCreateStateSet();
            ss->removeDefine("COLOR_MODIFIER");
            ss->removeUniform(mColorModifierUniform);

            mColorModifierUniform = nullptr;
        }
    }

    void Handle::setColorModifier(const glm::vec4 &cm)
    {
        if(mColorModifierUniform != nullptr)
        {
            mColorModifierUniform->set(GlmAdapter::toOsg(cm));

        }else
        {
            throw od::Exception("Tried to set color modifier on Handle with disabled color modifier");
        }
    }

    odRender::Rig *Handle::getRig()
    {
        return nullptr;
    }

    void Handle::addLight(od::Light *light)
    {
        mLightStateAttribute->addLight(light);
    }

    void Handle::removeLight(od::Light *light)
    {
        mLightStateAttribute->removeLight(light);
    }

    void Handle::clearLightList()
    {
        mLightStateAttribute->clearLightList();
    }

    void Handle::setGlobalLight(const glm::vec3 &direction, const glm::vec3 &diffuse, const glm::vec3 &ambient)
    {
        osg::Vec3f dif = GlmAdapter::toOsg(diffuse);
        osg::Vec3f amb = GlmAdapter::toOsg(ambient);
        osg::Vec3f dir = GlmAdapter::toOsg(direction);

        mLightStateAttribute->setLayerLight(dif, amb, dir);
    }

}

