import subprocess
import os
import sys
import time
import signal
import argparse
import shutil
import threading

processes = []

def check_dependencies():
    print(f"[*] Ensuring dependency installation for Pyroxene v2...")
    subprocess.run(["python3", "install.py"])

def get_logger_error_level(error_level_arg):
    error_level_set = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"}
    if error_level_arg in error_level_set:
        return error_level_arg
    else:
        return "ERROR"

def build_logger_foundry(clean_severity):
    logger_foundry_path = os.path.join(os.getcwd(), "logger_foundry")
    build_logger_foundry_script = os.path.join(logger_foundry_path, "build_logger_foundry.py")
    if clean_severity[0]:
        subprocess.run(["python3", build_logger_foundry_script, "--cmake-prefix", os.environ["LOGGER_FOUNDRY_INSTALL_PATH"], "--clean", clean_severity[1]], cwd=logger_foundry_path, check=True)
    else:
        subprocess.run(["python3", build_logger_foundry_script, "--cmake-prefix", os.environ["LOGGER_FOUNDRY_INSTALL_PATH"]], cwd=logger_foundry_path, check=True)
    # VERIFY THIS LATER
    install_prefix = os.path.join(os.getcwd(), "lib")
    return install_prefix

def clean_logger_foundry(clean): 
    if (clean):
        shutil.rmtree("logger_foundry_lib", ignore_errors=True)
        os.makedirs("logger_foundry_lib", exist_ok=True)

def cleanup_background_processes():
    for process in processes:
        if process.poll() is None:
            #print(f"Killing process: {process.pid}")
            try:
                os.killpg(os.getpgid(process.pid), signal.SIGTERM)
            except:
                pass
    shutil.rmtree("tmp", ignore_errors=True)
    #subprocess.run(["rm", "-rf", "tmp"])
    #subprocess.run(["rm", "-rf", "logger_foundry_lib"])

def print_active_processes():
    for process in processes:
        if process.poll() is None:
            print(f"Process {process.pid} is active.")
        else:
            print(f"Process {process.pid} exited with code {process.poll()}")

def wait_for_socket(socket_path, timeout=30):
    #print(f"Waiting on socket: {socket_path}")
    start = time.time()
    while not os.path.exists(socket_path):
        if time.time() - start > timeout:
            raise TimeoutError(f"Timeout - Waiting for socket: {socket_path}")
        time.sleep(0.01)

def write_pids(process_list):
    process_string = ":".join(str(process.pid) for process in process_list)
    with open("tmp/pids.txt", "w") as f:
        f.write(process_string)

def env_files_setup():
    #subprocess.run(["rm", "-rf", "logs"])
    subprocess.run(["rm", "-rf", "tmp"])
    os.makedirs("logs", exist_ok=True)
    os.makedirs("tmp", exist_ok=True)
    os.makedirs("logger_foundry_lib", exist_ok=True)
    os.chmod("tmp", 0o755)

def init_env_vars():
    os.environ["PYROXENE_LOG_PATH"] = os.path.join(os.getcwd(), "logs", "pyroxene.log")
    os.environ["PYROXENE_LOG_SOCKET_PATH"] = os.path.join(os.getcwd(), "tmp", "logger_daemon.sock")
    os.environ["FRONTEND_SOCKET_PATH"] = os.path.join(os.getcwd(), "tmp", "frontend_pyroxene.sock")
    os.environ["BACKEND_SOCKET_PATH"] = os.path.join(os.getcwd(), "tmp", "backend_pyroxene.sock")
    os.environ["PID_PATH"] = os.path.join(os.getcwd(), "tmp", "pids.txt")
    os.environ["LOGGER_FOUNDRY_INSTALL_PATH"] = os.path.join(os.getcwd(), "logger_foundry_lib")
    os.environ["LOGGER_FOUNDRY_INSTALL_PATH_LIB"] = os.path.join(os.getcwd(), "logger_foundry_lib/lib")

def dispatch_frontend(clean):
    if(clean[0]):
        frontend_proc = subprocess.Popen(["python3", "frontend_run.py", "--clean"], cwd="frontend-haskell", preexec_fn=os.setsid)
    else:
        frontend_proc = subprocess.Popen(["python3", "frontend_run.py"], cwd="frontend-haskell", preexec_fn=os.setsid)

    processes.append(frontend_proc)

