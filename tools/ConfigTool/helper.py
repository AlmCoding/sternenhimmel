import json
import asyncio
import threading
from datetime import datetime
from typing import Any, Dict, List, Union


def format_log_message(message: str, tag: str) -> str:
    timestamp = datetime.now().strftime("%H:%M:%S")
    return f"[{timestamp}]{tag}: {message}"


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


def print_pretty_json(data: Union[Dict, List], indent: int = 2) -> str:
    """
    Format JSON data (Python dict/list) in the specified compact format.

    Args:
        data: JSON data as Python dict or list
        indent: Number of spaces for indentation (default: 2)

    Returns:
        Formatted JSON string
    """

    def _format_value(value: Any, current_indent: int, is_root_array: bool = False) -> str:
        """Recursively format JSON values."""
        if isinstance(value, dict):
            return _format_object(value, current_indent)
        elif isinstance(value, list):
            return _format_array(value, current_indent, is_root_array)
        elif isinstance(value, str):
            return json.dumps(value)
        elif isinstance(value, bool):
            return str(value).lower()
        else:
            return str(value)

    def _format_object(obj: Dict, current_indent: int) -> str:
        """Format JSON object with proper indentation."""
        if not obj:
            return "{}"

        indent_str = " " * current_indent
        next_indent = current_indent + indent

        items = []
        for key, value in obj.items():
            formatted_value = _format_value(value, next_indent)
            items.append(f'{" " * next_indent}"{key}": {formatted_value}')

        return "{\n" + ",\n".join(items) + "\n" + indent_str + "}"

    def _format_array(array: List, current_indent: int, is_root_array: bool = False) -> str:
        """Format JSON array with proper handling."""
        if not array:
            return "[]"

        indent_str = " " * current_indent
        next_indent = current_indent + indent

        # For arrays that are direct children of objects, put each element on a new line
        if current_indent > 0 and not is_root_array:
            items = []
            for item in array:
                formatted_item = _format_value(item, next_indent, True)
                items.append(f'{" " * next_indent}{formatted_item}')
            return "[\n" + ",\n".join(items) + "\n" + indent_str + "]"
        else:
            # For nested arrays or root arrays, format compactly
            items = [_format_value(item, 0, True) for item in array]
            return "[" + ", ".join(items) + "]"

    pretty_string = _format_value(data, 0, True)
    # print(pretty_string)
    return pretty_string
