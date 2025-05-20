import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import threading
import queue
import time
import os

class EncryptionGUI:
    def __init__(self, root):
        self.root = root
        root.title("File Encryption/Decryption")

        # Directory selection
        tk.Label(root, text="Select Directory:").grid(row=0, column=0, sticky='w', padx=10, pady=5)
        self.dir_path_var = tk.StringVar()
        tk.Entry(root, textvariable=self.dir_path_var, width=50).grid(row=0, column=1, padx=10)
        tk.Button(root, text="Browse", command=self.browse_directory).grid(row=0, column=2, padx=10)

        # Algorithm selection
        tk.Label(root, text="Select Algorithm:").grid(row=1, column=0, sticky='w', padx=10, pady=5)
        self.algorithm_var = tk.StringVar(value="caesar")
        algo_options = ["caesar", "rsa", "xor"]
        tk.OptionMenu(root, self.algorithm_var, *algo_options).grid(row=1, column=1, sticky='w', padx=10)

        # Action selection
        tk.Label(root, text="Action:").grid(row=2, column=0, sticky='w', padx=10, pady=5)
        self.action_var = tk.StringVar(value="encrypt")
        tk.Radiobutton(root, text="Encrypt", variable=self.action_var, value="encrypt").grid(row=2, column=1, sticky='w', padx=10)
        tk.Radiobutton(root, text="Decrypt", variable=self.action_var, value="decrypt").grid(row=2, column=1, sticky='e', padx=10)

        # Start button
        tk.Button(root, text="Start", command=self.start_process).grid(row=3, column=1, pady=20)

        # Status box
        self.status_text = tk.Text(root, height=8, width=60, state='disabled')
        self.status_text.grid(row=4, column=0, columnspan=3, padx=10, pady=10)

    def browse_directory(self):
        directory = filedialog.askdirectory()
        if directory:
            self.dir_path_var.set(directory)

    def start_process(self):
        directory = self.dir_path_var.get()
        algorithm = self.algorithm_var.get()
        action = self.action_var.get()

        if not directory:
            messagebox.showerror("Error", "Please select a directory.")
            return

        self.set_ui_state('disabled')
        self.append_status(f"Starting {action}ion with {algorithm} on: {directory}\n")

        threading.Thread(target=self.run_cryption_process, args=(directory, algorithm, action), daemon=True).start()

    def run_cryption_process(self, directory, algorithm, action):
        try:
            cryption_path = '/mnt/c/VIDIT/Operating_System_PROJECT/cryption'
            if not os.path.exists(cryption_path):
                self.append_status(f"ERROR: Executable not found at {cryption_path}\n")
                self.set_ui_state('normal')
                return

            cmd = [cryption_path, directory, algorithm, action]
            self.append_status(f"Running command: {' '.join(cmd)}\n")

            process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

            stdout_queue = queue.Queue()
            stderr_queue = queue.Queue()

            def enqueue_output(pipe, q):
                for line in iter(pipe.readline, ''):
                    q.put(line)
                pipe.close()

            t_out = threading.Thread(target=enqueue_output, args=(process.stdout, stdout_queue))
            t_err = threading.Thread(target=enqueue_output, args=(process.stderr, stderr_queue))
            t_out.start()
            t_err.start()

            while True:
                try:
                    while True:
                        line = stdout_queue.get_nowait()
                        self.append_status(line)
                except queue.Empty:
                    pass

                try:
                    while True:
                        line = stderr_queue.get_nowait()
                        self.append_status("ERR: " + line)
                except queue.Empty:
                    pass

                if process.poll() is not None:
                    break

                time.sleep(0.1)

            t_out.join()
            t_err.join()

            retcode = process.poll()
            if retcode == 0:
                self.append_status("Process completed successfully.\n")
            else:
                self.append_status(f"Process exited with code {retcode}\n")

        except Exception as e:
            self.append_status(f"Error: {e}\n")

        self.set_ui_state('normal')

    def append_status(self, message):
        self.status_text.config(state='normal')
        self.status_text.insert(tk.END, message)
        self.status_text.see(tk.END)
        self.status_text.config(state='disabled')

    def set_ui_state(self, state):
        for child in self.root.winfo_children():
            if isinstance(child, (tk.Button, tk.Entry, tk.OptionMenu, tk.Radiobutton)):
                child.config(state=state)

if __name__ == "__main__":
    root = tk.Tk()
    gui = EncryptionGUI(root)
    root.mainloop()
