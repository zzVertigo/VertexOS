#include <system/drivers/processor.h>

cpu_info_t cpu_info;

extern cpuid_t cpu_id;

static const char* vendor_id_strings[] = {
    "GenuineIntel",
    "AMDisbetter!",
    "AuthenticAMD",
    "CentaurHauls",
    "CyrixInstead",
    "TransmetaCPU",
    "GenuineTMx86",
    "Geode by NSC",
    "NexGenDriven",
    "RiseRiseRise",
    "SiS SiS SiS ",
    "UMC UMC UMC ",
    "VIA VIA VIA ",
    "Vortex86 SoC",
    "KVMKVMKVM",
    "Microsoft Hv",
    "VIA VIA VIA ",
    "VMwareVMware",
    " lrpepyh vr"
};

static const char* vendor_company_strings[] = {
    "Intel",
    "K5-AMD",
    "AMD",
    "Centaur",
    "Cyrix",
    "Transmeta",
    "Transmetax86",
    "National Semiconductor",
    "NexGen",
    "Rise",
    "SiS",
    "UMC ",
    "VIA ",
    "Vortex86",
    "KVM",
    "Microsoft Hypervisor",
    "VIA",
    "VMware",
    "Parrelels"
};

void processor_dump_context(registers_t context) {
    printf("EAX = 0x%x, EBX = 0x%x, ECX = 0x%x, EDX = 0x%x\n", context.eax, context.ebx, context.ecx, context.edx);
    printf("ESP = 0x%x, EIP = 0x%x, EBP = 0x%x, EDI = 0x%x\n", context.esp, context.eip, context.ebp, context.edi);
    printf("ESI = 0x%x, EFLAGS = 0x%x", context.esi, context.eflags);
}

static inline void find_comp(char *ID) {
    for (int i = 0; vendor_id_strings[i]; i++) {
        if (strcmp(ID, vendor_id_strings[i]) == 0) {
            cpu_info.company_name = (char*)vendor_id_strings[i];
            return;
        }

        cpu_info.company_name = "Unknown";
    }
}

static void signature_process(u32 sig) {
    if(strcmp(cpu_info.company_name, "Intel") == 0)
    {
        int bit_twelve = (sig >> 12) & 1;
        int bit_thirteen = (sig >> 13) & 1;
        if(bit_twelve == 0 && bit_thirteen == 0)
            cpu_info.processor_type = "Original OEM Proccecor";
        else if(bit_twelve == 0 && bit_thirteen == 1)
            cpu_info.processor_type = "OverDrive Proccecor";
        else if(bit_twelve == 1 && bit_thirteen == 0)
            cpu_info.processor_type = "Dual Proccecor";
        else if(bit_twelve == 1 && bit_thirteen == 1)
            cpu_info.processor_type = "Intel Reserved.";
        else
            cpu_info.processor_type = "Unknown";
    }
}

void cpu_info_setup() {
    cpu_info.vendor_id = (char*) cpu_id.vendorID;
    cpu_info.ecx_features = cpu_id.features_ecx;
    cpu_info.edx_features = cpu_id.features_edx;
    cpu_info.signature = cpu_id.signature;
    find_comp(cpu_info.vendor_id);
    signature_process(cpu_info.signature);
}