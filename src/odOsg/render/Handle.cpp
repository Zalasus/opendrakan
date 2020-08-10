/*
 * Handle.cpp
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#include <odOsg/render/Handle.h>

#include <osg/Callback>

#include <odCore/Downcast.h>
#include <odCore/Logger.h>

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
        /* FIXME: this synchronization concept is flawed. think of a better one
        if(mutex.try_lock())
        {
            mutex.unlock();
            throw od::Exception("Accessed Handle without locking it's mutex");
        }*/
#endif
    }


    class HandleUpdateCallback : public osg::Callback
    {
    public:

        HandleUpdateCallback(Handle &handle)
        : mHandle(handle)
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

            mHandle.update(simTime, simTime-mLastSimTime, fs->getFrameNumber());

            mLastSimTime = simTime;

            return traverse(object, data);
        }


    private:

        Handle &mHandle;
        double mLastSimTime;
        bool mFirstUpdate;
    };


    Handle::Handle(Renderer &renderer)
    : mParentGroup(nullptr)
    , mFrameListener(nullptr)
    , mTransform(new osg::PositionAttitudeTransform)
    , mLightStateAttribute(new LightStateAttribute(renderer, Constants::MAX_LIGHTS))
    {
        mTransform->getOrCreateStateSet()->setAttribute(mLightStateAttribute, osg::StateAttribute::ON);

        // TODO: maybe allow to designate static objects that don't need this?
        //mUpdateCallback = new HandleUpdateCallback(*this);
        //mTransform->addUpdateCallback(mUpdateCallback);
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
        return mModel.get();
    }

    void Handle::setModel(std::shared_ptr<odRender::Model> model)
    {
        _assert_mutex_locked(mMutex);

        auto osgModel = od::confident_downcast<Model>(model);

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
            ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
            break;

        case odRender::RenderBin::SKY:
            if(mDepth == nullptr)
            {
                mDepth = new osg::Depth;
                mDepth->setWriteMask(false);
            }
            ss->setAttribute(mDepth, osg::StateAttribute::ON);
            ss->setRenderBinDetails(-1, "RenderBin");
            ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
            break;

        case odRender::RenderBin::TRANSPARENT:
            if(mDepth != nullptr)
            {
                ss->removeAttribute(mDepth);
                mDepth = nullptr;
            }
            ss->setRenderBinDetails(1, "DepthSortedBin");
            ss->setMode(GL_BLEND, osg::StateAttribute::ON);
            break;

        default:
            break;
        }
    }

    void Handle::addFrameListener(odRender::FrameListener *listener)
    {
        throw od::UnsupportedException("Frame listeners unsupported right now");
    }

    void Handle::removeFrameListener(odRender::FrameListener *listener)
    {
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

    void Handle::addLight(std::shared_ptr<od::Light> light)
    {
        _assert_mutex_locked(mMutex);

        mLightStateAttribute->addLight(light);
    }

    void Handle::removeLight(std::shared_ptr<od::Light> light)
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

    void Handle::update(double simTime, double relTime, uint32_t frameNumber)
    {
        //mTransform->setPosition(GlmAdapter::toOsg(mNextUpdatePosition));
        //mTransform->setAttitude(GlmAdapter::toOsg(mNextUpdateRotation));
    }

}
