<p align="center">
  <img src="https://cdn.discordapp.com/attachments/1358078757797101628/1358458283555295383/image.png?ex=67f3ea5d&is=67f298dd&hm=57f19698b3be97ef09f4d113a9b72f0d3e502f3c623dd3ba0e09a6ae9633b53d&"/>
</p>


<p align="center">
  <img src="https://img.shields.io/badge/status-active-success?style=flat-square"/>
  <img src="https://img.shields.io/badge/C-optimized-blue?style=flat-square"/>
  <img src="https://img.shields.io/badge/ZipCrypto-only-critical?style=flat-square"/>
</p>

> 🚀 An ultra-fast tool for brute-forcing files `.zip` protected with **ZipCrypto**, written in C with support for **aggressive multithreading**, real password validation and optimized reading of wordlists.

---

## ✨ Features

- 🔍 Accurate password checking by **full content reading**
- 💥 **Aggressive multithreading** for maximum performance
- ⚡ Quick reading of wordlist with `mmap()`
- 🛡️ **Only** support **ZipCrypto** (traditional WinZip) encryption
- ✅ Real confirmation: only consider the password correct if **all files** in the zip are read successfully

---

## 🛠️ Compilation

```bash
sudo apt install libzip-dev
gcc -O3 -Wall -pthread -march=native -flto -o 0xzipforce 0xzipforce.c -lzip
```

---

## 📦 Use

```bash
./0xzipforce -a file.zip -w wordlist.txt
```

### Parameters

- `-a`: Password protected `.zip` file
- `-w`: Wordlist path (e.g. `rockyou.txt`)

---

## 🧪 Usage example

<p align="center">
  <img src="https://cdn.discordapp.com/attachments/1358078757797101628/1358460268522246445/exemplo.gif?ex=67f3ec36&is=67f29ab6&hm=fb1d882f2a93cb29b7886b48816419adb2196dee52d07e8cf12813bd6d68f7d6&"/>
</p>

---

## 📌 Important

- ❗ `0xzipforce` **only works with Zip files that use ZipCrypto** (`Traditional PKWARE encryption`)
- ❌ Does not support Zip with AES encryption (modern WinZip)
- 🔍 Check encryption mode before starting brute-force

---

## 🧠 About the project

This tool was written from scratch with a focus on **speed and accuracy**, using:

- `libzip` for handling `.zip` files
- `mmap()` for efficient wordlist reading
- `pthread` for splitting the load between multiple threads
- Real validation: ensures that the password decrypts **all** files correctly

---

## ❤️ Author

🥷 Slayer  
🔗 [pwnbuffer.org](https://pwnbuffer.org)  
🐧 Free code is free knowledge.
