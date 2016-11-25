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


import os.path
import string

#----------------------------------------------------------------------------
#
# Set up the Muhkuh Build System.
#
SConscript('mbs/SConscript')
Import('atEnv')

import skweek
skweek.ApplyToEnv(atEnv.DEFAULT)

# Create a build environment for the Cortex-R7 and Cortex-A9 based netX chips.
env_cortexR7 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])
env_cortexR7.CreateCompilerEnv('NETX4000_RELAXED', ['arch=armv7', 'thumb'], ['arch=armv7-r', 'thumb'])

# Create a build environment for the Cortex-M4 based netX chips.
env_cortexM4 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])
env_cortexM4.CreateCompilerEnv('NETX90_MPW', ['arch=armv7', 'thumb'], ['arch=armv7e-m', 'thumb'])

# Build the platform libraries.
SConscript('platform/SConscript')

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
	tSkweek = atEnv.DEFAULT.Skweek(strSkweek, strSmackyPath)

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
# Get the source code version from the VCS.
#
atEnv.DEFAULT.Version('#targets/version/version.h', 'templates/version.h')


#----------------------------------------------------------------------------
#
# Build all files.
#
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

astrIncludePaths = ['src', '#platform/src', '#platform/src/lib', '#targets/version']


# Convert the skweek binaries to objects.
env_netx4000_tunes = atEnv.NETX4000_RELAXED.Clone()
skw_netx4000 = convert_skweek_to_obj(env_netx4000_tunes, 'netx4000_relaxed')

env_netx90_tunes = atEnv.NETX90_MPW.Clone()
skw_netx90 = convert_skweek_to_obj(env_netx90_tunes, 'netx90_mpw')

# Build a netX test version.
tEnv = atEnv.NETX4000_RELAXED.Clone()
tEnv.Replace(LDFILE = 'src/netx4000/netx4000_cr7.ld')
tEnv.Append(CPPPATH = astrIncludePaths)
tEnv.Append(CPPDEFINES = [['TUNE_NAME', 'simpsons_theme'], ['CFG_VERBOSE', 1]])
tSrc = tEnv.SetBuildPath('targets/netx4000_relaxed', 'src', sources_common + sources_test)
tElf = tEnv.Elf('targets/netx4000_relaxed/netx4000_skweek.elf', tSrc + tEnv['PLATFORM_LIBRARY'])
tTxt = tEnv.ObjDump('targets/netx4000_relaxed/netx4000_skweek.txt', tElf, OBJDUMP_FLAGS=['--disassemble', '--source', '--all-headers', '--wide'])
tBinNetx4000Testbench = tEnv.ObjCopy('targets/skweek_netx4000.bin', tElf)

tEnv = atEnv.NETX90_MPW.Clone()
tEnv.Replace(LDFILE = 'src/netx90/netx90.ld')
tEnv.Append(CPPPATH = astrIncludePaths)
tEnv.Append(CPPDEFINES = [['TUNE_NAME', 'simpsons_theme'], ['CFG_VERBOSE', 1]])
tSrc = tEnv.SetBuildPath('targets/netx90_mpw', 'src', sources_common + sources_test)
tElf = tEnv.Elf('targets/netx90_mpw/netx90_skweek.elf', tSrc + tEnv['PLATFORM_LIBRARY'])
tTxt = tEnv.ObjDump('targets/netx90_mpw/netx90_skweek.txt', tElf, OBJDUMP_FLAGS=['--disassemble', '--source', '--all-headers', '--wide'])
tBinNetx90Testbench = tEnv.ObjCopy('targets/skweek_netx90.bin', tElf)


