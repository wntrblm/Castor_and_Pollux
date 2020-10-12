import csv

entries = []

with open("calibration-table.csv", "r") as fh:
    reader = csv.DictReader(fh)

    for row in reader:
        entries.append(row)
 
print("""\
#include "gem_voice_param_table.h"
#include "fix16.h"

const struct gem_voice_voltage_and_period gem_voice_voltage_and_period_table[] = {\
""")

for row in entries:
    print(f"  {{.voltage = F16({row['Input CV']}), .period = {row['period reg']}}},")

print("""\
};

struct gem_voice_dac_codes gem_voice_dac_codes_table[] = {\
""")

for row in entries:
    print(f"  {{.castor = {row['castor calibrated dac code']}, .pollux = {row['pollux calibrated dac code']} }},")


print("""\
};

size_t gem_voice_param_table_len = sizeof(gem_voice_voltage_and_period_table) / sizeof(struct gem_voice_voltage_and_period);
""")