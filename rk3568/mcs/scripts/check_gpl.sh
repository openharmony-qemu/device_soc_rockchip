#!/usr/bin/env bash
set -u
CODE=${CODE:-/srv/workspace/openharmony_OpenHarmony-6.1-LTS_default_20260910192724_huawei_2e711df5d/code}
SRC=$CODE/out/kernel/src_tmp/linux-6.6
for s in of_address_to_resource of_property_read_string register_kprobe unregister_kprobe; do
  n=$(grep -rhoE "EXPORT_SYMBOL(_GPL|_NS|_GPL_NS)?\($s\)" "$SRC" --include=*.c --include=*.h --include=*.S | wc -l)
  echo "$s => $n"
done
