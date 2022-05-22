# Alper Canberk BALCI
# 2017400087
# 22.05.2020 00:17 Unfortunately debug lasted 3 hours

import os
import pwd
import sys
import datetime
import hashlib
import argparse

hash_dictionary = {}  # hash -> list of duplicates of this hash

size_dictionary = {}  # hash -> size
have_seen_before_list = []  # find the duplicates, then put them into this list.

parser = argparse.ArgumentParser()  # create a parser object
parser.add_argument("-d", action="store_true", default=False)  # d flag
parser.add_argument("-f", action="store_true", default=True)  # f flag default open (what if d comes)

parser.add_argument("-c", action="store_true", default=True)  # c flag default
parser.add_argument("-n", action="store_true", default=False)  # n flag
parser.add_argument("-cn", action="store_true", default=False)  # cn flag

parser.add_argument("-s", action="store_true", default=False)  # s flag

parser.add_argument("dirs", nargs="*", default=".", type=str)  # directories

args = parser.parse_args()  # parse the argumants into args

dirs_gonnabe_checked = args.dirs  # directories given in the arguments

if args.d:
    args.f = False

if args.s and not args.n:
    args.s = True

if args.n and not args.cn:
    args.c = False

if args.cn:
    args.c = False
    args.n = False

if args.cn and args.s:
    args.s = True

if args.n and (not args.cn) and (not args.c):
    args.s = False


fflag = args.f
dflag = args.d
cflag = args.c
nflag = args.n
cnflag = args.cn
sflag = args.s


def file_hash(filename,fullfilename):
    #print("filehasha girdim")
    if cflag:
        with open(fullfilename, "rb") as f:
            content = f.read()
        content_hash = hashlib.sha256(content).hexdigest()

        return content_hash
    elif nflag:
        namehash = hashlib.sha256(filename.encode()).hexdigest()

        return namehash
    elif cnflag:
        with open(fullfilename, "rb") as f:
            content = f.read()
        content_hash = hashlib.sha256(content).hexdigest()
        namehash = hashlib.sha256(filename.encode()).hexdigest()
        concatenate_of_these = namehash + content_hash
        hash_of_concatenate = hashlib.sha256(concatenate_of_these.encode()).hexdigest()

        return hash_of_concatenate  # hash value of file for cn is concatenation of name and content hashes.


