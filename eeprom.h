//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
// the 3684 DVK board uses port pin PA7 as an EEPROM write-protect enable/disable.
// If your design uses a different pin, modify the following macros accordingly.
#define EEPROM_ENABLE_WRITE_PROTECT()  OEA &= ~0x80             // float PA7
#define EEPROM_DISABLE_WRITE_PROTECT() PA7 = 0; OEA |= 0x80     // drive PA7 low


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void EEWaitForStop();
bit EEWaitForAck();
static void EEStartAndAddr();
extern void WaitForEEPROMWrite();
bit EEPROMWrite(WORD addr, BYTE * ptr, BYTE length);
bit EEPROMRead(WORD addr, BYTE length, BYTE *buf);
void WaitForEEPROMWrite2();
bit EEWaitForDone();

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
extern BYTE			DB_Addr;					// Dual Byte Address stat
extern BYTE			I2C_Addr;				// I2C address
//----------------------------------------------------------------------------

