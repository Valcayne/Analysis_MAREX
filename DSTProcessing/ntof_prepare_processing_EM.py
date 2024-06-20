#!/usr/bin/env python3
####!/usr/bin/python
#
# prepares list of CTA files for processing
#
# Frank Gunsing <gunsing@cea.fr>
# last update: 2018-07-15 
# last update: 2021-04-05 
#
# Michael Bacak 09.07.2021
# links changed from CTA test to CTA production
# check for nfiles in a run. If <0 then don't create empty run directory
# recall files immediately and check status
# split lists for recalling

# Adrian Sanchez 18.03.2024
# Import changes in "find_and_sort_list()" from ntof_prepare_processing2.py 
# 

import os, sys
import math
import re
import argparse
from subprocess import Popen, PIPE
import ast

# -------------------------------------------------------------
# some global variables

# before 2014, run number is always below phase2limit
PHASE2LIMIT = 20000

# splitting run lists for preparing the files
runlistsplitting = 1000

# general pattern for regexp
# matches for example 
#     run105640.idx.finished
#     run13000_1.idx.finished 
#     run100000_1_s4.raw.finished
#
#     run105702.idx.finished   # no more stream information in idx after 2014
#     run105702_9_s1.raw.finished


REGEXP ='run(\d+)(_(\d+))?(_s(\d+))?\.(raw|idx)\.finished$'  

#MAXJOBS = 400     # maximum number of jobs
#MAXJOBS = 10     # maximum number of jobs
MAXJOBS = 4     # maximum number of jobs
#MAXJOBS = 1    # maximum number of jobs

XRD_CASTOR_PREFIX = 'root://castorpublic.cern.ch/'
DEFAULT_CTA       = "root://eosctapublicpps.cern.ch" 
DEFAULT_CTA       = "root://eosctapublicdisk.cern.ch" # MB

__version__ = "1.1.0"

# -------------------------------------------------------------
def printf(format, *args):     # use c-style printf, from O'Reilly's Python Cookbook
    sys.stdout.write(format % args)

# -------------------------------------------------------------
def find_and_sort_list(run, basepath, extension, locverbose = True):

    cmd = ""
    if (basepath.startswith("/castor")):   # for CASTOR use 'nsfind' 
        cmd = "nsfind " + basepath + " -name 'run" + str(run) + ".*\." + extension + "\.finished$" + "'"  # note that -name take regexp '.*', not shell wildcard '*'
    else:  # for CTA use 'eos find' 
        #cmd = "eos " + DEFAULT_CTA + " find -f -name 'run" + str(run) + "*." + extension + ".finished" + "'  " + basepath
        if "raw" in extension:
            cmd = "eos " + DEFAULT_CTA + " find -f -name 'run" + str(run)  + "'  " + basepath + "/" +str(run) + "/stream1"
        if "idx" in extension:
            cmd = "eos " + DEFAULT_CTA + " find -f -name 'run" + str(run)  + "'  " + basepath + "/" +str(run) + "/stream0"

    if (args.verbose): 
        printf("----- executing command = |%s| ...\n", cmd)

    # issue shell command and catch output in list
    p = Popen(cmd, stdout=PIPE, shell=True)
    output, err = p.communicate()    
#    this_list = output.split("\n")                 # python2
    this_list = output.decode('utf-8').split("\n")  # python3  

    # sort list on segment number if present
    sort_list = []
    sort_on_runnr = sort_on_segnr = sort_on_streamnr = False
    for fname in this_list:
        fname = fname.strip()    # strip possible leading and trailing whitespace
        runnr = segnr = streamnr = -1
        ftype = "not_ok"
        search_result = re.search(REGEXP, fname) 
        if (search_result):
            if search_result.group(1): runnr    = int(search_result.group(1))
            if search_result.group(3): segnr    = int(search_result.group(3))
            if search_result.group(5): streamnr = int(search_result.group(5))
            if search_result.group(6): ftype    = search_result.group(6)
            sort_list.append( (runnr, segnr, streamnr, ftype, fname) )      # order of fields will be used later
            #printf("   found: runnr=|%d|, segnr=|%d|, streamnr=|%d|, ftype=|%s| \n", runnr, segnr, streamnr, ftype)
        else: 
            #printf("   no match found. \n")
            continue
            
    sort_list.sort(key = lambda x:x[1]) # sort on segnr

    return [x[4] for x in sort_list]    # return list of fname only




