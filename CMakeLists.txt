# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 3.4)

# Notre projet est étiqueté hello
project(HodEngine)

# Crée des variables avec les fichiers à compiler
set(SRCS
    main.c
    hello.c
    )
	
file(GLOB_RECURSE SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        )
	
# On indique que l'on veut un exécutable "hello" compilé à partir des fichiers décrits par les variables SRCS et HEADERS
add_executable(HodEngine ${SRCS} ${HEADERS})
