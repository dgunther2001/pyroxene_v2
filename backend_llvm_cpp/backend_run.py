import argparse
import os
import subprocess
import shutil

def parse_cmd_line_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--clean", nargs="?", const="soft", type=str, choices=["soft", "hard"])
    cmd_line_arguments = parser.parse_args()
    return cmd_line_arguments

def main():
    os.makedirs("build", exist_ok=True)

    with open(os.devnull, 'w') as devnull:

        cmd_args = parse_cmd_line_args()

        if cmd_args.clean == "soft":
            subprocess.run(["make", "clean"], cwd="build", stdout=devnull, check=True)

        if cmd_args.clean == "hard":
            shutil.rmtree("build", ignore_errors=True)
            shutil.rmtree(".cache", ignore_errors=True)
            subprocess.run(["mkdir", "build"])

        subprocess.run(["cmake", ".."], stdout=devnull, cwd="build", check=True)
        subprocess.run(["make"], cwd="build", stdout=devnull, check=True)
        subprocess.run(["./driver"], cwd="build", check=True)


main()