# -------------------------------------------------------------
def prep_and_stage(streamlist, verbose=True):
  # STAGE FILES
  cmd = "xrdfs " + DEFAULT_CTA + " prepare -s -f "
  for el in streamlist:
    cmd += " " + el
  if (args.verbose):  # MB
    printf("----- executing command = |%s| ...\n", cmd) # MB

  p = Popen(cmd, stdout=PIPE, shell=True)
  output, err = p.communicate()

  # check status of files
  cmd = "xrdfs " + DEFAULT_CTA + " query prepare -f "
  for el in streamlist:
    cmd += " " + el
  if (args.verbose):  # MB
    printf("----- executing command = |%s| ...\n", cmd) # MB

  p = Popen(cmd, stdout=PIPE, shell=True)
  output, err = p.communicate()

  # this_streamlist = output.decode('utf-8').split("\n")  # python3   # MB
  this_streamlist = output.decode('utf-8')  # New structure of 'query prepare -f' command # AS (July 2023)

  searchstrings = ['online','exists','path_exists','on_tape','requested','has_reqid','req_time']
  bigString = this_streamlist
  for rIDX, rawfile in enumerate(streamlist):
    # posRAW = bigString.find(rawfile)
    posRAWpath = bigString.find(rawfile) # AS (July 2023)
    if posRAWpath > -1:                  # AS (July 2023)
      posRAW = bigString.rfind("{",0,posRAWpath)
    else:
      posRAW = -1
    if posRAW > -1:
      states = []
      for searchstring in searchstrings:
        # searchstringMod = searchstring + '":'
        searchstringMod = searchstring + '" : ' # AS (July 2023)
        posSTAGED = bigString.find(searchstringMod,posRAW)
        posSTAGED2 = bigString.find(",",posSTAGED)
        state = bigString[posSTAGED+len(searchstringMod):posSTAGED2]
        states.append(str(state))
        #print(posRAW,posSTAGED,posSTAGED2, state)
        if (searchstring == 'online'):
          print(rawfile, searchstring, state)
        #if (args.verbose and  searchstring == 'requested' and states[0] == "false"):
          #print(rawfile, searchstring, state)
            
    else:
      printf("ERROR: %s\n",rawfile)


# -------------------------------------------------------------
def create_lists(run, basepath, outputdir, verbose=True, prfx=False):
    #
    # todo: can be simplified by returning all fields of sort_list
    #
    
    # find idx files for run
    list_idx = find_and_sort_list(run, basepath, "idx")

    # find raw files for run
    list_raw = find_and_sort_list(run, basepath, "raw")
    
    # initialize lists of file names to be written in files
    per_stream_dict = {}        # empty dict (hash)
    per_stream_dict["0"] = [];  # create empty list for stream 0
    all_jobs_list = [];
   
    print(list_idx)
   
    # loop over idx files and find corresponding raw files 
    for f_idx in list_idx:
        search_result_idx = re.search(REGEXP, f_idx)  
        segnr_idx = -1
        if (search_result_idx):
            if search_result_idx.group(3): segnr_idx    = int(search_result_idx.group(3))
#        if ( segnr_idx<0 ): continue        # idx file must have valid segnr, only for pre-phaseIII data
        valid_line = "%s" % (f_idx)
        if not f_idx in per_stream_dict["0"]: 
            per_stream_dict["0"].append(f_idx)   # with dict, no multiple items of f_idx

        for f_raw in list_raw:
            search_result_raw = re.search(REGEXP, f_raw)  
            segnr_raw = streamnr_raw = -1
            if (search_result_raw):
                if search_result_raw.group(3): segnr_raw    = int(search_result_raw.group(3))
                if search_result_raw.group(5): streamnr_raw = int(search_result_raw.group(5))
#            if ( segnr_raw != segnr_idx ): continue  # segment number must be the same.  No segnr for idx >= phaseIII 
            if ( (segnr_raw<0) or (streamnr_raw<0) ): continue  # raw file must have valid segnr and streamnr
            valid_line +=  "\t%s" % (f_raw)
 
