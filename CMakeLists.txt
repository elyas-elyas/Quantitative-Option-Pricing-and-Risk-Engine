cmake_minimum_required(VERSION 3.14)
project(OptionTradingDashboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- [NEW] Silence macOS OpenGL deprecation warnings ---
if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

# --- 1. Trouve OpenGL ---
find_package(OpenGL REQUIRED)

# --- [FIX MAC] Aide CMake à trouver OpenMP sur macOS via Homebrew ---
if(APPLE)
    if(EXISTS "/opt/homebrew/opt/libomp")
        # Chemin pour Apple Silicon (M1/M2/M3)
        set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    elseif(EXISTS "/usr/local/opt/libomp")
        # Chemin pour Mac Intel
        set(OpenMP_ROOT "/usr/local/opt/libomp")
    endif()
endif()

# --- 2. Détection OpenMP ---
find_package(OpenMP)

# --- 3. Téléchargement automatique des dépendances (FetchContent) ---
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.89.9
)
FetchContent_MakeAvailable(imgui)

# ImPlot
FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG        v0.16
)
FetchContent_MakeAvailable(implot)

# --- 4. Configuration des dossiers d'inclusion ---
include_directories(include)
include_directories(${imgui_SOURCE_DIR})
include_directories(${imgui_SOURCE_DIR}/backends)
include_directories(${implot_SOURCE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})

# --- 5. Création de l'exécutable ---
add_executable(TradingApp 
    src/gui_main.cpp 
    # Sources ImGui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    # Sources ImPlot
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

# --- 6. Liaison des librairies (Linking) ---

# Liaison OpenMP (Multithreading)
if(OpenMP_CXX_FOUND)
    target_link_libraries(TradingApp PRIVATE OpenMP::OpenMP_CXX)
    # Sur Mac, parfois les flags ne passent pas automatiquement
    if(APPLE)
        target_compile_options(TradingApp PRIVATE -Xpreprocessor -fopenmp)
    endif()
    message(STATUS "OpenMP found and linked!")
else()
    message(WARNING "OpenMP NOT found. Using single-thread mode. (Did you run 'brew install libomp'?)")
endif()

# Liaison Graphique (Spécifique macOS vs Autres)
if(APPLE)
    target_link_libraries(TradingApp PRIVATE glfw ${OPENGL_LIBRARIES} "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
else()
    target_link_libraries(TradingApp PRIVATE glfw ${OPENGL_LIBRARIES})
endif()