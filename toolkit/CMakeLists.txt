#Diretriz que aponta a versao minima do CMake a ser utilizada, assim como as Polices
cmake_minimum_required(VERSION 2.8.11)

#Cria o projeto pai
project(Ethanon_Engine)

#Indica o diretorio onde procurar as proximas informacoes p/ o CMake
add_subdirectory(Source)