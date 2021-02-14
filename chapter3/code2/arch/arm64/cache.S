.global __tlbi_vmalle1
__tlbi_vmalle1:
    tlbi    vmalle1
    dsb     sy
    isb
    ret
