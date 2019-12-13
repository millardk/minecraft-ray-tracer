main: main.cpp
	g++ -std=c++17 -pthread -O3 main.cpp image.cpp linalg.cpp reader.cpp rt.cpp -o raytracer
