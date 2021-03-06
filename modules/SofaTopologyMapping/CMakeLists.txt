cmake_minimum_required(VERSION 3.12)
project(SofaTopologyMapping)

set(HEADER_FILES
    CenterPointTopologicalMapping.h
    Edge2QuadTopologicalMapping.h
    Hexa2QuadTopologicalMapping.h
    Hexa2TetraTopologicalMapping.h
    IdentityTopologicalMapping.h
    Mesh2PointMechanicalMapping.h
    Mesh2PointMechanicalMapping.inl
    Mesh2PointTopologicalMapping.h
    Quad2TriangleTopologicalMapping.h
    SimpleTesselatedHexaTopologicalMapping.h
    SimpleTesselatedTetraMechanicalMapping.h
    SimpleTesselatedTetraMechanicalMapping.inl
    SimpleTesselatedTetraTopologicalMapping.h
    SubsetTopologicalMapping.h
    Tetra2TriangleTopologicalMapping.h
    Triangle2EdgeTopologicalMapping.h
    config.h
    initTopologyMapping.h
)

set(SOURCE_FILES
    CenterPointTopologicalMapping.cpp
    Edge2QuadTopologicalMapping.cpp
    Hexa2QuadTopologicalMapping.cpp
    Hexa2TetraTopologicalMapping.cpp
    IdentityTopologicalMapping.cpp
    Mesh2PointMechanicalMapping.cpp
    Mesh2PointTopologicalMapping.cpp
    Quad2TriangleTopologicalMapping.cpp
    SimpleTesselatedHexaTopologicalMapping.cpp
    SimpleTesselatedTetraMechanicalMapping.cpp
    SimpleTesselatedTetraTopologicalMapping.cpp
    SubsetTopologicalMapping.cpp
    Tetra2TriangleTopologicalMapping.cpp
    Triangle2EdgeTopologicalMapping.cpp
    initTopologyMapping.cpp
)


add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaGeneralTopology)

sofa_add_targets_to_package(
    PACKAGE_NAME SofaGeneral
    TARGETS ${PROJECT_NAME} AUTO_SET_TARGET_PROPERTIES
    )
