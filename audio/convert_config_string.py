import ctypes
import sys
import os
import array

global arrayelementsize


def Generate_CRC(inlist):
    crc = int(~0)&0xffffffff
    for i in range(len(inlist)-1):
        crc = crc ^ (int(inlist[i]&0xffffffff))
        for j in range(4*8):
            crc = ((int(crc >> 1)&0xffffffff) ^ int(0xEDB88320 & ((-(crc & 1))&0xffffffff)  ))&0xffffffff

    crc=(~crc)&0xffffffff


    if (crc>0x7fffffff):
        crc=-(0x7fffffff-(crc&0x7fffffff)+1)

    print("Calculated CRC= ", crc)
    print("Actual CRC=", inlist[len(inlist)-1])

    return(crc)


def convertto24bit(binarydata):

    if (binarydata[14]==2):
        print("16 bit detected")
        binarydata[14]=4
    elif (binarydata[14]==4):
        print("24 bit detected")
        return(1)
    else:
        print("unknown config")
        return(0)

    ilist = [16,18,20]

    for i in ilist:
        if (binarydata[i]==2):
            binarydata[i]=4
        elif (binarydata[14]==4):
            print("unknown config 4")
            return(0)
        else:
            print("unknown config")
            return(0)

    for i in range(21,len(binarydata)-1):
        if (binarydata[i]==17):
            if (binarydata[i+1]==0):
                binarydata[i]=53
            if (binarydata[i+1]==1):
                binarydata[i]=53
            if (binarydata[i+1]==2):
                binarydata[i]=53
            if (binarydata[i+1]==3):
                binarydata[i]=53
        if (binarydata[i]==16):
            if (binarydata[i+1]==5):
                binarydata[i]=52
            if (binarydata[i+1]==4):
                binarydata[i]=52

    crc=Generate_CRC(binarydata)
    binarydata[len(binarydata)-1]=crc
    return(1)


def detectbitpersample(binarydata):

    if (binarydata[14]==4):
        print("24 bit detected")
    elif (binarydata[14]==2):
        print("16 bit detected")
    else:
        print("unknown config")

def convertto16bit(binarydata):

    if (binarydata[14]==4):
        print("24 bit detected")
        binarydata[14]=2
    elif (binarydata[14]==2):
        print("16 bit detected")
        return(1)
    else:
        print("unknown config")
        return(0)

    ilist = [16,18,20]

    for i in ilist:
        if (binarydata[i]==4):
            binarydata[i]=2
        elif (binarydata[14]==2):
            print("unknown config 4")
            return(0)
        else:
            print("unknown config")
            return(0)

    for i in range(21,len(binarydata)-1):
        if (binarydata[i]==53):
            if (binarydata[i+1]==0):
                binarydata[i]=17
            if (binarydata[i+1]==1):
                binarydata[i]=17
            if (binarydata[i+1]==2):
                binarydata[i]=17
            if (binarydata[i+1]==3):
                binarydata[i]=17
        if (binarydata[i]==52):
            if (binarydata[i+1]==5):
                binarydata[i]=16
            if (binarydata[i+1]==4):
                binarydata[i]=16

    crc=Generate_CRC(binarydata)
    binarydata[len(binarydata)-1]=crc
    return(1)


