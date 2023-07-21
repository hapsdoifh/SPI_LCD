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
CC=mips-openwrt-linux-gcc
CCC=mips-openwrt-linux-g++
CXX=mips-openwrt-linux-g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU_MIPS_1907-Linux
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
	${OBJECTDIR}/lcd/lcd_author_name.o \
	${OBJECTDIR}/lcd/lcd_ili9341.o \
	${OBJECTDIR}/lcd/lcd_ili9341_font.o \
	${OBJECTDIR}/lcd/lcd_ili9341_font_brandname_eng.o \
	${OBJECTDIR}/lcd/lcd_ili9341_image_logo.o \
	${OBJECTDIR}/lcd/lcd_omcj4x8c_16.o \
	${OBJECTDIR}/lcd/lcd_omcj4x8c_font.o \
	${OBJECTDIR}/lcd/lcd_spi.o \
	${OBJECTDIR}/lcd/touch_ads7843.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spi_lcd

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spi_lcd: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/spi_lcd ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/lcd/lcd_author_name.o: lcd/lcd_author_name.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_author_name.o lcd/lcd_author_name.c

${OBJECTDIR}/lcd/lcd_ili9341.o: lcd/lcd_ili9341.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_ili9341.o lcd/lcd_ili9341.c

${OBJECTDIR}/lcd/lcd_ili9341_font.o: lcd/lcd_ili9341_font.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_ili9341_font.o lcd/lcd_ili9341_font.c

${OBJECTDIR}/lcd/lcd_ili9341_font_brandname_eng.o: lcd/lcd_ili9341_font_brandname_eng.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_ili9341_font_brandname_eng.o lcd/lcd_ili9341_font_brandname_eng.c

${OBJECTDIR}/lcd/lcd_ili9341_image_logo.o: lcd/lcd_ili9341_image_logo.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_ili9341_image_logo.o lcd/lcd_ili9341_image_logo.c

${OBJECTDIR}/lcd/lcd_omcj4x8c_16.o: lcd/lcd_omcj4x8c_16.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_omcj4x8c_16.o lcd/lcd_omcj4x8c_16.c

${OBJECTDIR}/lcd/lcd_omcj4x8c_font.o: lcd/lcd_omcj4x8c_font.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_omcj4x8c_font.o lcd/lcd_omcj4x8c_font.c

${OBJECTDIR}/lcd/lcd_spi.o: lcd/lcd_spi.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/lcd_spi.o lcd/lcd_spi.c

${OBJECTDIR}/lcd/touch_ads7843.o: lcd/touch_ads7843.c
	${MKDIR} -p ${OBJECTDIR}/lcd
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lcd/touch_ads7843.o lcd/touch_ads7843.c

${OBJECTDIR}/main.o: main.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
