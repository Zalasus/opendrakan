/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "db/Model.h"

#include <algorithm>
#include <osg/Geometry>
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

			mTextureRefs.push_back(textureRef);
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

			Face f;
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

		// bone names
		uint16_t boneNameCount;
		dr >> boneNameCount;
		for(size_t i = 0; i < boneNameCount; ++i)
		{
			// NOTE: this may supposedly contain some empty entries
			char boneName[33] = { 0 };
			int32_t parentIndex;

			dr.read(boneName, 32);
			dr >> parentIndex;

			std::string boneNameStr(boneName);
		}

		// bone data
		uint16_t boneDataCount;
		dr >> boneDataCount;

		if(boneDataCount > boneNameCount)
		{
			throw Exception("More bone data than bones defined");
		}

		for(size_t boneIndex = 0; boneIndex < boneDataCount; ++boneIndex)
		{
			osg::Matrixf inverseBoneTransform;
			int32_t meshIndex; // Unknown use. These values can be seen in the editor.
            int32_t firstChildIndex; // negative if no children
            int32_t nextSiblingIndex; // negative if end of parent's child list

            dr >> inverseBoneTransform
			   >> meshIndex
			   >> firstChildIndex
			   >> nextSiblingIndex;

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
				}
            }
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
	}

	void Model::buildGeometry()
	{
		if(!mTexturesLoaded || !mVerticesLoaded || !mFacesLoaded)
		{
			throw Exception("Must load at least vertices, textures and faces before building geometry");
		}

		Logger::debug() << "Building geometry for model " << std::hex << getAssetId() << std::dec;

		LodMeshInfo mesh;
		if(mLodMeshInfos.size() == 0)
		{
			mesh.firstVertexIndex = 0;
			mesh.vertexCount = mVertices.size();
			mesh.firstFaceIndex = 0;
			mesh.faceCount = mFaces.size();

		}else
		{
			mesh = mLodMeshInfos[0];
		}
		std::vector<osg::Vec3f> vertices(mVertices.begin() + mesh.firstVertexIndex, mVertices.begin() + mesh.vertexCount);
		std::vector<Face> faces(mFaces.begin() + mesh.firstFaceIndex, mFaces.begin() + mesh.faceCount);

		SegmentedGeode::build(getDatabase(), vertices, faces, mTextureRefs.size());

		osgUtil::SmoothingVisitor sm;
        this->accept(sm);

        // model faces are oriented CW for some reason
        this->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE), osg::StateAttribute::ON);
	}
}

