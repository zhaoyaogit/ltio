cmake_minimum_required(VERSION 2.8)

SET(COMPONENT_SOURCES
  ./log_metrics/metrics_item.cc
  ./log_metrics/metrics_stash.cc
  ./log_metrics/metrics_container.cc

  ./count_min_sketch/count_min_sketch.cc

  ./bloomfilter/bloom_filter.cc

  ./source_loader/source.cc
  ./source_loader/parser/parser.cc
  ./source_loader/loader/loader_factory.cc
  ./source_loader/loader/file_loader.cc
  ./source_loader/parser/column_parser.cc
  ./source_loader/parser/column_parser.cc

  #inverted_indexer
  ./inverted_indexer/indexer.cc
  ./inverted_indexer/expression.cc
  ./inverted_indexer/bitmap_merger.cc
  ./inverted_indexer/posting_field/field.cc
  ./inverted_indexer/posting_field/range_field.cc
  ./inverted_indexer/posting_field/general_str_field.cc
  ./inverted_indexer/posting_field/bitmap_posting_list.cc
  ./inverted_indexer/posting_field/posting_list_manager.cc
)

add_library(ltcomponent_objs OBJECT ${COMPONENT_SOURCES})
ltio_default_properties(ltcomponent_objs)

#add_library(ltcomponent STATIC ${COMPONENT_SOURCES})
#ltio_default_properties(ltcomponent)
#target_link_libraries(ltcomponent
#  PUBLIC ${CORO_LIBRARY}
#  )
#target_compile_definitions(ltcomponent
#  PUBLIC ${CORO_LIB_COMPILE_DEFINE}
#)
#target_include_directories(ltcomponent PUBLIC
#  ${LtIO_INCLUDE_DIRS}
#  ${PROJECT_SOURCE_DIR}
#  ${PROJECT_SOURCE_DIR}/thirdparty
#)
