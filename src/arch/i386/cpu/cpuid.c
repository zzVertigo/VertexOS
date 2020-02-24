#include <arch/i386/cpu/cpuid.h>

cpuid_t cpu_id;

bool has_cpuid_ins(){
	int res;
	__asm__ __volatile__ (".intel_syntax\n\t"
	                      "pushfd\n\t"
	                      "pushfd\n\t"
	                      "xor dword ptr [%%esp],0x200000\n\t"
	                      "popfd\n\t"
	                      "pushfd\n\t"
	                      "pop %%eax\n\t"
	                      "xor %%eax,[%%esp]\n\t"
	                      "popfd\n\t"
	                      "and %%eax,0x200000\n\t"
	                      ".att_syntax"
	                      :"=a"(res));
	return res;
}

void cpuid_setup() {
    if (!has_cpuid_ins())
        return;

    int res;

    __cpuid(CPUID_GETVENDORSTRING, res, cpu_id.vendorID[0], cpu_id.vendorID[2], cpu_id.vendorID[1]);
    __cpuid(CPUID_GETFEATURES, cpu_id.signature, res, cpu_id.features_ecx, cpu_id.features_edx);
}