def traversing_dir_i(dir_i_, full_path_of_dir_i):  # name of dir, full path of dir
    dircontents = os.listdir(full_path_of_dir_i)  # like ls,
    #  print("dircontents:", dircontents)
    sizes = []  # to sum up sizes and calculate size of the dir_i
    sizeofdir = 0
    hashed_everything = ""  # to determine hash of the dir_i

    hashesofentries = []
    if len(dircontents) > 0:

        for name in dircontents:  # list of contents: text.txt etc.

            currentitem = full_path_of_dir_i + "/" + name  # full path of name
            st = os.stat(currentitem)

            if os.path.isdir(currentitem):  # if name is a directory

                #  (name_hash, name_size) = traversing_dir_i(name, currentitem)


                # hash of contents of the dir, recursively maybe n maybe c maybe cn
                if dflag:  # if looking for duplicate directories
                    name_hash, name_size = traversing_dir_i(name, currentitem)
                    sizeofdir = sizeofdir + name_size
                    # size of directory recursively
                    hashesofentries.append(name_hash)
                    # add the hash of dir entry of name to the list. this list will be sorted

                    if name_hash in hash_dictionary:
                        if not (currentitem in hash_dictionary[name_hash]): # if seen before, there is a duplicate.
                            hash_dictionary[name_hash].append(currentitem)  # add the duplicate

                    else:
                        hash_dictionary[name_hash] = [currentitem]  # add it to dictionary hash -> full path
                        size_dictionary[name_hash] = name_size  # add size of name to dictionary
                
                elif fflag:
                    traversing_dir_i(name, currentitem)  # find hash of files in the dir, recursively

            else:  # if name is a file
                hash_current = file_hash(name, currentitem)  # hash of the file for the flags n, c, or cn
                hashesofentries.append(hash_current)  # collect files in a list. I will sort these to be hashed again.
                sizeofname = st.st_size
                sizes.append(sizeofname)
                sizeofdir = sizeofdir + sizeofname
                if fflag: # if looking for duplicate files
                    if hash_current in hash_dictionary:  # if this has been seen before, there is a duplicate.
                        if not (currentitem in hash_dictionary[hash_current]):
                            hash_dictionary[hash_current].append(currentitem)  # add the duplicate

                    else:  # this file has nor been seen yet, it is the first occurrence
                        hash_dictionary[hash_current] = [currentitem]  # add it to dictionary hash -> full path
                        size_dictionary[hash_current] = sizeofname  # add sizeofname to dictionary

        hashesofentries.sort()

        for sortedlistelement in hashesofentries:
            hashed_everything = hashed_everything + sortedlistelement  # concatenate all elements

        if nflag:
            hashed_everything = hashlib.sha256(dir_i_.encode()).hexdigest() + hashed_everything  # name + namelist

        elif cnflag:
            hashed_everything = hashlib.sha256(dir_i_.encode()).hexdigest() + hashed_everything  # name + contentlist

        hashed_everything = hashlib.sha256(hashed_everything.encode()).hexdigest()
        if dflag:
            return hashed_everything, sizeofdir  # returns tuple (hash,size)

    else:  # if it is an empty directory
        if dflag:

            if nflag:
                emptyhash = hashlib.sha256(dir_i_.encode()).hexdigest()
                emptysize = 0
                return emptyhash, emptysize

            elif cflag:
                emptyhash = hashlib.sha256("".encode()).hexdigest()
                emptysize = 0
                return emptyhash, emptysize

            elif cnflag:
                emptyhashcontent = hashlib.sha256("".encode()).hexdigest()
                emptyhashname = hashlib.sha256(dir_i_.encode()).hexdigest()
                hashable = emptyhashname + emptyhashcontent
                emptysize = 0
                return hashable, emptysize


def traversing_dirs(dirs_gonnabe):
    if len(dirs_gonnabe) > 0:
        for dir_i in dirs_gonnabe:
            full_path_of_dir_i = os.path.abspath(dir_i)
            if fflag:
                traversing_dir_i(dir_i, full_path_of_dir_i)
            if dflag:
                hashX, sizeY = traversing_dir_i(dir_i, full_path_of_dir_i)
                if hashX in hash_dictionary:  # look for
                    hash_dictionary[hashX].append(full_path_of_dir_i)

                else:
                    hash_dictionary[hashX] = [full_path_of_dir_i]
                    size_dictionary[hashX] = sizeY


def finding_duplicates():

    to_be_printed = []
    if not sflag:

        for an_hash, duplicate_list in hash_dictionary.items():

            if len(hash_dictionary[an_hash]) > 1:
                hash_dictionary[an_hash].sort()
                to_be_printed.append(hash_dictionary[an_hash])
        to_be_printed.sort()

        for listyahu in to_be_printed:
            for a_path in listyahu:
                print(a_path)

            print()

    else:

        for an_hash, duplicate_list in hash_dictionary.items():
            if len(hash_dictionary[an_hash]) > 1:
                size_of_an_hash = size_dictionary[an_hash]
                hash_dictionary[an_hash].sort()
                to_be_printed.append((size_of_an_hash, hash_dictionary[an_hash]))
            to_be_printed.sort()
            to_be_printed.reverse()


        for size_and_pathlist in to_be_printed:
            for path in size_and_pathlist[1]:
                print(path, "\t", size_and_pathlist[0])

            print()


traversing_dirs(dirs_gonnabe_checked)
finding_duplicates()

