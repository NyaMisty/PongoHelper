#include <pongo.h>

#include "printf.h"
#include "stub.h"
#include "capstone.h"
#include "tinyalloc.h"

void (*existing_preboot_hook)();

void m_preboot_hook()
{
    command_puts("Called pre-boot hook");
    printf("aaa: %d", existing_preboot_hook);
    command_puts("Resetting ramdisk_size to 0");
    ramdisk_size = 0;
    command_puts("Kick off!");
    for (long long i = 0; i < 3*1000*1000*1000LL; i++);
    
    /* Do patches here */
    //if (existing_preboot_hook != NULL)
    //	existing_preboot_hook();
    return;
}

void print_addr(const char *str, uint64_t val) {
    printf("%s: %llx\n", str, val);
}

void dump_runtime_vars() {
    print_addr("loader_xfer_recv_data", loader_xfer_recv_data);
    print_addr("loader_xfer_recv_count", loader_xfer_recv_count);
    
    print_addr("gBootArgs", gBootArgs);
    printf("  .Revision: %d\n", gBootArgs->Revision);
    printf("  .Version: %d\n", gBootArgs->Version);
    print_addr("  .virtBase", gBootArgs->virtBase);
    print_addr("  .physBase", gBootArgs->physBase);
    print_addr("  .memSize", gBootArgs->memSize);
    print_addr("  .topOfKernelData", gBootArgs->topOfKernelData);
    printf("  .machineType: %d\n", gBootArgs->machineType);
    print_addr("  .CommandLine: ", &gBootArgs->CommandLine);
    print_addr("  .bootFlags", gBootArgs->bootFlags);
    print_addr("  .memSizeActual", gBootArgs->memSizeActual);
    
    print_addr("gEntryPoint", gEntryPoint);
    print_addr("gDeviceTree", gDeviceTree);
    print_addr("gDevType", gDevType);
    print_addr("ramdisk_buf", ramdisk_buf);
    print_addr("autoboot_count", autoboot_count);
    print_addr("existing_preboot_hook", existing_preboot_hook);
}

uint64_t get_hex_num(char *input) {
    if (input[0] != '0' || input[1] != 'x')
        return 0;
    uint64_t ret = 0, cur = 2;
    while (1) {
        char c = input[cur++];
        if (!c)
            break;
        if (c >= '0' && c <= '9'){
            c = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            c = c - 'a' + 10;
        } else {
            command_puts("Wrong format");
        }
        ret *= 16;
        ret += c;
    }
    return ret;
}

void mem_read(const char *cmd, char *args) {
    uint64_t addr = get_hex_num(args);
    uint32_t data = *(uint32_t *)addr;
    printf("Read data: %08x\n", data);
}

void mem_write(const char *cmd, char *args) {
    char tempbuf[34];
    int i = 0; char c = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    if (c == 0){
        command_puts(tempbuf);
        command_puts("mwrite target_addr width value");
        return;
    }
    tempbuf[i] = 0;
    uint64_t target = get_hex_num(tempbuf);
    
    char width = *args++;
    if (width != 'b' && width != 'w' && width != 'd' && width != 'q') {
        printf("wrong width: %c", width);
        return;
    }
    
    args++; // eat space
    
    strcpy(tempbuf, args);
    uint64_t value = get_hex_num(tempbuf);
    
    printf("Writing to %x\n", target);
    printf("Width %c\n", width);
    printf("Value %x\n", value);
    switch(width) {
        case 'b':
            *(uint8_t *)target = value;
            break;
        case 'w':
            *(uint16_t *)target = value;
            break;
        case 'd':
            *(uint32_t *)target = value;
            break;
        case 'q':
            *(uint64_t *)target = value;
            break;
    }
    command_puts("write success");
}

void disassemble(const char *cmd, char *args) {
    setup_capstone();
    char tempbuf[34];
    int i = 0; char c = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    tempbuf[i] = 0;
    uint64_t addr = get_hex_num(tempbuf);
    print_addr("Going to disassemble", addr);
    
    uint64_t len = 20;
    if (c != 0) {
        args++;
        strcpy(tempbuf, args);
        len = get_hex_num(tempbuf);
    }
    print_addr("Disassemble len", len);
    
    cs_insn* insn;
    len = 5;
    size_t count = cs_disasm(cs_handle, (unsigned char*)addr, len * 4, addr, 0, &insn);
    printf("Got %d ins\n", count);
    if (count) {
        size_t j;
        for (j = 0; j < count; j++) {
            //printf("Processed ins %d\n", j);
            printf("0x%llx  ", insn[j].address);
            for (int off = 0; off < insn[j].size; off++){
                printf("%02X ", insn[j].bytes[off]);
            }
            printf("\t%s\t\t%s\n", insn[j].mnemonic, insn[j].op_str);
            /*printf("0x%llx:\t%s\t\t%s\n",
                   insn[j].address, insn[j].mnemonic, insn[j].op_str);*/
        }

        cs_free(insn, count);
    }
    printf("Disassembled %d instruction", count);
}


void module_entry() {
    existing_preboot_hook = preboot_hook;
    preboot_hook = m_preboot_hook;
    dump_runtime_vars();
    
    init_ta();
    
    setup_capstone();
    
    command_register("mread", "Read memory", mem_read);
    command_register("mwrite", "Write memory", mem_write);
    command_register("dis", "Disassemble memory", disassemble);
}

char* module_name = "test_module";

struct pongo_exports exported_symbols[] = {
    {.name = 0, .value = 0}
};
	
