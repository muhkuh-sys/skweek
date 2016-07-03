import argparse
import array
import sys
import xml.etree.ElementTree



class Convert:
	__tXml = None
	__tXmlRoot = None
	__strTuneName = None
	__fTimerFrequencyInHz = None

	# This dictionary converts a note to a frequency in Hz.
	__aNote2Frequency = {
		'C0':        16.35,
		'C#0':       17.32,
		'Db0':       17.32,
		'D0':        18.35,
		'D#0':       19.45,
		'Eb0':       19.45,
		'E0':        20.60,
		'F0':        21.83,
		'F#0':       23.12,
		'Gb0':       23.12,
		'G0':        24.50,
		'G#0':       25.96,
		'Ab0':       25.96,
		'A0':        27.50,
		'A#0':       29.14,
		'Bb0':       29.14,
		'B0':        30.87,
		'C1':        32.70,
		'C#1':       34.65,
		'Db1':       34.65,
		'D1':        36.71,
		'D#1':       38.89,
		'Eb1':       38.89,
		'E1':        41.20,
		'F1':        43.65,
		'F#1':       46.25,
		'Gb1':       46.25,
		'G1':        49.00,
		'G#1':       51.91,
		'Ab1':       51.91,
		'A1':        55.00,
		'A#1':       58.27,
		'Bb1':       58.27,
		'B1':        61.74,
		'C2':        65.41,
		'C#2':       69.30,
		'Db2':       69.30,
		'D2':        73.42,
		'D#2':       77.78,
		'Eb2':       77.78,
		'E2':        82.41,
		'F2':        87.31,
		'F#2':       92.50,
		'Gb2':       92.50,
		'G2':        98.00,
		'G#2':      103.83,
		'Ab2':      103.83,
		'A2':       110.00,
		'A#2':      116.54,
		'Bb2':      116.54,
		'B2':       123.47,
		'C3':       130.81,
		'C#3':      138.59,
		'Db3':      138.59,
		'D3':       146.83,
		'D#3':      155.56,
		'Eb3':      155.56,
		'E3':       164.81,
		'F3':       174.61,
		'F#3':      185.00,
		'Gb3':      185.00,
		'G3':       196.00,
		'G#3':      207.65,
		'Ab3':      207.65,
		'A3':       220.00,
		'A#3':      233.08,
		'Bb3':      233.08,
		'B3':       246.94,
		'C4':       261.63,
		'C#4':      277.18,
		'Db4':      277.18,
		'D4':       293.66,
		'D#4':      311.13,
		'Eb4':      311.13,
		'E4':       329.63,
		'F4':       349.23,
		'F#4':      369.99,
		'Gb4':      369.99,
		'G4':       392.00,
		'G#4':      415.30,
		'Ab4':      415.30,
		'A4':       440.00,
		'A#4':      466.16,
		'Bb4':      466.16,
		'B4':       493.88,
		'C5':       523.25,
		'C#5':      554.37,
		'Db5':      554.37,
		'D5':       587.33,
		'D#5':      622.25,
		'Eb5':      622.25,
		'E5':       659.25,
		'F5':       698.46,
		'F#5':      739.99,
		'Gb5':      739.99,
		'G5':       783.99,
		'G#5':      830.61,
		'Ab5':      830.61,
		'A5':       880.00,
		'A#5':      932.33,
		'Bb5':      932.33,
		'B5':       987.77,
		'C6':      1046.50,
		'C#6':     1108.73,
		'Db6':     1108.73,
		'D6':      1174.66,
		'D#6':     1244.51,
		'Eb6':     1244.51,
		'E6':      1318.51,
		'F6':      1396.91,
		'F#6':     1479.98,
		'Gb6':     1479.98,
		'G6':      1567.98,
		'G#6':     1661.22,
		'Ab6':     1661.22,
		'A6':      1760.00,
		'A#6':     1864.66,
		'Bb6':     1864.66,
		'B6':      1975.53,
		'C7':      2093.00,
		'C#7':     2217.46,
		'Db7':     2217.46,
		'D7':      2349.32,
		'D#7':     2489.02,
		'Eb7':     2489.02,
		'E7':      2637.02,
		'F7':      2793.83,
		'F#7':     2959.96,
		'Gb7':     2959.96,
		'G7':      3135.96,
		'G#7':     3322.44,
		'Ab7':     3322.44,
		'A7':      3520.00,
		'A#7':     3729.31,
		'Bb7':     3729.31,
		'B7':      3951.07,
		'C8':      4186.01,
		'C#8':     4434.92,
		'Db8':     4434.92,
		'D8':      4698.63,
		'D#8':     4978.03,
		'Eb8':     4978.03,
		'E8':      5274.04,
		'F8':      5587.65,
		'F#8':     5919.91,
		'Gb8':     5919.91,
		'G8':      6271.93,
		'G#8':     6644.88,
		'Ab8':     6644.88,
		'A8':      7040.00,
		'A#8':     7458.62,
		'Bb8':     7458.62,
		'B8':      7902.13
	}

	def __init__(self, tSmackyFile, tSkweekFile):
		# Set the default speed.
		uiDefaultSpeed = 250
		# Set the default tune name.
		self.__strTuneName = "unknown tune"
		# Set the player systems duration of one timer tick in seconds.
		# All players run with the netX timer unit which is operating at 100MHz.
		self.__fTimerFrequencyInHz = 100000000

		# Open the smacky file.
		self.__tXml = xml.etree.ElementTree.parse(tSmackyFile)
		self.__tXmlRoot = self.__tXml.getroot()

		# The root tag of a smacky file must be 'score'.
		if self.__tXmlRoot.tag!='score':
			raise Exception('The input file has no "score" root but "%s". Is this really a Smacky file?' % self.__tXmlRoot.tag)

		# Get the tune name from the root tag.
		if 'name' in self.__tXmlRoot.attrib:
			self.__strTuneName = self.__tXmlRoot.attrib['name']

		# Collect all notes in this array.
		print('Processing tune "%s"...' % self.__strTuneName)

		# Treat the root tag as a block from here on.
		tData = self.__parseBlock(self.__tXmlRoot, uiDefaultSpeed)

		# Write all data to the output file.
		tData.tofile(tSkweekFile)



	def __write_uint32(self, aucArray, tValue):
		# Convert the input to a long.
		ulValue = long(tValue)
		uc0 = ulValue&0xff
		uc1 = (ulValue>>8)&0xff
		uc2 = (ulValue>>16)&0xff
		uc3 = (ulValue>>24)&0xff
		aucArray.extend([uc0, uc1, uc2, uc3])



	def __parseBlock(self, tBlockTag, uiDefaultSpeed):
		# Get the tempo for the block.
		uiSpeed = uiDefaultSpeed
		if 'tempo' in tBlockTag.attrib:
			uiSpeed = long(tBlockTag.attrib['tempo'])

		# Get the repeat counter.
		# The default repeat count is '1'.
		uiRepeat = 1
		if 'loops' in tBlockTag.attrib:
			uiRepeat = long(tBlockTag.attrib['loops'])

		print('Starting new block with tempo %d and loops %d.' % (uiSpeed, uiRepeat))

		# Collect the block data in an array.
		tData = array.array('B')

		# Start a repeat loop if the block has a loop value larger than 1.
		if uiRepeat!=1:
			tData.append(ord('B'))
			self.__write_uint32(tData, uiRepeat)

		# Loop over all elements in the block.
		for tNode in list(tBlockTag):
			# Is this a note?
			strTag = tNode.tag
			if strTag=='note':
				# A note must have a "value" attribute.
				if not 'value' in tNode.attrib:
					raise Exception('The "note" node has no "value" attribute!')
				# Get the note.
				strNote = tNode.attrib['value']
				# Get all components.
				astrNote = strNote.split(',')
				# Combine the components without a separator.
				strNoteCompact = ''.join(astrNote)
				# Does this note exist?
				if not strNoteCompact in self.__aNote2Frequency:
					raise Exception('Unknown note "%s"!' % strNote)
				# Get the frequency in Hz.
				fNoteFrequency = self.__aNote2Frequency[strNoteCompact]

				# The 'duration' attribute is optional. The default duration is 1.
				# FIXME: Just guessing: the duration is in milliseconds?
				fDurationMs = 1.0
				if 'duration' in tNode.attrib:
					fDurationMs = float(tNode.attrib['duration'])
				# Multiply the duration with the tempo.
				fDurationMs *= uiSpeed

				# Convert the frequency of the note in timer ticks.
				fNoteTicks = self.__fTimerFrequencyInHz / fNoteFrequency
				# Round the ticks.
				ulNoteTicks = round(fNoteTicks)

				# Convert the duration in timer ticks.
				fDurationTicks = fDurationMs * self.__fTimerFrequencyInHz / 1000.0
				# Round the ticks.
				ulDurationTicks = round(fDurationTicks)

				print('Note %s (%fHz -> %d ticks) with duration %fms (->%d ticks)' % (strNoteCompact, fNoteFrequency, ulNoteTicks, fDurationMs, ulDurationTicks))

				tData.append(ord('N'))
				self.__write_uint32(tData, ulNoteTicks)
				self.__write_uint32(tData, ulDurationTicks)

			elif strTag=='pause':
				# The 'duration' attribute is optional. The default duration is 1.
				# FIXME: Just guessing: the duration is in milliseconds?
				fDurationMs = 1.0
				if 'duration' in tNode.attrib:
					fDurationMs = float(tNode.attrib['duration'])
				# Multiply the duration with the tempo.
				fDurationMs *= uiSpeed

				# Convert the duration in timer ticks.
				fDurationTicks = fDurationMs * self.__fTimerFrequencyInHz / 1000.0
				# Round the ticks.
				ulDurationTicks = round(fDurationTicks)

				print('Pause with duration %f (->%f ticks)' % (fDurationMs, fDurationTicks))

				tData.append(ord('P'))
				self.__write_uint32(tData, ulDurationTicks)

			elif strTag=='block':
				# Start a new block with the current tempo as default.
				tData.extend(self.__parseBlock(tNode, uiSpeed))

			else:
				raise Exception('Unknown tag "%s"!' % strTag)

		# End the repeat loop if the block has a loop value larger than 1.
		if uiRepeat!=1:
			tData.append(ord('E'))

		return tData


def main():
	tParser = argparse.ArgumentParser(description='Convert a Smacky XML tune to skweek format.')
	tParser.add_argument('infile', nargs='?', type=argparse.FileType('r'), default=sys.stdin,
	                     help='read the input data from INPUT_FILENAME', metavar='INPUT_FILENAME')
	tParser.add_argument('outfile', nargs='?', type=argparse.FileType('w'), default=sys.stdout,
	                     help='write the output data to OUTPUT_FILENAME', metavar='OUTPUT_FILENAME')
	aOptions = tParser.parse_args()

	tConvert = Convert(aOptions.infile, aOptions.outfile)
	if aOptions.infile != sys.stdin:
	        aOptions.infile.close()
	if aOptions.outfile != sys.stdout:
	        aOptions.outfile.close()


if __name__ == '__main__':
	main()
