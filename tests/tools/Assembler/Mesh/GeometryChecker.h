#pragma once
#include <tr1/memory>
#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Assembler/Mesh/FEMeshAdaptor.h>
#include <Mesh.h>

using ::testing::Contains;

namespace GeometryChecker {
    typedef FEMeshAdaptor::Ptr FEMeshPtr;
    typedef Mesh::Ptr MeshPtr;

    void check_nodes(FEMeshPtr m1, MeshPtr m2);
    void check_face_elements(FEMeshPtr m1, MeshPtr m2);
    void check_voxel_elements(FEMeshPtr m1, MeshPtr m2);
}