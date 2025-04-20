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
            subprocess.run(["stack", "clean"], stdout=devnull, check=True)
            #subprocess.run(["stack", "setup"],  check=True)
            #stdout=devnull, stderr=devnull,
        
        # , "--verbosity", "warn"
        subprocess.run(["stack", "build", "--verbosity", "error"], stdout=devnull, check=True)
        subprocess.run(["stack", "exec", "frontend-haskell"],  check=True)

main()