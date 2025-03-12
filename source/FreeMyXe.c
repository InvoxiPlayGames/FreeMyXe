#include <xtl.h>
#include <stdint.h>
#include <string.h>
#include "xboxkrnl.h"
#include "ppcasm.h"
#include "fs.h"
#include "hv_funcs.h"
#include "version.h"
#include "locale.h"

LocalisationMessages_t english = {
    L"About to start patching HV and kernel...\n\nYour CPU key is:\n%S\n\nWrite that down and keep it safe!",
    L"OK",
    L"Yay!",
    L"Launch XeLL instead",
    L"Failed to launch XeLL?! Oh well, I'll patch the HV and kernel anyway...",
    L"Hypervisor and kernel have been patched!\n\nYour CPU key is:\n%S\n\nSource code for FreeMyXe:\ngithub.com/InvoxiPlayGames/FreeMyXe\n\nHave fun!"
};

// translation provided by lexd0g
LocalisationMessages_t spanish = {
    L"A punto de empezar a parchear el hipervisor y el kernel...\n\nLa clave de tu CPU es:\n%S\n\nEscr\u00EDbela y mantela segura!",
    L"Vale",
    L"Yupi!",
    L"Lanzar XeLL en su lugar",
    L"XeLL no se ha podido iniciar?! Bueno, parcheare el hipervisor y el kernel de todos modos...",
    L"El hipervisor y el kernel han sido parcheados!\n\nLa clave de tu CPU es:\n%S\n\nC\u00F3digo fuente de FreeMyXe:\ngithub.com/InvoxiPlayGames/FreeMyXe\n\nDivi\u00E9rtete!"
};

// translation provided by veselcraft/tailsxsu
LocalisationMessages_t russian = {
    L"Сейчас начнём патчить гипервизор и ядро...\n\nТвой ключ процессора:\n%S\n\nОбязательно сохрани его и храни от чужих глаз!",
    L"Замечательно",
    L"Ура!",
    L"Запустить XeLL",
    L"XeLL не запускается?! Похуй, патчим гипервизор и ядро...",
    L"Гипервизор и ядро пропатчены!\n\nТвой ключ процессора:\n%S\n\nИсходники FreeMyXe можно посмотреть по этой ссылке:\ngithub.com/InvoxiPlayGames/FreeMyXe\n\nРазвлекайся!"
};

LocalisationMessages_t *currentLocalisation = &english;

static LPWSTR buttons[1] = {L"OK"};
static MESSAGEBOX_RESULT result;
static XOVERLAPPED overlapped;
static wchar_t dialog_text_buffer[256];

void MessageBox(wchar_t *text)
{
    if (XShowMessageBoxUI(XUSER_INDEX_ANY, L"FreeMyXe " FREEMYXE_VERSION, text, 1, buttons, 0, XMB_ALERTICON, &result, &overlapped) == ERROR_IO_PENDING)
    {
        while (!XHasOverlappedIoCompleted(&overlapped))
            Sleep(50);
    }
}

int MessageBoxMulti(wchar_t *text, wchar_t *button1, wchar_t *button2)
{
    LPWSTR multiButtons[2] = {button1, button2};
    if (XShowMessageBoxUI(XUSER_INDEX_ANY, L"FreeMyXe " FREEMYXE_VERSION, text, 2, multiButtons, 0, XMB_ALERTICON, &result, &overlapped) == ERROR_IO_PENDING)
    {
        while (!XHasOverlappedIoCompleted(&overlapped))
            Sleep(50);
    }
    return result.dwButtonPressed;
}

