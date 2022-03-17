/*********************************************************************************
 *  This header file will be used for placing constants using #define(s).
 *  Note: This constants are *globally* available!
 * ===============================================================================
 *
 *          Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#ifndef CROSSROAD_CONSTANTS_H
#define CROSSROAD_CONSTANTS_H

#define TIME_PER_UNIT                   60  /* virtual seconds */
#define REAL_TIMEOUT_MILLIS             10 * 1000  /* real milli-seconds */
#define G_GENERIC_VEHICLE_COUNT         100
#define KILL_SWITCH_ACTIVATION_COUNTER  3   /* number of timeouts */
#define MMAP_INPUT_CONTROLLER           TEXT("input_controlor_intersectie")
#define MMAP_OUTPUT_CONTROLLER          TEXT("output_controlor_intersectie")
#define F_OUTPUT_LOGS                   TEXT("io/output.txt")
#define G_BARRIER                       TEXT("io_barrier_event")

#endif //CROSSROAD_CONSTANTS_H
