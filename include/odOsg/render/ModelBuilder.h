/*
 * GeometryBuilder.h
 *
 *  Created on: 26 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GEOMETRYUILDER_H_
#define INCLUDE_ODCORE_RENDER_GEOMETRYUILDER_H_

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <odCore/db/Asset.h>
#include <odCore/db/Model.h>

namespace odDb
{
	class AssetProvider;
}

namespace odOsg
{

    class Renderer;
    class Model;

	/**
	 * Builder for constructing Models from Riot engine model data. It splits models into multiple segments
	 * with only one texture per segment, which can then be turned into a Geometry. VBOs are shared among all
	 * segments and vertices are reused where possible.
	 *
	 * Automatically generates normals and duplicates vertices if neccessary.
	 *
	 * Construction is O(n*log(n)).
	 */
	class ModelBuilder
	{
	public:

        typedef std::vector<glm::vec3>::const_iterator VertexIterator;
        typedef std::vector<odDb::Model::Polygon>::const_iterator PolygonIterator;
        typedef std::vector<odDb::Model::BoneAffection>::const_iterator BoneAffectionIterator;

		ModelBuilder(Renderer *renderer, const std::string &geometryName, odDb::AssetProvider &assetProvider);

		inline void setBuildSmoothNormals(bool b) { mSmoothNormals = b; }
		inline void setCWPolygonFlag(bool b) { mCWPolys = b; }
		inline void setUseClampedTextures(bool b) { mUseClampedTextures = b; }

		void setVertexVector(VertexIterator begin, VertexIterator end); /// < @brief Copied the passed range to the internal vertex vector
		void setVertexVector(std::vector<glm::vec3> &&v); ///< @brief Moves the passed vertex vector to the internal one without copying.

		void setPolygonVector(PolygonIterator begin, PolygonIterator end);
		void setBoneAffectionVector(BoneAffectionIterator begin, BoneAffectionIterator end);

		od::RefPtr<Model> build();
		void buildAndAppend(Model *model);


	private:

		struct Triangle
		{
			size_t vertexIndices[3];
			glm::vec2 uvCoords[3];
			odDb::AssetRef texture;

			void flip()
			{
			    // swapping verts 0 and 2 reverses winding order, thus flipping the triangle
			    std::swap(vertexIndices[0], vertexIndices[2]);
			    std::swap(uvCoords[0], uvCoords[2]);
			}
		};

		void _buildNormals();
		void _makeIndicesUniqueAndGenerateUvs();
		void _disambiguateAndGenerateUvs();

		Renderer *mRenderer;

		std::string mGeometryName;
		odDb::AssetProvider &mAssetProvider;

		bool mSmoothNormals;
		bool mCWPolys;
		bool mUseClampedTextures;

		std::vector<Triangle> mTriangles;

		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec3> mNormals;
		std::vector<glm::vec2> mUvCoords;
		std::vector<glm::vec4> mBoneIndices;
		std::vector<glm::vec4> mBoneWeights;
		bool mHasBoneInfo;
	};

}

#endif /* INCLUDE_ODCORE_RENDER_GEOMETRYUILDER_H_ */
