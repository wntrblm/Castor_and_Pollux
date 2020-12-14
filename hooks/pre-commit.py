#!/usr/bin/env python3

import sys
import git

repo = git.Repo()


def check_for_outdated_pdfs():
    """Simple check that makes sure that PDFs for schematics are up-to-date"""
    modified_paths = [diff.a_path for diff in repo.index.diff(None)]
    modified_paths.extend([diff.a_path for diff in repo.index.diff("HEAD")])
    modified_schs = [path for path in modified_paths if path.endswith(".sch")]

    for path in modified_schs:
        pdf_path = path.replace(".sch", ".pdf")
        if pdf_path not in modified_paths:
            print(f"Schematic {path} modified but PDF {pdf_path} hasn't been updated.", file=sys.stderr)
            exit(-1)


if __name__ == "__main__":
    check_for_outdated_pdfs()