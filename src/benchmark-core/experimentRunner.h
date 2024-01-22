#pragma once

#include <random>
#include <iostream>
#include "json.hpp"
#include "Dataset.h"
#include "ComputedConfig.h"
#include "support-radius-estimation/SupportRadiusEstimation.h"
#include "utils/progressBar.h"

template<typename DescriptorMethod, typename DescriptorType>
std::vector<DescriptorType> computeReferenceDescriptors(const std::vector<VertexInDataset>& representativeSet, const nlohmann::json& config, const Dataset& dataset, uint64_t randomSeed, float supportRadius) {
    std::mt19937_64 randomEngine(randomSeed);
    std::vector<uint64_t> randomSeeds(representativeSet.size());
    for(uint32_t i = 0; i < representativeSet.size(); i++) {
        randomSeeds.at(i) = randomEngine();
    }

    std::vector<DescriptorType> representativeDescriptors(representativeSet.size());
    uint32_t completedCount = 0;
    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < representativeSet.size(); i++) {
        ShapeDescriptor::OrientedPoint descriptorOrigin;
        VertexInDataset vertex = representativeSet.at(i);
        const DatasetEntry& entry = dataset.at(vertex.meshID);
        ShapeDescriptor::cpu::Mesh mesh = Shapebench::readDatasetMesh(config, entry);
        descriptorOrigin.vertex = mesh.vertices[vertex.vertexIndex];
        descriptorOrigin.normal = mesh.normals[vertex.vertexIndex];
        representativeDescriptors.at(i) = Shapebench::computeSingleDescriptor<DescriptorMethod, DescriptorType>(mesh, descriptorOrigin, config, supportRadius, randomSeeds.at(i));
        ShapeDescriptor::free(mesh);

        #pragma omp atomic
        completedCount++;

        if(completedCount % 100 == 0 || completedCount == representativeSet.size()) {
            std::cout << "\r    ";
            drawProgressBar(completedCount, representativeSet.size());
            std::cout << " " << completedCount << "/" << representativeSet.size() << std::flush;
        }
    }
    return representativeDescriptors;
}

template<typename DescriptorMethod, typename DescriptorType>
void testMethod(const nlohmann::json& configuration, const std::filesystem::path configFileLocation, const Dataset& dataset, uint64_t randomSeed) {
    std::mt19937_64 engine(randomSeed);
    std::filesystem::path computedConfigFilePath = configFileLocation.parent_path() / std::string(configuration.at("computedConfigFile"));
    std::cout << "Main config file: " << configFileLocation.string() << std::endl;
    std::cout << "Computed values config file: " << computedConfigFilePath.string() << std::endl;
    ComputedConfig computedConfig(computedConfigFilePath);
    const std::string methodName = DescriptorMethod::getName();

    // Getting a support radius
    float supportRadius = 0;
    if(!computedConfig.containsKey(methodName, "supportRadius")) {
        std::cout << "No support radius has been computed yet for this method." << std::endl;
        std::cout << "Performing support radius estimation.." << std::endl;
        supportRadius = Shapebench::estimateSupportRadius<DescriptorMethod, DescriptorType>(configuration, dataset, engine());
        std::cout << "    Chosen support radius: " << supportRadius << std::endl;
        computedConfig.setFloatAndSave(methodName, "supportRadius", supportRadius);
    } else {
        supportRadius = computedConfig.getFloat(methodName, "supportRadius");
        std::cout << "Cached support radius was found for this method: " << supportRadius << std::endl;
    }

    // Computing reference descriptors and their distance to the representative set
    uint32_t representativeSetSize = configuration.at("experiments").at("sharedSettings").at("representativeSetSize");
    uint32_t sampleSetSize = configuration.at("experiments").at("sharedSettings").at("sampleSetSize");

    uint64_t representativeSetRandomSeed = engine();
    uint64_t sampleSetRandomSeed = engine();

    std::cout << "Computing reference descriptors.." << std::endl;
    std::vector<VertexInDataset> representativeSet = dataset.sampleVertices(representativeSetRandomSeed, representativeSetSize);
    std::vector<DescriptorType> referenceDescriptors = computeReferenceDescriptors<DescriptorMethod, DescriptorType>(representativeSet, configuration, dataset, representativeSetRandomSeed, supportRadius);


    std::vector<VertexInDataset> sampleVerticesSet = dataset.sampleVertices(sampleSetRandomSeed, sampleSetSize);





    // Running experiments

    uint64_t clutterExperimentRandomSeed = engine();
    runClutterExperiment<DescriptorMethod, DescriptorType>(configuration, computedConfig, dataset, clutterExperimentRandomSeed);

}