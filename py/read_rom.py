#! /usr/bin/python
import serial
import struct
import time 
import config

def single_read(serial,addr):
    serial.write (("SRA%06X" %( addr&0xffffff )).encode())
    return int(serial.read(2).decode(),16)

ser = serial.Serial(config.SERIAL_DEVICE,
                    config.SERIAL_BAUDRATE,
                    timeout=config.SERIAL_TIMEOUT_SEC)

#
# header_map_mode
#
# 0x20 : LoROM             FF4
# 0x21 : HiROM             FF5
# 0x23 : SA-1 ROM          Super Mario RPG
# 0x30 : LoROM + FastROM   Ultima VII
# 0x31 : HiROM + FastROM   FF6
# 0x32 : ExLoROM           Star Ocean
# 0x35 : ExHiROM           Tales Of Phantasia



# get title
header_title = ""
for addr in range(21):
    header_title+=chr(single_read(ser,0x00ffc0+addr))
header_title=header_title.replace('\x00',"")
header_title=header_title.rstrip()
header_title=header_title.replace('\xa1',"。")
header_title=header_title.replace('\xa2',"「")
header_title=header_title.replace('\xa3',"」")
header_title=header_title.replace('\xa4',"、")
header_title=header_title.replace('\xa5',"・")
header_title=header_title.replace('\xa6',"ヲ")
header_title=header_title.replace('\xa7',"ァ")
header_title=header_title.replace('\xa8',"ィ")
header_title=header_title.replace('\xa9',"ゥ")
header_title=header_title.replace('\xaa',"ェ")
header_title=header_title.replace('\xab',"ォ")
header_title=header_title.replace('\xac',"ャ")
header_title=header_title.replace('\xad',"ュ")
header_title=header_title.replace('\xae',"ョ")
header_title=header_title.replace('\xaf',"ッ")
header_title=header_title.replace('\xb0',"ー")
header_title=header_title.replace('\xb1',"ア")
header_title=header_title.replace('\xb2',"イ")
header_title=header_title.replace('\xb3',"ウ")
header_title=header_title.replace('\xb4',"エ")
header_title=header_title.replace('\xb5',"オ")
header_title=header_title.replace('\xb6',"カ")
header_title=header_title.replace('\xb7',"キ")
header_title=header_title.replace('\xb8',"ク")
header_title=header_title.replace('\xb9',"ケ")
header_title=header_title.replace('\xba',"コ")
header_title=header_title.replace('\xbb',"サ")
header_title=header_title.replace('\xbc',"シ")
header_title=header_title.replace('\xbd',"ス")
header_title=header_title.replace('\xbe',"セ")
header_title=header_title.replace('\xbf',"ソ")
header_title=header_title.replace('\xc0',"タ")
header_title=header_title.replace('\xc1',"チ")
header_title=header_title.replace('\xc2',"ツ")
header_title=header_title.replace('\xc3',"テ")
header_title=header_title.replace('\xc4',"ト")
header_title=header_title.replace('\xc5',"ナ")
header_title=header_title.replace('\xc6',"ニ")
header_title=header_title.replace('\xc7',"ヌ")
header_title=header_title.replace('\xc8',"ネ")
header_title=header_title.replace('\xc9',"ノ")
header_title=header_title.replace('\xca',"ハ")
header_title=header_title.replace('\xcb',"ヒ")
header_title=header_title.replace('\xcc',"フ")
header_title=header_title.replace('\xcd',"ヘ")
header_title=header_title.replace('\xce',"ホ")
header_title=header_title.replace('\xcf',"マ")
header_title=header_title.replace('\xd0',"ミ")
header_title=header_title.replace('\xd1',"ム")
header_title=header_title.replace('\xd2',"メ")
header_title=header_title.replace('\xd3',"モ")
header_title=header_title.replace('\xd4',"ヤ")
header_title=header_title.replace('\xd5',"ユ")
header_title=header_title.replace('\xd6',"ヨ")
header_title=header_title.replace('\xd7',"ラ")
header_title=header_title.replace('\xd8',"リ")
header_title=header_title.replace('\xd9',"ル")
header_title=header_title.replace('\xda',"レ")
header_title=header_title.replace('\xdb',"ロ")
header_title=header_title.replace('\xdc',"ワ")
header_title=header_title.replace('\xdd',"ン")
header_title=header_title.replace('\xde',"゛")
header_title=header_title.replace('\xdf',"゜")
header_title=header_title.replace("ウ゛","ヴ")
header_title=header_title.replace("カ゛","ガ")
header_title=header_title.replace("キ゛","ギ")
header_title=header_title.replace("ク゛","グ")
header_title=header_title.replace("ケ゛","ゲ")
header_title=header_title.replace("コ゛","ゴ")
header_title=header_title.replace("サ゛","ザ")
header_title=header_title.replace("シ゛","ジ")
header_title=header_title.replace("ス゛","ズ")
header_title=header_title.replace("セ゛","ゼ")
header_title=header_title.replace("ソ゛","ゾ")
header_title=header_title.replace("タ゛","ダ")
header_title=header_title.replace("チ゛","ヂ")
header_title=header_title.replace("ツ゛","ヅ")
header_title=header_title.replace("テ゛","デ")
header_title=header_title.replace("ト゛","ド")
header_title=header_title.replace("ハ゛","バ")
header_title=header_title.replace("ヒ゛","ビ")
header_title=header_title.replace("フ゛","ブ")
header_title=header_title.replace("ヘ゛","ベ")
header_title=header_title.replace("ホ゛","ボ")
header_title=header_title.replace("ハ゜","パ")
header_title=header_title.replace("ヒ゜","ピ")
header_title=header_title.replace("フ゜","プ")
header_title=header_title.replace("ヘ゜","ペ")
header_title=header_title.replace("ホ゜","ポ")



