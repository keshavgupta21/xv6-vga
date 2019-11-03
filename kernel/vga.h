typedef struct {
    uint32 port;
    uint8 index;
    uint8 val;
} vga_config_t;

vga_config_t vga_config_text_80_25[] = {
    // Miscelleneous Output Register
    {.port = 0x3c2, .index = 0xff, .val = 0x67},

    // Set up register 0x3c4
    {.port = 0x3c4, .index = 0x00, .val = 0x0c},
    {.port = 0x3c4, .index = 0x01, .val = 0x00},
    {.port = 0x3c4, .index = 0x01, .val = 0x03},
    {.port = 0x3c4, .index = 0x01, .val = 0x00},
    {.port = 0x3c4, .index = 0x01, .val = 0x02},

    // Unlock VGA register access
    {.port = 0x3d4, .index = 0x11, .val = 0x0e},

    // Set up register 0x3d4
    {.port = 0x3d4, .index = 0x00, .val = 0x5f},
    {.port = 0x3d4, .index = 0x01, .val = 0x4f},
    {.port = 0x3d4, .index = 0x02, .val = 0x50},
    {.port = 0x3d4, .index = 0x03, .val = 0x82},
    {.port = 0x3d4, .index = 0x04, .val = 0x55},
    {.port = 0x3d4, .index = 0x05, .val = 0x81},
    {.port = 0x3d4, .index = 0x06, .val = 0xbf},
    {.port = 0x3d4, .index = 0x07, .val = 0x1f},

    {.port = 0x3d4, .index = 0x08, .val = 0x00},
    {.port = 0x3d4, .index = 0x09, .val = 0x4f},
    {.port = 0x3d4, .index = 0x0a, .val = 0x20},
    {.port = 0x3d4, .index = 0x0b, .val = 0x0e},
    {.port = 0x3d4, .index = 0x0c, .val = 0x00},
    {.port = 0x3d4, .index = 0x0d, .val = 0x00},
    {.port = 0x3d4, .index = 0x0e, .val = 0x01},
    {.port = 0x3d4, .index = 0x0f, .val = 0xe0},

    {.port = 0x3d4, .index = 0x10, .val = 0x9c},
    {.port = 0x3d4, .index = 0x11, .val = 0x8e},
    {.port = 0x3d4, .index = 0x12, .val = 0x8f},
    {.port = 0x3d4, .index = 0x13, .val = 0x28},
    {.port = 0x3d4, .index = 0x14, .val = 0x1f},
    {.port = 0x3d4, .index = 0x15, .val = 0x96},
    {.port = 0x3d4, .index = 0x16, .val = 0xb9},
    {.port = 0x3d4, .index = 0x17, .val = 0xa3},
    {.port = 0x3d4, .index = 0x18, .val = 0xff},

    // Set up register 0x3ce
    {.port = 0x3ce, .index = 0x00, .val = 0x00},
    {.port = 0x3ce, .index = 0x01, .val = 0x00},
    {.port = 0x3ce, .index = 0x02, .val = 0x00},
    {.port = 0x3ce, .index = 0x03, .val = 0x00},
    {.port = 0x3ce, .index = 0x04, .val = 0x00},
    {.port = 0x3ce, .index = 0x05, .val = 0x10},
    {.port = 0x3ce, .index = 0x06, .val = 0x0e},
    {.port = 0x3ce, .index = 0x07, .val = 0x00},
    {.port = 0x3ce, .index = 0x08, .val = 0xff},

    // 0x3da read must happen here in order to set 3c0 into index mode
    // (right now it happens automatically on each IO operation))
    
    // Set up 0x3c0
    {.port = 0x3c0, .index = 0x00, .val = 0x00},
    {.port = 0x3c0, .index = 0x01, .val = 0x01},
    {.port = 0x3c0, .index = 0x02, .val = 0x02},
    {.port = 0x3c0, .index = 0x03, .val = 0x03},
    {.port = 0x3c0, .index = 0x04, .val = 0x04},
    {.port = 0x3c0, .index = 0x05, .val = 0x05},
    {.port = 0x3c0, .index = 0x06, .val = 0x14},
    {.port = 0x3c0, .index = 0x07, .val = 0x07},

    {.port = 0x3c0, .index = 0x08, .val = 0x38},
    {.port = 0x3c0, .index = 0x09, .val = 0x39},
    {.port = 0x3c0, .index = 0x0a, .val = 0x3a},
    {.port = 0x3c0, .index = 0x0b, .val = 0x3b},
    {.port = 0x3c0, .index = 0x0c, .val = 0x3c},
    {.port = 0x3c0, .index = 0x0d, .val = 0x3d},
    {.port = 0x3c0, .index = 0x0e, .val = 0x3e},
    {.port = 0x3c0, .index = 0x0f, .val = 0x3f},

    {.port = 0x3c0, .index = 0x10, .val = 0x0c},
    {.port = 0x3c0, .index = 0x11, .val = 0x00},
    {.port = 0x3c0, .index = 0x12, .val = 0x0f},
    {.port = 0x3c0, .index = 0x13, .val = 0x08},
    {.port = 0x3c0, .index = 0x14, .val = 0x00},

    // Set up register 0x3c4, change to plane 2 to write font data
    {.port = 0x3c4, .index = 0x00, .val = 0x01},
    {.port = 0x3c4, .index = 0x02, .val = 0x04},
    {.port = 0x3c4, .index = 0x04, .val = 0x07},
    {.port = 0x3c4, .index = 0x00, .val = 0x03},

    // Set up register 0x3ce
    {.port = 0x3ce, .index = 0x04, .val = 0x02},
    {.port = 0x3ce, .index = 0x05, .val = 0x00},
    {.port = 0x3ce, .index = 0x06, .val = 0x00},

    // TODO: need to set font up

    // Set up register 0x3c4, change back to planes 0 and 1
    {.port = 0x3c4, .index = 0x00, .val = 0x01}, // 69
    {.port = 0x3c4, .index = 0x02, .val = 0x03}, // 70
    {.port = 0x3c4, .index = 0x03, .val = 0x00}, // 71
    {.port = 0x3c4, .index = 0x04, .val = 0x02}, // 72
    {.port = 0x3c4, .index = 0x00, .val = 0x03}, // 73

    // Set up regsiter 0x3ce
    {.port = 0x3ce, .index = 0x02, .val = 0x00}, // 74
    {.port = 0x3ce, .index = 0x03, .val = 0x00}, // 75
    {.port = 0x3ce, .index = 0x04, .val = 0x00}, // 76
    {.port = 0x3ce, .index = 0x05, .val = 0x10}, // 77
    {.port = 0x3ce, .index = 0x06, .val = 0x0e}, // 78

    // Enable display
    {.port = 0x3c0, .index = 0xff, .val = 0x20}, // 79
};

typedef enum {
    Mode_Control_1 = 0,
    Overscan_Register,
    Color_Plane_Enable,
    Horizontal_Panning,
    Color_Select,
    Miscellaneous_Output_Register,
    Clock_Mode_Register,
    Character_select,
    Memory_Mode_Register,
    Mode_Register,
    Miscellaneous_Register,
    Horizontal_Total,
    Horizontal_Display_Enable_End,
    Horizontal_Blank_Start,
    Horizontal_Blank_End,
    Horizontal_Retrace_Start,
    Horizontal_Retrace_End,
    Vertical_Total,
    Overflow_Register,
    Preset_Row_Scan,
    Maximum_Scan_Line,
    Vertical_Retrace_Start,
    Vertical_Retrace_End,
    Vertical_Display_Enable_End,
    Logical_Width,
    Underline_Location,
    Vertical_Blank_Start,
    Vertical_Blank_End,
    Mode_Control_2
} REG_NAMES;
