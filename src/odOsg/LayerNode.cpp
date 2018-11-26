/*
 * LayerNode.cpp
 *
 *  Created on: 26 Nov 2018
 *      Author: zal
 */

#include <odOsg/LayerNode.h>

#include <odCore/Layer.h>
#include <odCore/Level.h>

#include <odOsg/GeometryBuilder.h>
#include <odOsg/Renderer.h>
#include <odOsg/ShaderFactory.h>

namespace odOsg
{

    LayerNode::LayerNode(Renderer *renderer, od::Layer *layer, osg::Group *layerGroup)
    : mRenderer(renderer)
    , mLayerGroup(layerGroup)
    , mLayerTransform(new osg::PositionAttitudeTransform)
    , mGeometry(new Geometry)
    {
        _buildLayerGeometry(layer);

        mLayerGroup->addChild(mLayerTransform);
    }

    LayerNode::~LayerNode()
    {
        mLayerGroup->removeChild(mLayerTransform);
    }

    odRender::Geometry *LayerNode::getGeometry()
    {
        return mGeometry;
    }

    void LayerNode::_buildLayerGeometry(od::Layer *layer)
    {
        GeometryBuilder gb(mRenderer, *mGeometry, "layer " + layer->getName(), layer->getLevel());

        uint32_t width = layer->getWidth();
        uint32_t height = layer->getHeight();
        const std::vector<od::Layer::Vertex> &layerVertices = layer->getVertexVector();
        const std::vector<od::Layer::Cell> &layerCells = layer->getCellVector();

        std::vector<glm::vec3> &vertices = mGeometry->getVertexArray();

        vertices.reserve(layerVertices.size());
        for(size_t i = 0; i < layerVertices.size(); ++i)
        {
            size_t aXRel = i%(width+1);
            size_t aZRel = i/(width+1); // has to be an integer operation to floor it

            vertices.push_back(glm::vec3(aXRel, layerVertices[i].heightOffsetLu, aZRel));
        }

        std::vector<odDb::Model::Polygon> polygons; // TODO: move the Polygon struct somewhere where it belongs
        polygons.reserve(layer->getVisibleTriangleCount());
        for(size_t triIndex = 0; triIndex < width*height*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            od::Layer::Cell cell = layerCells[cellIndex];
            odDb::Model::Polygon poly;
            poly.vertexCount = 3;
            poly.texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;
            poly.doubleSided = (layer->getLayerType() == od::Layer::TYPE_BETWEEN);

            if(poly.texture == od::Layer::HoleTextureRef || poly.texture == od::Layer::InvisibleTextureRef)
            {
                continue;
            }

            int aZRel = cellIndex/width; // has to be an integer operation to floor it

            // calculate indices of corner vertices
            // z x> --a------b---
            // V      | cell | cell
            //        |  #n  | #n+1
            //      --c------d---
            size_t a = cellIndex + aZRel; // add row index since we want to skip top right vertex in every row passed so far
            size_t b = a + 1;
            size_t c = a + (width+1); // one row below a, one row contains width+1 vertices
            size_t d = c + 1;

            glm::vec2 uvA(cell.texCoords[3]/0xffff, cell.texCoords[7]/0xffff);
            glm::vec2 uvB(cell.texCoords[2]/0xffff, cell.texCoords[6]/0xffff);
            glm::vec2 uvC(cell.texCoords[0]/0xffff, cell.texCoords[4]/0xffff);
            glm::vec2 uvD(cell.texCoords[1]/0xffff, cell.texCoords[5]/0xffff);

            if(!(cell.isBackslashCell()))
            {
                if(isLeft)
                {
                    poly.vertexIndices[0] = c;
                    poly.vertexIndices[1] = b;
                    poly.vertexIndices[2] = a;
                    poly.uvCoords[0] = uvC;
                    poly.uvCoords[1] = uvB;
                    poly.uvCoords[2] = uvA;

                }else
                {
                    poly.vertexIndices[0] = c;
                    poly.vertexIndices[1] = d;
                    poly.vertexIndices[2] = b;
                    poly.uvCoords[0] = uvC;
                    poly.uvCoords[1] = uvD;
                    poly.uvCoords[2] = uvB;
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    poly.vertexIndices[0] = a;
                    poly.vertexIndices[1] = c;
                    poly.vertexIndices[2] = d;
                    poly.uvCoords[0] = uvA;
                    poly.uvCoords[1] = uvC;
                    poly.uvCoords[2] = uvD;

                }else
                {
                    poly.vertexIndices[0] = a;
                    poly.vertexIndices[1] = d;
                    poly.vertexIndices[2] = b;
                    poly.uvCoords[0] = uvA;
                    poly.uvCoords[1] = uvD;
                    poly.uvCoords[2] = uvB;
                }
            }

            if(layer->getLayerType() == od::Layer::TYPE_CEILING)
            {
                // swap two vertices, thus reversing the winding order
                std::swap(poly.vertexIndices[0], poly.vertexIndices[1]);
                std::swap(poly.uvCoords[0], poly.uvCoords[1]);
            }

            polygons.push_back(poly);
        }
        gb.setPolygonVector(polygons.begin(), polygons.end());

        gb.build();
        mLayerTransform->addChild(mGeometry->getOsgGeode());

        osg::ref_ptr<osg::Program> layerProg = mRenderer->getShaderFactory().getProgram("layer");
        mLayerTransform->getOrCreateStateSet()->setAttribute(layerProg);

        mLayerTransform->setPosition(osg::Vec3(layer->getOriginX(), layer->getWorldHeightLu(), layer->getOriginZ()));
    }

}

