set(genkw_SOURCES opm/json/JsonObject.cpp
                  opm/eclipse/Parser/createDefaultKeywordList.cpp
                  opm/eclipse/Deck/Deck.cpp
                  opm/eclipse/Deck/DeckItem.cpp
                  opm/eclipse/Deck/DeckKeyword.cpp
                  opm/eclipse/Deck/DeckRecord.cpp
                  opm/eclipse/Deck/DeckOutput.cpp
                  opm/eclipse/Generator/KeywordGenerator.cpp
                  opm/eclipse/Generator/KeywordLoader.cpp
                  opm/eclipse/Parser/MessageContainer.cpp
                  opm/eclipse/Parser/ParseContext.cpp
                  opm/eclipse/Parser/ParserEnums.cpp
                  opm/eclipse/Parser/ParserItem.cpp
                  opm/eclipse/Parser/ParserKeyword.cpp
                  opm/eclipse/Parser/ParserRecord.cpp
                  opm/eclipse/RawDeck/RawKeyword.cpp
                  opm/eclipse/RawDeck/RawRecord.cpp
                  opm/eclipse/RawDeck/StarToken.cpp
                  opm/eclipse/Units/Dimension.cpp
                  opm/eclipse/Units/UnitSystem.cpp
                  opm/eclipse/Utility/Stringview.cpp
)
if(NOT cjson_FOUND)
  list(APPEND genkw_SOURCES external/cjson/cJSON.c)
endif()
add_executable(genkw ${genkw_SOURCES})

target_link_libraries(genkw ecl Boost::regex Boost::filesystem Boost::system)
target_include_directories(genkw PRIVATE opm/include)

# Generate keyword list
include(opm/eclipse/share/keywords/keyword_list.cmake)
string(REGEX REPLACE "([^;]+)" "${PROJECT_SOURCE_DIR}/opm/eclipse/share/keywords/\\1" keyword_files "${keywords}")
configure_file(opm/eclipse/keyword_list.argv.in keyword_list.argv)

# Generate keyword source
add_custom_command(
    OUTPUT  ${PROJECT_BINARY_DIR}/ParserKeywords.cpp ${PROJECT_BINARY_DIR}/inlinekw.cpp
    COMMAND genkw keyword_list.argv
                  ${PROJECT_BINARY_DIR}/ParserKeywords.cpp
                  ${PROJECT_BINARY_DIR}/include/
                  opm/parser/eclipse/Parser/ParserKeywords
                  ${PROJECT_BINARY_DIR}/inlinekw.cpp
    DEPENDS genkw ${keyword_files} opm/eclipse/share/keywords/keyword_list.cmake
)