# Build snippets for all different tunes.
global PROJECT_VERSION
for strBase, tSkweekObj in skw_netx4000.iteritems():
	tEnv = atEnv.NETX4000_RELAXED.Clone()
	tEnv.Replace(LDFILE = 'src/netx4000/netx4000_cr7.ld')
	tEnv.Append(CPPPATH = astrIncludePaths)
	tEnv.Append(CPPDEFINES = [['TUNE_NAME', strBase], ['CFG_VERBOSE', 0]])
	strWorkingPath = os.path.join('targets', 'standalone', 'netx4000_relaxed', strBase)
	tSrc = tEnv.SetBuildPath(strWorkingPath, 'src', sources_common + sources_standalone)
	tElf = tEnv.Elf(os.path.join(strWorkingPath, 'netx4000.elf'), tSrc + tEnv['PLATFORM_LIBRARY'] + tSkweekObj)
	tTxt = tEnv.ObjDump(os.path.join(strWorkingPath, 'netx4000.txt'), tElf, OBJDUMP_FLAGS=['--disassemble', '--source', '--all-headers', '--wide'])
	tBin = tEnv.ObjCopy(os.path.join(strWorkingPath, 'netx4000.bin'), tElf)
	tTmp = tEnv.GccSymbolTemplate(os.path.join(strWorkingPath, 'snippet.xml'), tElf, GCCSYMBOLTEMPLATE_TEMPLATE='templates/hboot_snippet.xml', GCCSYMBOLTEMPLATE_BINFILE=tBin[0])

	# Create the snippet from the parameters.
	aArtifactGroupReverse = ['org', 'muhkuh', 'hboot', 'sniplib']
	atSnippet = {
		'group': '.'.join(aArtifactGroupReverse),
		'artifact': 'skweek_%s' % strBase,
		'version': PROJECT_VERSION,
		'vcs_id': tEnv.Version_GetVcsId(),
		'vcs_url': 'https://github.com/muhkuh-sys/skweek',
		'license': 'GPL-2.0',
		'author_name': 'Muhkuh team',
		'author_url': 'https://github.com/muhkuh-sys',
		'description': 'Play the tune "%s" on a speaker connected to a GPIO pin.' % string.replace(strBase, '_', ' '),
		'categories': ['netx4000', 'music', 'speaker', 'skweek_%s' % strBase],
		'parameter': {
			'MMIO_INDEX':
			{
				'help': 'Index of the MMIO pin connected to the speaker.',
				'default': None
			},
			'PORTCONTROL_INDEX':
			{
				'help': 'Index of the portcontrol pin connected to the speaker.',
				'default': None
			},
			'PORTCONTROL_VALUE':
			{
				'help': 'Value for the portcontrol register.',
				'default': None
			}
		}
	}
	strArtifactPath = 'targets/snippets/%s/%s/%s' % ('/'.join(aArtifactGroupReverse), atSnippet['artifact'], PROJECT_VERSION)
	tSnippet = tEnv.HBootSnippet('%s/%s-%s.xml' % (strArtifactPath, atSnippet['artifact'], PROJECT_VERSION), tTmp, PARAMETER=atSnippet)

	# Create the POM file.
	tPOM = tEnv.POMTemplate('%s/%s-%s.pom' % (strArtifactPath, atSnippet['artifact'], PROJECT_VERSION), 'templates/pom.xml', POM_TEMPLATE_GROUP=atSnippet['group'], POM_TEMPLATE_ARTIFACT=atSnippet['artifact'], POM_TEMPLATE_VERSION=atSnippet['version'], POM_TEMPLATE_PACKAGING='xml')

	# Build the HBOOT definition for this image.
	tDef_nxhx4000 = tEnv.POMTemplate(os.path.join(strWorkingPath, 'standalone_nxhx4000.xml'), 'templates/standalone_nxhx4000.xml', POM_TEMPLATE_GROUP=atSnippet['group'], POM_TEMPLATE_ARTIFACT=atSnippet['artifact'], POM_TEMPLATE_VERSION=atSnippet['version'])
	tImg_nxhx4000 = tEnv.HBootImage(os.path.join(strWorkingPath, '%s-%s_nxhx4000.img' % (atSnippet['artifact'], PROJECT_VERSION)), tDef_nxhx4000)
	tEnv.Depends(tImg_nxhx4000, tSnippet)

#----------------------------------------------------------------------------
#
# Make a local demo installation.
#
# Copy all binary binaries.
Command('targets/testbench/netx/skweek_netx4000.bin', tBinNetx4000Testbench, Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/netx/skweek_netx90.bin',   tBinNetx90Testbench,   Copy("$TARGET", "$SOURCE"))

# Copy all tunes.
for strBase, tSkweek in atSkweek.iteritems():
	Command('targets/testbench/tunes/%s.bin' % strBase, tSkweek, Copy("$TARGET", "$SOURCE"))

# Copy all LUA scripts.
Command('targets/testbench/skweek.lua',  'lua/skweek.lua', Copy("$TARGET", "$SOURCE"))
