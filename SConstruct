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

# Create a build environment for the Cortex-R based netX chips.
env_cortex7 = env_default.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])


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
env_netx4000_intram = env_netx4000_default.Clone()
env_netx4000_intram.Replace(LDFILE = 'src/netx4000/netx4000_cr7.ld')
src_netx4000_intram = env_netx4000_intram.SetBuildPath('targets/netx4000_intram', 'src', sources)
elf_netx4000_intram = env_netx4000_intram.Elf('targets/netx4000_intram/netx4000_skweek_intram.elf', src_netx4000_intram + platform_lib_netx4000)
txt_netx4000_intram = env_netx4000_intram.ObjDump('targets/netx4000_intram/netx4000_skweek_intram.txt', elf_netx4000_intram, OBJDUMP_FLAGS=['--disassemble', '--source', '--all-headers', '--wide'])
bin_netx4000_intram = env_netx4000_intram.ObjCopy('targets/skweek_netx4000.bin', elf_netx4000_intram)


#----------------------------------------------------------------------------
#
# Make a local demo installation.
#
# Copy all binary binaries.
Command('targets/testbench/netx/skweek_netx4000.bin',  bin_netx4000_intram,  Copy("$TARGET", "$SOURCE"))

# Copy all LUA scripts.
#Command('targets/testbench/lua/ramtest.lua',  'lua/ramtest.lua', Copy("$TARGET", "$SOURCE"))
