import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext
import datetime
import asyncio
import threading
import queue
from ConfigTool import ConfigTool
import sys
import io


FIRMWARE_VERSION = "V0.0.1"
CONFIG_TOOL = ConfigTool()


class PrintBuffer:
    def __enter__(self):
        self._buffer = io.StringIO()
        self._old_stdout = sys.stdout
        sys.stdout = self._buffer
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        sys.stdout = self._old_stdout

    def get_value(self):
        return self._buffer.getvalue()


async def async_load(file_path):
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.load(file_path)
        except Exception as e:
            print(f"Error loading file: {e}")
    return status, buf.get_value()


async def async_connect():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.connect()
        except Exception as e:
            print(f"Error connecting to device: {e}")
    return status, buf.get_value()


async def async_get_info():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            info = await CONFIG_TOOL.get_info()
            status = info is not None
        except Exception as e:
            print(f"Error getting device info: {e}")
    return status, buf.get_value()


async def async_disconnect():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.disconnect()
        except Exception as e:
            print(f"Error disconnecting from device: {e}")
    return status, buf.get_value()


async def async_upload():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.upload()
        except Exception as e:
            print(f"Error uploading config: {e}")
    return status, buf.get_value()


async def async_verify():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.verify()
        except Exception as e:
            print(f"Error verifying config: {e}")
    return status, buf.get_value()


async def async_save():
    global CONFIG_TOOL
    status = False
    with PrintBuffer() as buf:
        try:
            status = await CONFIG_TOOL.save()
        except Exception as e:
            print(f"Error saving config: {e}")
    return status, buf.get_value()


class AsyncLoopThread:
    def __init__(self):
        self.loop = asyncio.new_event_loop()
        self.thread = threading.Thread(target=self._run_loop, daemon=True)
        self.thread.start()

    def _run_loop(self):
        asyncio.set_event_loop(self.loop)
        self.loop.run_forever()

    def run_coro(self, coro):
        """Submit a coroutine to the background loop"""
        return asyncio.run_coroutine_threadsafe(coro, self.loop)

    def stop(self):
        self.loop.call_soon_threadsafe(self.loop.stop)


