import os
import subprocess

def main():
    os.makedirs("build", exist_ok=True)

    cmake_prefix_path = os.environ["LOGGER_FOUNDRY_INSTALL_PATH_LIB"]

    with open(os.devnull, 'w') as devnull:
        subprocess.run(["cmake", "..", f"-DCMAKE_INSTALL_PREFIX={cmake_prefix_path}"], stdout=devnull, cwd="build", check=True)
        subprocess.run(["make"], cwd="build", stdout=devnull, check=True)
        subprocess.run(["./driver"], cwd="build", check=True)

main()