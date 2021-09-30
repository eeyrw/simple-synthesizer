import wave
import struct
import os
from math import log2, pow
import numpy as np
from sf2utils.sf2parse import Sf2File
from io import StringIO
from string import Template
import struct
import argparse
import pprint
from scipy import signal


def noteToFreq(note):
    a = 440  # frequency of A (coomon value is 440Hz)
    return (a / 32) * (2 ** ((note - 9) / 12))


def calcIncrement(maxAmp, sampleRate, targetFreq):
    return int((maxAmp/sampleRate)*targetFreq)


def getCStyleSampleDataString(sampleArray, colWidth, dataDescription=''):
    file_str = StringIO()
    newLineCounter = 0
    file_str.write(dataDescription+'\n')
    for sample in sampleArray:
        file_str.write("%6d," % sample)
        if newLineCounter > colWidth:
            newLineCounter = 0
            file_str.write("\n")
        else:
            newLineCounter += 1
    return file_str.getvalue()


def formatFileByParam(templateFile, outputFile, param):
    with open(templateFile, 'r') as tmplFile:
        tmplString = tmplFile.read()
        s = Template(tmplString)
        with open(outputFile, 'w') as outFile:
            outFile.write(s.safe_substitute(param))


if __name__ == "__main__":
    incrementDataString = getCStyleSampleDataString([calcIncrement(
        65536, 32000, noteToFreq(i)) for i in range(0, 128)], 8, dataDescription='// Increment:')
    print(incrementDataString)
