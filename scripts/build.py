#!/usr/bin/env python3

from glob import glob
from pathlib import Path
import os
import itertools
import hashlib
import subprocess
import sys
from datetime import datetime

PathVar = os.environ.get('Path')
Paths = PathVar.split(';')
PATH = ""
for candidatePath in Paths:
    if "devkitARM" in candidatePath:
        PATH = candidatePath
        break
if PATH == "":
	print('DevKit does not exist in your Path variable.\nChecking default location.')
	PATH = 'C://devkitPro//devkitARM//bin'
	if os.path.isdir(PATH) == False:
		print("...\nDevkit not found.")
		sys.exit(1)
	else:
		print("Devkit found.")
		
PREFIX = '/arm-none-eabi-'
AS = (PATH + PREFIX + 'as')
CC = (PATH + PREFIX + 'gcc')
LD = (PATH + PREFIX + 'ld')
GR = (PATH + '/grit')
ARP = ('armips')
OBJCOPY = (PATH + PREFIX + 'objcopy')
SRC = './src'
BUILD = './build'
IMAGES = '\Images'
ASFLAGS = ['-mthumb', '-I', SRC]
LDFLAGS = ['BPRE.ld', '-T', 'linker.ld']
CFLAGS = ['-mthumb', '-mno-thumb-interwork', '-mcpu=arm7tdmi', '-mtune=arm7tdmi',
'-mno-long-calls', '-march=armv4t', '-Wall', '-Wextra','-Os', '-fira-loop-pressure', '-fipa-pta']

def run_command(cmd):
	try:
		subprocess.check_output(cmd)
	except subprocess.CalledProcessError as e:
		print(e.output.decode(), file = sys.stderr)
		sys.exit(1)

def make_output_file(filename):
	'''Return hash of filename to use as object filename'''
	m = hashlib.md5()
	m.update(filename.encode())
	newfilename = os.path.join(BUILD, m.hexdigest() + '.o')
	
	try:
		if os.path.getmtime(newfilename) > os.path.getmtime(filename): #If the object file was created after the file was last modified
			return [newfilename, False]
		else:
			return [newfilename, True]
	
	except FileNotFoundError: #If the object file doesn't exist, then obviously it needs to be made
		return [newfilename, True]

CharMap = "charmap.tbl"

def PokeByteTableMaker():
    dicty = {}
    with open(CharMap) as file:
            for line in file:
                if line.strip() != "/FF" and line.strip() != "":
                    if (line[2] == '=' and line[3] != ""):
                        try:
                            if line[3] == '\\':
                                dicty[line[3] + line[4]] = int(line.split('=')[0], 16)
                            else:
                                dicty[line[3]] = int(line.split('=')[0], 16)
                        except:
                            pass
            dicty[' '] = 0
        
    return dicty

def process_assembly(in_file):
	'''Assemble'''
	out_file_list = make_output_file(in_file)
	out_file = out_file_list[0]
	if out_file_list[1] == False:
		return out_file #No point in recompiling file
	
	print ('Assembling %s' % in_file)
	cmd = [AS] + ASFLAGS + ['-c', in_file, '-o', out_file]
	run_command(cmd)
	return out_file
	
def process_c(in_file):
	'''Compile C'''
	out_file_list = make_output_file(in_file)
	out_file = out_file_list[0]
	if out_file_list[1] == False:
		return out_file #No point in recompiling file
	
	print ('Compiling %s' % in_file)
	cmd = [CC] + CFLAGS + ['-c', in_file, '-o', out_file]
	run_command(cmd)
	
	return out_file

