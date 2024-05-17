#pragma once

#include <filesystem>
#include <vector>
#include <shapeDescriptor/shapeDescriptor.h>
#include "nlohmann/json.hpp"

namespace ShapeBench {
    struct VertexInDataset {
        uint32_t meshID = 0;
        uint32_t vertexIndex = 0;
    };

    template<typename DescriptorType>
    struct DescriptorOfVertexInDataset {
        uint32_t meshID = 0;
        uint32_t vertexIndex = 0;
        DescriptorType descriptor;
        ShapeDescriptor::OrientedPoint vertex = {{0, 0, 0}, {0, 0, 0}};
    };

    struct DatasetEntry {
        std::filesystem::path meshFile;
        uint32_t vertexCount = 0;
        uint32_t id = 0xFFFFFFFF;
        float computedObjectRadius = 0;
        ShapeDescriptor::cpu::float3 computedObjectCentre = {0, 0, 0};

        bool operator<(DatasetEntry& other);
    };

    class Dataset {
        std::vector<DatasetEntry> entries;
    public:
        std::vector<VertexInDataset> sampleVertices(uint64_t randomSeed, uint32_t count, uint32_t verticesPerObject) const;
        std::vector<VertexInDataset> resampleVerticesInSameObject(unsigned long randomSeed, const uint32_t verticesPerObject, const std::vector<ShapeBench::VertexInDataset>& originalSampleVertexList) const;
        const DatasetEntry& at(uint32_t meshID) const;
        ShapeDescriptor::cpu::Mesh loadMesh(const DatasetEntry& entry);
        void loadCache(const nlohmann::json& cacheJson);

        static ShapeBench::Dataset computeOrLoadCache(const nlohmann::json& configuration, const std::filesystem::path& cacheDirectory);
    };
}
