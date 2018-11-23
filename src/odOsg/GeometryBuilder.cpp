/*
 * GeometryBuilder.cpp
 *
 *  Created on: 26 Mar 2018
 *      Author: zal
 */

#include <odOsg/GeometryBuilder.h>

#include <cassert>
#include <algorithm>

#include <osg/Geometry>

#include <odCore/Exception.h>
#include <odCore/db/AssetProvider.h>
#include <odCore/db/Texture.h>
#include <odCore/render/Geometry.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Geometry.h>

namespace odOsg
{

	GeometryBuilder::GeometryBuilder(Geometry &geometry, const std::string &geometryName, odDb::AssetProvider &assetProvider)
	: mGeometryName(geometryName)
	, mAssetProvider(assetProvider)
	, mSmoothNormals(true)
	, mNormalsFromCcw(false)
	, mGeometry(geometry)
	, mVertices(geometry.getVertexArray())
    , mNormals(geometry.getNormalArray())
    , mUvCoords(geometry.getTextureCoordArray())
    , mBoneIndices(geometry.getBoneIndexArray())
    , mBoneWeights(geometry.getBoneWeightArray())
	{
	    mVertices.clear();
	    mNormals.clear();
	    mUvCoords.clear();
	    mBoneIndices.clear();
	    mBoneWeights.clear();
	}

	void GeometryBuilder::setVertexVector(VertexIterator begin, VertexIterator end)
	{
		mVertices.assign(begin, end);
	}

	void GeometryBuilder::setPolygonVector(PolygonIterator begin, PolygonIterator end)
	{
		// need to iterate over polygons and convert quads to triangles and duplicate polygon if it is double-sided

	    size_t polygonCount = (end - begin); // assuming we only have single-sided triangles, this is the minimum we have to allocate

		mTriangles.clear();
		mTriangles.reserve(polygonCount * 1.05); // assume we need 5% more triangles than polys most of the time TODO: run a statistic on this

		for(auto it = begin; it != end; ++it)
		{
			if(it->vertexCount != 3 && it->vertexCount != 4)
			{
				throw od::UnsupportedException("Only triangle or quad polygons supported");
			}

			// the 0 1 2 triangle always appears
			Triangle tri;
			tri.texture = it->texture;
			tri.uvCoords[0] = it->uvCoords[0];
			tri.uvCoords[1] = it->uvCoords[1];
			tri.uvCoords[2] = it->uvCoords[2];
			tri.vertexIndices[0] = it->vertexIndices[0];
			tri.vertexIndices[1] = it->vertexIndices[1];
			tri.vertexIndices[2] = it->vertexIndices[2];
			mTriangles.push_back(tri);

			if(it->doubleSided)
			{
				// swapping verts 0 and 2 reverses winding order, thus flipping the triangle
				std::swap(tri.vertexIndices[0], tri.vertexIndices[2]);
				std::swap(tri.uvCoords[0], tri.uvCoords[2]);
				mTriangles.push_back(tri);
			}

			// if the poly is a quad, add another triangle
			if(it->vertexCount == 4)
			{
				tri.uvCoords[0] = it->uvCoords[0];
				tri.uvCoords[1] = it->uvCoords[2];
				tri.uvCoords[2] = it->uvCoords[3];
				tri.vertexIndices[0] = it->vertexIndices[0];
				tri.vertexIndices[1] = it->vertexIndices[2];
				tri.vertexIndices[2] = it->vertexIndices[3];
				mTriangles.push_back(tri);

				if(it->doubleSided)
				{
					std::swap(tri.vertexIndices[0], tri.vertexIndices[2]);
					std::swap(tri.uvCoords[0], tri.uvCoords[2]);
					mTriangles.push_back(tri);
				}
			}
		}
	}

	void GeometryBuilder::setBoneAffectionVector(BoneAffectionIterator begin, BoneAffectionIterator end)
	{
		// here we turn the BoneAffection objects into the index and weight vectors

	    if(begin == end)
	    {
	        mGeometry.setHasBoneInfo(false);
	        return;
	    }

		mBoneIndices.resize(mVertices.size(), glm::vec4(0.0));
		mBoneWeights.resize(mVertices.size(), glm::vec4(0.0)); // default weight of 0 will make unused bones uneffective,
															   //  regardless of index -> less logic in the vertex shader!

		std::vector<size_t> influencingBonesCount(mVertices.size(), 0);
		bool alreadyWarned = false; // flag preventing spamming of log if many verts exceed bone limit
		for(auto it = begin; it != end; ++it)
		{
			if(it->vertexIndex >= mVertices.size())
			{
				Logger::error() << "Affected vertex index of bone out of bounds. Was " << it->vertexIndex << " where size was " << mVertices.size();
				throw od::Exception("Affected vertex index of bone out of bounds");
			}

			size_t &currentBoneCount = influencingBonesCount[it->vertexIndex];
			if(currentBoneCount >= 4)
			{
				// TODO: perhaps overwrite bone with lowest weight rather than ignoring all bones past the fourth?
				if(!alreadyWarned)
				{
					Logger::warn() << "Found vertex with more than 4 affecting bones in '" << mGeometryName << "'. Ignoring excess bones";
					alreadyWarned = true;
				}

				continue;
			}

			mBoneIndices[it->vertexIndex][currentBoneCount] = it->jointIndex;
			mBoneWeights[it->vertexIndex][currentBoneCount] = it->vertexWeight;
			++currentBoneCount;
		}

		mGeometry.setHasBoneInfo(true);
	}

