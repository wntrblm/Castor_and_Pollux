import os.path
import pathlib
import platform
import subprocess
import time
import xml.dom.minidom
import zipfile
import io
import shutil

import requests

from libwinter import uf2conv

try:
    import win32api

    WINDOWS = True
except ImportError:
    WINDOWS = False

PLATFORM = platform.system()

if PLATFORM == "Darwin":
    MACOS = True
else:
    MACOS = False


if WINDOWS:
    JLINK_PATH = "C:\Program Files (x86)\SEGGER\JLink\JLink.exe"
elif MACOS:
    JLINK_PATH = "JlinkExe"


CACHE_DIRECTORY = ".cache"
BOOTLOADER_RELEASES_URL = (
    "https://api.github.com/repos/adafruit/uf2-samdx1/releases/latest"
)
CIRCUITPYTHON_RELEASES_BASE = "https://adafruit-circuit-python.s3.amazonaws.com/"
CIRCUITPYTHON_RELEASES_URL = "https://adafruit-circuit-python.s3.amazonaws.com/?list-type=2&prefix=bin/{device_name}/en_US/"


def _find_drive_by_name_windows(name):
    drives = win32api.GetLogicalDriveStrings()
    drives = drives.split("\000")[:-1]
    for drive in drives:
        info = win32api.GetVolumeInformation(drive)
        if info[0] == name:
            return drive
    raise RuntimeError(f"No drive {name} found.")


def _find_drive_by_name_macos(name):
    drive = os.path.join(f"/Volumes/{name}")
    if os.path.exists(drive):
        return drive
    raise RuntimeError(f"No drive {name} found, expected at {drive}.")


def find_drive_by_name(name):
    if WINDOWS:
        return _find_drive_by_name_windows(name)
    elif MACOS:
        return _find_drive_by_name_macos(name)
    else:
        raise EnvironmentError("Idk what platform I'm running on.")


def wait_for_drive(name, timeout=10):
    for n in range(timeout):
        try:
            path = find_drive_by_name(name)
            if n > 1:
                # Wait a second because the drive may not be fully mounted.
                time.sleep(1)
            return path
        except RuntimeError:
            time.sleep(1)
            pass

    raise RuntimeError(f"Drive {path} never showed up.")


def flush(path):
    if WINDOWS:
        drive, _ = os.path.splitdrive(path)
        subprocess.run(
            ["sync", drive], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
        )
    elif MACOS:
        mountpoint = os.path.join(os.path.join(*os.path.split(path)[:2]))
        fd = os.open(mountpoint, os.O_RDONLY)
        os.fsync(fd)
        os.close(fd)


def unmount(path):
    if WINDOWS:
        pass
    elif MACOS:
        disk = None
        mount_output = subprocess.check_output(["mount"]).decode("utf-8").splitlines()

        for line in mount_output:
            items = line.split(" ")
            if items[2] == path:
                disk = items[0].split("/").pop()
                break

        if disk is None:
            print(f"Warning: unable to find device for {path}")
            return

        subprocess.check_output(["diskutil", "unmount", disk])


def copyfile(src, dst):
    # shutil can be a little wonky, so do this manually.
    with open(src, "rb") as fh:
        contents = fh.read()

    with open(dst, "wb") as fh:
        fh.write(contents)
        fh.flush()

    flush(dst)


def deploy_files(srcs_and_dsts, destination):
    os.makedirs(os.path.join(destination, "lib"), exist_ok=True)

    for src, dst in srcs_and_dsts.items():

        full_dst = os.path.join(destination, dst)

        if os.path.isdir(src):
            full_dst = os.path.join(full_dst, os.path.basename(src))
            if os.path.exists(full_dst):
                shutil.rmtree(full_dst)
            shutil.copytree(src, full_dst)

        else:
            if os.path.splitext(full_dst)[1]:
                # Destination is a filename, make sure parent directories exist.
                os.makedirs(os.path.dirname(full_dst), exist_ok=True)
            else:
                # Destination is a directory, make sure it exists.
                os.makedirs(full_dst, exist_ok=True)

            shutil.copy(src, full_dst)

        src = os.path.relpath(src, start=os.path.join(os.curdir, ".."))
        print(f"Copied {src} to {dst}")

    flush(destination)


def clean_pycache(root):
    for p in pathlib.Path(root).rglob("*.py[co]"):
        p.unlink()
    for p in pathlib.Path(root).rglob("__pycache__"):
        p.rmdir()


def find_latest_bootloader(device_name):
    response = requests.get(BOOTLOADER_RELEASES_URL).json()
    for asset in response["assets"]:
        asset_name = asset["name"]
        if device_name in asset_name and asset_name.endswith(".bin"):
            return asset["browser_download_url"]

    raise RuntimeError(f"Could not find bootloader for {device_name}")


def find_latest_circuitpython(device_name):
    response = requests.get(CIRCUITPYTHON_RELEASES_URL.format(device_name=device_name))
    doc = xml.dom.minidom.parseString(response.text)
    files = doc.getElementsByTagName("Contents")
    files.sort(
        key=lambda tag: tag.getElementsByTagName("LastModified")[
            0
        ].firstChild.nodeValue,
        reverse=True,
    )

    for file in files:
        key = file.getElementsByTagName("Key")[0].firstChild.nodeValue
        release = key.rsplit("en_US")[-1][1:]

        # if a - is in release, it's an alpha/beta/rc/hash build
        if "-" in release:
            continue

        return CIRCUITPYTHON_RELEASES_BASE + key

    raise RuntimeError(f"Could not find CircuitPython release for {device_name}")


def get_cache_path(name):
    return os.path.join(CACHE_DIRECTORY, name)


def download_file_to_cache(url, name):
    os.makedirs(CACHE_DIRECTORY, exist_ok=True)
    dst_path = get_cache_path(name)

    # ":" Indicates a zip file that needs a single file extracted from it.
    if url.startswith("https+zip://"):
        url, zip_path = url.rsplit(":", 1)
        url = url.replace("https+zip", "https")
    else:
        zip_path = None

    if (
        os.path.exists(dst_path)
        and os.path.getmtime(dst_path) > time.time() - 24 * 60 * 60
    ):
        print(f"Using cached {name}.")
        return dst_path

    response = requests.get(url)

    if zip_path:
        unzip_file(response.content, zip_path, dst_path)
    else:
        with open(dst_path, "wb") as fh:
            fh.write(response.content)

    return dst_path


def download_files_to_cache(urls_and_names):
    for url, name in urls_and_names.items():
        download_file_to_cache(url, name)


def unzip_file(zip_content, zip_path, dst_path):
    zip_data = io.BytesIO(zip_content)

    with zipfile.ZipFile(zip_data, "r") as zipfh:
        file_data = zipfh.read(zip_path)

    with open(dst_path, "wb") as fh:
        fh.write(file_data)


def convert_uf2_to_bin(file):
    with open(file, "rb") as fh:
        inputbuf = fh.read()

    outputbuf = uf2conv.convert_from_uf2(inputbuf)

    with open(file + ".bin", "wb") as fh:
        fh.write(outputbuf)


def run_jlink(device, script):
    subprocess.check_call(
        [
            JLINK_PATH,
            "-device",
            "ATSAMD51J20",
            "-autoconnect",
            "1",
            "-if",
            "SWD",
            "-speed",
            "4000",
            "-CommanderScript",
            script,
        ]
    )


def removeprefix(self: str, prefix: str) -> str:
    if self.startswith(prefix):
        return self[len(prefix) :]
    else:
        return self[:]