print (header_title)
header_map_mode = single_read(ser,0x00ffd5)
print ("Map mode: 0x%x" % (header_map_mode))

# ROM type
header_rom_type = single_read(ser,0x00ffd6)
print ("ROM type: 0x%x" % (header_rom_type))

# ROM size
header_rom_size = single_read(ser,0x00ffd7)
print ("ROM size: 0x%x , %d KB" % (header_rom_size,2**header_rom_size))

# RAM size
header_ram_size = single_read(ser,0x00ffd8)
print ("RAM size: 0x%x , %d KB" % (header_ram_size,2**header_ram_size))

# Destination Code
header_destination_code = single_read(ser,0x00ffd9)
print ("Destination code: 0x%x" % (header_destination_code))

# Fixed value
header_fixed_value = single_read(ser,0x00ffda)
print ("Fixed value : 0x%x" % (header_fixed_value))

# version
header_version = single_read(ser,0x00ffdb)
print ("Version: 0x%x" % (header_version))

# complement check high
header_comp_sum =  single_read(ser,0x00ffdd) << 8 | single_read(ser,0x00ffdc)
# sum check low
header_sum      =  single_read(ser,0x00ffdf) << 8 | single_read(ser,0x00ffde)

if ((header_comp_sum ^ header_sum)==0xffff):
    print("sum comp check ok")
    print("sum : 0x%04x"%(header_sum))
else:
    print("sum comp check error")
    print("comp sum : 0x%04x" %(header_comp_sum))
    print("     sum : 0x%04x" %(header_sum))
    quit()


outfile=header_title.replace('-','_').replace('/','_').replace('\"','_').replace('\?','_').replace(' ','_').replace("\'","_")+".sfc"
snesf=open((outfile),"wb")
# whole sum
checksum=0
# 1/2 sum
# quat sum from half
checksum_half_to_q=0
# half qart sum from half
checksum_half_to_hq=0

# LoROM
if (header_map_mode == 0x20 or header_map_mode == 0x30 or header_map_mode == 0x32):
    for bank in range(2**(header_rom_size-5)):
        ser.write(( "PRA%02X%02X" %( bank,0x80)).encode())
        for addr in range(32*1024):
            data = int(ser.read(2).decode(),16)
            checksum=(checksum+data)&0xffff
            if (bank<2**(header_rom_size-6)):
                checksum_half_to_hq=(checksum_half_to_hq+data)&0xffff
                checksum_half_to_q =(checksum_half_to_q +data)&0xffff
            else:
                if( ( header_rom_size>=8) and (bank < ((2**(header_rom_size-6))+(2**(header_rom_size-8)))) ):
                    checksum_half_to_hq=(checksum_half_to_hq+4*data)&0xffff
                if( ( header_rom_size>=7) and (bank < ((2**(header_rom_size-6))+(2**(header_rom_size-7)))) ):
                    checksum_half_to_q =(checksum_half_to_q +2*data)&0xffff

            snesf.write(struct.pack("B", data ))