	void GeometryBuilder::build()
	{
		if(mSmoothNormals)
		{
		    _buildNormals();
		    _disambiguateAndGenerateUvs();

		}else
		{
		    _makeIndicesUniqueAndGenerateUvs();
		    _buildNormals();
		}

		// sort by texture. most models are already sorted, so this is O(n) most of the time
		auto pred = [](Triangle &left, Triangle &right){ return left.texture < right.texture; };
		std::sort(mTriangles.begin(), mTriangles.end(), pred);

		// count number of unique textures
		odDb::AssetRef lastTexture = odDb::AssetRef::NULL_REF;
		size_t textureCount = 0;
		for(auto it = mTriangles.begin(); it != mTriangles.end(); ++it)
        {
		    if(lastTexture == odDb::AssetRef::NULL_REF || lastTexture != it->texture)
		    {
		        lastTexture = it->texture;
		        ++textureCount;
		    }
        }

		// count number of triangles per texture.
        //  this will allow us to preallocate the IBO array as well as pick between int/short/byte arrays
		lastTexture = odDb::AssetRef::NULL_REF;
		size_t textureIndex = 0;
        std::vector<size_t> triangleCountsPerTexture(textureCount, 0);
        for(auto it = mTriangles.begin(); it != mTriangles.end(); ++it)
        {
            if(lastTexture == odDb::AssetRef::NULL_REF)
            {
                lastTexture = it->texture;
            }

            if(lastTexture != it->texture)
            {
                lastTexture = it->texture;
                textureIndex++;
            }

            triangleCountsPerTexture[textureIndex]++;
        }

        osg::ref_ptr<osg::Geode> geode = mGeometry.getOsgGeode();
        osg::ref_ptr<osg::Vec3Array> osgVertexArray = GlmAdapter::convertToOsgArray<osg::Vec3Array>(mVertices);
        osg::ref_ptr<osg::Vec3Array> osgNormalArray = GlmAdapter::convertToOsgArray<osg::Vec3Array>(mNormals);
        osg::ref_ptr<osg::Vec2Array> osgTextureCoordArray = GlmAdapter::convertToOsgArray<osg::Vec2Array>(mUvCoords);
        //osg::ref_ptr<osg::Vec4Array> osgBoneIndexArray = GlmAdapter::convertToOsgArray<osg::Vec4Array>(mBoneIndices);
        //osg::ref_ptr<osg::Vec4Array> osgBoneWeightArray = GlmAdapter::convertToOsgArray<osg::Vec4Array>(mBoneWeights);

        // give osg arrays to geometry
        mGeometry.setOsgVertexArray(osgVertexArray);
        //mGeometry.setOsgColorArray(osgColorArray);
        mGeometry.setOsgNormalArray(osgNormalArray);
        mGeometry.setOsgTextureCoordArray(osgTextureCoordArray);

		osg::ref_ptr<osg::Geometry> osgGeometry;
		osg::ref_ptr<osg::DrawElements> drawElements;
		lastTexture = odDb::AssetRef::NULL_REF;
		textureIndex = 0;
		for(auto it = mTriangles.begin(); it != mTriangles.end(); ++it)
		{
		    if(it->texture.isNull())
		    {
		        continue;
		    }

			if(lastTexture != it->texture || osgGeometry == nullptr)
			{
			    if(osgGeometry != nullptr)
			    {
			        assert(drawElements->getNumIndices() == triangleCountsPerTexture[textureIndex]*3);

			        ++textureIndex;
			    }

			    osgGeometry = new osg::Geometry;
			    osgGeometry->setVertexArray(osgVertexArray);
			    osgGeometry->setNormalArray(osgNormalArray);
			    osgGeometry->setTexCoordArray(0, osgTextureCoordArray);
			    geode->addDrawable(osgGeometry);

			    size_t vertsForThisTexture = triangleCountsPerTexture[textureIndex] * 3;
			    if(mVertices.size() <= 0xff)
                {
                    osg::ref_ptr<osg::DrawElementsUByte> drawElementsUbyte = new osg::DrawElementsUByte(osg::PrimitiveSet::TRIANGLES);
                    drawElementsUbyte->reserve(vertsForThisTexture);
                    drawElements = drawElementsUbyte;

                }else if(mVertices.size() <= 0xffff)
                {

                    osg::ref_ptr<osg::DrawElementsUShort> drawElementsUshort = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES);
                    drawElementsUshort->reserve(vertsForThisTexture);
                    drawElements = drawElementsUshort;

                }else
                {
                    osg::ref_ptr<osg::DrawElementsUInt> drawElementsUint = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
                    drawElementsUint->reserve(vertsForThisTexture);
                    drawElements = drawElementsUint;
                }
			    osgGeometry->addPrimitiveSet(drawElements);

				//odDb::AssetPtr<odDb::Texture> dbTexture = mAssetProvider.getAssetByRef<odDb::Texture>(it->texture);

				lastTexture = it->texture;
			}

			for(size_t vn = 0; vn < 3; ++vn)
			{
				drawElements->addElement(it->vertexIndices[vn]);
			}
		}
	}

	void GeometryBuilder::_buildNormals()
	{
		// calculate normals per triangle, sum them up for each vertex and normalize them in the end
		//  TODO: perhaps consider maximum crease here?

		mNormals.resize(mVertices.size(), glm::vec3(0.0));

		for(auto it = mTriangles.begin(); it != mTriangles.end(); ++it)
		{
			// note: Drakan uses CW orientation by default
			glm::vec3 normal = glm::cross((mVertices[it->vertexIndices[2]] - mVertices[it->vertexIndices[0]])
							   , (mVertices[it->vertexIndices[1]] - mVertices[it->vertexIndices[0]]));

			if(mNormalsFromCcw) normal *= -1;

			for(size_t i = 0; i < 3; ++i)
			{
			    size_t &vertIndex = it->vertexIndices[i];
                mNormals[vertIndex] += normal;
			}
		}

		for(auto it = mNormals.begin(); it != mNormals.end(); ++it)
		{
			*it = glm::normalize(*it);
		}
	}

	void GeometryBuilder::_makeIndicesUniqueAndGenerateUvs()
	{
	    // for flat shading, we can't use shared vertices. this method will duplicate all shared vertices
	    //  since that makes disambiguating UVs unnecessary, we create the UV array here, too.

	    std::vector<bool> visited(mVertices.size(), false);

        for(auto it = mTriangles.begin(); it != mTriangles.end(); ++it)
        {
            for(size_t vn = 0; vn < 3; ++vn)
            {
                size_t &vertIndex = it->vertexIndices[vn];

                if(!visited[vertIndex])
                {
                    visited[vertIndex] = true;

                }else
                {
                    // vertex is shared. duplicate it
                    mVertices.push_back(mVertices.at(vertIndex));

                    // if there is bone affection info, duplicate that too
                    if(mGeometry.hasBoneInfo())
                    {
                        mBoneIndices.push_back(mBoneIndices[vertIndex]);
                        mBoneWeights.push_back(mBoneWeights[vertIndex]);
                    }

                    // lastly, make sure the triangle uses that new vertex
                    vertIndex = mVertices.size() - 1;
                }
            }
        }

        // generating the UV array is straightforward now that no vertices are shared anymore
        mUvCoords.resize(mVertices.size(), glm::vec2(0.0));
        for(auto it = mTriangles.begin(); it != mTriangles.end(); ++it)
        {
            for(size_t vn = 0; vn < 3; ++vn)
            {
                size_t vertIndex = it->vertexIndices[vn];

                mUvCoords[vertIndex] = it->uvCoords[vn];
            }
        }
	}

	void GeometryBuilder::_disambiguateAndGenerateUvs()
	{
		// iterate over triangles and duplicate shared vertices when their uv coordinates are incompatible.
		//  NOTE: we share VBOs between all geometries. only IBOs are unique per texture. thus, we only need to
		//         make sure UVs are unique per vertex, not textures.

		mUvCoords.resize(mVertices.size(), glm::vec2(0.0));

		std::vector<bool> visited(mVertices.size(), false);

		for(auto it = mTriangles.begin(); it != mTriangles.end(); ++it)
		{
			for(size_t vn = 0; vn < 3; ++vn)
			{
				size_t &vertIndex = it->vertexIndices[vn];

				// if we haven't touched the vertex yet, assign uv coordinates to it. if we have touched it,
				//  their uvs must match. otherwise, duplicate it and adjust the indices in the triangles
				if(!visited[vertIndex])
				{
					mUvCoords[vertIndex] = it->uvCoords[vn];
					visited[vertIndex] = true;

				}else if(mUvCoords[vertIndex] != it->uvCoords[vn])
				{
					// incompatible UVs. duplicate vertex and normal
					mVertices.push_back(mVertices[vertIndex]);
					mNormals.push_back(mNormals[vertIndex]);

					// if there is bone affection info, duplicate that too
					if(mGeometry.hasBoneInfo())
					{
						mBoneIndices.push_back(mBoneIndices[vertIndex]);
						mBoneWeights.push_back(mBoneWeights[vertIndex]);
					}

					// the only thing that's different are uv coords:
					mUvCoords.push_back(it->uvCoords[vn]);

					// lastly, make sure the triangle uses that new vertex
					vertIndex = mVertices.size() - 1;
				}
			}
		}
	}
}