def binary2header(fileinput,fileinput1,fileoutput,to24bit,fileoutput1,fileoutput2):
    binarydata=array.array(arrayelementsize)
    file=open(fileinput,'rb')
    if file==-1:
        print('error cannot open file for reading:',fileinput)
        return
    file.seek(0,2)
    arraysize=int(file.tell()/4)
    print('length of config string RX:', arraysize)
    file.seek(0,0)
    binarydata.fromfile(file,arraysize)
    file.close()

    binarydata1=array.array(arrayelementsize)
    file1=open(fileinput1,'rb')
    if file1==-1:
        print('error cannot open file for reading:',fileinput)
        return
    file1.seek(0,2)
    arraysize1=int(file1.tell()/4)
    print('length of config string TX:', arraysize1)
    file1.seek(0,0)
    binarydata1.fromfile(file1,arraysize1)
    file1.close()



    if (to24bit==0):
        detectbitpersample(binarydata)

    elif (to24bit==1):
        result=convertto16bit(binarydata)
        if (result==0):
            print("Problem converting to 16bit")
            exit(0)

    elif (to24bit==2):
        result=convertto24bit(binarydata)
        if (result==0):
            print("Problem converting to 24bit")
            exit(0)


    fileo=open(fileoutput,'wt')
    if (fileo==-1):
        print('error cannot open file for writing:',fileoutput)
        return

    fileo.write('\n\n\n#define CSPL_CONFIG_RX ')

    for i in range(len(binarydata)-1):
        fileo.write(str(binarydata[i])+', ')

    fileo.write(str(binarydata[len(binarydata)-1])+'\n\n\n\n')

    fileo.write('\n\n\n#define CSPL_CONFIG_TX ')

    for i in range(len(binarydata1)-1):
        fileo.write(str(binarydata1[i])+', ')

    fileo.write(str(binarydata1[len(binarydata1)-1])+'\n\n\n\n')

    fileo.close()
    print('wrote file:',fileoutput)

    if (fileoutput1!=None):
        fileo=open(fileoutput1,'wb')
        if (fileo==-1):
            print('error cannot open file for writing:',fileoutput1)
            return

        binarydata.tofile(fileo)
        fileo.close()
        print('wrote file:',fileoutput1)

    if (fileoutput2!=None):
        fileo=open(fileoutput2,'wb')
        if (fileo==-1):
            print('error cannot open file for writing:',fileoutput2)
            return

        binarydata1.tofile(fileo)
        fileo.close()
        print('wrote file:',fileoutput2)



def header2binary(fileinput,fileoutput,to24bit,fileoutput1,fileoutput2):
    RX=array.array(arrayelementsize)
    TX=array.array(arrayelementsize)
    file=open(fileinput,'rt')
    if file==-1:
        print('error cannot open file for reading:',fileinput)
        return

    while True:
        line=file.readline()
        sline=line.split()
        if (len(sline)>1):
            if (sline[1]=='CSPL_CONFIG_RX'):
                for r in range(2,len(sline)-1,1):
                    RX.append(int(sline[r][0:len(sline[r])-1]))
                RX.append(int(sline[len(sline)-1]))

            if (sline[1]=='CSPL_CONFIG_TX'):
                for r in range(2,len(sline)-1,1):
                    TX.append(int(sline[r][0:len(sline[r])-1]))
                TX.append(int(sline[len(sline)-1]))

        if (line == ''):
            break

    file.close()

    if (to24bit==0):
        detectbitpersample(RX)

    elif (to24bit==1):
        result=convertto16bit(RX)
        if (result==0):
            print("Problem converting to 16bit")
            exit(0)

    elif (to24bit==2):
        result=convertto24bit(RX)
        if (result==0):
            print("Problem converting to 24bit")
            exit(0)

    if (fileoutput!=None):
        fileo=open(fileoutput,'wt')
        if (fileo==-1):
            print('error cannot open file for writing:',fileoutput)
            return
        if (len(RX)>0):
            fileo.write('\n\n\n#define CSPL_CONFIG_RX ')

            for i in range(len(RX)-1):
                fileo.write(str(RX[i])+', ')

            fileo.write(str(RX[len(RX)-1])+'\n\n\n\n')

        if (len(TX)>0):
            fileo.write('\n\n\n#define CSPL_CONFIG_TX ')

            for i in range(len(TX)-1):
                fileo.write(str(TX[i])+', ')

            fileo.write(str(TX[len(TX)-1])+'\n\n\n\n')

        fileo.close()
        print('wrote file:',fileoutput)

    if (len(RX)>0):
        if (fileoutput1!=None):
            fileo=open(fileoutput1,'wb')
            if (fileo==-1):
                print('error cannot open file for writing:',fileoutput1)
                return
            RX.tofile(fileo)
            fileo.close()
            print('wrote file:',fileoutput1)

    if (len(TX)>0):
        if (fileoutput2!=None):
            fileo=open(fileoutput2,'wb')
            if (fileo==-1):
                print('error cannot open file for writing:',fileoutput2)
                return
            TX.tofile(fileo)
            fileo.close()
            print('wrote file:',fileoutput2)

    pass


