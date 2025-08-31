import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext, ttk
import datetime
import queue
import wrapper as aw
from helper import AsyncLoopThread


FIRMWARE_VERSION = "V0.0.3"


class ConfigToolApp:
    class State:
        def __init__(self):
            self.connected = False
            self.loaded = False
            self.uploaded = False
            self.verified = False
            self.saved = False
            self.cmd_loaded = False

    def __init__(self, root):
        global FIRMWARE_VERSION
        self.root = root
        self.root.title(f"Config Tool - {FIRMWARE_VERSION}")
        self.root.geometry("800x800")
        self.root.minsize(600, 600)

        # Queue for async results
        self.loop = AsyncLoopThread()
        self.result_queue = queue.Queue()
        self.state = self.State()
        aw.register_print_callback(self.print_callback)

        # --- Responsive grid ---
        for col in range(4):
            self.root.grid_columnconfigure(col, weight=(1 if col == 1 else 0))
        self.root.grid_rowconfigure(7, weight=1)

        # Connect section
        self.connect_btn = tk.Button(root, text="Connect", width=12, command=self.toggle_connection)
        self.connect_btn.grid(row=0, column=0, padx=5, pady=10, sticky="w")

        self.info_btn = tk.Button(root, text="Info", width=12, command=self.get_info, state="disabled")
        self.info_btn.grid(row=0, column=1, padx=5, pady=10, sticky="w")

        # OTA section
        ttk.Separator(root, orient="horizontal").grid(row=1, column=0, columnspan=4, sticky="ew", pady=10)

        self.ota_file_path_var = tk.StringVar()
        tk.Label(root, text="Firmware File:").grid(row=2, column=0, padx=5, pady=5, sticky="w")

        self.ota_file_entry = tk.Entry(root, textvariable=self.ota_file_path_var)
        self.ota_file_entry.grid(row=2, column=1, padx=5, pady=5, sticky="ew")

        self.ota_browse_btn = tk.Button(root, text="Browse", command=self.browse_ota_file)
        self.ota_browse_btn.grid(row=2, column=2, padx=5, pady=5, sticky="ew")

        self.ota_load_btn = tk.Button(root, text="Update", command=self.ota_update, state="disabled")
        self.ota_load_btn.grid(row=2, column=3, padx=5, pady=5, sticky="ew")

        # Config section
        ttk.Separator(root, orient="horizontal").grid(row=3, column=0, columnspan=4, sticky="ew", pady=10)

        self.config_file_path_var = tk.StringVar()
        tk.Label(root, text="Config File:").grid(row=4, column=0, padx=5, pady=5, sticky="w")

        self.file_entry = tk.Entry(root, textvariable=self.config_file_path_var)
        self.file_entry.grid(row=4, column=1, padx=5, pady=5, sticky="ew")

        self.browse_btn = tk.Button(root, text="Browse", command=self.browse_config_file)
        self.browse_btn.grid(row=4, column=2, padx=5, pady=5, sticky="ew")

        self.load_btn = tk.Button(root, text="Load", command=self.load_config)
        self.load_btn.grid(row=4, column=3, padx=5, pady=5, sticky="ew")

        self.upload_btn = tk.Button(root, text="Upload", width=12, command=self.upload_config, state="disabled")
        self.upload_btn.grid(row=5, column=2, padx=5, pady=10, sticky="e")

        self.verify_btn = tk.Button(root, text="Verify", width=12, command=self.verify_config, state="disabled")
        self.verify_btn.grid(row=5, column=3, padx=5, pady=10, sticky="e")

        self.save_btn = tk.Button(root, text="Save", width=12, command=self.save_config, state="disabled")
        self.save_btn.grid(row=6, column=3, padx=5, pady=10, sticky="e")

        # Log section
        tk.Label(root, text="Log:").grid(row=6, column=0, sticky="w", padx=5)

        self.log_area = scrolledtext.ScrolledText(root, state="disabled", wrap="word", font=("Courier New", 10))
        self.log_area.grid(row=7, column=0, columnspan=4, padx=5, pady=5, sticky="nsew")

        self.clear_btn = tk.Button(root, text="Clear Log", width=12, command=self.clear_log, state="normal")
        self.clear_btn.grid(row=9, column=0, padx=5, pady=10, sticky="w")

        # Command section
        self.cmd_file_path_var = tk.StringVar()
        tk.Label(root, text="Command File:").grid(row=10, column=0, padx=5, pady=5, sticky="w")

        self.cmd_file_entry = tk.Entry(root, textvariable=self.cmd_file_path_var)
        self.cmd_file_entry.grid(row=10, column=1, padx=5, pady=5, sticky="ew")

        self.cmd_browse_btn = tk.Button(root, text="Browse", command=self.browse_cmd_file)
        self.cmd_browse_btn.grid(row=10, column=2, padx=5, pady=5, sticky="ew")

        self.cmd_load_btn = tk.Button(root, text="Load", command=self.load_cmd)
        self.cmd_load_btn.grid(row=10, column=3, padx=5, pady=5, sticky="ew")

        self.cmd_send_btn = tk.Button(root, text="Send", width=12, command=self.send_cmd, state="disabled")
        self.cmd_send_btn.grid(row=11, column=2, padx=5, pady=10, sticky="e")

        self.stop_show_btn = tk.Button(root, text="Stop Show", width=12, command=self.stop_show, state="disabled")
        self.stop_show_btn.grid(row=11, column=3, padx=5, pady=10, sticky="e")

        # Periodically check result queue
        self.root.after(100, self.process_queue)

    def update_buttons(self, action_ongoing: bool = False):
        if action_ongoing:
            # Disable all buttons during ongoing action
            self.connect_btn.config(state="disabled")
            self.info_btn.config(state="disabled")
            # OTA buttons
            self.ota_browse_btn.config(state="disabled")
            self.ota_load_btn.config(state="disabled")
            # Config buttons
            self.browse_btn.config(state="disabled")
            self.load_btn.config(state="disabled")
            self.upload_btn.config(state="disabled")
            self.verify_btn.config(state="disabled")
            self.save_btn.config(state="disabled")
            # Command buttons
            self.cmd_browse_btn.config(state="disabled")
            self.cmd_load_btn.config(state="disabled")
            self.cmd_send_btn.config(state="disabled")
            self.stop_show_btn.config(state="disabled")
            return

        self.browse_btn.config(state="normal")
        self.load_btn.config(state="normal")
        self.cmd_browse_btn.config(state="normal")
        self.cmd_load_btn.config(state="normal")
        self.ota_browse_btn.config(state="normal")

        if self.state.connected:
            self.connect_btn.config(state="normal", text="Disconnect")  # Connected => allow disconnect
            self.info_btn.config(state="normal")  # Allow getting info when connected
            self.stop_show_btn.config(state="normal")  # Stop show always enabled when connected
            self.ota_load_btn.config(state="normal")  # OTA always enabled when connected

            if self.state.loaded:
                self.upload_btn.config(state="normal")
                self.verify_btn.config(state="normal")

                if self.state.verified:
                    self.save_btn.config(state="normal")

            if self.state.cmd_loaded:
                self.cmd_send_btn.config(state="normal")
        else:
            self.connect_btn.config(state="normal", text="Connect")

    # --- Logging ---
    def log(self, message, add_prefix: bool = True):
        timestamp = datetime.datetime.now().strftime("%H:%M:%S")
        self.log_area.config(state="normal")
        if add_prefix:
            self.log_area.insert(tk.END, f"[{timestamp}][ConfigToolApp] {message}\n")
        else:
            self.log_area.insert(tk.END, f"{message}\n")
        self.log_area.see(tk.END)
        self.log_area.config(state="disabled")

    # --- GUI Actions ---
    def browse_config_file(self):
        filename = filedialog.askopenfilename(title="Select Config File", filetypes=[("Config file", "*.json")])
        if filename:
            self.config_file_path_var.set(filename)
            self.log(f"Selected file: '{filename}'")

    def browse_cmd_file(self):
        filename = filedialog.askopenfilename(title="Select Command File", filetypes=[("Command file", "*.json")])
        if filename:
            self.cmd_file_path_var.set(filename)
            self.log(f"Selected command file: '{filename}'")

    def browse_ota_file(self):
        filename = filedialog.askopenfilename(title="Select Firmware File", filetypes=[("Binary file", "*.bin")])
        if filename:
            self.ota_file_path_var.set(filename)
            self.log(f"Selected firmware file: '{filename}'")

    def toggle_connection(self):
        if not self.state.connected:
            fut = self.loop.run_coro(aw.async_connect())
            fut.add_done_callback(lambda f: self.result_queue.put(("connect", f.result())))
            self.update_buttons(action_ongoing=True)
            self.log("Connecting ...")
        else:
            fut = self.loop.run_coro(aw.async_disconnect())
            fut.add_done_callback(lambda f: self.result_queue.put(("disconnect", f.result())))
            self.update_buttons(action_ongoing=True)
            self.log("Disconnecting ...")

    def load_config(self):
        filepath = self.config_file_path_var.get()
        if not filepath:
            messagebox.showwarning("Warning", "Please select a file first.")
            return
        fut = self.loop.run_coro(aw.async_load_config(filepath))
        fut.add_done_callback(lambda f: self.result_queue.put(("load_config", f.result())))
        self.update_buttons(action_ongoing=True)

    def get_info(self):
        fut = self.loop.run_coro(aw.async_get_info())
        fut.add_done_callback(lambda f: self.result_queue.put(("get_info", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Getting device info ...")

    def upload_config(self):
        fut = self.loop.run_coro(aw.async_upload_config())
        fut.add_done_callback(lambda f: self.result_queue.put(("upload_config", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Upload started ...")

    def verify_config(self):
        fut = self.loop.run_coro(aw.async_verify_config())
        fut.add_done_callback(lambda f: self.result_queue.put(("verify_config", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Verification started ...")

    def save_config(self):
        fut = self.loop.run_coro(aw.async_save_config())
        fut.add_done_callback(lambda f: self.result_queue.put(("save_config", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Saving calibration ...")

    def clear_log(self):
        self.log_area.config(state="normal")
        self.log_area.delete(1.0, tk.END)
        self.log_area.config(state="disabled")

    def load_cmd(self):
        filepath = self.cmd_file_path_var.get()
        if not filepath:
            messagebox.showwarning("Warning", "Please select a sequence file first.")
            return
        fut = self.loop.run_coro(aw.async_load_cmd(filepath))
        fut.add_done_callback(lambda f: self.result_queue.put(("load_cmd", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Loading command file ...")

    def send_cmd(self):
        fut = self.loop.run_coro(aw.async_send_cmd())
        fut.add_done_callback(lambda f: self.result_queue.put(("send_cmd", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Sending command ...")

    def stop_show(self):
        fut = self.loop.run_coro(aw.async_stop_show())
        fut.add_done_callback(lambda f: self.result_queue.put(("stop_show", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Stopping sequence ...")

    def ota_update(self):
        filepath = self.ota_file_path_var.get()
        if not filepath:
            messagebox.showwarning("Warning", "Please select a firmware file first.")
            return
        fut = self.loop.run_coro(aw.async_ota_update(filepath))
        fut.add_done_callback(lambda f: self.result_queue.put(("ota_update", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Starting OTA update ...")

    def print_callback(self, message: str):
        self.result_queue.put(("print", message))

    # --- Async result handling ---
    def process_queue(self):
        """Check for results from async tasks"""
        try:
            while True:
                tag, result = self.result_queue.get_nowait()
                status, message = result if isinstance(result, tuple) else (False, result)
                update_buttons = True
                if tag == "connect":
                    self.state.connected = status
                elif tag == "disconnect":
                    self.state.connected = False
                elif tag == "get_info":
                    pass
                elif tag == "load_config":
                    self.state.loaded = status
                    self.state.uploaded = False
                    self.state.verified = False
                elif tag == "upload_config":
                    self.state.uploaded = status
                elif tag == "verify_config":
                    self.state.verified = status
                elif tag == "save_config":
                    self.state.saved = status
                elif tag == "load_cmd":
                    self.state.cmd_loaded = status
                elif tag == "send_cmd":
                    pass
                elif tag == "stop_show":
                    pass
                elif tag == "ota_update":
                    pass
                elif tag == "print":
                    update_buttons = False  # Just a log message, no button update
                else:
                    raise ValueError(f"Unhandled tag in result queue: {tag}")

                if update_buttons:
                    self.update_buttons(action_ongoing=False)
                self.log(message, add_prefix=False)
        except queue.Empty:
            pass
        self.root.after(100, self.process_queue)


if __name__ == "__main__":
    root = tk.Tk()
    app = ConfigToolApp(root)
    try:
        root.mainloop()
    finally:
        app.loop.stop()  # Clean shutdown of background loop
