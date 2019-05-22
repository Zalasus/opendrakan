/*
 * Array.h
 *
 *  Created on: May 20, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_ARRAY_H_
#define INCLUDE_ODCORE_RENDER_ARRAY_H_

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <odCore/Exception.h>

namespace odRender
{

    /**
     * @brief An abitrary-length sequence container for vertex data.
     *
     * This is an abstraction of a generic renderer array. Generally, this is equivalent to
     * an std::vector<T>.
     *
     * Renderer implementations must provide their arrays as this type. Neccessary conversions can be
     * done in the ArrayAccessHandler that is required to access an array anyway.
     */
    template <typename T>
    using Array = std::vector<T>;

    /**
     * @brief Interface template for managing access to a geometry's arrays.
     *
     * Depending on the rendering system, it might not be possible to access an array directly. Thus,
     * this interface forces the user to acquire the array first, perform operations, and then release it.
     * This allows the renderer to copy array data to a buffer upon acquisition, and copy it back once
     * the user releases the handler.
     *
     * If the renderer can directly work with the data contained in an odRender::Array, it's implementations
     * of this interface can simply ignore the acquire() and release() calls, or use them for synchronization
     * only.
     *
     * The renderer must implement this for each array element type used. Whether it uses a template for this,
     * too, or implements each one manually is up to the renderer.
     *
     * TODO: read-only access without write-back
     */
    template <typename T>
    class ArrayAccessHandler
    {
    public:

        typedef T _ElementType;

        virtual ~ArrayAccessHandler()
        {
            release();
        }

        virtual Array<_ElementType> &getArray() = 0;

        virtual void acquire() = 0;
        virtual void release() = 0;

    };

    typedef ArrayAccessHandler<glm::vec2> Vec2ArrayAccessor;
    typedef ArrayAccessHandler<glm::vec3> Vec3ArrayAccessor;
    typedef ArrayAccessHandler<glm::vec4> Vec4ArrayAccessor;


    /**
     * @brief Scoped RAII wrapper around an ArrayAccessHandler.
     *
     * This allows the user to use an ArrayAccessHandler just like a normal array.
     * The array is acquired upon creating the accessor and released upon it's destruction.
     *
     * Non-virtual access to the array's methods is provided via delegates.
     */
    template <typename T>
    class ArrayAccessor
    {
    public:

        explicit ArrayAccessor(std::unique_ptr<ArrayAccessHandler<T>> &&handler)
        : mHandler(std::move(handler))
        {
            if(mHandler == nullptr)
            {
                throw od::Exception("Created ArrayAccessor from null ArrayAccessHandler");
            }

            mHandler->acquire();

            mArray = &mHandler->getArray();
        }

        ArrayAccessor(ArrayAccessor<T> &aa) = delete;

        ~ArrayAccessor()
        {
            mHandler->release();
        }

        T &operator[](int index)
        {
            return (*mArray)[index];
        }

        size_t size() const
        {
            return mArray->size();
        }

        void resize(size_t s)
        {
            mArray->resize(s);
        }


    private:

        std::unique_ptr<ArrayAccessHandler<T>> mHandler;
        Array<T> *mArray;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_ARRAY_H_ */
