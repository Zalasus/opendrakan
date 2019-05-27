/*
 * Handle.cpp
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#include <odOsg/render/Handle.h>

#include <osg/Callback>

#include <odCore/Downcast.h>

#include <odCore/render/FrameListener.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Constants.h>
#include <odOsg/render/Model.h>
#include <odOsg/render/Rig.h>

namespace odOsg
{

    static void _assert_mutex_locked(std::mutex &mutex)
    {
#ifndef NDEBUG
        if(mutex.try_lock())
        {
            mutex.unlock();
            throw od::Exception("Accessed Handle without locking it's mutex");
        }
#endif
    }


    class HandleFrameListenerCallback : public osg::Callback
    {
    public:

        HandleFrameListenerCallback(Handle *handle, odRender::FrameListener *fl)
        : mHandle(handle)
        , mFrameListener(fl)
        , mLastSimTime(0.0)
        , mFirstUpdate(true)
        {
        }

        virtual bool run(osg::Object* object, osg::Object* data) override
        {
            osg::NodeVisitor *nv = data->asNodeVisitor();
            if(nv == nullptr)
            {
                return traverse(object, data);
            }

            const osg::FrameStamp *fs = nv->getFrameStamp();
            if(fs == nullptr)
            {
                return traverse(object, data);
            }

            double simTime = fs->getSimulationTime();

            if(mFirstUpdate)
            {
                mLastSimTime = simTime;
                mFirstUpdate = false;
            }

            if(mFrameListener != nullptr)
            {
                std::lock_guard<std::mutex> lock(mHandle->getMutex());

                mFrameListener->onFrameUpdate(simTime, simTime-mLastSimTime, fs->getFrameNumber());
            }

            mLastSimTime = simTime;

            return traverse(object, data);
        }


    private:

        Handle *mHandle;
        odRender::FrameListener *mFrameListener;
        double mLastSimTime;
        bool mFirstUpdate;
    };


    Handle::Handle(Renderer *renderer, osg::Group *parentGroup)
    : mParentGroup(parentGroup)
    , mModel(nullptr)
    , mFrameListener(nullptr)
    , mTransform(new osg::PositionAttitudeTransform)
    , mLightStateAttribute(new LightStateAttribute(renderer, Constants::MAX_LIGHTS))
    {
        mTransform->getOrCreateStateSet()->setAttribute(mLightStateAttribute, osg::StateAttribute::ON);

        if(mParentGroup != nullptr)
        {
            mParentGroup->addChild(mTransform);
        }
    }

    Handle::~Handle()
    {
        if(mUpdateCallback != nullptr)
        {
            mTransform->removeUpdateCallback(mUpdateCallback);
            mUpdateCallback = nullptr;
        }

        if(mParentGroup != nullptr)
        {
            mParentGroup->removeChild(mTransform);
        }
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
        _assert_mutex_locked(mMutex);

        mTransform->setPosition(GlmAdapter::toOsg(pos));
    }

    void Handle::setOrientation(const glm::quat &orientation)
    {
        _assert_mutex_locked(mMutex);

        mTransform->setAttitude(GlmAdapter::toOsg(orientation));
    }

    void Handle::setScale(const glm::vec3 &scale)
    {
        _assert_mutex_locked(mMutex);

        mTransform->setScale(GlmAdapter::toOsg(scale));
    }

    odRender::Model *Handle::getModel()
    {
        return mModel;
    }

    void Handle::setModel(odRender::Model *model)
    {
        _assert_mutex_locked(mMutex);

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
        _assert_mutex_locked(mMutex);

        int mask = visible ? -1 : 0;
        mTransform->setNodeMask(mask);
    }

    void Handle::setModelPartVisible(size_t partIndex, bool visible)
    {
        _assert_mutex_locked(mMutex);

        throw od::UnsupportedException("setModelPartVisible is unsupported");
    }

    void Handle::setRenderBin(odRender::RenderBin rb)
    {
        _assert_mutex_locked(mMutex);

        osg::StateSet *ss = mTransform->getOrCreateStateSet();

        switch(rb)
        {
        case odRender::RenderBin::NORMAL:
            if(mDepth != nullptr)
            {
                ss->removeAttribute(mDepth);
                mDepth = nullptr;
            }
            ss->setRenderBinDetails(0, "RenderBin");
            break;

        case odRender::RenderBin::SKY:
            if(mDepth == nullptr)
            {
                mDepth = new osg::Depth;
                mDepth->setWriteMask(false);
            }
            ss->setAttribute(mDepth, osg::StateAttribute::ON);
            ss->setRenderBinDetails(-1, "RenderBin");
            break;

        case odRender::RenderBin::TRANSPARENT:
            if(mDepth != nullptr)
            {
                ss->removeAttribute(mDepth);
                mDepth = nullptr;
            }
            ss->setRenderBinDetails(1, "DepthSortedBin");
            break;

        default:
            break;
        }
    }

    void Handle::addFrameListener(odRender::FrameListener *listener)
    {
        _assert_mutex_locked(mMutex);

        if(mFrameListener != nullptr)
        {
            throw od::UnsupportedException("Multiple frame listeners unsupported as of now");
        }

        mFrameListener = listener;

        if(mFrameListener != nullptr && mUpdateCallback == nullptr)
        {
            mUpdateCallback = new HandleFrameListenerCallback(this, mFrameListener);
            mTransform->addUpdateCallback(mUpdateCallback);

        }else if(mFrameListener == nullptr && mUpdateCallback != nullptr)
        {
            mTransform->removeUpdateCallback(mUpdateCallback);
            mUpdateCallback = nullptr;
        }
    }

    void Handle::removeFrameListener(odRender::FrameListener *listener)
    {
        _assert_mutex_locked(mMutex);

        if(mFrameListener != listener)
        {
            return;
        }

        mFrameListener = nullptr;;
    }

    void Handle::setEnableColorModifier(bool enable)
    {
        _assert_mutex_locked(mMutex);

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
        _assert_mutex_locked(mMutex);

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
        // FIXME: should we enforce the mutex to be locked here?

        if(mRig == nullptr)
        {
            mRig = std::make_unique<Rig>(mTransform);
        }

        return mRig.get();
    }

    void Handle::addLight(od::Light *light)
    {
        _assert_mutex_locked(mMutex);

        mLightStateAttribute->addLight(light);
    }

    void Handle::removeLight(od::Light *light)
    {
        _assert_mutex_locked(mMutex);

        mLightStateAttribute->removeLight(light);
    }

    void Handle::clearLightList()
    {
        _assert_mutex_locked(mMutex);

        mLightStateAttribute->clearLightList();
    }

    void Handle::setGlobalLight(const glm::vec3 &direction, const glm::vec3 &diffuse, const glm::vec3 &ambient)
    {
        _assert_mutex_locked(mMutex);

        osg::Vec3f dif = GlmAdapter::toOsg(diffuse);
        osg::Vec3f amb = GlmAdapter::toOsg(ambient);
        osg::Vec3f dir = GlmAdapter::toOsg(direction);

        mLightStateAttribute->setLayerLight(dif, amb, dir);
    }

}

