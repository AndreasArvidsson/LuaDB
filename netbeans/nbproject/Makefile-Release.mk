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
CND_CONF=Release
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
LDLIBSOPTIONS=../lib/bson.so ../lib/lua51.so ../lib/mongoc.so ../lib/snappy.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin: ../lib/bson.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin: ../lib/lua51.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin: ../lib/mongoc.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin: ../lib/snappy.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin ${OBJECTFILES} ${LDLIBSOPTIONS}

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
	$(COMPILE.c) -O2 -I../include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/LuaC.o ../src/LuaC.c

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
	${RM} -r ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mongoc.so ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bson.so ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lua51.so ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/snappy.so
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/luaMongo.bin

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
