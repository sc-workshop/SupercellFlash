
# install supercell-core
FetchContent_Declare(
    supercell-core
    GIT_REPOSITORY https://github.com/sc-workshop/SC-Core.git
    GIT_TAG 2.0
)

FetchContent_MakeAvailable(supercell-core)

# install supercell-compression
FetchContent_Declare(
    supercell-compression
    GIT_REPOSITORY https://github.com/sc-workshop/SupercellCompression.git
    GIT_TAG 2.0
)

FetchContent_MakeAvailable(supercell-compression)

# install supercell-texture
FetchContent_Declare(
    supercell-texture
    GIT_REPOSITORY https://github.com/sc-workshop/SupercellTexture
    GIT_TAG main
)

FetchContent_MakeAvailable(supercell-texture)