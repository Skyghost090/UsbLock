# UsbLock
## A service to automatic mount encrypted storage unit
### usblock [OPTIONS] [UNIT]

### RULES
* Please before eject your unit umount dm mountpoint
* Please configure a 1 device
* After execute param -r reboot system before create other unit
* Please type a unit based on /dev/disk/by-id/

### OPTIONS
* -l: for lock a unit
* -r: for remove device

## How to build
* Only run "make" on the shell
