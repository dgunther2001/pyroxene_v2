import os
import subprocess
import argparse
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

        if cmd_args.clean:
            lockfile = "Cargo.lock"
            if os.path.isfile(lockfile):
                os.remove(lockfile)
                
            subprocess.run(["cargo", "update"], stdout=devnull, check=True)
            subprocess.run(["cargo", "clean", "--quiet"], stdout=devnull, check=True)

        subprocess.run(["cargo", "build", "--quiet"], stdout=devnull, check=True)
        subprocess.run(["cargo", "run", "--quiet"], check=True)

main()