#pragma once

namespace mfl::color_display {

enum St7735Commands {
    NOP        = 0x00, // no operation
    SWRESET    = 0x01, // software reset
    RDDID      = 0x04, // read display ID
    RDDST      = 0x09, // read display status
    RDDPM      = 0x0a, // read power mode
    RDDMADCTL  = 0x0b, // read memory access control
    RDDCOLMOD  = 0x0c, // read pixel format
    RDDIM      = 0x0d, // read display image format
    RDDSM      = 0x0e, // read display signal mode
    RDSD       = 0x0f, // read self diagnostics
    SLPIN      = 0x10, // sleep in & booster off
    SLPOUT     = 0x11, // sleep out & booster on
    PTLON      = 0x12, // partial mode on
    NORON      = 0x13, // partial mode off

    INVOFF     = 0x20, // onversion off
    INVON      = 0x21, // inversion on
    SETGAMMA   = 0x26, // set gamma mode
    DISPOFF    = 0x28, // display off
    DISPON     = 0x29, // display on
    CASET      = 0x2A, // set column address
    RASET      = 0x2B, // set row address
    RAMWR      = 0x2C, // write to memory
    COLSET     = 0x2d, // set color
    RAMRD      = 0x2E, // read from memory

    PTLAR      = 0x30, // set partial start or end address
    SCRDEF     = 0x33, // set scroll definition
    TEOFF      = 0x34, // tearing effect mode off
    TEON       = 0x45, // tearing effect mode on
    MADCTL     = 0x36, // memory access control
    VRTSCRADDR = 0x37, // vertical scrolling address
    IDMOFF     = 0x38, // idle mode off
    IDMON      = 0x39, // idle mode on
    COLMOD     = 0x3A, // pixel format



    RDID1      = 0xDA, // read ID 1
    RDID2      = 0xDB, // read ID 2
    RDID3      = 0xDC, // read ID 3
};

enum MomryAccessParams {
    MADCTL_MY  = 0x80, //
    MADCTL_MX  = 0x40,
    MADCTL_MV  = 0x20,
    MADCTL_ML  = 0x10,
    MADCTL_RGB = 0x00,
};

}