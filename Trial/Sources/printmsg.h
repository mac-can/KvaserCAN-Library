/*  -- $HeadURL: https://svn.uv-software.net/projects/uv-software/CAN/I386/DRV/trunk/API/misc/printmsg.h $ --
 *
 *  project   :  CAN - Controller Area Network
 *
 *  purpose   :  Print CAN Messages (Monitor)
 *
 *  copyright :  (C) 2019, UV Software, Berlin
 *
 *  compiler  :  Microsoft Visual C/C++ Compiler
 *               Apple LLVM Compiler (clang)
 *               GNU C/C++ Compiler (gcc)
 *
 *  export    :  void msg_print_id(FILE *stream, unsigned long id, int rtr, int ext, unsigned char dlc, int mode);
 *               void msg_print_id_fd(FILE *stream, unsigned long id, int rtr, int ext, int fdf, int brs, int esi, int len, int mode);
 *               void msg_print_data(FILE *stream, unsigned char data, int last, int mode);
 *               void msg_print_space(FILE *stream, int last, int mode);
 *               void msg_print_ascii(FILE *stream, unsigned char data, int mode);
 *               void msg_print_indent(FILE *stream, const char *prefix, int mode);
 *               void msg_print_indent_fd(FILE *stream, const char *prefix, int mode);
 *               void msg_print_time(FILE *stream, struct msg_timestamp *timestamp, int mode);
 *
 *  includes  :  (none)
 *
 *  author    :  Uwe Vogt, UV Software
 *
 *  e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *  -----------  description  --------------------------------------------
 */
 /** @file        bitrates.h
  *
  *  @brief       Print CAN Messages (Monitor)
  *
  *  @author      $Author: neptune $
  *
  *  @version     $Rev: 589 $
  *
  *  @defgroup    print_msg Print CAN Messages (Monitor)
  *  @{
  */
#ifndef PRINTMSG_H_INCLUDED
#define PRINTMSG_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */

#include <stdio.h>


/*  -----------  defines  ------------------------------------------------
 */

#define MSG_TIME_ZERO   (0)
#define MSG_TIME_ABS    (1)
#define MSG_TIME_REL    (2)

#define MSG_MODE_HEX    (0)
#define MSG_MODE_DEC    (1)
#define MSG_MODE_OCT    (2)

#define MSG_ASCII_OFF   (0)
#define MSG_ASCII_ON    (1)


/*  -----------  types  --------------------------------------------------
 */

/** time-stamp of a CAN message
 */
struct msg_timestamp {
    long tv_sec;                    /**< seconds */
    long tv_usec;                   /**< microseconds */
};

/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */

/** @brief       TBD
 */
void msg_print_id(FILE *stream, unsigned long id, int rtr, int ext, unsigned char dlc, int mode);
void msg_print_id_fd(FILE *stream, unsigned long id, int rtr, int ext, int fdf, int brs, int esi, int len, int mode);
void msg_print_data(FILE *stream, unsigned char data, int last, int mode);
void msg_print_space(FILE *stream, int last, int mode);
void msg_print_ascii(FILE *stream, unsigned char data, int mode);
void msg_print_indent(FILE *stream, const char *prefix, int mode);
void msg_print_indent_fd(FILE *stream, const char *prefix, int mode);
void msg_print_time(FILE *stream, struct msg_timestamp *timestamp, int mode);


#endif /* PRINTMSG_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
