#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/511e4115/CliArgs.o \
	${OBJECTDIR}/_ext/511e4115/Date.o \
	${OBJECTDIR}/_ext/511e4115/File.o \
	${OBJECTDIR}/_ext/511e4115/Log.o \
	${OBJECTDIR}/_ext/511e4115/LuaBsonTypes.o \
	${OBJECTDIR}/_ext/511e4115/LuaC.o \
	${OBJECTDIR}/_ext/511e4115/LuaCollection.o \
	${OBJECTDIR}/_ext/511e4115/LuaCursor.o \
	${OBJECTDIR}/_ext/511e4115/LuaDB.o \
	${OBJECTDIR}/_ext/511e4115/LuaManager.o \
	${OBJECTDIR}/_ext/511e4115/LuaParserUtil.o \
	${OBJECTDIR}/_ext/511e4115/Main.o \
	${OBJECTDIR}/_ext/511e4115/MemoryManager.o \
	${OBJECTDIR}/_ext/511e4115/MongoCollection.o \
	${OBJECTDIR}/_ext/511e4115/MongoDB.o \
	${OBJECTDIR}/_ext/511e4115/MongoManager.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++17
CXXFLAGS=-std=c++17

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../lib/libbson-1.0.so.0 ../lib/libluajit-5.1.so.2 ../lib/libmongoc-1.0.so.0 ../lib/libsnappy.so.0

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/luaMongo.bin

${CND_DISTDIR}/luaMongo.bin: ../lib/libbson-1.0.so.0

${CND_DISTDIR}/luaMongo.bin: ../lib/libluajit-5.1.so.2

${CND_DISTDIR}/luaMongo.bin: ../lib/libmongoc-1.0.so.0

${CND_DISTDIR}/luaMongo.bin: ../lib/libsnappy.so.0

${CND_DISTDIR}/luaMongo.bin: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}
	${LINK.cc} -o ${CND_DISTDIR}/luaMongo.bin ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/511e4115/CliArgs.o: ../src/CliArgs.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/CliArgs.o ../src/CliArgs.cpp

${OBJECTDIR}/_ext/511e4115/Date.o: ../src/Date.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/Date.o ../src/Date.cpp

${OBJECTDIR}/_ext/511e4115/File.o: ../src/File.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/File.o ../src/File.cpp

${OBJECTDIR}/_ext/511e4115/Log.o: ../src/Log.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/Log.o ../src/Log.cpp

${OBJECTDIR}/_ext/511e4115/LuaBsonTypes.o: ../src/LuaBsonTypes.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaBsonTypes.o ../src/LuaBsonTypes.cpp

${OBJECTDIR}/_ext/511e4115/LuaC.o: ../src/LuaC.c
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.c) -g -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaC.o ../src/LuaC.c

${OBJECTDIR}/_ext/511e4115/LuaCollection.o: ../src/LuaCollection.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaCollection.o ../src/LuaCollection.cpp

${OBJECTDIR}/_ext/511e4115/LuaCursor.o: ../src/LuaCursor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaCursor.o ../src/LuaCursor.cpp

${OBJECTDIR}/_ext/511e4115/LuaDB.o: ../src/LuaDB.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaDB.o ../src/LuaDB.cpp

${OBJECTDIR}/_ext/511e4115/LuaManager.o: ../src/LuaManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaManager.o ../src/LuaManager.cpp

${OBJECTDIR}/_ext/511e4115/LuaParserUtil.o: ../src/LuaParserUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaParserUtil.o ../src/LuaParserUtil.cpp

${OBJECTDIR}/_ext/511e4115/Main.o: ../src/Main.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/Main.o ../src/Main.cpp

${OBJECTDIR}/_ext/511e4115/MemoryManager.o: ../src/MemoryManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/MemoryManager.o ../src/MemoryManager.cpp

${OBJECTDIR}/_ext/511e4115/MongoCollection.o: ../src/MongoCollection.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/MongoCollection.o ../src/MongoCollection.cpp

${OBJECTDIR}/_ext/511e4115/MongoDB.o: ../src/MongoDB.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/MongoDB.o ../src/MongoDB.cpp

${OBJECTDIR}/_ext/511e4115/MongoManager.o: ../src/MongoManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/MongoManager.o ../src/MongoManager.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r ${CND_DISTDIR}/libbson-1.0.so.0 ${CND_DISTDIR}/libluajit-5.1.so.2 ${CND_DISTDIR}/libsnappy.so.0 ${CND_DISTDIR}/libmongoc-1.0.so.0
	${RM} ${CND_DISTDIR}/luaMongo.bin

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
