import subprocess
import time

FER = [0, 1, 5, 10]
TPROP = [0, 150000, 500000, 750000]
INFOSIZE = [256, 512, 1024, 2048]

def main():
    for tprop in TPROP:
        for fer in FER:
            for infosize in INFOSIZE:
                subprocess.run(["make", "efficiency", f"FER={fer}", f"T_PROP={tprop/1000}", f"MAX_INFO_SIZE={infosize}"], stdout=subprocess.DEVNULL)
                p1 = subprocess.Popen(["./build/application", "1", "10"], stdout=subprocess.DEVNULL)
                p2 = subprocess.Popen(["./build/application", "0", "11", "./testing/pinguim.gif"], stdout=subprocess.DEVNULL)

                start = time.time()
                p1.wait(); p2.wait()
                total = time.time() - start

                if p1.returncode == 0 and p2.returncode == 0:
                    print(f"Time of execution for FER={fer}, T_PROP={tprop}, MAX_INFO_SIZE={infosize}: {total}", flush=True)
                else:
                    print("Error", flush=True)
                
                input()

if __name__ == "__main__":
    main()
