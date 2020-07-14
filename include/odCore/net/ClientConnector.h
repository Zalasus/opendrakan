/*
 * @file ClientConnector.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_NET_CLIENTCONNECTOR_H_
#define INCLUDE_ODCORE_NET_CLIENTCONNECTOR_H_

#include <future>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/LevelObject.h>

namespace odNet
{

    /**
     * @brief Class for encapsulating any combination of tranlation, rotation and scaling.
     */
    struct ObjectTransform
    {
        using TransformTypeMask = uint8_t;
        static constexpr TransformTypeMask TRANSLATED = 1;
        static constexpr TransformTypeMask ROTATED    = 2;
        static constexpr TransformTypeMask SCALED     = 4;

        TransformTypeMask type;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        ObjectTransform()
        : type(0)
        {
        }

        void setPosition(const glm::vec3 &p)
        {
            position = p;
            type |= TRANSLATED;
        }

        void setRotation(const glm::quat &r)
        {
            rotation = r;
            type |= ROTATED;
        }

        void setScale(const glm::vec3 &s)
        {
            scale = s;
            type |= SCALED;
        }

    };


    enum class CommandResult
    {
        ACK,
        NACK,
        TIMEOUT
    };


    class ClientConnector
    {
    public:

        virtual ~ClientConnector() = default;

        virtual std::future<CommandResult> loadLevel(const std::string &path) = 0;
        virtual std::future<CommandResult> levelObjectTranformed(od::LevelObjectId id, const ObjectTransform &tf) = 0;

    };

}

#endif
