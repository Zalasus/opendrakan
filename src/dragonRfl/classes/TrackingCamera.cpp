/*
 * Camera.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/classes/TrackingCamera.h>

#include <dragonRfl/RflDragon.h>
#include <dragonRfl/LocalPlayer.h>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Client.h>

#include <odCore/input/InputManager.h>
#include <odCore/input/InputListener.h>

#include <odCore/rfl/Rfl.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Model.h>
#include <odCore/render/Geometry.h>
#include <odCore/render/Array.h>
#include <odCore/render/Camera.h>

#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{

	TrackingCameraFields::TrackingCameraFields()
	: trackingMode(CameraTrackingMode::RUBBER_BAND)
	, rubberBandStrength(2)
	, spinSpeed(20)
	, crosshairDistance(8)
	{
	}

	void TrackingCameraFields::probeFields(odRfl::FieldProbe &probe)
	{
	    probe("Camera Options")
                (trackingMode, "Tracking Mode")
                (rubberBandStrength, "Rubber Band Strength")
                (spinSpeed, "Spin Speed")
                (crosshairDistance, "Cross-hair Distance (lu)");
	}


    TrackingCamera_Cl::TrackingCamera_Cl(od::LevelObject &objectToTrack)
    : mObjectToTrack(objectToTrack)
    {
    }

	void TrackingCamera_Cl::onLoaded()
	{
	    getLevelObject().setSpawnStrategy(od::SpawnStrategy::Always);
	}

    void TrackingCamera_Cl::onSpawned()
    {
        //_setupCameraModel();
    }

    void TrackingCamera_Cl::onDespawned()
    {
        mRenderHandle = nullptr;
    }

    void TrackingCamera_Cl::onStart()
    {
        auto &obj = getLevelObject();

        //obj.getLevel().activateLayerPVS(obj.getAssociatedLayer());

	    // set initial view matrix
	    _updateCameraViewMatrix();

	    obj.setEnableUpdate(true);

        mInputListener = getClient().getInputManager().createInputListener();
        mInputListener->setMouseMoveCallback([this](auto pos){ _mouseHandler(pos); });
    }

    void TrackingCamera_Cl::onStop()
    {
        mInputListener = nullptr;
    }

	void TrackingCamera_Cl::onUpdate(float relTime)
	{
	    _updateCameraTracking();
	}

	void TrackingCamera_Cl::onLayerChanged(od::Layer *from, od::Layer *to)
	{
	    //getLevelObject().getLevel().activateLayerPVS(to);
	}

    void TrackingCamera_Cl::onTransformChanged()
    {
        _updateCameraViewMatrix();

        if(mRenderHandle != nullptr)
        {
            mRenderHandle->setPosition(getLevelObject().getPosition());
            mRenderHandle->setOrientation(getLevelObject().getRotation());
        }
    }

    glm::vec2 TrackingCamera_Cl::cursorPosToYawPitch(const glm::vec2 &cursorPos)
    {
        // the tranlation to pitch/yaw is easier in NDC. convert from GUI space
        glm::vec2 posNdc(2*cursorPos.x - 1, 1 - 2*cursorPos.y);
        float pitch =  glm::half_pi<float>()*posNdc.y;
        float yaw   = -glm::pi<float>()*posNdc.x;
        return {yaw, pitch};
    }

    void TrackingCamera_Cl::_mouseHandler(const glm::vec2 &pos)
    {
        glm::vec2 yawPitch = TrackingCamera_Cl::cursorPosToYawPitch(pos);
        mYaw = yawPitch.x;
        mPitch = yawPitch.y;

        _updateCameraTracking();
    }

    void TrackingCamera_Cl::_updateCameraTracking()
    {    
        glm::vec3::value_type maxDistance = 3;
        glm::vec3::value_type bounceBackDistance = 0.1; // TODO: calculate this based of FOV or something

        glm::vec3 eye = mObjectToTrack.getPosition();
        glm::quat lookDirectionQuat(glm::vec3(mPitch, mYaw, 0));
        glm::vec3 lookDirection = lookDirectionQuat * glm::vec3(0, 0, 1);

        // perform raycast to find obstacle closest point with unobstructed view of player
        glm::vec3 from = mObjectToTrack.getPosition();
        glm::vec3 to = from + lookDirection * maxDistance;

        static const odPhysics::PhysicsTypeMasks::Mask mask = odPhysics::PhysicsTypeMasks::Layer | odPhysics::PhysicsTypeMasks::LevelObject;

        odPhysics::RayTestResult result;
        bool hit = getClient().getPhysicsSystem().rayTestClosest(from, to, mask, mObjectToTrack.getPhysicsHandle(), result);
        if(!hit)
        {
            eye = to;

        }else
        {
            // we hit something. bounce a bit off of surface (like a reflected beam of light) so we don't clip into the object

            glm::vec3 reflectedDirection = lookDirection - 2*glm::dot(lookDirection, result.hitNormal)*result.hitNormal;
            reflectedDirection = glm::normalize(reflectedDirection);

            eye = result.hitPoint + reflectedDirection * bounceBackDistance;
        }

        getLevelObject().setPosition(eye);
        getLevelObject().setRotation(lookDirectionQuat);
    }

    void TrackingCamera_Cl::_updateCameraViewMatrix()
    {
        glm::vec3 eyepoint = getLevelObject().getPosition();
        glm::quat lookDirection = getLevelObject().getRotation();

        glm::vec3 front = lookDirection * glm::vec3(0, 0, -1); // rynn's model's look direction is negative z!
        glm::vec3 up = lookDirection * glm::vec3(0, 1, 0);

        getClient().getRenderer().getCamera()->lookAt(eyepoint, eyepoint + front, up);
    }

    void TrackingCamera_Cl::_setupCameraModel()
    {
        // creates a visual representation of the camera position and direction
        auto &renderer = getClient().getRenderer();

        float d = 0.05;
        float hw = 0.04;
        float hh = 0.03;
        glm::vec3 verts[] =
        {
            { 0, 0, 0 },
            {    hw,  hh,     -d },
            {   -hw,  hh,     -d },
            {   -hw, -hh,     -d },
            {    hw, -hh,     -d },
            { -hw/2,  hh,     -d },
            {  hw/2,  hh,     -d },
            {     0,  hh*1.3, -d }
        };

        int32_t indices[] =
        {
            0, 1,
            0, 2,
            0, 3,
            0, 4,

            1, 2,
            2, 3,
            3, 4,
            4, 1,

            5, 7,
            6, 7
        };

        auto geometry = renderer.createGeometry(odRender::PrimitiveType::LINES, true);
        {
            odRender::ArrayAccessor<glm::vec3> accessor(geometry->getVertexArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);
            accessor.resize(sizeof(verts)/sizeof(verts[0]));
            for(size_t i = 0; i < accessor.size(); ++i)
            {
                accessor[i] = verts[i];
            }
        }

        {
            odRender::ArrayAccessor<int32_t> accessor(geometry->getIndexArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);
            accessor.resize(sizeof(indices)/sizeof(indices[0]));
            for(size_t i = 0; i < accessor.size(); ++i)
            {
                accessor[i] = indices[i];
            }
        }

        auto model = renderer.createModel();
        model->addGeometry(geometry);

        mRenderHandle = renderer.createHandle(odRender::RenderSpace::LEVEL);
        mRenderHandle->setModel(model);
        mRenderHandle->setPosition(getLevelObject().getPosition());
        mRenderHandle->setOrientation(getLevelObject().getRotation());
    }

}
