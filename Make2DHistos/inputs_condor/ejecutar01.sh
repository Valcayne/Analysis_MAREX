# Example 1
# Simple HTCondor submit description file
# Everything with a leading # is a comment


executable   = /afs/cern.ch/user/e/emendoza/Er-2024/Make2DHistos/Make2DHistos
arguments    = $(RunNumber) 
output       = outputs_condor/Make2DHisto_$(RunNumber).out
error        = outputs_condor/Make2DHisto_$(RunNumber).err
log          = outputs_condor/Make2DHisto_$(RunNumber).log
requirements = (OpSysAndVer =?= "AlmaLinux9") && (CERNEnvironment =!= "qa")

+JobFlavour = "longlunch"


queue RunNumber from inputs_condor/arguments_2024_Si_01.txt

