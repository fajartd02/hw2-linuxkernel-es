cmd_/home/fajar/Documents/hw2/Module.symvers := sed 's/\.ko$$/\.o/' /home/fajar/Documents/hw2/modules.order | scripts/mod/modpost -m -a  -o /home/fajar/Documents/hw2/Module.symvers -e -i Module.symvers   -T -
