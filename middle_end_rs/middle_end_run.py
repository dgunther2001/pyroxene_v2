import os
import subprocess

def main():
    os.makedirs("build", exist_ok=True)

    with open(os.devnull, 'w') as devnull:
        subprocess.run(["cargo", "build", "--quiet"], stdout=devnull, check=True)
        subprocess.run(["cargo", "run", "--quiet"], check=True)

main()