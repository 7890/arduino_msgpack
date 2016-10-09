/**
 * Copyright (C) 2014 SINTEF <nicolas.harrand@gmail.com>
 *
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE, Version 3, 29 June 2007;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * //tb/1610
 * //pick some methods for datatypes compatible with OSC, make them buffer based
 * //half-baked slip framing of messages
 */

#include "Arduino.h"

#define SLIP_END 0300
#define SLIP_ESC 0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

//=============================================================================
static inline void write_SLIP_ESC(Stream * s) {s->write((unsigned char)SLIP_ESC);}
static inline void write_SLIP_ESC_ESC(Stream * s) {s->write((unsigned char)SLIP_ESC_ESC);}
static inline void write_SLIP_END(Stream * s) {s->write((unsigned char)SLIP_END);}
static inline void write_SLIP_ESC_END(Stream * s) {s->write((unsigned char)SLIP_ESC_END);}

//=============================================================================
static inline uint32_t write_slip_escaped(Stream * s, byte * header, uint32_t h_len, byte * args, uint32_t a_len)
{
	uint32_t total_bytes_written=0;
	int length = h_len;

	//start
	write_SLIP_END(s);
	total_bytes_written++;

	while(length--) {
		switch (*header) {
		 case SLIP_ESC:
			write_SLIP_ESC(s);
			write_SLIP_ESC_ESC(s);
			total_bytes_written+=2;
			break;
		 case SLIP_END:
			write_SLIP_ESC(s);
			write_SLIP_ESC_END(s);
			total_bytes_written+=2;
			break;
		 default:
			s->write(*header);
			total_bytes_written++;
		}
		header++;
	}

	length = a_len;

	while(length--) {
		switch (*args) {
		 case SLIP_ESC:
			write_SLIP_ESC(s);
			write_SLIP_ESC_ESC(s);
			total_bytes_written+=2;
			break;
		 case SLIP_END:
			write_SLIP_ESC(s);
			write_SLIP_ESC_END(s);
			total_bytes_written+=2;
			break;
		 default:
			s->write(*args);
			total_bytes_written++;
		}
		args++;
	}

	//end
	write_SLIP_END(s);
	total_bytes_written++;

	return total_bytes_written;
}//end write_slip_escaped()

//=============================================================================
static inline uint32_t write_c_unpacked(byte * arr, uint32_t pos, char c)
{
	arr[pos++]=c;
	return pos;
}

//=============================================================================
static inline uint32_t write_i8(byte * arr, uint32_t pos, int8_t i)
{
	if((i < -32) || (i > 127)) {
		arr[pos++]=0xd0;
		arr[pos++]=i;
	} else {
		arr[pos++]=i;
	}
	return pos;
}

//=============================================================================
static inline uint32_t write_c(byte * arr, uint32_t pos, int8_t c)
{
	return write_i8(arr, pos, c);
	///return write_c_unpacked(arr,pos,c);
}

//=============================================================================
static inline uint32_t write_i16(byte * arr, uint32_t pos, int16_t i)
{
	if((i < -127) || (i > 254)) {
		arr[pos++]=0xd1;
		arr[pos++]=i >> 8;
		arr[pos++]=i & 0xff;
	} else {
		pos=write_i8(arr, pos, (int8_t) i);
	}
	return pos;
}

//=============================================================================
static inline uint32_t write_i32(byte * arr, uint32_t pos, int32_t i)
{
	if((i < -32768) || (i > 32767)) {
		arr[pos++]=0xd2;
		arr[pos++]=(i >> 24)& 0xff;
		arr[pos++]=(i >> 16)& 0xff;
		arr[pos++]=(i >> 8)& 0xff;
		arr[pos++]=i & 0xff;
	} else {
		pos=write_i16(arr, pos, (int16_t) i);
	}
	return pos;
}