#            if not per_stream_dict.has_key(str(streamnr_raw)): per_stream_dict[str(streamnr_raw)] = [] # python2 # create dict for this stream if first time
            if not str(streamnr_raw) in per_stream_dict: per_stream_dict[str(streamnr_raw)] = []        # python3 # create dict for this stream if first time
            per_stream_dict[str(streamnr_raw)].append(f_raw) 
        all_jobs_list.append( valid_line )

    printf("test nfiles: %i\n",len(all_jobs_list)) # MB
    if (len(all_jobs_list)>0 and all_jobs_list[0].find(".raw")>0) : # MB
      #print(all_jobs_list) # MB

      
      # create output directory
      outputdir = outputdir.rstrip('/')
      output_rundir = outputdir + "/" + str(run)
      if not os.path.exists(output_rundir): os.mkdir(output_rundir)
     
      streamdict = {}
      # loop over streams and write files
      for stream in per_stream_dict.keys():
          fname = "run" + str(run) + ".stream" + stream
          streamlist = [] # MB
          with open(output_rundir + "/" + fname, 'w+') as f:
              for line in per_stream_dict[stream]:
                  if (basepath.startswith("/castor")):  
                      xline = XRD_CASTOR_PREFIX + line if prfx else line
                  else:
                      xline = DEFAULT_CTA + line if prfx else line
                  f.write(xline + "\n")
                  streamlist.append(xline) # MB
          streamdict.update({stream:streamlist})
          
          
          # prepare, stage and check status of files
          # split to avoid "OSError: [Errno 7] Argument list too long: '/bin/sh'"
          if ( len(streamlist)<1000 ):
            prep_and_stage(streamlist, verbose)
          else:
            sub_streamlists = [streamlist[x:x+runlistsplitting] for x in range(0, len(streamlist), runlistsplitting)]
            for sub_streamlist in sub_streamlists:
              prep_and_stage(sub_streamlist, verbose)
            
      printf("\n\n")
      
      
      # write several .files with MAXJOBS lines per file # MB
      # this will only work for single stream runs with a single idx file, not for pre 2016 data (several streams/idx)
      idxList = streamdict["0"]
      rawList = streamdict["1"]
      
      if(len(idxList) == 1 and len(rawList) >= 1):
        nfiles = int(math.ceil(1.0*len(rawList)/MAXJOBS))
        for i in range(0, nfiles):
          fname = "run" + str(run) + "_%04d"%(i+1) + ".files" 
          with open(output_rundir + "/" + fname, 'w+') as f:
            for j in range(0, MAXJOBS):
              n = i * MAXJOBS + j
              if ( n < len(rawList) ): 
                towrite = idxList[0] + " " + rawList[n] + "\n" 
                f.write( towrite )
      else:
        printf("ERROR: something is wrong with number of idx or raw files. 2016+ data/stream format?")

        
      printf("\n\n") # MB
      
      
      

#      # write several .files, with MAXJOBS lines per file
#      nfiles = int(math.ceil(1.0*len(all_jobs_list)/MAXJOBS))
#      if (verbose):
#          printf("-------- nfiles=%d, MAXJOBS=%d,   len=%d\n", nfiles, MAXJOBS, len(all_jobs_list))
#      for i in range(0, nfiles):
#          fname = "run" + str(run) + "_%04d"%(i+1) + ".files" 
#          if (verbose):
#              printf("-------- fname = |%s| \n", fname)
#          with open(output_rundir + "/" + fname, 'w+') as f:
#              for j in range(0, MAXJOBS):
#                  n = i * MAXJOBS + j
#                  #printf("-------- n=%d, i=%d, j=%d, MAXJOBS=%d \n", n, i, j, MAXJOBS)
#                  if ( n < len(all_jobs_list) ): 
#                      f.write( all_jobs_list[n] + "\n" )

#      printf("run %d, info read from '%s', files written in directory '%s'\n", run, basepath, output_rundir)


    printf("\n\n") # MB
    return

# -------------------------------------------------------------
if __name__ == '__main__':

    # requires python 3, because of obsolete has_key, and for Popen.communicate byte return 
    if sys.version_info >= (3,):
       pass
    else:
       print("needs python 3")
       exit()

    examples = '''examples:
       ntof_prepare_processing.py -d '/castor/cern.ch/ntof/2011/U238TAC' -r 13000
       ntof_prepare_processing.py -d '/castor/cern.ch/ntof/2011/U238TAC' -r 13000 -l 13002
       ntof_prepare_processing.py -d '/castor/cern.ch/ntof/2011/U238TAC' -o "./test" -r 13000 -l 13002
       ntof_prepare_processing.py -d '/castor/cern.ch/ntof/2016/ear1/U233_TAC' -o "./test" -r 105640 -v

       ntof_prepare_processing.py -d '/eos/ctapublicpps/archivetest/ntof/tmp_test_fg/2011_U236'  -o "./test" -r 12700 -l 12704 -v
       
    '''

    parser = argparse.ArgumentParser(prog='ntof_prepare_processing.py', description='script to prepare directories with run information for further processing.', epilog=examples, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-v', '--verbose',               action='store_true', dest='verbose', help='verbose output')
    parser.add_argument('-d', '--directory',  type=str,  action='store',      dest='basepath',  required=True, help='full path to castor or local experiment directory')
    parser.add_argument('-o', '--outdir',     type=str,  action='store',      dest='outdir',    default="./",  help='path to output directory')
    parser.add_argument('-r', '--run_first',  type=int,  action='store',      dest='run_first', required=True, help='first (or only) run to process')
    parser.add_argument('-l', '--run_last',   type=int,  action='store',      dest='run_last',  default=-1,    help='last run of range to process')
    parser.add_argument('-x', '--xrd_prefix',            action='store_true', dest='use_prfx',  default=False, help='prefix files with CASTOR instance (for xrdcp)')
    args = parser.parse_args()

    parser.add_argument('-nl',  action='store_true', dest='noline', default=False)

    # check if valid input
    last_run = args.run_last
    if (args.run_last < 0):
        last_run = args.run_first
    if not (os.path.exists(args.outdir) ):
        os.makedirs(args.outdir)

    # get run information
    for run in range(args.run_first, last_run + 1):
         create_lists(run, args.basepath, args.outdir, args.verbose, args.use_prfx)

    printf("Bye!\n\n");


