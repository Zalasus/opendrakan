/*
 * Array.h
 *
 *  Created on: May 20, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_ARRAY_H_
#define INCLUDE_ODCORE_RENDER_ARRAY_H_

#include <vector>

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
     */
    template <typename T>
    class ArrayAccessHandler
    {
    public:

        typedef T _ElementType;

        virtual ~ArrayAccessHandler() = default;

        virtual Array<_ElementType> &getArray() = 0;

        virtual void acquire(bool readback) = 0;
        virtual void release(bool writeback) = 0;

    };

    typedef ArrayAccessHandler<glm::vec2> Vec2ArrayAccessor;
    typedef ArrayAccessHandler<glm::vec3> Vec3ArrayAccessor;
    typedef ArrayAccessHandler<glm::vec4> Vec4ArrayAccessor;

    /**
     * @brief Modes that allow a user to express how they wish to access an array via a ArrayAccessor.
     *
     * These are just optimizations hints that might not actually do anything, depending on the implementation.
     */
    enum class ArrayAccessMode
    {
        MODIFY, ///< Array contents are read from renderer when acquiring and written back when releasing.
        READ,   ///< Array contents are read from renderer, but modifications might be discarded.
        REPLACE ///< The initial array contents might not reflect the array state in the renderer (but size will).
    };

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

        ArrayAccessor(ArrayAccessHandler<T> &handler, ArrayAccessMode mode = ArrayAccessMode::MODIFY)
        : mHandler(handler)
        , mMode(mode)
        {
            bool readback = (mMode == ArrayAccessMode::MODIFY) || (mMode == ArrayAccessMode::READ);
            mHandler.acquire(readback);

            mArray = &mHandler.getArray();
        }

        ArrayAccessor(ArrayAccessor<T> &aa) = delete;

        ~ArrayAccessor()
        {
            bool writeback = (mMode == ArrayAccessMode::MODIFY) || (mMode == ArrayAccessMode::REPLACE);
            mHandler.release(writeback);
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

        void clear()
        {
            mArray->clear();
        }


    private:

        ArrayAccessHandler<T> &mHandler;
        ArrayAccessMode mMode;
        Array<T> *mArray;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_ARRAY_H_ */