def usage():
    print('Possible usages: ')
    print('1. Convert from binary to text header file:')
    print('\t\targs: bin <binary file RX> <binary file TX> <output text file>')

    print('2. Convert from binary from 16bit to 24bit then output to text header file:')
    print('\t\targs: bin16 <binary file RX> <binary file TX> <output text file>')

    print('3. Convert from binary from 16bit to 24bit then output to text header file and binary file')
    print('\t\targs: bin16 <binary file RX> <binary file TX> <output text file> <binary rx file> <binary tx file>')

    print('4. Convert from binary from 24bit to 16bit then output to text header file:')
    print('\t\targs: bin24 <binary file RX> <binary file TX> <output text file>')

    print('5. Convert from binary from 24bit to 16bit then output to text header file and binary file')
    print('\t\targs: bin24 <binary file RX> <binary file TX> <output text file> <binary rx file> <binary tx file>')

    print('6. Convert from text to binary file')
    print('\t\targs: txt <text file> <output binary file RX> <output binary file TX>')

    print('7. Convert from text 16bit to 24bit to text file')
    print('\t\targs: txt16 <text file> <text output file>')

    print('8. Convert from text 16bit to 24bit to text file and binary files')
    print('\t\targs: txt16 <text file> <text output file> <output binary file RX> <output binary file TX>')

    print('9. Convert from text 24bit to 16bit to text file')
    print('\t\targs: txt24 <text file> <text output file>')

    print('10. Convert from text 24bit to 16bit to text file and binary files')
    print('\t\targs: txt24 <text file> <text output file> <output binary file RX> <output binary file TX>')



    return



def main(argv):

    print('CSPL config string conversion used for android system')

    if (len(argv)<4):
        usage()
        return

    global arrayelementsize

    print("python version: ",sys.version_info)
    print("Current Os name: ",os.name)

    validsize=0
    for r in ['l','i']:
        arrayelementsize=r
        t=array.array(arrayelementsize)
        if (t.itemsize==4):
            validsize=1
            break

    if (validsize!=1):
        print("Expected element size donot match firmware requirement")


    if   (argv[1]=='bin'):
        binary2header(argv[2],argv[3],argv[4],0)
    elif (argv[1]=='txt'):
        header2binary(argv[2],None,0,argv[3],argv[4])
    elif   (argv[1]=='bin16'):
        if (len(argv)==5):
            binary2header(argv[2],argv[3],argv[4],1,None,None)
        elif (len(argv)==7):
            binary2header(argv[2],argv[3],argv[4],1,argv[5],argv[6])
    elif   (argv[1]=='bin24'):
        if (len(argv)==5):
            binary2header(argv[2],argv[3],argv[4],2,None,None)
        elif (len(argv)==7):
            binary2header(argv[2],argv[3],argv[4],2,argv[5],argv[6])
    elif   (argv[1]=='txt16'):
        if (len(argv)==4):
            header2binary(argv[2],argv[3],1,None,None)
        elif (len(argv)==6):
            header2binary(argv[2],argv[3],1,argv[4],argv[5])
    elif   (argv[1]=='txt24'):
        if (len(argv)==4):
            header2binary(argv[2],argv[3],2,None,None)
        elif (len(argv)==6):
            header2binary(argv[2],argv[3],2,argv[4],argv[5])

    else :
        usage()
        return


if __name__ == "__main__":
    main(sys.argv)
