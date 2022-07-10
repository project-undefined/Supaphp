

NAME				=	supaphp



INI_DIR				=	/etc/php/7.4/cli/conf.d



EXTENSION_DIR		=	$(shell php-config --extension-dir)


EXTENSION 			=	${NAME}.so
INI 				=	${NAME}.ini


COMPILER			=	g++
LINKER				=	g++


COMPILER_FLAGS		=	-Wall -c  -I${HOME}/vcpkg/installed/x64-linux/include -L${HOME}/vcpkg/installed/x64-linux/lib -std=c++17 -fpic -o
LINKER_FLAGS		=	-shared
LINKER_DEPENDENCIES	=   -L/usr/lib/ -I/usr/include/ -I${HOME}/vcpkg/installed/x64-linux/include -L${HOME}/vcpkg/installed/x64-linux/lib -lphpcpp -lcpr -lcurl -lssl -lcrypto -lz -lpthread


RM					=	rm -f
CP					=	cp -f
MKDIR				=	mkdir -p


SOURCES				=	$(wildcard *.cpp)
OBJECTS				=	$(SOURCES:%.cpp=%.o)


all:					${OBJECTS} ${EXTENSION}


${EXTENSION}:			${OBJECTS}
						${LINKER} ${LINKER_FLAGS} -o $@ ${OBJECTS} ${LINKER_DEPENDENCIES}

${OBJECTS}:
						${COMPILER} ${COMPILER_FLAGS} $@ ${@:%.o=%.cpp}

install:		
						${CP} ${EXTENSION} ${EXTENSION_DIR}
						${CP} ${INI} ${INI_DIR}
				
clean:
						${RM} ${EXTENSION} ${OBJECTS}

