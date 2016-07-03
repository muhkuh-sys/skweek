import argparse
import array
import sys
import wave
import math
import struct



class Play:
	__tWave = None


	def __init__(self, tInputFile, strOutputFile):
		# Read the complete input file.
		strSkweekData = tInputFile.read()
		# Convert the input data to an array.
		aucSkweekData = array.array('B', strSkweekData)

		# Create the output file.
		self.tWavFile = wave.open(strOutputFile, 'w')
		self.tWavFile.setnchannels(1)
		self.tWavFile.setsampwidth(2)
		self.tWavFile.setframerate(44100)

		self.__convert(aucSkweekData)

		self.tWavFile.close()



	def __writeNote(self, fFrequencyHz, fDurationInSeconds):
		for i in range(int(fDurationInSeconds * 44100)):
			value = int(32767.0*math.cos(fFrequencyHz*math.pi*float(i)/float(44100)))
			data = struct.pack('<h', value)
			self.tWavFile.writeframesraw( data )



	def __read_uint32(self, aucData, uiPosition):
		ulValue  = aucData[uiPosition]
		ulValue |= aucData[uiPosition+1]<<8
		ulValue |= aucData[uiPosition+2]<<16
		ulValue |= aucData[uiPosition+3]<<24
		return ulValue



	def __convert(self, aucSkweekData):
		# Start at offset 0.
		uiPosition = 0
		# Get the total length of the Skweek data.
		uiPositionMax = len(aucSkweekData)

		# Create an empty stack.
		atStack = []

		# Parse the Skweek data.
		while uiPosition<uiPositionMax:
			# Get the command.
			ucCommand = aucSkweekData[uiPosition]
			if ucCommand==ord('N'):
				# Play a note.
				# Parameters are the note in ticks and the duration in ticks.
				ulNoteTicks = self.__read_uint32(aucSkweekData, uiPosition+1)
				ulDurationTicks = self.__read_uint32(aucSkweekData, uiPosition+5)
				uiPosition += 9
				
				# Convert the note from ticks to Hz.
				fNoteFrequencyHz = 100000000.0 / ulNoteTicks
				
				# Convert the duration from ticks to seconds.
				# One tick is a 100MHz clock. The duration of one tick is 1/100000000 seconds.
				fDurationS = ulDurationTicks / 100000000.0
				
				print('Play note with %fHz for %dms.' % (fNoteFrequencyHz, fDurationS*1000.0))
				self.__writeNote(fNoteFrequencyHz, fDurationS)
			elif ucCommand==ord('P'):
				# Pause
				# Parameter is the duration in ticks.
				ulDurationTicks = self.__read_uint32(aucSkweekData, uiPosition+1)
				uiPosition += 5
				
				# Convert the duration from ticks to milliseconds.
				# One tick is a 100MHz clock. The duration of one tick is 1/100000000 seconds.
				fDurationS = ulDurationTicks / 100000000.0
				
				print('Pause for %dms.' % (fDurationS*1000.0))
				self.__writeNote(0, fDurationS)
			elif ucCommand==ord('B'):
				# Repeat block.
				# Parameter is the number of repeats.
				uiLoops = self.__read_uint32(aucSkweekData, uiPosition+1)
				uiPosition += 5
				# Push the new entry on the stack.
				atStack.append({'pos':uiPosition, 'cnt':uiLoops})
				print('Start block with loop=%d.' % uiLoops)
			elif ucCommand==ord('E'):
				# Close block.
				# Get the top block from the stack.
				tTop = atStack[-1]
				uiRepeat = tTop['cnt']
				if uiRepeat>1:
					uiRepeat -= 1
					atStack[-1]['cnt'] = uiRepeat
					uiPosition = tTop['pos']
					print('Block repeat loop=%d, back to position %d.' % (uiRepeat, uiPosition))
				else:
					# Remove the last stack element.
					atStack.pop()
					uiPosition += 1
					print('Block repeat end.')
			else:
				raise Exception('Unknown command 0x%02x.' % ucCommand)



def main():
	tParser = argparse.ArgumentParser(description='Convert a Smacky XML tune to skweek format.')
	tParser.add_argument('infile', nargs='?', type=argparse.FileType('r'), default=sys.stdin,
	                     help='read the input data from INPUT_FILENAME', metavar='INPUT_FILENAME')
	tParser.add_argument('outfile', nargs='?',
	                     help='write the output data to OUTPUT_FILENAME', metavar='OUTPUT_FILENAME')
	aOptions = tParser.parse_args()

	tConvert = Play(aOptions.infile, aOptions.outfile)

	if aOptions.infile != sys.stdin:
	        aOptions.infile.close()


if __name__ == '__main__':
	main()
