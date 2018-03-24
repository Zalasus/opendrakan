/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "db/Model.h"

#include <algorithm>
#include <limits>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/FrontFace>
#include <osgUtil/SmoothingVisitor>

#include "db/Asset.h"
#include "Exception.h"
#include "db/Database.h"
#include "OdDefines.h"
#include "db/ModelFactory.h"
#include "db/Texture.h"
#include "db/Skeleton.h"

namespace od
{

	Model::Model(Database &db, RecordId modelId)
	: Asset(db, modelId)
	, mModelName("")
	, mTriangleCount(0)
	, mQuadCount(0)
	, mVerticesLoaded(false)
	, mTexturesLoaded(false)
	, mFacesLoaded(false)
	{
	}

	void Model::loadNameAndShading(ModelFactory &factory, DataReader &&dr)
	{
		dr >> mModelName;

		uint32_t shadingType;
		dr >> shadingType;
	}

	void Model::loadVertices(ModelFactory &factory, DataReader &&dr)
	{
		uint16_t vertexCount;
		dr >> vertexCount;

		mVertices.reserve(vertexCount);
		for(size_t i = 0; i < vertexCount; ++i)
		{
			osg::Vec3f vertex;

			dr >> vertex;

			mVertices.push_back(vertex);
		}

		mVerticesLoaded = true;
	}

	void Model::loadTextures(ModelFactory &factory, DataReader &&dr)
	{
		uint32_t textureCount;
		dr >> textureCount;

		mTextureRefs.reserve(textureCount);
		for(size_t i = 0; i < textureCount; ++i)
		{
			AssetRef textureRef;
			dr >> textureRef;

			if(!textureRef.isNullTexture())
			{
				mTextureRefs.push_back(textureRef);
			}
		}

		mTexturesLoaded = true;
	}

	void Model::loadFaces(ModelFactory &factory, DataReader &&dr)
	{
		if(!mTexturesLoaded || !mVerticesLoaded)
		{
			throw Exception("Must load vertices and textures before loading faces!");
		}

		uint16_t faceCount;
		dr >> faceCount;

		mFaces.reserve(faceCount);
		for(size_t i = 0; i < faceCount; ++i)
		{
			uint16_t textureIndex;
			uint16_t vertexCount;

			dr >> DataReader::Ignore(2)
			   >> vertexCount
			   >> textureIndex;

			SegmentedGeode::Face f;
			f.texture = mTextureRefs[textureIndex];
			f.vertexCount = vertexCount;

			if(f.vertexCount == 3)
			{
				++mTriangleCount;

			}else if(f.vertexCount == 4)
			{
				++mQuadCount;

			}else
			{
				throw UnsupportedException("Can't load model with non-triangle/non-quad primitives");
			}

			for(size_t i = 0; i < f.vertexCount; ++i)
			{
				uint16_t vertexIndex;

				dr >> vertexIndex
				   >> f.vertexUvCoords[i];

				f.vertexIndices[i] = vertexIndex; // TODO: instead of doing it this way, maybe add a "readAs<type>" modifier?
			}

			mFaces.push_back(f);
		}

		mFacesLoaded = true;
	}

