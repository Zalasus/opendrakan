
#include <dragonRfl/classes/Fader.h>

#include <odCore/Client.h>
#include <odCore/LevelObject.h>

#include <odCore/db/Class.h>

#include <odCore/gui/Quad.h>

#include <odCore/render/Handle.h>
#include <odCore/render/Renderer.h>

#include <dragonRfl/RflDragon.h>
#include <dragonRfl/gui/DragonGui.h>

namespace dragonRfl
{

	FaderFields::FaderFields()
	: fadeInTime(1.0)
	, fadedTime(2.0)
	, fadeOutTime(true)
	, startMode(StartMode::RUN_WHEN_TRIGGERED)
	, color(255, 255, 255)
	{
	}

	void FaderFields::probeFields(odRfl::FieldProbe &probe)
	{
		probe("Fader")
				(fadeInTime, "Fade In Time (s)")
                (fadedTime, "Faded Time (-1.0s = trigger)")
                (fadeOutTime, "Fade Out Time (s)")
                (startMode, "Start Mode")
                (color, "Color");
	}


    Fader_Sv::Fader_Sv()
    : mTime(0.0f)
    , mPhase(FadePhase::NOT_TRIGGERED)
    {
    }

	void Fader_Sv::onSpawned()
	{
        auto &obj = getLevelObject();
        obj.setEnableUpdate(true);

        if(mFields.startMode == StartMode::RUN_INSTANTLY)
        {
            mPhase = FadePhase::FADE_IN;
            mTime = 0.0f;
        }
	}

	void Fader_Sv::onUpdate(float relTime)
	{
        float fadeState;
        switch(mPhase)
        {
        case FadePhase::NOT_TRIGGERED:
            //fadeState = 1.0f;
            return;

        case FadePhase::FADE_IN:
            mTime += relTime;
            fadeState = mTime / mFields.fadeInTime;
            if(mTime >= mFields.fadeInTime)
            {
                mTime = 0.0f;
                mPhase = FadePhase::FADED;
            }
            break;

        case FadePhase::FADED:
            //fadeState = 1.0f;
            if(mFields.fadedTime >= 0) // negative faded-time would mean "wait for trigger to start fade-out"
            {
                mTime += relTime;
                if(mTime >= mFields.fadedTime)
                {
                    mTime = 0.0f;
                    mPhase = FadePhase::FADE_OUT;
                }
            }
            return;

        case FadePhase::FADE_OUT:
            if(mTime < mFields.fadeOutTime)
            {
                mTime += relTime;
                fadeState = 1.0f - mTime / mFields.fadeOutTime;

            }else
            {
                fadeState = 0.0f;
                mPhase = FadePhase::NOT_TRIGGERED;
            }
            break;
        }

        mStates.fade = glm::clamp(fadeState, 0.0f, 1.0f);
        getLevelObject().extraStatesDirty();
    }

    void Fader_Sv::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
        switch(mPhase)
        {
        case FadePhase::NOT_TRIGGERED:
            if(mFields.startMode == StartMode::RUN_WHEN_TRIGGERED)
            {
                mPhase = FadePhase::FADE_IN;
                mTime = 0.0f;
            }
            break;

        case FadePhase::FADED:
            if(mFields.fadedTime < 0)
            {
                // TODO: does the numerical value of the message affect this in any way?
                mPhase = FadePhase::FADE_OUT;
                mTime = 0.0f;
            }
            break;

        default:
            break;
        }
    }


    Fader_Cl::Fader_Cl()
    {
    }

	void Fader_Cl::onSpawned()
	{
        auto &obj = getLevelObject();

        auto initialColorMod = mFields.color.asColorVector();

        if(obj.getClass()->hasModel())
        {
            obj.setupRenderingAndPhysics(od::ObjectRenderMode::NORMAL, od::ObjectPhysicsMode::NO_PHYSICS);
            mHandleToFade = obj.getRenderHandle();

            // if we should run instantly, make sure we start invisible until the first state update arrives
            initialColorMod.a = (mFields.startMode == StartMode::RUN_INSTANTLY) ? 0.0f : 1.0f;

        }else
        {
            auto &renderer = getClient().getRenderer();

            odGui::Quad quad(renderer);
            quad.setVertexCoords({0.0f,0.0f}, {1.0f,1.0f});
            quad.setColor({1.0f, 1.0f, 1.0f, 1.0f});

            mHandleToFade = quad.getHandle();
            mHandleToFade->setVisible(getLevelObject().isVisible());

            renderer.moveToRenderSpace(mHandleToFade, odRender::RenderSpace::GUI);

            // while models start as visible, full-screen quads start invisible
            initialColorMod.a = 0.0f;
        }

        if(mHandleToFade != nullptr)
        {
            mHandleToFade->setEnableColorModifier(true);
            mHandleToFade->setColorModifier(initialColorMod);
            mHandleToFade->setRenderBin(odRender::RenderBin::TRANSPARENT);
        }
	}

    void Fader_Cl::onVisibilityChanged(bool v)
    {
        if(mHandleToFade != nullptr)
        {
            mHandleToFade->setVisible(v);
        }
    }

    void Fader_Cl::onExtraStatesChanged()
    {
        if(mHandleToFade != nullptr)
        {
            auto colorMod = mFields.color.asColorVector();
            colorMod.a = mStates.fade.get();
            mHandleToFade->setColorModifier(colorMod);
        }
    }

}
