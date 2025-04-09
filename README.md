# 🔐 UsbLock

**UsbLock** is a service that automatically mounts encrypted storage devices securely and conveniently.

---

## 🚀 Usage

```bash
usblock [OPTIONS] [DEVICE]
```

### 📍 Device Format

Use the persistent device path, usually found under:

```
/dev/disk/by-id/
```

---

## ⚙️ Options

- `-l` — 🔒 Lock (unmount and secure) the specified device  
- `-r` — 🗑️ Remove the currently configured device  
  ⚠️ **Important:** After running `-r`, you **must reboot** the system before configuring a new device.

---

## 📋 Rules & Recommendations

- 💡 Always **unmount the dm-crypt mount point** before removing the USB physically.
- ⚠️ Only **one device** can be configured at a time.
- 🔁 After using `-r`, **reboot** before setting up a new device.

---

## 🛠️ Build Instructions

To build UsbLock, simply run:

```bash
make
```

✅ No additional dependencies required.