// the Freeboot syscall 0 backdoor for 17559
static uint8_t freeboot_memcpy_bytecode[] =
{
    0x3D, 0x60, 0x72, 0x62, 0x61, 0x6B, 0x74, 0x72, 0x7F, 0x03, 0x58, 0x40, 0x41, 0x9A, 0x00, 0x08, 
    0x48, 0x00, 0x1C, 0xCA, 0x2B, 0x04, 0x00, 0x04, 0x41, 0x99, 0x00, 0x94, 0x41, 0x9A, 0x00, 0x44, 
    0x38, 0xA0, 0x15, 0x4C, 0x3C, 0xC0, 0x38, 0x80, 0x2B, 0x04, 0x00, 0x02, 0x40, 0x9A, 0x00, 0x0C, 
    0x60, 0xC6, 0x00, 0x07, 0x48, 0x00, 0x00, 0x0C, 0x2B, 0x04, 0x00, 0x03, 0x40, 0x9A, 0x00, 0x1C, 
    0x38, 0x00, 0x00, 0x00, 0x90, 0xC5, 0x00, 0x00, 0x7C, 0x00, 0x28, 0x6C, 0x7C, 0x00, 0x2F, 0xAC, 
    0x7C, 0x00, 0x04, 0xAC, 0x4C, 0x00, 0x01, 0x2C, 0x38, 0x60, 0x00, 0x01, 0x4E, 0x80, 0x00, 0x20, 
    0x7D, 0x88, 0x02, 0xA6, 0xF9, 0x81, 0xFF, 0xF8, 0xF8, 0x21, 0xFF, 0xF1, 0x7C, 0xA8, 0x03, 0xA6, 
    0x7C, 0xE9, 0x03, 0xA6, 0x80, 0x86, 0x00, 0x00, 0x90, 0x85, 0x00, 0x00, 0x7C, 0x00, 0x28, 0x6C, 
    0x7C, 0x00, 0x2F, 0xAC, 0x7C, 0x00, 0x04, 0xAC, 0x4C, 0x00, 0x01, 0x2C, 0x38, 0xA5, 0x00, 0x04, 
    0x38, 0xC6, 0x00, 0x04, 0x42, 0x00, 0xFF, 0xE0, 0x4E, 0x80, 0x00, 0x20, 0x38, 0x21, 0x00, 0x10, 
    0xE9, 0x81, 0xFF, 0xF8, 0x7D, 0x88, 0x03, 0xA6, 0x4E, 0x80, 0x00, 0x20, 0x2B, 0x04, 0x00, 0x05, 
    0x40, 0x9A, 0x00, 0x14, 0x7C, 0xC3, 0x33, 0x78, 0x7C, 0xA4, 0x2B, 0x78, 0x7C, 0xE5, 0x3B, 0x78, 
    0x48, 0x00, 0xA8, 0x82, 0x38, 0x60, 0x00, 0x02, 0x4E, 0x80, 0x00, 0x20, 
};

// the memory protection patches for 17559
static uint8_t memory_protection_bytecode[] =
{
    0x38, 0x80, 0x00, 0x07, 0x7C, 0x21, 0x20, 0x78, 0x7C, 0x35, 0xEB, 0xA6, 0x48, 0x00, 0x11, 0xC2
};

// the XEX key derivation patches for 17559
static uint8_t xex_key_derivation_bytecode[] =
{
    0x2B, 0x3C, 0x00, 0x00, 0x41, 0x9A, 0x00, 0x30, 0x2F, 0x03, 0x00, 0x00, 0x40, 0x9A, 0x00, 0x10, 
    0x38, 0x80, 0x00, 0xF0, 0x48, 0x00, 0x00, 0x18, 0x60, 0x00, 0x00, 0x00, 0x2B, 0x1D, 0x00, 0x00, 
    0x38, 0x9F, 0x04, 0x40, 0x40, 0x9A, 0x00, 0x08, 0x38, 0x80, 0x00, 0x54, 0x7F, 0x83, 0xE3, 0x78, 
    0x4B, 0xFF, 0x65, 0xC1, 0x3B, 0xE0, 0x00, 0x00, 
};

// dashlaunch loading kernel bytecode for 17559
static uint8_t dashlaunch_loading_bytecode[176] =
{
    0x40, 0x98, 0x00, 0x08, 0x4E, 0x80, 0x00, 0x20, 0x3C, 0x60, 0x80, 0x10, 0x3C, 0xA0, 0x00, 0x00, 
    0x38, 0x80, 0x00, 0x00, 0x60, 0x84, 0x00, 0x08, 0x60, 0x63, 0xBF, 0xD0, 0x38, 0xC0, 0x00, 0x00, 
    0x4B, 0xF7, 0x18, 0x61, 0x38, 0x60, 0x00, 0x00, 0x3C, 0x80, 0x80, 0x10, 0x60, 0x84, 0xBF, 0xEC, 
    0x4C, 0x00, 0x01, 0x2C, 0x90, 0x64, 0x00, 0x00, 0x4B, 0xF5, 0x54, 0x64, 0x38, 0xA1, 0x00, 0x54, 
    0x3C, 0xE0, 0x80, 0x10, 0x60, 0xE7, 0xBF, 0xEC, 0x81, 0x07, 0x00, 0x00, 0x4C, 0x00, 0x01, 0x2C, 
    0x2B, 0x08, 0x00, 0x00, 0x41, 0x9A, 0x00, 0x0C, 0x7F, 0xFF, 0xFB, 0x78, 0x4B, 0xFF, 0xFF, 0xEC, 
    0x4E, 0x80, 0x00, 0x20, 0x2B, 0x03, 0x00, 0x14, 0x40, 0x9A, 0x00, 0x24, 0x3C, 0xE0, 0x80, 0x10, 
    0x60, 0xE7, 0xBF, 0xEC, 0x81, 0x07, 0x00, 0x00, 0x4C, 0x00, 0x01, 0x2C, 0x2B, 0x08, 0x00, 0x00, 
    0x41, 0x9A, 0x00, 0x0C, 0x7F, 0xFF, 0xFB, 0x78, 0x4B, 0xFF, 0xFF, 0xEC, 0x4B, 0xFF, 0xC4, 0x44, 
    0x5C, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x5C, 0x46, 0x6C, 0x61, 0x73, 0x68, 0x5C, 0x6C, 0x61, 
    0x75, 0x6E, 0x63, 0x68, 0x2E, 0x78, 0x65, 0x78, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x56, 0x78, 
} ;


