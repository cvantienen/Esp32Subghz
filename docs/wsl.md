Here is a **short, simple README** for attaching USB devices to WSL using `usbipd`.
Clean, minimal, copy-paste ready:

```md
# USB → WSL Attach Guide (usbipd)

Quick commands to attach a USB device from Windows to WSL using **usbipd**.

---

## 1. List USB Devices
Shows all USB devices and their BUSID.
```

usbipd.exe list

```

---

## 2. Attach a USB Device (Manual)
Use the BUSID from the list.
```

usbipd attach --wsl --busid 1-5

```

---

## 3. Auto-Attach USB Device
Automatically attaches the device whenever WSL is running.
```

usbipd attach --wsl --busid 1-5 --auto-attach

```

---

## Notes
- Run commands **in Windows PowerShell** (not inside WSL).
- Check `/dev/tty*` or `/dev/bus/usb` in WSL after attaching.
- If permission issues appear, restart WSL and try again.

```

If you want, I can add troubleshooting, examples, or a full index-style README.
 n