#define CONFIG_SDEXT_SUPPORT

// #define SDCARD_IMG_FN "/tmp/sdcard.img"

#define SDEXT_PHYSADDR_CART_P3_SELMASK_ON    0x1C000
#define SDEXT_PHYSADDR_CART_P3_SELMASK_OFF         1

typedef uint16_t	Uint16;
typedef uint8_t		Uint8;

extern unsigned int sdext_cp3m_usability;

Uint8 sdext_read_cart_p3 ( Uint16 addr );
void sdext_write_cart_p3 ( Uint16 addr, Uint8 data );
void sdext_init ( void );

