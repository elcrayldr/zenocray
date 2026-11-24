cmake_minimum_required(VERSION 3.22)
project(CRAY_LDR_URBAN_INSTRUMENTS VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
if(BUILD_TESTING)
  add_executable(cray_core_tests Tests/CoreTests.cpp)
  add_test(NAME cray_core_math COMMAND cray_core_tests)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(CRAY_LDR_FACTORY_PRESETS
  SOURCES
    FactoryLibrary/Presets/Init_Sub_Sine.json
    FactoryLibrary/Presets/Init_Urban_Saw.json
)

juce_add_plugin(CRAY_LDR_URBAN_INSTRUMENTS
  COMPANY_NAME "CRAY LDR"
  BUNDLE_ID "com.crayldr.urbaninstruments"
  PLUGIN_MANUFACTURER_CODE Cray
  PLUGIN_CODE UIn1
  FORMATS VST3 Standalone
  PRODUCT_NAME "CRAY LDR URBAN INSTRUMENTS"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD FALSE)

# Generate JuceHeader.h for the plugin target.
juce_generate_juce_header(CRAY_LDR_URBAN_INSTRUMENTS)

target_sources(CRAY_LDR_URBAN_INSTRUMENTS PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginProcessor.h
  Source/PluginEditor.cpp
  Source/PluginEditor.h
  Source/Audio/SynthVoice.cpp
  Source/Audio/SynthVoice.h
  Source/Audio/SamplerEngine.cpp
  Source/Audio/SamplerEngine.h
  Source/Audio/Effects/BasicEffects.cpp
  Source/Audio/Effects/BasicEffects.h
  Source/GUI/LookAndFeel.cpp
  Source/GUI/LookAndFeel.h)

target_compile_definitions(CRAY_LDR_URBAN_INSTRUMENTS PUBLIC
  JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(CRAY_LDR_URBAN_INSTRUMENTS PRIVATE
  CRAY_LDR_FACTORY_PRESETS
  juce::juce_audio_utils juce::juce_audio_formats juce::juce_dsp juce::juce_gui_extra
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags)