// this doesn't work!
void ApplyXeBuildPatches(uint8_t *patch_data)
{
    uint32_t *patches = (uint32_t *)patch_data;
    while (1)
    {
        uint32_t length;
        size_t size;
        uint32_t i = 0;
        // get the address - if it's 0xFFFFFFFF we've hit the end
        uint32_t address = patches[0];
        if (address == 0xFFFFFFFF)
            break;
        // get the length and byte size of the patch
        length = patches[1];
        size = length * sizeof(uint32_t);
        patches += 2;
        // print info
        DbgPrint("0x%08x: 0x%x words (0x%x bytes)\n", address, length, size);
        for (i = 0; i < length; i++)
        {
            if (address > 0x40000)
            {
                *(uint32_t *)(0x80000000 | (address)) = patches[i];
            }
            else
            {
                WriteHypervisorUInt32(address, patches[i]);
            }
        }
        patches += length;
    }
}

static uint8_t xell_buffer[0x40000];
void LaunchXell()
{
    int xell_file = FSOpenFile("GAME:\\xell-1f.bin");
    int xell_filesize = 0;
    int xell_bytesread = 0;
    // check for glitch xell
    if (xell_file == -1)
    {
        xell_file = FSOpenFile("GAME:\\xell-gggggg.bin");
    }
    // if we don't have either then fail, we shouldn't have gotten here
    if (xell_file == -1)
    {
        DbgPrint("Failed to find XeLL bin!\n");
        return;
    }
    xell_filesize = FSFileSize(xell_file);
    if (xell_filesize != sizeof(xell_buffer))
    {
        DbgPrint("XeLL file incorrect size!\n");
        FSCloseFile(xell_file);
        return;
    }
    xell_bytesread = FSReadFile(xell_file, 0, xell_buffer, sizeof(xell_buffer));
    if (xell_bytesread != sizeof(xell_buffer))
    {
        DbgPrint("XeLL couldn't read all bytes!\n");
        FSCloseFile(xell_file);
        return;
    }
    // attempt to launch XeLL
    HypervisorExecute(0x800000001c000000, xell_buffer, sizeof(xell_buffer));
}

uint64_t returnTrue = (((uint64_t)LI(3, 1)) << 32) | (BLR);
uint64_t returnZero = (((uint64_t)LI(3, 0)) << 32) | (BLR);

