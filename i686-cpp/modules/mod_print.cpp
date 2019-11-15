extern "C" int mod_print() {
    reinterpret_cast<unsigned short *>(0xB8000)[0] =
        (0x4F << 8) | '*';
    return 0xC001;
}
