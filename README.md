# 🔐 UsbLock

**UsbLock** is a service that automatically mounts encrypted storage devices securely and conveniently.

---

## 🚀 Usage

```bash
usblock [DEVICE] [DEVICE ID]
```

### 🖥️ Example

```bash
usblock /dev/sdb /dev/disk/by-id/usb-SanDisk_Cruzer_Blade_4C531001411123119193-0:0
```

---

## 📋 Rules & Recommendations

- 💡 Always **unmount the dm-crypt mount point** before removing the USB physically.
- ⚠️ Only **one device** can be configured at a time.

---

## 🛠️ Build Instructions

To build UsbLock before install cryptsetup and simply run:

```bash
make
```
