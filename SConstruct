# -*- coding: utf-8 -*-
#-------------------------------------------------------------------------#
#   Copyright (C) 2016 by Christoph Thelen                                #
#   doc_bacardi@users.sourceforge.net                                     #
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program; if not, write to the                         #
#   Free Software Foundation, Inc.,                                       #
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
#-------------------------------------------------------------------------#


#----------------------------------------------------------------------------
#
# Set up the Muhkuh Build System.
#
SConscript('mbs/SConscript')
Import('env_default')

import os.path

import skweek
skweek.ApplyToEnv(env_default)


# Create a build environment for the Cortex-R based netX chips.
env_cortex7 = env_default.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])


#----------------------------------------------------------------------------
#
# Convert all smacky tunes to skweek.
#
astrSmackyTunes = Split("""
	tunes/smacky-1.6.0/au_clair_de_la_lune.xml
	tunes/smacky-1.6.0/internationale.xml
	tunes/smacky-1.6.0/jeux_interdits.xml
	tunes/smacky-1.6.0/jingle_bells.xml
	tunes/smacky-1.6.0/les_etoiles_filantes.xml
	tunes/smacky-1.6.0/mario_bros_main_theme.xml
	tunes/smacky-1.6.0/merry_christmas.xml
	tunes/smacky-1.6.0/metal_gear_solid_main_theme_v2.xml
	tunes/smacky-1.6.0/metal_gear_solid_main_theme.xml
	tunes/smacky-1.6.0/mon_beau_sapin.xml
	tunes/smacky-1.6.0/mon_voisin_totoro.xml
	tunes/smacky-1.6.0/muse_plug_in_baby.xml
	tunes/smacky-1.6.0/panthere_rose.xml
	tunes/smacky-1.6.0/petit_papa_noel.xml
	tunes/smacky-1.6.0/simpsons_theme.xml
	tunes/smacky-1.6.0/star_wars_imperial_march.xml
	tunes/smacky-1.6.0/star_wars_luke_theme.xml
	tunes/smacky-1.6.0/star_wars_main_theme.xml
	tunes/smacky-1.6.0/tetris_theme.xml
	tunes/smacky-1.6.0/zelda_berceuse.xml
	tunes/smacky-1.6.0/zelda_chant_du_temps.xml
	tunes/smacky-1.6.0/zelda_main_theme.xml
""")
atSkweek = {}
for strSmackyPath in astrSmackyTunes:
	# Get the basename.
	strDummy, strBase = os.path.split(strSmackyPath)
	strBase, strDummy = os.path.splitext(strBase)

	# Construct the output path.
	strSkweek = os.path.join('targets', 'tunes', strBase + '.bin')

	# Convert the smacky file to skweek.
	tSkweek = env_default.Skweek(strSkweek, strSmackyPath)

	# Add the file to the list of tunes.
	atSkweek[strBase] = tSkweek


def convert_skweek_to_obj(tEnv, strId):
	atSkweekObj = {}

	# Loop over all skweek files.
	for strBase,tSkweek in atSkweek.iteritems():
		# Convert the skweek file to an object.
		tObj = tEnv.ObjImport(os.path.join('targets', strId, 'tunes', strBase + '.o'), tSkweek)
		atSkweekObj[strBase] = tObj

	return atSkweekObj


#----------------------------------------------------------------------------
#
# Create the compiler environments.
#
astrIncludePaths = ['src', '#platform/src', '#platform/src/lib', '#targets/version']


env_netx4000_default = env_cortex7.CreateCompilerEnv('4000', ['arch=armv7', 'thumb'], ['arch=armv7-r', 'thumb'])
env_netx4000_default.Append(CPPPATH = astrIncludePaths)
env_netx4000_default.Replace(BOOTBLOCK_CHIPTYPE = 4000)

Export('env_netx4000_default')


#----------------------------------------------------------------------------
#
# Get the source code version from the VCS.
#
env_default.Version('#targets/version/version.h', 'templates/version.h')


#----------------------------------------------------------------------------
#
# Build the platform libraries.
#
PLATFORM_LIB_CFG_BUILDS = [4000]
SConscript('platform/SConscript', exports='PLATFORM_LIB_CFG_BUILDS')
Import('platform_lib_netx4000')


#----------------------------------------------------------------------------
# This is the list of sources. The elements must be separated with whitespace
# (i.e. spaces, tabs, newlines). The amount of whitespace does not matter.
sources_common = """
	src/skweek.c
"""

sources_test = """
	src/header.c
	src/init_netx_test.S
	src/main_test.c
"""

sources_standalone = """
	src/init_standalone.S
	src/main_standalone.c
"""

#----------------------------------------------------------------------------
#
# Build all files.
#

# Convert the skweek binaries to objects.
env_netx4000_tunes = env_netx4000_default.Clone()
skw_netx4000 = convert_skweek_to_obj(env_netx4000_tunes, 'netx4000')

# Build a netX test version.
tEnv = env_netx4000_default.Clone()
tEnv.Replace(LDFILE = 'src/netx4000/netx4000_cr7.ld')
tEnv.Append(CPPDEFINES = [['TUNE_NAME', 'simpsons_theme'], ['CFG_VERBOSE', 1]])
tSrc = tEnv.SetBuildPath('targets/netx4000', 'src', sources_common + sources_test)
tElf = tEnv.Elf('targets/netx4000/netx4000_skweek.elf', tSrc + platform_lib_netx4000 + skw_netx4000['simpsons_theme'])
tTxt = tEnv.ObjDump('targets/netx4000/netx4000_skweek.txt', tElf, OBJDUMP_FLAGS=['--disassemble', '--source', '--all-headers', '--wide'])
tBinNetx4000Standalone = tEnv.ObjCopy('targets/skweek_netx4000.bin', tElf)


# Build standalone versions for all different tunes.
for strBase, tSkweekObj in skw_netx4000.iteritems():
	tEnv = env_netx4000_default.Clone()
	tEnv.Replace(LDFILE = 'src/netx4000/netx4000_cr7.ld')
	tEnv.Append(CPPDEFINES = [['TUNE_NAME', strBase], ['CFG_VERBOSE', 0]])
	strWorkingPath = os.path.join('targets', 'netx4000', 'standalone', strBase)
	tSrc = tEnv.SetBuildPath(strWorkingPath, 'src', sources_common + sources_standalone)
	tElf = tEnv.Elf(os.path.join(strWorkingPath, 'netx4000.elf'), tSrc + platform_lib_netx4000 + tSkweekObj)
	tTxt = tEnv.ObjDump(os.path.join(strWorkingPath, 'netx4000.txt'), tElf, OBJDUMP_FLAGS=['--disassemble', '--source', '--all-headers', '--wide'])
	tBB0 = tEnv.HBootImage(os.path.join('targets', 'netx4000_skweek_%s.img' % strBase), 'src/netx4000/CR7_to_INTRAM.xml', KNOWN_FILES=dict({'tElfCR7': tElf[0]}))

#----------------------------------------------------------------------------
#
# Make a local demo installation.
#
# Copy all binary binaries.
Command('targets/testbench/netx/skweek_netx4000.bin', tBinNetx4000Standalone, Copy("$TARGET", "$SOURCE"))

# Copy all LUA scripts.
Command('targets/testbench/lua/skweek.lua',  'lua/skweek.lua', Copy("$TARGET", "$SOURCE"))
