#!/usr/bin/python

import sys
import csv
import math

TOTAL_BLOCKS = 0
TOTAL_INODES = 0
INODE_BLOCKS = 0
first_nonrsv_block = 0
first_nonrsv_inode = 0
blocks = []
inodes = []
rc = 0

def block_name(i):
	name = ""
	if (i == 12):
		name = "INDIRECT "
	elif (i == 13):
		name = "DOUBLE INDIRECT "
	elif (i == 14):
		name = "TRIBLE INDIRECT "
	return name

def check_block_num(block_num, b_name, offset, inode_N):

	b_n = block_name(b_name)

	if ( block_num < 0 or block_num > TOTAL_BLOCKS):
		print "INVALID {}BLOCK {} IN INODE {} AT OFFSET {}".format(b_n,block_num,inode_N,offset)
	elif (block_num > 0):
		#????????
		if (block_num < first_nonrsv_block):
			print "RESERVED {}BLOCK {} IN INODE {} AT OFFSET {}".format(b_n,block_num,inode_N,offset)
		
		# check duplicates.
		if (blocks[block_num][0] == -1 or blocks[block_num][0] == 0):
			if (blocks[block_num][0] == 0):
				print "ALLOCATED BLOCK {} ON FREELIST".format(block_num)
			blocks[block_num][0] = inode_N
			blocks[block_num][1] = b_name
			blocks[block_num][2] = offset
		elif (blocks[block_num][0] > 0):
			print "DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(block_name(blocks[block_num][1]),block_num,blocks[block_num][0],blocks[block_num][2])
			print "DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(b_n,block_num,blocks[block_num][0],offset)
			blocks[block_num][0] = -2
		elif (blocks[block_num][0] == -2):
			print "DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(b_n,block_num,blocks[block_num][0],offset)
		



def read_superblock(line):
	global TOTAL_BLOCKS
	global TOTAL_INODES
	global INODE_BLOCKS
	global blocks
	global inodes
	global first_nonrsv_inode
	TOTAL_BLOCKS = int(line[1])
	TOTAL_INODES = int(line[2])
	INODE_BLOCKS = int(math.ceil(float(TOTAL_INODES) / (int(line[3]) / int(line[4]))))
	first_nonrsv_inode = int(line[7])
	blocks = [[-1 for col in range(3)]for row in range(TOTAL_BLOCKS+1)] 
	inodes = [[-2 0 0] for _ in range(0, first_nonrsv_inode)] + [[-1 0 0] for _ in range(first_nonrsv_inode, TOTAL_INODES)]

def read_group(line):
	global first_nonrsv_block
	first_nonrsv_block = int(line[8]) + INODE_BLOCKS

def read_bfree(line):
	blocks[int(line[1])][0] = 0

def read_ifree(inode_N):
	inodes[inode_N - 1][0] = 0

def read_inode(line):
	#global inodes
	global rc
	inode_N = int(line[1])
	if (int(line[3]) != 0):
		if (inodes[inode_N - 1][0] == 0):
			rc = 2
			print "ALLOCATED INODE {} ON FREELIST".format(inode_N)	

		inodes[inode_N - 1][0] = int(line[6])
		
		#check for blocks
		for i in range(0,15):
			offset = i
			if (offset == 13):
				offset = 268
			elif (offset == 14):
				offset = 65804

			block_num = int(line[12+i])	
			check_block_num(block_num,i,offset, inode_N)

				


def read_dirent(line):
	global rc
	ref_inode = int(line[3])
	if (ref_inode > 0) and (ref_inode <= TOTAL_INODES):
		if (inodes[ref_inode - 1][0] > 0):
			inodes[ref_inode - 1][1] += 1
			if (inodes[ref_inode - 1][2] == 0):
				inodes[ref_inode - 1][2] = int(line[1])
		elif (inodes[ref_inode - 1][0] == 0):
			rc = 2
			print "DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(line[1], line[6], line[3])
	else:
		rc = 2
		print "DIRECTORY INODE {} NAME {} INVALID INODE {}".format(line[1], line[6], line[3])

	if (line[6] == "'.'"):
		if (line[1] != line[3]):
			rc = 2
			print "DIRECTORY INODE {} NAME '.' LINK TO INODE {} SHOULD BE {}".format(line[1], line[3], line[1])
	elif (line[6] == "'..'"):
		if (int(line[3]) != inodes[int(line[1]) - 1][2]):
			rc = 2
			print "DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(line[1], line[3], inodes[int(line[1]) - 1][2])


def read_indirect(line):
	block_num = int(line[5])
	offset = int(line[3])
	b_n = int(line[2])+11
	check_block_num(block_num, b_n, offset,int(line[1]))



def scan_blocks():
	for i in range(first_nonrsv_block, TOTAL_BLOCKS):
		if (blocks[i][0] == -1):
			print "UNREFERENCED BLOCK {}".format(i)

def scan_inodes():
	global rc
	for i in range(0, TOTAL_INODES):
		if (inodes[i][0] == -1):
			rc = 2
			print "UNALLOCATED INODE {} NOT ON FREELIST".format(i + 1)
		elif (inodes[i][0] > 0):
			if (inodes[i][0] != inodes[i][1]):
				rc = 2
				print "INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(i + 1, inodes[i][1], inodes[i][0])

def main():
	SB = []
	GP = []
	BF = []
	IF = []
	IN = []
	DE = []
	ID = []
	if (len(sys.argv) != 2):
		print >> sys.stderr, "Invalid argument count."
		sys.exit(1)
	try:
		f_read = open(sys.argv[1], "rb")
		reader = csv.reader(f_read)
	except:
		print >> sys.stderr, "Cannot read csv file."
		sys.exit(1)
	for line in reader:
		if (line[0] == "SUPERBLOCK"):
			SB.append(line)
		
		elif (line[0] == "GROUP"):
			GP.append(line)
		
		elif (line[0] == "BFREE"):
			BF.append(line)
			
		elif (line[0] == "IFREE"):
			IF.append(line)
			
		elif (line[0] == "INODE"):
			IN.append(line)

		elif (line[0] == "DIRENT"):
			DE.append(line)

		else:
			ID.append(line)

	for line in SB:
		read_superblock(line)
	for line in GP:
		read_group(line)
	for line in BF:
		read_bfree(line)
	for line in IF:
		read_ifree(int(line[1]))
	for line in IN:
		read_inode(line)
	DE.sort(key = lambda l: int(l[1]))
	for line in DE:
		read_dirent(line)

	scan_inodes()
	
	for line in ID:
		read_indirect(line)
	scan_blocks()
	print(TOTAL_BLOCKS, TOTAL_INODES, INODE_BLOCKS, len(blocks), len(inodes), first_nonrsv_block, first_nonrsv_inode)
	#
	sys.exit(rc)

main()