import subprocess
import os
import sys
import time
import signal

processes = []

def cleanup_background_processes():
    for process in processes:
        if process.poll() is None:
            #print(f"Killing process: {process.pid}")
            process.terminate()
            try:
                process.wait(timeout=2)
            except:
                process.kill()
    subprocess.run(["rm", "-rf", "tmp"])

def print_active_processes():
    for process in processes:
        if process.poll() is None:
            print(f"Process {process.pid} is active.")
        else:
            print(f"Process {process.pid} exited with code {process.poll()}")

def wait_for_socket(socket_path, timeout=5):
    #print(f"Waiting on socket: {socket_path}")
    start = time.time()
    while not os.path.exists(socket_path):
        if time.time() - start > timeout:
            raise TimeoutError(f"Timeout - Waiting for socket: {socket_path}")
        time.sleep(0.01)

def init_env():
    subprocess.run(["rm", "-rf", "logs"])
    subprocess.run(["rm", "-rf", "tmp"])
    os.makedirs("logs", exist_ok=True)
    os.makedirs("tmp", exist_ok=True)
    os.chmod("tmp", 0o755)

    os.environ["PYROXENE_LOG_PATH"] = os.path.join(os.getcwd(), "logs", "pyroxene.log")
    os.environ["PYROXENE_LOG_SOCKET_PATH"] = os.path.join(os.getcwd(), "tmp", "logger_daemon.sock")

    os.environ["FRONTEND_SOCKET_PATH"] = os.path.join(os.getcwd(), "tmp", "frontend_pyroxene.sock")
    os.environ["BACKEND_SOCKET_PATH"] = os.path.join(os.getcwd(), "tmp", "backend_pyroxene.sock")

    if "--log" in sys.argv:
        os.environ["PYROXENE_LOG"] = "1"
        open(os.environ["PYROXENE_LOG_PATH"], "w").close()
    else:
        os.environ["PYROXENE_LOG"] = "0"

    logger_proc = subprocess.Popen(["./run_logger.sh"], cwd="logger_daemon")
    processes.append(logger_proc)
    backend_proc = subprocess.Popen(["./backend_run.sh"], cwd="backend_llvm_cpp")
    processes.append(backend_proc)
    wait_for_socket("tmp/backend_pyroxene.sock")
    middle_proc = subprocess.Popen(["./middle_end_run.sh"], cwd="middle_end_rs")
    processes.append(middle_proc)
    wait_for_socket("tmp/frontend_pyroxene.sock")
    frontend_proc = subprocess.Popen(["./frontend_hs.sh"], cwd="frontend-haskell")
    processes.append(frontend_proc)

    return frontend_proc

def signal_handler(signal, frame):
    print(f"\nReceived signal {signal}. Terminating all processes.")
    cleanup_background_processes()
    sys.exit(1)

def main():
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    try:
        frontend_process = init_env()
        frontend_process.wait()
    except Exception as excp:
        print(f"Error: {excp}")
        cleanup_background_processes()
        sys.exit(1)

    cleanup_background_processes()
    sys.exit(0)

main()
