import os

struct_id = int(input("0 - for pci_dev, 1 - for inode "))
if struct_id != 1 and struct_id != 0:
    print("Wrong struct id")
    exit(1)
pid = int(input("PID "))
os.system("echo '%d %d' > /proc/lab_driver" % (struct_id, pid))
os.system("cat /proc/lab_driver 2>/dev/null")