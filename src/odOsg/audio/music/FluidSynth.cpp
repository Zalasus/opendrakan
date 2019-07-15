/*
 * FluidSynth.cpp
 *
 *  Created on: 25 Jun 2019
 *      Author: zal
 */

#include <odOsg/audio/music/FluidSynth.h>

#include <sstream>
#include <cstdio>

#include <fluidsynth/misc.h>
#include <fluidsynth/settings.h>
#include <fluidsynth/sfont.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>

#include <odCore/db/MusicContainer.h>

namespace odOsg
{

    class DlsLoaderWrapper
    {
    public:

        DlsLoaderWrapper(odDb::MusicContainer &container, const od::Guid &guid)
        : mContainer(container)
        , mGuid(guid)
        , mInputCursor(mContainer.getCursorForDls(guid))
        , mReader(mInputCursor.getReader())
        {
        }

        void read(void *buf, int count)
        {
            mReader.read(reinterpret_cast<char*>(buf), count);
        }

        void seek(long offset, int origin)
        {
            switch(origin)
            {
            case SEEK_SET:
                mReader.seek(offset);
                break;

            case SEEK_CUR:
                mReader.seek(mReader.tell() + offset);
                break;

            default:
                throw od::Exception("Unsupported seek offset");
            }
        }

        long tell()
        {
            return mReader.tell();
        }

        static void *_dlsLoader_open(const char *filename)
        {
            if(filename[0] != '&')
            {
                return nullptr;
            }

            void *ptr = nullptr;
            std::istringstream ss(filename+1);
            ss >> ptr;

            if(ss.fail())
            {
                return nullptr;
            }

            return ptr;
        }

        static int _dlsLoader_read(void *buf, int count, void *handle)
        {
            if(handle == nullptr) return FLUID_FAILED;

            DlsLoaderWrapper *loader = reinterpret_cast<DlsLoaderWrapper*>(handle);
            loader->read(buf, count);

            return FLUID_OK;
        }

        static int _dlsLoader_seek(void *handle, long offset, int origin)
        {
            if(handle == nullptr) return FLUID_FAILED;

            DlsLoaderWrapper *loader = reinterpret_cast<DlsLoaderWrapper*>(handle);
            loader->seek(offset, origin);

            return FLUID_OK;
        }

        static int _dlsLoader_close(void *handle)
        {
            return FLUID_OK;
        }

        static long _dlsLoader_tell(void *handle)
        {
            if(handle == nullptr) return FLUID_FAILED;

            DlsLoaderWrapper *loader = reinterpret_cast<DlsLoaderWrapper*>(handle);
            return loader->tell();
        }


    private:

        odDb::MusicContainer &mContainer;
        od::Guid &mGuid;
        od::SrscFile::RecordInputCursor mInputCursor;
        od::DataReader mReader;

    };


    static void _errorCheck(int errorCode, const char *errorMessage)
    {
        if(errorCode != FLUID_OK)
        {
            throw od::Exception(errorMessage);
        }
    }


    class Settings
    {
    public:

        Settings()
        {
            mInstance = new_fluid_settings();
            if(mInstance == nullptr)
            {
                throw od::Exception("Failed to create fluid synth settings");
            }
        }

        ~Settings()
        {
            delete_fluid_settings(mInstance);
        }

        inline fluid_settings_t *getInstance() { return mInstance; }

        void setString(const char *key, const char *value)
        {
            fluid_settings_setstr(mInstance, key, value);
        }


    private:

        fluid_settings_t *mInstance;

    };


