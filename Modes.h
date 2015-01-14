/*
 *  Modes.h
 *  MRF49XA
 *
 *  Created by William Dillon on 2/11/14.
 *  Copyright (c) 2014 Oregon State University (COAS). All rights reserved.
 *
 *  Modified by David Freitag 1/13/15
 *
 */

#ifndef MODES_H
#define MODES_H

enum device_mode {
    MODE_INVALID,
    MODE_SERIAL,
    MODE_SERIAL_ECC,
    MODE_CAPTURE,
    MODE_TEST_ALT,
    MODE_TEST_ZERO,
    MODE_TEST_ONE,
    MODE_TEST_PING
};

#endif