def process_string(filename):
    '''Build Strings'''
    charMap = PokeByteTableMaker()
    out_file = filename.split(".string")[0] + '.s'
    
    try:
        if os.path.getmtime(make_output_file(out_file)[0]) > os.path.getmtime(filename): #If the .o file was created after the image was last modified
            return make_output_file(out_file)[0]
    except FileNotFoundError:
        pass

    output = open(out_file, 'w')
    output.write(".thumb\n.text\n.align 2\n\n")
    with open(filename, 'r') as file:
        readingState = 0
        for line in file:
            if line == "\n" or line == "":
                continue
            
            if line[:2] == "//":
                continue
            
            line = line.rstrip("\n\r")
            if readingState == 0:
                if line[:6].upper() == "#ORG @" and line[6:] != "":
                    line = line.strip()
                    title = line[6:]
                    output.write(".global " + title + "\n" + title + ":\n")
                    readingState = 1
                else:
                    print('Warning! Error with line: "' + line + '" in file: "' + filename + '"')
                    
            elif readingState == 1:
                if line[:6].upper() == "#ORG @" and line[6:] != "":
                    line = line.strip()
                    title = line[6:]
                    output.write(".global " + title + "\n" + title + ":\n")
                else:
                    output.write(".byte ")
                    buffer = False
                    escapeChar = False
                    for char in line:        
                        if buffer == True:
                            if char == ']':
                                buffer = False
                                output.write(", ")
                            else:
                                output.write(char)

                        elif escapeChar == True:
                            escapeChar = False
                            try:
                                output.write(hex(charMap["\\" + char]) + ", ")

                            except KeyError:
                                print('Error parsing string: "' + line + '"')
                                break


                        else:
                            try:
                                output.write(hex(charMap[char]) + ", ")

                            except KeyError:
                                if (char == '['):
                                    output.write("0x")
                                    buffer = True
                                elif (char == '\\'):
                                    escapeChar = True
                                else:
                                    print('Error parsing string: "' + line + '"')
                                    break

                    output.write("0xFF\n\n")
                    readingState = 0
    
    output.close()
    
    out_file_list = make_output_file(out_file)
    new_out_file = out_file_list[0]
    if out_file_list[1] == False:
        os.remove(out_file)
        return new_out_file	#No point in recompiling file

    print ('Building Strings %s' % filename)
    cmd = [AS] + ASFLAGS + ['-c', out_file, '-o', new_out_file]
    run_command(cmd)
    os.remove(out_file)
    return new_out_file

def process_image(in_file):
	'''Compile Image'''
	if '.bmp' in in_file:
		out_file = in_file.split('.bmp')[0] + '.s'
	else:
		out_file = in_file.split('.png')[0] + '.s'
	
	namelist = in_file.split("\\") #Get path of grit flags
	namelist.pop(len(namelist) - 1)
	flags = ""
	for i in namelist:
		flags += i + "\\"
	flags += "gritflags.txt"
	
	try:
		with open(flags, 'r') as file:
			for line in file:
				cmd = [GR, in_file] + line.split() + ['-o', out_file]
				break #only needs the first line
	except FileNotFoundError:
		print("No gritflags.txt found in directory with " + in_file)
		return 0
	
	try:
		if os.path.getmtime(make_output_file(out_file)[0]) > os.path.getmtime(in_file): #If the .o file was created after the image was last modified
			return make_output_file(out_file)[0]
		else:
			run_command(cmd)
	
	except FileNotFoundError:
		run_command(cmd) #No .o file has been created

	out_file_list = make_output_file(out_file)
	new_out_file = out_file_list[0]
	if out_file_list[1] == False:
		os.remove(out_file)
		return new_out_file	#No point in recompiling file

	cmd = [AS] + ASFLAGS + ['-c', out_file, '-o', new_out_file]
	run_command(cmd)
	os.remove(out_file)
	return new_out_file

def link(objects):
	'''Link objects into one binary'''
	linked = 'build/linked.o'
	cmd = [LD] + LDFLAGS + ['-o', linked] + list(objects)
	run_command(cmd)
	return linked
	
def objcopy(binary):
	cmd = [OBJCOPY, '-O', 'binary', binary, 'build/output.bin']
	run_command(cmd)
	
def run_glob(globstr, fn):
	'''Glob recursively and run the processor function on each file in result'''
	if sys.version_info > (3, 4):
		files = glob(os.path.join(SRC, globstr), recursive = True)
		return map(fn, files)
	else:
		files = Path(SRC).glob(globstr)
		return map(fn, map(str, files))

def main():
	starttime = datetime.now()
	globs = {
			'**/*.s': process_assembly,
			'**/*.c': process_c,
			'**/*.string': process_string,
			'**/*.png': process_image,
			'**/*.bmp': process_image
	}
	
	print ('Compiling Images')
	
	# Create output directory
	try:
		os.makedirs(BUILD)
	except FileExistsError:
		pass
	
	# Gather source files and process them
	objects = itertools.starmap(run_glob, globs.items())
	
	# Link and extract raw binary
	linked = link(itertools.chain.from_iterable(objects))
	objcopy(linked)
	print('Built in ' + str(datetime.now() - starttime) + '.')
	
if __name__ == '__main__':
	main()
