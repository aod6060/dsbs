cc=g++
bin-dir=./bin
include-dir=-I./include
lib-dir=-L./lib
src-solution=./src/d_solution
src-builder=./src/d_builder
obj-solution=./obj/d_solution
obj-builder=./obj/d_builder
options=-std=c++17


all: d_solution d_builder


d_solution:
	$(cc) -c $(src-solution)/main.cpp -o $(obj-solution)/main.o $(include-dir) $(options)
	$(cc) $(obj-solution)/main.o -o $(bin-dir)/d_solution $(lib-dir) -ljsoncpp

d_builder:
	$(cc) -c $(src-builder)/main.cpp -o $(obj-builder)/main.o $(include-dir) $(options)
	$(cc) $(obj-builder)/main.o -o $(bin-dir)/d_builder $(lib-dir) -ljsoncpp