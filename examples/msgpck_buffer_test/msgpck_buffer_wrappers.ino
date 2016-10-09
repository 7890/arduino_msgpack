#include "msgpck_buffer.h"

//internal variables used by wrapper methods
uint32_t index_args_buf=0;
uint32_t index_header_buf=0;
byte * msg_header_buf;
byte * msg_args_buf;
String typetags="";
uint32_t index_args_pack_buf=0;
byte * msg_args_pack_buf;
uint32_t bytes_sent=0;

//call in setup()
void msg_init()
{
	msg_header_buf=new byte[256];
	msg_args_buf=new byte[512];
	msg_args_pack_buf=new byte[512];
	msg_new();
}

//call before creating a new message
void msg_new()
{
	index_args_buf=0;
	index_header_buf=0;
	index_args_pack_buf=0;
	typetags="";
}

uint32_t msg_add_T()
{
	typetags+='T';
	return index_args_buf;
}

uint32_t msg_add_F()
{
	typetags+='F';
	return index_args_buf;
}

uint32_t msg_add_N()
{
	typetags+='N';
	return index_args_buf;
}

uint32_t msg_add_I()
{
	typetags+='I';
	return index_args_buf;
}

uint32_t msg_add_c(int8_t c)
{
	index_args_buf=write_c(msg_args_buf,index_args_buf,(int8_t)c);
	typetags+='c';
	return index_args_buf;
}

uint32_t msg_add_s(String str)
{
	index_args_buf=write_s(msg_args_buf,index_args_buf,str);
	typetags+='s';
	return index_args_buf;
}

uint32_t msg_add_i(int32_t i)
{
	index_args_buf=write_i32(msg_args_buf,index_args_buf,i);
	typetags+='i';
	return index_args_buf;
}

uint32_t msg_add_h(int64_t i)
{
	index_args_buf=write_i64(msg_args_buf,index_args_buf,i);
	typetags+='h';
	return index_args_buf;
}

uint32_t msg_add_f(float f)
{
	index_args_buf=write_f(msg_args_buf,index_args_buf,f);
	typetags+='f';
	return index_args_buf;
}

uint32_t msg_add_bytes(byte * bin, uint32_t bin_size)
{
	index_args_buf=write_bin(msg_args_buf,index_args_buf,bin,bin_size);
	typetags+='b';
	return index_args_buf;
}

uint32_t write_header(String address)
{
	index_header_buf=0;
	index_header_buf=write_c_unpacked(msg_header_buf,index_header_buf,'!');
	index_header_buf=write_s(msg_header_buf,index_header_buf,address);
	index_header_buf=write_s(msg_header_buf,index_header_buf,typetags);
	return index_header_buf;
}

uint32_t write_header_short(int32_t id)
{
	index_header_buf=0;
	index_header_buf=write_c_unpacked(msg_header_buf,index_header_buf,'!');
	index_header_buf=write_s(msg_header_buf,index_header_buf,"/@");
	index_header_buf=write_s(msg_header_buf,index_header_buf,"ib");
	index_header_buf=write_i32(msg_header_buf,index_header_buf,id); //shortcut id
	
	///should write binary header
	///index_header_buf=write_i32(msg_header_buf,index_header_buf,index_args_buf); //blob size
	return index_header_buf;
}

uint32_t render_pack_args()
{
	index_args_pack_buf=0;
	
	index_args_pack_buf=write_bin_packed_blob(msg_args_pack_buf,index_args_pack_buf,msg_args_buf,index_args_buf);
	return index_args_pack_buf;
}

uint32_t render_to_serial()
{	
	for(uint32_t i=0;i<index_header_buf;i++)
	{
		Serial.write(msg_header_buf[i]);
	}

	for(uint32_t i=0;i<index_args_buf;i++)
	{
		Serial.write(msg_args_buf[i]);
	}
	return index_header_buf+index_args_buf;
}

uint32_t render_to_serial_slip()
{
	/*
	///if shortcut
	render_pack_args();
	return write_slip_escaped(&Serial,msg_header_buf,index_header_buf,msg_args_pack_buf,index_args_pack_buf);
	*/

	return write_slip_escaped(&Serial,msg_header_buf,index_header_buf,msg_args_buf,index_args_buf);
}

uint32_t msg_send(String address)
{
	write_header(address);
	return render_to_serial_slip();
}

uint32_t msg_send_short(int32_t id)
{
	write_header_short(id);
	return render_to_serial_slip();
}
