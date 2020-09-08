import csv

entries = []

with open("calibration-table.csv", "r") as fh:
    reader = csv.DictReader(fh)

    for row in reader:
        entries.append(row)


entries.reverse()

for row in entries:
    print(f"{{.voltage = {row['Input CV']}, .period_reg = {row['period reg']}, .castor_dac_code = {row['calibrated dac code']}, .pollux_dac_code = {row['calibrated dac code']} }},")