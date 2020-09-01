import csv

entries = []

with open("calibration-table.csv", "r") as fh:
    reader = csv.DictReader(fh)

    for row in reader:
        entries.append(row)


entries.reverse()

for row in entries:
    print(f"{{.adc_code = {row['adc code']}, .period_reg = {row['period reg']}, .dac_code = {row['dac code']} }},")