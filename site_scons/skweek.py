# -*- coding: utf-8 -*-


import smacky2skweek

import os.path

from SCons.Script import *


def skweek_action(target, source, env):
	tInputFile = open(source[0].get_path(), 'rt')
	tOutputFile = open(target[0].get_path(), 'wb')

	tConvert = smacky2skweek.Convert(tInputFile, tOutputFile, False)

	tInputFile.close()
	tOutputFile.close()

	return 0



def skweek_emitter(target, source, env):
	return target, source



def skweek_string(target, source, env):
	return 'Skweek %s' % target[0].get_path()



def ApplyToEnv(env):
	#----------------------------------------------------------------------------
	#
	# Add the Skweek builder.
	#
	skweek_act = SCons.Action.Action(skweek_action, skweek_string)
	skweek_bld = Builder(action=skweek_act, emitter=skweek_emitter, suffix='.xml')
	env['BUILDERS']['Skweek'] = skweek_bld

