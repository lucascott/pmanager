# PROCESS MANAGER in C
Custom shell which capable of handle multiple instances of the process and interact with them thrugh simple commands:

| ------ | ------ |
| pnew | create a new process called <name> |
| pclose <name> | close the process with named <name> |
| plist | display the list of all the processes |
| pinfo <name> | display more info about the process <name> |
| pspawn <name> | clone the process named <name> |
| prmall <name> | close the process <name> and his whole tree of children processes |
| ptree | display all the processes with the hierarchical tree view |
| quit | close remaining open processes and quit |
