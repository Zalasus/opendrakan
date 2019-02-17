/*
 * LayerHandleImpl.cpp
 *
 *  Created on: Feb 11, 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/LayerHandleImpl.h>

#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include <odCore/Layer.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/BulletPhysicsSystem.h>

namespace odBulletPhysics
{

    LayerHandle::LayerHandle(od::Layer &layer, btCollisionWorld *collisionWorld)
    : mLayer(layer)
    , mCollisionWorld(collisionWorld)
    {
        _buildCollisionShape();

        if(mShape != nullptr)
        {
            mCollisionObject = std::make_unique<btCollisionObject>();
            mCollisionObject->setCollisionShape(mShape.get());
            mCollisionObject->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

            mCollisionObject->setUserIndex(mLayer.getId());
            mCollisionObject->setUserPointer(static_cast<odPhysics::Handle*>(this));

            glm::vec3 worldOffset = mLayer.getOrigin();
            btTransform worldTransform(btQuaternion(0, 0, 0, 1), BulletAdapter::toBullet(worldOffset));
            mCollisionObject->setWorldTransform(worldTransform);

            mCollisionWorld->addCollisionObject(mCollisionObject.get(), BulletCollisionGroups::LAYER, BulletCollisionGroups::ALL);
        }
    }

    LayerHandle::~LayerHandle()
    {
        if(mCollisionObject != nullptr)
        {
            mCollisionObject->setUserIndex(-1);
            mCollisionObject->setUserPointer(nullptr);
            mCollisionWorld->removeCollisionObject(mCollisionObject.get());
        }
    }

    void LayerHandle::_buildCollisionShape()
    {
        if(mLayer.getCollidingTriangleCount() == 0)
        {
            return;
        }

        const std::vector<od::Layer::Vertex> &vertices = mLayer.getVertexVector();
        const std::vector<od::Layer::Cell> &cells = mLayer.getCellVector();
        uint32_t width = mLayer.getWidth();
        uint32_t height = mLayer.getHeight();

        bool mustUse32BitIndices = (vertices.size() - 1 > 0xffff); // should save us some memory most of the time
        mTriMesh = std::make_unique<btTriangleMesh>(mustUse32BitIndices, false);
        btTriangleMesh *mesh = mTriMesh.get(); // because we call members very often and unique_ptr has some overhead

        // first, add all vertices in grid to shape
        mesh->preallocateVertices(vertices.size() * 3); // bullet seems to be buggy here. it actually needs 3 times the space it reserves
        for(size_t i = 0; i < vertices.size(); ++i)
        {
            size_t aXRel = i%(width+1);
            size_t aZRel = i/(width+1); // has to be an integer operation to floor it
            float aX = aXRel; // ignore origin so shape is relative to layer origin. we place it in world coords via the collision object
            float aZ = aZRel;

            mesh->findOrAddVertex(btVector3(aX, vertices[i].heightOffsetLu, aZ), false);
        }

        // second, push indices for each triangle, ignoring those without texture as these define holes the player can walk/fall through
        mesh->preallocateIndices(mLayer.getCollidingTriangleCount() * 3);
        for(size_t triIndex = 0; triIndex < width*height*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            od::Layer::Cell cell = cells[cellIndex];
            odDb::AssetRef texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;

            if(texture == od::Layer::HoleTextureRef) // unlike when building geometry, we want to include invisible triangles here!
            {
                continue;
            }

            int aZRel = cellIndex/width; // has to be an integer operation to floor it

            // calculate indices of corner vertices
            size_t a = cellIndex + aZRel; // add row index since we want to skip top right vertex in every row passed so far
            size_t b = a + 1;
            size_t c = a + (width+1); // one row below a, one row contains width+1 vertices
            size_t d = c + 1;

            if(!(cell.flags & OD_LAYER_FLAG_DIV_BACKSLASH))
            {
                if(isLeft)
                {
                    mesh->addTriangleIndices(c, b, a);

                }else
                {
                    mesh->addTriangleIndices(c, d, b);
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    mesh->addTriangleIndices(a, c, d);

                }else
                {
                    mesh->addTriangleIndices(a, d, b);
                }
            }
        }

        mShape = std::make_unique<btBvhTriangleMeshShape>(mesh, true, true);
    }

}



