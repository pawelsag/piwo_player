set(QT_INSTALL_DIR "C:/Qt")
set(COMILER_DIRECTORY "${QT_INSTALL_DIR}/Tools/mingw730_64/bin")
# variable necessary to locate qt cmake config file for find packge
set(CMAKE_PREFIX_PATH "${QT_INSTALL_DIR}/5.12.2/mingw73_64/bin")

set(CMAKE_C_COMPILER "${COMILER_DIRECTORY}/gcc.exe")
set(CMAKE_CXX_COMPILER "${COMILER_DIRECTORY}/g++.exe")
set(CMAKE_AR "${COMILER_DIRECTORY}/gcc-ar.exe")
