all:
	cd build; ninja;
run:
	cd bin; ./interpreter	
cmake:
	cmake . -B build -DCMAKE_C_COMPILER=clang;
