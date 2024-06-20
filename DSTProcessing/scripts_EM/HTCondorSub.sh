#!/bin/bash

# Author: Michael.Bacak@cern.ch
# Date:   2 October 2017

# default HTCondor queue
DEFAULT_QUEUE='longlunch' # FG-2021-04-05
DEFAULT_QUEUE='longlunch' # MB-2022-09-15 for TAC data
DEFAULT_QUEUE='microcentury' # MB-2024-05 VICTOR to go faster
#    from https://batchdocs.web.cern.ch/local/submit.html
#    espresso     = 20 minutes
#    microcentury = 1 hour
#    longlunch    = 2 hours
#    workday      = 8 hours
#    tomorrow     = 1 day
#    testmatch    = 3 days
#    nextweek     = 1 week


fileName=$1.CondorSub.sh
rm -rf $fileName

HTsubfileName=$1.script.sh
HToutputfileName=$1.outHT
HTerrputfileName=$1.errHT
HTlogputfileName=$1.logHT

echo 'executable            = ' $HTsubfileName >> $fileName
echo 'arguments             = ' >> $fileName
echo 'output                = ' $HToutputfileName >> $fileName
echo 'error                 = ' $HTerrputfileName >> $fileName
echo 'log                   = ' $HTlogputfileName >> $fileName
echo 'requirements          = (OpSysAndVer == "CentOS7") && (CERNEnvironment =!= "qa")' >> $fileName
#echo 'request_cpus = 1 ' >> $fileName
#echo 'request_disk = 20MB ' >> $fileName
#echo 'request_memory = 8000MB ' >> $fileName

# FG 
#echo "+JobFlavour           = \"$DEFAULT_QUEUE\" " >> $fileName  
echo '+JobFlavour           = "workday" ' >> $fileName
echo 'queue' >> $fileName


chmod +x $fileName
