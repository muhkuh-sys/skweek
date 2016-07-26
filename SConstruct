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
sources = """
	src/header.c
	src/init_netx_test.S
	src/main.c
"""

#----------------------------------------------------------------------------
#
# Build all files.
#

# Blinki programm for cortex CPUs
env_netx4000 = env_netx4000_default.Clone()
skw_netx4000 = convert_skweek_to_obj(env_netx4000, 'netx4000')
env_netx4000.Replace(LDFILE = 'src/netx4000/netx4000_cr7.ld')
src_netx4000 = env_netx4000.SetBuildPath('targets/netx4000_intram', 'src', sources)
elf_netx4000 = env_netx4000.Elf('targets/netx4000/netx4000_skweek.elf', src_netx4000 + platform_lib_netx4000 + skw_netx4000['simpsons_theme'])
txt_netx4000 = env_netx4000.ObjDump('targets/netx4000/netx4000_skweek.txt', elf_netx4000, OBJDUMP_FLAGS=['--disassemble', '--source', '--all-headers', '--wide'])
bin_netx4000 = env_netx4000.ObjCopy('targets/skweek_netx4000.bin', elf_netx4000)


#----------------------------------------------------------------------------
#
# Make a local demo installation.
#
# Copy all binary binaries.
Command('targets/testbench/netx/skweek_netx4000.bin',  bin_netx4000, Copy("$TARGET", "$SOURCE"))

# Copy all LUA scripts.
#Command('targets/testbench/lua/ramtest.lua',  'lua/ramtest.lua', Copy("$TARGET", "$SOURCE"))
