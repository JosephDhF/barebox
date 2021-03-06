/**
 * @file
 * @brief exported generic APIs which various board files implement
 *
 * FileName: arch/arm/boards/phycard-a-l1/board.h
 *
 * This file will not contain any board specific implementations.
 */
/*
 * (C) Copyright 2008
 * Texas Instruments, <www.ti.com>
 * Raghavendra KH <r-khandenahally@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __BOARD_OMAP_H_
#define __BOARD_OMAP_H_

/** Generic Board initialization called from platform.S */
void board_init(void);

#endif         /* __BOARD_OMAP_H_ */
