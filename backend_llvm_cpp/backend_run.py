import os
import subprocess

def main():
    os.makedirs("build", exist_ok=True)

    with open(os.devnull, 'w') as devnull:
        subprocess.run(["cmake", ".."], stdout=devnull, cwd="build", check=True)
        subprocess.run(["make"], cwd="build", stdout=devnull, check=True)
        subprocess.run(["./driver"], cwd="build", check=True)

main()