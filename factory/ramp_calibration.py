from libgemini import gemini

period_to_dac_code = {}

with open("calibration-table.csv", "r") as fh:
    reader = csv.DictReader(fh)

    for row in reader:
        period_to_dac_code[row["period reg"]] = row["castor calibrated dac code"]


def main(save):
    gem = gemini.Gemini()

    gem.enter_calibration_mode()

    for period, dac_code in period_to_dac_code.items():
        print(f"Period: {period}")
        gem.set_period(1, period)

        while True:
            gem.set_dac(2, dac_code, gain=1)
            reply = input("(u)p, (d)own, (u)p a (l)ittle, (d)own a (l), or (o)kay)? ")
            if reply == "u":
                dac_code += 10
            if reply == "d":
                dac_code -= 10
            if reply == "ul":
                dac_code += 1
            if reply == "dl":
                dac_code -= 1
            if reply == "o":
                break
        
        period_to_dac_code[period] = dac_code

    # TODO: Save these in NVM.
    if save:
        for dac_code in dac_codes:
            print(f"{dac_code}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--dry_run", type=bool, action="store_true", default=False, help="Don't save the calibration values.")

    args = parser.parse_args()

    main(not args.dry_run)