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
    //setup_capstone();
    char tempbuf[0x400];
    int i = 0; char c = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    tempbuf[i] = 0;
    uint64_t addr = get_hex_num(tempbuf);
    print_addr("Going to disassemble", addr);
    
    uint64_t len = 20;
    if (c != 0) {
        strcpy(tempbuf, args);
        printf("%s", args);
        len = get_hex_num(tempbuf);
    }
    print_addr("Disassemble len", len);
    
    memcpy(tempbuf, addr, len * 4);
    cs_insn* insn;
    size_t count = cs_disasm(cs_handle, (unsigned char*)tempbuf, len * 4, addr, 0, &insn);
    printf("Got %d ins\n", count);
    if (count) {
        size_t j;
        for (j = 0; j < count; j++) {
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
    printf("Disassembled %d instruction\n", count);
}

void memfind(const char *cmd, char *args) {
    char tempbuf[0x400];
    int i = 0; char c = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    tempbuf[i] = 0;
    uint64_t addr = get_hex_num(tempbuf);
    if (c == 0) {
        printf("Wrong usage1\n");
        return;
    }
    printf("Got target addr: 0x%llx\n", addr);
    
    i = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    tempbuf[i] = 0;
    uint64_t len = get_hex_num(tempbuf);
    if (c == 0) {
        printf("Wrong usage2\n");
        return;
    }
    printf("Got len: 0x%llx\n", len);
    
    i = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    tempbuf[i] = 0;
    uint64_t _pattern = get_hex_num(tempbuf);
    char *pattern = (char *)&_pattern;
    if (c == 0) {
        printf("Wrong usage3\n");
        return;
    }
    printf("Got pattern: 0x%llx\n", _pattern);
    
    i = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    tempbuf[i] = 0;
    uint64_t pattern_size = get_hex_num(tempbuf);
    printf("Got pattern size: 0x%llx\n", pattern_size);
    
    uint64_t curaddr = addr;
    while (curaddr < addr + len) {
        //printf("Searching pattern from %llx\n", curaddr);
        memcpy(tempbuf, curaddr, sizeof(tempbuf));
        for (int i = 0; i < sizeof(tempbuf) - pattern_size; i++) {
            for (int c = 0; c < pattern_size; c++) {
                if (tempbuf[i + c] != pattern[c]) {
                    goto notgood;
                }
            }
            printf("Found pattern at address 0x%llx\n", curaddr + c);
            return;
        notgood:
            continue;
        }
        curaddr += (sizeof(tempbuf) - pattern_size) & ~0xf;
    }
    printf("Pattern not found\n");
}

int prop_callback(dt_find_cb_t *userarg, dt_node_t *node, int level, const char *name, void *val, uint32_t length) {
    if (strcmp(name, "name") == 0) {
        printf("propcallback: node: %llx level: %d name: %s val: %llx length: %d\n", node, level, name, val, length);
        printf("curnode_name: %s\n", val);
        if (strcmp(val, userarg->name) == 0){
            
            printf("Found target node!\n");
            userarg->node = node;
            return 1;
        }
    }
    return 0;
}

extern dt_node_t *memmap = NULL;

void find_memmap() {
    dt_find_cb_t tempbuf;
    tempbuf.name = "memory-map";
    tempbuf.node = NULL;
    printf("Passing %llx as custom arg\n", tempbuf);
    dt_parse(gDeviceTree, 0, NULL, NULL, NULL, prop_callback, &tempbuf);
    if (!tempbuf.node) {
        printf("Failed to find memmap\n");
    }
    printf("Found memmap at %llx\n", tempbuf.node);
    memmap = tempbuf.node;
}

void print_memmap(){
    if (!memmap) {
        find_memmap();
    }
    int i = 0, off = 0;
    while(i++ < memmap->nprop) {
       dt_prop_t *prop = &memmap->prop[off];
       
       if (prop->len == 16){
           // memmap entry
           struct memmap *curentry = prop->val;
           printf("prop key: %s, len: %d, addr: %llx, memsize: %llx\n", prop->key, prop->len, curentry->addr, curentry->size);
       } else if (prop->len == 4) {
           // DWORD entry
           printf("prop key: %s, len: %d, val: %d\n", prop->key, prop->len, *(uint32_t *)prop->val);
       } else {
           // string entry
           printf("prop key: %s, len: %d, val: %s\n", prop->key, prop->len, prop->val);
       }
       
       //off += DT_KEY_LEN + 4 + prop->len;
       off += (((prop->len & 0xFFFFFF) + 3) & 0x1FFFFFC) + 36;
    }
}

#define HEXDUMP_COLS 8
// from https://gist.github.com/richinseattle/c527a3acb6f152796a580401057c78b4
void _hexdump(void *mem, unsigned int len)
{
        unsigned int i, j;
        
        for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
        {
                /* print offset */
                if(i % HEXDUMP_COLS == 0)
                {
                        printf("0x%06x: ", i);
                }
 
                /* print hex data */
                if(i < len)
                {
                        printf("%02x ", 0xFF & ((char*)mem)[i]);
                }
                else /* end of block, just aligning for ASCII dump */
                {
                        printf("   ");
                }
                
                /* print ASCII dump */
                if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
                {
                        for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
                        {
                                if(j >= len) /* end of block, not really printing */
                                {
                                        putchar(' ');
                                }
                                else if(isprint(((char*)mem)[j])) /* printable char */
                                {
                                        putchar(0xFF & ((char*)mem)[j]);
                                }
                                else /* other char */
                                {
                                        putchar('.');
                                }
                        }
                        putchar('\n');
                }
        }
}
 
void hexdump(const char *cmd, char *args) {
    char tempbuf[0x400];
    int i = 0; char c = 0;
    while ((c = *args++) != ' ' && c != 0) {
        tempbuf[i++] = c;
    }
    tempbuf[i] = 0;
    uint64_t addr = get_hex_num(tempbuf);
    print_addr("Going to dump", addr);
    
    uint64_t len = 48;
    if (c != 0) {
        strcpy(tempbuf, args);
        printf("%s", args);
        len = get_hex_num(tempbuf);
    }
    print_addr("Dump len", len);
    _hexdump(addr, len);
}

void module_entry() {
    printf("Got preboot_hook: 0x%llx\n", preboot_hook);
    existing_preboot_hook = preboot_hook;
    preboot_hook = m_preboot_hook;
    
    printf("Dumping runtime vars...\n");
    dump_runtime_vars();
    
    init_ta();
    
    setup_capstone();
    
    find_memmap();
    
    command_register("mread", "Read memory", mem_read);
    command_register("mwrite", "Write memory", mem_write);
    command_register("mfind", "Search memory", memfind);
    command_register("dis", "Disassemble memory", disassemble);
    command_register("dumpvars", "Dump runtime vars", dump_runtime_vars);
    command_register("memmap", "Print memmap", print_memmap);
    command_register("hexdump", "Hexdump", hexdump);
}

char* module_name = "test_module";

struct pongo_exports exported_symbols[] = {
};

