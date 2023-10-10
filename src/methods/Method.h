#pragma once

#include "shapeDescriptor/gpu/types/array.h"
#include "shapeDescriptor/gpu/types/Mesh.h"
#include "shapeDescriptor/common/types/OrientedPoint.h"

namespace Shapebench {
    template<typename DescriptorType>
    struct Method {
        bool usesMeshInput();
        bool usesPointCloudInput();
        ShapeDescriptor::gpu::array<DescriptorType> computeDescriptors(const ShapeDescriptor::gpu::Mesh mesh,
                                                                       const ShapeDescriptor::gpu::array<ShapeDescriptor::OrientedPoint> device_descriptorOrigins,
                                                                       float supportRadius);
        ShapeDescriptor::gpu::array<DescriptorType> computeDescriptors(const ShapeDescriptor::gpu::PointCloud mesh,
                                                                       const ShapeDescriptor::gpu::array<ShapeDescriptor::OrientedPoint> device_descriptorOrigins,
                                                                       float supportRadius);
    };


}