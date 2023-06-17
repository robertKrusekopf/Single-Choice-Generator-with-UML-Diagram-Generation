gen:
	g++ -std=c++2a -c src/picture.cpp 
	g++ -std=c++2a -c src/parameter.cpp 
	g++ -std=c++2a -c src/randomNumberFunction.cpp 
	g++ -std=c++2a -c src/question.cpp 
	g++ -std=c++2a -c src/vectorHelperFunctions.cpp 
	g++ -std=c++2a -c src/converter.cpp 
	g++ -std=c++2a -c src/main.cpp 
	

clean: 
	rm -f obj/*
	rm -rf output/*