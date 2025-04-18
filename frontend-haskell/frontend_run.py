import os
import subprocess

def main():
    os.makedirs("build", exist_ok=True)

    with open(os.devnull, 'w') as devnull:
        subprocess.run(["stack", "build", "--verbosity", "warn"], stdout=devnull, check=True)
        subprocess.run(["stack", "exec", "frontend-haskell"],  check=True)

main()