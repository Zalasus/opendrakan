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

#include "OdDefines.h"
#include "Exception.h"
#include "db/Asset.h"
#include "db/ModelFactory.h"
#include "db/Texture.h"
#include "db/Skeleton.h"

#define OD_POLYGON_FLAG_DOUBLESIDED 0x02

namespace od
{

	Model::Model(AssetProvider &ap, RecordId modelId)
	: Asset(ap, modelId)
	, mModelName("")
	, mVerticesLoaded(false)
	, mTexturesLoaded(false)
	, mPolygonsLoaded(false)
	{
	}

	void Model::loadNameAndShading(ModelFactory &factory, DataReader &&dr)
	{
		dr >> mModelName;

		uint32_t shadingType;
		dr >> shadingType;

		this->setName(mModelName);
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

	void Model::loadPolygons(ModelFactory &factory, DataReader &&dr)
	{
		if(!mTexturesLoaded || !mVerticesLoaded)
		{
			throw Exception("Must load vertices and textures before loading polygons!");
		}

		uint16_t polygonCount;
		dr >> polygonCount;

		mPolygons.reserve(polygonCount);
		for(size_t i = 0; i < polygonCount; ++i)
		{
			uint16_t flags;
			uint16_t textureIndex;
			uint16_t vertexCount;

			dr >> flags
			   >> vertexCount
			   >> textureIndex;

			Polygon poly;
			poly.doubleSided = flags & OD_POLYGON_FLAG_DOUBLESIDED;
			poly.texture = mTextureRefs[textureIndex];
			poly.vertexCount = vertexCount;

			if(poly.vertexCount != 3 && poly.vertexCount != 4)
			{
				throw UnsupportedException("Can't load model with non-triangle/non-quad primitives");
			}

			for(size_t i = 0; i < poly.vertexCount; ++i)
			{
				uint16_t vertexIndex;

				dr >> vertexIndex
				   >> poly.uvCoords[i];

				poly.vertexIndices[i] = vertexIndex; // TODO: instead of doing it this way, maybe add a "readAs<type>" modifier?
			}

			mPolygons.push_back(poly);
		}

		mPolygonsLoaded = true;
	}

	void Model::loadBoundingData(ModelFactory &factory, DataReader &&dr)
	{
		if(isCharacter())
		{
			throw Exception("Character models can't have bounds info");
		}

		osg::BoundingSpheref mainBs;
		OrientedBoundingBox mainObb;
		uint16_t shapeCount;
		uint16_t shapeType; // 0 = spheres, 1 = boxes

		dr >> mainBs
		   >> mainObb
		   >> shapeCount
		   >> shapeType;

		ModelBounds::ShapeType type = (shapeType == 0) ? ModelBounds::SPHERES : ModelBounds::BOXES;
		mModelBounds.reset(new ModelBounds(type, shapeCount));
		mModelBounds->setMainBounds(mainBs, mainObb);

		for(size_t i = 0; i < shapeCount; ++i)
		{
			uint16_t firstChildIndex;
			uint16_t nextSiblingIndex;
			dr >> firstChildIndex
			   >> nextSiblingIndex;

			mModelBounds->addHierarchyEntry(firstChildIndex, nextSiblingIndex);
		}

		for(size_t i = 0; i < shapeCount; ++i)
		{
			if(shapeType == 0)
			{
				osg::BoundingSpheref bs;
				dr >> bs;
				mModelBounds->addSphere(bs);

			}else
			{
				OrientedBoundingBox obb;
				dr >> obb;
				mModelBounds->addBox(obb);

				// ignore the field of words after each box
				uint16_t unkPolyCount;
				dr >> unkPolyCount;
				dr.ignore(unkPolyCount*2);
			}
		}

		//Logger::info() << "Bounding data for model " << mModelName;
		//mModelBounds->printInfo();

		mModelBounds->getCollisionShape(); // to trigger building the shape
	}

	void Model::loadLodsAndBones(ModelFactory &factory, DataReader &&dr)
	{
		if(hasBounds())
		{
			throw Exception("Character models can't have bounds info");
		}

		uint16_t lodCount;
		std::vector<std::string> lodNames;

		dr >> DataReader::Ignore(16*4) // bounding info (16 floats)
		   >> lodCount;

		if(lodCount == 0)
		{
			throw Exception("Expected at least one LOD in model");
		}
		mLodMeshInfos.resize(lodCount);

		lodNames.resize(lodCount - 1);
		for(int32_t i = 0; i < lodCount - 1; ++i)
		{
			dr >> lodNames[i];
		}


		mSkeletonBuilder.reset(new SkeletonBuilder);

		// node info
		uint16_t nodeInfoCount;
		dr >> nodeInfoCount;
		for(size_t i = 0; i < nodeInfoCount; ++i)
		{
			char nodeName[33] = { 0 };
			int32_t jointInfoIndex;

			dr.read(nodeName, 32);
			dr >> jointInfoIndex;

			mSkeletonBuilder->addBoneNode(std::string(nodeName), jointInfoIndex);
		}

		// joint info
		uint16_t jointInfoCount;
		dr >> jointInfoCount;
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

            mSkeletonBuilder->addJointInfo(inverseBoneTransform, meshIndex, firstChildIndex, nextSiblingIndex);

            // affected vertex lists, one for each LOD
            for(size_t lodIndex = 0; lodIndex < lodCount; ++lodIndex)
            {
            	std::vector<BoneAffection> &boneAffections = mLodMeshInfos[lodIndex].boneAffections;

				uint16_t affectedVertexCount;
				dr >> affectedVertexCount;
				boneAffections.reserve(boneAffections.size() + affectedVertexCount);
				for(size_t vertexIndex = 0; vertexIndex < affectedVertexCount; ++vertexIndex)
				{
					uint32_t affectedVertexIndex;
					float weight;
					dr >> affectedVertexIndex
					   >> weight;

					BoneAffection bAff;
					bAff.jointIndex = jointIndex;
					bAff.vertexIndex = affectedVertexIndex;
					bAff.vertexWeight = weight;
					boneAffections.push_back(bAff);
				}

				if(lodIndex == 0)
				{
					mSkeletonBuilder->pfffSetWeightCount(affectedVertexCount);
				}
            }


		}

		// lod info
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
				   >> mesh.firstPolygonIndex
				   >> mesh.polygonCount;

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
			uint32_t jointIndex;
			osg::Matrixf xformA;
			osg::Matrixf xformB;
            uint16_t capCount;

            dr >> jointIndex
			   >> xformA
			   >> xformB
			   >> capCount;

            mSkeletonBuilder->makeChannel(jointIndex);

            for(size_t capIndex = 0; capIndex < capCount; ++capIndex)
            {
            	uint32_t firstCapPolygonIndex;
                uint32_t capPolygonCount;
                uint32_t firstPartPolygonIndex;
                uint32_t partPolygonCount;
                uint32_t unk;
                uint16_t affectedVertexCount;

                dr >> firstCapPolygonIndex
				   >> capPolygonCount
				   >> firstPartPolygonIndex
				   >> partPolygonCount
				   >> unk
				   >> affectedVertexCount;

				for(size_t vertexIndex = 0; vertexIndex < affectedVertexCount; ++vertexIndex)
				{
					uint32_t affectedVertexIndex;
					float weight;
					dr >> affectedVertexIndex
					   >> weight;
				}
            }
		}