	void Model::loadLodsAndBones(ModelFactory &factory, DataReader &&dr)
	{
	    mVertexAffections.resize(mVertices.size());

		uint16_t lodCount;
		std::vector<std::string> lodNames;

		dr >> DataReader::Ignore(16*4) // bounding info (16 floats)
		   >> lodCount;

		if(lodCount == 0)
		{
			throw Exception("Expected at least one LOD in model");
		}

		lodNames.resize(lodCount - 1);
		for(int32_t i = 0; i < lodCount - 1; ++i)
		{
			dr >> lodNames[i];
		}


		SkeletonBuilder sb;

		// node info
		uint16_t nodeInfoCount;
		dr >> nodeInfoCount;
		for(size_t i = 0; i < nodeInfoCount; ++i)
		{
			char nodeName[33] = { 0 };
			int32_t jointInfoIndex;

			dr.read(nodeName, 32);
			dr >> jointInfoIndex;

			sb.addBoneNode(std::string(nodeName), jointInfoIndex);
		}

		// joint info
		uint16_t jointInfoCount;
		dr >> jointInfoCount;
		int32_t maxVertexIndex = -1;
		for(size_t jointIndex = 0; jointIndex < jointInfoCount; ++jointIndex)
		{
			osg::Matrixf inverseBoneTransform;
			int32_t meshIndex;
            int32_t firstChildIndex;
            int32_t nextSiblingIndex;

            dr >> inverseBoneTransform
			   >> meshIndex
			   >> firstChildIndex
			   >> nextSiblingIndex;

            sb.addJointInfo(inverseBoneTransform, meshIndex, firstChildIndex, nextSiblingIndex);

            // affected vertex lists, one for each LOD
            for(size_t lodIndex = 0; lodIndex < lodCount; ++lodIndex)
            {
				uint16_t affectedVertexCount;
				dr >> affectedVertexCount;
				for(size_t vertexIndex = 0; vertexIndex < affectedVertexCount; ++vertexIndex)
				{
					uint32_t affectedVertexIndex;
					float weight;
					dr >> affectedVertexIndex
					   >> weight;

					maxVertexIndex = std::max(maxVertexIndex, (int32_t)affectedVertexIndex);

					// count only for first lod as we don't have lod info yet
					if(lodIndex == 0)
					{
                        if(affectedVertexIndex >= mVertexAffections.size())
                        {
                            throw Exception("Affected vertex's index in bone data out of bounds");
                        }
                        mVertexAffections[affectedVertexIndex].affectingBoneCount++;
				    }
				}
            }
		}

        sb.build();

		size_t maxAffection = 0;
		for(VertexAffection rv : mVertexAffections)
		{
		    maxAffection = std::max(rv.affectingBoneCount, maxAffection);
		}


		// lod info
		mLodMeshInfos.resize(lodCount);
		for(size_t lodIndex = 0; lodIndex < lodCount; ++lodIndex)
		{
			uint16_t meshCount;
			dr >> meshCount;

			if(meshCount != 1)
			{
				throw UnsupportedException("Multi-mesh-models currently unsupported");
			}

			for(size_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
			{
				LodMeshInfo &mesh = mLodMeshInfos[lodIndex];

				dr >> mesh.distanceThreshold
				   >> mesh.usage
				   >> mesh.nodeIndex
				   >> mesh.firstVertexIndex
				   >> mesh.vertexCount
				   >> mesh.firstFaceIndex
				   >> mesh.faceCount;

				mesh.lodName = (lodIndex == 0) ? mModelName : lodNames[lodIndex - 1];
			}
		}


		// animations refs
		uint16_t animCount;
		dr >> animCount;
		mAnimationRefs.resize(animCount);
		for(size_t animIndex = 0; animIndex < animCount; ++animIndex)
		{
			// the database index of this is always zero. however, i see no reason not to support loading
			//  animations cross-database, so we don't enforce this.
			dr >> mAnimationRefs[animIndex];
		}


		// channels
		uint16_t channelCount;
		dr >> channelCount;
		for(size_t channelIndex = 0; channelIndex < channelCount; ++channelIndex)
		{
			uint32_t nodeIndex;
			osg::Matrixf xformA;
			osg::Matrixf xformB;
            uint16_t capCount;

            dr >> nodeIndex
			   >> xformA
			   >> xformB
			   >> capCount;

            sb.makeChannel(nodeIndex);

            for(size_t capIndex = 0; capIndex < capCount; ++capIndex)
            {
            	uint32_t firstCapFaceIndex;
                uint32_t capFaceCount;
                uint32_t firstPartFaceIndex;
                uint32_t partFaceCount;
                uint32_t unk;
                uint16_t vertexCount;

                dr >> firstCapFaceIndex
				   >> capFaceCount
				   >> firstPartFaceIndex
				   >> partFaceCount
				   >> unk
				   >> vertexCount;

				for(size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
				{
					uint32_t affectedVertexIndex;
					float weight;
					dr >> affectedVertexIndex
					   >> weight;
				}
            }
		}


		Logger::info() << "Model " << mModelName;
		Logger::info() << "Max affecting bones per vertex: " << maxAffection;
		Logger::info() << "Max affected vertex index: " << maxVertexIndex << " Vertex count: " << mVertices.size() << " Lod count: " << mLodMeshInfos.size();
		//Logger::info() << "Printing skeleton stats:";
		//sb.printInfo(std::cout);
 	}

	void Model::buildGeometry()
	{
		if(!mTexturesLoaded || !mVerticesLoaded || !mFacesLoaded)
		{
			throw Exception("Must load at least vertices, textures and faces before building geometry");
		}

		if(mLodMeshInfos.size() > 0)
		{
			osg::ref_ptr<osg::LOD> lodNode(new osg::LOD);
			lodNode->setRangeMode(osg::LOD::DISTANCE_FROM_EYE_POINT);
			lodNode->setCenterMode(osg::LOD::USE_BOUNDING_SPHERE_CENTER);

			for(auto it = mLodMeshInfos.begin(); it != mLodMeshInfos.end(); ++it)
			{
				osg::ref_ptr<SegmentedGeode> newGeode(new SegmentedGeode);

				// FIXME: LOD meshes have holes. somehow we don't cover all faces here
				auto verticesBegin = mVertices.begin() + it->firstVertexIndex;
				auto verticesEnd = mVertices.begin() + it->vertexCount + it->firstVertexIndex;
				auto facesBegin = mFaces.begin() + it->firstFaceIndex;
				auto facesEnd = mFaces.begin() + it->faceCount + it->firstFaceIndex;

				newGeode->build(getDatabase(), verticesBegin, verticesEnd, facesBegin, facesEnd, mTextureRefs.size());

				float minDistance = it->distanceThreshold;
				float maxDistance = ((it+1) == mLodMeshInfos.end()) ? std::numeric_limits<float>::max() : (it+1)->distanceThreshold;
				lodNode->addChild(newGeode, minDistance, maxDistance);
			}

			this->addChild(lodNode);

		}else
		{
			osg::ref_ptr<SegmentedGeode> newGeode(new SegmentedGeode);
			newGeode->build(getDatabase(), mVertices.begin(), mVertices.end(), mFaces.begin(), mFaces.end(), mTextureRefs.size());
			this->addChild(newGeode);
		}

        // model faces are oriented CW for some reason
        this->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE), osg::StateAttribute::ON);
	}
}

