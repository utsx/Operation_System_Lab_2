import os
def main():
    struct_id = int(input("Choose struct [0 - for pci_dev, 1 - for inode]: "))
    pid = int(input("Enter pid: "))
    if struct_id != 1 and struct_id != 0:
        print("Wrong struct id")
        return
    os.system("echo '%d %d' > /proc/lab_driver" % (struct_id, pid))
    os.system("cat /proc/lab_driver 2>/dev/null")
if __name__ == '__main__':
    main()