/**
 * @file: config.h
 *
 * @date: 2020-06-28
 *
 * @author: Lukas Güldenstein
 */

#ifndef _CONFIG_HPP
#define _CONFIG_HPP

/* -------------------------------- includes -------------------------------- */
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#define I2C_BANK GPIO_C
#define I2C_SCL (P8)
#define I2C_SDA (P9)

#endif /* _CONFIG_HPP */
