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

    cmake_prefix_path = os.environ["LOGGER_FOUNDRY_INSTALL_PATH_LIB"]

    with open(os.devnull, 'w') as devnull:

        cmd_args = parse_cmd_line_args()

        if cmd_args.clean == "soft":
            subprocess.run(["make", "clean"], cwd="build", stdout=devnull, check=True)

        if cmd_args.clean == "hard":
            shutil.rmtree("build", ignore_errors=True)
            shutil.rmtree(".cache", ignore_errors=True)
            subprocess.run(["mkdir", "build"])

        os.environ["LD_LIBRARY_PATH"] = cmake_prefix_path

        env = os.environ.copy()
        subprocess.run(["cmake", "..", f"-DCMAKE_INSTALL_PREFIX={cmake_prefix_path}"], stdout=devnull, cwd="build", check=True)
        subprocess.run(["make"], cwd="build", stdout=devnull, check=True)
        subprocess.run(["./driver"], cwd="build", check=True, env=env)

main()