def dispatch_middlend(clean):
    if(clean[0]):
         middle_proc = subprocess.Popen(["python3", "middle_end_run.py", "--clean"], cwd="middle_end_rs", preexec_fn=os.setsid)
    else:
        middle_proc = subprocess.Popen(["python3", "middle_end_run.py"], cwd="middle_end_rs", preexec_fn=os.setsid)
    
    processes.append(middle_proc)
    wait_for_socket("tmp/frontend_pyroxene.sock")

def dispatch_backend(clean):
    if (clean[0]):
        backend_proc = subprocess.Popen(["python3", "backend_run.py", "--clean", clean[1]], cwd="backend_llvm_cpp", preexec_fn=os.setsid)
    else:
        backend_proc = subprocess.Popen(["python3", "backend_run.py"], cwd="backend_llvm_cpp", preexec_fn=os.setsid)

    processes.append(backend_proc)
    wait_for_socket("tmp/backend_pyroxene.sock")

def check_compile_and_run_logger_foundry(cmd_line_arguments):
    if cmd_line_arguments.log:
        clean_severity = check_cmd_line_arg_clean_and_return_severity(cmd_line_arguments.clean)

        os.environ["PYROXENE_LOG"] = "1"
        os.environ["LOG_FOUNDRY_LIB_PATH"] = build_logger_foundry(clean_severity)

        os.environ["ERROR_LEVEL"] = get_logger_error_level(cmd_line_arguments.log_level)

        #open(os.environ["PYROXENE_LOG_PATH"], "w").close()
        

        if (clean_severity[0]):
            logger_proc = subprocess.Popen(["python3", "build_pyroxene_daemon.py", "--clean", clean_severity[1]], cwd="logger_daemon_pyroxene", preexec_fn=os.setsid)
        else:
            logger_proc = subprocess.Popen(["python3", "build_pyroxene_daemon.py"], cwd="logger_daemon_pyroxene", preexec_fn=os.setsid)
        wait_for_socket("tmp/logger_daemon.sock")
        return logger_proc
    else:
        os.environ["PYROXENE_LOG"] = "0"
        return None

def parse_cmd_line_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", action="store_true")
    parser.add_argument("--log-level", nargs="?", const="ERROR", type=str)
    parser.add_argument("--clean", nargs="?", const="soft", type=str, choices=["soft", "hard"])
    parser.add_argument("--ci", nargs="?", const=30, type=int)
    cmd_line_arguments = parser.parse_args()
    return cmd_line_arguments

def check_if_logger_proc_exists_and_append_to_procs(logger_proc):
    if os.getenv("PYROXENE_LOG") == "1":
        processes.append(logger_proc)

def check_cmd_line_arg_clean_and_return_severity(clean):
    if (clean):
        return (True, clean)
    return (False, None)


def run_ci_timeout_thread(processes, timeout):
    def monitor():
        start_time = time.time();   
        while time.time() - start_time < timeout:
            all_finished = all(p.poll() is not None for p in processes)
            if all_finished:
                return
            time.sleep(1)

        print(f"[!] CI Process Timeout Reached. Terminating All Processes.")
        cleanup_background_processes()
    
    threading.Thread(target=monitor, daemon=True).start()


def init_env():
    check_dependencies()

    cmd_line_arguments = parse_cmd_line_args()

    clean_logger_foundry(cmd_line_arguments.clean)

    env_files_setup()
    init_env_vars()

    logger_proc = check_compile_and_run_logger_foundry(cmd_line_arguments)
        
    clean_arg = check_cmd_line_arg_clean_and_return_severity(cmd_line_arguments.clean)

    
    dispatch_backend(clean_arg)
    dispatch_middlend(clean_arg)
    dispatch_frontend(clean_arg)

    write_pids(processes)

    check_if_logger_proc_exists_and_append_to_procs(logger_proc)
    
    
    if (cmd_line_arguments.ci):
        run_ci_timeout_thread(processes, cmd_line_arguments.ci)
    



def signal_handler(signal, frame):
    print(f"\nReceived signal {signal}. Terminating all processes.")
    cleanup_background_processes()
    sys.exit(1)

def main():

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    try:
        init_env()
        for process in processes:
            process.wait()
    except Exception as excp:
        print(f"Error: {excp}")
        cleanup_background_processes()
        sys.exit(1)

    cleanup_background_processes()
    sys.exit(0)

main()