    FluidSynth::FluidSynth()
    : mSynth(nullptr)
    , mMusicContainer(nullptr)
    {
        Settings settings;
        settings.setString("player.timing-source", "sample");

        mSynth = new_fluid_synth(settings.getInstance());
        if(mSynth == nullptr) throw od::Exception("Failed to create fluid synth");

        fluid_sfloader_t *sfloader = new_fluid_defsfloader(settings.getInstance()); // TODO: do I have to delete this? o.o
        if(sfloader == nullptr) throw od::Exception("Failed to create fluid soundfont loader");
        int result = fluid_sfloader_set_callbacks(sfloader,
                                                  &DlsLoaderWrapper::_dlsLoader_open,
                                                  &DlsLoaderWrapper::_dlsLoader_read,
                                                  &DlsLoaderWrapper::_dlsLoader_seek,
                                                  &DlsLoaderWrapper::_dlsLoader_tell,
                                                  &DlsLoaderWrapper::_dlsLoader_close);
        _errorCheck(result, "Failed to set soundfont loader callbacks");
        result = fluid_synth_add_sfloader(mSynth, sfloader);
        _errorCheck(result, "Failed to add soundfont loader to synth");
    }

    FluidSynth::~FluidSynth()
    {
        delete_fluid_synth(mSynth);
    }

    void FluidSynth::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        int result = fluid_synth_noteon(mSynth, channel, note, velocity);
        _errorCheck(result, "Failed to send noteon message");
    }

    void FluidSynth::noteOff(uint8_t channel, uint8_t note)
    {
        int result = fluid_synth_noteoff(mSynth, channel, note);
        _errorCheck(result, "Failed to send noteoff message");
    }

    void FluidSynth::pitchBend(uint8_t channel, uint16_t value)
    {
        int result = fluid_synth_pitch_bend(mSynth, channel, value);
        _errorCheck(result, "Failed to send pitch bend message");
    }

    void FluidSynth::controllerChange(uint8_t channel, uint8_t controller, uint8_t value)
    {
        int result = fluid_synth_cc(mSynth, channel, controller, value);
        _errorCheck(result, "Failed to send controller change message");
    }

    void FluidSynth::channelPressure(uint8_t channel, uint8_t value)
    {
        int result = fluid_synth_channel_pressure(mSynth, channel, value);
        _errorCheck(result, "Failed to send channel pressure message");
    }

    void FluidSynth::keyPressure(uint8_t channel, uint8_t key, uint8_t value)
    {
        int result = fluid_synth_key_pressure(mSynth, channel, key, value);
        _errorCheck(result, "Failed to send key pressure message");
    }

    void FluidSynth::allNotesOff()
    {
    }

    void FluidSynth::fillInterleavedStereoBuffer(int16_t *buffer, size_t size)
    {
        int result = fluid_synth_write_s16(mSynth, size/2, buffer, 0, 2, buffer, 1, 2);
        _errorCheck(result, "Synthesis failed");
    }

    void FluidSynth::preloadDls(const od::Guid &dlsGuid)
    {
        _getOrLoadDls(dlsGuid);
    }

    void FluidSynth::assignPreset(uint8_t channel, uint32_t bank, uint32_t patch, const od::Guid &dlsGuid)
    {
        int sfId = _getOrLoadDls(dlsGuid);

        int result = fluid_synth_program_select(mSynth, channel, sfId, bank, patch);
        _errorCheck(result, "Failed to select program");
    }

    int FluidSynth::_getOrLoadDls(const od::Guid &dlsGuid)
    {
        auto it = mSoundFontIdMap.find(dlsGuid);
        if(mSoundFontIdMap.find(dlsGuid) != mSoundFontIdMap.end())
        {
            return it->second;
        }

        if(mMusicContainer == nullptr)
        {
            throw od::Exception("No music container provided to FluidSynth when a DLS needed to be loaded");
        }

        // beware: pretty bad pointer/string abuse. there is much that can go wrong here, but as I see it, fluidsynth
        //         doesn't offer a better solution for this without forcing us into writing our own DLS loader

        DlsLoaderWrapper loaderWrapper(*mMusicContainer, dlsGuid);

        std::ostringstream ss("&");
        ss << static_cast<void*>(&loaderWrapper);
        std::string str = ss.str();

        int sfId = fluid_synth_sfload(mSynth, str.c_str(), 0);
        if(sfId == FLUID_FAILED) throw od::Exception("Failed to load DLS");

        mSoundFontIdMap.insert(std::make_pair(dlsGuid, sfId));

        return sfId;
    }

}
