cmake_minimum_required(VERSION 3.16)
project(FiberSim)

find_package(Geant4 REQUIRED)

add_executable(FiberSim
    src/main.cc
    src/DetectorConstruction.cc
    src/PrimaryGeneratorAction.cc
    src/ActionInitialization.cc
    src/RunAction.cc
    src/EventAction.cc
    src/SteppingAction.cc
    src/Materials.cc
)

target_include_directories(FiberSim PRIVATE include)

target_link_libraries(FiberSim PRIVATE ${Geant4_LIBRARIES})


foreach(_macro ${MACRO_FILES})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_macro}
        ${PROJECT_BINARY_DIR}/${_macro}
        COPYONLY
    )
endforeach()