		//Logger::info() << "Skel info for model " << mModelName;
		//mSkeletonBuilder->printInfo(std::cout);
 	}

	void Model::buildGeometry()
	{
		if(!mTexturesLoaded || !mVerticesLoaded || !mPolygonsLoaded)
		{
			throw Exception("Must load at least vertices, textures and polygons before building geometry");
		}

		if(mLodMeshInfos.size() > 0)
		{
			osg::ref_ptr<osg::LOD> lodNode(new osg::LOD);
			lodNode->setRangeMode(osg::LOD::DISTANCE_FROM_EYE_POINT);
			lodNode->setCenterMode(osg::LOD::USE_BOUNDING_SPHERE_CENTER);

			for(auto it = mLodMeshInfos.begin(); it != mLodMeshInfos.end(); ++it)
			{
				GeodeBuilder gb(it->lodName, this->getAssetProvider());
				gb.setClampTextures(false);

				// the count fields in the mesh info sometimes do not cover all vertices and polygons. gotta be something with those "LOD caps"
				//  instead of using those values, use all vertices up until the next lod until we figure out how else to handle this
				size_t actualVertexCount = ((it+1 == mLodMeshInfos.end()) ? mVertices.size() : (it+1)->firstVertexIndex) - it->firstVertexIndex;
				size_t actualPolyCount = ((it+1 == mLodMeshInfos.end()) ? mPolygons.size() : (it+1)->firstPolygonIndex) - it->firstPolygonIndex;

				auto verticesBegin = mVertices.begin() + it->firstVertexIndex;
				auto verticesEnd = mVertices.begin() + actualVertexCount + it->firstVertexIndex;
				gb.setVertexVector(verticesBegin, verticesEnd);

				auto polygonsBegin = mPolygons.begin() + it->firstPolygonIndex;
				auto polygonsEnd = mPolygons.begin() + actualPolyCount + it->firstPolygonIndex;
				gb.setPolygonVector(polygonsBegin, polygonsEnd);

				auto bonesBegin = it->boneAffections.begin();
				auto bonesEnd = it->boneAffections.end();
				gb.setBoneAffectionVector(bonesBegin, bonesEnd);

				osg::ref_ptr<osg::Geode> newGeode(new osg::Geode);
				gb.build(newGeode);

				float minDistance = it->distanceThreshold;
				float maxDistance = ((it+1) == mLodMeshInfos.end()) ? std::numeric_limits<float>::max() : (it+1)->distanceThreshold;
				lodNode->addChild(newGeode, minDistance, maxDistance);
			}

			this->addChild(lodNode);

		}else
		{
			GeodeBuilder gb(mModelName, this->getAssetProvider());
			gb.setClampTextures(false);
			gb.setVertexVector(mVertices.begin(), mVertices.end());
			gb.setPolygonVector(mPolygons.begin(), mPolygons.end());

			osg::ref_ptr<osg::Geode> newGeode(new osg::Geode);
			gb.build(newGeode);

			this->addChild(newGeode);
		}

        // model faces are oriented CW for some reason
        this->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE), osg::StateAttribute::ON);
	}
}

