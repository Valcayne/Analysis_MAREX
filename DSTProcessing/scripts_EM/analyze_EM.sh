#l#!/bin/bash
#
# Author: Vasilis.Vlachoudis@cern.ch
# Date:   1 July 2015
# Modified for HTCondor: Michael.Bacak@cern.ch
# Date:   2 October 2017
# Modified: Frank.Gunsing@cern.ch  adapted for CTA,
# Date:   5 April 2021

if [ $# -lt 2 ]; then
	echo "syntax: analyze.sh <run-directory> <psa-user-input> [<output-directory>] [<queue>]"
	echo
	echo "   <run-directory>:  running directory . for current"
	echo "   <psa-user-input>: location of psa UserInput.h"
	echo "   <output-directory>: location of output directory"
	echo "   <queue>:          LSF batch queue default 8nh"
	exit 0
fi

MAXJOBS=400
# flag for using CTA (1) or CASTOR (0)
useCTA=1

# default CTA instance
DEFAULT_CTA="root://eosctapublicpps.cern.ch" # FG-2021-04-05
DEFAULT_CTA="root://eosctapublicdisk.cern.ch" # MB-2021-07-13

out=1 # FG: not used?

verbose=1

path=$(dirname $0)
BINDIR=$(readlink -m $path)
#source ${BINDIR}/setup.sh
#BINDIR=/afs/cern.ch/user/e/emendoza/Test2021/EAR2/DSTProcessing/myscripts/
echo BINDIR is $BINDIR

# Change to running directory
if [ $1 != . ]; then
	cd $1
fi
# Absolute path of psa
psa=$(readlink -m $2)
if [ .$4 != . ]; then
	queue=$4
else
	queue=1nd # FG: not used ?
fi

# Change to run directory
output=$(pwd)

u="$USER"
EOSPATH=$output/
if [ "$u" == "ntofpro" ]; then
	EOSPATH=/eos/experiment/ntof/processing/official
fi

if [ .$3 != . ]; then
	EOSPATH=$3
fi


onqueue=0
skipped=0
completed=0
errors=0
submitted=0
alldone=1

first=1
# Loop over all *.files
for file in *.files; do
	fn=$(echo $file | cut -f1 -d.)
  RunNumber=`sed 's/.*run\(.*\)_.*/\1/' <<< "$fn"`
  
	# only the first time
	if [ $first -eq 1 ]; then
		first=0
		# Check if master root is there
		run=$(echo $fn | cut -f1 -d_)
		prefix=$(echo ${run:0:3})
		if [ "$prefix" == "run" ]; then
			if [ -f ${run}.root ]; then
				echo "ALL done: ${run}.root"
				break
			fi
		else
			break
		fi
	fi

	# if .done exists job is completed do nothing
	if [ -f ${fn}.done ]; then
		completed=$((completed + 1))

	# if .job exists: check status running, crashed or ended
	elif [ -f ${fn}.job ]; then
		jobid=$(tail -1 ${fn}.job)
		running=$(grep -c ${jobid} $4)
		if [ $running -eq 1 ]; then
			#if [ $finished -ne 1 ]; then
			# Running or pending on queue
			onqueue=$((onqueue + 1))
			alldone=0
		else
			# Check for possible errors if root is absent or lock is present
			if [ -f ${fn}.lock -o ! -f $EOSPATH/${RunNumber}/${fn}.root ]; then
				errors=$((errors + 1))
				echo "ERROR in processing ${fn}"
				       


				# Remove root as precaution
				#rm -f $EOSPATH/${fn}.root
				rm -f ${fn}.lock

				
				if [ "$u" == "ntofpro" ]; then
	        rm -rf $EOSPATH/${fn}.lock
        fi

				if [ -f ${fn}.job ]; then
					# Count the jobs
					njobs=$(wc -l ${fn}.job | cut -f1 -d\ )
					if [ $njobs -lt 3 ]; then
						# re-submit the job
						${BINDIR}/faster_EM.sh ${fn} $psa $EOSPATH/${RunNumber}
						${BINDIR}/HTCondorSub.sh ${fn}
						EXECUTE=$fn.CondorSub.sh
						out=$(condor_submit ${EXECUTE})
						echo re-submit the job


						rc=$?
						jobid=$(echo $out | rev | cut -c 2- | rev | rev | cut -d ' ' -f 1 | rev)

						# Check if rc=0 and jobid is an integer
						if [ $rc -eq 0 -a $jobid -eq $jobid ] 2>/dev/null; then
							njobs=$((njobs + 1))
							echo "Try #${njobs}: Submitted job: $fn $jobid"
							echo $jobid >>${fn}.job
							submitted=$((submitted + 1))
						else
							echo "ERROR submitting job $rc: $out"
							errors=$((errors + 1))
						fi
						alldone=0
					else
						mv ${fn}.job ${fn}.done
						completed=$((completed + 1))
					fi
				fi
			else
				# When no error rename .job to .done
				mv ${fn}.job ${fn}.done
				completed=$((completed + 1))
			fi
		fi

	# if .root is present then skip the processing
	elif [ -f $EOSPATH/${RunNumber}/${fn}.root ]; then

		skipped=$((skipped + 1))

	# Submit job if all files are staged
	else
		alldone=0

		n=$(wc -l <$4)
		n=$((n - 6))
		if [ $n -ge $MAXJOBS ]; then
			break
		fi

		# to check if all files are staged
		# convert to a list of files
		cat $file | sed s"/\s/\n/g" >${fn}.list

		# Check if any file is not staged yet
		# notstaged=`stager_qry -f ${fn}.list | grep -vc 'STAGED\|CANBEMIGR'`  # FG: for CASTOR only
		# All files are staged?
		#if [ $notstaged -eq 0 ]; then   # FG: for CASTOR only

		# FG:
		notstaged=0
		if [ $useCTA -eq 1 ]; then
			for thisf in $(cat ${fn}.list); do
				#allStaged=$(xrdfs $DEFAULT_CTA query prepare 0 $thisf | jq -r ' .responses[] | [.on_tape, .online] | all ') # MB
				allStaged=$(xrdfs $DEFAULT_CTA query prepare 0 $thisf | jq -r ' .responses[] | [.online] | all ') # MB, no need fore on_tape to recall
				if [ $allStaged == "false" ]; then
					notstaged=1
					break
				fi
			done
		else
			notstaged=$(stager_qry -f ${fn}.list | grep -vc 'STAGED\|CANBEMIGR')
		fi

		if [ $notstaged -eq 0 ]; then 
			echo "all files are staged..." # FG
			${BINDIR}/faster_EM.sh ${fn} $psa $EOSPATH/${RunNumber}
			${BINDIR}/HTCondorSub.sh ${fn}
			EXECUTE=$fn.CondorSub.sh
			out=$(condor_submit ${EXECUTE})
			echo firstsubmission

			rc=$?
			jobid=$(echo $out | rev | cut -c 2- | rev | rev | cut -d ' ' -f 1 | rev)

			# Check if rc=0 and jobid is an integer
			if [ $rc -eq 0 -a $jobid -eq $jobid ] 2>/dev/null; then
				echo "Submitted job: $fn $jobid"
				echo $jobid >>${fn}.job
				submitted=$((submitted + 1))

			else
				echo "ERROR submitting job $rc: $out"
				errors=$((errors + 1))
			fi
		else
			echo "staging files..." # FG
			if [ $useCTA -eq 1 ]; then
				echo "for CTA..., user = $USER" # FG
				for thisf in $(cat ${fn}.list); do
					xrdfs $DEFAULT_CTA prepare -s $thisf
				done
			else
				stager_get -f ${fn}.list
			fi
		fi
	fi
done

if [ $verbose -eq 1 ]; then
	echo "Job summary"
	echo "${onqueue}   Running or Pending"
	echo "${completed}   Runs completed"
	echo "${skipped}   Already existing (skipped)"
	echo "${errors}   Errors"
	echo "${submitted}   Submitted"
fi

echo 
echo
echo MERGE BEGINS
echo
echo

# Create the root file if all done
rc=1
if [ $alldone -eq 1 ]; then
	run=$(echo $fn | cut -f1 -d_)
	prefix=$(echo ${run:0:3})
	if [ "$prefix" == "run" ]; then
		MERGED_ROOT=$EOSPATH/${RunNumber}/merged_${run}.root

    if [ "$u" == "ntofpro" ]; then
	    MERGED_ROOT=$EOSPATH/${RunNumber}/merged_${run}.root
    else
#      MERGED_ROOT=$EOSPATH/merged_${run}.root
	    MERGED_ROOT=$EOSPATH/${RunNumber}/merged_${run}.root
    fi

		if [ ! -f ${MERGED_ROOT} ]; then # if merged file does not exist, check job status
			if [ ! -f merged.job ]; then    # if there is no merged.job the job needs to be submitted
				${BINDIR}/merge_EM.sh ${run} $EOSPATH/${RunNumber}
				${BINDIR}/HTCondorSub.sh ${run}.merge.sh
				EXECUTE=${run}.merge.sh.CondorSub.sh
				out=$(condor_submit ${EXECUTE})
				echo merge

				rc=$?
				jobid=$(echo $out | rev | cut -c 2- | rev | rev | cut -d ' ' -f 1 | rev)

				# Check if rc=0 and jobid is an integer
				if [ $rc -eq 0 -a $jobid -eq $jobid ] 2>/dev/null; then
					echo "Submitted job: $EXECUTE $jobid"
					echo $jobid >>merged.job
				fi
				rc=1
			else #check job status
				jobid=$(tail -1 merged.job)
				running=$(grep -c ${jobid} $4)
				if [ $running -ne 1 ]; then # if not running/pending
					${BINDIR}/merge_EM.sh ${run} $EOSPATH/${RunNumber}
					${BINDIR}/HTCondorSub.sh ${run}.merge.sh
					EXECUTE=${run}.merge.sh.CondorSub.sh
					out=$(condor_submit ${EXECUTE})
					echo merge not running

					rc=$?
					jobid=$(echo $out | rev | cut -c 2- | rev | rev | cut -d ' ' -f 1 | rev)

					# Check if rc=0 and jobid is an integer
					if [ $rc -eq 0 -a $jobid -eq $jobid ] 2>/dev/null; then
						echo "Submitted job: $EXECUTE $jobid"
						echo $jobid >>merged.job
					fi
					rc=1
				fi # //running
			fi
		else                          # there is a merged file, check if the merge.job has completed
			if [ ! -f merged.job ]; then # if there is no merged.job there is a problem
				echo "ERROR: merged file exist on EOS but no merged.job exists"
			else
				jobid=$(tail -1 merged.job)
				running=$(grep -c ${jobid} $4)

				if [ $running -ne 1 ]; then # if job has finished then finish the run
					rc=0
					#mv ${MERGED_ROOT} $EOSPATH/done/${run}.root
          if [ "$u" == "ntofpro" ]; then
	          mv ${MERGED_ROOT} $EOSPATH/done/${run}.root
          else
 #           mv ${MERGED_ROOT} $EOSPATH/../done/${run}.root
	          mv ${MERGED_ROOT} $EOSPATH/done/${run}.root
          fi
				fi
			fi
		fi
	else
		echo "ERROR: there is an issue in the names of the variables X"$prefix"X"
	fi # //prefix run
fi

# go back if needed
if [ $1 != . ]; then
	cd -
fi
exit $rc
