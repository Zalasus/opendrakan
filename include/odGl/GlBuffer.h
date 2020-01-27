/**
 * @file GlBuffer.h
 * @date Jan 27, 2020
 * @author zal
 */

#ifndef INCLUDE_ODGL_GLBUFFER_H_
#define INCLUDE_ODGL_GLBUFFER_H_

#include <odGl/GL.h>

namespace odGl
{

    template <typename T>
    class GlBuffer
    {
    public:

        GlBuffer(GLuint location, GLint numComponents, GLenum componentType, bool keepLocalCopy)
        : mBufferId(0)
        {
            glGenBuffers(&mBufferId, 1);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferId);
            glVertexAttribPointer(location, numComponents, componentType, false, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        ~GlBuffer();

        void bind()
        {
            glBindBuffer()
        }


    private:

        GLuint mBufferId;

    };

}


#endif /* INCLUDE_ODGL_GLBUFFER_H_ */
