#include <idc.idc>

static main()
{
    auto ea, file, start, filename;
        
    filename = "c:\\x5_fw_b7.bin";
    
    file = fopen(filename, "rb");
    if (!file)
    {
        Message("Unable to open firmware: %s\n", filename);
        return;
    }
    
    // In the beginning of the firmware they copy
    // from FLASH (000xxxxx) to SDRAM (30xxxxxx) and IRAM(10xxxxxx)
  
    // This table is extracted from these routines
    
    // x5_fw_b7.bin
    load(file, "SDRAM_00", 0x000105ee, 0x30c00000, 0x30c00400); // Load code at 0x10010 = file offset 0x1040
    load(file, "IRAM_00" , 0x000109ee, 0x10000000, 0x100008c4); // Load code at 0x10042 = file offset 0x1072
    load(file, "IRAM_01" , 0x000112b2, 0x100008d0, 0x10001140); // Load code at 0x10074 = file offset 0x10a4
    load(file, "IRAM_02" , 0x00011b22, 0x100011d0, 0x10001504); // Load code at 0x100a6 = file offset 0x10d6
    load(file, "IRAM_03" , 0x00011e56, 0x1000bc90, 0x100107da); // Load code at 0x100d8 = file offset 0x1108
    load(file, "IRAM_04" , 0x000169a0, 0x100107dc, 0x10014540); // Load code at 0x1010a = file offset 0x113a
    load(file, "SDRAM_01", 0x0001a704, 0x30ec6ea0, 0x30ec9174); // Load code at 0x101a0 = file offset 0x11d0
    load(file, "SDRAM_02", 0x0001c9d8, 0x30ec9178, 0x30f82412); // Load code at 0x101d2 = file offset 0x1202
    load(file, "SDRAM_03", 0x000d5c72, 0x30f82418, 0x30fab5be); // Load code at 0x10204 = file offset 0x1234
    load(file, "SDRAM_04", 0x000fee18, 0x30fab5c0, 0x30fac3e0); // Load code at 0x10236 = file offset 0x1266
    load(file, "SDRAM_05", 0x000ffc38, 0x30fac5a0, 0x30fae0b8); // Load code at 0x10268 = file offset 0x1298
    load(file, "SDRAM_06", 0x00101750, 0x30fae0b8, 0x30fb05d0); // Load code at 0x1029a = file offset 0x12ca
    load(file, "SDRAM_07", 0x00103c68, 0x30fb5550, 0x30fb6548); // Load code at 0x102cc = file offset 0x12fc
    load(file, "SDRAM_08", 0x00104c60, 0x30fb6550, 0x30fb7388); // Load code at 0x102fe = file offset 0x132e
    load(file, "SDRAM_09", 0x00105a98, 0x30fb7390, 0x30fb80f4); // Load code at 0x10330 = file offset 0x1360
    load(file, "SDRAM_10", 0x001067fc, 0x30fb8100, 0x30fbac12); // Load code at 0x10362 = file offset 0x1392
    load(file, "SDRAM_11", 0x0010930e, 0x30fbac20, 0x30fbcc78); // Load code at 0x10394 = file offset 0x13c4
    load(file, "SDRAM_12", 0x0010b366, 0x30fbcc80, 0x30fbe098); // Load code at 0x103c6 = file offset 0x13f6
    load(file, "SDRAM_13", 0x0010c77e, 0x30fbe0a0, 0x30fbf1e4); // Load code at 0x103f8 = file offset 0x1428
    load(file, "SDRAM_14", 0x0010d8c2, 0x30fbf1f0, 0x30fc0860); // Load code at 0x1042a = file offset 0x145a
    load(file, "SDRAM_15", 0x0010ef32, 0x30fc0860, 0x30fcc22c); // Load code at 0x1045c = file offset 0x148c
    load(file, "IRAM_05" , 0x0011a8fe, 0x100148f0, 0x10015154); // Load code at 0x1048e = file offset 0x14be
    load(file, "SDRAM_16", 0x0011b162, 0x30fcc230, 0x30fceb56); // Load code at 0x104c0 = file offset 0x14f0
    load(file, "SDRAM_17", 0x0011da88, 0x30fceb60, 0x30fcf634); // Load code at 0x104f2 = file offset 0x1522
    load(file, "IRAM_06" , 0x0011e55c, 0x10016344, 0x1001cb82); // Load code at 0x10524 = file offset 0x1554
    load(file, "IRAM_07" , 0x00124d9a, 0x10014bbc, 0x10015c94); // Load code at 0x10556 = file offset 0x1586
    load(file, "SDRAM_18", 0x00125e72, 0x30fd3be0, 0x30ff5b64); // Load code at 0x10588 = file offset 0x15b8
    
    Wait();
    
    start = 0x30F79EAC;
    MakeName(start, "start");
    MakeComm(start, "This function is called when IRAM/SDRAM has been loaded");
    Jump(start);
}

static load(file, segment_name, flash_addr, dest_start, dest_end)
{
    if (dest_end <= dest_start)
        return;
    
    SegCreate(dest_start, dest_end, 0,0,0,0);
    SegRename(dest_start, segment_name);
    SegClass (dest_start, "CODE");
    
    fseek(file, 0, 0);
    loadfile(file, flash_addr - 0xEFD0, dest_start, dest_end - dest_start);

    make_code(dest_start, dest_end);
}                                                                

// Very basic analysis: Search for rts and rte and make code/function
// 0x4e 0x71 is some nop instruction which is used as a filler so skip them
static make_code(start, end)
{
    auto ea;
    auto next1, next2;
        
    ea = start;
    while (ea != BADADDR && ea < end)    
    {                                                                 
        MakeCode(ea);                                                 
                                                                      
        ea = ea + 2;                                                  
                                                                      
        while ( (Byte(ea) == 0x4e && Byte(ea+1) == 0x71) ||           
                (Byte(ea) == 0x71 && Byte(ea+1) == 0x4e) )            
        {                                                             
            ea = ea + 2;                                              
        }                                                             
                                                                      
        if (MakeCode(ea))                                             
        {                                                             
            MakeFunction(ea, BADADDR);                                
        }                                                             
                                                                      
        next1 = FindBinary(ea, SEARCH_DOWN, "4e 75");  // rts
        next2 = FindBinary(ea, SEARCH_DOWN, "4e 73");  // rte
        
        if (next1 == BADADDR)
            ea = next2;
        else if (next1 < next2)
            ea = next1;
        else
            ea = next2;
    } 

}
