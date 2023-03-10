set(CMAKE_MODULE_PATH    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules    ${CMAKE_MODULE_PATH})
find_package(Graphviz)
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Gui Graf Hist Gpad )
if(GRAPHVIZ_FOUND AND ROOT_FOUND)
  message(STATUS "HepMC3 ViewerExample: GrpahViz and ROOT found. GUI Browser example enabled. ${ROOT_LIBRARIES}  ${GRAPHVIZ_GRAPH_LIBRARY}  ${GRAPHVIZ_CGRAPH_LIBRARY} ${GRAPHVIZ_GVC_LIBRARY}")
  include_directories(${PROJECT_SOURCE_DIR}/rootIO/include ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}  ${ROOT_INCLUDE_DIRS} ${GRAPHVIZ_INCLUDE_DIRS} ./)
  ROOT_GENERATE_DICTIONARY(HepMC3ViewerFrameDict include/HepMC3ViewerFrame.h LINKDEF include/LinkDef.h OPTIONS -inlineInputHeader)
  ADD_EXECUTABLE(HepMC3Viewer.exe  src/main.cc  src/HepMC3ViewerFrame.cc HepMC3ViewerFrameDict.cxx  )
  set_target_properties(HepMC3Viewer.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})
  target_compile_definitions(HepMC3Viewer.exe PRIVATE ${GRAPHVIZ_DEFINES} )
  include_directories( include  )

  SET(linklibraties ${ROOT_LIBRARIES}  ${GRAPHVIZ_CGRAPH_LIBRARY}  ${GRAPHVIZ_GVC_LIBRARY}  ${HEPMC3_LIB} ${HEPMC3_ROOTIO_LIB} )
  if (GRAPHVIZ_GRAPH_LIBRARY)
    list(APPEND linklibraties  ${GRAPHVIZ_GRAPH_LIBRARY}  )
  endif()
  TARGET_LINK_LIBRARIES(HepMC3Viewer.exe   ${linklibraties})
  if (USE_INSTALLED_HEPMC3)
    install(TARGETS HepMC3Viewer.exe DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_BINDIR} FILES_MATCHING PATTERN "*.pcm" PATTERN "*config"  PATTERN "CMakeFiles" EXCLUDE )
    install(DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR}/ DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILES_MATCHING PATTERN "*env.sh*")
  endif()
elseif()
  message(STATUS "HepMC3 ViewerExample: GraphViz or ROOT package not found. GUI Browser example disabled.")
endif()