# HiROM or ExHiROM
elif  (header_map_mode == 0x21 or header_map_mode == 0x23 or header_map_mode == 0x31 or header_map_mode == 0x35 or header_map_mode == 0x3a):
    for bank in range(2**(header_rom_size-6)):

        # SPC7110 bank mapping
        if(header_map_mode == 0x3a):
            ser.write(( "SWA%06X%02X" %( 0x004830,0x80)).encode())
            ser.read(2)
            ser.write(( "SWA%06X%02X" %( 0x004831,0x00)).encode())
            ser.read(2)
            ser.write(( "SWA%06X%02X" %( 0x004832,0x01)).encode())
            ser.read(2)
            ser.write(( "SWA%06X%02X" %( 0x004833,0x02)).encode())
            ser.read(2)
            ser.write(( "SWA%06X%02X" %( 0x004834,0xFF)).encode())
            ser.read(2)



        if (bank<64):
            ser.write(( "PRA%02X%02X" %( 0xC0+bank,0x00)).encode())
            print(( "PRA%02X%02X" %( 0xC0+bank,0x00)))
        else:
            ser.write(( "PRA%02X%02X" %( 0x40+(bank-64),0x00)).encode())
            print(( "PRA%02X%02X" %( 0x40+(bank-64),0x00)))


        for addr in range(32*1024):
            data = int(ser.read(2).decode(),16)


            checksum=(checksum+data)&0xffff
            if (bank<2**(header_rom_size-7)):
                checksum_half_to_hq=(checksum_half_to_hq+data)&0xffff
                checksum_half_to_q =(checksum_half_to_q +data)&0xffff
            else:
                if( ( header_rom_size>=9) and (bank < ((2**(header_rom_size-7))+(2**(header_rom_size-9)))) ):
                    checksum_half_to_hq=(checksum_half_to_hq+4*data)&0xffff
                if( ( header_rom_size>=8) and (bank < ((2**(header_rom_size-7))+(2**(header_rom_size-8)))) ):
                    checksum_half_to_q =(checksum_half_to_q +2*data)&0xffff
            snesf.write(struct.pack("B", data ))

        
        if (bank<64):
            ser.write(( "PRA%02X%02X" %( 0xC0+bank,0x80)).encode())
            print(( "PRA%02X%02X" %( 0xC0+bank,0x80)))
        else:
            ser.write(( "PRA%02X%02X" %( 0x40+(bank-64),0x80)).encode())
            print(( "PRA%02X%02X" %( 0x40+(bank-64),0x80)))
        for addr in range(32*1024):
            data = int(ser.read(2).decode(),16)
            checksum=(checksum+data)&0xffff
            if (bank<2**(header_rom_size-7)):
                checksum_half_to_hq=(checksum_half_to_hq+data)&0xffff
                checksum_half_to_q =(checksum_half_to_q +data)&0xffff
            else:
                if( ( header_rom_size>=9) and (bank < ((2**(header_rom_size-7))+(2**(header_rom_size-9)))) ):
                    checksum_half_to_hq=(checksum_half_to_hq+4*data)&0xffff
                if( ( header_rom_size>=8) and (bank < ((2**(header_rom_size-7))+(2**(header_rom_size-8)))) ):
                    checksum_half_to_q =(checksum_half_to_q +2*data)&0xffff
            snesf.write(struct.pack("B", data ))





if (checksum_half_to_hq==header_sum):
    print("Checksum verification result ... 5/8 size : OK")
    snesf.seek(5*32*1024*(2**(header_rom_size-8)))
    snesf.truncate()
    snesf.close  
elif (checksum_half_to_q==header_sum):
    print("Checksum verification result ... 3/4 size : OK")
    snesf.seek(3*32*1024*(2**(header_rom_size-7)))
    snesf.truncate()
    snesf.close
elif (checksum==header_sum ):
    print("Checksum verification result : OK")
    print("Calculated checksum 0x%04x"%(checksum))
    snesf.close()
else:
    print("Checksum verification result : FAIL")
    
    
quit()
