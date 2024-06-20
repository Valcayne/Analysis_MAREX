# Simple HTCondor submit description file
# Everything with a leading # is a comment
 
executable   = /afs/cern.ch/work/v/valcayne/2024_Er_U_ProgramsC6D6/InputsFor_Er_Cu_U/Make2DHisto
arguments    = $(run)
#output       = outputs_condor/DTAnalyzer01_$(EXTRRun)_$(RAWRun)_det$(DetN)_$(Quant)_$(nEn)_$(nE1).out
output       = Condor/outputs/output_$(run).out
error        = Condor/outputs/error_$(run).err
#error        = outputs_condor/DTAnalyzer01_$(EXTRRun)_$(RAWRun)_det$(DetN)_$(Quant)_$(nEn)_$(nE1).err
log          = Condor/outputs/log_$(run).err
transfer_output_files   = ""
requirements = (OpSysAndVer =?= "AlmaLinux9")
#requirements = (OpSysAndVer =?= "CentOS7")
#+JobFlavour = "workday"
#+JobFlavour = "espresso"
+JobFlavour = "longlunch"



queue run from /afs/cern.ch/work/v/valcayne/2024_Er_U_ProgramsC6D6/InputsFor_Er_Cu_U/Condor/Inputs/ArgumentsForCondor2DHisto.txt