//=============================================================================
static inline uint32_t write_i64(byte * arr, uint32_t pos, int64_t i)
{
	if((i < -2147483647) || (i > 2147483647)) {
		arr[pos++]=0xd3;
		arr[pos++]=i >> 56;
		arr[pos++]=i >> 48;
		arr[pos++]=i >> 40;
		arr[pos++]=i >> 32;
		arr[pos++]=i >> 24;
		arr[pos++]=i >> 16;
		arr[pos++]=i >> 8;
		arr[pos++]=i & 0xff;
	} else {
		pos=write_i32(arr, pos, (int32_t) i);
	}
	return pos;
}

//=============================================================================
static inline uint32_t write_f(byte * arr, uint32_t pos, float f)
{
	union float_to_byte {float f; byte b[4];}
	f2b;
	f2b.f = f;
	arr[pos++]=0xca;
	arr[pos++]=f2b.b[3];
	arr[pos++]=f2b.b[2];
	arr[pos++]=f2b.b[1];
	arr[pos++]=f2b.b[0];
	return pos;
}

//=============================================================================
static inline uint32_t write_s(byte * arr, uint32_t pos, char * str, uint32_t str_size)
{
	if(str_size > 65535) {
		arr[pos++]=0xdb;
		arr[pos++]=(str_size >> 24) & 0xff;
		arr[pos++]=(str_size >> 16) & 0xff;
		arr[pos++]=(str_size >> 8) & 0xff;
		arr[pos++]=str_size & 0xff;
	} else if(str_size > 255) {
		arr[pos++]=0xda;
		arr[pos++]=(str_size >> 8) & 0xff;
		arr[pos++]=str_size & 0xff;
	} else if (str_size > 31) {
		arr[pos++]=0xd9;
		arr[pos++]=str_size & 0xff;
	} else {
		arr[pos++]=0xa0 + str_size;
	}
	uint32_t i;
	for(i=0;i<str_size;i++){
		arr[pos++]=str[i];
	}
	return pos;
}

//=============================================================================
static inline uint32_t write_s(byte * arr, uint32_t pos, String str)
{
	char buf[str.length()+1];
	str.toCharArray(buf, str.length()+1);
	return write_s(arr, pos, buf, str.length());
}

//=============================================================================
static inline uint32_t write_bin_packed_blob(byte * arr, uint32_t pos, byte * bin, uint32_t bin_size)
{
	//for auto-inserted '!' indicates packed blob
	//packed blob
	bin_size++;

	if(bin_size > 65535) {
		arr[pos++]=0xc6;
		arr[pos++]=(bin_size >> 24) & 0xff;
		arr[pos++]=(bin_size >> 16) & 0xff;
		arr[pos++]=(bin_size >> 8) & 0xff;
		arr[pos++]=bin_size & 0xff;
		///
		arr[pos++]='!';
	} else if(bin_size > 255) {
		arr[pos++]=0xc5;
		arr[pos++]=(bin_size >> 8) & 0xff;
		arr[pos++]=bin_size & 0xff;
		///
		arr[pos++]='!';
	} else {
		arr[pos++]=0xc4;
		arr[pos++]=bin_size & 0xff;
		///
		arr[pos++]='!';
	}
	uint32_t i;
	for(i=0;i<bin_size;i++){
		arr[pos++]=bin[i];
	}
	return pos;
}//end write_bin_packed_blob()

//=============================================================================
static inline uint32_t write_bin(byte * arr, uint32_t pos, byte * bin, uint32_t bin_size)
{
	if(bin_size > 65535) {
		arr[pos++]=0xc6;
		arr[pos++]=(bin_size >> 24) & 0xff;
		arr[pos++]=(bin_size >> 16) & 0xff;
		arr[pos++]=(bin_size >> 8) & 0xff;
		arr[pos++]=bin_size & 0xff;
	} else if(bin_size > 255) {
		arr[pos++]=0xc5;
		arr[pos++]=(bin_size >> 8) & 0xff;
		arr[pos++]=bin_size & 0xff;
	} else {
		arr[pos++]=0xc4;
		arr[pos++]=bin_size & 0xff;
	}
	uint32_t i;
	for(i=0;i<bin_size;i++){
		arr[pos++]=bin[i];
	}
	return pos;
}
//EOF
