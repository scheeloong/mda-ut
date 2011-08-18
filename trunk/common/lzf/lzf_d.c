/*
 * Copyright (c) 2000-2002 Marc Alexander Lehmann <pcg@goof.com>
 * 
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 * 
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 * 
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 *   3.  The name of the author may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#include "lzfP.h"

unsigned int 
lzf_decompress (const void *const in_data,  unsigned int in_len,
                void             *out_data, unsigned int out_len)
{
  u8 const *ip = (u8 const *)in_data;
  u8       *op = (u8*)out_data;
  /*u8 const *const in_end  = ip + in_len;
  EJ : problem of NEXT that use ip[2] in lzf_compress
    Decrement in_end by 2 and copy the
    the last 2 uncompressed byte after
    op */
  const u8 *in_end  = ip + in_len-2;
  /*End EJ*/
u8       *const out_end = op + out_len;

 /*EJ : case of there is less than 2 bytes to compress*/
  switch(in_len){
      case 2: op[1]=ip[1];
      case 1: op[0]=ip[0];
      case 0: return in_len;
  }
  /*End EJ */
  

  do
    {
      unsigned int ctrl = *ip++;

      if (ctrl < (1 << 5)) /* literal run */
        {
          ctrl++;

          if (op + ctrl > out_end)
            {
              errno = E2BIG;
              return 0;
            }

#if USE_MEMCPY
          memcpy (op, ip, ctrl);
          op += ctrl;
          ip += ctrl;
#else
          do
            *op++ = *ip++;
          while (--ctrl);
#endif
        }
      else /* back reference */
        {
          unsigned int len = ctrl >> 5;

          u8 *ref = op - ((ctrl & 0x1f) << 8) - 1;

          if (len == 7)
            len += *ip++;
          
          ref -= *ip++;

          if (op + len + 2 > out_end)
            {
              errno = E2BIG;
              return 0;
            }

          if (ref < (u8 *)out_data)
            {
              errno = EINVAL;
              return 0;
            }

          *op++ = *ref++;
          *op++ = *ref++;

          do
            *op++ = *ref++;
          while (--len);
        }
    }
  while (op < out_end && ip < in_end);
/*EJ : copy the last two uncompressed bytes*/
  *op++=in_end[0];
  *op++=in_end[1];
  /*End EJ*/
  return op - (u8 *)out_data;
}