void __cdecl main()
{
    uint8_t cpu_key[0x10];
    char cpu_key_string[0x21];
    // thanks libxenon!
    uint8_t rol_led_buf[16] = {0x99,0x00,0x00,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int has_xell = 0;

    memset(cpu_key, 0, sizeof(cpu_key));

    DbgPrint("FreeMyXe!\n");

    // call the boot animation
    DbgPrint("Sending LED command to fix ring of light state\n");
    HalSendSMCMessage(rol_led_buf, NULL);

    if (HvxGetVersions(0x72627472, 1, 0, 0, 0) == 1)
    {
        DbgPrint("Freeboot detected, installing POST out backdoor\n");
        // downgrade to badupdate backdoor for testing
        // set HvxPostOutput syscall to point to mtctr r4; bctr; pair
        SetUsingFreeboot(1);
        WriteHypervisorUInt32(0x00015FD0 + (0xD * 4), 0x00000354);
        SetUsingFreeboot(0);
    }
    else
    {
        DbgPrint("using BadUpdate POST out backdoor\n");
    }

    // read out the CPU key
    ReadHypervisor(cpu_key, 0x20, sizeof(cpu_key));
    sprintf(cpu_key_string, "%08X%08X%08X%08X", *(uint32_t *)(cpu_key + 0x0), *(uint32_t *)(cpu_key + 0x4), *(uint32_t *)(cpu_key + 0x8), *(uint32_t *)(cpu_key + 0xC));

    // check if we have a xell file
    has_xell = FSFileExists("GAME:\\xell-1f.bin") || FSFileExists("GAME:\\xell-gggggg.bin");

    DbgPrint("CPU key: %s\n", cpu_key_string);

    switch (XGetLanguage())
    {
        case XC_LANGUAGE_SPANISH:
            currentLocalisation = &spanish;
            break;
        case XC_LANGUAGE_RUSSIAN:
            currentLocalisation = &russian;
            break;
        case XC_LANGUAGE_ENGLISH:           // uh why would we set english locale for the second time
        default:
            currentLocalisation = &english;
            break;
    }

    buttons[0] = currentLocalisation->ok;

    wsprintfW(dialog_text_buffer, currentLocalisation->about_to_patch, cpu_key_string);

    if (has_xell)
    {
        int pick_result = MessageBoxMulti(dialog_text_buffer, currentLocalisation->ok, currentLocalisation->launch_xell_instead);
        if (pick_result == 1)
        {
            LaunchXell();
            MessageBox(currentLocalisation->failed_xell_launch);
        }
    }
    else
    {
        MessageBox(dialog_text_buffer);
    }

    DbgPrint("Writing syscall 0 backdoor...\n");
    // install the syscall 0 backdoor at a spare place in memory
    WriteHypervisor(0x0000B564, freeboot_memcpy_bytecode, sizeof(freeboot_memcpy_bytecode));
    // set syscall 0 to point to our backdoor
    WriteHypervisorUInt32(0x00015FD0, 0x0000B564);

    DbgPrint("Writing memory protection patches...\n");
    // write the patched memory protection instructions
    WriteHypervisor(0x0000154C, memory_protection_bytecode, sizeof(memory_protection_bytecode));
    // jump to the above shellcode
    WriteHypervisorUInt32(0x000011BC, 0x4800154E);
    HypervisorClearCache(0x000011BC);
    HypervisorClearCache(0x0000154C);

    DbgPrint("Writing expansion signature patches...\n");
    // HvxExpansionInstall
    WriteHypervisorUInt32(0x0003089c, 0x409A0008); // replace "sig != TRUE -> fail" check with a skip of the next byte if the sig was correct
    WriteHypervisorUInt32(0x000308a0, LI(29, 0)); // set a variable to 0 to avoid AES decryption
    WriteHypervisorUInt32(0x000308a4, NOP); // skip another validity check? idk
    WriteHypervisorUInt32(0x000308a8, NOP);
    WriteHypervisorUInt32(0x000304e8, NOP); // remove flag check
    WriteHypervisorUInt32(0x000304fc, NOP); // remove version check
    HypervisorClearCache(0x0003089c);
    HypervisorClearCache(0x000304e8);

    DbgPrint("Writing important patches...\n");
    WriteHypervisorUInt64(0x0000A560, returnTrue);
    HypervisorClearCache(0x0000A560);

    DbgPrint("Writing XEX loading patches...\n");
    // HvxLoadImageData - skip a hash check
    WriteHypervisorUInt32(0x0002A30C, NOP);
    WriteHypervisorUInt32(0x0002A310, NOP);
    HypervisorClearCache(0x0002A30C);
    // HvxResolveImports - patch a version check?
    WriteHypervisorUInt32(0x0002AA80, NOP);
    WriteHypervisorUInt32(0x0002AA8C, NOP);
    HypervisorClearCache(0x0002AA80);
    // HvxCreateImageMapping/HvxImageTransformKey subroutine, checks the media ID
    WriteHypervisorUInt64(0x00024D58, returnTrue);
    HypervisorClearCache(0x00024D58);
    // HvxCreateImageMapping remove segment hash check
    WriteHypervisorUInt32(0x0002CAE8, LI(3, 0));
    HypervisorClearCache(0x0002CAE8);
    // XEX AES key derivation
    WriteHypervisor(0x00029B08, xex_key_derivation_bytecode, sizeof(xex_key_derivation_bytecode));
    HypervisorClearCache(0x00029B08);
    // HvxCreateImageMapping hash check patch
    WriteHypervisorUInt32(0x0002CAE8, LI(3, 0));
    HypervisorClearCache(0x0002CAE8);
    // HvxCreateImageMapping keys flag check patch
    WriteHypervisorUInt32(0x0002CDD8, NOP);
    HypervisorClearCache(0x0002CDD8);
    // HvxImageTransformImageKey protected flag check patch
    WriteHypervisorUInt32(0x0002B778, NOP);
    HypervisorClearCache(0x0002B778);

    DbgPrint("Writing XeKeys patches...\n");
    WriteHypervisorUInt64(0x00006BB0, returnZero); // HvxSecuritySetDetected
    HypervisorClearCache(0x00006BB0);
    WriteHypervisorUInt64(0x00006C48, returnZero); // HvxSecurityGetDetected
    HypervisorClearCache(0x00006C48);
    WriteHypervisorUInt64(0x00006C98, returnZero); // HvxSecuritySetActivated
    HypervisorClearCache(0x00006C98);
    WriteHypervisorUInt64(0x00006D08, returnZero); // HvxSecurityGetActivated
    WriteHypervisorUInt64(0x00006D58, returnZero); // HvxSecuritySetStat
    HypervisorClearCache(0x00006D08);
    WriteHypervisorUInt32(0x0000813C, 0x48000030); // HvxKeysGetKey skip over key_flags check
    HypervisorClearCache(0x0000813C);

    DbgPrint("HV patched! Patching kernel\n");

    {
        uint64_t valTo = 0;
        HANDLE hKernel = NULL;
        PDWORD pdwFunction = NULL;

        XexGetModuleHandle("xboxkrnl.exe", &hKernel);

        // patch XeKeysVerifyRSASignature
        XexGetProcedureAddress(hKernel, 600, &pdwFunction);
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), returnTrue);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch XeKeysVerifyPIRSSignature
        XexGetProcedureAddress(hKernel, 862, &pdwFunction);
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), returnTrue);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch UsbdIsDeviceAuthenticated
        XexGetProcedureAddress(hKernel, 745, &pdwFunction);
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), returnTrue);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch XexpVerifyMediaType
        pdwFunction = (PDWORD)0x80078ed0;
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), returnTrue);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch XexpVerifyDeviceId
        pdwFunction = (PDWORD)0x80079e10;
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), returnZero);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch XexpConvertError
        pdwFunction = (PDWORD)0x8007b920;
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), returnZero);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch a hash check in XexpVerifyXexHeaders
        pdwFunction = (PDWORD)0x8007c034;
        valTo = (((uint64_t)NOP) << 32) | NOP;
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), valTo);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch XexpVerifyMinimumVersion
        pdwFunction = (PDWORD)0x8007af08;
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), returnZero);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // patch XexpTranslateAndVerifyBoundPath to always return true
        pdwFunction = (PDWORD)0x80078eb0;
        valTo = (((uint64_t)LI(3, 1)) << 32) | ADDI(1, 1, 0x190);
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), valTo);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // XexpLoadFile patch: flag check in XEX headers after RtlImageXexHeaderString
        pdwFunction = (PDWORD)0x8007C634;
        valTo = (((uint64_t)LI(11, 0)) << 32) | 0x556b0043; // rlwinm r11, r11, 0, 1, 1
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), valTo);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // XexpLoadFile patch: flag check in XEX headers after RtlImageXexHeaderField
        pdwFunction = (PDWORD)0x8007c684;
        valTo = (((uint64_t)LI(11, 0)) << 32) | 0x556b0043; // rlwinm r11, r11, 0, 2, 2
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), valTo);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // XexpLoadFile patch: call to RtlImageXexHeaderString
        pdwFunction = (PDWORD)0x8007C5E8;
        valTo = (((uint64_t)LI(3, 0)) << 32) | 0x28030000; // cmplwi r3, 0
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), valTo);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));

        // XeKeysConsoleSignatureVerification patch
        pdwFunction = (PDWORD)0x8010BF20;
        valTo = 0x2B05000038600001; // cmplwi r5, 0; li r3, 1;
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction), valTo);
        valTo = 0x419A000890650000; // beq to_the_blr; stw r5, 0(r3)
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction) + 0x8, valTo);
        valTo = 0x4e80002000000000; // blr
        WriteHypervisorUInt64_RMCI(MmGetPhysicalAddress(pdwFunction) + 0x10, valTo);
        HypervisorClearCache(MmGetPhysicalAddress(pdwFunction));
    }

    //ApplyXeBuildPatches(xebuild_17559_hvkern_patchset);
    
    DbgPrint("Done\n");

    Sleep(500);

    buttons[0] = currentLocalisation->yay;
    wsprintfW(dialog_text_buffer, currentLocalisation->patch_successful, cpu_key_string);
    MessageBox(dialog_text_buffer);
}
