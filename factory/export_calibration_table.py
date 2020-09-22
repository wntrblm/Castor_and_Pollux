import csv

entries = []

with open("calibration-table.csv", "r") as fh:
    reader = csv.DictReader(fh)

    for row in reader:
        entries.append(row)


for row in entries:
    print(f"{{.voltage = F16({row['Input CV']}), .period_reg = {row['period reg']}, .castor_dac_code = {row['castor calibrated dac code']}, .pollux_dac_code = {row['pollux calibrated dac code']} }},")