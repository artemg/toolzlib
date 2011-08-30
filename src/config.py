#!/usr/local/bin/python
import sys
import re

def myconf(inifile) :
    f = file(inifile)
    d = {}
    def get_quote_char(line):
        for char in line:
            if char in quotes:
                return char

    def getkey(line):
        #swallow everything up to the =
        return line[ : line.find('=') ].strip()

    def getval(line):
        #swallow everything after the =
        return line[ line.find('=') + 1 : ].strip()

    for line in f:
        line = line.strip()
        #skip comments and empty lines
        if line.startswith(';') or line.startswith('#') or line=='': continue
        #store sections as dicts
        k = getkey(line)
        v = getval(line)
        try:         
            d.update( {k:v} )
        except TypeError:
            print 'The ini file contains invalid characters'
    return d;

