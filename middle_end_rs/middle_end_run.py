import os
import subprocess
import argparse

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
            subprocess.run(["cargo", "clean", "--quiet"], stdout=devnull, check=True)

        subprocess.run(["cargo", "build", "--quiet"], stdout=devnull, check=True)
        subprocess.run(["cargo", "run", "--quiet"], check=True)

main()