class ConfigApp:
    class State:
        def __init__(self, connected=False, loaded=False, uploaded=False, verified=False, saved=False):
            self.connected = connected
            self.loaded = loaded
            self.uploaded = uploaded
            self.verified = verified
            self.saved = saved

    def __init__(self, root):
        global FIRMWARE_VERSION
        self.root = root
        self.root.title(f"Config Tool - {FIRMWARE_VERSION}")
        self.root.geometry("700x500")
        self.root.minsize(600, 300)

        # Queue for async results
        self.loop = AsyncLoopThread()
        self.result_queue = queue.Queue()

        # --- Responsive grid ---
        for col in range(4):
            self.root.grid_columnconfigure(col, weight=(1 if col == 1 else 0))
        self.root.grid_rowconfigure(3, weight=1)

        # --- File selection row ---
        self.file_path_var = tk.StringVar()
        tk.Label(root, text="Config File:").grid(row=0, column=0, padx=5, pady=5, sticky="w")

        self.file_entry = tk.Entry(root, textvariable=self.file_path_var)
        self.file_entry.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        self.browse_btn = tk.Button(root, text="Browse", command=self.browse_file)
        self.browse_btn.grid(row=0, column=2, padx=5, pady=5, sticky="ew")

        self.load_btn = tk.Button(root, text="Load", command=self.load_file)
        self.load_btn.grid(row=0, column=3, padx=5, pady=5, sticky="ew")

        # --- Action row ---
        self.connect_btn = tk.Button(root, text="Connect", width=12, command=self.toggle_connection)
        self.connect_btn.grid(row=1, column=0, padx=5, pady=10, sticky="w")

        self.info_btn = tk.Button(root, text="Info", width=12, command=self.info, state="disabled")
        self.info_btn.grid(row=1, column=1, padx=5, pady=10, sticky="w")

        self.upload_btn = tk.Button(root, text="Upload", width=12, command=self.upload, state="disabled")
        self.upload_btn.grid(row=1, column=2, padx=5, pady=10, sticky="e")

        self.verify_btn = tk.Button(root, text="Verify", width=12, command=self.verify, state="disabled")
        self.verify_btn.grid(row=1, column=3, padx=5, pady=10, sticky="e")

        self.save_btn = tk.Button(root, text="Save", width=12, command=self.save, state="disabled")
        self.save_btn.grid(row=2, column=3, padx=5, pady=10, sticky="e")

        # --- Log field ---
        tk.Label(root, text="Log:").grid(row=2, column=0, sticky="w", padx=5)

        self.log_area = scrolledtext.ScrolledText(root, state="disabled", wrap="word", font=("Courier New", 10))
        self.log_area.grid(row=3, column=0, columnspan=4, padx=5, pady=5, sticky="nsew")

        self.clear_btn = tk.Button(root, text="Clear", width=12, command=self.clear, state="normal")
        self.clear_btn.grid(row=4, column=3, padx=5, pady=10, sticky="e")

        self.state = self.State()

        # Periodically check result queue
        self.root.after(100, self.process_queue)

    def update_buttons(self, action_ongoing: bool = False):
        if action_ongoing:
            # Disable all buttons during ongoing action
            self.browse_btn.config(state="disabled")
            self.load_btn.config(state="disabled")

            self.connect_btn.config(state="disabled")
            self.info_btn.config(state="disabled")
            self.upload_btn.config(state="disabled")
            self.verify_btn.config(state="disabled")
            self.save_btn.config(state="disabled")
            return

        self.browse_btn.config(state="normal")
        self.load_btn.config(state="normal")

        if self.state.connected:
            self.connect_btn.config(state="normal", text="Disconnect")
            self.info_btn.config(state="normal")  # Always enabled when connected

            if self.state.loaded:
                self.upload_btn.config(state="normal")
                self.verify_btn.config(state="normal")

                if self.state.verified:
                    self.save_btn.config(state="normal")
        else:
            self.connect_btn.config(state="normal", text="Connect")

    # --- Logging ---
    def log(self, message):
        timestamp = datetime.datetime.now().strftime("%H:%M:%S")
        self.log_area.config(state="normal")
        self.log_area.insert(tk.END, f"[{timestamp}] {message}\n")
        self.log_area.see(tk.END)
        self.log_area.config(state="disabled")

    # --- GUI Actions ---
    def browse_file(self):
        filename = filedialog.askopenfilename(title="Select Config File", filetypes=[("Config file", "*.json")])
        if filename:
            self.file_path_var.set(filename)
            self.log(f"Selected file: '{filename}'")

    def toggle_connection(self):
        if not self.state.connected:
            fut = self.loop.run_coro(async_connect())
            fut.add_done_callback(lambda f: self.result_queue.put(("connect", f.result())))
            self.update_buttons(action_ongoing=True)
            self.log("Connecting ...")
        else:
            fut = self.loop.run_coro(async_disconnect())
            fut.add_done_callback(lambda f: self.result_queue.put(("disconnect", f.result())))
            self.update_buttons(action_ongoing=True)
            self.log("Disconnecting ...")

    def load_file(self):
        filepath = self.file_path_var.get()
        if not filepath:
            messagebox.showwarning("Warning", "Please select a file first.")
            return
        fut = self.loop.run_coro(async_load(filepath))
        fut.add_done_callback(lambda f: self.result_queue.put(("load", f.result())))
        self.update_buttons(action_ongoing=True)

    def info(self):
        fut = self.loop.run_coro(async_get_info())
        fut.add_done_callback(lambda f: self.result_queue.put(("info", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Getting device info ...")

    def upload(self):
        fut = self.loop.run_coro(async_upload())
        fut.add_done_callback(lambda f: self.result_queue.put(("upload", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Upload started ...")

    def verify(self):
        fut = self.loop.run_coro(async_verify())
        fut.add_done_callback(lambda f: self.result_queue.put(("verify", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Verification started ...")

    def save(self):
        fut = self.loop.run_coro(async_save())
        fut.add_done_callback(lambda f: self.result_queue.put(("save", f.result())))
        self.update_buttons(action_ongoing=True)
        self.log("Saving calibration ...")

    def clear(self):
        self.log_area.config(state="normal")
        self.log_area.delete(1.0, tk.END)
        self.log_area.config(state="disabled")

    # --- Async result handling ---
    def process_queue(self):
        """Check for results from async tasks"""
        try:
            while True:
                tag, result = self.result_queue.get_nowait()
                status, message = result if isinstance(result, tuple) else (False, result)
                if tag == "connect":
                    self.state.connected = status
                elif tag == "disconnect":
                    self.state.connected = False
                elif tag == "info":
                    pass  # No state change needed
                elif tag == "load":
                    self.state.loaded = status
                    self.state.uploaded = False
                    self.state.verified = False
                elif tag == "upload":
                    self.state.uploaded = status
                elif tag == "verify":
                    self.state.verified = status
                elif tag == "save":
                    self.state.saved = status
                else:
                    raise ValueError(f"Unhandled tag in result queue: {tag}")

                self.update_buttons(action_ongoing=False)
                self.log(message)
        except queue.Empty:
            pass
        self.root.after(100, self.process_queue)


if __name__ == "__main__":
    root = tk.Tk()
    app = ConfigApp(root)
    try:
        root.mainloop()
    finally:
        app.loop.stop()  # Clean shutdown of background loop
